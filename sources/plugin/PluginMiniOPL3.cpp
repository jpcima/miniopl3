/*
 * MiniOPL3 audio effect based on DISTRHO Plugin Framework (DPF)
 *
 * SPDX-License-Identifier: BSL-1.0
 *
 * Copyright (C) 2019 Jean Pierre Cimalando <jp-dev@inbox.ru>
 */

#include "PluginMiniOPL3.h"
#include "SharedMiniOPL3.h"
#include <cmath>

// -----------------------------------------------------------------------

PluginMiniOPL3::PluginMiniOPL3()
    : Plugin(paramCount, programCount, 0),
      fParams{new int[paramCount]{}},
      fRanges{new ParameterSimpleRange[paramCount]}
{
    sampleRateChanged(getSampleRate());

    for (unsigned index = 0; index < paramCount; ++index) {
        Parameter param;
        InitParameter(index, param);
        fRanges[index] = param.ranges;
        fParams[index] = param.ranges.def;
    }

    for (unsigned index = 0; index < paramCount; ++index)
        setParameterValue(index, fRanges[index].def);
}

// -----------------------------------------------------------------------
// Init

void PluginMiniOPL3::initParameter(uint32_t index, Parameter &parameter)
{
    InitParameter(index, parameter);
}

// -----------------------------------------------------------------------
// Programs

void PluginMiniOPL3::initProgramName(uint32_t index, String &programName)
{
    DISTRHO_SAFE_ASSERT_RETURN(index < programCount, );

    programName = EmbeddedPrograms[index].name;
}

void PluginMiniOPL3::loadProgram(uint32_t index)
{
    DISTRHO_SAFE_ASSERT_RETURN(index < programCount, );

    for (unsigned p = 0; p < paramCount; ++p)
        setParameterValue(p, EmbeddedPrograms[index].values[p]);
}

// -----------------------------------------------------------------------
// Internal data

/**
  Optional callback to inform the plugin about a sample rate change.
*/
void PluginMiniOPL3::sampleRateChanged(double newSampleRate)
{
    ADL_MIDIPlayer *player = adl_init(newSampleRate);
    fPlayer.reset(player);

    updateProgram();
    updateDeepVibrato();
    updateDeepTremolo();
    updateVolumeModel();
    updateNumChips();
}

/**
  Get the current value of a parameter.
*/
float PluginMiniOPL3::getParameterValue(uint32_t index) const
{
    DISTRHO_SAFE_ASSERT_RETURN(index < paramCount, 0);

    return fParams[index];
}

/**
  Change a parameter value.
*/
void PluginMiniOPL3::setParameterValue(uint32_t index, float floatingPointValue)
{
    DISTRHO_SAFE_ASSERT_RETURN(index < paramCount, );

    int value = (int)std::lrint(floatingPointValue);

    ParameterSimpleRange range = fRanges[index];
    value = (value < range.min) ? range.min : value;
    value = (value > range.max) ? range.max : value;

    fParams[index] = value;

    switch (index) {
    default:
        updateProgram();
        break;

    case paramDeepVibrato:
        updateDeepVibrato();
        break;

    case paramDeepTremolo:
        updateDeepTremolo();
        break;

    case paramVolumeModel:
        updateVolumeModel();
        break;

    case paramNumChips:
        updateNumChips();
        break;
    }
}

// -----------------------------------------------------------------------
// Process

void PluginMiniOPL3::activate()
{
    ADL_MIDIPlayer *player = fPlayer.get();

    adl_reset(player);
}


void PluginMiniOPL3::run(const float **inputs, float **outputs, uint32_t frames,
                         const MidiEvent *midiEvents, uint32_t midiEventCount)
{
    ADL_MIDIPlayer *player = fPlayer.get();

    (void)inputs;

    ADLMIDI_AudioFormat format;
    format.type = ADLMIDI_SampleType_F32;
    format.containerSize = sizeof(float);
    format.sampleOffset = sizeof(float);

    //
    float *lOut = outputs[0];
    float *rOut = outputs[1];

    constexpr uint32_t midiInterval = 64;
    uint32_t midiIndex = 0;

    for (uint32_t index = 0; index < frames;) {
        unsigned currentFrames = frames - index;
        if (currentFrames > midiInterval)
            currentFrames = midiInterval;

        while (midiIndex < midiEventCount && midiEvents[midiIndex].frame < index + currentFrames)
            handleEvent(midiEvents[midiIndex++]);

        adl_generateFormat(
            player, 2 * currentFrames,
            (uint8_t *)(lOut + index),
            (uint8_t *)(rOut + index),
            &format);

        // it's too quiet, give it a +6 dB
        float boost = 2.0f;
        for (unsigned i = 0; i < currentFrames; ++i) {
            lOut[i + index] *= boost;
            rOut[i + index] *= boost;
        }

        index += currentFrames;
    }

    while (midiIndex < midiEventCount)
        handleEvent(midiEvents[midiIndex++]);
}

void PluginMiniOPL3::handleEvent(const MidiEvent &event)
{
    ADL_MIDIPlayer *player = fPlayer.get();

    if (event.size >= 4)
        return;

    uint8_t status = event.data[0];
    if (status == 0xff) {
        adl_reset(player);
        return;
    }
    if ((status & 0xf0) == 0xf0)
        return;

    uint8_t d1 = event.data[1] & 0x7f;
    uint8_t d2 = event.data[2] & 0x7f;

    switch (status >> 4) {
    case 0b1001:
        if (d2 != 0) {
            adl_rt_noteOn(player, 0, d1, d2);
            break;
        }
        /* fall through */
    case 0b1000:
        adl_rt_noteOff(player, 0, d1);
        break;
    case 0b1010:
        adl_rt_noteAfterTouch(player, 0, d1, d2);
        break;
    case 0b1101:
        adl_rt_channelAfterTouch(player, 0, d1);
        break;
    case 0b1011:
        if (d1 == 0 || d1 == 32)
            break; // forbid Bank Select CCs
        adl_rt_controllerChange(player, 0, d1, d2);
        break;
    case 0b1110:
        adl_rt_pitchBendML(player, 0, d2, d1);
        break;

    // NO program change
    }
}

// -----------------------------------------------------------------------

void PluginMiniOPL3::updateProgram()
{
    ADL_MIDIPlayer *player = fPlayer.get();

    ADL_Instrument inst = createInstrumentOfParameters();

    //
    ADL_BankId defaultBankId = {0, 0, 0};
    ADL_Bank defaultBank = {};
    adl_getBank(player, &defaultBankId, ADLMIDI_Bank_Create, &defaultBank);
    adl_setInstrument(player, &defaultBank, 0, &inst);

    //
    updateFourOps();
}

void PluginMiniOPL3::updateDeepVibrato()
{
    ADL_MIDIPlayer *player = fPlayer.get();
    adl_setHVibrato(player, fParams[paramDeepVibrato]);
}

void PluginMiniOPL3::updateDeepTremolo()
{
    ADL_MIDIPlayer *player = fPlayer.get();
    adl_setHTremolo(player, fParams[paramDeepTremolo]);
}

void PluginMiniOPL3::updateVolumeModel()
{
    ADL_MIDIPlayer *player = fPlayer.get();
    int model = ADLMIDI_VolumeModel_Generic + fParams[paramVolumeModel];
    adl_setVolumeRangeModel(player, model);
}

void PluginMiniOPL3::updateNumChips()
{
    ADL_MIDIPlayer *player = fPlayer.get();

    unsigned numchips = fParams[paramNumChips];
    adl_setNumChips(player, numchips);

    //
    updateFourOps();
}

void PluginMiniOPL3::updateFourOps()
{
    ADL_MIDIPlayer *player = fPlayer.get();

    unsigned num4ops = 0;
    unsigned numchips = fParams[paramNumChips];
    if (fParams[paramAlgorithm] >= 2)
        num4ops = 6 * numchips;
    adl_setNumFourOpsChn(player, num4ops);
}

ADL_Instrument PluginMiniOPL3::createInstrumentOfParameters() const
{
    ADL_Instrument inst = {};

    inst.version = ADLMIDI_InstrumentVersion;

    inst.note_offset1 = fParams[paramTranspose1];
    inst.note_offset2 = fParams[paramTranspose2];
    inst.midi_velocity_offset = fParams[paramVelOffset];
    inst.second_voice_detune = fParams[paramFineTune2];

    inst.fb_conn1_C0 = fParams[paramFeedback1] << 1;
    inst.fb_conn2_C0 = fParams[paramFeedback2] << 1;

    unsigned alg = fParams[paramAlgorithm];
    if (alg < 2) {
        inst.fb_conn1_C0 |= alg;
        inst.inst_flags |= ADLMIDI_Ins_2op;
    }
    else {
        unsigned alg4 = alg - 2;
        inst.fb_conn1_C0 |= alg4 & 1;
        inst.fb_conn2_C0 |= (alg4 >> 1) & 1;
        inst.inst_flags |= ADLMIDI_Ins_4op;
        if (alg4 >= 4)
            inst.inst_flags |= ADLMIDI_Ins_Pseudo4op;
    }

    ADL_Operator *op1234[] = {
        &inst.operators[1],
        &inst.operators[0],
        &inst.operators[3],
        &inst.operators[2],
    };

    for (unsigned o = 0; o < 4; ++o) {
        ADL_Operator &op = *op1234[o];
        int *opParams = fParams.get() + o * (paramOp2Attack - paramOp1Attack);
        op.avekf_20 =
            (opParams[paramOp1Am] << 7) |
            (opParams[paramOp1Vib] << 6) |
            (opParams[paramOp1Eg] << 5) |
            (opParams[paramOp1KSR] << 4) |
            opParams[paramOp1Fmul];
        op.ksl_l_40 = (opParams[paramOp1KSL] << 6) | (63 - opParams[paramOp1Level]);
        op.atdec_60 = (opParams[paramOp1Attack] << 4) | opParams[paramOp1Decay];
        op.susrel_80 = ((15 - opParams[paramOp1Sustain]) << 4) | opParams[paramOp1Release];
        op.waveform_E0 = opParams[paramOp1Wave];
    }

    // XXX: a hack to skip measuring the envelope times
    inst.delay_off_ms = 65535;
    inst.delay_on_ms = 65535;

    return inst;
}

// -----------------------------------------------------------------------

Plugin *DISTRHO::createPlugin()
{
    return new PluginMiniOPL3;
}
