CXXFLAGS = -Wall -Werror -Wextra -std=c++11 -g3 # catch2 necesita c++11
INC_DIR = $(SRC_DIR)/incs $(SRC_DIR)/incs/test

# if you create a new folder with source files, it should be in the srcs
# folder, and you should put its name here:
UTEST_DIR = app/test/unit
DIRS = $(UTEST_DIR) app/http app/http/response app/http/response/get \
	app/http/response/post app/http/response/delete \
	$(UTEST_DIR)/http app/utils $(UTEST_DIR)/utils app/config \
	$(UTEST_DIR)/config_analyser app/config/analyser app/server \
	app/CGI

include $(SRC_DIR)/target/common.mk

NAME := $(BLD_DIR)/utest_app

.PHONY: all
all: $(NAME)

include $(SRC_DIR)/target/generaterules.mk

# take a look at what does a .d file look like to understand this directive
-include $(DEPS)
