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

struct __tm_node;
typedef mv_t tm_callback_f(struct __tm_node *timer);

typedef struct __tm_node {
    struct list_head node;
    tm_callback_f *cb;

    byte        flags;
    byte        ring_idx;
    uint16      ring_slot;
    uint32      expires;
    
    uint64      create;
} tm_node_t;

typedef struct {
    struct {
        struct list_head list;
        uint32 count;
    } slot[TM_SLOT];
    
    uint32 current;
    uint32 count;
} tm_ring_t;

typedef struct {
    bool        init;
    uint32    r;
    uint32    count;
    uint32    unit; // how much ms per tick
    uint64    ticks;
    tm_ring_t   ring[TM_RING];

    uint64    triggered_safe;
    uint64    triggered_unsafe;
    uint64    triggered_error;
    uint64    triggered_ok;
    uint64    inserted;
    uint64    removed;
} tm_clock_t;

#define DECLARE_REAL_TIMER  tm_clock_t __THIS_TIMER
#define DECLARE_FAKE_TIMER  extern tm_clock_t __THIS_TIMER

#ifdef __BUSYBOX__
#   define DECLARE_TIMER    DECLARE_FAKE_TIMER
#else
#   define DECLARE_TIMER    DECLARE_REAL_TIMER
#endif

DECLARE_FAKE_TIMER;

static inline tm_clock_t *
__this_timer(void)
{
    return &__THIS_TIMER;
}

static inline tm_ring_t *
__tm_ring(int idx)
{
    return &__this_timer()->ring[idx];
}

#define __tm_ring0                  __tm_ring(0)
#define __tm_ringx(_x)              __tm_ring(_x)
#define __tm_ringmax                __tm_ring(TM_RINGMAX)
#define __tm_foreach_ring(_ring)    for((_ring)=__tm_ring0; (_ring)<=__tm_ringmax; (_ring)++)
#define __tm_foreach_slot(_slot)    for((_slot)=0; (_slot)<TM_SLOT; (_slot)++)

static inline struct list_head *
__tm_slot(tm_ring_t *ring, int idx)
{
    return &ring->slot[idx].list;
}

static inline bool
__tm_is_once(tm_node_t *timer)
{
    return os_hasflag(timer->flags, TM_ONCE);
}
#define __tm_is_cycle(_timer)   (false==__tm_is_once(_timer))

static inline bool
__tm_is_pending(tm_node_t *timer)
{
    return os_hasflag(timer->flags, TM_PENDING);
}

static inline uint32
__tm_left(tm_node_t *timer)
{
    uint64 timeout = timer->create + (uint64)timer->expires;
    
    if (timeout > __this_timer()->ticks) {
        return (uint32)(timeout - __this_timer()->ticks);
    } else {
        return 0;
    }
}

static inline tm_ring_t *
__tm_find_ring(tm_node_t *timer, int *slot)
{
    int idx;
    uint32 left = __tm_left(timer);
    uint32 offset;
    tm_ring_t *ring;
    
    for (idx=TM_RINGMAX; idx>0; idx--) {
        offset = left;
        offset <<= TM_BIT*(TM_RINGMAX-idx);
        offset >>= TM_BIT*TM_RINGMAX;
        if (offset) {
            break;
        }
    }

    if (0==idx) {
        offset = left;
    }

    ring = __tm_ringx(idx);
    
    *slot = (ring->current + offset) & TM_MASK;
    
    return ring;
}

static inline void
__tm_insert(tm_node_t *timer)
{
    int slot = 0;
    tm_ring_t *ring = __tm_find_ring(timer, &slot);

    list_add(&timer->node, __tm_slot(ring, slot));
    timer->ring_idx     = ring - __tm_ring0;
    timer->ring_slot    = slot;
    timer->flags        |= TM_PENDING;

    ring->slot[slot].count++;
    ring->count++;
    __this_timer()->count++;
    __this_timer()->inserted++;
    
    if (__this_timer()->ticks) {
        //debug_test("timer(expires:%u) insert to ring(%d) slot(%d)", node->expires, node->ring_idx, node->ring_slot);
    }
}

static inline void
__tm_remove(tm_node_t *timer)
{
    tm_ring_t *ring = __tm_ringx(timer->ring_idx);

    if (__this_timer()->ticks) {
        //debug_test("timer(expires:%u) remove from ring(%d) slot(%d)", node->expires, node->ring_idx, node->ring_slot);
    }
    
    list_del(&timer->node);
    ring->slot[timer->ring_slot].count--;
    ring->count--;
    __this_timer()->count--;
    __this_timer()->removed++;
    timer->flags &= ~TM_PENDING;
}

static inline void
__tm_slot_dump(tm_ring_t *ring, int slot)
{
    struct list_head *head = __tm_slot(ring, slot);

    if (false==list_empty(head)) {
        os_println(__tab2 "ring(%u) slot(%d) count(%u)", 
            (uint32)(ring-__tm_ring0), 
            slot, 
            ring->slot[slot].count);
    }
}

static inline void
__tm_ring_dump(tm_ring_t *ring)
{
    int i;
    
    os_println(__tab "ring(%u) count(%u), current(%u)", 
        (uint32)(ring-__tm_ring0), 
        ring->count, 
        ring->current);
    
    if (ring->count) {
        for (i=0; i<TM_SLOT; i++) {
            __tm_slot_dump(ring, i);
        }
    }
}

static inline void
__tm_dump(void)
{
    if (__is_ak_debug(__ak_debug_timer|__ak_debug_trace|__ak_debug_test)) {
        tm_ring_t *ring;

        os_println("======================");
        os_println("CLOCK count(%u) ticks(%llu)", __this_timer()->count, __this_timer()->ticks);
        __tm_foreach_ring(ring) {
            __tm_ring_dump(ring);
        }
        os_println("======================");
    }
}

static inline int
__tm_ring_trigger(tm_ring_t *ring)
{
    struct list_head *head = __tm_slot(ring, ring->current);
    tm_node_t *timer, *n;
    mv_u mv = MV_INITER;
    int count = 0;
    
    list_for_each_entry_safe(timer, n, head, node) {
        __tm_remove(timer);
        
        /*
        * NOT timeout, re-insert it
        */
        if (__tm_left(timer)) {
            __tm_insert(timer);
            
            continue;
        }

        /*
        * timeout, trigger it
        */
        mv.v = (*timer->cb)(timer);
        if (0==mv2_error(mv)) {
            count++;
            
            __this_timer()->triggered_ok++;
        } else {
            __this_timer()->triggered_error++;
        }

        if (tm_is_safe(mv)) {
            __this_timer()->triggered_safe++;
        } else {
            __this_timer()->triggered_unsafe++;
            
            /*
            * after trigger, the timer is unsafe
            *   maybe the timer have destroy
            */
            continue;
        }

        if (__tm_is_cycle(timer)) {
            /*
            * reset create ticks
            */
            timer->create = __this_timer()->ticks;

            __tm_insert(timer);
        }
    }
    
    return count;
}

/* 
* return
*   success trigger nodes
*/
static inline int
__tm_trigger(tm_ring_t *ring)
{
    int count = 0;
    
    ring->current++;
    ring->current &= TM_MASK;
    
    count += __tm_ring_trigger(ring);
    
    // next ring do carry(+1)
    if (0==ring->current && ring < __tm_ringmax) { 
        count += __tm_trigger(ring+1);
    }

    return count;
}

/*
* return
*   <0: error
*  >=0: sucess tigger times
*/
static inline int
tm_trigger(uint32 times)
{
    uint32 i;
    int err, ret = 0;
    int count = 0;
    
    for (i=0; i<times; i++) {
        __this_timer()->ticks++;

        __tm_dump();
        err = __tm_trigger(__tm_ring0);
        if (err<0 && 0==ret) {
            ret = err; /* log first error */
        } else {
            count += err;
        }
    }
    
    return ret?ret:count;
}

static inline uint64
tm_ticks(void)
{
    return __this_timer()->ticks;
}

static inline void
tm_unit_set(uint32 ms) // how much ms per tick
{
    __this_timer()->unit = ms?ms:TM_MS;
}

static inline uint32 //ms, how much ms per tick
tm_unit(void)
{
    return __this_timer()->unit;
}

static inline int
tm_insert(
    tm_node_t *timer, // should embed in other struct
    int after, // ticks, after now
    tm_callback_f *cb,
    bool once
)
{
    if (NULL==timer) {
        return error_assertV(-EKEYNULL, "timer is nil");
    }
    else if (NULL==cb) {
        return error_assertV(-EINVAL1, "cb is nil");
    }
    else if (after <= 0) {
        return error_assertV(-EINVAL2, "after(%d) <= 0", after);
    }
    else if (__tm_is_pending(timer)) {
        debug_error("cannot insert pending timer");
        
        return -EINLIST; /* NO assert */
    } 
    
    timer->create   = __this_timer()->ticks;
    timer->expires  = (uint32)after;
    timer->flags    = once?TM_ONCE:0;
    timer->cb       = cb;

    __tm_insert(timer);
    
    return 0;
}

static inline int
tm_remove(tm_node_t *timer)
{
    if (NULL==timer) {
        return error_assertV(-EKEYNULL, "timer is nil");
    }
    else if (__tm_is_pending(timer)) {
        __tm_remove(timer);
        
        os_objzero(timer);

        return 0;
    }
    else {
        return -ENOEXIST;
    }
}

/*
* call it in callback
*/
static inline int
tm_once(tm_node_t *timer)
{
    if (NULL==timer) {
        return error_assertV(-EKEYNULL, "timer is nil");
    }
    else {
        os_setflag(timer->flags, TM_ONCE);

        return 0;
    }
}

static inline int
tm_change(tm_node_t *timer, uint32 after)
{
    if (NULL==timer) {
        return error_assertV(-EKEYNULL, "timer is nil");
    }
    else if (__tm_is_pending(timer)) {
        uint32 left = __tm_left(timer);

        __tm_remove(timer);

        if (after > left) {
            timer->expires += after - left;
        } else {
            timer->expires -= left - after;
        }
        
        __tm_insert(timer);

        return 0;
    } 
    else {
        return -ENOEXIST;
    }
}

/*
* call it in callback
*/
static inline int
tm_change_expires(tm_node_t *timer, uint32 expires)
{
    if (NULL==timer) {
        return error_assertV(-EKEYNULL, "timer is nil");
    }
    else if (__tm_is_cycle(timer)) {
        timer->expires = expires;

        return 0;
    } 
    else {
        return -ENOSUPPORT;
    }
}

/*
* return
*   >0: left time
*   =0: NOT pending
*   <0: error
*/
static inline int //ticks
tm_left(tm_node_t *timer)
{
    if (NULL==timer) {
        return error_assertV(-EKEYNULL, "timer is nil");
    }
    else if (__tm_is_pending(timer)) {
        return __tm_left(timer);
    }
    else {
        return 0;
    }
}

/*
* return
*   >0: expires(tm_insert's after)
*   =0: NOT pending
*   <0: error
*/
static inline int //ticks
tm_expires(tm_node_t *timer)
{
    if (NULL==timer) {
        return error_assertV(-EKEYNULL, "timer is nil");
    }
    else if (__tm_is_pending(timer)) {
        return timer->expires;
    } 
    else {
        return 0;
    }
}

static inline bool
tm_is_pending(tm_node_t *timer)
{
    if (NULL==timer) {
        return error_assertV(false, "timer is nil");
    } else {
        return __tm_is_pending(timer);
    }
}

static inline bool
tm_is_once(tm_node_t *timer)
{
    if (NULL==timer) {
        return error_assertV(false, "timer is nil");
    } else {
        return __tm_is_once(timer);
    }
}
#define tm_is_cycle(_timer)     (false==tm_is_once(_timer))

static inline void
tm_init(void)
{
    tm_ring_t *ring;
    int slot;

    if (false==__this_timer()->init) {
        __this_timer()->init = true;
        
        __tm_foreach_ring(ring) {
            __tm_foreach_slot(slot) {
                INIT_LIST_HEAD(__tm_slot(ring, slot));
            }
        }
    }
}

static inline void
tm_fini(void)
{
    tm_node_t *timer, *n;
    tm_ring_t *ring;
    int slot;

    if (true==__this_timer()->init) {
        __this_timer()->init = false;
        
        __tm_foreach_ring(ring) {
            __tm_foreach_slot(slot) {
                struct list_head *head = __tm_slot(ring, slot);
                
                list_for_each_entry_safe(timer, n, head, node) {
                    __tm_remove(timer);
                }
            }
        }
    }
}

static inline int
tm_fd(uint32 sec, uint32 nsec)
{
    struct itimerspec new = OS_ITIMESPEC_INITER(sec, nsec);
    int fd = INVALID_FD;
    
    fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
    if (fd<0) {
        return -errno;
    }

    timerfd_settime(fd, 0, &new, NULL);
    
    return fd;
}

static inline uint32
tm_fd_read(int fd)
{
    uint64 timeout = 0;
    int err = read(fd, &timeout, sizeof(timeout));
    uint32 times = (err<0)?1:(uint32)timeout;

    debug_timeout("read timerfd %d times", times);
    os_println("__ak_debug_timeout=0x%x", __ak_debug_timeout);
    
    return times;
}

static inline int
tm_fd_trigger(int fd)
{    
    return tm_trigger(tm_fd_read(fd));
}

static int 
setup_timer(int sec, int usec)
{
    struct itimerval itimer = OS_ITIMEVAL_INITER(sec, usec);
    
    int err = setitimer(ITIMER_REAL, &itimer, NULL);
    if (err<0) {
        debug_error("init timer error:%d", -errno);
        
        return -errno;
    }
    
    return 0;
}


#endif
/******************************************************************************/
#endif /* __TIMER_H_b00517b21c0f48b49c6d2c7b50c5ddb8__ */
