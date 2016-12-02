#ifndef __DLIST_H_0733f48ee6ac42659f61ab9e489a2425__
#define __DLIST_H_0733f48ee6ac42659f61ab9e489a2425__
/******************************************************************************/
#if 1
struct dlist_s;

typedef struct {
    struct list_head node;
    
    struct dlist_s *list;
} dlist_node_t;

typedef struct dlist_s {
    struct list_head head;

    uint32 count;
} dlist_t;

#define dlist_entry(_node, _type, _member)  container_of(_node, _type, _member)

#define DLIST_INITER(_list)      {  \
    .count = 0,                     \
    .head  = LIST_HEAD_INIT(_list), \
}

typedef bool dlist_eq_f(dlist_node_t *node);
typedef int  dlist_cmp_f(dlist_node_t *node);
typedef mv_t dlist_foreach_f(dlist_node_t *node);
#endif
/******************************************************************************/
#if 2
EXTERN bool
in_dlist(dlist_t *list, dlist_node_t *node);

EXTERN bool
is_dlist_empty(dlist_t *list);

EXTERN bool
is_dlist_first(dlist_t *list, dlist_node_t *node);

EXTERN bool
is_dlist_last(dlist_t *list, dlist_node_t *node);

EXTERN void
dlist_init(dlist_t *list);

EXTERN dlist_node_t *
dlist_first(dlist_t *list);

EXTERN dlist_node_t *
dlist_last(dlist_t *list);

EXTERN dlist_node_t *
dlist_next(dlist_t *list, dlist_node_t *node);

EXTERN dlist_node_t *
dlist_prev(dlist_t *list, dlist_node_t *node);

EXTERN int
dlist_del(dlist_t *list, dlist_node_t *node);

EXTERN int
dlist_add(dlist_t *list, dlist_node_t *node);

EXTERN int
dlist_add_tail(dlist_t *list, dlist_node_t *node);

EXTERN void
dlist_replace(dlist_node_t *old, dlist_node_t *new);

EXTERN void
dlist_replace_init(dlist_node_t *old, dlist_node_t *new);

#define dlist_first_entry(_list, _type, _member)   ({   \
    dlist_node_t *__node = dlist_first(_list);          \
    __node?dlist_entry(__node, _type, _member):NULL;    \
})
#define dlist_last_entry(_list, _type, _member)    ({   \
    dlist_node_t *__node = dlist_last(_list);           \
    __node?dlist_entry(__node, _type, _member):NULL;    \
})
#define dlist_next_entry(_list, _pos, _member)     ({   \
    dlist_node_t *__node = dlist_next(_list, &_pos->_member);   \
    __node?dlist_entry(__node, typeof(*(_pos)), _member):NULL;  \
})
#define dlist_prev_entry(_list, _pos, _member)     ({   \
    dlist_node_t *__node = dlist_prev(_list, &_pos->_member);   \
    __node?dlist_entry(__node, typeof(*(_pos)), _member):NULL;  \
})

#define dlistForeach(_list, _node) \
    for (_node=dlist_first(_list); _node; _node=dlist_next(_list, _node))

#define dlistForeachSafe(_list, _node, _tmp) \
    for (_node=dlist_first(_list), _tmp=dlist_next(_list, _node); \
         _node; \
         _node=_tmp, _tmp=dlist_next(_list, _tmp))

#define dlistForeachEntry(_list, _pos, _member) \
    for (_pos=dlist_first_entry(_list, typeof(*(_pos)), _member); \
         _pos; \
         _pos=dlist_next_entry(_list, _pos, _member))

#define dlistForeachEntrySafe(_list, _pos, _tmp, _member) \
    for (_pos=dlist_first_entry(_list, typeof(*(_pos)), _member), _tmp=dlist_next_entry(_list, _pos, _member); \
         _pos; \
         _pos=_tmp, _tmp=dlist_next_entry(_list, _tmp, _member))

EXTERN dlist_node_t *
dlist_find(dlist_t *list, dlist_eq_f *eq);

EXTERN int
dlist_foreach(dlist_t *list, dlist_foreach_f *foreach);
#endif

#ifdef __BOOT__
#include "weos/boot/dlist.c"
#endif
/******************************************************************************/
#endif /* __DLIST_H_0733f48ee6ac42659f61ab9e489a2425__ */
