# Uncomment to customize these variables
#CC=cc
#CXX=g++
#CPPFLAGS:=$(CPPFLAGS)
#CFLAGS:=$(CFLAGS)
#LDFLAGS:=$(LDFLAGS)
#LDLIBS:=$(LDLIBS)
#RM=rm -f

# Extra C/C++ source code files to be linked in with the monitors (must be .c,
# .cc, .cpp, or .C files)
EXTRA_SOURCES=

# Uncomment the first for a debug build. Uncomment the second for a release
# (optimized) build. DEBUG flag can be adjusted as necessary:
# -DDEBUG=0: Debug off (same as not defining DEBUG at all)
# -DDEBUG=1: Errors only (Serious errors will be printed even with debug off)
# -DDEBUG=2: Warnings (Non-serious abnormal conditions)
# -DDEBUG=3: Notice (Significant but normal occurrences)
# -DDEBUG=4: Debug (All debug messages on)
CFLAGS:=-g -DDEBUG=0 $(CFLAGS)
#CFLAGS:=-O2 -DNDEBUG $(CFLAGS)

# Where to place all object files, dependency makefiles, and executables.
# For example, to place everything in a "build" directory:
#BUILD_DIR=./build
BUILD_DIR=.

# Changes below this line not recommended
###############################################################################


COMMON_SOURCES=smedl_types.c event_queue.c monitor_map.c global_event_queue.c file.c json.c
SOURCES_CandidateSelection=CandidateSelection_mon.c CandidateSelection_local_wrapper.c CandidateSelection_global_wrapper.c
SOURCES_CandidateRank=CandidateRank_mon.c CandidateRank_local_wrapper.c CandidateRank_global_wrapper.c
SOURCES_CollectV=CollectV_mon.c CollectV_local_wrapper.c CollectV_global_wrapper.c
SOURCES_Collect=Collect_mon.c Collect_local_wrapper.c Collect_global_wrapper.c
SMEDL_SOURCES=$(COMMON_SOURCES) CanSys_file.c $(SOURCES_CandidateSelection) $(SOURCES_CandidateRank) $(SOURCES_CollectV) $(SOURCES_Collect)

SMEDL_OBJS=$(SMEDL_SOURCES:.c=.o)
SMEDL_OBJS:=$(SMEDL_OBJS:%=$(BUILD_DIR)/%)

EXTRA_OBJS=$(EXTRA_SOURCES)
EXTRA_OBJS:=$(EXTRA_OBJS:.c=.o)
EXTRA_OBJS:=$(EXTRA_OBJS:.cc=.o)
EXTRA_OBJS:=$(EXTRA_OBJS:.cpp=.o)
EXTRA_OBJS:=$(EXTRA_OBJS:.C=.o)
EXTRA_OBJS:=$(EXTRA_OBJS:%=$(BUILD_DIR)/%)

SOURCES=$(SMEDL_SOURCES) $(EXTRA_SOURCES)
OBJS=$(SMEDL_OBJS) $(EXTRA_OBJS)
DEPS=$(OBJS:.o=.d)

.PHONY: all clean

all: $(BUILD_DIR)/CanSys

$(BUILD_DIR)/CanSys: $(OBJS)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) $+ $(LDLIBS) -o $@

$(SMEDL_OBJS): $(BUILD_DIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -MMD -MP $(CFLAGS) -std=c99 -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -MMD -MP $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cc
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) -MMD -MP $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) -MMD -MP $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.C
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) -MMD -MP $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(DEPS) $(BUILD_DIR)/CanSys

-include $(DEPS)
