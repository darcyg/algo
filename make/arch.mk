#CROSS 	?= mips-openwrt-linux-


ifneq ($(CROSS),)
CROSSTOOLDIR						:=/opt/au/qsdk-dusun/qsdk
export	STAGING_DIR			:= $(CROSSTOOLDIR)/staging_dir
export	PATH						:=$(PATH):$(STAGING_DIR)/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-1.0.14/bin
ARCH										?= mips
CROSS_CFLAGS						:= -I$(CROSSTOOLDIR)/staging_dir/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-1.0.14/usr/include
CROSS_LDFLAGS						:= -L$(CROSSTOOLDIR)/staging_dir/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-1.0.14/usr/lib
endif

GCC 		?= $(CROSS)gcc
CXX			?= $(CROSS)g++
AR			?= $(CROSS)ar
AS			?= $(CROSS)gcc
RANLIB	?= $(CROSS)ranlib
STRIP 	?= $(CROSS)strip
OBJCOPY	?= $(CROSS)objcopy
OBJDUMP ?= $(CROSS)objdump
SIZE		?= $(CROSS)size
LD			?= $(CROSS)ld
MKDIR		?= mkdir -p

TARGET_CFLAGS 		+= -Wall -g -O2 
TARGET_CFLAGS			+= -I$(ROOTDIR)/lib/
TARGET_CFLAGS			+= -I$(ROOTDIR)/lib/src/base64 
TARGET_CFLAGS			+= -I$(ROOTDIR)/lib/src/md5
TARGET_CFLAGS			+= -I$(ROOTDIR)/lib/src/3des-ecb
TARGET_CFLAGS 		+= $(CROSS_CFLAGS)
TARGET_CXXFLAGS 	+= $(TARGET_CFLAGS) -std=c++0x

TARGET_LDFLAGS 		+= -lm -lrt -ldl -lpthread
TARGET_LDFLAGS		+= $(CROSS_LDFLAGS)


