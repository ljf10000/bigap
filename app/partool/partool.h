#ifndef __PARTOOL_H_cb66d4a55e5a4988886a1cb2a6532172__
#define __PARTOOL_H_cb66d4a55e5a4988886a1cb2a6532172__
/******************************************************************************/
#ifdef __BOOT__

extern int
boot_partool_show_byname(int argc, char *argv[],void *buf);

extern int
boot_partool_new(int argc, char *argv[]);

extern void
boot_partool_clean(void);

#endif
/******************************************************************************/
#endif /* __PARTOOL_H_cb66d4a55e5a4988886a1cb2a6532172__ */
