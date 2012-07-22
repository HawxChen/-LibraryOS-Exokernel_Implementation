// System call stubs.

#include <inc/syscall.h>
#include <inc/lib.h>

static inline int32_t
syscall (int num, int check, uint32_t a1, uint32_t a2, uint32_t a3,
         uint32_t a4, uint32_t a5)
{
    int32_t ret;

    // Generic system call: pass system call number in AX,
    // up to five parameters in DX, CX, BX, DI, SI.
    // Interrupt kernel with T_SYSCALL.
    //
    // The "volatile" tells the assembler not to optimize
    // this instruction away just because we don't use the
    // return value.
    //
    // The last clause tells the assembler that this can
    // potentially change the condition codes and arbitrary
    // memory locations.

#if 0
    asm volatile ("int %1\n":"=a" (ret):"i" (T_SYSCALL),
                  "a" (num),
                  "d" (a1),
                  "c" (a2), "b" (a3), "D" (a4), "S" (a5):"cc", "memory");
#endif
    asm volatile (
                  "pushl %%edx\n\t"
                  "pushl %%ecx\n\t"
                  "pushl %%ebx\n\t"
                  "pushl %%edi\n\t"
                  "pushl %%ebp\n\t"
                  "movl  %%esp,%%ebp\n\t"
                  "leal 0f, %%esi\n\t"
                  "sysenter\n\t"
                  "0:\n\t"
                  "popl %%ebp\n\t"
                  "popl %%edi\n\t"
                  "popl %%ebx\n\t"
                  "popl %%ecx\n\t"
                  "popl %%edx\n\t"
                  :"=a" (ret):"a" (num), "d" (a1), "c" (a2),
                  "b" (a3), "D" (a4):"%esp", "%esi");
#if 0
    asm volatile ("pushl %%ebp\n\t"
                  "movl  %%esp,%%ebp\n\t"
                  "leal 0f, %%esi\n\t"
                  "sysenter\n\t"
                  "0:\n\t"
                  "popl %%ebp\n\t"
                  :"=a" (ret):"a" (num), "d" (a1), "c" (a2),
                  "b" (a3), "D" (a4):"%ecx","%edx","%esp", "%esi");
#endif
    if (check && ret > 0)
        panic ("syscall %d returned %d (> 0)", num, ret);

    return ret;
}

void
sys_cputs (const char *s, size_t len)
{
    syscall (SYS_cputs, 0, (uint32_t) s, len, 0, 0, 0);
}

int
sys_cgetc (void)
{
    return syscall (SYS_cgetc, 0, 0, 0, 0, 0, 0);
}

int
sys_env_destroy (envid_t envid)
{
    return syscall (SYS_env_destroy, 1, envid, 0, 0, 0, 0);
}

envid_t
sys_getenvid (void)
{
    return syscall (SYS_getenvid, 0, 0, 0, 0, 0, 0);
}
