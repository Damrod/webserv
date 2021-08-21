CXXFLAGS = -Wall -Werror -Wextra -std=c++11 # catch2 necesita c++11
INC_DIR = $(SRC_DIR)/incs/test

# if you create a new folder with source files, it should be in the srcs
# folder, and you should put its name here:
DIRS = app/test

include $(SRC_DIR)/target/common.mk
SRCS := $(SRC_DIR)/app/test/utest_startup.cpp $(SRC_DIR)/app/startup.cpp

NAME := $(BLD_DIR)/utest_app
# this assigns the .o file names replacing the srcs part of the $(SRCS)
# path with bld and the .cpp part of the filename with .o
OBJ := $(subst $(SRC_DIR),$(BLD_DIR),$(SRCS:%.cpp=%.o))

# I tweaked this rule that George made to build the .d files also inside the
# bld folder, from the .o files
DEPS := $(patsubst %.o, %.d, $(OBJ))

.PHONY: all
all: $(NAME)

# Generate rules to compile .o files

$(foreach targetdir, $(OBJ_DIRS), $(eval $(call generateRules, $(targetdir))))

# Generate rules to create the dirs that will contain the .o files

$(foreach targetdir, $(OBJ_DIRS), $(eval $(call generateDirs, $(targetdir))))

$(NAME):  $(OBJ_DIRS) $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

# take a look at what does a .d file look like to understand this directive
-include $(DEPS)
