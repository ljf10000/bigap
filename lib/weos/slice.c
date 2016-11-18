/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifdef __APP__

static void 
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

int
slice_send(int fd, slice_t *slice, sockaddr_t *remote, int flag)
{
    struct iovec    iov = {0};
    struct msghdr   msg = {0};

    __slice_to_msg(slice, true, &iov, &msg, remote);
    
    return sendmsg(fd, &msg, flag);
}

int
slice_recv(int fd, slice_t *slice, sockaddr_t *remote, int flag)
{
    struct iovec    iov = {0};
    struct msghdr   msg = {0};
    
    __slice_to_msg(slice, false, &iov, &msg, remote);
    
    return recvmsg(fd, &msg, flag);
}

#endif
/******************************************************************************/
