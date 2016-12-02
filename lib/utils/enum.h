#ifndef __ENUM_H_1b2ec28c4a404b41b1508c4effa0487d__
#define __ENUM_H_1b2ec28c4a404b41b1508c4effa0487d__
/******************************************************************************/
#if 0
/*
* ENUM: c enum macro
*
*/
#define XXX_ENUM_MAPPER(_) \
    _(KEY_A, VALUE_A, "NAME-A"), \
    _(KEY_B, VALUE_B, "NAME-B"), \
    _(KEY_C, VALUE_C, "NAME-C"), \
    /* end */
INLINE_ENUM(xxx, XXX_ENUM_MAPPER, KEY_END);

static inline enum_ops_t *xxx_ops(void);
static inline bool is_good_xxx(int id);
static inline char *xxx_getnamebyid(int id);
static inline int xxx_getidbyname(const char *name);

#define KEY_A       KEY_A
#define KEY_B       KEY_B
#define KEY_C       KEY_C
#define KEY_END     KEY_END
#endif

typedef struct {
    int end;
    
    bool (*is_good)(int id);
    char *(*getname)(int id);
    int (*getid)(const char *name);
} enum_ops_t;

#define __ENUM_MAP_VALUE(_key, _value, _name)   _key = _value
#define __ENUM_MAP_NAME(_key, _value, _name)    [_key] = _name

#define INLINE_ENUM(_mod, _mapper, _end) \
    enum {                          \
        _mapper(__ENUM_MAP_VALUE)   \
                                    \
        _end                        \
    };                              \
                                    \
    static inline bool              \
    is_good_##_mod(int id)          \
    {                               \
        return IS_GOOD_ENUM(id, _end); \
    }                               \
                                    \
    static inline char **           \
    __##_mod##_strings(void)        \
    {                               \
        static char *array[_end] = { _mapper(__ENUM_MAP_NAME) }; \
                                    \
        return array;               \
    }                               \
                                    \
    static inline char *            \
    _mod##_getnamebyid(int id)      \
    {                               \
        char **array = __##_mod##_strings(); \
                                    \
        return is_good_##_mod(id)?array[id]:__unknow; \
    }                               \
                                    \
    static inline int               \
    _mod##_getidbyname(const char *s) \
    {                               \
        char **array = __##_mod##_strings(); \
                                    \
        return os_array_search_str(array, s, 0, _end); \
    }                               \
                                    \
    static inline enum_ops_t *      \
    _mod##_ops(void)                \
    {                               \
        static enum_ops_t ops = {   \
            .end = _end,            \
            .is_good = is_good_##_mod, \
            .getname = _mod##_getnamebyid, \
            .getid = _mod##_getidbyname, \
        };                          \
                                    \
        return &ops;                \
    }                               \
                                    \
    os_fake_declare                 \
    /* end */

#define EXTERN_ENUM(_mod, _mapper, _end) \
    enum {                          \
        _mapper(__ENUM_MAP_VALUE)   \
                                    \
        _end                        \
    };                              \
                                    \
    EXTERN bool                     \
    is_good_##_mod(int id);         \
                                    \
    EXTERN char **                  \
    __##_mod##_strings(void);       \
                                    \
    EXTERN char *                   \
    _mod##_getnamebyid(int id);     \
                                    \
    EXTERN int                      \
    _mod##_getidbyname(const char *s); \
                                    \
    EXTERN enum_ops_t *             \
    _mod##_ops(void);               \
                                    \
    os_fake_declare                 \
    /* end */

#define DECLARE_ENUM(_mod, _mapper, _end) \
    DECLARE bool                    \
    is_good_##_mod(int id)          \
    {                               \
        return IS_GOOD_ENUM(id, _end); \
    }                               \
                                    \
    DECLARE char **                 \
    __##_mod##_strings(void)        \
    {                               \
        static char *array[_end] = { _mapper(__ENUM_MAP_NAME) }; \
                                    \
        return array;               \
    }                               \
                                    \
    DECLARE char *                  \
    _mod##_getnamebyid(int id)      \
    {                               \
        char **array = __##_mod##_strings(); \
                                    \
        return is_good_##_mod(id)?array[id]:__unknow; \
    }                               \
                                    \
    DECLARE int                     \
    _mod##_getidbyname(const char *s) \
    {                               \
        char **array = __##_mod##_strings(); \
                                    \
        return os_array_search_str(array, s, 0, _end); \
    }                               \
                                    \
    DECLARE enum_ops_t *            \
    _mod##_ops(void)                \
    {                               \
        static enum_ops_t ops = {   \
            .end = _end,            \
            .is_good = is_good_##_mod, \
            .getname = _mod##_getnamebyid, \
            .getid = _mod##_getidbyname, \
        };                          \
                                    \
        return &ops;                \
    }                               \
                                    \
    os_fake_declare                 \
    /* end */

/******************************************************************************/
#endif /* __ENUM_H_1b2ec28c4a404b41b1508c4effa0487d__ */
