###########################################
# A Simple Makefile
###########################################

MARCH	=$(subst _,-,$(shell arch))

#CXX		= g++
CXXFLAGS= -g -Wall -Wno-char-subscripts -march=$(MARCH) -fpermissive -fPIC

NAME	= $(shell pwd|awk -F'/' '{print $$NF}')
LIBNAME := lib$(NAME).so
OUTDIR	?= .

INCLUDES := -I. -I.. -I/usr/local/include
LINKLIBS := -L/usr/local/lib -lpthread -luv -L$(OUTDIR) -lutils

COMPILEFLAGS:=$(CXXFLAGS)
LINKFLAGS:= -Wl,-rpath=.
SHARED := -shared

find_c_files=$(wildcard $(dir)/*.c)
find_cpp_files=$(wildcard $(dir)/*.cpp)
dirs = $(shell find . -name '*' -type d)
C_FILES = $(foreach dir,$(dirs),$(find_c_files))
CPP_FILES = $(foreach dir,$(dirs),$(find_cpp_files))

C_OBJECTS := $(C_FILES:.c=.o)
CPP_OBJECTS := $(CPP_FILES:.cpp=.obj)

.PHONY:all clean

all:$(LIBNAME)

$(LIBNAME): $(C_OBJECTS) $(CPP_OBJECTS)
	$(CXX) $(LINKFLAGS) $(SHARED) $^ -o $(OUTDIR)/$@ $(LINKLIBS)
	chmod +x $(OUTDIR)/$@
	
%.o:%.c
	$(CXX) $(COMPILEFLAGS) $(INCLUDES) -c $^ -o $@
%.obj:%.cpp
	$(CXX) $(COMPILEFLAGS) $(INCLUDES) -c $^ -o $@
	
clean:
	rm -f $(OUTDIR)/$(LIBNAME) $(C_OBJECTS) $(CPP_OBJECTS)
