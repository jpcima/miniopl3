#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX, Christopher Arndt, and Patrick Desaulniers
#

# --------------------------------------------------------------
# Installation directories

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
LIBDIR ?= $(PREFIX)/lib
DSSI_DIR ?= $(LIBDIR)/dssi
LADSPA_DIR ?= $(LIBDIR)/ladspa
ifneq ($(MACOS_OR_WINDOWS),true)
LV2_DIR ?= $(LIBDIR)/lv2
VST_DIR ?= $(LIBDIR)/vst
endif
ifeq ($(MACOS),true)
LV2_DIR ?= /Library/Audio/Plug-Ins/LV2
VST_DIR ?= /Library/Audio/Plug-Ins/VST
endif
ifeq ($(WINDOWS),true)
LV2_DIR ?= $(COMMONPROGRAMFILES)/LV2
VST_DIR ?= $(COMMONPROGRAMFILES)/VST2
endif

USER_DSSI_DIR ?= $(HOME)/.dssi
USER_LADSPA_DIR ?= $(HOME)/.ladspa
ifneq ($(MACOS_OR_WINDOWS),true)
USER_LV2_DIR ?= $(HOME)/.lv2
USER_VST_DIR ?= $(HOME)/.vst
endif
ifeq ($(MACOS),true)
USER_LV2_DIR ?= $(HOME)/Library/Audio/Plug-Ins/LV2
USER_VST_DIR ?= $(HOME)/Library/Audio/Plug-Ins/VST
endif
ifeq ($(WINDOWS),true)
USER_LV2_DIR ?= $(APPDATA)/LV2
USER_VST_DIR ?= $(APPDATA)/VST
endif

# --------------------------------------------------------------
# Project name, used for binaries

NAME = miniopl3

# --------------------------------------------------------------
# Plugin types to build

BUILD_LV2 ?= true
BUILD_VST2 ?= true
BUILD_JACK ?= false
BUILD_DSSI ?= false
BUILD_LADSPA ?= false

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	sources/plugin/PluginMiniOPL3.cpp \
	sources/plugin/SharedMiniOPL3.cpp \
	thirdparty/libADLMIDI/src/adlmidi.cpp \
	thirdparty/libADLMIDI/src/adlmidi_load.cpp \
	thirdparty/libADLMIDI/src/adlmidi_midiplay.cpp \
	thirdparty/libADLMIDI/src/adlmidi_opl3.cpp \
	thirdparty/libADLMIDI/src/adlmidi_private.cpp \
	thirdparty/libADLMIDI/src/chips/dosbox_opl3.cpp \
	thirdparty/libADLMIDI/src/chips/dosbox/dbopl.cpp \
	thirdparty/libADLMIDI/src/wopl/wopl_file.c

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

BUILD_CXX_FLAGS += -Isources -Imeta
BUILD_CXX_FLAGS += -Ithirdparty/libADLMIDI/include
BUILD_CXX_FLAGS += \
 -DLIBADLMIDI_VISIBILITY= \
 -DADLMIDI_DISABLE_NUKED_EMULATOR \
 -DADLMIDI_DISABLE_OPAL_EMULATOR \
 -DADLMIDI_DISABLE_JAVA_EMULATOR \
 -DDISABLE_EMBEDDED_BANKS \
 -DADLMIDI_DISABLE_MIDI_SEQUENCER \
 -DADLMIDI_DISABLE_CPP_EXTRAS

# --------------------------------------------------------------
# Enable all selected plugin types

ifeq ($(BUILD_LV2),true)
ifeq ($(HAVE_DGL),true)
TARGETS += lv2_sep
else
TARGETS += lv2_dsp
endif
endif

ifeq ($(BUILD_VST2),true)
TARGETS += vst
endif

ifeq ($(BUILD_JACK),true)
ifeq ($(HAVE_JACK),true)
TARGETS += jack
endif
endif

ifeq ($(BUILD_DSSI),true)
ifneq ($(MACOS_OR_WINDOWS),true)
ifeq ($(HAVE_DGL),true)
ifeq ($(HAVE_LIBLO),true)
TARGETS += dssi
endif
endif
endif
endif

ifeq ($(BUILD_LADSPA),true)
TARGETS += ladspa
endif

all: $(TARGETS)

# --------------------------------------------------------------

install: all
ifeq ($(BUILD_DSSI),true)
ifneq ($(MACOS_OR_WINDOWS),true)
ifeq ($(HAVE_DGL),true)
ifeq ($(HAVE_LIBLO),true)
	@install -Dm755 $(TARGET_DIR)/$(NAME)-dssi$(LIB_EXT) $(DESTDIR)$(DSSI_DIR)/$(NAME)-dssi$(LIB_EXT)
endif
endif
endif
endif
ifeq ($(BUILD_LADSPA),true)
	@install -Dm755 $(TARGET_DIR)/$(NAME)-ladspa$(LIB_EXT) $(DESTDIR)$(LADSPA_DIR)/$(NAME)-ladspa$(LIB_EXT)
endif
ifeq ($(BUILD_VST2),true)
	@install -Dm755 $(TARGET_DIR)/$(NAME)-vst$(LIB_EXT) $(DESTDIR)$(VST_DIR)/$(NAME)-vst$(LIB_EXT)
endif
ifeq ($(BUILD_LV2),true)
	@install -dm755 $(DESTDIR)$(LV2_DIR)/$(NAME).lv2 && \
		install -m644 $(TARGET_DIR)/$(NAME).lv2/*.ttl $(DESTDIR)$(LV2_DIR)/$(NAME).lv2 && \
		install -m755 $(TARGET_DIR)/$(NAME).lv2/*.so $(DESTDIR)$(LV2_DIR)/$(NAME).lv2
endif
ifeq ($(BUILD_JACK),true)
ifeq ($(HAVE_JACK),true)
	@install -Dm755 $(TARGET_DIR)/$(NAME)$(APP_EXT) $(DESTDIR)$(BINDIR)/$(NAME)$(APP_EXT)
endif
endif

install-user: all
ifeq ($(BUILD_DSSI),true)
ifneq ($(MACOS_OR_WINDOWS),true)
ifeq ($(HAVE_DGL),true)
ifeq ($(HAVE_LIBLO),true)
	@install -Dm755 $(TARGET_DIR)/$(NAME)-dssi$(LIB_EXT) $(USER_DSSI_DIR)/$(NAME)-dssi$(LIB_EXT)
endif
endif
endif
endif
ifeq ($(BUILD_LADSPA),true)
	@install -Dm755 $(TARGET_DIR)/$(NAME)-ladspa$(LIB_EXT) $(USER_LADSPA_DIR)/$(NAME)-ladspa$(LIB_EXT)
endif
ifeq ($(BUILD_VST2),true)
	@install -Dm755 $(TARGET_DIR)/$(NAME)-vst$(LIB_EXT) $(USER_VST_DIR)/$(NAME)-vst$(LIB_EXT)
endif
ifeq ($(BUILD_LV2),true)
	@install -dm755 $(USER_LV2_DIR)/$(NAME).lv2 && \
		install -m644 $(TARGET_DIR)/$(NAME).lv2/*.ttl $(USER_LV2_DIR)/$(NAME).lv2 && \
		install -m755 $(TARGET_DIR)/$(NAME).lv2/*.so $(USER_LV2_DIR)/$(NAME).lv2
endif
ifeq ($(BUILD_JACK),true)
ifeq ($(HAVE_JACK),true)
	@install -Dm755 $(TARGET_DIR)/$(NAME)$(APP_EXT) $(HOME)/bin/$(NAME)$(APP_EXT)
endif
endif

# --------------------------------------------------------------

PRESET_URI_PREFIX := http://jpcima.sdf1.org/lv2/miniopl3-presets\#
PRESET_BANKS := \
	"thirdparty/banks/2op by The Fat Man.wopl" \
	"thirdparty/banks/4op by The Fat Man.wopl" \
	"thirdparty/banks/DMXOPL3 by Sneakernets.wopl"

presets:
	make -C tools
	install -d presets/miniopl3-presets.lv2
	tools/bin/bank2preset$(APP_EXT) -M "$(PRESET_URI_PREFIX)" > presets/miniopl3-presets.lv2/manifest.ttl $(PRESET_BANKS)
	tools/bin/bank2preset$(APP_EXT) -L "$(PRESET_URI_PREFIX)" > presets/miniopl3-presets.lv2/presets.ttl $(PRESET_BANKS)

clean-presets:
	rm -rf presets

install-presets:
ifeq ($(BUILD_LV2),true)
	@install -dm755 $(DESTDIR)$(LV2_DIR)/miniopl3-presets.lv2 && \
		install -m644 presets/miniopl3-presets.lv2/*.ttl $(DESTDIR)$(LV2_DIR)/miniopl3-presets.lv2
endif

install-user-presets:
ifeq ($(BUILD_LV2),true)
	@install -dm755 $(USER_LV2_DIR)/miniopl3-presets.lv2 && \
		install -m644 presets/miniopl3-presets.lv2/*.ttl $(USER_LV2_DIR)/miniopl3-presets.lv2
endif

all: presets
clean: clean-presets
install: install-presets
install-user: install-user-presets

.PHONY: presets clean-presets install-presets install-user-presets

# --------------------------------------------------------------

.PHONY: all install install-user
