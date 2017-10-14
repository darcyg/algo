ROOTDIR=$(shell pwd)
WORKDIR=$(ROOTDIR)/build

ALGO_VERSION	:= 1.0.0

targets		+= libalgo_$(ALGO_VERSION).so
targets		+= test
targets		+= sqt


.PHONY: targets

all : $(targets)


## libalgo.a :
algosrcs				+= $(ROOTDIR)/lib/src/base64/base64.c
algosrcs				+= $(ROOTDIR)/lib/src/md5/md5.c
algosrcs				+= $(ROOTDIR)/lib/src/3des-ecb/des.cpp
algosrcs				+= $(ROOTDIR)/lib/algo.c

algosrcs				+= $(ROOTDIR)/lib/src/sqlite-amalgamation-3200100/sqlite3.c

algosrcs				:= $(subst .cpp,.c,$(algosrcs))
algoobjs				:= $(subst $(ROOTDIR),$(WORKDIR), $(subst .c,.o,$(algosrcs)))

## test :
testsrcs				+= $(ROOTDIR)/main.c

testsrcs				:= $(subst .cpp,.c,$(testsrcs))
testobjs				+= $(subst $(ROOTDIR),$(WORKDIR), $(subst .c,.o,$(testsrcs)))


## sqt :  
sqtsrcs					+= $(ROOTDIR)/lib/src/sqlite-amalgamation-3200100/sqlite3.c
sqtsrcs					+= $(ROOTDIR)/sqt.c
sqtobjs				+= $(subst $(ROOTDIR),$(WORKDIR), $(subst .c,.o,$(sqtsrcs)))



-include $(ROOTDIR)/make/arch.mk
-include $(ROOTDIR)/make/rules.mk

$(eval $(call LinkLio,libalgo_$(ALGO_VERSION).so,$(algoobjs)))

$(eval $(call LinkApp,test,$(testobjs)))

$(eval $(call LinkApp,sqt,$(sqtobjs)))


run :  
	@export LD_LIBRARY_PATH=./build/;./build/test

