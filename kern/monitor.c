// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/kdebug.h>
#include <kern/trap.h>

#define CMDBUF_SIZE	80          // enough for one VGA text line


struct Command
{
    const char *name;
    const char *desc;
    // return -1 to force monitor to exit
    int (*func) (int argc, char **argv, struct Trapframe * tf);
};

//command list
static struct Command commands[] = {
    {"help", "Display this list of commands", mon_help},
    {"kerninfo", "Display information about the kernel", mon_kerninfo},
    {"backtrace", "Display current calling stack", mon_backtrace},
};

#define NCOMMANDS (sizeof(commands)/sizeof(commands[0]))

unsigned read_eip ();

/***** Implementations of basic kernel monitor commands *****/

int
mon_help (int argc, char **argv, struct Trapframe *tf)
{
    int i;

    for (i = 0; i < NCOMMANDS; i++)
        cprintf ("%s - %s\n", commands[i].name, commands[i].desc);
    return 0;
}

int
mon_kerninfo (int argc, char **argv, struct Trapframe *tf)
{
    extern char entry[], etext[], edata[], end[];

    cprintf ("Special kernel symbols:\n");
    cprintf ("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
    cprintf ("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
    cprintf ("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
    cprintf ("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
    cprintf ("Kernel executable memory footprint: %dKB\n",
             (end - entry + 1023) / 1024);
    return 0;
}

/*
   Stack backtrace:
   ebp f0109e58  eip f0100a62  args 00000001 f0109e80 f0109e98 f0100ed2 00000031
   ebp f0109ed8  eip f01000d6  args 00000000 00000000 f0100058 f0109f28 00000061
   ...
 */
#define FORMAT_LENGTH 80
#define EBP(_v) ((uint32_t)_v)
//After
#define EIP(_ebp) ((uint32_t)*(_ebp+1))
#define ARG(_v,_cnt) ((uint32_t)*(_v+((_cnt)+2)))


int
mon_backtrace (int argc, char **argv, struct Trapframe *tf)
{
    // Your code here.
    int32_t cnt = 0;
    uint32_t *addr = 0;
    char format[FORMAT_LENGTH] = { 0 };
    char formatName[FORMAT_LENGTH] = { 0 };
    struct Eipdebuginfo info;
    strcpy (format, "  ebp %08x eip %08x args %08x %08x %08x %08x %08x\n");
    strcpy (formatName, "         %s:%d: %.*s+%d\n");
    addr = (uint32_t *) read_ebp ();

    cprintf ("Stack backtrace\n");
    for (; NULL != addr; cnt++)
    {
        cprintf (format, EBP (addr), EIP (addr), ARG (addr, 0), ARG (addr, 1),
                 ARG (addr, 2), ARG (addr, 3), ARG (addr, 4));

        debuginfo_eip (EIP (addr), &info);
        cprintf (formatName,
                 info.eip_file,
                 info.eip_line,
                 info.eip_fn_namelen,
                 info.eip_fn_name, EIP (addr) - info.eip_fn_addr);
        //Trace the linked list implemented by Stack.
        addr = (uint32_t *) * addr;
    }

    return 0;
}



/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd (char *buf, struct Trapframe *tf)
{
    int argc;
    char *argv[MAXARGS];
    int i;

    // Parse the command buffer into whitespace-separated arguments
    argc = 0;
    argv[argc] = 0;
    while (1)
    {
        // gobble whitespace
        while (*buf && strchr (WHITESPACE, *buf))
            *buf++ = 0;
        if (*buf == 0)
            break;

        // save and scan past next arg
        if (argc == MAXARGS - 1)
        {
            cprintf ("Too many arguments (max %d)\n", MAXARGS);
            return 0;
        }
        argv[argc++] = buf;
        while (*buf && !strchr (WHITESPACE, *buf))
            buf++;
    }
    argv[argc] = 0;

    // Lookup and invoke the command
    if (argc == 0)
        return 0;
    for (i = 0; i < NCOMMANDS; i++)
    {
        if (strcmp (argv[0], commands[i].name) == 0)
            return commands[i].func (argc, argv, tf);
    }
    cprintf ("Unknown command '%s'\n", argv[0]);
    return 0;
}

void
monitor (struct Trapframe *tf)
{
    char *buf;

    cprintf ("Welcome to the JOS kernel monitor!\n");
    cprintf ("Type 'help' for a list of commands.\n");

	if (tf != NULL)
		print_trapframe(tf);

    while (1)
    {
        buf = readline ("K> ");
        if (buf != NULL)
            if (runcmd (buf, tf) < 0)
                break;
    }
}

// return EIP of caller.
// does not work if inlined.
// putting at the end of the file seems to prevent inlining.
unsigned
read_eip ()
{
    uint32_t callerpc;
    __asm __volatile ("movl 4(%%ebp), %0":"=r" (callerpc));
    return callerpc;
}
