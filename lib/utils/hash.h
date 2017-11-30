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

typedef hash_idx_t hash_node_calc_f(hash_node_t *node);
typedef hash_idx_t hash_data_calc_f(void);
typedef bool hash_eq_f(hash_node_t *node);
typedef int hash_cmp_f(hash_node_t *node);
typedef void hash_change_f(hash_node_t *node);
#endif
/******************************************************************************/
#if 2
EXTERN int
hash_init(hash_t *h, hash_idx_t size);

EXTERN bool
in_hash(hash_t *h, hash_node_t *node);

EXTERN bool
is_hash_empty(hash_t *h);

EXTERN int
hash_del(hash_t *h, hash_node_t *node);

EXTERN int
hash_add(hash_t *h, hash_node_t *node, hash_node_calc_f *ncalc);

EXTERN int
hash_change(hash_t *h, hash_node_t *node, hash_change_f *change, hash_node_calc_f *ncalc);

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

EXTERN hash_node_t *
hash_find(hash_t *h, hash_data_calc_f *dcalc, hash_eq_f *eq);

EXTERN hash_idx_t
hash_bybuf(void *buf, uint32 len, hash_idx_t mask);
#endif

#ifdef __BOOT__
#include "weos/boot/hash.c"
#endif
/******************************************************************************/
#endif /* __HASH_H_699cf25513f14e17a40808355124348c__ */
