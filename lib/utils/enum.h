#ifndef __ENUM_H_1b2ec28c4a404b41b1508c4effa0487d__
#define __ENUM_H_1b2ec28c4a404b41b1508c4effa0487d__
/******************************************************************************/
#if 0
/*
* XENUM: c magic enum macro
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

#define KEY_A       KEY_A
#define KEY_B       KEY_B
#define KEY_C       KEY_C
#define KEY_END     KEY_END
#endif

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
    os_fake_declare                 \
    /* end */

typedef bool is_good_enum_f(int id);
typedef char *getnamebyid_f(int id);
typedef int getidbyname_f(char *name);

/******************************************************************************/
#endif /* __ENUM_H_1b2ec28c4a404b41b1508c4effa0487d__ */
