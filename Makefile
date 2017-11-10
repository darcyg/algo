ROOTDIR=$(shell pwd)
WORKDIR=$(ROOTDIR)/build

ALGO_VERSION	:= 1.0.0

#targets		+= libalgo_$(ALGO_VERSION).so
targets		+= smd


.PHONY: targets

all : $(targets)


## libalgo.a :
algosrcs				+= $(ROOTDIR)/lib/src/base64/base64.c
algosrcs				+= $(ROOTDIR)/lib/src/md5/md5.c
algosrcs				+= $(ROOTDIR)/lib/src/3des-ecb/des.cpp
algosrcs				+= $(ROOTDIR)/lib/algo.c
algosrcs				+= $(ROOTDIR)/lib/ds.c

algosrcs				+= $(ROOTDIR)/lib/src/sqlite-amalgamation-3200100/sqlite3.c

algosrcs				+= $(ROOTDIR)/lib/src/libhttpd-2.0/api.c
algosrcs				+= $(ROOTDIR)/lib/src/libhttpd-2.0/ember.c
algosrcs				+= $(ROOTDIR)/lib/src/libhttpd-2.0/ip_acl.c
algosrcs				+= $(ROOTDIR)/lib/src/libhttpd-2.0/protocol.c
algosrcs				+= $(ROOTDIR)/lib/src/libhttpd-2.0/version.c
algosrcs				+= $(ROOTDIR)/lib/lweb.c
algosrcs				+= $(ROOTDIR)/lib/util.cpp
algosrcs				+= $(ROOTDIR)/lib/cloud.cpp

algosrcs				+= $(ROOTDIR)/lib/src/jansson/dump.c       
algosrcs				+= $(ROOTDIR)/lib/src/jansson/hashtable_seed.c  
algosrcs				+= $(ROOTDIR)/lib/src/jansson/pack_unpack.c  
algosrcs				+= $(ROOTDIR)/lib/src/jansson/utf.c
algosrcs				+= $(ROOTDIR)/lib/src/jansson/error.c      
algosrcs				+= $(ROOTDIR)/lib/src/jansson/load.c            
algosrcs				+= $(ROOTDIR)/lib/src/jansson/strbuffer.c    
algosrcs				+= $(ROOTDIR)/lib/src/jansson/value.c
algosrcs				+= $(ROOTDIR)/lib/src/jansson/hashtable.c  
algosrcs				+= $(ROOTDIR)/lib/src/jansson/memory.c          
algosrcs				+= $(ROOTDIR)/lib/src/jansson/strconv.c
algosrcs				+= $(ROOTDIR)/lib/json_parser.c

algosrcs				:= $(subst .cpp,.c,$(algosrcs))
algoobjs				:= $(subst $(ROOTDIR),$(WORKDIR), $(subst .c,.o,$(algosrcs)))

## smd :
smdsrcs				+= $(ROOTDIR)/main.c
smdsrcs				+= $(algosrcs)
smdsrcs				+= $(ROOTDIR)/product/panel/web.c
smdsrcs				+= $(ROOTDIR)/product/panel/task.c
smdsrcs				+= $(ROOTDIR)/product/panel/trig.c
smdsrcs				+= $(ROOTDIR)/product/panel/comm.c
smdsrcs				+= $(ROOTDIR)/src/ayla/log.c
smdsrcs				+= $(ROOTDIR)/src/ayla/lookup_by_name.c
smdsrcs				+= $(ROOTDIR)/src/ayla/timer.c
smdsrcs				+= $(ROOTDIR)/src/ayla/time_utils.c
smdsrcs				+= $(ROOTDIR)/src/ayla/assert.c
smdsrcs				+= $(ROOTDIR)/src/ayla/file_event.c
smdsrcs				+= $(ROOTDIR)/src/ayla/file_io.c
smdsrcs				+= $(ROOTDIR)/src/lockqueue.c
smdsrcs				+= $(ROOTDIR)/src/list.c
smdsrcs				+= $(ROOTDIR)/src/mutex.c
smdsrcs				+= $(ROOTDIR)/src/cond.c
smdsrcs				+= $(ROOTDIR)/src/platform/system.c


smdsrcs				:= $(subst .cpp,.c,$(smdsrcs))
smdobjs				+= $(subst $(ROOTDIR),$(WORKDIR), $(subst .c,.o,$(smdsrcs)))
#smdobjs				+= $(ROOTDIR)/lib/src/curl/libcurl.a
#smdobjs				+= /opt/jerry/tool/sysroot/usr/lib/libz.a 
smdobjs				+= $(TARGET_ALIBS)


-include $(ROOTDIR)/make/arch.mk
-include $(ROOTDIR)/make/rules.mk

$(eval $(call LinkLio,libalgo_$(ALGO_VERSION).so,$(algoobjs)))

$(eval $(call LinkApp,smd,$(smdobjs)))

run :  
	export LD_LIBRARY_PATH=$(ROOTDIR)/devroot/;
	cp $(ROOTDIR)/build/smd $(ROOTDIR)/devroot/ -rf;
	sudo $(ROOTDIR)/build/smd -i 192.168.0.6 -p 10888

