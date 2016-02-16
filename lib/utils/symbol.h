#ifndef __SYMBOL_H_de411bf6443c4804bf7d2904733cfff6__
#define __SYMBOL_H_de411bf6443c4804bf7d2904733cfff6__
/******************************************************************************/
typedef struct {
    string_t s;

    bkdr_t bkdr;
} symbol_t;

#if 0
typedef struct {
    struct mlist_node node;
    string_t string;
} symbol_t;

typedef struct mlist_table symbol_table_t;

static inline int
sysbol_table_init(symbol_table_t *table, uint32_t size)
{
    return mlist_table_init(table, size);
}

static inline symbol_t *
__symbol_init(symbol_t *sym)
{
    INIT_LIST_HEAD(&sym->node.list);
    sym->string.sym = true;
    
    return sym;
}

static inline symbol_t *
__symbol_string_new(const char *s)
{
    symbol_t *sym = string_NEW(symbol_t, string, s);

    return sym?__symbol_init(sym):NULL;
}

static inline symbol_t *
__symbol_binary_new(const void *binary, uint32_t len)
{
    symbol_t *sym = binary_NEW(symbol_t, string, binary, len);

    return sym?__symbol_init(sym):NULL;
}

static inline symbol_t *
__symbol_entry(const struct mlist_node *node)
{
    return container_of(node, symbol_t, node);
}

static inline bool
__symbol_eq(
    const symbol_t *sym, 
    const void *binary, 
    bool bin, 
    uint32_t len, 
    uint32_t hash
)
{
    return __string_eq(&sym->string, binary, true, bin, len, hash);
}

static inline int
__symbol_cmp(
    const symbol_t *sym, 
    const void *binary, 
    bool bin, 
    uint32_t len
)
{
    return __string_cmp(&sym->string, binary, true, bin, len);
}

static inline symbol_t *
__symbol_insert(
    struct mlist_table *table, 
    int (*data_hash)(void),
    int (*node_hash)(struct mlist_node *node),
    bool (*eq)(struct mlist_node *node),
    struct mlist_node *(*new)(void)
)
{
    struct mlist_node *node = mlist_insert(table, data_hash, node_hash, eq, new);
    if (NULL==node) {
        return NULL;
    }

    symbol_t *sym = __symbol_entry(node);

    return sym;
}

static inline symbol_t *
__symbol_push_string(symbol_table_t *table, const char *s)
{
    uint32_t len = 0;
    uint32_t hash = os_str_BKDR(s, &len);

    int data_hash(void)
    {
        return hash & (table->size - 1);
    }
    
    bool eq(struct mlist_node *node)
    {
        return __symbol_eq(__symbol_entry(node), s, false, len, hash);
    }
    
    int node_hash(struct mlist_node *node)
    {
        symbol_t *sym = __symbol_entry(node);
        
        return sym->string.hash & (table->size - 1);
    }
    
    struct mlist_node *new(void)
    {
        symbol_t *sym = __symbol_string_new(s);

        return sym?&sym->node:NULL;
    }

    return __symbol_insert(table, data_hash, node_hash, eq, new);
}

static inline symbol_t *
__symbol_push_binary(symbol_table_t *table, void *binary, uint32_t len)
{
    uint32_t hash = os_bin_bkdr(binary, len);

    int data_hash(void)
    {
        return hash & (table->size - 1);
    }
    
    bool eq(struct mlist_node *node)
    {
        return __symbol_eq(__symbol_entry(node), binary, true, len, hash);
    }
    
    int node_hash(struct mlist_node *node)
    {
        symbol_t *sym = __symbol_entry(node);
        
        return sym->string.hash & (table->size - 1);
    }
    
    struct mlist_node *new(void)
    {
        symbol_t *sym = __symbol_binary_new(binary, len);

        return sym?&sym->node:NULL;
    }
    
    return __symbol_insert(table, data_hash, node_hash, eq, new);
}

static inline symbol_t *
symbol_push_string(symbol_table_t *table, char *s)
{
    if (NULL==table) {
        return os_assertV(NULL);
    }
    else if (NULL==s) {
        return os_assertV(NULL);
    }
    else {
        return __symbol_push_string(table, s);
    }
}

static inline symbol_t *
symbol_push_binary(symbol_table_t *table, void *binary, uint32_t len)
{
    if (NULL==table) {
        return os_assertV(NULL);
    }
    else if (NULL==binary) {
        return os_assertV(NULL);
    }
    else {
        return __symbol_push_binary(table, binary, len);
    }
}

static inline int
symbol_foreach(symbol_table_t *table, mv_t (*cb)(symbol_t *sym))
{
    mv_t node_cb(struct mlist_node *node)
    {
        return (*cb)(__symbol_entry(node));
    }
    
    return mlist_foreach(table, node_cb);
}

struct string_cursor {
    char *var;
    int len;
};

static inline int
string_cursor_end(struct string_cursor *cursor, int ch)
{
    int value = cursor->var[cursor->len];

    cursor->var[cursor->len] = ch;

    return value;
}

#define string_cursor_save(_cursor, _ch)    _ch = string_cursor_end(_cursor, 0)
#define string_cursor_restore(_cursor, _ch) string_cursor_end(_cursor, _ch)

#define string_cursor_string_call(_cursor, _call) do{ \
    int ch;                                         \
                                                    \
    string_cursor_save(_cursor, ch);                \
    _call, (void)0;                                 \
    string_cursor_restore(_cursor, ch);             \
}while(0)

static inline bool
string_cursor_eq(struct string_cursor *cursor, char *s)
{
    return 0==s[cursor->len] 
        && 0==os_memcmp(s, cursor->var, cursor->len);
}

#define __STRING_CURSOR(_var, _len) {   \
    .var = _var,                        \
    .len = _len,                        \
}   /* end */

#define STRING_CURSOR   __STRING_CURSOR(NULL, 0)

struct string_kv {
    struct string_cursor k, v;
};

#define __STRING_KV(_k, _klen, _v, _vlen) { \
    .k = __STRING_CURSOR(_k, _klen),        \
    .v = __STRING_CURSOR(_v, _vlen),        \
}   /* end */

#define STRING_KV   __STRING_KV(NULL, 0, NULL, 0)
#endif
/******************************************************************************/
#endif /* __SYMBOL_H_de411bf6443c4804bf7d2904733cfff6__ */
