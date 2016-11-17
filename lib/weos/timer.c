

#if 1
tm_clock_t *
__this_timer(void)
{
    if (NULL==__THIS_TIMER) {
        __THIS_TIMER = (tm_clock_t *)os_zalloc(sizeof(tm_clock_t));
    }
    
    return __THIS_TIMER;
}
#endif

