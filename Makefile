# Matt Bisson - 7/12/2014
#
# The Makefile is a means to an end... I don't care about how difficult it is
# to maintain.

################################################################################
# Basic set-up
################################################################################

# Nothing real here -- just exists in case I hard-code a version of a tool.
DOXYGEN = doxygen
MKDIR   = mkdir
RM      = rm
STRIP   = strip

ifeq ($(USE_RELEASE),)
DEBUG_BUILD = 1
endif

ifeq ($(USE_CLANG),)
CXX = g++
else
CXX = clang++
endif

################################################################################
# Compiler Configuration:
################################################################################

CFLAGS   = -Wall -Wextra -Wpedantic -Wno-missing-field-initializers \
           -march=native
CPPFLAGS = -I./include -I./windows_shim
ifdef DEBUG_BUILD
CFLAGS   += -g
CPPFLAGS += -D_DEBUG
else
CFLAGS   += -O3 -flto
endif

CXXFLAGS = $(CFLAGS) -std=c++11

################################################################################
# What we're building...
################################################################################

# Define where everything goes
OUTPUT_DIR     = ./out
OUTPUT_DOC_DIR = ./doc

# Let's create an output directory to match our source tree.  This can be manual
# for now.
OUTPUT_DIRS = \
    $(OUTPUT_DIR)/exec \
    $(OUTPUT_DIR)/addr2ln \
    $(OUTPUT_DIR)/proclib \
    $(OUTPUT_DIR)/proclib/demangle \
    $(OUTPUT_DIR)/pstack \
    $(OUTPUT_DIR)/psystem \
    $(OUTPUT_DIR)/psystem/dll \
    $(OUTPUT_DIR)/psystem/exception \
    $(OUTPUT_DIR)/psystem/framework \
    $(OUTPUT_DIR)/test

# Define the source modules used in the test executable (and generate the .o
# file names and path).
ADDR2LN_SRCS = \
    src/addr2ln/addr2ln.cpp \
    src/addr2ln/addr2ln_options.cpp
ADDR2LN_OBJS = \
    $(patsubst src/%,$(OUTPUT_DIR)/%,$(ADDR2LN_SRCS:.cpp=.o))

# Define the source modules used in the PSystem/ProcLib library (and generate
# the .o file names and path).
PROCLIB_SRCS = \
    src/proclib/base_debug_event_listener.cpp \
    src/proclib/debug_engine.cpp \
    src/proclib/demangle/demangle.cpp \
    src/proclib/process_debugger.cpp \
    src/psystem/dbghelp_symbol_engine.cpp \
    src/psystem/dll_export_iterator.cpp \
    src/psystem/dll_export_view.cpp \
    src/psystem/dll/debug_help_dll.cpp \
    src/psystem/exception/base_exception.cpp \
    src/psystem/exception/system_exception.cpp \
    src/psystem/framework/shared_library.cpp \
    src/psystem/image_symbol_iterator.cpp \
    src/psystem/module_info.cpp \
    src/psystem/process_info.cpp \
    src/psystem/psystem_ns.cpp \
    src/psystem/stack_frame.cpp \
    src/psystem/symbol.cpp \
    src/psystem/symbol_file_module.cpp \
    src/psystem/symbol_view.cpp \
    src/psystem/thread_info.cpp
PROCLIB_OBJS = $(patsubst src/%,$(OUTPUT_DIR)/%,$(PROCLIB_SRCS:.cpp=.o))

# Define the source modules used in the PStack executable (and generate the .o
# file names and path).
PSTACK_SRCS = \
    src/pstack/debug_event_dumper.cpp \
    src/pstack/pstack.cpp \
    src/pstack/pstack_event_handler.cpp \
    src/pstack/pstack_options.cpp
PSTACK_OBJS = $(patsubst src/%,$(OUTPUT_DIR)/%,$(PSTACK_SRCS:.cpp=.o))

# Define the source modules used in the test executable (and generate the .o
# file names and path).
TEST_APPLICATION_SRCS = \
    test/test_application.cpp
TEST_APPLICATION_OBJS = \
    $(patsubst test/%,$(OUTPUT_DIR)/test/%,$(TEST_APPLICATION_SRCS:.cpp=.o))

# Everything we're building.
EXES = \
    $(OUTPUT_DIR)/exec/addr2ln \
    $(OUTPUT_DIR)/exec/pstack \
    $(OUTPUT_DIR)/exec/test_application

################################################################################
# Make recipes
################################################################################

# Meta-targets...
.PHONY: all clean clean_docs distclean docs

# Clear out any suffix we're not using to avoid accidents, then re-set.
.SUFFIXES:
.SUFFIXES: .cpp .o .i

#
# "Phony" targets go here...
#
all: $(OUTPUT_DIRS) $(EXES)

docs:
	$(MKDIR) -p $(OUTPUT_DOC_DIR)
	$(DOXYGEN)

clean:
	$(RM) -f $(ADDR2LN_OBJS) $(PROCLIB_OBJS) $(PSTACK_OBJS) $(TEST_APPLICATION_OBJS) $(EXES)

clean_docs:
	$(RM) -rf $(OUTPUT_DOC_DIR)

distclean: clean clean_docs
	$(RM) -rf $(OUTPUT_DIR)
	$(RM) -f ./msvc/pstack.sdf ./msvc/pstack.opensdf ./msvc/pstack.v12.suo

#
# Wild-card recipes here...
#

# TO-DO -- THERE IS ABSOLUTELY NO DEPENDENCY CHECKING FOR HEADERS HERE.
$(OUTPUT_DIR)/test/%.i: test/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -E -o $@ $<
$(OUTPUT_DIR)/test/%.o: test/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

# TO-DO -- THERE IS ABSOLUTELY NO DEPENDENCY CHECKING FOR HEADERS HERE.
$(OUTPUT_DIR)/%.i: src/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -E -o $@ $<
$(OUTPUT_DIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

#
# Fixed targets...
#

# In case we need to create a destination for .o files...
$(OUTPUT_DIRS):
	@$(MKDIR) -p $(OUTPUT_DIRS)

$(OUTPUT_DIR)/exec/addr2ln: $(ADDR2LN_OBJS) $(PROCLIB_OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LOADLIBS) $(LDLIBS) -o $@
ifndef DEBUG_BUILD
	$(STRIP) $@
endif

$(OUTPUT_DIR)/exec/pstack: $(PSTACK_OBJS) $(PROCLIB_OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LOADLIBS) $(LDLIBS) -o $@
ifndef DEBUG_BUILD
	$(STRIP) $@
endif

$(OUTPUT_DIR)/exec/test_application: $(TEST_APPLICATION_OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LOADLIBS) $(LDLIBS) -lpthread -o $@
ifndef DEBUG_BUILD
	$(STRIP) $@
endif
