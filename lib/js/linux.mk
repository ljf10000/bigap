#
#don't change it
#
DIR_SELF=$(shell pwd)

#
#changed DIR_ROOT
#
export DIR_ROOT=$(DIR_SELF)/../..

#
#don't change it
#
include $(DIR_ROOT)/mk/define.mk

#
#changed me, append obj
#
OBJS=$(DIR_SELF)/duktape.o \
	$(DIR_SELF)/duk.o \
	$(DIR_SELF)/global.o \
	$(DIR_SELF)/libbz.o \
	$(DIR_SELF)/libc.o \
	$(DIR_SELF)/libcurl.o \
	$(DIR_SELF)/liblz.o \
	$(DIR_SELF)/libz.o \
	$(DIR_SELF)/my.o \
	#end
TARGET_NAME=js
SONAME=lib$(TARGET_NAME).so
SONAME_MASTER=$(SONAME).1
TARGET=$(SONAME_MASTER).0
TARGET_TYPE=lib
LIB_DEPEND=c m json-c
__CFLAGS+=

#
#don't change it
#
include $(DIR_ROOT)/mk/common.mk
