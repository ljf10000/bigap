#ifndef __DB_H_db573104d2404862b7487d1d6ebc8d4b__
#define __DB_H_db573104d2404862b7487d1d6ebc8d4b__
/******************************************************************************/
#ifndef DB_DPRINT
#define DB_DPRINT               0
#endif

#if DB_DPRINT
#define hx_println(_fmt, _args...)  printf(_fmt "\n", ##_args)
#else
#define hx_println(_fmt, _args...)  os_do_nothing()
#endif
/*
* node: hash_node_t *
*/
#define hx_entry(_node, _type, _member, _nidx) \
    safe_container_of(_node, _type, _member.hash[_nidx])

#define __hx_entry(_node, _type, _member, _nidx) \
    safe_container_of(_node, _type, _member.hash[_nidx])

#define in_hx_table(_node) __in_dlist(&(_node)->list)

#define EXTERN_DB_TABLE(_name, _hash_count) \
typedef struct {                        \
    dlist_t list;                       \
    hash_t  hash[_hash_count];          \
} _name##_table_t;                      \
                                        \
typedef struct {                        \
    dlist_node_t list;                  \
    hash_node_t  hash[_hash_count];     \
} _name##_node_t;                       \
                                        \
typedef mv_t _name##_foreach_f(_name##_node_t *node); \
                                        \
EXTERN _name##_node_t *                 \
_name##_entry(hash_node_t *node, int hidx); \
                                        \
EXTERN hash_idx_t                       \
_name##_size(_name##_table_t *table, int hidx); \
                                        \
EXTERN uint32                           \
_name##_count(_name##_table_t *table);  \
                                        \
EXTERN int                              \
_name##_init(_name##_table_t *table, int size_array[]); \
                                        \
EXTERN int                              \
_name##_add(_name##_table_t *table, _name##_node_t *node, hash_node_calc_f *nhash[]); \
                                        \
EXTERN int                              \
_name##_del(_name##_table_t *table, _name##_node_t *node); \
                                        \
EXTERN _name##_node_t *                 \
_name##_find(_name##_table_t *table, int hidx, hash_data_calc_f *dhash, hash_eq_f *eq); \
                                        \
EXTERN int                              \
_name##_foreach(_name##_table_t *table, _name##_foreach_f *foreach, bool safe); \
                                        \
os_fake_declare                         \
/* end */

#define DECLARE_DB_TABLE(_name, _hash_count)    \
DECLARE _name##_node_t *                \
_name##_entry(hash_node_t *node, int hidx) \
{                                       \
    return safe_container_of(node, _name##_node_t, hash[hidx]); \
}                                       \
                                        \
DECLARE hash_idx_t                      \
_name##_size(_name##_table_t *table, int hidx) \
{                                       \
    return table->hash[hidx].size;      \
}                                       \
                                        \
DECLARE uint32                          \
_name##_count(_name##_table_t *table)   \
{                                       \
    return table->list.count;           \
}                                       \
                                        \
DECLARE int                             \
_name##_init(_name##_table_t *table, int size_array[]) \
{                                       \
    int i, err, count = os_count_of(table->hash); \
                                        \
    for (i=0; i<count; i++) {           \
        err = hash_init(&table->hash[i], size_array[i]); \
        if (err<0) {                    \
            hx_println("hx init hash error:%d", err); \
            return err;                 \
        }                               \
    }                                   \
                                        \
    dlist_init(&table->list);           \
    hx_println("hx init ok");           \
                                        \
    return 0;                           \
}                                       \
                                        \
DECLARE int                             \
_name##_add(_name##_table_t *table, _name##_node_t *node, hash_node_calc_f *nhash[]) \
{                                       \
    int i, err, count = os_count_of(table->hash); \
                                        \
    for (i=0; i<count; i++) {           \
        err = hash_add(&table->hash[i], &node->hash[i], nhash[i]); \
        if (err<0) {                    \
            hx_println("hx add hash error:%d", err); \
            return err;                 \
        }                               \
    }                                   \
                                        \
    err = dlist_add(&table->list, &node->list); \
    if (err<0) {                        \
        hx_println("hx add list error:%d", err); \
        return err;                     \
    }                                   \
    hx_println("hx add ok");            \
                                        \
    return 0;                           \
}                                       \
                                        \
DECLARE int                             \
_name##_del(_name##_table_t *table, _name##_node_t *node) \
{                                       \
    int i, err, count = os_count_of(table->hash); \
                                        \
    for (i=0; i<count; i++) {           \
        err = hash_del(&table->hash[i], &node->hash[i]); \
        if (err<0) {                    \
            hx_println("hx del hash error:%d", err); \
            return err;                 \
        }                               \
    }                                   \
                                        \
    err = dlist_del(&table->list, &node->list); \
    if (err<0) {                        \
        hx_println("hx del list error:%d", err); \
        return err;                     \
    }                                   \
    hx_println("hx del ok");            \
                                        \
    return 0;                           \
}                                       \
                                        \
DECLARE _name##_node_t *                \
_name##_find(_name##_table_t *table, int hidx, hash_data_calc_f *dhash, hash_eq_f *eq) \
{                                       \
    hash_node_t *node = hash_find(&table->hash[hidx], dhash, eq); \
    if (NULL==node) {                   \
        return NULL;                    \
    }                                   \
                                        \
    return _name##_entry(node, hidx);   \
}                                       \
                                        \
DECLARE int                             \
_name##_foreach(_name##_table_t *table, _name##_foreach_f *foreach, bool safe) \
{                                       \
    _name##_node_t *node, *tmp;         \
    mv_u mv;                            \
                                        \
    if (safe) {                         \
        hx_println("hx safe foreach ...");  \
        dlistForeachEntrySafe(&table->list, node, tmp, list) { \
            mv.v = (*foreach)(node);    \
            hx_println("hx safe foreach node=%p", node); \
            if (is_mv2_break(mv)) {     \
                hx_println("hx safe foreach ok."); \
                return mv2_error(mv);   \
            }                           \
        }                               \
        hx_println("hx safe foreach ok.");  \
    } else {                            \
        hx_println("hx foreach ...");   \
        dlistForeachEntry(&table->list, node, list) { \
            mv.v = (*foreach)(node);    \
            hx_println("hx foreach node=%p", node); \
            if (is_mv2_break(mv)) {     \
                hx_println("hx foreach ok."); \
                return mv2_error(mv);   \
            }                           \
        }                               \
        hx_println("hx foreach ok.");   \
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
* int __h1_size(__h1_table_t *table, int hidx);
* int __h1_count(__h1_table_t *table);
* int __h1_add(__h1_table_t *table, __h1_node_t *node, hash_node_calc_f *nhash);
* void __h1_del(__h1_table_t *table, __h1_node_t *node);
* __h1_node_t *__h1_find(__h1_table_t *table, hash_data_calc_f *dhash, hash_eq_f *eq);
* int __h1_foreach(__h1_table_t *table, __h1_foreach_f *foreach, bool safe);
*/
#if USE_MOD_DB_H1
EXTERN_DB_TABLE(__h1, 1);

typedef __h1_table_t    h1_table_t;
typedef __h1_node_t     h1_node_t;
typedef __h1_foreach_f  h1_foreach_f;

#define h1_entry(_node, _type, _member)     safe_container_of(_node, _type, _member)

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

static inline uint32 
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
h1_foreach(h1_table_t *table, h1_foreach_f *foreach, bool safe)
{
    return __h1_foreach(table, foreach, safe);
}

DECLARE_DB_H1(_table, _mod, _type, _member)             \
static inline _type *                                   \
_mod##_hx_entry(hash_node_t *node)                      \
{                                                       \
    return hx_entry(node, _type, _member, 0);           \
}                                                       \
                                                        \
static inline _type *                                   \
_mod##_h1_entry(h1_node_t *node)                        \
{                                                       \
    return h1_entry(node, _type, _member);              \
}                                                       \
                                                        \
static inline int                                       \
_mod##_foreach(mv_t (*foreach)(_type *cn), bool safe)   \
{                                                       \
    mv_t node_foreach(h1_node_t *node)                  \
    {                                                   \
        return (*foreach)(_mod##_h1_entry(node));       \
    }                                                   \
                                                        \
    return h1_foreach(_table, node_foreach, safe);      \
}                                                       \
                                                        \
os_fake_declare                                         \
/* end */
#endif
/******************************************************************************/
/*
* __hx_table_t;
* __hx_node_t;
* __hx_foreach_f;
*
* void __hx_init(__hx_table_t *table, size_array);
* int __hx_size(__hx_table_t *table, int hidx);
* int __hx_count(__hx_table_t *table);
* __hx_node_t *__hx_add(__hx_table_t *table, __hx_node_t *node, hash_node_calc_f *nhash[]);
* void __hx_del(__hx_table_t *table, __hx_node_t *node);
* __hx_node_t *__hx_find(__hx_table_t *table, int hidx, hash_data_calc_f *dhash, hash_eq_f *eq);
* int __hx_foreach(__hx_table_t *table, __hx_foreach_f *foreach, bool safe);
*/
#if USE_MOD_DB_H2
EXTERN_DB_TABLE(__h2, 2);

typedef __h2_table_t    h2_table_t;
typedef __h2_node_t     h2_node_t;
typedef __h2_foreach_f  h2_foreach_f;

#define h2_entry(_node, _type, _member)     safe_container_of(_node, _type, _member)

static inline int 
h2_init(h2_table_t *table, int size_array[])
{
    return __h2_init(table, size_array);
}

static inline hash_idx_t 
h2_size(h2_table_t *table, int hidx)
{
    return __h2_size(table, hidx);
}

static inline uint32 
h2_count(h2_table_t *table)
{
    return __h2_count(table);
}

static inline int
h2_add(h2_table_t *table, h2_node_t *node, hash_node_calc_f *nhash[])
{
    return __h2_add(table, node, nhash);
}

static inline int 
h2_del(h2_table_t *table, h2_node_t *node)
{
    return __h2_del(table, node);
}

static inline h2_node_t *
h2_find(h2_table_t *table, int hidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h2_find(table, hidx, dhash, eq);
}

static inline int 
h2_foreach(h2_table_t *table, h2_foreach_f *foreach, bool safe)
{
    return __h2_foreach(table, foreach, safe);
}

DECLARE_DB_H2(_table, _mod, _type, _member)             \
static inline _type *                                   \
_mod##_hx_entry(hash_node_t *node, hash_idx_t nidx)     \
{                                                       \
    return hx_entry(node, _type, _member, nidx);        \
}                                                       \
                                                        \
static inline _type *                                   \
_mod##_h2_entry(h2_node_t *nod)                         \
{                                                       \
    return h2_entry(node, _type, _member);              \
}                                                       \
                                                        \
static inline int                                       \
_mod##_foreach(mv_t (*foreach)(_type *cn), bool safe)   \
{                                                       \
    mv_t node_foreach(h2_node_t *node)                  \
    {                                                   \
        return (*foreach)(_mod##_h2_entry(node));       \
    }                                                   \
                                                        \
    return h2_foreach(_table, node_foreach, safe);      \
}                                                       \
                                                        \
os_fake_declare                                         \
/* end */
#endif

#if USE_MOD_DB_H3
EXTERN_DB_TABLE(__h3, 3);

typedef __h3_table_t    h3_table_t;
typedef __h3_node_t     h3_node_t;
typedef __h3_foreach_f  h3_foreach_f;

#define h3_entry(_node, _type, _member)     safe_container_of(_node, _type, _member)

static inline int 
h3_init(h3_table_t *table, int size_array[])
{
    return __h3_init(table, size_array);
}

static inline hash_idx_t 
h3_size(h3_table_t *table, int hidx)
{
    return __h3_size(table, hidx);
}

static inline uint32 
h3_count(h3_table_t *table)
{
    return __h3_count(table);
}

static inline int
h3_add(h3_table_t *table, h3_node_t *node, hash_node_calc_f *nhash[])
{
    return __h3_add(table, node, nhash);
}

static inline int 
h3_del(h3_table_t *table, h3_node_t *node)
{
    return __h3_del(table, node);
}

static inline h3_node_t *
h3_find(h3_table_t *table, int hidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h3_find(table, hidx, dhash, eq);
}

static inline int 
h3_foreach(h3_table_t *table, h3_foreach_f *foreach, bool safe)
{
    return __h3_foreach(table, foreach, safe);
}

DECLARE_DB_H3(_table, _mod, _type, _member)             \
static inline _type *                                   \
_mod##_hx_entry(hash_node_t *node, hash_idx_t nidx)     \
{                                                       \
    return hx_entry(node, _type, _member, nidx);        \
}                                                       \
                                                        \
static inline _type *                                   \
_mod##_h3_entry(h3_node_t *nod)                         \
{                                                       \
    return h3_entry(node, _type, _member);              \
}                                                       \
                                                        \
static inline int                                       \
_mod##_foreach(mv_t (*foreach)(_type *cn), bool safe)   \
{                                                       \
    mv_t node_foreach(h3_node_t *node)                  \
    {                                                   \
        return (*foreach)(_mod##_h3_entry(node));       \
    }                                                   \
                                                        \
    return h3_foreach(_table, node_foreach, safe);      \
}                                                       \
                                                        \
os_fake_declare                                         \
/* end */
#endif

#if USE_MOD_DB_H4
EXTERN_DB_TABLE(__h4, 4);

typedef __h4_table_t    h4_table_t;
typedef __h4_node_t     h4_node_t;
typedef __h4_foreach_f  h4_foreach_f;

#define h4_entry(_node, _type, _member)     safe_container_of(_node, _type, _member)

static inline int 
h4_init(h4_table_t *table, int size_array[])
{
    return __h4_init(table, size_array);
}

static inline hash_idx_t 
h4_size(h4_table_t *table, int hidx)
{
    return __h4_size(table, hidx);
}

static inline uint32 
h4_count(h4_table_t *table)
{
    return __h4_count(table);
}

static inline int
h4_add(h4_table_t *table, h4_node_t *node, hash_node_calc_f *nhash[])
{
    return __h4_add(table, node, nhash);
}

static inline int 
h4_del(h4_table_t *table, h4_node_t *node)
{
    return __h4_del(table, node);
}

static inline h4_node_t *
h4_find(h4_table_t *table, int hidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h4_find(table, hidx, dhash, eq);
}

static inline int 
h4_foreach(h4_table_t *table, h4_foreach_f *foreach, bool safe)
{
    return __h4_foreach(table, foreach, safe);
}

DECLARE_DB_H4(_table, _mod, _type, _member)             \
static inline _type *                                   \
_mod##_hx_entry(hash_node_t *node, hash_idx_t nidx)     \
{                                                       \
    return hx_entry(node, _type, _member, nidx);        \
}                                                       \
                                                        \
static inline _type *                                   \
_mod##_h4_entry(h4_node_t *nod)                         \
{                                                       \
    return h4_entry(node, _type, _member);              \
}                                                       \
                                                        \
static inline int                                       \
_mod##_foreach(mv_t (*foreach)(_type *cn), bool safe)   \
{                                                       \
    mv_t node_foreach(h4_node_t *node)                  \
    {                                                   \
        return (*foreach)(_mod##_h4_entry(node));       \
    }                                                   \
                                                        \
    return h4_foreach(_table, node_foreach, safe);      \
}                                                       \
                                                        \
os_fake_declare                                         \
/* end */
#endif

#if USE_MOD_DB_H5
EXTERN_DB_TABLE(__h5, 5);

typedef __h5_table_t    h5_table_t;
typedef __h5_node_t     h5_node_t;
typedef __h5_foreach_f  h5_foreach_f;

#define h5_entry(_node, _type, _member)     safe_container_of(_node, _type, _member)

static inline int 
h5_init(h5_table_t *table, int size_array[])
{
    return __h5_init(table, size_array);
}

static inline hash_idx_t 
h5_size(h5_table_t *table, int hidx)
{
    return __h5_size(table, hidx);
}

static inline uint32 
h5_count(h5_table_t *table)
{
    return __h5_count(table);
}

static inline int
h5_add(h5_table_t *table, h5_node_t *node, hash_node_calc_f *nhash[])
{
    return __h5_add(table, node, nhash);
}

static inline int 
h5_del(h5_table_t *table, h5_node_t *node)
{
    return __h5_del(table, node);
}

static inline h5_node_t *
h5_find(h5_table_t *table, int hidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h5_find(table, hidx, dhash, eq);
}

static inline int 
h5_foreach(h5_table_t *table, h5_foreach_f *foreach, bool safe)
{
    return __h5_foreach(table, foreach, safe);
}

DECLARE_DB_H5(_table, _mod, _type, _member)             \
static inline _type *                                   \
_mod##_hx_entry(hash_node_t *node, hash_idx_t nidx)     \
{                                                       \
    return hx_entry(node, _type, _member, nidx);        \
}                                                       \
                                                        \
static inline _type *                                   \
_mod##_h5_entry(h5_node_t *nod)                         \
{                                                       \
    return h5_entry(node, _type, _member);              \
}                                                       \
                                                        \
static inline int                                       \
_mod##_foreach(mv_t (*foreach)(_type *cn), bool safe)   \
{                                                       \
    mv_t node_foreach(h5_node_t *node)                  \
    {                                                   \
        return (*foreach)(_mod##_h5_entry(node));       \
    }                                                   \
                                                        \
    return h5_foreach(_table, node_foreach, safe);      \
}                                                       \
                                                        \
os_fake_declare                                         \
/* end */
#endif

#if USE_MOD_DB_H6
EXTERN_DB_TABLE(__h6, 6);

typedef __h6_table_t    h6_table_t;
typedef __h6_node_t     h6_node_t;
typedef __h6_foreach_f  h6_foreach_f;

#define h6_entry(_node, _type, _member)     safe_container_of(_node, _type, _member)

static inline int 
h6_init(h6_table_t *table, int size_array[])
{
    return __h6_init(table, size_array);
}

static inline hash_idx_t 
h6_size(h6_table_t *table, int hidx)
{
    return __h6_size(table, hidx);
}

static inline uint32 
h6_count(h6_table_t *table)
{
    return __h6_count(table);
}

static inline int
h6_add(h6_table_t *table, h6_node_t *node, hash_node_calc_f *nhash[])
{
    return __h6_add(table, node, nhash);
}

static inline int 
h6_del(h6_table_t *table, h6_node_t *node)
{
    return __h6_del(table, node);
}

static inline h6_node_t *
h6_find(h6_table_t *table, int hidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h6_find(table, hidx, dhash, eq);
}

static inline int 
h6_foreach(h6_table_t *table, h6_foreach_f *foreach, bool safe)
{
    return __h6_foreach(table, foreach, safe);
}

DECLARE_DB_H6(_table, _mod, _type, _member)             \
static inline _type *                                   \
_mod##_hx_entry(hash_node_t *node, hash_idx_t nidx)     \
{                                                       \
    return hx_entry(node, _type, _member, nidx);        \
}                                                       \
                                                        \
static inline _type *                                   \
_mod##_h6_entry(h6_node_t *nod)                         \
{                                                       \
    return h6_entry(node, _type, _member);              \
}                                                       \
                                                        \
static inline int                                       \
_mod##_foreach(mv_t (*foreach)(_type *cn), bool safe)   \
{                                                       \
    mv_t node_foreach(h6_node_t *node)                  \
    {                                                   \
        return (*foreach)(_mod##_h6_entry(node));       \
    }                                                   \
                                                        \
    return h6_foreach(_table, node_foreach, safe);      \
}                                                       \
                                                        \
os_fake_declare                                         \
/* end */
#endif

#if USE_MOD_DB_H7
EXTERN_DB_TABLE(__h7, 7);

typedef __h7_table_t    h7_table_t;
typedef __h7_node_t     h7_node_t;
typedef __h7_foreach_f  h7_foreach_f;

#define h7_entry(_node, _type, _member)     safe_container_of(_node, _type, _member)

static inline int 
h7_init(h7_table_t *table, int size_array[])
{
    return __h7_init(table, size_array);
}

static inline hash_idx_t 
h7_size(h7_table_t *table, int hidx)
{
    return __h7_size(table, hidx);
}

static inline uint32 
h7_count(h7_table_t *table)
{
    return __h7_count(table);
}

static inline int
h7_add(h7_table_t *table, h7_node_t *node, hash_node_calc_f *nhash[])
{
    return __h7_add(table, node, nhash);
}

static inline int 
h7_del(h7_table_t *table, h7_node_t *node)
{
    return __h7_del(table, node);
}

static inline h7_node_t *
h7_find(h7_table_t *table, int hidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h7_find(table, hidx, dhash, eq);
}

static inline int 
h7_foreach(h7_table_t *table, h7_foreach_f *foreach, bool safe)
{
    return __h7_foreach(table, foreach, safe);
}

DECLARE_DB_H7(_table, _mod, _type, _member)             \
static inline _type *                                   \
_mod##_hx_entry(hash_node_t *node, hash_idx_t nidx)     \
{                                                       \
    return hx_entry(node, _type, _member, nidx);        \
}                                                       \
                                                        \
static inline _type *                                   \
_mod##_h7_entry(h7_node_t *nod)                         \
{                                                       \
    return h7_entry(node, _type, _member);              \
}                                                       \
                                                        \
static inline int                                       \
_mod##_foreach(mv_t (*foreach)(_type *cn), bool safe)   \
{                                                       \
    mv_t node_foreach(h7_node_t *node)                  \
    {                                                   \
        return (*foreach)(_mod##_h7_entry(node));       \
    }                                                   \
                                                        \
    return h7_foreach(_table, node_foreach, safe);      \
}                                                       \
                                                        \
os_fake_declare                                         \
/* end */
#endif

#if USE_MOD_DB_H8
EXTERN_DB_TABLE(__h8, 8);

typedef __h8_table_t    h8_table_t;
typedef __h8_node_t     h8_node_t;
typedef __h8_foreach_f  h8_foreach_f;

#define h8_entry(_node, _type, _member)     safe_container_of(_node, _type, _member)

static inline int 
h8_init(h8_table_t *table, int size_array[])
{
    return __h8_init(table, size_array);
}

static inline hash_idx_t 
h8_size(h8_table_t *table, int hidx)
{
    return __h8_size(table, hidx);
}

static inline uint32 
h8_count(h8_table_t *table)
{
    return __h8_count(table);
}

static inline int
h8_add(h8_table_t *table, h8_node_t *node, hash_node_calc_f *nhash[])
{
    return __h8_add(table, node, nhash);
}

static inline int 
h8_del(h8_table_t *table, h8_node_t *node)
{
    return __h8_del(table, node);
}

static inline h8_node_t *
h8_find(h8_table_t *table, int hidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h8_find(table, hidx, dhash, eq);
}

static inline int 
h8_foreach(h8_table_t *table, h8_foreach_f *foreach, bool safe)
{
    return __h8_foreach(table, foreach, safe);
}

DECLARE_DB_H8(_table, _mod, _type, _member)             \
static inline _type *                                   \
_mod##_hx_entry(hash_node_t *node, hash_idx_t nidx)     \
{                                                       \
    return hx_entry(node, _type, _member, nidx);        \
}                                                       \
                                                        \
static inline _type *                                   \
_mod##_h8_entry(h8_node_t *nod)                         \
{                                                       \
    return h8_entry(node, _type, _member);              \
}                                                       \
                                                        \
static inline int                                       \
_mod##_foreach(mv_t (*foreach)(_type *cn), bool safe)   \
{                                                       \
    mv_t node_foreach(h8_node_t *node)                  \
    {                                                   \
        return (*foreach)(_mod##_h8_entry(node));       \
    }                                                   \
                                                        \
    return h8_foreach(_table, node_foreach, safe);      \
}                                                       \
                                                        \
os_fake_declare                                         \
/* end */
#endif

#if USE_MOD_DB_H9
EXTERN_DB_TABLE(__h9, 9);

typedef __h9_table_t    h9_table_t;
typedef __h9_node_t     h9_node_t;
typedef __h9_foreach_f  h9_foreach_f;

#define h9_entry(_node, _type, _member)     safe_container_of(_node, _type, _member)

static inline int 
h9_init(h9_table_t *table, int size_array[])
{
    return __h9_init(table, size_array);
}

static inline hash_idx_t 
h9_size(h9_table_t *table, int hidx)
{
    return __h9_size(table, hidx);
}

static inline uint32 
h9_count(h9_table_t *table)
{
    return __h9_count(table);
}

static inline int
h9_add(h9_table_t *table, h9_node_t *node, hash_node_calc_f *nhash[])
{
    return __h9_add(table, node, nhash);
}

static inline int 
h9_del(h9_table_t *table, h9_node_t *node)
{
    return __h9_del(table, node);
}

static inline h9_node_t *
h9_find(h9_table_t *table, int hidx, hash_data_calc_f *dhash, hash_eq_f *eq)
{
    return __h9_find(table, hidx, dhash, eq);
}

static inline int 
h9_foreach(h9_table_t *table, h9_foreach_f *foreach, bool safe)
{
    return __h9_foreach(table, foreach, safe);
}

DECLARE_DB_H9(_table, _mod, _type, _member)             \
static inline _type *                                   \
_mod##_hx_entry(hash_node_t *node, hash_idx_t nidx)     \
{                                                       \
    return hx_entry(node, _type, _member, nidx);        \
}                                                       \
                                                        \
static inline _type *                                   \
_mod##_h9_entry(h9_node_t *nod)                         \
{                                                       \
    return h9_entry(node, _type, _member);              \
}                                                       \
                                                        \
static inline int                                       \
_mod##_foreach(mv_t (*foreach)(_type *cn), bool safe)   \
{                                                       \
    mv_t node_foreach(h9_node_t *node)                  \
    {                                                   \
        return (*foreach)(_mod##_h9_entry(node));       \
    }                                                   \
                                                        \
    return h9_foreach(_table, node_foreach, safe);      \
}                                                       \
                                                        \
os_fake_declare                                         \
/* end */
#endif

#ifdef __BOOT__
#include "weos/boot/db.c"
#endif
/******************************************************************************/
#endif /* __DB_H_db573104d2404862b7487d1d6ebc8d4b__ */
