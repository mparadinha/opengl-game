CXX=g++
CC=$(CXX)
CFLAGS=-Wall -pedantic
#CFLAGS+=-Wextra
CFLAGS+=-O0
CFLAGS+=-fdiagnostics-color=always
#CFLAGS+=-g -Og
#CFLAGS+=-pg
CXXFLAGS=$(CFLAGS)
CPPFLAGS=-I./include
LDFLAGS=
LDLIBS=
SRCDIR=.
SRC!=find $(SRCDIR) -name "*.cpp" -not -path "./old/*" | cut -d"/" -f2-
OBJDIR=.obj

# Define executables and their link dependencies here
EXECS = thingy
thingy_DEPS = $(SRC)
thingy_LIBS = dl glfw

debug:
	$(eval OBJDIR=.obj_dbg)
	$(eval CFLAGS+=-g -Og)
	@make CFLAGS="$(CFLAGS)" OBJDIR="$(OBJDIR)"

###############################################################################
# No more user editing is needed below
###############################################################################
define EXEC_templ =
$(1): $$(addprefix $$(OBJDIR)/,$$($(1)_DEPS:%.cpp=%.o))
	$(CC) -o $$@ $$(LDFLAGS) $$(LDLIBS) $$($(1)_LIBS:%=-l%) $$^
endef

$(foreach exec,$(EXECS),$(eval $(call EXEC_templ,$(exec))))

-include $(addprefix $(OBJDIR)/, $(SRC:.cpp=.d))
$(OBJDIR)/%.d: %.cpp
	@mkdir -p `dirname $@`
	$(CC) $(CPPFLAGS) -MM -MF $@ $^

$(OBJDIR)/%.o: %.cpp
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<

.PHONY=clean all
.DEFAULT_GOAL=all
all: $(EXECS)

clean:
	rm -rf $(OBJDIR) $(EXECS)

# This rebuilds everything if the Makefile was modified
# http://stackoverflow.com/questions/3871444/making-all-rules-depend-on-the-makefile-itself/3892826#3892826
-include .dummy
.dummy: Makefile
	@touch $@
	@$(MAKE) -s clean
