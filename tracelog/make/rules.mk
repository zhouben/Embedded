BINDIR = $(TOPDIR)/bin
OBJDIR = $(TOPDIR)/obj
GEN_HEADER_SCRIPT = $(TOPDIR)/tools/generate_header.sh
CFILES_TBL_HEADER = $(SRCDIR)/inc/cfiles_table.h
OBJLINK = objlink
EXE_FILE = $(BINDIR)/$(PROJECT)

.PHONY : all
all: $(OBJLINK) $(EXE_FILE) $(CFILES_TBL_HEADER)
pit: $(OBJLINK) $(EXE_FILE)
	@$(EXE_FILE)

OBJS = $(MODULE_CFILES:%.c=$(OBJDIR)/%.o)
OBJS += $(LOCAL_CFILES:%.c=$(OBJDIR)/$(PROJECT)/%.o)
OBJS += $(TEST_CFILES:%.c=$(OBJDIR)/%.o)
CFILES = $(MODULE_CFILES) $(LOCAL_CFILES) $(TEST_CFILES)
-include $(OBJS:.o=.d)

CFLAGS += $(INCPATHS)

define compile_c_file
@$(CC) $(CFLAGS)  -E -o $(@:%.o=%.i) $<
@$(CC) $(CFLAGS)  -c -o $@ $<
endef

.PHONY : $(OBJLINK)
$(OBJLINK):
	@test -d $(BINDIR) || $(MKDIR) $(BINDIR)
	@test -d $(OBJDIR) || $(MKDIR) $(OBJDIR)
	@test -d $(OBJDIR)/$(PROJECT) || $(MKDIR) $(OBJDIR)/$(PROJECT)
	@: alternative 1
	@: @test -f tmp || (touch tmp && (make -n |wc -l > /dev/null ) && rm tmp)
	@: alternative 2
	@if [ ! -f tmp ]; then touch tmp && (make -n |wc -l > /dev/null ) ; fi

$(EXE_FILE) : $(OBJS)
	@echo "CC $@"
	@$(CC) -o $@ $^
	@$(LN) $@ $(PROJECT)

$(OBJDIR)/$(PROJECT)/%.o: %.c
	$(compile_c_file)

$(OBJDIR)/%.o: %.c
	$(compile_c_file)

define generate_dep
@test -d $(@D) || $(MKDIR) -p $(@D)
@set -e ; rm -f $@; $(CC) -MM $(CFLAGS) $< | sed 's,^\w\+\.o,$(@:.d=.o),' > $@
endef

$(OBJDIR)/$(PROJECT)/%.d: %.c
	$(generate_dep)

$(OBJDIR)/%.d: %.c
	$(generate_dep)

CONFIG = config
.PHONY : $(CONFIG)
$(CONFIG): $(CFILES_TBL_HEADER)
	@true


$(CFILES_TBL_HEADER): $(CFILES)
	@echo generate $(CFILES_TBL_HEADER) depends on $^
	@echo $^|sed 's/ /\n/g' |sort|uniq|sh $(GEN_HEADER_SCRIPT) > $@

CLEAN_FILES = $(OBJDIR)
CLEAN_FILES += $(BINDIR)
CLEAN_FILES += $(PROJECT)
CLEAN_FILES += $(EXE_FILE)

.PHONY: clean
clean:
	@echo [CLEAN]
	@$(RM) $(CLEAN_FILES)
	
FORCE:
	@ture
