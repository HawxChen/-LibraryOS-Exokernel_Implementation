#ifndef JOS_INC_SPINLOCK_H
#define JOS_INC_SPINLOCK_H

#include <inc/types.h>

// Comment this to disable spinlock debugging
#define DEBUG_SPINLOCK

// Mutual exclusion lock.
char lock_record[2048];
struct spinlock {
    unsigned locked;		// Is the lock held?

#ifdef DEBUG_SPINLOCK
    // For debugging:
    char *name;			// Name of lock.
    struct Cpu *cpu;		// The CPU holding the lock.
    uintptr_t pcs[10];		// The call stack (an array of program counters)
    // that locked the lock.
#endif
};

void __spin_initlock(struct spinlock *lk, char *name);
void spin_lock(struct spinlock *lk);
void spin_unlock(struct spinlock *lk);

#define spin_initlock(lock)   __spin_initlock(lock, #lock)

extern struct spinlock kernel_lock;

static inline void lock_kernel(void) {
#ifdef bug_017
    extern uint32_t lock_cnt;
    spin_lock(&kernel_lock);
    lock_cnt++;
#else
    spin_lock(&kernel_lock);
#endif
}

static inline void unlock_kernel(void) {
#ifdef bug_017
    const unsigned int lock_div_base = 100000;
    extern uint32_t lock_cnt;
    static int i = 0;
    spin_unlock(&kernel_lock);
    lock_cnt++;
    if (i < lock_cnt / lock_div_base) {
	i = lock_cnt / lock_div_base;
	cprintf("unlock i:%d\n", lock_cnt);
	if (lock_cnt == 0xFFFFFFFF) {
	    i = 0;
	    lock_cnt = 0;
	}
    }
#else
    spin_unlock(&kernel_lock);
#endif
    // Normally we wouldn't need to do this, but QEMU only runs
    // one CPU at a time and has a long time-slice.  Without the
    // pause, this CPU is likely to reacquire the lock before
    // another CPU has even been given a chance to acquire it.
    asm volatile ("pause");
}

#endif
