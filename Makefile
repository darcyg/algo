ROOTDIR=$(shell pwd)
WORKDIR=$(ROOTDIR)/build

ALGO_VERSION	:= 1.0.0

targets		+= libalgo_$(ALGO_VERSION).so
targets		+= test


.PHONY: targets

all : $(targets)


## libalgo.a :
algosrcs				+= $(ROOTDIR)/lib/src/base64/base64.c
algosrcs				+= $(ROOTDIR)/lib/src/md5/md5.c
algosrcs				+= $(ROOTDIR)/lib/src/3des-ecb/des.cpp
algosrcs				+= $(ROOTDIR)/lib/algo.c
algosrcs				+= $(ROOTDIR)/lib/ds.c

algosrcs				+= $(ROOTDIR)/lib/src/sqlite-amalgamation-3200100/sqlite3.c

algosrcs				:= $(subst .cpp,.c,$(algosrcs))
algoobjs				:= $(subst $(ROOTDIR),$(WORKDIR), $(subst .c,.o,$(algosrcs)))

## test :
testsrcs				+= $(ROOTDIR)/main.c
testsrcs				+= $(algosrcs)

testsrcs				:= $(subst .cpp,.c,$(testsrcs))
testobjs				+= $(subst $(ROOTDIR),$(WORKDIR), $(subst .c,.o,$(testsrcs)))



-include $(ROOTDIR)/make/arch.mk
-include $(ROOTDIR)/make/rules.mk

$(eval $(call LinkLio,libalgo_$(ALGO_VERSION).so,$(algoobjs)))

$(eval $(call LinkApp,test,$(testobjs)))


run :  
	@export LD_LIBRARY_PATH=./build/;./build/test

