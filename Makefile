ifeq (openwrt,$(BIGAP_OS))
include openwrt.mk
else
include linux.mk
endif
