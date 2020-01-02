#include "bank2preset.h"
#include "../../sources/plugin/SharedMiniOPL3.cpp"
#include "../thirdparty/libADLMIDI/src/wopl/wopl_file.c"
#include <getopt.h>

static writeInstrumentFn gWriteInst = &writeInstrumentAsCpp;
static const char *gLv2UriPrefix = "";
static bool gLv2HeaderWritten = false;
static std::unordered_map<std::string, size_t> gLv2BanksKnown;
static Parameter gParameters[paramCount];

int main(int argc, char *argv[])
{
    for (int c; (c = getopt(argc, argv, "L:M:")) != -1;) {
        switch (c) {
        case 'L':
            gWriteInst = &writeInstrumentAsLv2PresetTtl;
            gLv2UriPrefix = optarg;
            break;
        case 'M':
            gWriteInst = &writeInstrumentAsLv2ManTtl;
            gLv2UriPrefix = optarg;
            break;
        default:
            return 1;
        }
    }

    unsigned numfiles = argc - optind;
    if (numfiles == 0) {
        fprintf(stderr, "No bank file has been specified.\n");
        return 1;
    }

    for (unsigned i = 0; i < paramCount; ++i)
        InitParameter(i, gParameters[i]);

    std::vector<WOPLFile_u> files{numfiles};
    std::vector<std::string> names{numfiles};

    for (unsigned i = 0; i < numfiles; ++i) {
        std::string &name = names[i];
        name = argv[optind + i];

        WOPLFile_u wopl{WOPL_LoadBankFromFile(name.c_str())};
        if (!wopl) {
            fprintf(stderr, "Cannot load the bank file in WOPL format.\n");
            return 1;
        }

        size_t pos = name.rfind('/');
        if (pos != name.npos)
            name = name.substr(pos + 1);
        if (name.size() >= 5 && !memcmp(name.data() + name.size() - 5, ".wopl", 5))
            name.resize(name.size() - 5);

        files[i] = std::move(wopl);
    }

    std::vector<Ins> instlist;
    for (unsigned i = 0; i < numfiles; ++i)
        extractAllInstruments(*files[i], names[i].c_str(), instlist);

    convertInstrumentList(instlist);

    return 0;
}

void convertInstrumentList(const std::vector<Ins> &instlist)
{
    unsigned spec = identifyMidiSpec(instlist);

    for (size_t index = 0, count = instlist.size(); index < count; ++index) {
        const Ins &ins = instlist[index];
        convertInstrument(index, ins, spec);
    }
}

void convertInstrument(unsigned index, const Ins &ins, unsigned spec)
{
    const WOPLFile &file = *ins.file;
    const WOPLInstrument &inst = ins.bank->ins[ins.program_number];

    unsigned algorithm = inst.fb_conn1_C0 & 1;

    if (inst.inst_flags & WOPL_Ins_4op) {
        algorithm |= (inst.fb_conn2_C0 & 1) << 1;
        algorithm += 2;
        if (inst.inst_flags & WOPL_Ins_Pseudo4op)
            algorithm += 4;
    }

    int values[paramCount];
    for (unsigned i = 0; i < paramCount; ++i)
        values[i] = gParameters[i].ranges.def;

    std::string name = inst.inst_name;
    while (!name.empty() && std::isspace((unsigned char)name.back()))
        name.pop_back();

    if (name.empty()) {
        // look it up in MIDI DB
        MidiProgramId id{ins.isdrum, ins.bank->bank_midi_msb, ins.bank->bank_midi_lsb, ins.program_number};
        const MidiProgram *pgm = getMidiProgram(id, spec);
        if (!pgm)
            pgm = getFallbackProgram(id, spec);
        if (pgm)
            name = pgm->patchName;
    }

    values[paramAlgorithm] = algorithm;
    values[paramFeedback1] = inst.fb_conn1_C0 >> 1;
    values[paramFeedback2] = inst.fb_conn2_C0 >> 1;
    values[paramTranspose1] = inst.note_offset1;
    values[paramTranspose2] = inst.note_offset2;
    values[paramFineTune2] = inst.second_voice_detune;
    values[paramVelOffset] = inst.midi_velocity_offset;

    const WOPLOperator *op1234[] = {
        &inst.operators[1],
        &inst.operators[0],
        &inst.operators[3],
        &inst.operators[2],
    };

    for (unsigned o = 0; o < 4; ++o) {
        const WOPLOperator &op = *op1234[o];
        int *opParams = values + o * (paramOp2Attack - paramOp1Attack);
        opParams[paramOp1Attack] = op.atdec_60 >> 4;
        opParams[paramOp1Decay] = op.atdec_60 & 15;
        opParams[paramOp1Sustain] = 15 - (op.susrel_80 >> 4);
        opParams[paramOp1Release] = op.susrel_80 & 15;
        opParams[paramOp1Wave] = op.waveform_E0 & 7;
        opParams[paramOp1Fmul] = op.avekf_20 & 15;
        opParams[paramOp1Level] = 63 - (op.ksl_l_40 & 63);
        opParams[paramOp1KSL] = op.ksl_l_40 >> 6;
        opParams[paramOp1Vib] = (op.avekf_20 >> 6) & 1;
        opParams[paramOp1Am] = (op.avekf_20 >> 7) & 1;
        opParams[paramOp1Eg] = (op.avekf_20 >> 5) & 1;
        opParams[paramOp1KSR] = (op.avekf_20 >> 4) & 1;
    }

    values[paramDeepVibrato] = (file.opl_flags & WOPL_FLAG_DEEP_VIBRATO) != 0;
    values[paramDeepTremolo] = (file.opl_flags & WOPL_FLAG_DEEP_TREMOLO) != 0;
    values[paramVolumeModel] = file.volume_model;

    gWriteInst(index, ins, name.c_str(), values);
}

void writeInstrumentAsCpp(unsigned index, const Ins &ins, const char *name, const int values[])
{
    (void)index;
    (void)ins;

    printf("{\"%s\", {", name);
    for (unsigned i = 0; i < paramCount; ++i) {
        if (i > 0) printf(",");
        printf("%d", values[i]);
    }
    printf("}},\n");
}

void writeInstrumentAsLv2ManTtl(unsigned index, const Ins &ins, const char *name, const int values[])
{
    (void)ins;
    (void)name;
    (void)values;

    if (!gLv2HeaderWritten) {
        printf(
            "@prefix syn:  <%s> ." "\n"
            "\n"
            "@prefix lv2:  <http://lv2plug.in/ns/lv2core#> ." "\n"
            "@prefix pset: <http://lv2plug.in/ns/ext/presets#> ." "\n"
            "@prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#> ." "\n",
            gLv2UriPrefix);
        gLv2HeaderWritten = true;
    }

    printf(
        "\n"
        "syn:preset%04u" "\n"
        "\t" "a pset:Preset ;" "\n"
        "\t" "lv2:appliesTo <%s> ;" "\n"
        "\t" "rdfs:seeAlso <presets.ttl> .\n",
        index, DISTRHO_PLUGIN_URI);
}

void writeInstrumentAsLv2PresetTtl(unsigned index, const Ins &ins, const char *name, const int values[])
{
    (void)ins;

    if (!gLv2HeaderWritten) {
        printf(
            "@prefix syn:  <%s> ." "\n"
            "\n"
            "@prefix lv2:  <http://lv2plug.in/ns/lv2core#> ." "\n"
            "@prefix pset: <http://lv2plug.in/ns/ext/presets#> ." "\n"
            "@prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#> ." "\n",
            gLv2UriPrefix);
        gLv2HeaderWritten = true;
    }

    auto bankInsert = gLv2BanksKnown.insert(
        std::pair<std::string, size_t>{ins.filename, gLv2BanksKnown.size()});
    unsigned bankno = bankInsert.first->second;

    if (bankInsert.second) {
        printf(
            "\n"
            "syn:bank%04u" "\n"
            "\t" "a pset:Bank ;" "\n"
            "\t" "rdfs:label \"\"\"%s\"\"\" ." "\n",
            bankno, ins.filename);
    }

    printf(
        "\n"
        "syn:preset%04u" "\n"
        "\t" "rdfs:label \"\"\"%s\"\"\" ;" "\n"
        "\t" "pset:bank syn:bank%04u ;" "\n",
        index, name, bankno);

    printf(
        "\t" "lv2:port\n");
    for (unsigned i = 0; i < paramCount; ++i) {
        printf(
            "\t" "[\n"
            "\t\t" "lv2:symbol \"\"\"%s\"\"\" ;\n"
            "\t\t" "pset:value %d.0 ;\n"
            "\t" "]",
            gParameters[i].symbol.buffer(), values[i]);

        if (i < paramCount - 1)
            printf(",\n");
        else
            printf(" .\n");
    }
}

void extractAllInstruments(const WOPLFile &file, const char *name, std::vector<Ins> &instlist)
{
    instlist.reserve(instlist.size() +
        128 * (file.banks_count_melodic + file.banks_count_percussion));

    for (unsigned i = 0, n = file.banks_count_melodic; i < n; ++i) {
        for (unsigned j = 0; j < 128; ++j) {
            const WOPLBank &bank = file.banks_melodic[i];
            const WOPLInstrument &inst = bank.ins[j];
            if ((inst.inst_flags & WOPL_Ins_IsBlank) == 0) {
                Ins ins;
                ins.file = &file;
                ins.filename = name;
                ins.bank = &bank;
                ins.program_number = j;
                ins.isdrum = false;
                instlist.push_back(ins);
            }
        }
    }

    for (unsigned i = 0, n = file.banks_count_percussion; i < n; ++i) {
        for (unsigned j = 0; j < 128; ++j) {
            const WOPLBank &bank = file.banks_percussive[i];
            const WOPLInstrument &inst = bank.ins[j];
            if ((inst.inst_flags & WOPL_Ins_IsBlank) == 0) {
                Ins ins;
                ins.file = &file;
                ins.filename = name;
                ins.bank = &bank;
                ins.program_number = j;
                ins.isdrum = true;
                instlist.push_back(ins);
            }
        }
    }
}

unsigned identifyMidiSpec(const std::vector<Ins> &instlist)
{
    unsigned numGS = 0;
    unsigned numXG = 0;

    for (size_t index = 0, count = instlist.size(); index < count; ++index) {
        const Ins &ins = instlist[index];
        MidiProgramId id(ins.isdrum, ins.bank->bank_midi_msb, ins.bank->bank_midi_lsb, ins.program_number);

        unsigned spec = kMidiSpecAny;
        const MidiProgram *bank = getMidiBank(id, spec, &spec);

        if (bank) {
            switch (spec) {
            case kMidiSpecSC:
            case kMidiSpecGS:
                ++numGS;
                break;
            case kMidiSpecXG:
                ++numXG;
                break;
            }
        }
    }

    unsigned spec = kMidiSpecGM1|kMidiSpecGM2;
    if (numGS > numXG)
        spec |= kMidiSpecGS|kMidiSpecSC;
    else if (numXG > numGS)
        spec |= kMidiSpecXG;

    return spec;
}

WOPLFile *WOPL_LoadBankFromFile(const char *filepath)
{
    FILE_u fh{fopen(filepath, "rb")};
    if (!fh)
        return nullptr;

    fseek(fh.get(), 0, SEEK_END);
    off_t size = ftell(fh.get());
    if (size < 0 || size > 32 * 1024 * 1024)
        return nullptr;

    std::unique_ptr<char[]> data{new char[size]};
    rewind(fh.get());

    if (fread(data.get(), size, 1, fh.get()) != 1)
        return nullptr;

    return WOPL_LoadBankFromMem(data.get(), size, nullptr);
}
