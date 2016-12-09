#ifndef __TIMER_H_b00517b21c0f48b49c6d2c7b50c5ddb8__
#define __TIMER_H_b00517b21c0f48b49c6d2c7b50c5ddb8__
/******************************************************************************/
#ifdef __APP__
#ifndef TM_MS
#define TM_MS           10
#endif

#ifndef TM_BIT
#define TM_BIT          8
#endif

#if !(2==TM_BIT || 4==TM_BIT || 8==TM_BIT || 16==TM_BIT)
#error "TM_BIT must be 2/4/8/16"
#endif

enum {
    TM_MASK     = os_mask(TM_BIT),
    TM_SLOT     = (TM_MASK+1),
    TM_RING     = (sizeof(uint32)*8/TM_BIT),
    TM_RINGMAX  = (TM_RING-1),
};

enum {
    TM_PENDING  = 0x01,
    TM_ONCE     = 0x02,
};

enum {
    TM_SAFE     = 0,
    TM_UNSAFE   = 1,
};

#define tm_SAFE(_result)        __mv2_return(TM_SAFE, _result)
#define tm_UNSAFE(_result)      __mv2_return(TM_UNSAFE, _result)

#define tm_is_safe(_tmu)        (TM_SAFE==mv2_control(_tmu))
#define tm_is_unsafe(_tmu)      (TM_UNSAFE==mv2_control(_tmu))

struct tm_node_s;
typedef mv_t tm_callback_f(struct tm_node_s *timer);

typedef struct tm_node_s {
    struct list_head node;
    tm_callback_f *cb;

    byte        flags;
    byte        ring_idx;
    uint16      ring_slot;
    uint32      expires;
    
    uint64      create;
} tm_node_t;

#define tm_timer_entry(_ptr, _type, _member) \
    safe_container_of(_ptr, _type, _member)

/*
* return
*   <0: error
*  >=0: sucess tigger times
*/
extern int
tm_trigger(uint32 times);

extern uint64
tm_ticks(void);

extern void // how much ms per tick
tm_unit_set(uint32 ms);

extern uint32 //ms, how much ms per tick
tm_unit(void);

extern int
tm_insert(
    tm_node_t *timer, // should embed in other struct
    int after, // ticks, after now
    tm_callback_f *cb,
    bool once
);

extern int
tm_remove(tm_node_t *timer);

/*
* call it in callback
*/
extern int
tm_once(tm_node_t *timer);

extern int
tm_change(tm_node_t *timer, uint32 after);

/*
* call it in callback
*/
extern int
tm_change_expires(tm_node_t *timer, uint32 expires);

/*
* return
*   >0: left time
*   =0: NOT pending
*   <0: error
*/
extern int //ticks
tm_left(tm_node_t *timer);

/*
* return
*   >0: expires(tm_insert's after)
*   =0: NOT pending
*   <0: error
*/
extern int //ticks
tm_expires(tm_node_t *timer);

extern bool
tm_is_pending(tm_node_t *timer);

extern bool
tm_is_once(tm_node_t *timer);
#define tm_is_cycle(_timer)     (false==tm_is_once(_timer))

extern void
tm_init(void);

extern void
tm_fini(void);

extern int
tm_fd(uint32 sec, uint32 nsec);

extern uint32
tm_fd_read(int fd);

static inline int
tm_fd_trigger(int fd)
{    
    return tm_trigger(tm_fd_read(fd));
}

extern int 
setup_timer(int sec, int usec);
#endif
/******************************************************************************/
#endif /* __TIMER_H_b00517b21c0f48b49c6d2c7b50c5ddb8__ */
