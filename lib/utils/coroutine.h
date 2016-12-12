#ifndef __COROUTINE_H_5d8fd5ee4fd14fdaa141fc278df9d147__
#define __COROUTINE_H_5d8fd5ee4fd14fdaa141fc278df9d147__
#if defined(__APP__) && !defined(__UCLIBC__)
/******************************************************************************/
#ifndef USE_MOD_COROUTINE
#define USE_MOD_COROUTINE       PC_VAL(1, 0)
#endif

#ifndef CO_STACK_DEFAULT
#define CO_STACK_DEFAULT        (1024*8)
#endif

#ifndef CO_MAIL_DEFAULT
#define CO_MAIL_DEFAULT         32
#endif

#ifndef CO_SUSPEND_TIMEOUT
#define CO_SUSPEND_TIMEOUT      2000//ms
#endif

#ifndef CO_NAME_LEN
#define CO_NAME_LEN             31
#endif

#ifndef CO_IDLE_TIME
#define CO_IDLE_TIME            1000 /* 1000ns==1ms, for usleep */
#endif

#ifndef CO_IDLE_NAME 
#define CO_IDLE_NAME            "idle"
#endif

#ifndef CO_NOEXIST_NAME
#define CO_NOEXIST_NAME         "noexist"
#endif

#ifndef CO_MAIL_POINTER_COUNT
#define CO_MAIL_POINTER_COUNT   4
#endif

#ifndef CO_GROW
#define CO_GROW                 32
#endif

#if 0
TYPE        LP32    ILP32   LP64    ILP64   LLP64
CHAR        8       8       8       8       8
SHORT       16      16      16      16      16
INT         16      32      32      64      32
LONG        32      32      64      64      32
LONG LONG   64      64      64      64      64
POINTER     32      32      64      64      64       
#endif

enum co_pri {
    CO_PRI_HIGHEST  = 0,
    CO_PRI_NORMAL   = 8,
    CO_PRI_LOWEST   = 15,

    CO_PRI_DEFAULT  = CO_PRI_NORMAL,
    
    CO_PRI_END
};

enum co_event {
    /*
    * write: user
    * read : user
    */
    CO_EV_READABLE,

    /*
    * write: system
    * read : user
    */
    CO_EV_KILL,
    CO_EV_MAIL,

    /*
    * write: system
    * read : system
    */
    CO_EV_SUSPEND_TIMEOUT,  /* timeout */

    CO_EV_END
};

enum co_fsm {
    CO_READY,
    CO_SUSPEND,
    CO_RUNNING,
    CO_DEAD,
    
    CO_END
};

union co_mail {
    void    *P[CO_MAIL_POINTER_COUNT];
    uint8   B[CO_MAIL_POINTER_COUNT*sizeof(void*)/sizeof( uint8)];
    uint16  S[CO_MAIL_POINTER_COUNT*sizeof(void*)/sizeof(uint16)];
    uint32  I[CO_MAIL_POINTER_COUNT*sizeof(void*)/sizeof(uint32)];
    uint64  L[CO_MAIL_POINTER_COUNT*sizeof(void*)/sizeof(uint64)];
};

typedef uint32 co_id_t;
typedef uint64 co_cred_t;

#if OS64
typedef uint32 co_mask_t;
#else
typedef uint64 co_mask_t;
#endif

typedef int co_main_f(void *data);

typedef struct {
    uint32    protectedH; /* first */
    char name[1+CO_NAME_LEN];
    ucontext_t  ctx;
    co_id_t id;
    co_cred_t cred;
    
    struct {
        struct list_head node;
        enum co_fsm state;
    } fsm;
    
    struct {
        uint64 fsm[CO_END];
    } st;
    
    struct {
        co_main_f *func;
        void *data;
    } main;

    struct {
        enum co_pri old;
        enum co_pri now;
    } pri;

    uint32 *protectedT;
    
    struct {
        int size;
        char *sp;
    } stack;

    struct {
        co_mask_t mask;
        int mail_limit;
        
        channel_t *box[CO_EV_END];
    } ev;

    struct {
        co_cred_t cred;
        tm_node_t timer;
    } suspend;
    
    uint32    protectedM; /* last */
}
coroutine_t;

extern void
co_fini(void);

extern void
co_init(void);

/*
* CAN call in main
*
* return: CO ID
* 
* param:
*   @name: CO Name
*   @main: CO function
*   @data: CO function param
*   @stack_size: CO stack size
*       <0, use default(CO_STACK_DEFAULT)
*   @mail_limit: CO mail box size
*       <=0, use default(CO_MAIL_DEFAULT)
*   @pri: CO pri
*   @suspend: init fsm
*       true: init fsm as CO_SUSPEND
*       false:init fsm as CO_READY
*/
extern co_id_t
co_new(
    char *name,
    co_main_f *main, 
    void *data, 
    int stack_size, /* default 8K */
    int mail_limit,
    enum co_pri pri,
    bool suspend
);

/*
* if call in main, return the IDLE ID
*/
extern co_id_t
co_self(void);

/*
* can NOT call in main
*/
extern void
co_resume(co_id_t id);

/*
* can NOT call in main
*/
extern void
co_yield(void);

/*
* CAN call in main
*/
extern void
co_idle(void);

/*
* can NOT call in main
*/
extern co_cred_t
co_cred(void);

/*
* can NOT call in main
*
* @timeout(ms)
*   <0: wait forever, not setup timer
*   =0: wait one tick, auto setup timer
*   >0: wait timeout(ms), auto setup timer
*
* return:
*   <0: error
*   =0: ok
*   >0: timeout
*/
extern int
co_suspend(int timeout/* ms */, co_cred_t cred);

/*
* can NOT call in main
*/
extern int
co_ready(co_id_t id, co_cred_t cred, bool immediately);

/*
* can NOT call in main
*/
extern char *
co_name(co_id_t id);

/*
* CAN call in main
*/
extern char *
co_self_name(void);

extern int
co_signal(co_id_t id, enum co_event ev);

extern co_mask_t
co_evmask_read(void);

extern co_mask_t
co_evmask_read_and_zero(void);

extern bool
co_ev_read(enum co_event ev);

extern bool
co_ev_read_and_zero(enum co_event ev);

/*
* aoto set CO_EV_MAIL event
*/
extern int
co_mail_send(co_id_t id, union co_mail *mail);

/*
* aoto clear CO_EV_MAIL event
*/
extern int
co_mail_recv(union co_mail *mail);
/******************************************************************************/
#endif
#endif /* __COROUTINE_H_5d8fd5ee4fd14fdaa141fc278df9d147__ */
