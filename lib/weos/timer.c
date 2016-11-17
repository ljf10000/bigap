
tm_clock_t *
__this_timer(void)
{
    static tm_clock_t *clock;
    
    if (NULL==clock) {
        clock = (tm_clock_t *)os_zalloc(sizeof(tm_clock_t));
    }
    
    return clock;
}

