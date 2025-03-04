DEFAULT_NET := master0002.bin
EVALFILE ?= $(DEFAULT_NET)
NET_LOCATION := https://github.com/yl25946/vault/raw/main/$(EVALFILE)
_THIS       := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
_ROOT       := $(_THIS)
CXX         := g++
CLEAN       := rm -rf
NOOP		:=:
NULL 	    := /dev/null
TARGET      := spaghet
WARNINGS     = -Wall -Wcast-qual -Wextra -Wshadow -Wdouble-promotion -Wformat=2 -Wnull-dereference -Wlogical-op -Wold-style-cast -Wundef -pedantic
CXXFLAGS    :=  -funroll-loops -O3 -flto -fno-exceptions -std=c++20 -DNDEBUG $(WARNINGS)
NATIVE       = -march=native
AVX2FLAGS    = -DUSE_AVX2 -DUSE_SIMD -mavx2 -mbmi
BMI2FLAGS    = -DUSE_AVX2 -DUSE_SIMD -mavx2 -mbmi -mbmi2
AVX512FLAGS  = -DUSE_AVX512 -DUSE_SIMD -mavx512f -mavx512bw

# engine name
NAME        := spaghet

TMPDIR = .tmp

# Detect Clang
ifeq ($(CXX), clang++)
	CXXFLAGS = -funroll-loops -O3 -flto -fuse-ld=lld -fno-exceptions -std=gnu++2a -DNDEBUG
endif

# Detect Windows
ifeq ($(OS), Windows_NT)
	MKDIR   := mkdir
	CLEAN   := del /S /q
	NOOP 	:= cd.
	NULL	:= NUL
else
ifeq ($(COMP), MINGW)
	MKDIR   := mkdir
else
	MKDIR   := mkdir -p
endif
endif


# Detect Windows
ifeq ($(OS), Windows_NT)
	uname_S  := Windows
	SUFFIX   := .exe
	CXXFLAGS += -static
else
	FLAGS    = -pthread
	SUFFIX  :=
	uname_S := $(shell uname -s)
endif

# Different native flag for macOS
ifeq ($(uname_S), Darwin)
	NATIVE = -mcpu=apple-a14	
	FLAGS = 
endif

ARCH_DETECTED =
PROPERTIES = $(shell echo | $(CXX) -march=native -E -dM -)
ifneq ($(findstring __AVX512F__, $(PROPERTIES)),)
	ifneq ($(findstring __AVX512BW__, $(PROPERTIES)),)
		ARCH_DETECTED = AVX512
	endif
endif
ifeq ($(ARCH_DETECTED),)
	ifneq ($(findstring __BMI2__, $(PROPERTIES)),)
		ARCH_DETECTED = BMI2
	endif
endif
ifeq ($(ARCH_DETECTED),)
	ifneq ($(findstring __AVX2__, $(PROPERTIES)),)
		ARCH_DETECTED = AVX2
	endif
endif

# Remove native for builds
ifdef build
	NATIVE =
else
	ifeq ($(ARCH_DETECTED), AVX512)
		CXXFLAGS += $(AVX512FLAGS)
	endif
	ifeq ($(ARCH_DETECTED), BMI2)
		CXXFLAGS += $(BMI2FLAGS)
	endif
	ifeq ($(ARCH_DETECTED), AVX2)
		CXXFLAGS += $(AVX2FLAGS)
	endif
endif

# SPECIFIC BUILDS
ifeq ($(build), native)
	NATIVE     = -march=native
	ARCH       = -x86-64-native
	ifeq ($(ARCH_DETECTED), AVX512)
		CXXFLAGS += $(AVX512FLAGS)
	endif
	ifeq ($(ARCH_DETECTED), BMI2)
		CXXFLAGS += $(BMI2FLAGS)
	endif
	ifeq ($(ARCH_DETECTED), AVX2)
		CXXFLAGS += $(AVX2FLAGS)
	endif
endif

ifeq ($(build), x86-64)
	NATIVE       = -mtune=znver1
	INSTRUCTIONS = -msse -msse2 -mpopcnt
	ARCH         = -x86-64
endif

ifeq ($(build), x86-64-modern)
	NATIVE       = -mtune=znver2
	INSTRUCTIONS = -m64 -msse -msse3 -mpopcnt
	ARCH         = -x86-64-modern
endif

ifeq ($(build), x86-64-avx2)
	NATIVE    = -march=bdver4 -mno-tbm -mno-sse4a -mno-bmi2
	ARCH      = -x86-64-avx2
	CXXFLAGS += $(AVX2FLAGS)
endif

ifeq ($(build), x86-64-bmi2)
	NATIVE    = -march=haswell
	ARCH      = -x86-64-bmi2
	CXXFLAGS += $(BMI2FLAGS)
endif

ifeq ($(build), x86-64-avx512)
	NATIVE    = -march=x86-64-v4 -mtune=znver4
	ARCH      = -x86-64-avx512
	CXXFLAGS += $(AVX512FLAGS)
endif

ifeq ($(build), debug)
	CXXFLAGS =  -g -fsanitize=undefined,address -fno-omit-frame-pointer -std=gnu++2a
	NATIVE   = -msse -msse3 -mpopcnt
	FLAGS    = -lpthread -lstdc++
	ifeq ($(ARCH_DETECTED), AVX512)
		CXXFLAGS += $(AVX512FLAGS)
	endif
	ifeq ($(ARCH_DETECTED), BMI2)
		CXXFLAGS += $(BMI2FLAGS)
	endif
	ifeq ($(ARCH_DETECTED), AVX2)
		CXXFLAGS += $(AVX2FLAGS)
	endif
endif

# Get what pgo flags we should be using

ifneq ($(findstring gcc, $(CCX)),)
	PGOGEN   = -fprofile-generate
	PGOUSE   = -fprofile-use
endif

ifneq ($(findstring clang, $(CCX)),)
	PGOMERGE = llvm-profdata merge -output=alexandria.profdata *.profraw
	PGOGEN   = -fprofile-instr-generate
	PGOUSE   = -fprofile-instr-use=alexandria.profdata
endif

# Add network name and Evalfile
CXXFLAGS +=-DEVALFILE=\"$(EVALFILE)\"

SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst %.cpp,$(TMPDIR)/%.o,$(SOURCES))
ifeq ($(OS), Windows_NT)
	DEPENDS := $(patsubst %.cpp,$(TMPDIR)\%.d,$(SOURCES))
else
	DEPENDS := $(patsubst %.cpp,$(TMPDIR)//%.d,$(SOURCES))
endif
EXE	    := $(NAME)$(SUFFIX)

all: net $(TARGET) 
clean:
	rm -rf $(TMPDIR) $(EXE)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(NATIVE) -MMD -MP -o $(EXE) $^ $(FLAGS)

$(TMPDIR)/%.o: %.cpp | $(TMPDIR)
	$(CXX) $(CXXFLAGS) $(NATIVE) -MMD -MP -c $< -o $@ $(FLAGS)

$(TMPDIR):
	$(MKDIR) "$(TMPDIR)" "$(TMPDIR)/src"

-include $(DEPENDS)

net: 
	@if [ ! -f $(EVALFILE) ]; then \
    echo "Network file not found, downloading default network..."; \
        wget -O $(EVALFILE) $(NET_LOCATION); \
    fi


# Usual disservin yoink for makefile related stuff
pgo:
	$(CXX) $(CXXFLAGS) $(PGO_GEN) $(NATIVE) $(INSTRUCTIONS) -MMD -MP -o $(EXE) $(SOURCES) $(LDFLAGS)
	./$(EXE) bench
	$(PGO_MERGE)
	$(CXX) $(CXXFLAGS) $(NATIVE) $(INSTRUCTIONS) $(PGO_USE) -MMD -MP -o $(EXE) $(SOURCES) $(LDFLAGS)
	@rm -f *.gcda *.profraw *.o $(DEPENDS) *.d  profdata