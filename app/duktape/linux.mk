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

INCS+=

#
#changed me, append obj
#
OBJS=$(DIR_SELF)/module/main.o $(JS_OBJS)

TARGET_NAME=js
TARGET=$(TARGET_NAME)
TARGET_TYPE=exe
LIB_DEPEND=c m json-c
CFLAGS+= -std=gnu99 \
	-Os -fomit-frame-pointer \
	-fstrict-aliasing \
	-DDUK_OPT_FORCE_ALIGN=4 \
	-D__THIS_APP=js \
	#end

#
#don't change it
#
include $(DIR_ROOT)/mk/common.mk
