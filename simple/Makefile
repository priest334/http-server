###########################################
# A Simple Makefile
###########################################

MARCH	=$(subst _,-,$(shell arch))

#CXX		= g++
CXXFLAGS= -g -Wall -Wno-char-subscripts -march=$(MARCH) -fpermissive -fPIC

NAME	= $(shell pwd|awk -F'/' '{print $$NF}')
EXENAME	:= $(NAME)
OUTDIR	?= .

INCLUDES := -I. -I.. -I/usr/local/include
LINKLIBS := -L/usr/local/lib -lpthread -luuid -L$(OUTDIR) -lutils -lserver

COMPILEFLAGS:=$(CXXFLAGS)
LINKFLAGS:= -Wl,-rpath=.

find_c_files=$(wildcard $(dir)/*.c)
find_cpp_files=$(wildcard $(dir)/*.cpp)
dirs = $(shell find . -name '*' -type d)
C_FILES = $(foreach dir,$(dirs),$(find_c_files))
CPP_FILES = $(foreach dir,$(dirs),$(find_cpp_files))

C_OBJECTS := $(C_FILES:.c=.o)
CPP_OBJECTS := $(CPP_FILES:.cpp=.obj)

.PHONY:all clean

all:$(EXENAME)
$(EXENAME): $(C_OBJECTS) $(CPP_OBJECTS)
	$(CXX) $(LINKFLAGS) -o $(OUTDIR)/$@ $^ $(LINKLIBS)
	chmod +x $(OUTDIR)/$@
	
%.o:%.c
	$(CXX) $(COMPILEFLAGS) $(INCLUDES) -c $^ -o $@
%.obj:%.cpp
	$(CXX) $(COMPILEFLAGS) $(INCLUDES) -c $^ -o $@
	
clean:
	rm -f $(OUTDIR)/$(EXENAME) $(C_OBJECTS) $(CPP_OBJECTS)
