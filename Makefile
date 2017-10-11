ROOTDIR=$(shell pwd)
WORKDIR=$(ROOTDIR)/build

targets		+= libalgo.so
targets		+= test


.PHONY: targets

all : $(targets)


## libalgo.a :
algosrcs				+= $(ROOTDIR)/lib/src/base64/base64.c
algosrcs				+= $(ROOTDIR)/lib/src/md5/md5.c
algosrcs				+= $(ROOTDIR)/lib/src/3des-ecb/des.cpp
algosrcs				+= $(ROOTDIR)/lib/algo.c

algosrcs				:= $(subst .cpp,.c,$(algosrcs))
algoobjs				:= $(subst $(ROOTDIR),$(WORKDIR), $(subst .c,.o,$(algosrcs)))

## test :
testsrcs				+= $(ROOTDIR)/main.c

testsrcs				:= $(subst .cpp,.c,$(testsrcs))
testobjs				+= $(subst $(ROOTDIR),$(WORKDIR), $(subst .c,.o,$(testsrcs)))


-include $(ROOTDIR)/make/arch.mk
-include $(ROOTDIR)/make/rules.mk

$(eval $(call LinkLio,libalgo.so,$(algoobjs)))

$(eval $(call LinkApp,test,$(testobjs)))


run :  libalgo.so test
	export LD_LIBRARY_PATH=./build/;./build/test

