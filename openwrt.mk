include $(TOPDIR)/rules.mk

PKG_NAME:=bigap
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
PKG_LIB_BUILD_DIR := $(PKG_BUILD_DIR)/lib
PKG_APP_BUILD_DIR := $(PKG_BUILD_DIR)/app

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += -Wall \
		-fexceptions -fno-omit-frame-pointer \
		-I$(STAGING_DIR)/usr/include \
		-I$(PKG_LIB_BUILD_DIR) \
		-D__OPENWRT__ \
		-DBACKTRACE_TYPE=0 \
		-D__TAB_AS_SPACE=4 \
		-D__PRODUCT__=6 \
		-std=gnu99 \
		-Wno-unused \
		-fmerge-all-constants \
		#end

TARGET_LDFLAGS+= -L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib

CAPPLIB_CFLAGS += -fPIC -shared

AK_PATH=etc/ak

define Package/bigap/install/common
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/config/
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_DIR) $(1)/$(AK_PATH)/
endef
####################################################################
define Package/ak
  SECTION:=apps
  CATEGORY:=bigap
  TITLE:=Autelan Basic App
  DEPENDS:=+libjson-c
endef

define Package/ak/install
	$(Package/bigap/install/common)
	
	$(INSTALL_BIN) $(PKG_APP_BUILD_DIR)/ak/ak $(1)/usr/bin
endef

define Package/ak/compile
	$(MAKE) -C $(PKG_APP_BUILD_DIR)/ak \
		CC="$(TARGET_CC)" \
		CFLAGS=" \
			$(TARGET_CFLAGS) \
			$(TARGET_CPPFLAGS) \
			" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		OS_TYPE=openwrt \
		#end
endef
####################################################################
define Package/jlogd
  SECTION:=apps
  CATEGORY:=bigap
  TITLE:=Autelan Basic App
  DEPENDS:=+libjson-c
endef

define Package/jlogd/install
	$(Package/bigap/install/common)
	
	$(INSTALL_BIN) $(PKG_APP_BUILD_DIR)/jlogd/jlogd $(1)/usr/bin
	$(INSTALL_DATA) $(PKG_APP_BUILD_DIR)/jlogd/jlogd.key $(1)/$(AK_PATH)/
endef

define Package/jlogd/compile
	$(MAKE) -C $(PKG_APP_BUILD_DIR)/jlogd \
		CC="$(TARGET_CC)" \
		CFLAGS=" \
			$(TARGET_CFLAGS) \
			$(TARGET_CPPFLAGS) \
			" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		OS_TYPE=openwrt \
		#end
endef
####################################################################
define Package/jlogger
  SECTION:=apps
  CATEGORY:=bigap
  TITLE:=Autelan Basic App
  DEPENDS:=+jlogd
endef

define Package/jlogger/install
	$(Package/bigap/install/common)
	
	$(INSTALL_BIN) $(PKG_APP_BUILD_DIR)/jlogger/jlogger $(1)/usr/bin
endef

define Package/jlogger/compile
	$(MAKE) -C $(PKG_APP_BUILD_DIR)/jlogger \
		CC="$(TARGET_CC)" \
		CFLAGS=" \
			$(TARGET_CFLAGS) \
			$(TARGET_CPPFLAGS) \
			" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		OS_TYPE=openwrt \
		#end
endef
####################################################################
define Package/smd
  SECTION:=apps
  CATEGORY:=bigap
  TITLE:=Autelan Basic App
  DEPENDS:=+jlogger
endef

define Package/smd/install
	$(Package/bigap/install/common)

	$(INSTALL_BIN) $(PKG_APP_BUILD_DIR)/smd/smd $(1)/usr/bin
	$(INSTALL_DATA) $(PKG_APP_BUILD_DIR)/smd/smd.key $(1)/$(AK_PATH)/
endef

define Package/smd/compile
	$(MAKE) -C $(PKG_APP_BUILD_DIR)/smd \
		CC="$(TARGET_CC)" \
		CFLAGS=" \
			$(TARGET_CFLAGS) \
			$(TARGET_CPPFLAGS) \
			" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		OS_TYPE=openwrt \
		#end
endef
####################################################################
define Package/smc
  SECTION:=apps
  CATEGORY:=bigap
  TITLE:=Autelan Basic App
  DEPENDS:=+smd
endef

define Package/smc/install
	$(Package/bigap/install/common)

	$(INSTALL_BIN) $(PKG_APP_BUILD_DIR)/smc/smc $(1)/usr/bin
endef

define Package/smc/compile
	$(MAKE) -C $(PKG_APP_BUILD_DIR)/smc \
		CC="$(TARGET_CC)" \
		CFLAGS=" \
			$(TARGET_CFLAGS) \
			$(TARGET_CPPFLAGS) \
			" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		OS_TYPE=openwrt \
		#end
endef
####################################################################
define Package/duktape
  SECTION:=apps
  CATEGORY:=bigap
  TITLE:=Autelan Basic App
  DEPENDS:=+libjson-c
endef

define Package/duktape/install
	$(Package/bigap/install/common)

	$(INSTALL_BIN) $(PKG_APP_BUILD_DIR)/duktape/js $(1)/usr/bin
endef

define Package/duktape/compile
	$(MAKE) -C $(PKG_APP_BUILD_DIR)/duktape \
		CC="$(TARGET_CC)" \
		CFLAGS=" \
			$(TARGET_CFLAGS) \
			$(TARGET_CPPFLAGS) \
			-DDUK_OPT_FORCE_ALIGN \
			-Os -fomit-frame-pointer \
			-fstrict-aliasing \
			" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		OS_TYPE=openwrt \
		#end
endef
####################################################################
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./* $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
	$(Package/ak/compile)
	$(Package/jlogd/compile)
	$(Package/jlogger/compile)
	$(Package/smd/compile)
	$(Package/smc/compile)
	$(Package/duktape/compile)
endef
####################################################################
$(eval $(call BuildPackage,ak))
$(eval $(call BuildPackage,jlogd))
$(eval $(call BuildPackage,jlogger))
$(eval $(call BuildPackage,smd))
$(eval $(call BuildPackage,smc))
$(eval $(call BuildPackage,duktape))
