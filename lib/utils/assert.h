#ifndef __ASSERT_H_a16d7611625c43ca872b25fbde8df3b6__
#define __ASSERT_H_a16d7611625c43ca872b25fbde8df3b6__

#if defined(__BOOT__) || defined(__APP__)
#define os_assert(_x)           assert(_x)
#else
/*
*   return os_assertV(x); 
*       ==> return ((void)0, x);
*
*   os_assert(0);
*       ==> (void)0;
*/
#define os_assert(_x)           (void)0
#endif

#define os_assertV(_x)          (os_assert(0), _x)
/******************************************************************************/
#endif /* __ASSERT_H_a16d7611625c43ca872b25fbde8df3b6__ */
