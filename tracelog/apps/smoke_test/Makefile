#TOPDIR = $(shell readlink -f ../..)
TOPDIR = ../..
PROJECT=$(shell basename `pwd`)

include $(TOPDIR)/make/top.mk

LOCAL_CFILES = main.c

CFLAGS = -g3 -ggdb
CFLAGA += -wall

include $(TOPDIR)/make/rules.mk
