// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>


// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
//#define  __OBSERVE_USR_PGTABLE__ //Here could see the user page table and its content.
static void
pgfault(struct UTrapframe *utf)
{
    void *addr = (void *) utf->utf_fault_va;
    uint32_t err = utf->utf_err;
    pte_t pte = 0;
    int r;
    //FEC_WR: check it.
    // Check that the faulting access was (1) a write, and (2) to a
    // copy-on-write page.  If not, panic.
    // Hint:
    //   Use the read-only page table mappings at vpt
    //   (see <inc/memlayout.h>).

    // Hawx: what's about the normal page fault handle, write to non exist address?
    // LAB 4: Your code here.
    pte = vpt[PGNUM(addr)];
#ifdef DEBUG_LIB_FORK_C
    cprintf("\t----- pgfault: thisenv_id:0x%4x, va:0x%x,pgn:0x%x\n", thisenv->env_id, (int)addr, (int)pte);
#endif
    if((PTE_P | PTE_U) != ((vpd[PDX(addr)] & (PTE_P | PTE_U))))
    {
        r = -1;
        goto FAIL;
    }

    pte = vpt[PGNUM(addr)];
    if(!( (PTE_P & pte) && (FEC_WR & err) && (pte & PTE_COW) ))
    {
        r = -2;
        goto FAIL;
    }

    // Allocate a new page, map it at a temporary location (PFTEMP),
    // copy the data from the old page to the new page, then move the new
    // page to the old page's address.
    // Hint:
    //   You should make three system calls.
    //   No need to explicitly delete the old page's mapping.

    // LAB 4: Your code here.
    //It might be something wrong. Check It, now.
    //Question: what's about the old mapping?!
    //As the old page for Read-Only
    //Now problem is rcsv rcsv rcsv then stack underflow.
    //Look Jos.out
    if ((r = sys_page_alloc(0, (void*)PFTEMP, PTE_P | PTE_U | PTE_W)) < 0)
    {
        r = -3;
        goto FAIL;
    }
    memmove((void*)PFTEMP,(void*)ROUNDDOWN(addr,PGSIZE) , PGSIZE);
    if ((r = sys_page_map(0,(void*)PFTEMP, 0, (void*)ROUNDDOWN(addr,PGSIZE), PTE_P | PTE_U | PTE_W)) < 0)
    {
        r = -4;
        goto FAIL;
    }
    if ((r = sys_page_unmap(0,(void*) PFTEMP)) < 0)
    {
        r = -5;
        goto FAIL;
    }


#ifdef DEBUG_LIB_FORK_C
    cprintf("\t----- done pgfault: thisenv_id:0x%4x, va:0x%x,pgn:0x%x\n", thisenv->env_id, (int)addr, (int)pte);
#endif
    return;
FAIL:
    panic("page fault handler failed by: %d\n",r);
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  
// (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
// 2 fork concurrent?
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
#if 1
    int r;
    pte_t *pte = NULL;
    if(!((PTE_P | PTE_U) == (vpd[pn >> PT_DIGITS] & (PTE_P | PTE_U)) )) {
        cprintf("Not Pass:vpd[pn >> PT_DIGITS] | (PTE_P | PTE_U)\n");
        return -E_INVAL;
    }


    /*Error: use the physical address as the virtual address.
    pgtable = (virtaddr_t*)PDE_ADDR(vpd[pn >> PT_DIGITS]);
    if(!((PTE_U | PTE_P) != (pgtable[PGNUM2PTX(pn)] & (PTE_U | PTE_P)))) {
        cprintf("Not Pass:pgtable[pn & 0x3FF] (PTE_U | PTE_P)))\n");
        return -E_INVAL;
    }
    */
    if((PTE_U | PTE_P) != (vpt[pn] & (PTE_U | PTE_P))) {
        cprintf("Not Pass:vpt[pn] (PTE_U | PTE_P)))\n");
        return -E_INVAL;
    }



    /*Error: use the physical address as the virtual address.
    if(!(pgtable[pn&0x3FF] & (PTE_W | PTE_COW))) {
        sys_page_map(0,
                (void*)PTE_ADDR(pgtable[PGNUM2PTX(pn)]),
                envid,
                (void*)PGNUM2LA(pn),
                PGOFF(pgtable[PGNUM2PTX(pn)]));
        return 0;
    }
    */
    //For read only page
    if(!(vpt[pn] & (PTE_W | PTE_COW))) {
        sys_page_map(0,
                (void*)PGNUM2LA(pn),
                envid,
                (void*)PGNUM2LA(pn),
                PGOFF(vpt[pn]));

        return 0;
    }

    // (Exercise: Why do we need to mark ours
    // copy-on-write again if it was already copy-on-write at the beginning of
    // this function?)
    // A: 2 fork concurrent?
    //  or
    // B: This page was happaned to be page fault to be allocated as the new one?
    
    //PTE_R will cause the page fault happaned.
    //PTE_COW will inform us to do copy on write.
#ifdef DEBUG_SYSCALL_C
//  cprintf("Before Mapped as COW: 0x%x\n",(PGOFF(vpt[pn]) | PTE_COW) & (~PTE_W));
#endif
    sys_page_map(0,
            (void*)PGNUM2LA(pn),
            envid,
            (void*)PGNUM2LA(pn),
            (PGOFF(vpt[pn]) | PTE_COW) & (~PTE_W));

    sys_page_map(0,
            (void*)PGNUM2LA(pn),
            0,
            (void*)PGNUM2LA(pn),
            (PGOFF(vpt[pn]) | PTE_COW) & (~PTE_W));
#ifdef DEBUG_SYSCALL_C
//    cprintf("Mapped as COW: 0x%x\n",PGOFF(vpt[pn]));
#endif



    return 0;
#else
    // LAB 4: Your code here.
    panic("duppage not implemented");
    return 0;
#endif
}

//
// User-level fork with copy-on-write.
// 1.Set up our page fault handler appropriately.
// 2.Create a child.
// 3.Copy our address space and page fault handler setup to the child.
// 4.Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   1.Use vpd, vpt, and duppage.
//   2.Remember to fix "thisenv" in the child process.
//   3.Neither user exception stack should ever be marked copy-on-write,
//     so you must allocate a new page for the child's user exception stack.
//
extern void _pgfault_upcall(void);
envid_t
fork(void)
{
#if 1
    // LAB 4: Your code here.
    int child_envid = 0;
    int r,pgn;
    int dir_i, table_i;
    struct Env *child_env = NULL;

    //const volatile struct Env *thisenv;
    set_pgfault_handler(pgfault);
    child_envid = sys_exofork();
    if(child_envid < 0)
    {
        panic("fork in copy-on-write:%e",child_envid);
    }
    else if(0 == child_envid)
    {
        thisenv = &envs[ENVX(sys_getenvid())];
        set_pgfault_handler(pgfault);
        //Normal user satck had be created in the env_alloc
        //Now, We set the UXSTACK
#ifdef DEBUG_LIB_FORK_C
        cprintf("I am Child:0x%x Return\n",thisenv->env_id);
#endif
        return 0;
    }
    //Error: child_env = &envs[ENVX(child_envid)];
    child_env =(struct Env *) &envs[ENVX(child_envid)];
#ifdef DEBUG_LIB_FORK_C
    cprintf("My child's envid:0x%x, I am 0x%x\n",child_envid, thisenv->env_id);
#endif
    sys_env_set_pgfault_upcall(child_envid, _pgfault_upcall);

    /*
       Correct Concept: user mode can't copy child's env_pgidr from parent's env_pgidr
       if((r = sys_page_map(child_env->env_id,(void*)vpd,0,UTEMP,PTE_P | PTE_U | PTE_W)) < 0)
       panic("sys_page_map: %e !!!!!\n", r);
       memmove(UTEMP,(void*)vpd,PGSIZE);
     */

    //Here needs to copy pgdir from parent to child

    /*
     * Here we do is the "remap"
     */ 
    /*
     *vpd: 0xef7bd000
     *vpt: 0xef400000
     */
#ifdef __OBSERVE_USR_PGTABLE__ //Here could see the user page table and its content.
    //Acording to inc/memlayout.h, Observe address translation: 
    //Virtual -> Linear -> Physical 
    cprintf("vpd:0x%08x,vpt:0x%08x\n",(uint32_t)vpd,(uint32_t)vpt);
    cprintf("Current envid:0x%x\n",thisenv->env_id);
    for(dir_i = 0; dir_i < (ULIM >> PDXSHIFT) ; dir_i++)
        //    for(dir_i = 0; dir_i < (UVPT >> PDXSHIFT) ; dir_i++)
    {
        if(0 != vpd[dir_i])
        {
            cprintf("addr:0x%08x,dir_i = 0x%x,vpd[dir_i]=0x%08x,&vpd[dir_i]:0x%08x\n",
                    dir_i << PDXSHIFT,
                    dir_i,
                    vpd[dir_i],
                    &vpd[dir_i]);
            for(table_i = dir_i*1024 ; table_i < ((dir_i+1)*1024) ; table_i++)
            {
                //                cprintf("\t\ttable_i:0x%08x\n",table_i);
                if(0 != vpt[table_i])
                {
                    cprintf("\taddr:0x%08x, pageNumber = 0x%x, table index = 0x%x, vpt[table_i]=0x%08x, &vpt[table_i]=0x%08x\n",
                            (table_i << PTXSHIFT),
                            table_i,
                            table_i & 0x3FF,
                            vpt[table_i],
                            &vpt[table_i]);
                }
            }
        }
    }
#endif //Here could see the user page table

    //Kernel scale's pgdir has been setup by env_setup_vm
    //Now we only has the ability to focus on User mode's scale
    for(pgn = 0; pgn < PGNUM(UTOP); pgn++)
    {
        if(pgn == PGNUM(UXSTACKBASE))
        {
            //allocation
            sys_page_alloc(child_envid, (void*) UXSTACKBASE, PTE_U | PTE_W | PTE_P);
        }
        else if(pgn < PGNUM(UXSTACKBASE) 
                && ( (PTE_P | PTE_U) == (vpd[pgn >> 10] & (PTE_P | PTE_U)))
                && ((PTE_P | PTE_U) == (vpt[pgn] & (PTE_P | PTE_U))) )
        {
#ifdef DEBUG_LIB_FORK_C
            cprintf("pgn = 0x%x\n", pgn);
#endif
            duppage(child_envid, pgn );
        }
        //02/20: here needed to re-think or even re-write.
    }


    // Start the child environment running
    if ((r = sys_env_set_status(child_envid, ENV_RUNNABLE)) < 0)
        panic("sys_env_set_status: %e!!!!!\n", r);

#ifdef DEBUG_LIB_FORK_C
    cprintf("allocated envid:0x%x\n",child_envid);
#endif
    return child_envid;
#else
    panic("Not Implemented Yet");
    return 0;
#endif 
}

// Challenge!
int
sfork(void)
{
    panic("sfork not implemented");
    return -E_INVAL;
}
