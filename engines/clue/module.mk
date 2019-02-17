MODULE := engines/clue

MODULE_OBJS = \
	clue.o \
	detection.o


# This module can be built as a plugin
ifeq ($(CLUE), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk