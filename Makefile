ROOTDIR=$(shell pwd)
WORKDIR=$(ROOTDIR)/build

targets		+= libalgo.a 
targets		+= test


.PHONY: targets

all : $(targets)


## libalgo.a :
algosrcs				+= $(ROOTDIR)/lib/src/base64/base64.c
algosrcs				+= $(ROOTDIR)/lib/src/md5/md5.c
algosrcs				+= $(ROOTDIR)/lib/algo.c

algoobjs				:= $(subst $(ROOTDIR),$(WORKDIR), $(subst .c,.o,$(algosrcs)))


## test :
testsrcs				+= $(ROOTDIR)/main.c

testobjs				:= $(subst $(ROOTDIR),$(WORKDIR), $(subst .c,.o,$(testsrcs)))
testobjs				+= $(ROOTDIR)/build/libalgo.a

	
-include $(ROOTDIR)/make/arch.mk
-include $(ROOTDIR)/make/rules.mk


$(eval $(call LinkLia,libalgo.a,$(algoobjs)))

$(eval $(call LinkApp,test,$(testobjs)))

run : 
	$(ROOTDIR)/build/test

