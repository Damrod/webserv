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

NAME = webserv
CXX = clang++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98
LINT = cpplint
LINTFLAGS = --recursive
RMF = rm -rf
MKDIR = mkdir -p
ERRIGNORE = 2>/dev/null

PROJDIR := $(CURDIR)

export SRC_DIR := $(PROJDIR)/srcs
export BLD_DIR := $(PROJDIR)/bld
export INC_DIR = $(SRC_DIR)/incs

# if you create a new folder with source files, it should be in the srcs
# folder, and you should put its name here:
DIRS = app

# this will recursively expand the above DIRS into full path name
SRC_DIRS := $(foreach dir, $(DIRS), $(addprefix $(SRC_DIR)/, $(dir)))
# this will replace the above srcs/ part of the path with bld/
OBJ_DIRS := $(foreach dir, $(DIRS), $(addprefix $(BLD_DIR)/, $(dir)))
# this will glob all cpp files inside the SRC_DIRS
SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
# this is here so that the macro generateRules can find its sources
VPATH := $(SRC_DIRS)
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

# Here I just changed the name of the OBJ_DIR since we now have several
$(NAME):  $(OBJ_DIRS) $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

# take a look at what does a .d file look like to understand this directive
-include $(DEPS)

catchsrc:=$(shell mktemp -d)

.PHONY: catch
catch:
	if ! test $$(find /usr/local/include/ -name catch3 | wc -c) -eq 0 ; then \
	git clone https://github.com/catchorg/Catch2.git $(catchsrc)/Catch2 ; \
	cd $(catchsrc)/Catch2 && \
	cmake -Bbuild -H. -DBUILD_TESTING=OFF && \
	sudo cmake --build build/ --target install ; \
	fi

TEST0 := $(BLD_DIR)/utest_app

$(TEST0) : $(OBJ_DIRS) $(OBJ)
	make -C srcs/app/test

.PHONY: run
run:
	$(BLD_DIR)/utest_app

.PHONY: test
test:
	make $(TEST0)

.PHONY: lint
lint:
	$(LINT) $(LINTFLAGS) .

.PHONY: clean
clean:
	$(RMF) $(BLD_DIR)

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)

.PHONY: re
re: fclean all
