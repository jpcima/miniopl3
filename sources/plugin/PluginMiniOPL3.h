/*
 * MiniOPL3 audio effect based on DISTRHO Plugin Framework (DPF)
 *
 * SPDX-License-Identifier: BSL-1.0
 *
 * Copyright (C) 2019 Jean Pierre Cimalando <jp-dev@inbox.ru>
 */

#ifndef PLUGIN_MINIOPL3_H
#define PLUGIN_MINIOPL3_H

#include "DistrhoPlugin.hpp"
#include <adlmidi.h>
#include <memory>

struct ParameterSimpleRange;

// -----------------------------------------------------------------------

class PluginMiniOPL3 : public Plugin {
public:
    PluginMiniOPL3();

protected:
    // -------------------------------------------------------------------
    // Information

    const char *getLabel() const noexcept override
    {
        return "MiniOPL3";
    }

    const char *getDescription() const override
    {
        return "FM synthesizer with OPL3";
    }

    const char *getMaker() const noexcept override
    {
        return "JPC";
    }

    const char *getHomePage() const override
    {
        return "http://jpcima.sdf1.org/lv2/miniopl3";
    }

    const char *getLicense() const noexcept override
    {
        return "https://spdx.org/licenses/BSL-1.0";
    }

    uint32_t getVersion() const noexcept override
    {
        return d_version(0, 1, 0);
    }

    // Go to:
    //
    // http://service.steinberg.de/databases/plugin.nsf/plugIn
    //
    // Get a proper plugin UID and fill it in here!
    int64_t getUniqueId() const noexcept override
    {
        return d_cconst('m', 'O', 'P', 'L');
    }

    // -------------------------------------------------------------------
    // Init

    void initParameter(uint32_t index, Parameter &parameter) override;

    // -------------------------------------------------------------------
    // Internal data

    float getParameterValue(uint32_t index) const override;
    void setParameterValue(uint32_t index, float value) override;

    // -------------------------------------------------------------------
    // Programs
    void initProgramName(uint32_t index, String &programName) override;
    void loadProgram(uint32_t index) override;

    // -------------------------------------------------------------------
    // States
    void initState(uint32_t index, String &stateKey, String &defaultStateValue) override;
    String getState(const char *key) const override;
    void setState(const char *key, const char *value) override;

    // -------------------------------------------------------------------
    // Optional

    // Optional callback to inform the plugin about a sample rate change.
    void sampleRateChanged(double newSampleRate) override;

    // -------------------------------------------------------------------
    // Process

    void activate() override;

    void run(const float **, float **outputs, uint32_t frames,
             const MidiEvent *midiEvents, uint32_t midiEventCount) override;

    void handleEvent(const MidiEvent &event);

    // -------------------------------------------------------------------

private:
    void updateProgram();
    void updateDeepVibrato();
    void updateDeepTremolo();
    void updateVolumeModel();
    void updateNumChips();
    void updateFourOps();

    ADL_Instrument createInstrumentOfParameters() const;

    // -------------------------------------------------------------------

private:
    std::unique_ptr<int[]> fParams;
    std::unique_ptr<ParameterSimpleRange[]> fRanges;

    struct ADL_delete
    {
        void operator()(ADL_MIDIPlayer *x) const noexcept { adl_close(x); }
    };

    std::unique_ptr<ADL_MIDIPlayer, ADL_delete> fPlayer;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginMiniOPL3)
};

// -----------------------------------------------------------------------

#endif  // #ifndef PLUGIN_MINIOPL3_H
