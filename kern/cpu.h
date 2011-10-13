
#ifndef JOS_INC_CPU_H
#define JOS_INC_CPU_H

#include <inc/types.h>
#include <inc/memlayout.h>
#include <inc/mmu.h>
#include <inc/env.h>

// Maximum number of CPUs
#define NCPU  8

// Values of status in struct Cpu
enum {
	CPU_UNUSED = 0,
	CPU_STARTED,
};

// Per-CPU state
struct Cpu {
	uint8_t cpu_id;                 // Local APIC ID; index into cpus[] below
	volatile unsigned cpu_status;   // The status of the CPU
	struct Env *cpu_env;            // The currently-running environment.
	struct Taskstate cpu_ts;        // Used by x86 to find stack for interrupt
};

// Initialized in mpconfig.c
extern struct Cpu cpus[NCPU];
extern int ncpu;                    // Total number of CPUs in the system
extern struct Cpu *bootcpu;         // The boot-strap processor (BSP)
extern volatile uint32_t *lapic;    // MMIO address to access the local-APIC

// Per-CPU kernel stacks
extern unsigned char percpu_kstacks[NCPU][KSTKSIZE];

int cpunum(void);
#define thiscpu (&cpus[cpunum()])

void mp_init(void);
void lapic_init(void);
void lapic_startap(uint8_t apicid, uint32_t addr);
void lapic_eoi(void);
void lapic_ipi(int vector);

#endif
