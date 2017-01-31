CC=gcc
MKDIR = mkdir -p
RM = rm -rf
LN = ln -sf

SRCDIR = $(TOPDIR)/src
TESTSDIR = $(TOPDIR)/tests

VPATH =
VPATH += $(SRCDIR)
VPATH += $(TESTSDIR)

INCPATHS =

include $(TOPDIR)/make/files.mk

INCPATHS += -I $(SRCDIR)/inc
INCPATHS += -I $(TESTSDIR)/inc
