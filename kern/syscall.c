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

// Print a string to the system console.
// The string is exactly 'len' characters long.
// Destroys the environment on memory errors.
static void
sys_cputs (const char *s, size_t len)
{
    // Check that the user has permission to read memory [s, s+len).
    // Destroy the environment if not.
    size_t iterate_len = len;
    char *s_cnt = (char *) s;
    bool flg = FALSE;

    // LAB 3: Your code here.
    user_mem_assert(curenv,(const char *)s,len,PTE_U | PTE_P);

    pte_t *ppte = NIL;
    do
    {
        ppte = pgdir_walk (curenv->env_pgdir, (void *) ROUNDDOWN (s_cnt, PGSIZE),NO_CREATE);
        if (NIL == ppte)
        {
            break;
        }

        if (!(*ppte & PTE_P))
        {
            break;
        }

        if (!(*ppte & PTE_U))
        {
            break;
        }
        cprintf ("!!!env_pgdir:0x%x,ppte:0x%x,pte:0x%x,s_cnt:0x%x\n",
                curenv->env_pgdir,(uint32_t)ppte,*ppte,(uint32_t)s_cnt);
        // Print the string supplied by the user.
        if (iterate_len < PGSIZE)
        {
            //Boundary condition
            s_cnt += iterate_len;
        }
        else
        {
            iterate_len -= PGSIZE;
            s_cnt += PGSIZE;
        }
        cprintf ("%.*s", len, s);
    // LAB 3: Your code here.
    //user_mem_assert(curenv,(const char *)s,len,PTE_U | PTE_P);
        return;

    }
    while (s_cnt <= (s + len));

    if (NIL != curenv)
    {
        env_destroy (curenv);
    }
    else
    {
        panic ("Serious Error");
    }
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
    default:
        return -E_INVAL;
    }

    //panic ("syscall not implemented");
}
