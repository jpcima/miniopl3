#pragma once
#include "DistrhoPlugin.hpp"

#ifndef maybe_unused
#   if defined(__GNUC__)
#       define maybe_unused __attribute__((unused))
#   else
#       define maybe_unused
#   endif
#endif

void InitParameter(uint32_t index, Parameter &parameter);

struct ParameterSimpleRange
{
    float def = 0.0;
    float min = 0.0;
    float max = 1.0;

    ParameterSimpleRange()
    {
    }

    ParameterSimpleRange(const ParameterRanges &other) noexcept
        : def{other.def}, min{other.min}, max{other.max}
    {
    }
};

enum ParameterId
{
    paramNumChips,

    paramDeepVibrato,
    paramDeepTremolo,
    paramVolumeModel,

    paramAlgorithm,
    paramFeedback1,
    paramFeedback2,
    paramTranspose1,
    paramTranspose2,
    paramFineTune2,
    paramVelOffset,

    #define PER_OP(F, X)       \
        F(Attack, X)           \
        F(Decay, X)            \
        F(Sustain, X)          \
        F(Release, X)          \
        F(Wave, X)             \
        F(Fmul, X)             \
        F(Level, X)            \
        F(KSL, X)              \
        F(Vib, X)              \
        F(Am, X)               \
        F(Eg, X)               \
        F(KSR, X)

    #define OP_PARAMETER(P, X) \
        paramOp##X##P,

    PER_OP(OP_PARAMETER, 1)
    PER_OP(OP_PARAMETER, 2)
    PER_OP(OP_PARAMETER, 3)
    PER_OP(OP_PARAMETER, 4)

    #undef PER_OP
    #undef OP_PARAMETER

    paramCount
};

struct Program
{
    const char *name;
    float values[paramCount];
};

maybe_unused static const Program EmbeddedPrograms[] = {
    {
        "Default piano",
        {2,1,1,4,0,4,0,0,0,0,0,15,2,0,4,0,1,48,2,0,0,0,0,15,2,0,7,0,1,57,0,0,0,0,0,0,0,15,0,0,0,63,0,0,0,0,0,0,0,15,0,0,0},
    },
};

maybe_unused static const unsigned programCount =
    sizeof(EmbeddedPrograms) / sizeof(EmbeddedPrograms[0]);
