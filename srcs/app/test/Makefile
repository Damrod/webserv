define generateRules
$(1)/%.o: %.cpp
	@echo Building $$@
	$(CXX) -c $(CXXFLAGS) -I $(INC_DIR) -o $$@ $$< -MMD
endef

define generateDirs
$(1):
	@echo Creating dir $$@
	$(MKDIR) $$@ $(ERRIGNORE)
endef

CXX = clang++
CXXFLAGS = -Wall -Werror -Wextra -std=c++11
LINT = cpplint
LINTFLAGS = --recursive
RMF = rm -rf
MKDIR = mkdir -p
ERRIGNORE = 2>/dev/null

PROJDIR := $(CURDIR)/../../..

SRC_DIR := $(PROJDIR)/srcs
BLD_DIR := $(PROJDIR)/bld
INC_DIR = $(SRC_DIR)/incs

# if you create a new folder with source files, it should be in the srcs
# folder, and you should put its name here:
DIRS = app/test

# this will recursively expand the above DIRS into full path name
SRC_DIRS := $(foreach dir, $(DIRS), $(addprefix $(SRC_DIR)/, $(dir)))
# this will replace the above srcs/ part of the path with bld/
OBJ_DIRS := $(foreach dir, $(DIRS), $(addprefix $(BLD_DIR)/, $(dir)))

SRCS := $(SRC_DIR)/app/test/utest_startup.cpp $(SRC_DIR)/app/startup.cpp

# this is here so that the macro generateRules can find its sources
VPATH := $(SRC_DIR)/app $(SRC_DIR)/app/test
NAME := $(BLD_DIR)/utest_app
# this assigns the .o file names replacing the srcs part of the $(SRCS)
# path with bld and the .cpp part of the filename with .o
OBJ := $(subst $(SRC_DIR),$(BLD_DIR),$(SRCS:%.cpp=%.o))

# I tweaked this rule that George made to build the .d files also inside the
# bld folder, from the .o files
DEPS := $(patsubst %.o, %.d, $(OBJ))

.PHONY: all
all: $(NAME)
	echo $(OBJ)

# Generate rules to compile .o files

$(foreach targetdir, $(OBJ_DIRS), $(eval $(call generateRules, $(targetdir))))

# Generate rules to create the dirs that will contain the .o files

$(foreach targetdir, $(OBJ_DIRS), $(eval $(call generateDirs, $(targetdir))))

# Aqui hay una chapuza porque no consigo que VPATH me funcione...
/home/alvaro/webserv/srcs/app/test/../../../bld/app/startup.o :
	clang++ -c -Wall -Werror -Wextra -std=c++11 -I /home/alvaro/webserv/srcs/incs -o /home/alvaro/webserv/srcs/app/test/../../../bld/app/startup.o ../startup.cpp -MMD


$(NAME):  $(OBJ_DIRS) $(OBJ)
	echo hello $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

# take a look at what does a .d file look like to understand this directive
-include $(DEPS)
