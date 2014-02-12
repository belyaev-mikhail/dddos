################################################################################
# Defs
################################################################################

CXX := g++

INCLUDE_DIRS :=
DEFS :=
USER_DEFS :=

INCLUDES := $(foreach dir, $(INCLUDE_DIRS), -I"$(dir)")

CXXFLAGS := \
	-std=c++11 \
	-O3 \
	$(INCLUDES) \
	$(DEFS) \
	$(USER_DEFS)

LDFLAGS := -pthread -ltins

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

ADDITIONAL_SOURCE_DIRS := $(PWD)/Part

ADDITIONAL_INCLUDE_DIRS := \
	$(PWD) \
	$(PWD)/lib/Theron-6.00.02/Include \

CXXFLAGS += $(foreach dir,$(ADDITIONAL_INCLUDE_DIRS),-I"$(dir)")

SOURCES := \
	$(shell ls $(PWD)/*.cpp) \
	$(shell find $(ADDITIONAL_SOURCE_DIRS) -name "*.cpp" -type f) \
	$(shell find $(ADDITIONAL_SOURCE_DIRS) -name "*.cc"  -type f)

OBJECTS := $(SOURCES:.cpp=.o)

DEPS := $(SOURCES:.cpp=.d)

ARCHIVES :=

EXES := wrapper

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
	make -C lib/Theron-6.00.02

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

$(EXES): $(OBJECTS) $(ARCHIVES)
	$(CXX) -g $(LDFLAGS) -o $@ $(OBJECTS) $(ARCHIVES)

clean:
	@rm -f $(EXES) $(OBJECTS) $(DEPS)

################################################################################
-include $(DEPS)
