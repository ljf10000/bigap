/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
DECLARE bool 
slice_is_clean(const slice_t *slice)
{
    return  0==slice_len(slice)     &&
            0==slice_size(slice)    &&
            0==slice_resv(slice)    &&
            NULL==slice_head(slice);
}

DECLARE int 
slice_reinit(slice_t *slice, uint32 size, uint32 resv, bool local)
{
    if (0==size) {
        debug_error("slice size==0");
        
        return -EINVAL1;
    }
    else if (false==slice_is_clean(slice) && resv >= size) {
        debug_error("resv=%d, size=%d", resv, size);
        
        return -EINVAL2;
    }

    slice_flag(slice)   = local?SLICE_STACK:0;
    slice_len(slice)    = 0;
    slice_size(slice)   = size - resv;
    slice_resv(slice)   = resv;
    slice_offset(slice) = 0;
    
    return 0;
}

DECLARE int
slice_alloc(slice_t *slice, uint32 size)
{
    void *buf = NULL;

    buf = os_zalloc(size);
    if (NULL==buf) {        
        return -ENOMEM;
    }

    slice_init(slice, buf, size, false);

    return 0;
}

DECLARE slice_t *
slice_clone(slice_t *dst, const slice_t *src)
{
    void *buf;
    
    buf = os_zalloc(slice_SIZE(src));
    if (NULL==buf) {
        return NULL;
    }
    
    os_memcpy(buf, slice_head(src), slice_resv(src) + slice_len(src));
    
    os_objdcpy(dst, src);
    slice_flag(dst) = (slice_flag(src) & ~SLICE_STACK);
    slice_head(dst) = buf;

    return dst;
}

DECLARE int
slice_grow(slice_t *slice, uint32 grow)
{
    void *buf;
    int size = slice_SIZE(slice);

    if (slice_in_stack(slice)) {
        debug_error("slice is in statck, can not grow");
        
        return os_assertV(-ENOSUPPORT);
    }

    if (size < SLICE_GROW_DOUBLE_LIMIT) {
        grow = OS_MAX(size, grow);
    }
    grow = OS_MAX(SLICE_GROW_STEP, grow);
    
    buf = os_realloc(slice_head(slice), size + grow);
    if (NULL==buf) {
        return -ENOMEM;
    }

    slice_head(slice) = buf;
    slice_size(slice) += grow;

    debug_trace("slice_grow: size %d, grow %d", slice_size(slice) - grow, grow);
    
    return 0;
}

DECLARE byte *
slice_pull(slice_t *slice, uint32 len)
{
    trace_assert(NULL!=slice, "slice is nil");

    if (slice_size(slice) < len) {
        return os_assertV(NULL);
    }
    
    if (0==slice_len(slice)) {
        /*
        * slice 刚初始化完毕，还没有填充数据，
        *
        * 可以 pull，无需调整 slice_len
        */
    }
    else if (slice_len(slice) < len) {
        /*
        * slice 已经使用并填充了数据
        *   但 (填充的数据长度) < (本次pull的长度)
        *
        * pull 失败
        */
        return NULL;
    } 
    else {
        /*
        * slice 已经使用并填充了数据
        *   但 (填充的数据长度) >= (本次pull的长度)
        *
        * 可以 pull，需要调整 slice_len
        */
        slice_len(slice) -= len;
    } 
    
    slice_size(slice) -= len;
    slice_resv(slice) += len;
    
    return slice_data(slice);
}

DECLARE byte *
slice_push(slice_t *slice, uint32 len)
{
    trace_assert(NULL!=slice,  "slice is nil");

    if (slice_resv(slice) < len) {
        return os_assertV(NULL);
    }
    
    slice_len(slice)    += len;
    slice_size(slice)   += len;
    slice_resv(slice)   -= len;

    return slice_data(slice);
}

DECLARE byte *
slice_put(slice_t *slice, uint32 len)
{
    trace_assert(NULL!=slice, "slice is nil");
    
    if (len > slice_remain(slice)) {
        return os_assertV(NULL);
    }
    
    slice_len(slice) += len;
    
    return slice_tail(slice);
}

DECLARE byte *
slice_trim(slice_t *slice, uint32 len)
{
    trace_assert(NULL!=slice, "slice is nil");
    
    if (len > slice_len(slice)) {
        return os_assertV(NULL);
    }
    
    slice_len(slice) -= len;
    
    return slice_tail(slice);
}

DECLARE byte *
slice_put_char(slice_t *slice, int ch)
{
    byte *new;
    
    trace_assert(NULL!=slice, "slice is nil");

    new  = slice_put(slice, 1);
    if (new) {
        new[0] = (ch & 0xff);
    }
    
    return new;
}

DECLARE byte *
slice_put_buf(slice_t *slice, void *buf, uint32 len)
{
    trace_assert(NULL!=slice, "slice is nil");

    if (NULL==buf) {
        return os_assertV(NULL);
    }
    
    os_memcpy(slice_tail(slice), buf, len);
    
    return slice_put(slice, len);
}

DECLARE int
slice_vsprintf(slice_t *slice, bool grow, char *fmt, va_list args)
{
    int len = 0, space;

    if (NULL==slice) {
        return os_vsprintf_size(fmt, args);
    }

try_again:
    space = slice_remain(slice);
    debug_trace("slice_vsprintf: remain %d", space);

    /*
    * 理论上 slice_remain 应该大于等于 0
    *
    * 将 space 重置为 1(便于后续减一操作)
    *
    * 所以, 当剩余空间为1或0时，
    *   vsnprintf 不会有任何写入操作
    */
    space = (space>0)?space:1;

    len = os_vsnprintf((char *)slice_tail(slice), space, fmt, args);
    debug_trace("slice_vsprintf: needed %d", len);

    if (os_snprintf_is_full(space, len)) { /* no space */
        debug_trace("slice_vsprintf: full");
        if (grow && 0==slice_grow(slice, len + 1 - space)) {
            debug_trace("slice_vsprintf: grow and try");

            goto try_again;
        } else {
            /* do nothing */
        }
    } else {
        slice_put(slice, len);
    }

    return len;
}

DECLARE int
slice_sprintf(slice_t *slice, bool grow, char *fmt, ...)
{
    va_list args;
    
    va_start(args, fmt);
    int len = slice_vsprintf(slice, grow, fmt, args);
    va_end(args);
    
    return len;
}

#ifdef __APP__

DECLARE void 
__slice_to_msg
(
    slice_t    *slice, 
    bool            is_send,
    struct iovec    *iov,
    struct msghdr   *msg,
    sockaddr_t      *remote
)
{
    iov->iov_base   = slice_data(slice);
 	iov->iov_len    = (true == is_send)?slice_len(slice):slice_size(slice);
 	msg->msg_name   = remote;
 	msg->msg_namelen= sizeof(*remote);
 	msg->msg_iov    = iov;
 	msg->msg_iovlen = 1;
}

DECLARE int
slice_send(int fd, slice_t *slice, sockaddr_t *remote, int flag)
{
    struct iovec    iov = {0};
    struct msghdr   msg = {0};

    __slice_to_msg(slice, true, &iov, &msg, remote);
    
    return sendmsg(fd, &msg, flag);
}

DECLARE int
slice_recv(int fd, slice_t *slice, sockaddr_t *remote, int flag)
{
    struct iovec    iov = {0};
    struct msghdr   msg = {0};
    
    __slice_to_msg(slice, false, &iov, &msg, remote);
    
    return recvmsg(fd, &msg, flag);
}
#endif
/******************************************************************************/
