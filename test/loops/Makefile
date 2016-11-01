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

#
#changed me, append obj
#
OBJS=$(DIR_SELF)/loops.o
TARGET_NAME=loops
TARGET=$(TARGET_NAME)
TARGET_TYPE=test
LIB_DEPEND=c json-c
CFLAGS+=-D__TEST__ 

#
#don't change it
#
include $(DIR_ROOT)/mk/common.mk
