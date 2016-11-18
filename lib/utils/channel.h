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
    uint32 size;
    uint32 limit;
    uint32 type;
    uint32 reader;
    uint32 writer;

    byte buf[0];
} channel_t;

extern bool
os_ch_left(channel_t *ch);

extern bool
os_ch_count(channel_t *ch);

extern bool
os_ch_is_empty(channel_t *ch);

extern bool
os_ch_is_full(channel_t *ch);

extern bool
os_ch_is_writeable(channel_t *ch, uint32 idx);

extern bool
os_ch_is_readable(channel_t *ch, uint32 idx);

#define os_ch_free(_ch)     os_free(_ch)

extern channel_t *
os_ch_new(uint32 type, uint32 limit, uint32 size);

extern int
os_ch_get(channel_t *ch, uint32 idx, void *obj);

extern int
os_ch_set(channel_t *ch, uint32 idx, void *obj);

extern int
os_ch_read(channel_t *ch, void *obj);

extern int
os_ch_write(channel_t *ch, void *obj);
/******************************************************************************/
#define os_och_new(_limit, _size)       os_ch_new(CHANNEL_OBJECT, _limit, _size)
#define os_och_get(_ch, _idx, _obj)     os_ch_get(_ch, _idx, _obj)
#define os_och_set(_ch, _idx, _obj)     os_ch_set(_ch, _idx, _obj)
#define os_och_read(_ch, _obj)          os_ch_read(_ch, _obj)
#define os_och_write(_ch, _obj)         os_ch_write(_ch, _obj)
/******************************************************************************/
#define os_8ch_new(_limit)          os_ch_new(CHANNEL_U8, _limit, 0)

static inline int
os_8ch_get(channel_t *ch, uint32 idx, byte *pv)
{
    return os_ch_get(ch, idx, pv);
}

static inline int
os_8ch_set(channel_t *ch, uint32 idx, byte v)
{
    return os_ch_set(ch, idx, &v);
}

static inline int
os_8ch_read(channel_t *ch, byte *pv)
{
    return os_ch_read(ch, pv);
}

static inline int
os_8ch_write(channel_t *ch, byte v)
{
    return os_ch_write(ch, &v);
}
/******************************************************************************/
#define os_16ch_new(_limit)         os_ch_new(CHANNEL_U16, _limit, 0)

static inline int
os_16ch_get(channel_t *ch, uint32 idx, uint16 *pv)
{
    return os_ch_get(ch, idx, pv);
}

static inline int
os_16ch_set(channel_t *ch, uint32 idx, uint16 v)
{
    return os_ch_set(ch, idx, &v);
}

static inline int
os_16ch_read(channel_t *ch, uint16 *pv)
{
    return os_ch_read(ch, pv);
}

static inline int
os_16ch_write(channel_t *ch, uint16 v)
{
    return os_ch_write(ch, &v);
}
/******************************************************************************/
#define os_32ch_new(_limit)         os_ch_new(CHANNEL_U32, _limit, 0)

static inline int
os_32ch_get(channel_t *ch, uint32 idx, uint32 *pv)
{
    return os_ch_get(ch, idx, pv);
}

static inline int
os_32ch_set(channel_t *ch, uint32 idx, uint32 v)
{
    return os_ch_set(ch, idx, &v);
}

static inline int
os_32ch_read(channel_t *ch, uint32 *pv)
{
    return os_ch_read(ch, pv);
}

static inline int
os_32ch_write(channel_t *ch, uint32 v)
{
    return os_ch_write(ch, &v);
}
/******************************************************************************/
#define os_64ch_new(_limit)         os_ch_new(CHANNEL_U64, _limit, 0)

static inline int
os_64ch_get(channel_t *ch, uint32 idx, uint64 *pv)
{
    return os_ch_get(ch, idx, pv);
}

static inline int
os_64ch_set(channel_t *ch, uint32 idx, uint64 v)
{
    return os_ch_set(ch, idx, &v);
}

static inline int
os_64ch_read(channel_t *ch, uint64 *pv)
{
    return os_ch_read(ch, pv);
}

static inline int
os_64ch_write(channel_t *ch, uint64 v)
{
    return os_ch_write(ch, &v);
}
/******************************************************************************/
#define os_pch_new(_limit)          os_ch_new(CHANNEL_POINTER, _limit, 0)

static inline int
os_pch_get(channel_t *ch, uint32 idx, void **pointer)
{
    return os_ch_get(ch, idx, (void *)pointer);
}

static inline int
os_pch_set(channel_t *ch, uint32 idx, void *pointer)
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
