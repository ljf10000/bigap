#ifndef __ENUM_H_1b2ec28c4a404b41b1508c4effa0487d__
#define __ENUM_H_1b2ec28c4a404b41b1508c4effa0487d__
/******************************************************************************/
#if 0
/*
* ENUM: c magic enum macro
*
*/
#define XXX_ENUM_MAPPER(_) \
    _(KEY_A, VALUE_A, "NAME-A"), \
    _(KEY_B, VALUE_B, "NAME-B"), \
    _(KEY_C, VALUE_C, "NAME-C"), \
    /* end */
DECLARE_ENUM(xxx, XXX_ENUM_MAPPER, KEY_END);

static inline bool is_good_xxx(int id);
static inline char *xxx_string(int id);
static inline int xxx_idx(char *name);
static inline enum_ops_t *xxx_ops_getter(void);

#define KEY_A       KEY_A
#define KEY_B       KEY_B
#define KEY_C       KEY_C
#define KEY_END     KEY_END
#endif

typedef struct {
    int end;
    
    bool (*is_good)(int id);
    char *(*getname)(int id);
    int (*getid)(char *name);
} enum_ops_t;

typedef enum_ops_t *get_enum_ops_f(void);

#define __ENUM_MAP_VALUE(_key, _value, _name)   _key = _value
#define __ENUM_MAP_NAME(_key, _value, _name)    [_key] = _name

#define DECLARE_ENUM(_mod, _mapper, _end) \
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
    _mod##_string(int id)           \
    {                               \
        char **array = __##_mod##_strings(); \
                                    \
        return is_good_##_mod(id)?array[id]:__unknow; \
    }                               \
                                    \
    static inline int               \
    _mod##_idx(char *s)             \
    {                               \
        char **array = __##_mod##_strings(); \
                                    \
        return os_array_search_str(array, s, 0, _end); \
    }                               \
                                    \
    static inline enum_ops_t *      \
    _mod##_ops_getter(void)         \
    {                               \
        static enum_ops_t ops = {   \
            .end = _end,            \
            .is_good = is_good_##_mod, \
            .getname = _mod##_string, \
            .getid = _mod##_idx,    \
        };                          \
    }                               \
                                    \
    os_fake_declare                 \
    /* end */

typedef bool is_good_enum_f(int id);
typedef char *getnamebyid_f(int id);
typedef int getidbyname_f(char *name);
/******************************************************************************/
#endif /* __ENUM_H_1b2ec28c4a404b41b1508c4effa0487d__ */
