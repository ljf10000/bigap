################################################################################
#LIB = -Wl,--start-group -lpthread -lm   -Wl,--end-group
################################################################################
ifeq ($(CFG_HI_EXPORT_FLAG),)
SDK_DIR       := $(shell pwd)/../../../../../../../..
LIB_DIR       := $(SDK_DIR)/pub/lib
SHARED_LIB_DIR:= $(LIB_DIR)/share
STATIC_LIB_DIR:= $(LIB_DIR)/static
INCLUDE_DIR   := $(SDK_DIR)/pub/include
ROOTFS_DIR    := $(SDK_DIR)/pub/rootfs
include $(SDK_DIR)/cfg.mak
include $(SDK_DIR)/base.mak
endif

TARGET=ttyUSB
TTYTARGET=clearTTY
CROSS_COMPILE=arm-hisiv200-linux-
CC:=$(CROSS_COMPILE)gcc
RM:=rm -rf

ifdef LTEFI
	LTEFI_CFLAGS+=-DLTEFI
endif
ifdef LTEFI_V2
        LTEFI_CFLAGS+=-DLTEFI_V2
endif
#add  end by autelan

image=
ifeq ($(CFG_HI_3G_SUPPORT),y)
image=ppp_dial
endif

################################################################################

all:$(image) install

ppp_dial:
	@$(CC) -static -o $@ $^ -I$(INCLUDE_DIR) -L$(STATIC_LIB_DIR) -lhi_3g -lhi_common $(LIB) -lpthread
	@$(STRIP) $@
	$(CC) $(LTEFI_CFLAGS) -o $(TTYTARGET) clearTTY.c -lpthread
	$(CC) $(LTEFI_CFLAGS) -o $(TARGET) ttyUSB.c -lpthread
clean:
	-@rm -f $(image)
	$(RM) $(TARGET) *.o *.ko *.mod.c .*.cmd *.order *.symvers .tmp_versions $(TARGET) $(TTYTARGET)

install:
	cp ppp_dial $(ROOTBOX_DIR)/usr/sbin
	cp ttyUSB $(ROOTBOX_DIR)/usr/sbin
	cp $(TTYTARGET) $(ROOTBOX_DIR)/usr/sbin/

uninstall:

