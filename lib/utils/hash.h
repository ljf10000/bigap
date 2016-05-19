#ifndef __HASH_H_699cf25513f14e17a40808355124348c__
#define __HASH_H_699cf25513f14e17a40808355124348c__
/******************************************************************************/
#include "utils/list.h"
/******************************************************************************/
#if 1
struct hash_s;

typedef uint32 hash_idx_t;
#define INVALID_HASH_IDX   0xffffffff

typedef struct {
    struct list_head  head;

    struct hash_s *hash;
} hash_bucket_t;

typedef struct {
    struct list_head node;

    hash_bucket_t *bucket;
} hash_node_t;

typedef struct hash_s {
    hash_bucket_t *bucket;
    hash_idx_t size;
    uint32 count;
} hash_t;

#define HASH_INITER(_size) {    \
    .size = _size,              \
}

#define hash_entry(_node, _type, _member)      container_of(_node, _type, _member)

typedef hash_idx_t hash_node_calc_f(hash_node_t *node);
typedef hash_idx_t hash_data_calc_f(void);
typedef bool hash_eq_f(hash_node_t *node);
typedef int hash_cmp_f(hash_node_t *node);
typedef void hash_change_f(hash_node_t *node);
#endif
/******************************************************************************/
#if 2
static inline bool
__in_hash(hash_node_t *node)
{
    return NULL!=node->bucket;
}

static inline bool
__is_good_hash_idx(hash_t *h, hash_idx_t idx)
{
    return IS_GOOD_ENUM(idx, h->size);
}

static inline hash_bucket_t *
__hash_bucket(hash_t *h, hash_idx_t idx)
{
    if (__is_good_hash_idx(h, idx)) {
        return &h->bucket[idx];
    } else {
        return NULL;
    }
}

static inline bool
__is_hash_empty(hash_t *h)
{
    return 0==h->count;
}

static inline int
__hash_init(hash_t *h, hash_idx_t size)
{
    hash_idx_t i;
    
    h->size = size;
    h->bucket = (hash_bucket_t *)os_malloc(size * sizeof(hash_bucket_t));
    if (NULL==h->bucket) {
        return -ENOMEM;
    }

    for (i=0; i<size; i++) {
        hash_bucket_t *bucket = __hash_bucket(h, i);
        
        INIT_LIST_HEAD(&bucket->head);
        bucket->hash = h;
    }
    
    return 0;
}

static inline void
__hash_del(hash_node_t *node)
{
    if (__in_hash(node) && false==__is_hash_empty(node->bucket->hash)) {
        list_del(&node->node);
        node->bucket->hash->count--;
        node->bucket = NULL;
    }
}

static inline void
__hash_add(hash_t *h, hash_node_t *node, hash_node_calc_f *nhash)
{
    if (false==__in_hash(node)) {
        hash_bucket_t *bucket = __hash_bucket(h, (*nhash)(node));
        if (bucket) {
            list_add(&node->node, &bucket->head);
            node->bucket = bucket;
            h->count++;
        }
    }
}

static inline void
__hash_change(hash_t *h, hash_node_t *node, hash_change_f *change, hash_node_calc_f *nhash)
{
    __hash_del(node);
    (*change)(node);
    __hash_add(h, node, nhash);
}
#endif
/******************************************************************************/
#if 3
static inline int
hash_init(hash_t *h)
{
    return __hash_init(h, h->size);
}

static inline bool
in_hash(hash_t *h, hash_node_t *node)
{
    return h && node && __in_hash(node) && h==node->bucket->hash;
}

static inline bool
is_hash_empty(hash_t *h)
{
    return NULL==h || __is_hash_empty(h);
}

static inline int
hash_del(hash_t *h, hash_node_t *node)
{
    if (in_hash(h, node)) {
        __hash_del(node);

        return 0;
    } else {
        return -ENOINLIST;
    }
}

static inline int
hash_add(hash_t *h, hash_node_t *node, hash_node_calc_f *nhash)
{
    if (h && node && nhash) {
        __hash_del(node);
        __hash_add(h, node, nhash);

        return 0;
    } else {
        return -EKEYNULL;
    }
}

static inline int
hash_change(hash_t *h, hash_node_t *node, hash_change_f *change, hash_node_calc_f *nhash)
{
    if (in_hash(h, node) && change && nhash) {
        __hash_change(h, node, change, nhash);

        return 0;
    } else {
        return -EKEYNULL;
    }
}

#define hash_bucket_foreach(_bucket, _node) \
    list_for_each_entry(_node, &(_bucket)->head, node)

#define hash_bucket_foreach_safe(_bucket, _node, _tmp) \
    list_for_each_entry_safe(_node, _tmp, &(_bucket)->head, node)

#define hash_bucket_foreach_entry(_bucket, _node, _member) \
    list_for_each_entry(_node, &(_bucket)->head, _member.node)

#define hash_bucket_foreach_entry_safe(_bucket, _node, _tmp, _member) \
    list_for_each_entry_safe(_node, _tmp, &(_bucket)->head, _member.node)

#define __hash_foreach_bucket(_h, _hidx, _bucket) \
    for (_hidx = 0, _bucket = __hash_bucket(_h, 0); \
        _hidx<(_h)->count; \
        _hidx++, _bucket = __hash_bucket(_h, _hidx)) \
    /* end */

#define hash_foreach(_h, _hidx, _bucket, _node) \
    __hash_foreach_bucket(_h, _hidx, _bucket) \
        hash_bucket_foreach(_bucket, _node) \
    /* end */

#define hash_foreach_safe(_h, _hidx, _bucket, _node, _tmp) \
    __hash_foreach_bucket(_h, _hidx, _bucket) \
        hash_bucket_foreach_safe(_bucket, _node, _tmp) \
    /* end */

#define hash_foreach_entry(_h, _hidx, _bucket, _node, _member) \
    __hash_foreach_bucket(_h, _hidx, _bucket) \
        hash_bucket_foreach_entry(_bucket, _node, _member) \
    /* end */

#define hash_foreach_entry_safe(_h, _hidx, _bucket, _node, _tmp, _member) \
    __hash_foreach_bucket(_h, _hidx, _bucket) \
        hash_bucket_foreach_entry_safe(_bucket, _node, _tmp, _member) \
    /* end */

static inline hash_node_t *
hash_find(hash_t *h, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    if (h && dhash && eq) {
        hash_bucket_t *bucket = __hash_bucket(h, (*dhash)());
        hash_node_t *node;
        
        if (bucket) {            
            hash_bucket_foreach(bucket, node) {
                if ((*eq)(node)) {
                    return node;
                }
            }
        }
    }
    
    return NULL;
}

static inline hash_idx_t
hash_bybuf(byte *buf, uint32 len, hash_idx_t mask)
{
    bkdr_t bkdr = os_bin_bkdr(buf, len);
    
    return (hash_idx_t)bkdr & mask;
}

#endif
/******************************************************************************/
#endif /* __HASH_H_699cf25513f14e17a40808355124348c__ */
