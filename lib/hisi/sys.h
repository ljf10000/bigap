#ifndef __SYS_H_1f53644f3fdc49a6a811fa6a0c591b1f__
#define __SYS_H_1f53644f3fdc49a6a811fa6a0c591b1f__
/******************************************************************************/
#ifdef __APP__
/******************************************************************************/
#if IS_PRODUCT_LTEFI_MD
#include "hi_common.h"
#endif

static inline int hisi_init(void)   { return hisi_module_call_0(SYS, Init); }
static inline int hisi_fini(void)   { return hisi_module_call_0(SYS, DeInit); }

#define hisi_sys_get_build_time(tm_pointer) \
    hisi_module_call_x(SYS, GetBuildTime, tm_pointer)
    
#define hisi_sys_get_chip_attr(chip_attr_pointer) \
    hisi_module_call_x(SYS, GetChipAttr, chip_attr_pointer)
    
#define hisi_sys_get_chip_capability(chip_capability, bool_pointer) \
    hisi_module_call_x(SYS, GetChipCapability, chip_capability, bool_pointer)
    
#define hisi_sys_get_conf(conf_pointer) \
    hisi_module_call_x(SYS, GetConf, conf_pointer)
#define hisi_sys_set_conf(conf_pointer) \
    hisi_module_call_x(SYS, SetConf, conf_pointer)
    
#define hisi_sys_get_mem_config(conf_pointer) \
    hisi_module_call_x(SYS, GetMemConfig, conf_pointer)
    
#define hisi_sys_get_time_stamp_ms(ms_pointer) \
    hisi_module_call_x(SYS, GetTimeStampMs, ms_pointer)
    
#define hisi_sys_get_version(version_pointer) \
    hisi_module_call_x(SYS, GetVersion, version_pointer)
    
#define hisi_sys_map_register(reg_addr, len, virtual_addr_pointer) \
    hisi_module_call_x(SYS, MapRegister, reg_addr, len, virtual_addr_pointer)
#define hisi_sys_unmap_register(virtual_addr_pointer) \
    hisi_module_call_x(SYS, UnmapRegister, virtual_addr_pointer)
    
#define hisi_sys_read_register(reg_addr, value_pointer) \
    hisi_module_call_x(SYS, ReadRegister, reg_addr, value_pointer)
#define hisi_sys_write_register(reg_addr, value) \
    hisi_module_call_x(SYS, WriteRegister, reg_addr, value)
    
#define hisi_sys_set_log_level(mod_id, log_level) \
    hisi_module_call_x(SYS, SetLogLevel, mod_id, log_level)
    
#define hisi_sys_set_log_path(path_string) \
    hisi_module_call_x(SYS, SetLogPath, path_string)
    
#define hisi_sys_set_store_path(path_string) \
    hisi_module_call_x(SYS, SetStorePath, path_string)
    
/******************************************************************************/
#endif /* __APP__ */
#endif /* __SYS_H_1f53644f3fdc49a6a811fa6a0c591b1f__ */
