CPP = g++

CPPFLAGS = -g0 -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat                          \
-Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy            \
-Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winline -Wnon-virtual-dtor 	  \
-Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion        \
-Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code \
-Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs    \
-Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow                        \
-fno-omit-frame-pointer -Wlarger-than=8192 -fPIE -Werror=vla 												  \
-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

BUILD = build

SRCMODULES = tree_funcs.cpp akinator_funcs.cpp inout_funcs.cpp tree_dump.cpp
DEBUG_MODULE = debug_lib/debug.cpp
OBJMODULES = $(addprefix $(BUILD)/,$(SRCMODULES:.cpp=.o)) 
DEBUG_OBJ = $(addprefix $(BUILD)/,$(notdir $(DEBUG_MODULE:.cpp=.o)))

$(BUILD)/differntiator: main.cpp $(OBJMODULES) $(DEBUG_OBJ)
	$(CPP) $(CPPFLAGS) $^ -o $@

$(BUILD)/%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(BUILD)/%.o: debug_lib/%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

deps.mk: $(SRCMODULES) $(DEBUG_MODULE)
	$(CPP) -MM $^ > $@

ifneq (clean, $(MAKECMDGOALS))
-include deps.mk
endif

.PHONY: clean

clean:
	rm -rf $(BUILD)