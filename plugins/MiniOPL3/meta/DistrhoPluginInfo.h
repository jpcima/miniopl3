/*
 * MiniOPL3 audio effect based on DISTRHO Plugin Framework (DPF)
 *
 * SPDX-License-Identifier: BSL-1.0
 *
 * Copyright (C) 2019 Jean Pierre Cimalando <jp-dev@inbox.ru>
 */

#ifndef DISTRHO_PLUGIN_INFO_H
#define DISTRHO_PLUGIN_INFO_H

#define DISTRHO_PLUGIN_BRAND "JPC"
#define DISTRHO_PLUGIN_NAME  "MiniOPL3"
#define DISTRHO_PLUGIN_URI   "http://jpcima.sdf1.org/lv2/miniopl3"

#define DISTRHO_PLUGIN_HAS_UI        0
#define DISTRHO_UI_USE_NANOVG        0

#define DISTRHO_PLUGIN_IS_RT_SAFE       1
#define DISTRHO_PLUGIN_IS_SYNTH         1
#define DISTRHO_PLUGIN_NUM_INPUTS       0
#define DISTRHO_PLUGIN_NUM_OUTPUTS      2
#define DISTRHO_PLUGIN_WANT_TIMEPOS     0
#define DISTRHO_PLUGIN_WANT_PROGRAMS    1
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT  1
#define DISTRHO_PLUGIN_WANT_MIDI_OUTPUT 0

#endif // DISTRHO_PLUGIN_INFO_H
