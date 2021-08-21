NAME = webserv
export CXX = clang++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98
LINT = cpplint
LINTFLAGS = --recursive --exclude=srcs/incs/test/catch2.hpp
RMF = rm -rf
export MKDIR = mkdir -p
export ERRIGNORE = 2>/dev/null

PROJDIR := $(CURDIR)

export SRC_DIR := $(PROJDIR)/srcs
export BLD_DIR := $(PROJDIR)/bld
INC_DIR = $(SRC_DIR)/incs

# if you create a new folder with source files, it should be in the srcs
# folder, and you should put its name here:
DIRS = app

include $(SRC_DIR)/target/common.mk
# this will glob all cpp files inside the SRC_DIRS
SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
# this is here so that the macro generateRules can find its sources
VPATH := $(SRC_DIRS)

.PHONY: all
all: $(NAME)

include $(SRC_DIR)/target/generaterules.mk

# take a look at what does a .d file look like to understand this directive
-include $(DEPS)

include $(SRC_DIR)/target/test.mk

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
