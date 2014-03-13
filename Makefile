################################################################################
# Defs
################################################################################

CXX := g++

INCLUDE_DIRS :=
DEFS :=
USER_DEFS :=

INCLUDES := $(foreach dir, $(INCLUDE_DIRS), -I"$(dir)")

CXXFLAGS := \
	-std=gnu++0x \
	-O3 \
	\
	$(INCLUDES) \
	$(DEFS) \
	$(USER_DEFS)

LDFLAGS := -pthread -ltins -lPocoNet -lPocoUtil -lPocoFoundation -ljsoncpp

################################################################################
# Compilation tweaking
################################################################################

# warnings to show 
WARNINGS_ON := all extra cast-qual float-equal switch \
	undef init-self pointer-arith cast-align effc++ \
	strict-prototypes strict-overflow=5 write-strings \
	aggregate-return super-class-method-mismatch
# warnings to hide
WARNINGS_OFF := unused-function
# warnings to treat as errors
WARNINGS_TAE := overloaded-virtual return-stack-address \
	implicit-function-declaration address-of-temporary \
	delete-non-virtual-dtor

ifeq ($(CXX), clang++)
CXXFLAGS += \
	$(foreach w,$(WARNINGS_ON), -W$(w)) \
	$(foreach w,$(WARNINGS_OFF),-Wno-$(w)) \
	$(foreach w,$(WARNINGS_TAE),-Werror-$(w))
endif

ADDITIONAL_SOURCE_DIRS := \
	$(PWD)/Remote \
	$(PWD)/Util \
	$(PWD)/Actors \
	$(PWD)/Driver \
	$(PWD)/Checkers

ADDITIONAL_INCLUDE_DIRS := \
	$(PWD) \
	$(PWD)/lib/Theron-6.00.02/Include \

CXXFLAGS += $(foreach dir,$(ADDITIONAL_INCLUDE_DIRS),-I"$(dir)")

MAIN_SOURCES := $(shell ls $(PWD)/*.cpp)

SOURCES := \
	$(shell find $(ADDITIONAL_SOURCE_DIRS) -name "*.cpp" -type f) \
	$(shell find $(ADDITIONAL_SOURCE_DIRS) -name "*.cc"  -type f)

MAIN_OBJECTS := $(MAIN_SOURCES:.cpp=.o)

OBJECTS := $(SOURCES:.cpp=.o)

MAIN_DEPS := $(MAIN_SOURCES:.cpp=.d)

DEPS := $(SOURCES:.cpp=.d)

ARCHIVES :=

EXES := agent manager

%.d: %.cpp
	@$(CXX) $(CXXFLAGS) -MM $*.cpp > $*.d
	@mv -f $*.d $*.dd
	@sed -e 's|.*:|$*.o $*.d:|' < $*.dd > $*.d
	@rm -f $*.dd

%.d: %.cc
	@$(CXX) $(CXXFLAGS) -MM $*.cc > $*.d
	@mv -f $*.d $*.dd
	@sed -e 's|.*:|$*.o $*.d:|' < $*.dd > $*.d
	@rm -f $*.dd

################################################################################
# Building libtheron
################################################################################

LIBTHERON := $(PWD)/lib/Theron-6.00.02/Lib/libtherond.a

$(LIBTHERON): 
	make CC=$(CXX) -C lib/Theron-6.00.02 library

ARCHIVES += $(LIBTHERON)

################################################################################
# Meta rules
################################################################################

.DEFAULT_GOAL := all

.PHONY: .FORCE

.FORCE:

################################################################################
# Rules
################################################################################

all: $(EXES)

$(EXES) : % : %.o agent.o $(OBJECTS) $(ARCHIVES)
	$(CXX) -g -o $@ $*.o $(OBJECTS) $(ARCHIVES) $(LDFLAGS)

#$(EXES): $(OBJECTS) $(ARCHIVES)
#	$(CXX) -g -o $@ $(OBJECTS) $(ARCHIVES) $(LDFLAGS)

clean:
	@rm -f $(EXES) $(OBJECTS) $(DEPS) $(MAIN_OBJECTS) $(MAIN_DEPS)

################################################################################
-include $(DEPS)
-include $(MAIN_DEPS)
