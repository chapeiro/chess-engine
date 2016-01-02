DEBUGFLAGS= -g3 -pg -ggdb 
# DEBUGFLAGS+= -funsafe-loop-optimizations
# DEBUGFLAGS+= -Wunsafe-loop-optimizations

CXX = g++-4.9

TFLAGS= -O3 
TFLAGS+= -finline 
TFLAGS+= -funit-at-a-time
TFLAGS+= -march=native
TFLAGS+= -fmerge-all-constants
TFLAGS+= -fmodulo-sched
TFLAGS+= -fmodulo-sched-allow-regmoves
#mstackrealign
TFLAGS+= -funsafe-loop-optimizations
TFLAGS+= -Wunsafe-loop-optimizations
TFLAGS+= -fsched-pressure
TFLAGS+= -fipa-pta
TFLAGS+= -fipa-matrix-reorg
TFLAGS+= -ftree-loop-distribution
TFLAGS+= -ftracer
TFLAGS+= -funroll-loops
TFLAGS+= -fwhole-program
TFLAGS+= -flto
# TFLAGS+= -g3 -pg -ggdb 

# DEBUGFLAGS+=$(TFLAGS)
OPTFLAGS= $(TFLAGS)
OPTFLAGS+= -DNDEBUG

INCLUDE_PATH=-I. 

CXXFLAGS= -Wl,--no-as-needed -lpthread -pthread -std=c++14 -Wall -D_GNU_SOURCE -fdiagnostics-color

CXXFLAGS+= $(INCLUDE_PATH)

DBG_DIR=debug
RLS_DIR=release

BIN_ROOT=bin
OBJ_ROOT=obj
SRC_ROOT=src
DEP_ROOT=.depend

BIN_DBG=$(BIN_ROOT)/$(DBG_DIR)/
BIN_RLS=$(BIN_ROOT)/$(RLS_DIR)/

OBJ_DBG=$(OBJ_ROOT)/$(DBG_DIR)/
OBJ_RLS=$(OBJ_ROOT)/$(RLS_DIR)/

DEP_DBG=$(DEP_ROOT)/$(DBG_DIR)/
DEP_RLS=$(DEP_ROOT)/$(RLS_DIR)/

SED_ODD=$(subst /,\/,$(OBJ_DBG))
SED_ORD=$(subst /,\/,$(OBJ_RLS))

SED_DDD=$(subst /,\/,$(DEP_DBG))
SED_DRD=$(subst /,\/,$(DEP_RLS))

CXX_SOURCESD= $(shell find $(SRC_ROOT) -name "*.cpp")
CXX_SOURCES= $(subst $(SRC_ROOT)/,,$(CXX_SOURCESD))
CXX_OBJECTS= $(CXX_SOURCES:.cpp=.o)

OBJ_FILES:=$(addprefix $(OBJ_DBG), $(CXX_OBJECTS)) $(addprefix $(OBJ_RLS), $(CXX_OBJECTS))

all: debug release

debug:CXXFLAGS+= $(DEBUGFLAGS) $(PROFFLAGS)
release:CXXFLAGS+= $(OPTFLAGS) $(PROFFLAGS)

release:BIN_DIR:= $(BIN_RLS)
release:IMP_DIR:= $(RLS_DIR)
release:OBJ_DIR:= $(OBJ_RLS)
release:CXX_OBJ_D:= $(addprefix $(OBJ_RLS), $(CXX_OBJECTS))

debug:BIN_DIR:= $(BIN_DBG)
debug:IMP_DIR:= $(DBG_DIR)
debug:OBJ_DIR:= $(OBJ_DBG)
debug:CXX_OBJ_D:= $(addprefix $(OBJ_DBG), $(CXX_OBJECTS))

-include $(addprefix $(DEP_DBG), $(CXX_SOURCES:.cpp=.d))
-include $(addprefix $(DEP_RLS), $(CXX_SOURCES:.cpp=.d))

$(BIN_RLS)cchapeiro:$(addprefix $(OBJ_RLS), $(CXX_OBJECTS))
$(BIN_DBG)cchapeiro:$(addprefix $(OBJ_DBG), $(CXX_OBJECTS))

release: $(BIN_RLS)cchapeiro
debug:   $(BIN_DBG)cchapeiro

.PHONY: all debug release 

space= 
#do no remove this lines!!! needed!!!
space+= 

vpath %.o $(subst $(space),:,$(dir $(OBJ_FILES)))
vpath %.cpp $(subst $(space),:,$(dir $(CXX_SOURCESD)))


$(sort $(subst //,/,$(dir $(OBJ_FILES)))):
	mkdir -p $@

%.o: 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(subst $(OBJ_DIR),$(SRC_ROOT)/,$(@:.o=.cpp)) -o $@

%cchapeiro:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^

clean:
	-rm -r $(OBJ_ROOT) $(BIN_ROOT) $(DEP_ROOT)
	mkdir -p $(BIN_DBG) $(BIN_RLS) $(OBJ_DBG) $(OBJ_RLS) $(DEP_DBG) $(DEP_RLS)

$(DEP_DBG)%.d: %.cpp Makefile
	@mkdir -p $(@D)
	$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $< | sed -r 's/^(\S+).(\S+):/$(SED_ODD)$(subst /,\/,$(subst $(SRC_ROOT)/,,$(<:.cpp=.o))) $(SED_DDD)$(subst /,\/,$(<:.cpp=.d)): \\\n Makefile \\\n/g' | sed -r 's/(\w)\s+(\w)/\1 \\\n \2/g' | sed '$$s/$$/\\\n | $(SED_ODD)$(subst /,\/,$(subst $(SRC_ROOT)/,,$(dir $<)))/g' | sed -r 's/(\w)+\/\.\.\///g' | awk '!x[$$0]++' > $@

$(DEP_RLS)%.d: %.cpp Makefile
	@mkdir -p $(@D)
	$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $< | sed -r 's/^(\S+).(\S+):/$(SED_ORD)$(subst /,\/,$(subst $(SRC_ROOT)/,,$(<:.cpp=.o))) $(SED_DRD)$(subst /,\/,$(<:.cpp=.d)): \\\n Makefile \\\n/g' | sed -r 's/(\w)\s+(\w)/\1 \\\n \2/g' | sed '$$s/$$/\\\n | $(SED_ORD)$(subst /,\/,$(subst $(SRC_ROOT)/,,$(dir $<)))/g' | sed -r 's/(\w)+\/\.\.\///g' | awk '!x[$$0]++' > $@
