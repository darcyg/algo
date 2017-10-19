#ARCH										?= mips
#ARCH										?= arm

ifeq ($(ARCH),mips)
CROSS										:= mips-openwrt-linux-
CROSSTOOLDIR						:=/opt/au/qsdk-dusun/qsdk
export	STAGING_DIR			:= $(CROSSTOOLDIR)/staging_dir
export	PATH						:=$(PATH):$(STAGING_DIR)/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-1.0.14/bin
CROSS_CFLAGS						:= -I$(CROSSTOOLDIR)/staging_dir/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-1.0.14/usr/include
CROSS_LDFLAGS						:= -L$(CROSSTOOLDIR)/staging_dir/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-1.0.14/usr/lib
endif


ifeq ($(ARCH),arm)
CROSS										:= arm-linux-androideabi-
CROSSTOOLDIR						:= /opt/jerry/tool
#export	STAGING_DIR			:= $(CROSSTOOLDIR)/staging_dir
export	PATH						:=$(PATH):$(CROSSTOOLDIR)/bin
#CROSS_CFLAGS						:= -I$(CROSSTOOLDIR)/sysroot/usr/include
#CROSS_LDFLAGS					:= -L$(CROSSTOOLDIR)/sysroot/usr/lib
CROSS_CFLAGS						:= 
CROSS_LDFLAGS						:=
endif



GCC 		:= $(CROSS)gcc
CXX			:= $(CROSS)g++
AR			:= $(CROSS)ar
AS			:= $(CROSS)gcc
RANLIB	:= $(CROSS)ranlib
STRIP 	:= $(CROSS)strip
OBJCOPY	:= $(CROSS)objcopy
OBJDUMP := $(CROSS)objdump
SIZE		:= $(CROSS)size
LD			:= $(CROSS)ld
MKDIR		:= mkdir -p

TARGET_CFLAGS 		+= -Wall -g -O2  -fPIC
TARGET_CFLAGS			+= -static
#TARGET_CFLAGS 		+= -Wall -g -O2 -static
TARGET_CFLAGS			+= -I$(ROOTDIR)/lib/
TARGET_CFLAGS			+= -I$(ROOTDIR)/lib/src/base64 
TARGET_CFLAGS			+= -I$(ROOTDIR)/lib/src/md5
TARGET_CFLAGS			+= -I$(ROOTDIR)/lib/src/3des-ecb
TARGET_CFLAGS			+= -I$(ROOTDIR)/lib/src/sqlite-amalgamation-3200100
TARGET_CFLAGS			+= -I$(ROOTDIR)/lib/src/libhttpd-2.0
TARGET_CFLAGS			+= -I$(ROOTDIR)/product/panel
TARGET_CFLAGS 		+= $(CROSS_CFLAGS)
TARGET_CXXFLAGS 	+= $(TARGET_CFLAGS) -std=c++0x

TARGET_CFLAGS			+= -DSQLITE_OMIT_LOAD_EXTENSION
ifeq ($(ARCH), arm) 
TARGET_CFLAGS			+= -DARCH_ARM
endif

#TARGET_LDFLAGS 		+= -lm -lrt -ldl -lpthread
#TARGET_LDFLAGS 		+= -ldl
ifeq ($(ARCH), mips) 
TARGET_LDFLAGS 		+= -lrt -lm -lpthread
endif
ifeq ($(ARCH),) 
TARGET_LDFLAGS 		+= -lrt -lm -lpthread
endif
TARGET_LDFLAGS 		+= -lstdc++ 

#TARGET_LDFLAGS 		+= -Xlinker
TARGET_LDFLAGS 		+= -static

#TARGET_LDFLAGS 		+= -L$(ROOTDIR)/build -lalgo_$(ALGO_VERSION)
#TARGET_LDFLAGS		+= $(CROSS_LDFLAGS)


