/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
ALWAYS_INLINE bool
__in_hash(hash_node_t *node)
{
    return NULL!=node->bucket;
}

ALWAYS_INLINE bool
__is_good_hash_idx(hash_t *h, hash_idx_t idx)
{
    return IS_GOOD_ENUM(idx, h->size);
}

ALWAYS_INLINE hash_bucket_t *
__hash_bucket(hash_t *h, hash_idx_t idx)
{
    if (__is_good_hash_idx(h, idx)) {
        return &h->bucket[idx];
    } else {
        return NULL;
    }
}

ALWAYS_INLINE bool
__is_hash_empty(hash_t *h)
{
    return 0==h->count;
}

DECLARE void
__hash_del(hash_node_t *node)
{
    if (__in_hash(node) && false==__is_hash_empty(node->bucket->hash)) {
        list_del(&node->node);
        node->bucket->hash->count--;
        node->bucket = NULL;
    }
}

DECLARE void
__hash_add(hash_t *h, hash_node_t *node, hash_node_calc_f *ncalc)
{
    if (false==__in_hash(node)) {
        hash_bucket_t *bucket = __hash_bucket(h, (*ncalc)(node));
        if (bucket) {
            list_add(&node->node, &bucket->head);
            node->bucket = bucket;
            h->count++;
        }
    }
}

DECLARE void
__hash_change(hash_t *h, hash_node_t *node, hash_change_f *change, hash_node_calc_f *ncalc)
{
    __hash_del(node);
    (*change)(node);
    __hash_add(h, node, ncalc);
}

DECLARE bool
in_hash(hash_t *h, hash_node_t *node)
{
    return h && node && __in_hash(node) && h==node->bucket->hash;
}

DECLARE bool
is_hash_empty(hash_t *h)
{
    return NULL==h || __is_hash_empty(h);
}

DECLARE int
hash_init(hash_t *h, hash_idx_t size)
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

DECLARE int
hash_del(hash_t *h, hash_node_t *node)
{
    if (in_hash(h, node)) {
        __hash_del(node);

        return 0;
    } else {
        return -ENOINLIST;
    }
}

DECLARE int
hash_add(hash_t *h, hash_node_t *node, hash_node_calc_f *ncalc)
{
    if (h && node && ncalc) {
        __hash_del(node);
        __hash_add(h, node, ncalc);

        return 0;
    } else {
        return -EKEYNULL;
    }
}

DECLARE int
hash_change(hash_t *h, hash_node_t *node, hash_change_f *change, hash_node_calc_f *ncalc)
{
    if (in_hash(h, node) && change && ncalc) {
        __hash_change(h, node, change, ncalc);

        return 0;
    } else {
        return -EKEYNULL;
    }
}

DECLARE hash_node_t *
hash_find(hash_t *h, hash_data_calc_f *dcalc, hash_eq_f *eq)
{
    if (h && dcalc && eq) {
        hash_bucket_t *bucket = __hash_bucket(h, (*dcalc)());
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

DECLARE hash_idx_t
hash_bybuf(void *buf, uint32 len, hash_idx_t mask)
{
    bkdr_t bkdr = os_bin_bkdr(buf, len);
    
    return (hash_idx_t)bkdr & mask;
}
/******************************************************************************/
