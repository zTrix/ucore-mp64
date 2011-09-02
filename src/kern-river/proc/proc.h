#ifndef __RIVER_PROC_H__
#define __RIVER_PROC_H__

#include <libs/types.h>
#include <glue_context.h>
#include <glue_mp.h>

#include <proc/sched.h>
#include <proc/event.h>

#define MAX_PROC_NAME 32

typedef void (*proc_idle_f) (void);
typedef struct proc_s *proc_t;
typedef struct proc_s
{
	uint32_t     type;
	uint32_t     status;
	char         name[MAX_PROC_NAME];
	
	context_s    kern_ctx;
	sched_node_s sched_node;
	uint32_t     time_slice;
	uint32_t     irq_save_level;

	proc_idle_f  idle;
	void        *private;

	event_pool_s event_pool;
} proc_s;

#define PROC_TYPE_UNKNOWN 0x0
#define PROC_TYPE_KTHREAD 0x1
#define PROC_TYPE_UTHREAD 0x2
#define PROC_TYPE_IDLE    0x3

#define PROC_STATUS_UNINIT          0
#define PROC_STATUS_RUNNABLE        1
#define PROC_STATUS_WAIT            2
#define PROC_STATUS_ZOMBIE          3

extern volatile proc_t proc_current;

int  proc_init(void);
int  proc_open(proc_t proc, const char *name, proc_idle_f idle, void *private, uintptr_t stack);
void proc_schedule(void);
int  proc_close(proc_t proc);
int  proc_exit(void);

#endif
