MODULE := engines/clue

MODULE_OBJS = \
	clue.o \
	detection.o \
	anim.o \
	text.o \
	base/base.o \
	cdrom/cdrom.o \
	data/dataappl.o \
	data/database.o \
	data/datacalc.o \
	data/relation.o \
	dialog/dialog.o \
	dialog/talkappl.o \
	disk/disk.o \
	error/error.o \
	gameplay/gp_app.o \
	gameplay/gp.o \
	gameplay/loadsave.o \
	gameplay/tcreques.o \
	gfx/gfx.o \
	gfx/gfxnch4.o \
	inphdl/inphdl.o \
	landscap/access.o \
	landscap/landscap.o \
	landscap/hardware.o \
	landscap/init.o \
	landscap/raster.o \
	landscap/scroll.o \
	landscap/spot.o \
	living/bob.o \
	living/living.o \
	memory/memory.o \
	organisa/organisa.o \
	planing/graphics.o \
	planing/guards.o \
	planing/io.o \
	planing/main.o \
	planing/planer.o \
	planing/player.o \
	planing/prepare.o \
	planing/support.o \
	planing/system.o \
	planing/sync.o \
	present/present.o \
	present/interac.o \
	scenes/cars.o \
	scenes/dealer.o \
	scenes/done.o \
	scenes/evidence.o \
	scenes/inside.o \
	scenes/invest.o \
	scenes/scenes.o \
	scenes/tools.o \
	sound/buffer.o \
	sound/fx.o \
	sound/newsound.o \
	sound/hsc.o \
	story/story.o

# This module can be built as a plugin
ifeq ($(CLUE), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
