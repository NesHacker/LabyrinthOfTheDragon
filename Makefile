ifndef GBDK_HOME
	GBDK_HOME = ~/gbdk/
endif

PROJECTNAME = LabyrinthOfTheDragon

SRC_DIR = src
DATA_DIR = data
OBJ_DIR = obj
RES_DIR = res

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

BIN = $(PROJECTNAME).gbc
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
DATA_FILES = $(wildcard $(DATA_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES)) \
	$(patsubst $(DATA_DIR)/%.c,$(OBJ_DIR)/%.o,$(DATA_FILES))
MAP_FILES = $(wildcard $(RES_DIR)/maps/%.tilemap)
TILEMAP_FILES = $(wildcard $(RES_DIR)/tilemaps/%.tilemap)
TILEPNG := $(wildcard assets/tiles/*.png)
TILEBIN := $(subst assets/,res/,$(patsubst %.png,%.bin,$(TILEPNG)))

.PHONY: all clean usage assets data

all: assets data $(BIN)

assets: asset_dirs strings tables $(TILEBIN)

asset_dirs:
	mkdir -p res/tiles

tables: assets/tables.csv
	$(TABLES2C)

strings: assets/strings.js
	$(STRINGS2C)

res/tiles/%.bin: assets/tiles/%.png
	$(PNG2BIN) $< $@

$(BIN): $(OBJ_FILES)
	$(LCC) $(LCCFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(LCC) $(LCCFLAGS) -c $< -o $@

data:
	touch $(DATA_DIR)/*.c

$(OBJ_DIR)/%.o: $(DATA_DIR)/%.c | $(OBJ_DIR)
	$(LCC) $(LCCFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

usage:
	~/gbdk/bin/romusage $(BIN)

clean:
	rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm *.noi *.rst
	rm -f res/tiles/*.bin res/tiles/manifest.json
	rm -f res/color_tables/*.bin res/color_tables/manifest.json
	rm -f data/strings_*bank*.c
	rm -f obj/*
	rm -f src/strings.h
	rm -f src/tables.c
