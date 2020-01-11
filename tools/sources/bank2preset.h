#pragma once
#include "../thirdparty/libADLMIDI/src/wopl/wopl_file.h"
#include <ins_names.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdio>
#include <cstring>
#include <cctype>

//
struct FILE_deleter { void operator()(FILE *x) const noexcept { fclose(x); } };
typedef std::unique_ptr<FILE, FILE_deleter> FILE_u;

//
struct WOPL_deleter { void operator()(WOPLFile *x) const noexcept { WOPL_Free(x); } };
typedef std::unique_ptr<WOPLFile, WOPL_deleter> WOPLFile_u;

//
struct Ins {
    const WOPLFile *file;
    const char *filename;
    const WOPLBank *bank;
    uint8_t program_number;
    bool isdrum;
};

//
void convertInstrumentList(const std::vector<Ins> &instlist);
void convertInstrument(unsigned index, const Ins &ins, unsigned spec);

//
typedef void (*writeInstrumentFn)(unsigned index, const Ins &ins, const char *name, const int values[]);
void writeInstrumentAsCpp(unsigned index, const Ins &ins, const char *name, const int values[]);
void writeInstrumentAsLv2ManTtl(unsigned index, const Ins &ins, const char *name, const int values[]);
void writeInstrumentAsLv2PresetTtl(unsigned index, const Ins &ins, const char *name, const int values[]);

//
void extractAllInstruments(const WOPLFile &file, const char *name, std::vector<Ins> &instlist);
unsigned identifyMidiSpec(const std::vector<Ins> &instlist);

//
WOPLFile *WOPL_LoadBankFromFile(const char *filepath);
