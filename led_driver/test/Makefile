CROSS	= arm-fsl-linux-gnueabi-
CC	     = $(CROSS)gcc 
STRIP	= $(CROSS)strip
CFLAGS	= -Wall -g -O0 -ggdb

EXEC	= led_test
RD_EXEC = read_led
WR_EXEC = write_led

CFILES = led_test.c read_led.c write_led.c
OBJDIR  = ./obj
BINDIR  = ./bin
OBJS    = $(CFILES:%.c=$(OBJDIR)/%.o)

all:  clean pre_build $(BINDIR)/$(EXEC) $(BINDIR)/$(RD_EXEC) $(BINDIR)/$(WR_EXEC)
	cp $(BINDIR)/* $(NFSROOT)

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(BINDIR)/$(EXEC) :$(EXEC:%=$(OBJDIR)/%.o)
	$(CC) $(CFLAGS) -o $@ $<
	#$(STRIP) $@

$(BINDIR)/$(RD_EXEC) :$(RD_EXEC:%=$(OBJDIR)/%.o)
	$(CC) $(CFLAGS) -o $@ $<

$(BINDIR)/$(WR_EXEC) :$(WR_EXEC:%=$(OBJDIR)/%.o)
	$(CC) $(CFLAGS) -o $@ $<

.PHONY : pre_build
pre_build:
	@- test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	@- test -d $(BINDIR) || mkdir -p $(BINDIR)

.PHONY : clean
clean:
	-rm -rf $(OBJDIR) $(BINDIR)
