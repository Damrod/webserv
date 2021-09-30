CXXFLAGS = -Wall -Werror -Wextra -std=c++11 -g3 # catch2 necesita c++11
INC_DIR = $(SRC_DIR)/incs $(SRC_DIR)/incs/test

# if you create a new folder with source files, it should be in the srcs
# folder, and you should put its name here:
DIRS = app/test app/http app/test/http app/utils app/test/utils app/config \
	app/test/config_analyser app/config/analyser app/server

include $(SRC_DIR)/target/common.mk

NAME := $(BLD_DIR)/utest_app

.PHONY: all
all: $(NAME)

include $(SRC_DIR)/target/generaterules.mk

# take a look at what does a .d file look like to understand this directive
-include $(DEPS)
