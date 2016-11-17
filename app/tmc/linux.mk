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
OBJS=$(DIR_SELF)/tmc.o
TARGET_NAME=tmc
TARGET=$(TARGET_NAME)
TARGET_TYPE=exe
LIB_DEPEND=c json-c weos
CFLAGS+=-DTM_TICKS=1000 -DTM_TIMEOUT=5000

#
#don't change it
#
include $(DIR_ROOT)/mk/common.mk
