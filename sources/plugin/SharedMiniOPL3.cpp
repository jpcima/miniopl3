#include "SharedMiniOPL3.h"

static void InitEnumValues(
    ParameterEnumerationValues &ev,
    std::initializer_list<std::pair<float, const char *>> args)
{
    ParameterEnumerationValue *values = new ParameterEnumerationValue[args.size()];
    ev.values = values;
    ev.count = args.size();
    size_t i = 0;
    for (std::pair<float, const char *> arg : args)
    {
        values[i].value = arg.first;
        values[i].label = arg.second;
        ++i;
    }
}

void InitParameter(uint32_t index, Parameter &parameter)
{
    DISTRHO_SAFE_ASSERT_RETURN(index < paramCount, );

    switch (index) {
    case paramNumChips:
        parameter.name = "Number of chips";
        parameter.symbol = "numchips";
        parameter.ranges = ParameterRanges(2, 1, 8);
        parameter.hints = /*kParameterIsAutomable|*/kParameterIsInteger;
        break;

    case paramDeepVibrato:
        parameter.name = "Deep vibrato";
        parameter.symbol = "deepvibrato";
        parameter.ranges = ParameterRanges(0, 0, 1);
        parameter.hints = kParameterIsAutomable|kParameterIsInteger|kParameterIsBoolean;
        break;

    case paramDeepTremolo:
        parameter.name = "Deep tremolo";
        parameter.symbol = "deeptremolo";
        parameter.ranges = ParameterRanges(0, 0, 1);
        parameter.hints = kParameterIsAutomable|kParameterIsInteger|kParameterIsBoolean;
        break;

    case paramVolumeModel:
        parameter.name = "Volume model";
        parameter.symbol = "volmodel";
        parameter.ranges = ParameterRanges(0, 0, 4);
        InitEnumValues(
            parameter.enumValues,
            {
                {0, "Generic"},
                {1, "Creative CMF"},
                {2, "Doom DMX"},
                {3, "Apogee Sound System"},
                {4, "Windows 9x"},
            });
        parameter.enumValues.restrictedMode = true;
        parameter.hints = kParameterIsAutomable|kParameterIsInteger;
        break;

    case paramAlgorithm:
        parameter.name = "Algorithm";
        parameter.symbol = "algorithm";
        parameter.ranges = ParameterRanges(0, 0, 9);
        InitEnumValues(
            parameter.enumValues,
            {
                {0, "2op [1 + 2]"},
                {1, "2op [1 mod 2]"},
                {2, "4op [1 mod 2 mod 3 mod 4]"},
                {3, "4op [1 + [2 mod 3 mod 4]]"},
                {4, "4op [1 mod 2] + [3 mod 4]"},
                {5, "4op [1 + [2 mod 3] + 4]"},
                {6, "2x2op [1 + 2] + [3 + 4]"},
                {7, "2x2op [1 mod 2] + [3 + 4]"},
                {8, "2x2op [1 + 2] + [3 mod 4]"},
                {9, "2x2op [1 mod 2] + [3 mod 4]"},
            });
        parameter.enumValues.restrictedMode = true;
        parameter.hints = kParameterIsAutomable|kParameterIsInteger;
        break;

   case paramFeedback1:
        parameter.name = "Feedback 1-2";
        parameter.symbol = "feedback1";
        parameter.ranges = ParameterRanges(0, 0, 7);
        parameter.hints = kParameterIsAutomable|kParameterIsInteger;
        break;

   case paramFeedback2:
        parameter.name = "Feedback 3-4 (4op, 2x2op)";
        parameter.symbol = "feedback2";
        parameter.ranges = ParameterRanges(0, 0, 7);
        parameter.hints = kParameterIsAutomable|kParameterIsInteger;
        break;

   case paramTranspose1:
        parameter.name = "Transpose 1-2";
        parameter.symbol = "transpose1";
        parameter.ranges = ParameterRanges(0, -127, 128);
        parameter.hints = kParameterIsAutomable|kParameterIsInteger;
        break;

   case paramTranspose2:
        parameter.name = "Transpose 3-4 (2x2op)";
        parameter.symbol = "transpose2";
        parameter.ranges = ParameterRanges(0, -127, 128);
        parameter.hints = kParameterIsAutomable|kParameterIsInteger;
        break;


   case paramFineTune2:
        parameter.name = "Fine tune 3-4 (2x2op)";
        parameter.symbol = "finetune2";
        parameter.ranges = ParameterRanges(0, -127, 128);
        parameter.hints = kParameterIsAutomable|kParameterIsInteger;
        break;

   case paramVelOffset:
        parameter.name = "Velocity offset";
        parameter.symbol = "veloffset";
        parameter.ranges = ParameterRanges(0, -127, 128);
        parameter.hints = kParameterIsAutomable|kParameterIsInteger;
        break;

#define PER_OP(X) \
    case paramOp##X##Attack: \
        parameter.name = "Operator " #X " attack"; \
        parameter.symbol = "op" #X "attack"; \
        parameter.ranges = ParameterRanges(0, 0, 15); \
        parameter.hints = kParameterIsAutomable|kParameterIsInteger; \
        break; \
    \
    case paramOp##X##Decay: \
        parameter.name = "Operator " #X " decay"; \
        parameter.symbol = "op" #X "decay"; \
        parameter.ranges = ParameterRanges(0, 0, 15); \
        parameter.hints = kParameterIsAutomable|kParameterIsInteger; \
        break; \
    \
    case paramOp##X##Sustain: \
        parameter.name = "Operator " #X " sustain"; \
        parameter.symbol = "op" #X "sustain"; \
        parameter.ranges = ParameterRanges(0, 0, 15); \
        parameter.hints = kParameterIsAutomable|kParameterIsInteger; \
        break; \
    \
    case paramOp##X##Release: \
        parameter.name = "Operator " #X " release"; \
        parameter.symbol = "op" #X "release"; \
        parameter.ranges = ParameterRanges(0, 0, 15); \
        parameter.hints = kParameterIsAutomable|kParameterIsInteger; \
        break; \
    \
    case paramOp##X##Wave: \
        parameter.name = "Operator " #X " waveform"; \
        parameter.symbol = "op" #X "wave"; \
        parameter.ranges = ParameterRanges(0, 0, 7); \
        InitEnumValues( \
            parameter.enumValues, \
            { \
                {0, "Sine"}, \
                {1, "Half sine"}, \
                {2, "Absolute sine"}, \
                {3, "Pulse sine"}, \
                {4, "Alternating sine"}, \
                {5, "Camel sine"}, \
                {6, "Square"}, \
                {7, "Logarithmic sawtooth"}, \
             }); \
        parameter.enumValues.restrictedMode = true; \
        parameter.hints = kParameterIsAutomable|kParameterIsInteger; \
        break; \
    \
    case paramOp##X##Fmul: \
        parameter.name = "Operator " #X " frequency multipler"; \
        parameter.symbol = "op" #X "fmul"; \
        parameter.ranges = ParameterRanges(0, 0, 15); \
        parameter.hints = kParameterIsAutomable|kParameterIsInteger; \
        break; \
    \
    case paramOp##X##Level: \
        parameter.name = "Operator " #X " level"; \
        parameter.symbol = "op" #X "level"; \
        parameter.ranges = ParameterRanges(0, 0, 63); \
        parameter.hints = kParameterIsAutomable|kParameterIsInteger; \
        break; \
    \
    case paramOp##X##KSL: \
        parameter.name = "Operator " #X " key scale level"; \
        parameter.symbol = "op" #X "ksl"; \
        parameter.ranges = ParameterRanges(0, 0, 3); \
        parameter.hints = kParameterIsAutomable|kParameterIsInteger; \
        break; \
    \
    case paramOp##X##Vib: \
        parameter.name = "Operator " #X " vibrato"; \
        parameter.symbol = "op" #X "vib"; \
        parameter.ranges = ParameterRanges(0, 0, 1); \
        parameter.hints = kParameterIsAutomable|kParameterIsInteger|kParameterIsBoolean; \
        break; \
    \
    case paramOp##X##Am: \
        parameter.name = "Operator " #X " tremolo"; \
        parameter.symbol = "op" #X "am"; \
        parameter.ranges = ParameterRanges(0, 0, 1); \
        parameter.hints = kParameterIsAutomable|kParameterIsInteger|kParameterIsBoolean; \
        break; \
    \
    case paramOp##X##Eg: \
        parameter.name = "Operator " #X " sustained"; \
        parameter.symbol = "op" #X "eg"; \
        parameter.ranges = ParameterRanges(0, 0, 1); \
        parameter.hints = kParameterIsAutomable|kParameterIsInteger|kParameterIsBoolean; \
        break; \
    \
    case paramOp##X##KSR: \
        parameter.name = "Operator " #X " key-scaled"; \
        parameter.symbol = "op" #X "ksr"; \
        parameter.ranges = ParameterRanges(0, 0, 1); \
        parameter.hints = kParameterIsAutomable|kParameterIsInteger|kParameterIsBoolean; \
        break; \

    PER_OP(1)
    PER_OP(2)
    PER_OP(3)
    PER_OP(4)

    #undef PER_OP
    }
}
