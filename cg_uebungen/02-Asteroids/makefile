# This makefile should be executed from a bash (eg. Git-bash in windows).
# However, the only incompatible command on windows is the "mkdir -p" which
# is harder to solve as one might think (no silent mkdir/md on windows at all).
CONF := DEBUG

FLAGS := -std=c++17 -Wall -Wno-comment\
		 -DWIN32_LEAN_AND_MEAN\
		 -DNOMINMAX\
		-DWINDOW_PUT_PIXEL

ifeq ($(CONF),DEBUG)
	FLAGS += -D_DEBUG -g
	BUILD_DIR := build/dbg
	OUT_NAME := softwareD.exe
else
	FLAGS += -O3 -march=native
	BUILD_DIR := build/rel
	OUT_NAME := software.exe
endif
.DEFAULT_GOAL := $(OUT_NAME)

INCLUDES := -I../dependencies/glm -I../dependencies -I../dependencies/glfw/include -I../dependencies/glad/include
LIBS := -L../dependencies/glfw/lib -lglfw3 -luser32 -lshell32 -lgdi32

# Recursive function to find files in a directory tree. From:
# https://stackoverflow.com/questions/2483182/recursive-wildcards-in-gnu-make
# call as $(call rwildcard, <subdir>, <ending> <ending> ...)
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

# Get all cpp files recursively
SRC_FILES := $(call rwildcard, , *.cpp)
SHARED_SRC_FILES := $(call rwildcard, ../framework, *.cpp) ../dependencies/glad/src/glad.cpp
# Generate the names of the resulting obj files (two alternatives which do the same)
#OBJ_FILES = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))
OBJ_FILES := $(SRC_FILES:%.cpp=$(BUILD_DIR)/%.o) $(SHARED_SRC_FILES:../%.cpp=$(BUILD_DIR)/%.o)
DEP_FILES := $(OBJ_FILES:%.o=%.d)

# Uncomment if you want to see what is going on:
# $(info SRC_FILES is $(SRC_FILES))
# $(info OBJ_FILES is $(OBJ_FILES))
# $(info DEP_FILES is $(DEP_FILES))

# Include all dependency .d files.
# The - in front makes the failure silent (if no .d exists)
-include $(DEP_FILES)

# There is a decrepancy how to create a directory recursive (e.g. build/dbg/misc/) between Unix
# and windows. In Unix mkdir -p <path> creates all necessary directories and finishes silently if
# they exist. In Windows makdir "<path>" works similar, but creates an error which pollutes the
# makefile output/stops the makefile. The 2> NUL ||: pipes the error message to the void and
# continues the make due to || true == ||:.
DIR_GUARD = @mkdir -p $(@D)
ifeq ($(OS),Windows_NT)
	DIR_GUARD = @mkdir "$(@D)" 2> NUL ||:
endif

# The -MMD flags additionaly creates a .d file with
# the same name as the .o file.
$(BUILD_DIR)/%.o: %.cpp
	$(DIR_GUARD)
	g++ $(FLAGS) $(INCLUDES) -MMD -c -o $@ $<

$(BUILD_DIR)/%.o: ../%.cpp
	$(DIR_GUARD)
	g++ $(FLAGS) $(INCLUDES) -MMD -c -o $@ $<

$(OUT_NAME): $(OBJ_FILES)
	g++ $(OBJ_FILES) $(LIBS) -o $(OUT_NAME)

# clean:
# 	rm -f getprime.exe getprimeD.exe
# 	rm -rf build/*
# 