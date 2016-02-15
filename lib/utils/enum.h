#ifndef __ENUM_H_1b2ec28c4a404b41b1508c4effa0487d__
#define __ENUM_H_1b2ec28c4a404b41b1508c4effa0487d__
/******************************************************************************/
#if 0
#define XXX_LIST(_) \
    _(KEY_A, VALUE_A, "NAME-A"), \
    _(KEY_B, VALUE_B, "NAME-B"), \
    _(KEY_C, VALUE_C, "NAME-C"), \
    /* end */
#endif

#define __ENUM_VALUE_MAP(_key, _value, _name)   _key = _value
#define __ENUM_NAME_MAP(_key, _value, _name)    [_key] = _name

#define DECLARE_ENUM(_mod, _list, _end) \
    enum {                          \
        _list(__ENUM_VALUE_MAP)     \
                                    \
        _end                        \
    };                              \
                                    \
    static inline bool              \
    is_good_##_mod(int id)          \
    {                               \
        return is_good_enum(id, _end); \
    }                               \
                                    \
    static inline char **           \
    __##_mod##_strings(void)        \
    {                               \
        static char *array[_end] = { _list(__ENUM_NAME_MAP) }; \
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
        return os_getstringarrayidx(array, s, 0, _end); \
    }                               \
    os_fake_declare                 \
    /* end */
/******************************************************************************/
#endif /* __ENUM_H_1b2ec28c4a404b41b1508c4effa0487d__ */
