/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>

#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/syscall.h>
#include <kern/console.h>
#include <kern/sched.h>
#include <kern/env.h>

// Print a string to the system console.
// The string is exactly 'len' characters long.
// Destroys the environment on memory errors.
static void
sys_cputs (const char *s, size_t len)
{
    // Check that the user has permission to read memory [s, s+len).
    // Destroy the environment if not.

    // LAB 3: Your code here.
    user_mem_assert (curenv, (const char *) s, len, PTE_U | PTE_P);
    cprintf ("%.*s", len, s);
    return;
}

// Read a character from the system console without blocking.
// Returns the character, or 0 if there is no input waiting.
static int
sys_cgetc (void)
{
    return cons_getc ();
}

// Returns the current environment's envid.
static envid_t
sys_getenvid (void)
{
    return curenv->env_id;
}

// Destroy a given environment (possibly the currently running environment).
//
// Returns 0 on success, < 0 on error.  Errors are:
//  -E_BAD_ENV if environment envid doesn't currently exist,
//      or the caller doesn't have permission to change envid.
static int
sys_env_destroy (envid_t envid)
{
    int r;
    struct Env *e;

    if ((r = envid2env (envid, &e, 1)) < 0)
        return r;
    if (e == curenv)
        cprintf ("[%08x] exiting gracefully\n", curenv->env_id);
    else
        cprintf ("[%08x] destroying %08x\n", curenv->env_id, e->env_id);
    env_destroy (e);
    return 0;
}

// Deschedule current environment and pick a different one to run.
static void
sys_yield(void)
{
	sched_yield();
}

static int check_addr_scale(unsigned int val,unsigned int min,unsigned int max)
{
    if( val >= min && val < max && !PGOFF(val))
        return 0;

    return -E_INVAL;
}

// Allocate a new environment.
// Returns envid of new environment, or < 0 on error.  Errors are:
//	-E_NO_FREE_ENV if no free environment is available.
//	-E_NO_MEM on memory exhaustion.
static envid_t
sys_exofork(void)
{
    // Create the new environment with env_alloc(), from kern/env.c.
    // It should be left as env_alloc created it, except that
    // status is set to ENV_NOT_RUNNABLE, and the register set is copied
    // from the current environment -- but tweaked so sys_exofork
    // will appear to return 0.

    // LAB 4: Your code here.
    cprintf("===Execute in sys_exofork===\n");
    struct Env *child_env = NULL;
    int ret_value;
    ret_value = env_alloc(&child_env,(curenv)->env_id);
    if(ret_value)
    {
        cprintf("===Error in sys_exofork===%e\n",ret_value);
        return ret_value;
    }

    //Just set up needed states.
    child_env->env_tf = curenv->env_tf;
    child_env->env_status =  ENV_NOT_RUNNABLE;
    // Hawx:
    // For Child part, just pass the return value by the eax assignment.
    child_env->env_tf.tf_regs.reg_eax = 0;
    //cprintf("===Ready to reutrn from exofork,child's envid:0x%08x,child's eax:0x%08x===\n" ,child_env->env_id, child_env->env_tf.tf_regs.reg_eax); //Debug
    
    //Hawx:
    //Child never execute here.
    //1. Here is the Kernel mode's stack, 
    //   we just copy the env_tf into child's address space 
    //   to fake pretending calling exofork
    //2. As for the return value, user process to distinguish between parent and child,
    //   For Parent part, just by the code "return child_env->env_id"
    //   For Child part, just pass the return value by the eax assignment.
    if(child_env->env_id == (curenv)->env_id)
    {
        cprintf("===Child Start===\n");
        return 0;
    }

    cprintf("===Parent Start===\n");
    return child_env->env_id;
}

// Set envid's env_status to status, which must be ENV_RUNNABLE
// or ENV_NOT_RUNNABLE.
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if status is not a valid status for an environment.
static int
sys_env_set_status(envid_t envid, int status)
{
    // Hint: Use the 'envid2env' function from kern/env.c to translate an
    // envid to a struct Env.
    // You should set envid2env's third argument to 1, which will
    // check whether the current environment has permission to set
    // envid's status.

    // LAB 4: Your code here.
    cprintf("===Execute in sys_env_set_status===\n");
    struct Env *e;
    if(envid2env(envid,&e,1))
    {
        return -E_BAD_ENV;
    }
    if(ENV_RUNNABLE != e->env_status && ENV_NOT_RUNNABLE !=e->env_status)
    {
        return -E_INVAL;
    }

    e->env_status = status;
    return 0;

}

// Set the page fault upcall for 'envid' by modifying the corresponding struct
// Env's 'env_pgfault_upcall' field.  When 'envid' causes a page fault, the
// kernel will push a fault record onto the exception stack, then branch to
// 'func'.
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
static int
sys_env_set_pgfault_upcall(envid_t envid, void *func)
{
    // LAB 4: Your code here.
    panic("sys_env_set_pgfault_upcall not implemented");
}

// Allocate a page of memory and map it at 'va' with permission
// 'perm' in the address space of 'envid'.
// The page's contents are set to 0.
//
// If a page is already mapped at 'va', that page is unmapped as a
// side effect.(Hawx: What's the other way to handle that.)
//
// perm -- PTE_U | PTE_P must be set, PTE_AVAIL | PTE_W may or may not be set,
//         but no other bits may be set.  See PTE_SYSCALL in inc/mmu.h.
// #define PTE_SYSCALL	(PTE_AVAIL | PTE_P | PTE_W | PTE_U)
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if va >= UTOP, or va is not page-aligned.
//	-E_INVAL if perm is inappropriate (see above).
//	-E_NO_MEM if there's no memory to allocate the new page,
//		or to allocate any necessary page tables.
static int
sys_page_alloc(envid_t envid, void *va, int perm)
{
    // Hint: This function is a wrapper around page_alloc() and
    //   page_insert() from kern/pmap.c.
    //   Most of the new code you write should be to check the
    //   parameters for correctness.
    //   If page_insert() fails, remember to free the page you
    //   allocated!
    // LAB 4: Your code here.
    cprintf("===Execute in sys_page_alloc===\n");
    struct Page* page_need = NULL;
    struct Env *e;

    if(envid2env(envid,&e,1))
        return -E_BAD_ENV;

    if(((int)va >= UTOP) || (perm & ~PTE_SYSCALL) )
    {
        return -E_INVAL;
    }
    page_need = page_alloc(ALLOC_ZERO);

    if(!page_need)
    {
        return -E_NO_MEM;
    }

    if(page_insert(e->env_pgdir,page_need,va,perm)) 
    {
        page_free(page_need);
        return -E_NO_MEM;
    }

    return 0;
}

// Map the page of memory at 'srcva' in srcenvid's address space
// at 'dstva' in dstenvid's address space with permission 'perm'.
// Perm has the same restrictions as in sys_page_alloc, except
// that it also must not grant write access to a read-only
// page.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if srcenvid and/or dstenvid doesn't currently exist,
//		or the caller doesn't have permission to change one of them.
//	-E_INVAL if srcva >= UTOP or srcva is not page-aligned,
//		or dstva >= UTOP or dstva is not page-aligned.
//	-E_INVAL is srcva is not mapped in srcenvid's address space.
//	-E_INVAL if perm is inappropriate (see sys_page_alloc).
//	-E_INVAL if (perm & PTE_W), but srcva is read-only in srcenvid's
//		address space.
//	-E_NO_MEM if there's no memory to allocate any necessary page tables.
static int
sys_page_map(envid_t srcenvid/*In fork: child*/, void *srcva,
	     envid_t dstenvid/*In fork: parent*/, void *dstva, int perm)
{
    // Hint: This function is a wrapper around page_lookup() and
    //   page_insert() from kern/pmap.c.
    //   Again, most of the new code you write should be to check the
    //   parameters for correctness.
    //   Use the third argument to page_lookup() to
    //   check the current permissions on the page.

    // LAB 4: Your code here.
    cprintf("===Execute in sys_page_map===\n");
    pte_t* src_pte = NIL;
    struct Page* src_page;
    struct Env *src_e;
    struct Env *dst_e;

    if(envid2env(srcenvid,&src_e,1) 
            || envid2env(dstenvid,&dst_e,0) 
            || (perm & (~PTE_SYSCALL)))
        return -E_BAD_ENV;

    if((check_addr_scale((uint32_t)srcva,0,(uint32_t)UTOP))
            || (check_addr_scale((uint32_t)dstva,0,(uint32_t)UTOP))
            || PGOFF(srcva)
            || PGOFF(dstva))
        return -E_INVAL;

    if((!(src_page = page_lookup(src_e->env_pgdir,srcva,&src_pte))))
        return -E_INVAL;

    // Perm has the same restrictions as in sys_page_alloc, except
    // that it also must not grant write access to a read-only
    // page.
    if( !((*src_pte) & PTE_W) && (perm & PTE_W))
        return -E_INVAL;

    return page_insert(dst_e->env_pgdir,src_page,dstva,perm);
}

// Unmap the page of memory at 'va' in the address space of 'envid'.
// If no page is mapped, the function silently succeeds.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if va >= UTOP, or va is not page-aligned.
static int
sys_page_unmap(envid_t envid, void *va)
{
    // Hint: This function is a wrapper around page_remove().
    // LAB 4: Your code here.
    cprintf("===Execute in sys_page_unmap===\n");
    struct Env* e;
    if(envid2env(envid,&e,1))
        return -E_BAD_ENV;

    if(check_addr_scale((uint32_t)va,0,(uint32_t)UTOP))
        return -E_INVAL;

    page_remove(e->env_pgdir,va);

    return 0;

}

// Try to send 'value' to the target env 'envid'.
// If srcva < UTOP, then also send page currently mapped at 'srcva',
// so that receiver gets a duplicate mapping of the same page.
//
// The send fails with a return value of -E_IPC_NOT_RECV if the
// target is not blocked, waiting for an IPC.
//
// The send also can fail for the other reasons listed below.
//
// Otherwise, the send succeeds, and the target's ipc fields are
// updated as follows:
//    env_ipc_recving is set to 0 to block future sends;
//    env_ipc_from is set to the sending envid;
//    env_ipc_value is set to the 'value' parameter;
//    env_ipc_perm is set to 'perm' if a page was transferred, 0 otherwise.
// The target environment is marked runnable again, returning 0
// from the paused sys_ipc_recv system call.  (Hint: does the
// sys_ipc_recv function ever actually return?)
//
// If the sender wants to send a page but the receiver isn't asking for one,
// then no page mapping is transferred, but no error occurs.
// The ipc only happens when no errors occur.
//
// Returns 0 on success, < 0 on error.
// Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist.
//		(No need to check permissions.)
//	-E_IPC_NOT_RECV if envid is not currently blocked in sys_ipc_recv,
//		or another environment managed to send first.
//	-E_INVAL if srcva < UTOP but srcva is not page-aligned.
//	-E_INVAL if srcva < UTOP and perm is inappropriate
//		(see sys_page_alloc).
//	-E_INVAL if srcva < UTOP but srcva is not mapped in the caller's
//		address space.
//	-E_INVAL if (perm & PTE_W), but srcva is read-only in the
//		current environment's address space.
//	-E_NO_MEM if there's not enough memory to map srcva in envid's
//		address space.
static int
sys_ipc_try_send(envid_t envid, uint32_t value, void *srcva, unsigned perm)
{
	// LAB 4: Your code here.
	panic("sys_ipc_try_send not implemented");
}

// Block until a value is ready.  Record that you want to receive
// using the env_ipc_recving and env_ipc_dstva fields of struct Env,
// mark yourself not runnable, and then give up the CPU.
//
// If 'dstva' is < UTOP, then you are willing to receive a page of data.
// 'dstva' is the virtual address at which the sent page should be mapped.
//
// This function only returns on error, but the system call will eventually
// return 0 on success.
// Return < 0 on error.  Errors are:
//	-E_INVAL if dstva < UTOP but dstva is not page-aligned.
static int
sys_ipc_recv(void *dstva)
{
	// LAB 4: Your code here.
	panic("sys_ipc_recv not implemented");
	return 0;
}

// Dispatches to the correct kernel function, passing the arguments.
int32_t
syscall (uint32_t syscallno, uint32_t a1, uint32_t a2, uint32_t a3,
         uint32_t a4, uint32_t a5)
{
    // Call the function corresponding to the 'syscallno' parameter.
    // Return any appropriate return value.
    // LAB 3: Your code here.
    //Ongoing: which call should I call...
    switch (syscallno)
    {
    case SYS_cputs:
        sys_cputs ((const char *) a1, a2);
        return 0;
    case SYS_cgetc:
        return sys_cgetc ();
    case SYS_getenvid:
        return sys_getenvid ();
    case SYS_env_destroy:
        return sys_env_destroy (a1);
    case SYS_yield:
         sys_yield();
         return 0;
    case SYS_exofork:
         return sys_exofork();
    case SYS_page_alloc:
         return sys_page_alloc(a1,(void*)a2,a3);
    case SYS_page_map:
         return sys_page_map(a1,(void*)a2,a3,(void*)a4,a5);
    case SYS_page_unmap:
         return sys_page_unmap(a1,(void*)a2);
    case SYS_env_set_status:
         return sys_env_set_status(a1,a2);
    default:
        return -E_INVAL;
    }

    //panic ("syscall not implemented");
}
