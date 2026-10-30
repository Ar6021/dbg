BINDIR := bin
OBJDIR := obj

INCDIR := inc

SRCDIR := src
SRCS := $(strip $(shell git ls-files --cached --others --exclude-standard $(SRCDIR) | xargs -r ls -1 2>/dev/null | grep '\.cpp$$'))
OBJS := $(strip $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.obj, $(SRCS)))

DEPENDENCIES := $(OBJS:.obj=.d)

OUT := $(BINDIR)/dbglog

FORMATTER  = clang-format
FMT_EXTENSIONS = -name "*.hpp"

EXTRA_CXXFLAGS := -ggdb3

CXX := g++
CXXFLAGS := -I$(INCDIR)/ -Wall -std=c++20 -MMD -MP $(EXTRA_CXXFLAGS) $(LCXXFLAGS)

LXXFLAGS = $(EXTRA_LXXFLAGS) $(LLXXFLAGS) # nothing

$(info OUT:     ./$(OUT))

EXEC_CMD = ./$(OUT)

all: $(OUT)

$(OUT): $(OBJS)
	mkdir -p $(@D)
	$(CXX) $(OBJS) -o $(OUT) $(LXXFLAGS)

$(OBJDIR)/%.obj: $(SRCDIR)/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPENDENCIES)

rall: all run

run:
	$(EXEC_CMD)

format:
	@echo "Formatting headers in $(INCDIR)/..."
	find $(INCDIR)/ -type f \( $(FMT_EXTENSIONS) \) -exec $(FORMATTER) -i {} +
	@echo "Formatting complete!"

debug:
	gdb --args $(EXEC_CMD)

clean:
	rm -rvf bin/
	rm -rvf docs.tmp/
	rm -rvf objs/

doc:
	doxygen

.PHONY : all rall run clean doc debug
