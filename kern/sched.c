#include <inc/assert.h>

#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/monitor.h>


// Choose a user environment to run and run it.
void
sched_yield(void)
{
	struct Env *idle;
	int i = -1,cnt;

	// Implement simple round-robin scheduling.
	//
	// Search through 'envs' for an ENV_RUNNABLE environment in
	// circular fashion starting just after the env this CPU was
	// last running.  Switch to the first such environment found.
	//
	// If no envs are runnable, but the environment previously
	// running on this CPU is still ENV_RUNNING, it's okay to
	// choose that environment.
	//
	// Never choose an environment that's currently running on
	// another CPU (env_status == ENV_RUNNING) and never choose an
	// idle environment (env_type == ENV_TYPE_IDLE).  If there are
	// no runnable environments, simply drop through to the code
	// below to switch to this CPU's idle environment.

	// LAB 4: Your code here.

        if(0 == thiscpu->cpu_env)
        {
            i = 0;
        }
        else if(thiscpu->cpu_env->env_status == ENV_RUNNING)
        { /*Note: It might check for future needed state*/
            thiscpu->cpu_env->env_status = ENV_RUNNABLE;
            i = (curenv - envs) + 1;//bug_019
        }

        if(i == -1)
        {
           i = thiscpu->cpu_env - envs;
        }
        
	for ( cnt = 0 ;cnt < NENV ;i++, cnt++) 
        {
            if(i >= NENV)
                i = i % NENV;
            if(envs[i].env_type != ENV_TYPE_IDLE && envs[i].env_status == ENV_RUNNABLE)
            {
                /*env_run will do 
                 * 1: set thiscpu's cpu_env to the passed env. 
                 * 2: set env's state to ENV_RUNNING
                 * */
                env_run(&envs[i]);
                break;
            }
        }

	// For debugging and testing purposes, if there are no
	// runnable environments other than the idle environments,
	// drop into the kernel monitor.
#ifdef TESTING_GRADE_PURPOSE
	for (i = 0; i < NENV; i++) {
		if (envs[i].env_type != ENV_TYPE_IDLE &&
		    (envs[i].env_status == ENV_RUNNABLE ||
                     envs[i].env_status == ENV_RUNNING))
                {
//                    cprintf("===env %d is the GUY===\n",i); //Debug
                    break;
                }
	}
	if (i == NENV) {
		cprintf("No more runnable environments!\n");
		while (1) monitor(NULL);
	}
#endif

        // Run this CPU's idle environment when nothing else is runnable.'
        if(cnt == NENV)
        {
            idle = &envs[cpunum()];
            if (!(idle->env_status == ENV_RUNNABLE || idle->env_status == ENV_RUNNING))
                panic("CPU %d: No idle environment!", cpunum());
            env_run(idle);
            //Compare it to co-routine scheduler in xv6
            //It is never returned here.
            cprintf("CPU %d is back\n",cpunum());
        }


}
