#
# $Id$
# Copyright 2002,2003,2004,2005,2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Based on:
#   GCC-AVR standard Makefile part 2
#   Volker Oth 1/2000
#
# Author: Bernardo Innocenti <bernie@develer.com>
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

# Select Bourne Again SHell as default make shell
SHELL := bash

# Checker build
ifeq ($(C),1)
CC = $(CHECKER)
CFLAGS += -Wundef -D__x86_64__=1 -D__unix__=1 -D__linux__=1 -D__STDC_VERSION__=199901L
endif

# Initialize $(top_srcdir) with current directory, unless it was already initialized
top_srcdir ?= $(shell pwd)

# Virtual Product: based on target products may be different.
# e.g. Embedded target = hex, s19, bin
#      Hosted = exe
TRG_TGT = $(TRG:%=$(OUTDIR)/%.tgt)

RECURSIVE_TARGETS = all-recursive install-recursive clean-recursive

# The default target
.PHONY: all
all:: all-recursive $(TRG_TGT)

# Generate project documentation
.PHONY: docs
docs:
	$L "Building documentation"
	$Q $(DOXYGEN)

# Generate ctags
.PHONY: tags
tags:
	$L "Rebuilding C tags database"
	$Q ctags -R --exclude=doc

# Run testsuite
.PHONY: check
check:
	$L "Running testsuite"
	$Q ./run_tests.sh

define build_target

ifeq ($$($(1)_EMBEDDED_TGT),1)
#use embedded specific map flags
$(1)_MAP_FLAGS = $$(MAP_FLAGS_EMB)
#In embedded we need s19, hex and bin
$$(OUTDIR)/$(1).tgt : $$(OUTDIR)/$(1).s19 $$(OUTDIR)/$(1).hex $$(OUTDIR)/$(1).bin
else
#use hosted specific map flags
$(1)_MAP_FLAGS = $$(MAP_FLAGS_HOST)
#in hosted application we need only executable file.
$$(OUTDIR)/$(1).tgt : $$(OUTDIR)/$(1)
endif

$(1)_LDFLAGS += $$($(1)_MAP_FLAGS)

ifneq ($$(strip $$($(1)_MCU)),)
# Define all project specific object files
$(1)_CFLAGS    += -mmcu=$$($(1)_MCU)
$(1)_CXXFLAGS  += -mmcu=$$($(1)_MCU)
$(1)_ASFLAGS   += -mmcu=$$($(1)_MCU)
$(1)_CPPAFLAGS += -mmcu=$$($(1)_MCU)
$(1)_LDFLAGS   += -mmcu=$$($(1)_MCU)
endif
ifneq ($$(strip $$($(1)_CPU)),)
# Define all project specific object files
$(1)_CFLAGS    += -mcpu=$$($(1)_CPU)
$(1)_CXXFLAGS  += -mcpu=$$($(1)_CPU)
$(1)_ASFLAGS   += -mcpu=$$($(1)_CPU)
$(1)_CPPAFLAGS += -mcpu=$$($(1)_CPU)
$(1)_LDFLAGS   += -mcpu=$$($(1)_CPU)
endif
ifneq ($$(strip $$($(1)_LDSCRIPT)),)
$(1)_LDFLAGS += -Wl,-T$$($(1)_LDSCRIPT)
endif

$(1)_CC      = $$($(1)_CROSS)$$(CC)
$(1)_CXX     = $$($(1)_CROSS)$$(CXX)
$(1)_AS      = $$($(1)_CROSS)$$(AS)
$(1)_OBJCOPY = $$($(1)_CROSS)$$(OBJCOPY)
$(1)_STRIP   = $$($(1)_CROSS)$$(STRIP)

$(1)_COBJ    = $$(foreach file,$$($(1)_CSRC:%.c=%.o),$$(OBJDIR)/$(1)/$$(file))
$(1)_CXXOBJ  = $$(foreach file,$$($(1)_CXXSRC:%.cpp=%.o),$$(OBJDIR)/$(1)/$$(file))
$(1)_PCOBJ   = $$(foreach file,$$($(1)_PCSRC:%.c=%_P.o),$$(OBJDIR)/$(1)/$$(file))
$(1)_AOBJ    = $$(foreach file,$$($(1)_ASRC:%.s=%.o),$$(OBJDIR)/$(1)/$$(file))
$(1)_CPPAOBJ = $$(foreach file,$$($(1)_CPPASRC:%.S=%.o),$$(OBJDIR)/$(1)/$$(file))
$(1)_OBJ    := $$($(1)_COBJ) $$($(1)_CXXOBJ) $$($(1)_PCOBJ) $$($(1)_AOBJ) $$($(1)_CPPAOBJ)
$(1)_SRC    := $$($(1)_CSRC) $$($(1)_CXXSRC) $$($(1)_PCSRC) $$($(1)_ASRC) $$($(1)_CPPASRC)
OBJ         += $$($(1)_OBJ)

ifneq ($$(strip $$($(1)_CXXSRC)),)
$(1)_LD = $$($(1)_CROSS)$$(LDXX)
else
$(1)_LD = $$($(1)_CROSS)$$(LD)
endif

# Compile: instructions to create assembler and/or object files from C source
$$($(1)_COBJ) : $$(OBJDIR)/$(1)/%.o : %.c
	$L "$(1): Compiling $$< (C)"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$($(1)_CC) -c $$(CFLAGS) $$($(1)_CFLAGS) $$($(1)_CPPFLAGS) $$(CPPFLAGS) $$< -o $$@

# Compile: instructions to create assembler and/or object files from C++ source
$$($(1)_CXXOBJ) : $$(OBJDIR)/$(1)/%.o : %.cpp
	$L "$(1): Compiling $$< (C++)"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$($(1)_CXX) -c $$(CXXFLAGS) $$($(1)_CXXFLAGS) $$($(1)_CPPFLAGS) $$(CPPFLAGS) $$< -o $$@

# Generate assembly sources from C files (debug)
$$(OBJDIR)/$(1)/%.s : %.c
	$L "$(1): Generating asm source $$<"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$($(1)_CC) -S $$(CFLAGS) $$($(1)_CFLAGS) $$($(1)_CPPFLAGS) $$< -o $$@

# Generate special progmem variant of a source file
$$($(1)_PCOBJ) : $$(OBJDIR)/$(1)/%_P.o : %.c
	$L "$(1): Compiling $$< (PROGMEM)"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$($(1)_CC) -c -D_PROGMEM $$(CFLAGS) $$($(1)_CFLAGS) $$($(1)_CPPFLAGS) $$(CPPFLAGS) $$< -o $$@

# Assemble: instructions to create object file from assembler files
$$($(1)_AOBJ): $$(OBJDIR)/$(1)/%.o : %.s
	$L "$(1): Assembling $$<"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$($(1)_AS) -c $$(ASFLAGS) $$($(1)_ASFLAGS) $$< -o $$@

$$($(1)_CPPAOBJ): $$(OBJDIR)/$(1)/%.o : %.S
	$L "$(1): Assembling with CPP $$<"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$($(1)_CC) -c $$(CPPAFLAGS) $$($(1)_CPPAFLAGS) $$($(1)_CPPFLAGS) $$(CPPFLAGS) $$< -o $$@


# Link: instructions to create elf output file from object files
$$(OUTDIR)/$(1).elf $$(OUTDIR)/$(1)_nostrip: bumprev $$($(1)_OBJ) $$($(1)_LDSCRIPT)
	$L "$(1): Linking $$@"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$($(1)_LD) $$($(1)_OBJ) $$(LIB) $$(LDFLAGS) $$($(1)_LDFLAGS) -o $$@

# Strip debug info
$$(OUTDIR)/$(1): $$(OUTDIR)/$(1)_nostrip
	$L "$(1): Generating stripped executable $$@"
	$Q $$($(1)_STRIP) -o $$@ $$^

# Compile and link (program-at-a-time)
$$(OUTDIR)/$(1)_whole.elf: bumprev $$($(1)_SRC) $$($(1)_LDSCRIPT)
	$L "$(1): Compiling and Linking whole program $$@"
	@$$(MKDIR_P) $$(dir $$@)
	$Q $$($(1)_CC) $$($(1)_SRC) $$(CFLAGS) $$($(1)_CFLAGS) $$(LIB) $$(LDFLAGS) $$($(1)_LDFLAGS) -o $$@

# Flash target
# NOTE: we retry in case of failure because the STK500 programmer is crappy
.PHONY: flash_$(1)
flash_$(1): $(OUTDIR)/$(1).s19 flash_$(1)_local
	if ! $(AVRDUDE) $(DPROG) -p $$($(1)_MCU) -U flash:w:$$< ; then \
	     $(AVRDUDE) $(DPROG) -p $$($(1)_MCU) -U flash:w:$$< ; \
	fi
	#avarice --mkII -j usb --erase --program --verify --file images/triface.elf

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

$$(OUTDIR)/$(1).hex: $$(OUTDIR)/$(1).elf
	$$($(1)_OBJCOPY) -O ihex $$< $$@

$$(OUTDIR)/$(1).s19: $$(OUTDIR)/$(1).elf
	$$($(1)_OBJCOPY) -O srec $$< $$@

$$(OUTDIR)/$(1).bin: $$(OUTDIR)/$(1).elf
	$$($(1)_OBJCOPY) -O binary $$< $$@

$$(OUTDIR)/$(1).obj: $$(OUTDIR)/$(1).elf
	$$($(1)_OBJCOPY) -O avrobj $$< $$@

$$(OUTDIR)/$(1).rom: $$(OUTDIR)/$(1).elf
	$$($(1)_OBJCOPY) -O $$(FORMAT) $$< $$@
#	$$($(1)_OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" -O $$(FORMAT) $$< $$(@:.rom=.eep)

endef

# Generate build rules for all targets
$(foreach t,$(TRG),$(eval $(call build_target,$(t))))

# Generate Qt's moc files from headers
# NOTE: moc totally sucks and can generate empty files for some error conditions,
#       leading to puzzling linker errors.  Kill 'em and abort build.
%_moc.cpp: %.h
	$(MOC) -o $@ $<
	if [ ! -s $< ]; then \
		rm -f $@; \
		exit 1; \
	fi


%.cof: %.elf
	$(COFFCONVERT) -O coff-ext-avr $< $@
#	$(COFFCONVERT) -O coff-avr $< $@   # For use with AVRstudio 3

#make instruction to delete created files
cleanall: clean
clean: clean-recursive
	-$(RM_R) $(OBJDIR)
	-$(RM_R) $(OUTDIR)

$(RECURSIVE_TARGETS):
	@target=`echo $@ | sed s/-recursive//`; \
	for dir in $(SUBDIRS); do \
		if [ -e $$dir/configure.in ] || [ -e $$dir/configure.ac ] && [ ! -x $$dir/configure ]; then \
			echo "Running autogen.sh in $$dir..."; \
			( cd $$dir && chmod a+x autogen.sh && ./autogen.sh && rm -f Makefile || exit 1 ); \
		fi; \
		if [ ! -e $$dir/Makefile ]; then \
			if [ -e "$$dir/build-$(ARCH)" ]; then \
				echo "Running build script in $$dir..."; \
				( cd $$dir && chmod a+x build && ./build || exit 1 ); \
			else \
				echo "Running configure in $$dir..."; \
				( cd $$dir && ./configure --prefix=$(PREFIX) || exit 1 ); \
			fi; \
		fi; \
		$(MAKE) -C $$dir $$target || exit 1; \
	done

BUILDREV_H = buildrev.h

ifeq ($(shell [ -e bertos/verstag.c ] && echo yes),yes)
.PHONY: bumprev
bumprev:
	@buildnr=0; \
	if [ -f $(BUILDREV_H) ]; then \
		buildnr=`sed <"$(BUILDREV_H)" -n -e 's/#define VERS_BUILD \([0-9][0-9]*\)/\1/p'`; \
	fi; \
	buildnr=`expr $$buildnr + 1`; \
	buildhost=`hostname | sed -n -e '1h;2,$$H;$${g;s/\n//g;p;}'`; \
	echo "#define VERS_BUILD $$buildnr" >"$(BUILDREV_H)"; \
	echo "#define VERS_HOST  \"$$buildhost\"" >>"$(BUILDREV_H)"; \
	echo "Building revision $$buildnr"
else
.PHONY: bumprev
bumprev:

endif

# Include dependencies
ifneq ($(strip $(OBJ)),)
-include $(OBJ:%.o=%.d)
endif
