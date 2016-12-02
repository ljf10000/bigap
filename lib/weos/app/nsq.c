/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
DECLARE_ENUMER(nsq_fsm, NSQ_FSM_ENUM_MAPPER, NSQ_FSM_END);
DECLARE_ENUMER(nsq_auth, NSQ_AUTH_ENUM_MAPPER, NSQ_AUTH_END);
DECLARE_ENUMER(nsq_error, NSQ_ERROR_ENUM_MAPPER, NSQ_E_END);
DECLARE_ENUMER(nsq_frame, NSQ_FRAME_ENUM_MAPPER, NSQ_FRAME_END);


bool
is_good_nsq_name_char(int ch)
{
    return  '.'==ch || '_'==ch ||
            (ch>='a' && ch<='z') ||
            (ch>='A' && ch<='Z') ||
            (ch>='0' && ch<='9');
}

bool
is_good_nsq_name(char *name)
{
    char *p = name;
    int len = 0;

    while(*p) {
        len++;
        
        if (false==is_good_nsq_name_char(*p)) {
            return false;
        }
        else if (len>NSQ_NAME_SIZE) {
            return false;
        }
    }

    return true;
}

int
nsqb_data(nsq_buffer_t *b, char *data, uint32 len)
{
    uint32 len_n = htonl(len);
    int err = 0;

    err = nsqb_number(b, len_n);
    if (err<0) {
        return err;
    }
    
    err = nsqb_buffer(b, data, len);
    if (err<0) {
        return err;
    }
    
    return 0;
}

int
nsqb_MAGIC(nsq_buffer_t *b)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_command(b, NSQ_INSTRUCTION_MAGIC);
    if (err<0) {
        return err;
    }

    return 0;
}

int
nsqb_CLS(nsq_buffer_t *b)
{
    int err;
    
    nsqb_clean(b);
    
    err = nsqb_command(b, NSQ_INSTRUCTION_CLS);
    if (err<0) {
        return err;
    }

    return 0;
}

int
nsqb_IDENTIFY(nsq_buffer_t *b, char *json)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_command(b, NSQ_INSTRUCTION_IDENTIFY);
    if (err<0) {
        return err;
    }

    err = nsqb_data(b, json, os_strlen(json));
    if (err<0) {
        return err;
    }

    return 0;
}

int
nsqb_SUB(nsq_buffer_t *b, char *topic, char *channel)
{
    int err;
    
    nsqb_clean(b);
    
    err = nsqb_sprintf(b, NSQ_INSTRUCTION_SUB "%s %s\n", topic, channel);
    if (err<0) {
        return err;
    }

    return 0;
}

int
nsqb_PUB(nsq_buffer_t *b, char *topic, char *data, uint32 len)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_sprintf(b, NSQ_INSTRUCTION_PUB "%s\n", topic);
    if (err<0) {
        return err;
    }

    err = nsqb_data(b, data, len);
    if (err<0) {
        return err;
    }

    return 0;
}

int
nsqb_MPUB(nsq_buffer_t *b, char *topic, struct iovec *iov, uint32 count)
{
    uint32 count_n = htonl(count);
    int i, err;
    
    nsqb_clean(b);

    err = nsqb_sprintf(b, NSQ_INSTRUCTION_MPUB "%s\n", topic);
    if (err<0) {
        return err;
    }

    err = nsqb_number(b, count_n);
    if (err<0) {
        return err;
    }

    for (i=0; i<count; i++) {
        err = nsqb_data(b, iov[i].iov_base, iov[i].iov_len);
        if (err<0) {
            return err;
        }
    }
    
    return 0;
}

int
nsqb_RDY(nsq_buffer_t *b, uint32 count)
{
    int err;
    
    nsqb_clean(b);
    
    err = nsqb_sprintf(b, NSQ_INSTRUCTION_RDY "%u\n", count);
    if (err<0) {
        return err;
    }

    return 0;
}

int
nsqb_FIN(nsq_buffer_t *b, byte *msg_id)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_command(b, NSQ_INSTRUCTION_FIN);
    if (err<0) {
        return err;
    }
    
    err = nsqb_buffer(b, msg_id, NSQ_MSGID_SIZE);
    if (err<0) {
        return err;
    }
    
    err = nsqb_buffer(b, "\n", 1);
    if (err<0) {
        return err;
    }

    return 0;
}

int
nsqb_REQ(nsq_buffer_t *b, byte *msg_id, int timeout)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_command(b, NSQ_INSTRUCTION_REQ);
    if (err<0) {
        return err;
    }
    
    err = nsqb_buffer(b, msg_id, NSQ_MSGID_SIZE);
    if (err<0) {
        return err;
    }

    err = nsqb_sprintf(b, "%d\n", timeout);
    if (err<0) {
        return err;
    }
    
    return 0;
}

int
nsqb_TOUCH(nsq_buffer_t *b, char *msg_id)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_command(b, NSQ_INSTRUCTION_REQ);
    if (err<0) {
        return err;
    }
    
    err = nsqb_buffer(b, msg_id, NSQ_MSGID_SIZE);
    if (err<0) {
        return err;
    }

    err = nsqb_buffer(b, "\n", 1);
    if (err<0) {
        return err;
    }
    
    return 0;
}

int
nsqb_NOP(nsq_buffer_t *b)
{
    int err;
    
    nsqb_clean(b);
    
    err = nsqb_command(b, NSQ_INSTRUCTION_NOP);
    if (err<0) {
        return err;
    }

    return 0;
}

int
nsqb_AUTH(nsq_buffer_t *b, char *secret, uint32 len)
{
    int err;
    
    nsqb_clean(b);

    err = nsqb_command(b, NSQ_INSTRUCTION_AUTH);
    if (err<0) {
        return err;
    }

    err = nsqb_data(b, secret, len);
    if (err<0) {
        return err;
    }

    return 0;
}

int
nsqb_recv(int fd, nsq_buffer_t *b)
{
    nsq_msg_t *msg;
    int err, len;
    uint32 size;
    
    nsqb_clean(b);

    /*
    * read msg header
    */
    size = sizeof(nsq_msg_t);
    err = len = __io_recv(fd, b->buf, size, 0);
    if (err<0) {
        goto error;
    }
    else if (len<size) {
        err = -ETOOSMALL; goto error;
    }
    
    /*
    * check msg size
    */
    msg = (nsq_msg_t *)b->buf;
    nsq_msg_ntoh(msg);
    if (nsq_msg_total_size(msg) < size) {
        err = -ETOOSMALL; goto error;
    }
    b->len += len;

    if (0==nsq_msg_body_size(msg)) {
        err = 0; goto error;
    }
    
    /*
    * read msg body
    */
    size = nsq_msg_body_size(msg);
    err = len = __io_recv(fd, b->buf + b->len, size, 0);
    if (err<0) {
        goto error;
    }
    if (len<size) {
        err = -ETOOSMALL; goto error;
    }
    b->len += len;
    b->buf[b->len] = 0;
    if (is_nsq_frame_message(msg->type)) {
        nsq_body_ntoh(msg);
    }

    err = 0;
error:
    nsq_msg_dump(msg, debug_proto);
    
    return err;
}
/******************************************************************************/
