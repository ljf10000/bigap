#ifndef __DB_H_db573104d2404862b7487d1d6ebc8d4b__
#define __DB_H_db573104d2404862b7487d1d6ebc8d4b__
/******************************************************************************/
/*
* node: hash_node_t *
*/
#define hx_entry(_node, _type, _member, _nidx) \
    container_of(_node, _type, _member.hash[_nidx])

#define in_hx_table(_node) __in_dlist(&(_node)->list)

#define DECLARE_DB_TABLE(_name, _idx_count) \
typedef struct {                        \
    dlist_t list;                       \
    hash_t  hash[_idx_count];           \
} _name##_table_t;                      \
                                        \
typedef struct {                        \
    dlist_node_t list;                  \
    hash_node_t  hash[_idx_count];      \
} _name##_node_t;                       \
                                        \
typedef mv_t _name##_foreach_f(_name##_node_t *node); \
                                        \
static inline _name##_node_t *          \
_name##_entry(hash_node_t *node, int nidx) \
{                                       \
    return hash_entry(node, _name##_node_t, hash[nidx]); \
}                                       \
                                        \
static inline hash_idx_t                \
_name##_size(_name##_table_t *table, int nidx) \
{                                       \
    return table->hash[nidx].size;      \
}                                       \
                                        \
static inline uint32_t                  \
_name##_count(_name##_table_t *table)   \
{                                       \
    return table->list.count;           \
}                                       \
                                        \
static inline int                       \
_name##_init(_name##_table_t *table, int size_array[]) \
{                                       \
    int i, err, count = os_count_of(table->hash); \
                                        \
    for (i=0; i<count; i++) {           \
        err = __hash_init(&table->hash[i], size_array[i]); \
        if (err<0) {                    \
            return err;                 \
        }                               \
    }                                   \
                                        \
    dlist_init(&table->list);           \
                                        \
    return 0;                           \
}                                       \
                                        \
static inline int                       \
_name##_add(_name##_table_t *table, _name##_node_t *node, hash_node_calc_f *nhash[]) \
{                                       \
    int i, err, count = os_count_of(table->hash); \
                                        \
    for (i=0; i<count; i++) {           \
        err = hash_add(&table->hash[i], &node->hash[i], nhash[i]); \
        if (err<0) {                    \
            return err;                 \
        }                               \
    }                                   \
                                        \
    err = dlist_add(&table->list, &node->list); \
    if (err<0) {                        \
        return err;                     \
    }                                   \
                                        \
    return 0;                           \
}                                       \
                                        \
static inline int                       \
_name##_del(_name##_table_t *table, _name##_node_t *node) \
{                                       \
    int i, err, count = os_count_of(table->hash); \
                                        \
    for (i=0; i<count; i++) {           \
        err = hash_del(&table->hash[i], &node->hash[i]); \
        if (err<0) {                    \
            return err;                 \
        }                               \
    }                                   \
                                        \
    err = dlist_del(&table->list, &node->list); \
    if (err<0) {                        \
        return err;                     \
    }                                   \
                                        \
    return 0;                           \
}                                       \
                                        \
static inline _name##_node_t *          \
_name##_find(_name##_table_t *table, int nidx, hash_data_calc_f *dhash, hash_eq_f *eq) \
{                                       \
    hash_node_t *node = hash_find(&table->hash[nidx], dhash, eq); \
    if (NULL==node) {                   \
        return NULL;                    \
    }                                   \
                                        \
    return _name##_entry(node, nidx);   \
}                                       \
                                        \
static inline int                       \
_name##_foreach(_name##_table_t *table, _name##_foreach_f *foreach) \
{                                       \
    _name##_node_t *node;               \
    mv_u mv;                            \
                                        \
    dlistForeachEntry(&table->list, node, list) { \
        mv.v = (*foreach)(node);        \
        if (is_mv2_break(mv)) {         \
            return mv2_error(mv);       \
        }                               \
    }                                   \
                                        \
    return 0;                           \
}                                       \
                                        \
static inline int                       \
_name##_foreach_safe(_name##_table_t *table, _name##_foreach_f *foreach) \
{                                       \
    _name##_node_t *node, *tmp;         \
    mv_u mv;                            \
                                        \
    dlistForeachEntrySafe(&table->list, node, tmp, list) { \
        mv.v = (*foreach)(node);        \
        if (is_mv2_break(mv)) {         \
            return mv2_error(mv);       \
        }                               \
    }                                   \
                                        \
    return 0;                           \
}                                       \
                                        \
os_fake_declare                         \
/* end */
/******************************************************************************/
/*
* __h1_table_t;
* __h1_node_t;
* __h1_foreach_f;
* 
* void __h1_init(__h1_table_t *table, size_array);
* int __h1_size(__h1_table_t *table, int nidx);
* int __h1_count(__h1_table_t *table);
* int __h1_add(__h1_table_t *table, __h1_node_t *node, hash_node_calc_f *nhash);
* void __h1_del(__h1_table_t *table, __h1_node_t *node);
* __h1_node_t *__h1_find(__h1_table_t *table, hash_data_calc_f *dhash, hash_eq_f *eq);
* int __h1_foreach(__h1_table_t *table, __h1_foreach_f *foreach);
* int __h1_foreach_safe(__h1_table_t *table, __h1_foreach_f *foreach);
*/
DECLARE_DB_TABLE(__h1, 1);

typedef __h1_table_t    h1_table_t;
typedef __h1_node_t     h1_node_t;
typedef __h1_foreach_f  h1_foreach_f;

static inline int 
h1_init(h1_table_t *table, int size)
{
    int size_array[] = {size};

    return __h1_init(table, size_array);
}

static inline hash_idx_t 
h1_size(h1_table_t *table)
{
    return __h1_size(table, 0);
}

static inline uint32_t 
h1_count(h1_table_t *table)
{
    return __h1_count(table);
}

static inline int
h1_add(h1_table_t *table, h1_node_t *node, hash_node_calc_f *nhash)
{
    hash_node_calc_f *hash[] = {nhash};
    
    return __h1_add(table, node, hash);
}

static inline int 
h1_del(h1_table_t *table, h1_node_t *node)
{
    return __h1_del(table, node);
}

static inline h1_node_t *
h1_find(h1_table_t *table, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h1_find(table, 0, dhash, eq);
}

static inline int 
h1_foreach(h1_table_t *table, h1_foreach_f *foreach)
{
    return __h1_foreach(table, foreach);
}

static inline int 
h1_foreach_safe(h1_table_t *table, h1_foreach_f *foreach)
{
    return __h1_foreach_safe(table, foreach);
}
/******************************************************************************/
/*
* __hx_table_t;
* __hx_node_t;
* __hx_foreach_f;
*
* void __hx_init(__hx_table_t *table, size_array);
* int __hx_size(__hx_table_t *table, int nidx);
* int __hx_count(__hx_table_t *table);
* __hx_node_t *__hx_add(__hx_table_t *table, __hx_node_t *node, hash_node_calc_f *nhash[]);
* void __hx_del(__hx_table_t *table, __hx_node_t *node);
* __hx_node_t *__hx_find(__hx_table_t *table, int nidx, hash_data_calc_f *dhash, hash_eq_f *eq);
* int __hx_foreach(__hx_table_t *table, __hx_foreach_f *foreach);
* int __hx_foreach_safe(__hx_table_t *table, __hx_foreach_f *foreach);
*/
DECLARE_DB_TABLE(__h2, 2);
DECLARE_DB_TABLE(__h3, 3);
DECLARE_DB_TABLE(__h4, 4);
DECLARE_DB_TABLE(__h5, 5);
DECLARE_DB_TABLE(__h6, 6);
DECLARE_DB_TABLE(__h7, 7);
DECLARE_DB_TABLE(__h8, 8);
DECLARE_DB_TABLE(__h9, 9);

typedef __h2_table_t    h2_table_t;
typedef __h3_table_t    h3_table_t;
typedef __h4_table_t    h4_table_t;
typedef __h5_table_t    h5_table_t;
typedef __h6_table_t    h6_table_t;
typedef __h7_table_t    h7_table_t;
typedef __h8_table_t    h8_table_t;
typedef __h9_table_t    h9_table_t;

typedef __h2_node_t     h2_node_t;
typedef __h3_node_t     h3_node_t;
typedef __h4_node_t     h4_node_t;
typedef __h5_node_t     h5_node_t;
typedef __h6_node_t     h6_node_t;
typedef __h7_node_t     h7_node_t;
typedef __h8_node_t     h8_node_t;
typedef __h9_node_t     h9_node_t;

typedef __h2_foreach_f  h2_foreach_f;
typedef __h3_foreach_f  h3_foreach_f;
typedef __h4_foreach_f  h4_foreach_f;
typedef __h5_foreach_f  h5_foreach_f;
typedef __h6_foreach_f  h6_foreach_f;
typedef __h7_foreach_f  h7_foreach_f;
typedef __h8_foreach_f  h8_foreach_f;
typedef __h9_foreach_f  h9_foreach_f;

static inline int 
h2_init(h2_table_t *table, int size_array[])
{
    return __h2_init(table, size_array);
}

static inline int 
h3_init(h3_table_t *table, int size_array[])
{
    return __h3_init(table, size_array);
}

static inline int 
h4_init(h4_table_t *table, int size_array[])
{
    return __h4_init(table, size_array);
}

static inline int 
h5_init(h5_table_t *table, int size_array[])
{
    return __h5_init(table, size_array);
}

static inline int 
h6_init(h6_table_t *table, int size_array[])
{
    return __h6_init(table, size_array);
}

static inline int 
h7_init(h7_table_t *table, int size_array[])
{
    return __h7_init(table, size_array);
}

static inline int 
h8_init(h8_table_t *table, int size_array[])
{
    return __h8_init(table, size_array);
}

static inline int 
h9_init(h9_table_t *table, int size_array[])
{
    return __h9_init(table, size_array);
}

static inline hash_idx_t 
h2_size(h2_table_t *table, int nidx)
{
    return __h2_size(table, nidx);
}

static inline hash_idx_t 
h3_size(h3_table_t *table, int nidx)
{
    return __h3_size(table, nidx);
}

static inline hash_idx_t 
h4_size(h4_table_t *table, int nidx)
{
    return __h4_size(table, nidx);
}

static inline hash_idx_t 
h5_size(h5_table_t *table, int nidx)
{
    return __h5_size(table, nidx);
}

static inline hash_idx_t 
h6_size(h6_table_t *table, int nidx)
{
    return __h6_size(table, nidx);
}

static inline hash_idx_t 
h7_size(h7_table_t *table, int nidx)
{
    return __h7_size(table, nidx);
}

static inline hash_idx_t 
h8_size(h8_table_t *table, int nidx)
{
    return __h8_size(table, nidx);
}

static inline hash_idx_t 
h9_size(h9_table_t *table, int nidx)
{
    return __h9_size(table, nidx);
}

static inline uint32_t 
h2_count(h2_table_t *table)
{
    return __h2_count(table);
}

static inline uint32_t 
h3_count(h3_table_t *table)
{
    return __h3_count(table);
}

static inline uint32_t 
h4_count(h4_table_t *table)
{
    return __h4_count(table);
}

static inline uint32_t 
h5_count(h5_table_t *table)
{
    return __h5_count(table);
}

static inline uint32_t 
h6_count(h6_table_t *table)
{
    return __h6_count(table);
}

static inline uint32_t 
h7_count(h7_table_t *table)
{
    return __h7_count(table);
}

static inline uint32_t 
h8_count(h8_table_t *table)
{
    return __h8_count(table);
}

static inline uint32_t 
h9_count(h9_table_t *table)
{
    return __h9_count(table);
}

static inline int
h2_add(h2_table_t *table, h2_node_t *node, hash_node_calc_f *nhash[])
{
    return __h2_add(table, node, nhash);
}

static inline int
h3_add(h3_table_t *table, h3_node_t *node, hash_node_calc_f *nhash[])
{
    return __h3_add(table, node, nhash);
}

static inline int
h4_add(h4_table_t *table, h4_node_t *node, hash_node_calc_f *nhash[])
{
    return __h4_add(table, node, nhash);
}

static inline int
h5_add(h5_table_t *table, h5_node_t *node, hash_node_calc_f *nhash[])
{
    return __h5_add(table, node, nhash);
}

static inline int
h6_add(h6_table_t *table, h6_node_t *node, hash_node_calc_f *nhash[])
{
    return __h6_add(table, node, nhash);
}

static inline int
h7_add(h7_table_t *table, h7_node_t *node, hash_node_calc_f *nhash[])
{
    return __h7_add(table, node, nhash);
}

static inline int
h8_add(h8_table_t *table, h8_node_t *node, hash_node_calc_f *nhash[])
{
    return __h8_add(table, node, nhash);
}

static inline int
h9_add(h9_table_t *table, h9_node_t *node, hash_node_calc_f *nhash[])
{
    return __h9_add(table, node, nhash);
}

static inline int 
h2_del(h2_table_t *table, h2_node_t *node)
{
    return __h2_del(table, node);
}

static inline int 
h3_del(h3_table_t *table, h3_node_t *node)
{
    return __h3_del(table, node);
}

static inline int 
h4_del(h4_table_t *table, h4_node_t *node)
{
    return __h4_del(table, node);
}

static inline int 
h5_del(h5_table_t *table, h5_node_t *node)
{
    return __h5_del(table, node);
}

static inline int 
h6_del(h6_table_t *table, h6_node_t *node)
{
    return __h6_del(table, node);
}

static inline int 
h7_del(h7_table_t *table, h7_node_t *node)
{
    return __h7_del(table, node);
}

static inline int 
h8_del(h8_table_t *table, h8_node_t *node)
{
    return __h8_del(table, node);
}

static inline int 
h9_del(h9_table_t *table, h9_node_t *node)
{
    return __h9_del(table, node);
}

static inline h2_node_t *
h2_find(h2_table_t *table, int nidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h2_find(table, nidx, dhash, eq);
}

static inline h3_node_t *
h3_find(h3_table_t *table, int nidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h3_find(table, nidx, dhash, eq);
}

static inline h4_node_t *
h4_find(h4_table_t *table, int nidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h4_find(table, nidx, dhash, eq);
}

static inline h5_node_t *
h5_find(h5_table_t *table, int nidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h5_find(table, nidx, dhash, eq);
}

static inline h6_node_t *
h6_find(h6_table_t *table, int nidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h6_find(table, nidx, dhash, eq);
}

static inline h7_node_t *
h7_find(h7_table_t *table, int nidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h7_find(table, nidx, dhash, eq);
}

static inline h8_node_t *
h8_find(h8_table_t *table, int nidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h8_find(table, nidx, dhash, eq);
}

static inline h9_node_t *
h9_find(h9_table_t *table, int nidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h9_find(table, nidx, dhash, eq);
}

static inline int 
h2_foreach(h2_table_t *table, h2_foreach_f *foreach)
{
    return __h2_foreach(table, foreach);
}

static inline int 
h3_foreach(h3_table_t *table, h3_foreach_f *foreach)
{
    return __h3_foreach(table, foreach);
}

static inline int 
h4_foreach(h4_table_t *table, h4_foreach_f *foreach)
{
    return __h4_foreach(table, foreach);
}

static inline int 
h5_foreach(h5_table_t *table, h5_foreach_f *foreach)
{
    return __h5_foreach(table, foreach);
}

static inline int 
h6_foreach(h6_table_t *table, h6_foreach_f *foreach)
{
    return __h6_foreach(table, foreach);
}

static inline int 
h7_foreach(h7_table_t *table, h7_foreach_f *foreach)
{
    return __h7_foreach(table, foreach);
}

static inline int 
h8_foreach(h8_table_t *table, h8_foreach_f *foreach)
{
    return __h8_foreach(table, foreach);
}

static inline int 
h9_foreach(h9_table_t *table, h9_foreach_f *foreach)
{
    return __h9_foreach(table, foreach);
}

static inline int 
h2_foreach_safe(h2_table_t *table, h2_foreach_f *foreach)
{
    return __h2_foreach_safe(table, foreach);
}

static inline int 
h3_foreach_safe(h3_table_t *table, h3_foreach_f *foreach)
{
    return __h3_foreach_safe(table, foreach);
}

static inline int 
h4_foreach_safe(h4_table_t *table, h4_foreach_f *foreach)
{
    return __h4_foreach_safe(table, foreach);
}

static inline int 
h5_foreach_safe(h5_table_t *table, h5_foreach_f *foreach)
{
    return __h5_foreach_safe(table, foreach);
}

static inline int 
h6_foreach_safe(h6_table_t *table, h6_foreach_f *foreach)
{
    return __h6_foreach_safe(table, foreach);
}

static inline int 
h7_foreach_safe(h7_table_t *table, h7_foreach_f *foreach)
{
    return __h7_foreach_safe(table, foreach);
}

static inline int 
h8_foreach_safe(h8_table_t *table, h8_foreach_f *foreach)
{
    return __h8_foreach_safe(table, foreach);
}

static inline int 
h9_foreach_safe(h9_table_t *table, h9_foreach_f *foreach)
{
    return __h9_foreach_safe(table, foreach);
}
/******************************************************************************/
#endif /* __DB_H_db573104d2404862b7487d1d6ebc8d4b__ */
