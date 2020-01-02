CXX ?= g++
CXXFLAGS ?= -O2 -g
LDFLAGS ?=

CXXFLAGS += -std=c++11
CXXFLAGS += -Wall -Wextra
CXXFLAGS += -MD -MP
CXXFLAGS += -Isources

TARGET_MACHINE := $(shell $(CXX) -dumpmachine)
ifneq (,$(findstring mingw,$(TARGET_MACHINE)))
APP_EXT := .exe
LDFLAGS += -static
endif

CXXFLAGS += -Ithirdparty/OPL3BankEditor/sources
CXXFLAGS += -I../dpf/distrho -I../plugins/MiniOPL3/meta

SOURCES := \
	sources/bank2preset.cpp \
	thirdparty/OPL3BankEditor/sources/ins_names.cpp
OBJS := $(patsubst %.cpp,build/%.o,$(SOURCES))

all: bin/bank2preset$(APP_EXT)

clean:
	rm -rf bin build

bin/bank2preset$(APP_EXT): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(LDFLAGS)

build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY: all clean

-include $(OBJS:%.o=%.d)
