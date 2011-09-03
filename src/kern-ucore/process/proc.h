#ifndef __KERN_PROCESS_PROC_H__
#define __KERN_PROCESS_PROC_H__

#include <types.h>
#include <list.h>
#include <trap.h>
#include <memlayout.h>
#include <unistd.h>
#include <sem.h>
#include <event.h>
#include <glue_mp.h>

// process's state in his life cycle
enum proc_state {
    PROC_UNINIT = 0,  // uninitialized
    PROC_SLEEPING,    // sleeping
    PROC_RUNNABLE,    // runnable(maybe running)
    PROC_ZOMBIE,      // almost dead, and wait parent proc to reclaim his resource
};

#define PROC_ATTR_ROLE 3
#define PROC_ATTR_ROLE_NORMAL 0
#define PROC_ATTR_ROLE_IDLE   1

#define PROC_IS_IDLE(proc)   (((proc)->attribute & PROC_ATTR_ROLE) == PROC_ATTR_ROLE_IDLE)

struct context {
    uint64_t rip;
    uint64_t rsp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t rbx;
    uint64_t rbp;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
};

#define PROC_NAME_LEN               15
#define MAX_PROCESS                 4096
#define MAX_PID                     (MAX_PROCESS * 2)

extern list_entry_t proc_list;
extern list_entry_t proc_mm_list;

struct inode;
struct fs_struct;

struct proc_struct {
    enum proc_state state;                      // Process state
	// uint32_t attribute;                         // Special attributes
    int pid;                                    // Process ID
    int runs;                                   // the running times of Proces
    uintptr_t kstack;                           // Process kernel stack
    volatile bool need_resched;                 // bool value: need to be rescheduled to release CPU?
    struct proc_struct *parent;                 // the parent process
    struct mm_struct *mm;                       // Process's memory management field
    struct context context;                     // Switch here to run process
    struct trapframe *tf;                       // Trap frame for current interrupt
    uintptr_t cr3;                              // CR3 register: the base addr of Page Directroy Table(PDT)
    uint32_t flags;                             // Process flag
    char name[PROC_NAME_LEN + 1];               // Process name
    list_entry_t list_link;                     // Process link list 
    list_entry_t hash_link;                     // Process hash list
    int exit_code;                              // return value when exit
    uint32_t wait_state;                        // Process waiting state: the reason of sleeping
    struct proc_struct *cptr, *yptr, *optr;     // Process's children, yonger sibling, Old sibling
    list_entry_t thread_group;                  // the threads list including this proc which share resource (mem/file/sem...)
    struct run_queue *rq;                       // running queue contains Process
    list_entry_t run_link;                      // the entry linked in run queue
    int time_slice;                             // time slice for occupying the CPU
    sem_queue_t *sem_queue;                     // the user semaphore queue which process waits
    event_t event_box;                          // the event which process waits   
    struct fs_struct *fs_struct;                // the file related info(pwd, files_count, files_array, fs_semaphore) of process
};

#define PF_EXITING                  0x00000001      // getting shutdown

//the wait state
#define WT_CHILD                    (0x00000001 | WT_INTERRUPTED)  // wait child process
#define WT_TIMER                    (0x00000002 | WT_INTERRUPTED)  // wait timer
#define WT_KSWAPD                    0x00000003                    // wait kswapd to free page
#define WT_KBD                      (0x00000004 | WT_INTERRUPTED)  // wait the input of keyboard
#define WT_KSEM                      0x00000100                    // wait kernel semaphore
#define WT_USEM                     (0x00000101 | WT_INTERRUPTED)  // wait user semaphore
#define WT_EVENT_SEND               (0x00000110 | WT_INTERRUPTED)  // wait the sending event
#define WT_EVENT_RECV               (0x00000111 | WT_INTERRUPTED)  // wait the recving event 
#define WT_MBOX_SEND                (0x00000120 | WT_INTERRUPTED)  // wait the sending mbox
#define WT_MBOX_RECV                (0x00000121 | WT_INTERRUPTED)  // wait the recving mbox
#define WT_PIPE                     (0x00000200 | WT_INTERRUPTED)  // wait the pipe
#define WT_INTERRUPTED               0x80000000                    // the wait state could be interrupted

#define le2proc(le, member)         \
    to_struct((le), struct proc_struct, member)

extern struct proc_struct * volatile current;
extern struct proc_struct * volatile idleproc;
extern struct proc_struct *initproc;
extern struct proc_struct *kswapd;

void proc_init(void);
void proc_init_ap(void);

void proc_run(struct proc_struct *proc);
int kernel_thread(int (*fn)(void *), void *arg, uint32_t clone_flags);

char *set_proc_name(struct proc_struct *proc, const char *name);
char *get_proc_name(struct proc_struct *proc);
void cpu_idle(void) __attribute__((noreturn));

struct proc_struct *find_proc(int pid);
void may_killed(void);
int do_fork(uint32_t clone_flags, uintptr_t stack, struct trapframe *tf);
int do_exit(int error_code);
int do_exit_thread(int error_code);
int do_execve(const char *name, int argc, const char **argv);
int do_yield(void);
int do_wait(int pid, int *code_store);
int do_kill(int pid, int error_code);
int do_brk(uintptr_t *brk_store);
int do_sleep(unsigned int time);
int do_mmap(uintptr_t *addr_store, size_t len, uint32_t mmap_flags);
int do_munmap(uintptr_t addr, size_t len);
int do_shmem(uintptr_t *addr_store, size_t len, uint32_t mmap_flags);

#endif /* !__KERN_PROCESS_PROC_H__ */

