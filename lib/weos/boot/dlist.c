/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
ALWAYS_INLINE dlist_node_t *
____dlist_first(dlist_t *list)
{
    return list_entry(list->head.next, dlist_node_t, node);
}

ALWAYS_INLINE dlist_node_t *
____dlist_last(dlist_t *list)
{
    return list_entry(list->head.prev, dlist_node_t, node);
}

ALWAYS_INLINE dlist_node_t *
____dlist_next(dlist_node_t *node)
{
    return list_entry(node->node.next, dlist_node_t, node);
}

ALWAYS_INLINE dlist_node_t *
____dlist_prev(dlist_node_t *node)
{
    return list_entry(node->node.prev, dlist_node_t, node);
}

DECLARE bool
__in_dlist(dlist_node_t *node)
{
    return NULL!=node->list;
}

ALWAYS_INLINE bool
__is_dlist_empty(dlist_t *list)
{
    return list_empty(&list->head) && 0==list->count;
}

ALWAYS_INLINE bool
__is_dlist_first(dlist_node_t *node)
{
    return __in_dlist(node) && is_list_first(&node->node, &node->list->head);
}

ALWAYS_INLINE bool
__is_dlist_last(dlist_node_t *node)
{
    return __in_dlist(node) && is_list_last(&node->node, &node->list->head);
}

ALWAYS_INLINE dlist_node_t *
__dlist_first(dlist_t *list)
{
    return __is_dlist_empty(list)?NULL:____dlist_first(list);
}

ALWAYS_INLINE dlist_node_t *
__dlist_last(dlist_t *list)
{
    return __is_dlist_empty(list)?NULL:____dlist_last(list);
}

ALWAYS_INLINE dlist_node_t *
__dlist_next(dlist_node_t *node)
{
    return __is_dlist_last(node)?NULL:____dlist_next(node);
}

ALWAYS_INLINE dlist_node_t *
__dlist_prev(dlist_node_t *node)
{
    return __is_dlist_first(node)?NULL:____dlist_prev(node);
}

DECLARE void
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

DECLARE void
__dlist_add(dlist_t *list, dlist_node_t *node)
{
    if (false==__in_dlist(node)) {
        list_add(&node->node, &list->head);

        node->list = list;
        list->count++;
    }
}

DECLARE void
__dlist_add_tail(dlist_t *list, dlist_node_t *node)
{
    if (false==__in_dlist(node)) {
        list_add_tail(&node->node, &list->head);

        node->list = list;
        list->count++;
    }
}

DECLARE void
__dlist_replace(dlist_node_t *old, dlist_node_t *new)
{
    list_replace(&old->node, &new->node);
    
    os_swap_value(old->list, new->list);
}

DECLARE void
__dlist_replace_init(dlist_node_t *old, dlist_node_t *new)
{
    list_replace_init(&old->node, &new->node);
    
    os_swap_value(old->list, new->list);
}

DECLARE bool
in_dlist(dlist_t *list, dlist_node_t *node)
{
    return list && node && list==node->list && __in_dlist(node);
}

DECLARE bool
is_dlist_empty(dlist_t *list)
{
    return NULL==list || __is_dlist_empty(list);
}

DECLARE bool
is_dlist_first(dlist_t *list, dlist_node_t *node)
{
    return in_dlist(list, node) && __is_dlist_first(node);
}

DECLARE bool
is_dlist_last(dlist_t *list, dlist_node_t *node)
{
    return in_dlist(list, node) && __is_dlist_last(node);
}

DECLARE void
dlist_init(dlist_t *list)
{
    list->count = 0;
    
    INIT_LIST_HEAD(&list->head);
}

DECLARE dlist_node_t *
dlist_first(dlist_t *list)
{
    return list?__dlist_first(list):NULL;
}

DECLARE dlist_node_t *
dlist_last(dlist_t *list)
{
    return list?__dlist_last(list):NULL;
}

DECLARE dlist_node_t *
dlist_next(dlist_t *list, dlist_node_t *node)
{
    return in_dlist(list, node)?__dlist_next(node):NULL;
}

DECLARE dlist_node_t *
dlist_prev(dlist_t *list, dlist_node_t *node)
{
    return in_dlist(list, node)?__dlist_prev(node):NULL;
}

DECLARE int
dlist_del(dlist_t *list, dlist_node_t *node)
{
    if (in_dlist(list, node)) {
        __dlist_del(node);

        return 0;
    } else {
        return -ENOINLIST;
    }
}

DECLARE int
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

DECLARE int
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

DECLARE void
dlist_replace(dlist_node_t *old, dlist_node_t *new)
{
    if (old && new) {
        __dlist_replace(old, new);
    }
}

DECLARE void
dlist_replace_init(dlist_node_t *old, dlist_node_t *new)
{
    if (old && new) {
        __dlist_replace_init(old, new);
    }
}

DECLARE dlist_node_t *
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

DECLARE int
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

DECLARE int
dlist_foreach_safe(dlist_t *list, dlist_foreach_f *foreach)
{
    dlist_node_t *node;
    mv_u mv;
    
    dlistForeach(list, node) {
        mv.v = (*foreach)(node);
        if (is_mv2_break(mv)) {
            return mv2_error(mv);
        }
    }

    return 0;
}
/******************************************************************************/
