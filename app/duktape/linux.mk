#
#don't change it
#
DIR_SELF=$(shell pwd)

#
#changed DIR_ROOT
#
DIR_ROOT=$(DIR_SELF)/../..

#
#don't change it
#
include $(DIR_ROOT)/mk/define.mk

INCS+=-I$(DIR_SELF)/src -I$(DIR_SELF)/module

#
#changed me, append obj
#
OBJS=$(DIR_SELF)/module/main.o

TARGET_NAME=js
TARGET=$(TARGET_NAME)
TARGET_TYPE=exe
LIB_DEPEND=c m json-c js
CFLAGS+= -std=gnu99 \
	-Os -fomit-frame-pointer \
	-fstrict-aliasing \
	-DDUK_OPT_FORCE_ALIGN=4 \
	#end

#
#don't change it
#
include $(DIR_ROOT)/mk/common.mk
