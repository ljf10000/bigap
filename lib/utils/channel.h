#ifndef __CHANNEL_H_91835dac42e940458282784927bce6fd__
#define __CHANNEL_H_91835dac42e940458282784927bce6fd__
/******************************************************************************/
enum channel_type {
    CHANNEL_OBJECT,
    CHANNEL_POINTER,
    CHANNEL_U8,
    CHANNEL_U16,
    CHANNEL_U32,
    CHANNEL_U64,

    CHANNEL_END
};

typedef struct {
    uint32_t size;
    uint32_t limit;
    uint32_t type;
    uint32_t reader;
    uint32_t writer;

    byte buf[0];
} channel_t;

static inline int
__ch_align(channel_t *ch, uint32_t number)
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

static inline byte *
__ch_buffer(channel_t *ch, uint32_t idx)
{
    return ch->buf + ch->size * idx;
}

static inline bool
__ch_count(channel_t *ch)
{
    return __ch_align(ch, ch->limit + ch->writer - ch->reader);
}

static inline bool
__ch_left(channel_t *ch)
{
    return ch->limit - __ch_count(ch);
}

static inline bool
__ch_is_empty(channel_t *ch)
{
    return 0==__ch_count(ch);
}

static inline bool
__ch_is_full(channel_t *ch)
{
    return __ch_left(ch) <= 1;
}

static inline bool
__ch_is_writeable(channel_t *ch, uint32_t idx)
{
    uint32_t reader = ch->reader;
    uint32_t writer = ch->writer;
    
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

static inline bool
__ch_is_readable(channel_t *ch, uint32_t idx)
{
    return false==__ch_is_writeable(ch, idx);
}

static inline int
__ch_SIZE(uint32_t type, uint32_t size)
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
            size = sizeof(uint16_t);
            
            break;
        case CHANNEL_U32:
            size = sizeof(uint32_t);
            
            break;
        case CHANNEL_U64:
            size = sizeof(uint64_t);
            
            break;
        default:
            return os_assertV(0);
    }

    return size;
}

static inline channel_t *
__ch_new(uint32_t type, uint32_t limit, uint32_t size)
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

static inline int
__ch_get(channel_t *ch, uint32_t idx, void *obj)
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
            __ch_GET(ch, uint8_t, idx, obj);
            
            break;
        case CHANNEL_U16:
            __ch_GET(ch, uint16_t, idx, obj);
            
            break;
        case CHANNEL_U32:
            __ch_GET(ch, uint32_t, idx, obj);
            
            break;
        case CHANNEL_U64:
            __ch_GET(ch, uint64_t, idx, obj);
            
            break;
        default:
            return -EINVAL9;
    }
    
    return 0;
}

static inline int
__ch_set(channel_t *ch, uint32_t idx, void *obj)
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
            __ch_SET(ch, uint8_t, idx, obj);
            
            break;
        case CHANNEL_U16:
            __ch_SET(ch, uint16_t, idx, obj);
            
            break;
        case CHANNEL_U32:
            __ch_SET(ch, uint32_t, idx, obj);
            
            break;
        case CHANNEL_U64:
            __ch_SET(ch, uint64_t, idx, obj);
            
            break;
        default:
            return -EINVAL0;
    }
    
    return 0;
}

static inline int
__ch_read(channel_t *ch, void *obj)
{
    uint32_t reader = ch->reader;
    int err = 0;

    if (false==__ch_is_readable(ch, reader)) {
        return -EEMPTY;
    }

    err = __ch_get(ch, reader, obj);
    if (err) {
        return err;
    }
    
    ch->reader = __ch_align(ch, reader+1);
    
    return 0;
}

static inline int
__ch_write(channel_t *ch, void *obj)
{
    uint32_t writer = ch->writer;
    int err = 0;

    if (false==__ch_is_writeable(ch, writer)) {
        return -EFULL;
    }

    err = __ch_set(ch, writer, obj);
    if (err) {
        return err;
    }
    
    ch->writer = __ch_align(ch, writer+1);
    
    return 0;
}
/******************************************************************************/
static inline bool
os_ch_left(channel_t *ch)
{
    if (NULL==ch) {
        return os_assertV(0);
    } else {
        return __ch_left(ch);
    }
}

static inline bool
os_ch_count(channel_t *ch)
{
    if (NULL==ch) {
        return os_assertV(0);
    } else {
        return __ch_count(ch);
    }
}

static inline bool
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

static inline bool
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

static inline bool
os_ch_is_writeable(channel_t *ch, uint32_t idx)
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

static inline bool
os_ch_is_readable(channel_t *ch, uint32_t idx)
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

static inline channel_t *
os_ch_new(uint32_t type, uint32_t limit, uint32_t size)
{
    return __ch_new(type, os_power_align(limit), __ch_SIZE(type, size));
}

#define os_ch_free(_ch)     os_free(_ch)

static inline int
os_ch_get(channel_t *ch, uint32_t idx, void *obj)
{
    if (NULL==ch) {
        return os_assertV(-EKEYBAD);
    } else if (NULL==obj) {
        return os_assertV(-EINVAL1);
    } else {
        return __ch_get(ch, idx, obj);
    }
}

static inline int
os_ch_set(channel_t *ch, uint32_t idx, void *obj)
{
    if (NULL==ch) {
        return os_assertV(-EKEYBAD);
    } else if (NULL==obj) {
        return os_assertV(-EINVAL1);
    } else {
        return __ch_set(ch, idx, obj);
    }
}

static inline int
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

static inline int
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
#define os_och_new(_limit, _size)       os_ch_new(CHANNEL_OBJECT, _limit, _size)
#define os_och_get(_ch, _idx, _obj)     os_ch_get(_ch, _idx, _obj)
#define os_och_set(_ch, _idx, _obj)     os_ch_set(_ch, _idx, _obj)
#define os_och_read(_ch, _obj)          os_ch_read(_ch, _obj)
#define os_och_write(_ch, _obj)         os_ch_write(_ch, _obj)
/******************************************************************************/
#define os_8ch_new(_limit)          os_ch_new(CHANNEL_U8, _limit, 0)

static inline int
os_8ch_get(channel_t *ch, uint32_t idx, uint8_t *pv)
{
    return os_ch_get(ch, idx, pv);
}

static inline int
os_8ch_set(channel_t *ch, uint32_t idx, uint8_t v)
{
    return os_ch_set(ch, idx, &v);
}

static inline int
os_8ch_read(channel_t *ch, uint8_t *pv)
{
    return os_ch_read(ch, pv);
}

static inline int
os_8ch_write(channel_t *ch, uint8_t v)
{
    return os_ch_write(ch, &v);
}
/******************************************************************************/
#define os_16ch_new(_limit)         os_ch_new(CHANNEL_U16, _limit, 0)

static inline int
os_16ch_get(channel_t *ch, uint32_t idx, uint16_t *pv)
{
    return os_ch_get(ch, idx, pv);
}

static inline int
os_16ch_set(channel_t *ch, uint32_t idx, uint16_t v)
{
    return os_ch_set(ch, idx, &v);
}

static inline int
os_16ch_read(channel_t *ch, uint16_t *pv)
{
    return os_ch_read(ch, pv);
}

static inline int
os_16ch_write(channel_t *ch, uint16_t v)
{
    return os_ch_write(ch, &v);
}
/******************************************************************************/
#define os_32ch_new(_limit)         os_ch_new(CHANNEL_U32, _limit, 0)

static inline int
os_32ch_get(channel_t *ch, uint32_t idx, uint32_t *pv)
{
    return os_ch_get(ch, idx, pv);
}

static inline int
os_32ch_set(channel_t *ch, uint32_t idx, uint32_t v)
{
    return os_ch_set(ch, idx, &v);
}

static inline int
os_32ch_read(channel_t *ch, uint32_t *pv)
{
    return os_ch_read(ch, pv);
}

static inline int
os_32ch_write(channel_t *ch, uint32_t v)
{
    return os_ch_write(ch, &v);
}
/******************************************************************************/
#define os_64ch_new(_limit)         os_ch_new(CHANNEL_U64, _limit, 0)

static inline int
os_64ch_get(channel_t *ch, uint32_t idx, uint64_t *pv)
{
    return os_ch_get(ch, idx, pv);
}

static inline int
os_64ch_set(channel_t *ch, uint32_t idx, uint64_t v)
{
    return os_ch_set(ch, idx, &v);
}

static inline int
os_64ch_read(channel_t *ch, uint64_t *pv)
{
    return os_ch_read(ch, pv);
}

static inline int
os_64ch_write(channel_t *ch, uint64_t v)
{
    return os_ch_write(ch, &v);
}
/******************************************************************************/
#define os_pch_new(_limit)          os_ch_new(CHANNEL_POINTER, _limit, 0)

static inline int
os_pch_get(channel_t *ch, uint32_t idx, void **pointer)
{
    return os_ch_get(ch, idx, (void *)pointer);
}

static inline int
os_pch_set(channel_t *ch, uint32_t idx, void *pointer)
{
    return os_ch_set(ch, idx, (void *)&pointer);
}

static inline int
os_pch_read(channel_t *ch, void **pointer)
{
    return os_ch_read(ch, (void *)pointer);
}

static inline int
os_pch_write(channel_t *ch, void *pointer)
{
    return os_ch_write(ch, (void *)&pointer);
}
/******************************************************************************/
#endif /* __CHANNEL_H_91835dac42e940458282784927bce6fd__ */
