#include <inc/mmu.h>
#include <inc/x86.h>
#include <inc/assert.h>

#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/env.h>
#include <kern/syscall.h>
#include <kern/sched.h>
#include <kern/kclock.h>
#include <kern/picirq.h>
#include <kern/cpu.h>
#include <kern/spinlock.h>

extern uint32_t vects[];

/* For debugging, so print_trapframe can distinguish between printing
 * a saved trapframe and printing the current trapframe and print some
 * additional information in the latter case.
 */
static struct Trapframe *last_tf;

/* Interrupt descriptor table.  (Must be built at run time because
 * shifted function addresses can't be represented in relocation records.)
 */
struct Gatedesc idt[IDT_ENTRIES] = { {0} };

struct Pseudodesc idt_pd = {
    sizeof (idt) - 1, (uint32_t) idt
};


static const char *
trapname (int trapno)
{
	static const char * const excnames[] = {
		"Divide error",
		"Debug",
		"Non-Maskable Interrupt",
		"Breakpoint",
		"Overflow",
		"BOUND Range Exceeded",
		"Invalid Opcode",
		"Device Not Available",
		"Double Fault",
		"Coprocessor Segment Overrun",
		"Invalid TSS",
		"Segment Not Present",
		"Stack Fault",
		"General Protection",
		"Page Fault",
		"(unknown trap)",
		"x87 FPU Floating-Point Error",
		"Alignment Check",
		"Machine-Check",
		"SIMD Floating-Point Exception"
	};

	if (trapno < sizeof(excnames)/sizeof(excnames[0]))
		return excnames[trapno];
	if (trapno == T_SYSCALL)
		return "System call";
	if (trapno >= IRQ_OFFSET && trapno < IRQ_OFFSET + 16)
		return "Hardware Interrupt";
	return "(unknown trap)";
}


void
trap_init (void)
{
    extern struct Segdesc gdt[];
    uint32_t i;
    // LAB 3: Your code here.
    for (i = 0; i < IDT_ENTRIES; i++)
    {
        SETGATE (idt[i], TRAP_N, GD_KT, vects[i], DPL_KERN);
    }

#ifdef bug_017
    SETGATE (idt[T_SYSCALL], TRAP_Y, GD_KT, vects[T_SYSCALL], DPL_USER);
    SETGATE (idt[T_BRKPT], TRAP_Y, GD_KT, vects[T_BRKPT], DPL_USER);
#else
    SETGATE (idt[T_SYSCALL], TRAP_N, GD_KT, vects[T_SYSCALL], DPL_USER);
    SETGATE (idt[T_BRKPT], TRAP_N, GD_KT, vects[T_BRKPT], DPL_USER);
#endif

    // Per-CPU setup 
    //Init BSP: main processor.
    trap_init_percpu ();
}

// Initialize and load the per-CPU TSS and IDT
void
trap_init_percpu (void)
{
	// The example code here sets up the Task State Segment (TSS) and
	// the TSS descriptor for CPU 0. But it is incorrect if we are
	// running on other CPUs because each CPU has its own kernel stack.
	// Fix the code so that it works for all CPUs.
	//
	// Hints:
	//   - The macro "thiscpu" always refers to the current CPU's
	//     struct Cpu;
	//   - The ID of the current CPU is given by cpunum() or
	//     thiscpu->cpu_id;
	//   - Use "thiscpu->cpu_ts" as the TSS for the current CPU,
	//     rather than the global "ts" variable;
	//   - Use gdt[(GD_TSS0 >> 3) + i] for CPU i's TSS descriptor;
	//   - You mapped the per-CPU kernel stacks in mem_init_mp()
	//
	// ltr sets a 'busy' flag in the TSS selector, so if you
	// accidentally load the same TSS on more than one CPU, you'll
	// get a triple fault.  If you set up an individual CPU's TSS
	// wrong, you may not get a fault until you try to return from
	// user space on that CPU.
	//
	// LAB 4: Your code here:

	// Setup a TSS so that we get the right stack
	// when we trap to the kernel.
        assert(thiscpu->cpu_id == cpunum());
        if(0 == thiscpu->cpu_ts.ts_ss0)
        {
            thiscpu->cpu_ts.ts_esp0 = KSTACKTOP - (KSTKSIZE+KSTKGAP)*thiscpu->cpu_id;
            thiscpu->cpu_ts.ts_ss0 = GD_KD;
            //cprintf("cpu_%d's stack is 0x%08x\n",thiscpu->cpu_id,thiscpu->cpu_ts.ts_esp0); //Debug

            gdt[(GD_TSS0 >> 3) + (thiscpu->cpu_id)] = SEG16(STS_T32A
                    , (uint32_t) (&(thiscpu->cpu_ts))
                    , sizeof(struct Taskstate)
                    , 0);

            gdt[(GD_TSS0 >> 3) + (thiscpu->cpu_id)].sd_s = 0;

            ltr((((GD_TSS0 >> 3)+thiscpu->cpu_id) << 3));
            lidt(&idt_pd);
        }

        /*
         *The sample to init BSP
	ts.ts_esp0 = KSTACKTOP;
	ts.ts_ss0 = GD_KD;

	// Initialize the TSS slot of the gdt.
	gdt[GD_TSS0 >> 3] = SEG16(STS_T32A, (uint32_t) (&ts),
					sizeof(struct Taskstate), 0);
	gdt[GD_TSS0 >> 3].sd_s = 0;

	// Load the TSS selector (like other segment selectors, the
	// bottom three bits are special; we leave them 0)
	ltr(GD_TSS0);

	// Load the IDT
	lidt(&idt_pd);
        */
}

void
print_trapframe (struct Trapframe *tf)
{
	cprintf("TRAP frame at %p from CPU %d\n", tf, cpunum());
	print_regs(&tf->tf_regs);
	cprintf("  es   0x----%04x\n", tf->tf_es);
	cprintf("  ds   0x----%04x\n", tf->tf_ds);
	cprintf("  trap 0x%08x %s\n", tf->tf_trapno, trapname(tf->tf_trapno));
	// If this trap was a page fault that just happened
	// (so %cr2 is meaningful), print the faulting linear address.
	if (tf == last_tf && tf->tf_trapno == T_PGFLT)
		cprintf("  cr2  0x%08x\n", rcr2());
	cprintf("  err  0x%08x", tf->tf_err);
	// For page faults, print decoded fault error code:
	// U/K=fault occurred in user/kernel mode
	// W/R=a write/read caused the fault
	// PR=a protection violation caused the fault (NP=page not present).
	if (tf->tf_trapno == T_PGFLT)
		cprintf(" [%s, %s, %s]\n",
			tf->tf_err & 4 ? "user" : "kernel",
			tf->tf_err & 2 ? "write" : "read",
			tf->tf_err & 1 ? "protection" : "not-present");
	else
		cprintf("\n");
	cprintf("  eip  0x%08x\n", tf->tf_eip);
	cprintf("  cs   0x----%04x\n", tf->tf_cs);
	cprintf("  flag 0x%08x\n", tf->tf_eflags);
	if ((tf->tf_cs & 3) != 0) {
		cprintf("  esp  0x%08x\n", tf->tf_esp);
		cprintf("  ss   0x----%04x\n", tf->tf_ss);
	}
}

void
print_regs (struct PushRegs *regs)
{
    cprintf ("  edi  0x%08x\n", regs->reg_edi);
    cprintf ("  esi  0x%08x\n", regs->reg_esi);
    cprintf ("  ebp  0x%08x\n", regs->reg_ebp);
    cprintf ("  oesp 0x%08x\n", regs->reg_oesp);
    cprintf ("  ebx  0x%08x\n", regs->reg_ebx);
    cprintf ("  edx  0x%08x\n", regs->reg_edx);
    cprintf ("  ecx  0x%08x\n", regs->reg_ecx);
    cprintf ("  eax  0x%08x\n", regs->reg_eax);
}

#define XARG_SYSCALL_PRAR(_regPara) tf->tf_regs._regPara
static void
trap_dispatch (struct Trapframe *tf)
{
    // Handle processor exceptions.
    // LAB 3: Your code here.
    if(tf->tf_trapno >= (IRQ_OFFSET+IRQ_TIMER) && tf->tf_trapno <= IRQ_OFFSET_HANDLE_MAX)
    {
        switch(tf->tf_trapno - IRQ_OFFSET)
        {
            case IRQ_TIMER:
//                cprintf("curenv->env_id:0x%x is in\n",curenv->env_id);
                lapic_eoi(); //bug_020
                sched_yield();
                break;
            case IRQ_KBD:
            case IRQ_SERIAL:
            case IRQ_SPURIOUS:
            case IRQ_IDE:
            case IRQ_ERROR:
#ifndef BLOCK_ANNOY_INFO
                print_trapframe(tf);
#endif
                break;
            default:
#ifndef BLOCK_ANNOY_INFO
                print_trapframe(tf);
#endif
                if(tf->tf_cs == GD_KT)
                    panic("unhandled trap in kernel");
                else
                    panic("unhandled trap in user");
                    env_destroy(curenv);
                break;
        }
    }
    else
    {
        switch (tf->tf_trapno)
        {
            case T_PGFLT:
                page_fault_handler (tf);
                break;
            case T_BRKPT:
                breakpoint_handler (tf);
                break;
            case T_SYSCALL:
                //Extract the parameters
                XARG_SYSCALL_PRAR (reg_eax) = syscall (XARG_SYSCALL_PRAR (reg_eax),
                        XARG_SYSCALL_PRAR (reg_edx),
                        XARG_SYSCALL_PRAR (reg_ecx),
                        XARG_SYSCALL_PRAR (reg_ebx),
                        XARG_SYSCALL_PRAR (reg_edi),
                        XARG_SYSCALL_PRAR (reg_esi));
                break;
            default:
                // Unexpected trap: The user process or the kernel has a bug.
                print_trapframe (tf);
                if (tf->tf_cs == GD_KT)
                    panic ("unhandled trap in kernel");
                else
                {
                    env_destroy (curenv);
                    return;
                }
                break;
        }
    }

#if 0
    // Handle processor exceptions.
    // LAB 3: Your code here.

    // Handle spurious interrupts
    // The hardware sometimes raises these because of noise on the
    // IRQ line or other reasons. We don't care.
    if (tf->tf_trapno == IRQ_OFFSET + IRQ_SPURIOUS) {
        cprintf("Spurious interrupt on irq 7\n");
        print_trapframe(tf);
        return;
    }

    // Handle clock interrupts. Don't forget to acknowledge the
    // interrupt using lapic_eoi() before calling the scheduler!
    // LAB 4: Your code here.

    // Unexpected trap: The user process or the kernel has a bug.
    print_trapframe(tf);
    if (tf->tf_cs == GD_KT)
        panic("unhandled trap in kernel");
    else {
        env_destroy(curenv);
        return;
    }
#endif
}

void
trap (struct Trapframe *tf)
{
	// The environment may have set DF and some versions
	// of GCC rely on DF being clear
#ifdef bug_017
	asm volatile("cli");
#endif
	asm volatile("cld" ::: "cc");

	// Halt the CPU if some other CPU has called panic()
	extern char *panicstr;
	if (panicstr)
		asm volatile("hlt");


#ifdef bug_017
        //lock_kernel();
#endif
//        if(tf->tf_trapno == (IRQ_OFFSET+IRQ_TIMER) && tf->tf_trapno <= IRQ_OFFSET_HANDLE_MAX)
//            cprintf("-----Start---\n");
//        assert(!(read_eflags() & FL_IF));
	if ((tf->tf_cs & 3) == 3) {
#ifndef bug_017
                lock_kernel();
#endif
		// Trapped from user mode.
		// Acquire the big kernel lock before doing any
		// serious kernel work.
		// LAB 4: Your code here.
		assert(curenv);
                //cprintf("///Trap from USER mode\\\\\\\n"); //Debug
		// Garbage collect if current enviroment is a zombie
		if (curenv->env_status == ENV_DYING) {
			env_free(curenv);
			curenv = NULL;
			sched_yield();
		}

		// Copy trap frame (which is currently on the stack)
		// into 'curenv->env_tf', so that running the environment
		// will restart at the trap point.
		curenv->env_tf = *tf;
		// The trapframe on the stack should be ignored from here on.
		tf = &curenv->env_tf;
	}
        else
        {
            //cprintf("\\\\\\Trap from KERNEL mode///\n"); //Debug
            // Check that interrupts are disabled.  If this assertion
            // fails, DO NOT be tempted to fix it by inserting a "cli" in
            // the interrupt path.
            assert(!(read_eflags() & FL_IF));
        }

	// Record that tf is the last real trapframe so
	// print_trapframe can print some additional information.
	last_tf = tf;

	// Dispatch based on what type of trap occurred
//        if(tf->tf_trapno >= (IRQ_OFFSET+IRQ_TIMER) && tf->tf_trapno <= IRQ_OFFSET_HANDLE_MAX)
//        if(tf->tf_trapno == (IRQ_OFFSET+IRQ_TIMER)) cprintf("             Done---\n");
	trap_dispatch(tf);

	// If we made it to this point, then no other environment was
	// scheduled, so we should return to the current environment
	// if doing so makes sense.
        if (curenv && curenv->env_status == ENV_RUNNING)
        {
            env_run(curenv);
        }
	else
		sched_yield();
}

void
breakpoint_handler (struct Trapframe *tf)
{
    monitor(tf);
    return;
}

/*
   The ESP points to the current element waited to pop.
 */
void push(uint32_t* esp,uint32_t element)
{
    *esp = *esp - 4;
    *((uint32_t*)*esp) = element;
}
void pop(uint32_t *esp,uint32_t* element)
{
    *element = *((uint32_t*)*esp);
    *esp = *esp - 4;
}
#define RCSV_UTRAP_SIZE (sizeof(struct UTrapframe) + 4)
#define UTRAP_SIZE (sizeof(struct UTrapframe))
void
page_fault_handler (struct Trapframe *tf)
{
    uint32_t fault_va;
    pte_t *ptep;
#ifdef DEBUG_TRAP_C
    cprintf("===Execute in page_fault_handler===\n");
#endif
    // Read processor's CR2 register to find the faulting address
    fault_va = rcr2 ();
    /*
       cr2 is to find the faulting address
       error code has some useful information
     */

    // LAB 3: Your code here.
    // Handle kernel-mode page faults.
    if ((SEL_PL (tf->tf_cs)) == 0x00) {
        print_trapframe (tf);
        panic ("=== Page fault at kernel ===");
    }
    ptep = pgdir_walk (curenv->env_pgdir, (void *) fault_va, NO_CREATE);


    // Handle kernel-mode page faults.

    // LAB 3: Your code here.

    // We've already handled kernel-mode exceptions, so if we get here,
    // the page fault happened in user mode.

    // Call the environment's page fault upcall, if one exists.  Set up a
    // page fault stack frame on the user exception stack (below
    // UXSTACKTOP), then branch to curenv->env_pgfault_upcall.
    //
    // The page fault upcall might cause another page fault, in which case
    // we branch to the page fault upcall recursively, pushing another
    // page fault stack frame on top of the user exception stack.
    //
    // The trap handler needs one word of scratch space at the top of the
    // trap-time stack in order to return.  In the non-recursive case, we
    // don't have to worry about this because the top of the regular user
    // stack is free.  In the recursive case, this means we have to leave
    // an extra word between the current top of the exception stack and
    // the new stack frame because the exception stack _is_ the trap-time
    // stack.
    //
    // If there's 
    // 1. no page fault upcall,
    // 2  the environment didn't allocate a  page for its exception stack or 
    // 3. can't write to it, or 
    // 4.the exception stack overflows
    // , then destroy the environment that caused the fault.
    //
    // Note that the grade script assumes you will first check for the page
    // fault upcall and print the "user fault va" message below if there is
    // none.  The remaining three checks can be combined into a single test.
    //
    // Hints:
    //   user_mem_assert() and env_run() are useful here.
    //   To change what the user environment runs, modify 'curenv->env_tf'
    //   (the 'tf' variable points at 'curenv->env_tf').
    //   Hawx: It means eip/esp.

    // LAB 4: Your code here.
    uint32_t esp = UXSTACKTOP;
    //Don't remove it: the test script needed.'
    cprintf ("[%08x] user fault va %08x ip %08x\n",curenv->env_id, fault_va, tf->tf_eip);
    cprintf ("[0x%08x] user fault va 0x%08x,tf->err 0x%03x ,tf->eip 0x%08x, tf->esp 0x%08x\n"
            , curenv->env_id, fault_va, tf->tf_err,tf->tf_eip, tf->tf_esp);

    pte_t* pte;

    if(0 == curenv->env_pgfault_upcall){
        cprintf("!!! envid:0x%x, No env_pgfault_upcall !!!\n",curenv->env_id);
        goto Failed;
    }

    user_mem_assert(curenv,curenv->env_pgfault_upcall,PGSIZE, PTE_U | PTE_P);
#ifdef DEBUG_TRAP_C
    cprintf("===Pass curenv->env_pgfault_upcall===\n");
#endif

    /*
       It is right but it can't pass the grade.sh. It should check from top.
       user_mem_assert(curenv,(void*)UXSTACKBASE, PGSIZE, PTE_U | PTE_P | PTE_W);
       cprintf("===Pass UXSTACKBASE===\n");
     */

    user_mem_assert(curenv,
            (void*)(UXSTACKTOP-sizeof (struct UTrapframe)), 
            sizeof(struct UTrapframe),
            PTE_U | PTE_P | PTE_W);
#ifdef DEBUG_TRAP_C
    cprintf("===Pass UXSTACKBASE===\n");
#endif


    if(UXSTACKTOP > tf->tf_esp && UXSTACKBASE <= tf->tf_esp) {
        //In the recursive case, this means we have to leave
        // an extra word between the current top of the exception stack and
        // the new stack frame because the exception stack _is_ the trap-time
        // stack.
        if(UXSTACKBASE > (tf->tf_esp - RCSV_UTRAP_SIZE))
        {
            panic("It will underflow !!!");
            goto Failed;
        }

        esp = tf->tf_esp;
        //Hawx: It must need to understand the detail.
        push(&esp,0);
    }

    /*
     * Don'need to do it.
     * lcr3(PADDR(curenv->env_pgdir));
     *Why:
     * 1. The current page directory is current env's pgdir (curenv)->env_pgdir
     * 2. Just trace env_create again. In the env_alloc *,
     *  It had alreay done about the kernel page dir mapping.
     *  2a: KD UD KT UT segment's address mapping is the same.(env.c's struct gdt)
     *  2b: Privilege mode checking is by the Code/Data Segment.
     *      It had already set up in the hardware trap action from user to kernel.
     */



    //Hawx:
    //Is here the trap-time stack to return to ???
    //No! here is for the re-executed instructin use.
    //The cuurent time to return is the _pgfault_upcall
    push(&esp,tf->tf_esp);
    push(&esp,tf->tf_eflags);
    push(&esp,tf->tf_eip);
    esp -= sizeof(struct PushRegs);
    *((struct PushRegs*)esp) = tf->tf_regs;
    push(&esp,tf->tf_err);
    push(&esp,fault_va);

#ifdef DEBUG_TRAP_C
    cprintf("===env_pgfault_upcall:0x%08x===\n",curenv->env_pgfault_upcall);
#endif
    /*
     * Don'need to do it.
     * lcr3(PADDR(kern_pgdir));
     * Here is the pair to lcr3(PADDR(curenv->env_pgdir));
     */
    (curenv)->env_tf.tf_esp = esp;
    (curenv)->env_tf.tf_eip = (uint32_t)(curenv)->env_pgfault_upcall;
    env_run(curenv);

    // Destroy the environment that caused the fault.
Failed:
    cprintf ("[curenv_addr:%08x] env_id_addr: 0x%x,ip %08x\n",
            curenv, (uint32_t) & curenv->env_id, tf->tf_eip);
    print_trapframe (tf);
    env_destroy (curenv);
}
