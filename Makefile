# Lunar: Silver Star Story Complete — matching decompilation build
#
# Compiles the C under decomp/<serial>/src, assembles what is still
# assembly (byte ranges of your own copy of the program: decomp/<serial>/asm
# holds labels and `.incbin` lines, never the game's bytes), links with the
# tree's linker script, and byte-compares against the original program
# image. `make` must always end with OK — a mismatch means a change broke
# the binary. BUILD.md has the one-time setup.
#
#   make                                   the game's main EXE (SLUS_006.28)
#   make TARGET_SERIAL=SLUS_006.28-MENU    its movie-player overlay
#   make TARGET_SERIAL=SLUS_009.21         the bonus disc
#   make programs                          all three, one after another
#   make CODE=sdk [programs]               the same, with the routines Sony's
#                                          SDK holds read from it, not the disc
#   make sources [CODE=sdk]                where the last build's bytes came from
#
# One PROGRAM, one serial, two trees keyed by it: the sources it is
# decompiled from and the objects make writes. The DEFAULT serial is
# declared in TWO places and they must agree:
#
#     the fallback below             -> what make BUILDS
#     recipe/_paths.DEFAULT_TARGET   -> what the build scripts READ
#
# and a third consumer, $(CFG_DIR)/program.ld, names the object paths
# literally. A disagreement does not fail loudly — it splits build output
# across two trees and surfaces much later as "program.map missing".
#
# TARGET_SERIAL retargets make at another program's tree IN STEP with the
# scripts — _paths.py reads the same variable.
SERIAL    := $(if $(TARGET_SERIAL),$(TARGET_SERIAL),SLUS_006.28)
PROGRAMS  := SLUS_006.28 SLUS_006.28-MENU SLUS_009.21
DECOMP    := decomp/$(SERIAL)

# WHERE THE BYTES THAT ARE NOT C COME FROM. Your disc's program image is the
# source that is always there: decomp/<serial>/asm/ names byte ranges of it.
# CODE lists other sources to look in FIRST, each a folder
# decomp/<serial>/from-<source>/ that holds the same paths as asm/ for the
# routines it can supply; a routine it does not have still comes from the
# disc. A tree with no such folder builds as if CODE were empty.
#
#   sdk   Sony's PSY-Q 4.5 library files (recipe/toolchain.sh sdk; BUILD.md)
#
# Every choice must end with the same OK line: the sources hold the same
# bytes, and the build is what proves it. Each choice builds in a folder of
# its own, so objects made from one source are never linked into another.
CODE      ?=
KNOWN_SOURCES := sdk
ifneq ($(filter-out $(KNOWN_SOURCES),$(CODE)),)
$(error CODE=$(CODE): unknown source (known: $(KNOWN_SOURCES)))
endif
empty     :=
space     := $(empty) $(empty)
VARIANT   := $(if $(strip $(CODE)),-$(subst $(space),-,$(strip $(CODE))),)
BUILD_DIR := build/$(SERIAL)$(VARIANT)
SOURCE_DIRS := $(wildcard $(foreach s,$(CODE),$(DECOMP)/from-$(s)))
export BUILD_DIR
SRC_DIR   := $(DECOMP)/src
ASM_DIR   := $(DECOMP)/asm
CFG_DIR   := $(DECOMP)/config
LD_SCRIPT := $(CFG_DIR)/program.ld
TARGET    := $(DECOMP)/expected/code.bin

# The toolchain is not in git: recipe/toolchain.sh puts it in toolchain/.
# TOOLCHAIN_DIR names one kept elsewhere (the build scripts read it too).
TOOLCHAIN := $(if $(TOOLCHAIN_DIR),$(TOOLCHAIN_DIR),toolchain)
# Sony's SDK libraries, for CODE=sdk only. PSYQ_SDK names a copy of your own.
SDK_DIR   := $(if $(PSYQ_SDK),$(PSYQ_SDK),$(TOOLCHAIN)/psyq-4.5)
SDK_USED  := $(if $(filter sdk,$(CODE)),$(if $(wildcard $(DECOMP)/from-sdk),yes,),)
CROSS   := $(TOOLCHAIN)/binutils/bin/mipsel-linux-gnu-
AS      := $(CROSS)as
LD      := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy
PYTHON  := python3

# -G8 matches the original toolchain: cc1 emits `.extern sym, size` and GAS
# expands symbolic loads of small externs to %gp_rel — required for matching
# sdata access. Hand-written asm is unaffected (its gp refs are explicit).
# -I $(DECOMP) is how the asm finds `expected/code.bin` and `asm/...`. The
# folders of CODE come before it, so a stub they hold is found first; the
# SDK's own folder is how their stubs find `lib/...`.
ASFLAGS := -EL -march=r3000 -mabi=32 -G8 --no-pad-sections \
           -I $(DECOMP)/include $(addprefix -I ,$(SOURCE_DIRS)) -I $(DECOMP) \
           $(if $(SDK_USED),-I $(SDK_DIR))

C_FILES   := $(wildcard $(SRC_DIR)/*.c)
C_OBJS    := $(patsubst $(DECOMP)/%.c,$(BUILD_DIR)/%.c.o,$(C_FILES))
DATA_S    := $(wildcard $(ASM_DIR)/data/*.s)
DATA_OBJS := $(patsubst $(DECOMP)/%.s,$(BUILD_DIR)/%.s.o,$(DATA_S))

all: check

check: $(BUILD_DIR)/program.bin
	$(PYTHON) recipe/check.py $< $(TARGET)

# every program this repository holds; stops at the first that fails
programs:
	@set -e; for s in $(PROGRAMS); do \
	    echo "== $$s"; $(MAKE) --no-print-directory TARGET_SERIAL=$$s check; \
	done

# The program image is yours to supply: it is the one input this
# repository does not and will not contain.
$(TARGET):
	@test -f $@ || { echo "$@ is missing." >&2; \
	  echo "  Make it from your own disc:  python3 recipe/extract.py <disc image>" >&2; \
	  echo "  (BUILD.md, \"The program images\")" >&2; exit 1; }

# Sony's libraries are not in this repository either.
SDK_STAMP := $(SDK_DIR)/lib/libapi.lib
$(SDK_STAMP):
	@test -f $@ || { echo "CODE=sdk needs Sony's SDK libraries, and $(SDK_DIR) does not hold them." >&2; \
	  echo "  Fetch them:  recipe/toolchain.sh sdk     (BUILD.md, \"A second source for the routines\")" >&2; exit 1; }
SOURCE_DEPS := $(if $(SDK_USED),$(SDK_STAMP))

# where the bytes of the last build came from, by source
sources:
	@$(PYTHON) recipe/sources.py $(BUILD_DIR) $(DECOMP)

# how much of each program is decompiled: builds all three, then writes the
# README's overview, docs/progress.md and docs/progress-history.csv
status:
	@$(if $(CODE),echo "make status measures the default builds: run it without CODE=" >&2; exit 1)
	@$(MAKE) --no-print-directory programs
	@$(PYTHON) recipe/status.py

$(BUILD_DIR)/program.bin: $(BUILD_DIR)/program.elf
	$(OBJCOPY) -O binary $< $@

# every -T script is a prerequisite: editing one must relink, or the
# "make green" after it proves nothing
LD_EXTRA := $(CFG_DIR)/undefined_funcs_auto.txt \
            $(CFG_DIR)/undefined_syms_auto.txt \
            $(CFG_DIR)/label_fixups.txt $(CFG_DIR)/alias_syms.txt

# The linker script names its objects by path, under build/<serial>/. A
# build in another folder links from a copy with that prefix changed.
$(BUILD_DIR)/program.ld: $(LD_SCRIPT)
	@mkdir -p $(dir $@)
	sed 's#build/$(SERIAL)/#$(BUILD_DIR)/#g' $< > $@

$(BUILD_DIR)/program.elf: $(C_OBJS) $(DATA_OBJS) $(BUILD_DIR)/program.ld $(LD_EXTRA)
	$(LD) -EL -T $(BUILD_DIR)/program.ld $(addprefix -T ,$(LD_EXTRA)) \
	      -Map $(BUILD_DIR)/program.map -o $@

# Everything compile.py READS is a prerequisite, for the same reason
# LD_EXTRA is one. The .d files only carry assembler deps (.s includes),
# so without this list a config edit rebuilds nothing and an incremental
# `make` stays green over a changed image.
#   cc_flags.txt    per-file cc1 flags (the -fno-delayed-branch gate)
#   far_syms.txt    which externs get their `.extern sym,size` stripped
#   program.ld      TU membership / ordering, read for scope decisions
#   symbol_addrs.txt gp-window checks
#   compile.py + delayfill.py  the codegen path itself
#   include/*.h     the .d files are written by GAS (--MD), so they carry
#                   no cpp deps: editing psyq_types.h would rebuild nothing
#   maspsx.py       the codegen post-pass compile.py shells out to
#   per-function lists (wildcard: not every program has them) — each read
#   by compile.py exactly like far_syms.txt
CC_DEPS := $(CFG_DIR)/cc_flags.txt $(CFG_DIR)/far_syms.txt \
           $(LD_SCRIPT) $(CFG_DIR)/symbol_addrs.txt \
           $(wildcard $(CFG_DIR)/epilogue_load_slot.txt) \
           $(wildcard $(CFG_DIR)/return_slot_fill.txt) \
           $(wildcard $(CFG_DIR)/no_delayed_branch.txt) \
           $(wildcard $(CFG_DIR)/zero_remat.txt) \
           $(wildcard $(CFG_DIR)/arg_slot_fill.txt) \
           $(wildcard $(CFG_DIR)/load_hazard_nop.txt) \
           $(wildcard $(CFG_DIR)/cc272.txt) \
           recipe/compile.py recipe/delayfill.py recipe/_paths.py \
           $(wildcard $(DECOMP)/include/*.h) \
           $(TOOLCHAIN)/maspsx/maspsx.py

# Pure INCLUDE_ASM stubs are translated straight to .include directives;
# files with real C go through cpp | cc1 (GCC 2.8.1 PSX) | maspsx.
# recipe/compile.py picks the path. $(TARGET) is a prerequisite of every
# object because any of them may `.incbin` a range of it.
$(BUILD_DIR)/%.c.o: $(DECOMP)/%.c $(CC_DEPS) $(TARGET) $(SOURCE_DEPS)
	@mkdir -p $(dir $@)
	$(PYTHON) recipe/compile.py $< $(@:.o=.s)
	$(AS) $(ASFLAGS) --MD $@.d -o $@ $(@:.o=.s)
	@$(PYTHON) recipe/fix_depfile.py $@.d

$(BUILD_DIR)/%.s.o: $(DECOMP)/%.s $(TARGET) $(SOURCE_DEPS)
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) --MD $@.d -o $@ $<
	@$(PYTHON) recipe/fix_depfile.py $@.d

clean:
	rm -rf $(BUILD_DIR)

-include $(C_OBJS:.o=.o.d) $(DATA_OBJS:.o=.o.d)

.PHONY: all check programs sources status clean
