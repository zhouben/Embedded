BINDIR = $(TOPDIR)/bin
OBJDIR = $(TOPDIR)/obj
OBJLINK = objlink
EXE_FILE = $(BINDIR)/$(PROJECT)

.PHONY : all
all: $(OBJLINK) $(EXE_FILE)
pit: $(OBJLINK) $(EXE_FILE)
	@$(EXE_FILE)

OBJS = $(MODULE_CFILES:%.c=$(OBJDIR)/%.o)
OBJS += $(LOCAL_CFILES:%.c=$(OBJDIR)/$(PROJECT)/%.o)
OBJS += $(TEST_CFILES:%.c=$(OBJDIR)/%.o)

-include $(OBJS:.o=.d)

CFLAGS += $(INCPATHS)

define compile_c_file
@echo $^
@$(CC) $(CFLAGS)  -E -o $(@:%.o=%.i) $<
@: $(CC) $(CFLAGS)  -MM -o $(@:%.o=%.d) $<
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
