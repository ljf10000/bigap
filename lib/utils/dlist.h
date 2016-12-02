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
static inline dlist_node_t *
____dlist_first(dlist_t *list)
{
    return list_entry(list->head.next, dlist_node_t, node);
}

static inline dlist_node_t *
____dlist_last(dlist_t *list)
{
    return list_entry(list->head.prev, dlist_node_t, node);
}

static inline dlist_node_t *
____dlist_next(dlist_node_t *node)
{
    return list_entry(node->node.next, dlist_node_t, node);
}

static inline dlist_node_t *
____dlist_prev(dlist_node_t *node)
{
    return list_entry(node->node.prev, dlist_node_t, node);
}

static inline bool
__in_dlist(dlist_node_t *node)
{
    return NULL!=node->list;
}

static inline bool
__is_dlist_empty(dlist_t *list)
{
    return list_empty(&list->head) && 0==list->count;
}

static inline bool
__is_dlist_first(dlist_node_t *node)
{
    return __in_dlist(node) && is_list_first(&node->node, &node->list->head);
}

static inline bool
__is_dlist_last(dlist_node_t *node)
{
    return __in_dlist(node) && is_list_last(&node->node, &node->list->head);
}

static inline dlist_node_t *
__dlist_first(dlist_t *list)
{
    return __is_dlist_empty(list)?NULL:____dlist_first(list);
}

static inline dlist_node_t *
__dlist_last(dlist_t *list)
{
    return __is_dlist_empty(list)?NULL:____dlist_last(list);
}

static inline dlist_node_t *
__dlist_next(dlist_node_t *node)
{
    return __is_dlist_last(node)?NULL:____dlist_next(node);
}

static inline dlist_node_t *
__dlist_prev(dlist_node_t *node)
{
    return __is_dlist_first(node)?NULL:____dlist_prev(node);
}

#define dlist_entry(_node, _type, _member)        container_of(_node, _type, _member)
#define __dlist_first_entry(_list, _type, _member)   ({ \
    dlist_node_t *__node = __dlist_first(_list);        \
    __node?dlist_entry(__node, _type, _member):NULL;    \
})
#define __dlist_last_entry(_list, _type, _member)    ({ \
    dlist_node_t *__node = __dlist_last(_list);         \
    __node?dlist_entry(__node, _type, _member):NULL;    \
})
#define __dlist_next_entry(_pos, _member)            ({ \
    dlist_node_t *__node = __dlist_next(&_pos->_member);\
    __node?dlist_entry(__node, typeof(*(_pos)), _member):NULL; \
})
#define __dlist_prev_entry(_pos, _member)            ({ \
    dlist_node_t *__node = __dlist_prev(&_pos->_member);\
    __node?dlist_entry(__node, typeof(*(_pos)), _member):NULL; \
})

static inline void
__dlist_del(dlist_node_t *node)
{
    if (__in_dlist(node)) {
        __list_del_entry(&node->node);
        node->node.prev = NULL;
        node->node.next = NULL;
        node->list->count--;
        node->list = NULL;
    }
}

static inline void
__dlist_add(dlist_t *list, dlist_node_t *node)
{
    if (false==__in_dlist(node)) {
        list_add(&node->node, &list->head);

        node->list = list;
        list->count++;
    }
}

static inline void
__dlist_add_tail(dlist_t *list, dlist_node_t *node)
{
    if (false==__in_dlist(node)) {
        list_add_tail(&node->node, &list->head);

        node->list = list;
        list->count++;
    }
}

static inline void
__dlist_replace(dlist_node_t *old, dlist_node_t *new)
{
    list_replace(&old->node, &new->node);
    
    os_swap_value(old->list, new->list);
}

static inline void
__dlist_replace_init(dlist_node_t *old, dlist_node_t *new)
{
    list_replace_init(&old->node, &new->node);
    
    os_swap_value(old->list, new->list);
}
#endif
/******************************************************************************/
#if 3
static inline bool
in_dlist(dlist_t *list, dlist_node_t *node)
{
    return list && node && list==node->list && __in_dlist(node);
}

static inline bool
is_dlist_empty(dlist_t *list)
{
    return NULL==list || __is_dlist_empty(list);
}

static inline bool
is_dlist_first(dlist_t *list, dlist_node_t *node)
{
    return in_dlist(list, node) && __is_dlist_first(node);
}

static inline bool
is_dlist_last(dlist_t *list, dlist_node_t *node)
{
    return in_dlist(list, node) && __is_dlist_last(node);
}

static inline void
dlist_init(dlist_t *list)
{
    list->count = 0;
    
    INIT_LIST_HEAD(&list->head);
}

static inline dlist_node_t *
dlist_first(dlist_t *list)
{
    return list?__dlist_first(list):NULL;
}

static inline dlist_node_t *
dlist_last(dlist_t *list)
{
    return list?__dlist_last(list):NULL;
}

static inline dlist_node_t *
dlist_next(dlist_t *list, dlist_node_t *node)
{
    return in_dlist(list, node)?__dlist_next(node):NULL;
}

static inline dlist_node_t *
dlist_prev(dlist_t *list, dlist_node_t *node)
{
    return in_dlist(list, node)?__dlist_prev(node):NULL;
}

static inline int
dlist_del(dlist_t *list, dlist_node_t *node)
{
    if (in_dlist(list, node)) {
        __dlist_del(node);

        return 0;
    } else {
        return -ENOINLIST;
    }
}

static inline int
dlist_add(dlist_t *list, dlist_node_t *node)
{
    if (list && node) {
        if (__in_dlist(node)) {
            __dlist_del(node);
        }

        __dlist_add(list, node);

        return 0;
    } else {
        return -EKEYNULL;
    }
}

static inline int
dlist_add_tail(dlist_t *list, dlist_node_t *node)
{
    if (list && node) {
        if (__in_dlist(node)) {
            __dlist_del(node);
        }

        __dlist_add_tail(list, node);

        return 0;
    } else {
        return -EKEYNULL;
    }
}

static inline void
dlist_replace(dlist_node_t *old, dlist_node_t *new)
{
    if (old && new) {
        __dlist_replace(old, new);
    }
}

static inline void
dlist_replace_init(dlist_node_t *old, dlist_node_t *new)
{
    if (old && new) {
        __dlist_replace_init(old, new);
    }
}

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

static inline dlist_node_t *
dlist_find(dlist_t *list, dlist_eq_f *eq)
{
    if (list && eq) {
        dlist_node_t *node;
        
        dlistForeach(list, node) {
            if ((*eq)(node)) {
                return node;
            }
        }
    }
    
    return NULL;
}

static inline int
dlist_foreach(dlist_t *list, dlist_foreach_f *foreach)
{
    dlist_node_t *node, *tmp;
    mv_u mv;
    
    dlistForeachSafe(list, node, tmp) {
        mv.v = (*foreach)(node);
        if (is_mv2_break(mv)) {
            return mv2_error(mv);
        }
    }

    return 0;
}
#endif

#ifdef __BOOT__
#include "weos/boot/dlist.c"
#endif
/******************************************************************************/
#endif /* __DLIST_H_0733f48ee6ac42659f61ab9e489a2425__ */
