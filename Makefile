CXX ?= g++

SRC_DIR = $(shell pwd)/src
OBJ_DIR = obj

CPP_FILES = $(wildcard $(SRC_DIR)/*.cc)
OBJ_FILES = $(addprefix $(OBJ_DIR)/,$(notdir $(CPP_FILES:.cc=.o)))

LDFLAGS ?= -pthread -flto
CXXFLAGS ?= -Wall -std=c++11 -O3 -march=native -flto -pthread -fno-exceptions

# can we use pext here?
# use it if is supported and no Ryzen1/2
ARCH_INFO = $(shell echo | gcc -march=native -dM -E -)

ifneq ($(findstring __BMI2__, $(ARCH_INFO)), )
	ifeq ($(findstring __znver1, $(ARCH_INFO)), )
		ifeq ($(findstring __znver2, $(ARCH_INFO)), )
			CXXFLAGS += -D_UPEXT_
		endif
	endif
endif

KNL := $(shell uname -s)

ifneq ($(filter MINGW64_NT%,$(KNL)),)
ifneq ($(shell command -v x86_64-w64-mingw32-c++ ), )
	CXX ?= x86_64-w64-mingw32-c++
else
	CXX ?= x86_64-w64-mingw32-c++-posix
endif
endif

LDFLAGS += -static -Wl,--no-as-needed

# Special tuning compilation
tune: CXXFLAGS  = -Wall -std=c++11 -O3 -march=native -flto -pthread -fopenmp -fno-exceptions -D_TUNE_
tune: LDFLAGS += -pthread -flto -fopenmp

EXE = Drofa_dev
TUNE_EXE = Drofa_tune

all: $(EXE) | $(OBJ_DIR)

tune: $(TUNE_EXE) | $(OBJ_DIR)

$(TUNE_EXE): $(OBJ_FILES)
	$(CXX) $(LDFLAGS) -o $@ $^

$(EXE): $(OBJ_FILES)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(TEST_BIN_NAME)
	rm -f $(TUNE_BIN_NAME)
	rm -f $(BIN_NAME)
