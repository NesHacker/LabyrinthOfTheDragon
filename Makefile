#
# Simple Makefile that compiles all .c and .s files in the same folder
#

# If you move this project you can change the directory
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
ifndef GBDK_HOME
	GBDK_HOME = ~/gbdk/
endif

# These are used to control the number of banks of each type in the LCC flags
# down below
ROM_BANKS=32
RAM_BANKS=4
CART_TYPE=0x1B

LCC = $(GBDK_HOME)bin/lcc
LCCFLAGS = -Wm-yC -Wm-yt$(CART_TYPE) -Wl-yo$(ROM_BANKS) -Wl-ya$(RAM_BANKS)
PNG2BIN := ./tools/png2bin
TABLES2C := ./tools/tables2c
STRINGS2C := ./tools/strings2c

# GBDK_DEBUG = ON
ifdef GBDK_DEBUG
	LCCFLAGS += -debug -v
endif

# You can set the name of the .gb ROM file here
PROJECTNAME    = TheMonsterManual

BINS = $(PROJECTNAME).gb
CSOURCES := $(wildcard src/*.c)
DATASOURCES := $(wildcard data/*.c)
ASMSOURCES := $(wildcard src/*.s)

TILEPNG := $(wildcard assets/tiles/*.png)
TILEBIN := $(subst assets/,res/,$(patsubst %.png,%.bin,$(TILEPNG)))

# TILEBIN := $(patsubst asset/%.png,res/%.bin,$(TILEPNG))

all: assets $(BINS)

usage:
	~/gbdk/bin/romusage $(BINS)

assets: tables strings $(TILEBIN)

tables: assets/tables.csv
	$(TABLES2C)

strings: assets/strings.js
	$(STRINGS2C)

res/tiles/%.bin: assets/tiles/%.png
	$(PNG2BIN) $< $@

compile.bat: Makefile
	@echo "REM Automatically generated from Makefile" > compile.bat
	@make -sn | sed y/\\//\\\\/ | sed s/mkdir\ \-p/mkdir/ | grep -v make >> compile.bat

# Compile and link all source files in a single call to LCC
$(BINS):	$(CSOURCES) $(ASMSOURCES) $(DATASOURCES)
	$(LCC) $(LCCFLAGS) -o $@ $(CSOURCES) $(ASMSOURCES) $(DATASOURCES)

clean:
	rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm *.noi *.rst
	rm -f res/tiles/*.bin res/tiles/manifest.json
	rm -f res/color_tables/*.bin res/color_tables/manifest.json
	rm -f data/strings_*bank*.c
