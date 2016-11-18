/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
static int
__ch_align(channel_t *ch, uint32 number)
{
    return number % ch->limit;
}
#define __ch_data(_ch, _type)   ((_type *)((_ch)->buf))

#define __ch_GET(_ch, _type, _idx, _obj)        do{ \
    *(_type *)_obj = __ch_data(_ch, _type)[_idx];   \
    __ch_data(_ch, _type)[_idx] = (_type)0;         \
}while(0)

#define __ch_SET(_ch, _type, _idx, _obj)        do{ \
    __ch_data(_ch, _type)[_idx] = *(_type *)_obj;   \
}while(0)

static byte *
__ch_buffer(channel_t *ch, uint32 idx)
{
    return ch->buf + ch->size * idx;
}

static bool
__ch_count(channel_t *ch)
{
    return __ch_align(ch, ch->limit + ch->writer - ch->reader);
}

static bool
__ch_left(channel_t *ch)
{
    return ch->limit - __ch_count(ch);
}

static bool
__ch_is_empty(channel_t *ch)
{
    return 0==__ch_count(ch);
}

static bool
__ch_is_full(channel_t *ch)
{
    return __ch_left(ch) <= 1;
}

static bool
__ch_is_writeable(channel_t *ch, uint32 idx)
{
    uint32 reader = ch->reader;
    uint32 writer = ch->writer;
    
    idx = __ch_align(ch, idx);

    if (reader==writer) {
        /*
        * ch is empty
        */
        return true;
    }
    else if (reader < writer) {
        return idx >= writer || idx < reader;
    }
    else { /* reader > writer */
        return idx >= writer && idx < reader;
    }
}

static bool
__ch_is_readable(channel_t *ch, uint32 idx)
{
    return false==__ch_is_writeable(ch, idx);
}

static int
__ch_SIZE(uint32 type, uint32 size)
{
    switch(type) {
        case CHANNEL_OBJECT:
            if (size <= 0) {
                return os_assertV(0);
            }
            
            break;
        case CHANNEL_POINTER:
            size = sizeof(void *);
            
            break;
        case CHANNEL_U8:
            size = sizeof(byte);
            
            break;
        case CHANNEL_U16:
            size = sizeof(uint16);
            
            break;
        case CHANNEL_U32:
            size = sizeof(uint32);
            
            break;
        case CHANNEL_U64:
            size = sizeof(uint64);
            
            break;
        default:
            return os_assertV(0);
    }

    return size;
}

static channel_t *
__ch_new(uint32 type, uint32 limit, uint32 size)
{
    if (0==size) {
        return os_assertV(NULL);
    }

    channel_t *ch = (channel_t *)os_zalloc(sizeof(*ch) + limit * size);
    if (ch) {
        ch->size    = size;
        ch->limit   = limit;
        ch->type    = type;
    }
    
    return ch;
}

static int
__ch_get(channel_t *ch, uint32 idx, void *obj)
{
    idx = __ch_align(ch, idx);
    
    switch(ch->type) {
        case CHANNEL_OBJECT: {
            byte *buffer = __ch_buffer(ch, idx);
            
            os_memcpy(obj, buffer, ch->size);
            os_memzero(buffer, ch->size);
            
        }   break;
        case CHANNEL_POINTER:
            __ch_GET(ch, void*, idx, obj);

            trace_assert(NULL != *(void **)obj, "*obj is nil");
            break;
        case CHANNEL_U8:
            __ch_GET(ch, byte, idx, obj);
            
            break;
        case CHANNEL_U16:
            __ch_GET(ch, uint16, idx, obj);
            
            break;
        case CHANNEL_U32:
            __ch_GET(ch, uint32, idx, obj);
            
            break;
        case CHANNEL_U64:
            __ch_GET(ch, uint64, idx, obj);
            
            break;
        default:
            return -EINVAL9;
    }
    
    return 0;
}

static int
__ch_set(channel_t *ch, uint32 idx, void *obj)
{
    idx = __ch_align(ch, idx);
    
    switch(ch->type) {
        case CHANNEL_OBJECT:
            os_memcpy(__ch_buffer(ch, idx), obj, ch->size);
            
            break;
        case CHANNEL_POINTER:
            __ch_SET(ch, void*, idx, obj);
            
            break;
        case CHANNEL_U8:
            __ch_SET(ch, byte, idx, obj);
            
            break;
        case CHANNEL_U16:
            __ch_SET(ch, uint16, idx, obj);
            
            break;
        case CHANNEL_U32:
            __ch_SET(ch, uint32, idx, obj);
            
            break;
        case CHANNEL_U64:
            __ch_SET(ch, uint64, idx, obj);
            
            break;
        default:
            return -EINVAL0;
    }
    
    return 0;
}

static int
__ch_read(channel_t *ch, void *obj)
{
    uint32 reader = ch->reader;
    int err = 0;

    if (false==__ch_is_readable(ch, reader)) {
        return -EEMPTY;
    }

    err = __ch_get(ch, reader, obj);
    if (err<0) {
        return err;
    }
    
    ch->reader = __ch_align(ch, reader+1);
    
    return 0;
}

static int
__ch_write(channel_t *ch, void *obj)
{
    uint32 writer = ch->writer;
    int err = 0;

    if (false==__ch_is_writeable(ch, writer)) {
        return -EFULL;
    }

    err = __ch_set(ch, writer, obj);
    if (err<0) {
        return err;
    }
    
    ch->writer = __ch_align(ch, writer+1);
    
    return 0;
}
/******************************************************************************/
bool
os_ch_left(channel_t *ch)
{
    if (NULL==ch) {
        return os_assertV(0);
    } else {
        return __ch_left(ch);
    }
}

bool
os_ch_count(channel_t *ch)
{
    if (NULL==ch) {
        return os_assertV(0);
    } else {
        return __ch_count(ch);
    }
}

bool
os_ch_is_empty(channel_t *ch)
{
    if (NULL==ch) {
        /*
        * NULL as full
        */
        return os_assertV(false);
    } else {
        return __ch_is_empty(ch);
    }
}

bool
os_ch_is_full(channel_t *ch)
{
    if (NULL==ch) {
        /*
        * NULL as full
        */
        return os_assertV(true);
    } else {
        return __ch_is_full(ch);
    }
}

bool
os_ch_is_writeable(channel_t *ch, uint32 idx)
{
    if (NULL==ch) {
        /*
        * NULL as full
        */
        return os_assertV(false);
    } else {
        return __ch_is_writeable(ch, idx);
    }
}

bool
os_ch_is_readable(channel_t *ch, uint32 idx)
{
    if (NULL==ch) {
        /*
        * NULL as full
        */
        return os_assertV(true);
    } else {
        return __ch_is_readable(ch, idx);
    }
}

channel_t *
os_ch_new(uint32 type, uint32 limit, uint32 size)
{
    return __ch_new(type, os_power_align(limit), __ch_SIZE(type, size));
}

int
os_ch_get(channel_t *ch, uint32 idx, void *obj)
{
    if (NULL==ch) {
        return os_assertV(-EKEYBAD);
    } else if (NULL==obj) {
        return os_assertV(-EINVAL1);
    } else {
        return __ch_get(ch, idx, obj);
    }
}

int
os_ch_set(channel_t *ch, uint32 idx, void *obj)
{
    if (NULL==ch) {
        return os_assertV(-EKEYBAD);
    } else if (NULL==obj) {
        return os_assertV(-EINVAL1);
    } else {
        return __ch_set(ch, idx, obj);
    }
}

int
os_ch_read(channel_t *ch, void *obj)
{
    if (NULL==ch) {
        return os_assertV(-EKEYBAD);
    } else if (NULL==obj) {
        return os_assertV(-EINVAL1);
    } else {
        return __ch_read(ch, obj);
    }
}

int
os_ch_write(channel_t *ch, void *obj)
{
    if (NULL==ch) {
        return os_assertV(-EKEYBAD);
    } else if (NULL==obj) {
        return os_assertV(-EINVAL1);
    } else {
        return __ch_write(ch, obj);
    }
}
/******************************************************************************/
