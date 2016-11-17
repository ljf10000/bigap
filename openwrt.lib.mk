include $(TOPDIR)/rules.mk

PKG_NAME:=bigap-lib
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
PKG_LIB_BUILD_DIR := $(PKG_BUILD_DIR)/lib

include $(INCLUDE_DIR)/package.mk

UBACKTRACE=0
MAKE_FLAGS+="UBACKTRACE=$(UBACKTRACE)"

TARGET_CFLAGS += -Wall \
		-fexceptions -fno-omit-frame-pointer \
		-I$(STAGING_DIR)/usr/include \
		-I$(PKG_LIB_BUILD_DIR) \
		-D__OPENWRT__ \
		-D__BACKTRACE__=$(UBACKTRACE) \
		-D__TAB_AS_SPACE=4 \
		-D__PRODUCT__=8 \
		-DJLOG_SERVER=\\\"1.0.0.5\\\" \
		-std=gnu99 \
		-Wno-unused \
		-fmerge-all-constants \
		#end

TARGET_LDFLAGS+= -L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib

BIGAP_LIB_CFLAGS += -fPIC -shared

AK_PATH=etc/ak

define Package/bigap-lib/install/common
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/config/
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_DIR) $(1)/lib/script/
	$(INSTALL_DIR) $(1)/lib/functions/
	$(INSTALL_DIR) $(1)/$(AK_PATH)/
endef
####################################################################
define Package/libweos
  SECTION:=libs
  CATEGORY:=bigap-lib
  TITLE:=SuperWalle Basic Lib
  DEPENDS:=+netifd +json-c
endef

define Package/libweos/install
	$(Package/bigap-lib/install/common)
	
	$(INSTALL_DATA) $(PKG_LIB_BUILD_DIR)/weos/libweos.so $(1)/usr/lib
endef

define Package/libweos/compile
	$(MAKE) -C $(PKG_LIB_BUILD_DIR)/weos \
		CC="$(TARGET_CC)" \
		CFLAGS=" \
			$(TARGET_CFLAGS) \
			$(TARGET_CPPFLAGS) \
			$(BIGAP_LIB_CFLAGS) \
			" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		OS_TYPE=openwrt \
		#end
	$(CP) $(PKG_LIB_BUILD_DIR)/weos/libweos.so $(STAGING_DIR)/usr/lib
endef
####################################################################
define Package/libjs
  SECTION:=libs
  CATEGORY:=bigap-lib
  TITLE:=SuperWalle Basic Lib
  DEPENDS:=+netifd +json-c +libweos
endef

define Package/libjs/install
	$(Package/bigap-lib/install/common)
	
	$(INSTALL_DATA) $(PKG_LIB_BUILD_DIR)/js/libjs.so $(1)/usr/lib
endef

define Package/libjs/compile
	$(MAKE) -C $(PKG_LIB_BUILD_DIR)/js \
		CC="$(TARGET_CC)" \
		CFLAGS=" \
			$(TARGET_CFLAGS) \
			$(TARGET_CPPFLAGS) \
			$(BIGAP_LIB_CFLAGS) \
			" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		OS_TYPE=openwrt \
		#end
	$(CP) $(PKG_LIB_BUILD_DIR)/js/libjs.so $(STAGING_DIR)/usr/lib
endef
####################################################################
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./* $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
	$(Package/libweos/compile)
	$(Package/libjs/compile)
endef
####################################################################
$(eval $(call BuildPackage,libweos))
$(eval $(call BuildPackage,libjs))
