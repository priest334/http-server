###########################################
# A Simple Makefile
###########################################

OUTDIR=bin
SUBDIRS=utils database server sysapp
SUBDIRS_CLEAN=$(SUBDIRS:%=%_clean)

.PHONY: all clean $(SUBDIRS) $(SUBDIRS_CLEAN)

all: $(SUBDIRS)
$(SUBDIRS):
	test -d $(OUTDIR) || mkdir -p $(OUTDIR)
	@+OUTDIR=../$(OUTDIR) make -C $@

clean: $(SUBDIRS_CLEAN)
$(SUBDIRS_CLEAN):
	@+OUTDIR=../$(OUTDIR) make clean -C $(@:%_clean=%)



