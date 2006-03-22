#
# $Id$
# Copyright 2002, 2003, 2004 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Based on:
#   GCC-AVR standard Makefile part 2
#   Volker Oth 1/2000
#
# Author: Bernardo Innocenti <bernie@develer.com>
#
# $Log$
# Revision 1.1  2006/03/22 09:51:53  bernie
# Add build infrastructure.
#
# Revision 1.39  2005/11/22 12:10:24  batt
# Avoid double build version increment.
#
# Revision 1.38  2005/11/18 13:29:33  batt
# Bumprev now work on linking and not only on make all.
#
# Revision 1.37  2005/03/20 03:59:44  bernie
# Fix link message to display target file name.
#
# Revision 1.36  2004/10/29 17:05:33  customer_pw
# Allow overriding flash_foobar rules.
#
# Revision 1.35  2004/10/20 10:00:14  customer_pw
# Simplify variable
#
# Revision 1.34  2004/10/19 11:06:51  bernie
# Set top_srcdir.
#
# Revision 1.33  2004/10/19 10:56:20  bernie
# More specific logging messages.
#
# Revision 1.32  2004/10/18 14:40:45  customer_pw
# Add fuse var empty check
#
# Revision 1.31  2004/10/15 17:49:27  batt
# Do not verify avr chip after flashing to speed up the programming task.
#
# Revision 1.30  2004/10/09 10:34:16  aleph
# Fix broken linker rule
#
# Revision 1.29  2004/10/08 17:26:50  customer_pw
# No infinite loop in fuse programming rule; Better dependencies for linking.
#
# Revision 1.28  2004/10/03 18:26:52  bernie
# Unparenthesize $Q.
#
# Revision 1.27  2004/09/30 14:49:53  customer_pw
# Add silent build
#
# Revision 1.26  2004/09/28 16:58:52  customer_pw
# Repeat twice eeprom flashing
#
# Revision 1.25  2004/09/27 12:20:13  customer_pw
# Remove annoying flashing loop
#
# Revision 1.24  2004/09/23 17:19:50  customer_pw
# Per-target fuse rules, with retry.
#
# Revision 1.23  2004/09/20 02:49:28  bernie
# Use  for linking.
#
# Revision 1.22  2004/09/14 22:19:09  bernie
# Create missing dirs.
#
# Revision 1.21  2004/08/31 10:25:10  customer_pw
# Remove mainly useless -y write count option of avrdude
#

# Remove all default pattern rules
.SUFFIXES:

# Verbosity
ifeq ($(V),1)
# Verbose build
Q :=
L := @echo >/dev/null
else
# Quiet build
Q := @
L := @echo
endif

# Initialize $(top_srcdir) with current directory, unless it was already initialized
top_srcdir ?= $(shell pwd)

# Products
TRG_ELF = $(TRG:%=$(OUTDIR)/%.elf)
TRG_S19 = $(TRG:%=$(OUTDIR)/%.s19)
TRG_HEX = $(TRG:%=$(OUTDIR)/%.hex)
TRG_BIN = $(TRG:%=$(OUTDIR)/%.bin)
TRG_ROM = $(TRG:%=$(OUTDIR)/%.rom)
TRG_COF = $(TRG:%=$(OUTDIR)/%.cof)

# The default target
.PHONY: all
all:: $(TRG_S19) $(TRG_HEX)

# Generate project documentation
.PHONY: docs
docs:
	$Q $(DOXYGEN)

define build_target

ifneq ($$(strip $$($(1)_MCU)),)
# Define all project specific object files
$(1)_CFLAGS    += -mmcu=$$($(1)_MCU)
$(1)_CXXFLAGS  += -mmcu=$$($(1)_MCU)
$(1)_ASFLAGS   += -mmcu=$$($(1)_MCU)
$(1)_CPPAFLAGS += -mmcu=$$($(1)_MCU)
$(1)_LDFLAGS   += -mmcu=$$($(1)_MCU)
endif
ifneq ($$(strip $$($(1)_LDSCRIPT)),)
$(1)_LDFLAGS += -Wl,-T$$($(1)_LDSCRIPT)
endif

$(1)_COBJ    = $$(foreach file,$$($(1)_CSRC:%.c=%.o),$$(OBJDIR)/$(1)/$$(file))
$(1)_CXXOBJ  = $$(foreach file,$$($(1)_CXXSRC:%.cpp=%.o),$$(OBJDIR)/$(1)/$$(file))
$(1)_PCOBJ   = $$(foreach file,$$($(1)_PCSRC:%.c=%_P.o),$$(OBJDIR)/$(1)/$$(file))
$(1)_AOBJ    = $$(foreach file,$$($(1)_ASRC:%.s=%.o),$$(OBJDIR)/$(1)/$$(file))
$(1)_CPPAOBJ = $$(foreach file,$$($(1)_CPPASRC:%.S=%.o),$$(OBJDIR)/$(1)/$$(file))
$(1)_OBJ    := $$($(1)_COBJ) $$($(1)_CXXOBJ) $$($(1)_PCOBJ) $$($(1)_AOBJ) $$($(1)_CPPAOBJ)
$(1)_SRC    := $$($(1)_CSRC) $$($(1)_CXXSRC) $$($(1)_PCSRC) $$($(1)_ASRC) $$($(1)_CPPASRC)
OBJ         += $$($(1)_OBJ)

# Compile: instructions to create assembler and/or object files from C source
$$($(1)_COBJ) : $$(OBJDIR)/$(1)/%.o : %.c
	$L "$(1): Compiling $$<"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$(CC) -c $$(CFLAGS) $$($(1)_CFLAGS) $$< -o $$@

# Compile: instructions to create assembler and/or object files from C++ source
$$($(1)_CXXOBJ) : $$(OBJDIR)/$(1)/%.o : %.cpp
	$L "$(1): Compiling $$<"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$(CXX) -c $$(CFLAGS) $$($(1)_CFLAGS) $$< -o $$@

# Generate assembly sources from C files (debug)
$$(OBJDIR)/$(1)/%.s : %.c
	$L "$(1): Generating asm source $$<"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$(CC) -S $$(CFLAGS) $$($(1)_CFLAGS) $$< -o $$@

# Generate special progmem variant of a source file
$$($(1)_PCOBJ) : $$(OBJDIR)/$(1)/%_P.o : %.c
	$L "$(1): Compiling $$< (PROGMEM)"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$(CC) -c -D_PROGMEM $$(CFLAGS) $$($(1)_CFLAGS) $$< -o $$@

# Assemble: instructions to create object file from assembler files
$$($(1)_AOBJ): $$(OBJDIR)/$(1)/%.o : %.s
	$L "$(1): Assembling $$<"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$(AS) -c $$(ASFLAGS) $$($(1)_ASFLAGS) $$< -o $$@

$$($(1)_CPPAOBJ): $$(OBJDIR)/$(1)/%.o : %.S
	$L "$(1): Assembling with CPP $$<"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$(CC) -c $$(CPPAFLAGS) $$($(1)_CPPAFLAGS) $$< -o $$@

# Link: instructions to create elf output file from object files
$$(OUTDIR)/$(1).elf: bumprev $$($(1)_OBJ) $$($(1)_LDSCRIPT)
	$L "$(1): Linking $$@"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$(LD) $$($(1)_OBJ) $$(LIB) $$(LDFLAGS) $$($(1)_LDFLAGS) -o $$@

# Compile and link (program-at-a-time)
$$(OUTDIR)/$(1)_whole.elf: bumprev $$($(1)_SRC) $$($(1)_LDSCRIPT)
	$L "$(1): Compiling and Linking whole program $$@"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$(CC) $$($(1)_SRC) $$(CFLAGS) $$($(1)_CFLAGS) $$(LIB) $$(LDFLAGS) $$($(1)_LDFLAGS) -o $$@

# Flash target
# NOTE: we retry in case of failure because the STK500 programmer is crappy
.PHONY: flash_$(1)
flash_$(1): $(OUTDIR)/$(1).s19 flash_$(1)_local
	if ! $(AVRDUDE) $(DPROG) -p $$($(1)_MCU) -U flash:w:$$< ; then \
	     $(AVRDUDE) $(DPROG) -p $$($(1)_MCU) -U flash:w:$$< ; \
	fi

.PHONY: flash_$(1)_local
flash_$(1)_local:

.PHONY: fuses_$(!)
fuses_$(1):
	if [ ! -z "$$($(1)_efuse)" ] ; then \
		if ! $(AVRDUDE) $(DPROG) -p $$($(1)_MCU) -U efuse:w:$$($(1)_efuse):m ; then \
		     $(AVRDUDE) $(DPROG) -p $$($(1)_MCU) -U efuse:w:$$($(1)_efuse):m ; \
		fi \
	fi
	if [ ! -z "$$($(1)_hfuse)" ] ; then \
		if ! $(AVRDUDE) $(DPROG) -p $$($(1)_MCU) -U hfuse:w:$$($(1)_hfuse):m ; then \
		     $(AVRDUDE) $(DPROG) -p $$($(1)_MCU) -U hfuse:w:$$($(1)_hfuse):m ; \
		fi \
	fi
	if [ ! -z "$$($(1)_lfuse)" ] ; then \
		if ! $(AVRDUDE) $(DPROG) -p $$($(1)_MCU) -U lfuse:w:$$($(1)_lfuse):m ; then \
		     $(AVRDUDE) $(DPROG) -p $$($(1)_MCU) -U lfuse:w:$$($(1)_lfuse):m ; \
		fi \
	fi
	if [ ! -z "$$($(1)_lock)" ] ; then \
		if ! $(AVRDUDE) $(DPROG) -p $$($(1)_MCU) -U lock:w:$$($(1)_lock):m ; then \
		     $(AVRDUDE) $(DPROG) -p $$($(1)_MCU) -U lock:w:$$($(1)_lock):m ; \
		fi \
	fi
endef

# Generate build rules for all targets
$(foreach t,$(TRG),$(eval $(call build_target,$(t))))


%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

%.s19: %.elf
	$(OBJCOPY) -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.obj: %.elf
	$(OBJCOPY) -O avrobj $< $@

%.rom: %.elf
	$(OBJCOPY) -O $(FORMAT) $< $@
#	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" -O $(FORMAT) $< $(@:.rom=.eep)

%.cof: %.elf
	$(COFFCONVERT) -O coff-ext-avr $< $@
#	$(COFFCONVERT) -O coff-avr $< $@   # For use with AVRstudio 3

#make instruction to delete created files
clean:
	-$(RM_R) $(OBJDIR)
	-$(RM_R) $(OUTDIR)

BUILDREV_H = buildrev.h

bumprev:
	@buildnr=0; \
	if [ -f $(BUILDREV_H) ]; then \
		buildnr=`sed <"$(BUILDREV_H)" -n -e 's/#define VERS_BUILD \([0-9][0-9]*\)/\1/p'`; \
	fi; \
	buildnr=`expr $$buildnr + 1`; \
	buildhost=`hostname`; \
	echo "#define VERS_BUILD $$buildnr" >"$(BUILDREV_H)"; \
	echo "#define VERS_HOST  \"$$buildhost\"" >>"$(BUILDREV_H)"; \
	echo "Building revision $$buildnr"

# Include dependencies
-include $(OBJ:%.o=%.d)

