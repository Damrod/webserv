NAME = webserv
CXX = clang++
DBG ?=
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -g3
LINT = cpplint
LINTFLAGS = --recursive --exclude=srcs/incs/test/catch2.hpp \
		--exclude=nocommit/ --exclude=*.nocommit
RMF = rm -rf
MKDIR = mkdir -p
ERRIGNORE = 2>/dev/null
QUIET=@

PROJDIR := $(CURDIR)

SRC_DIR := $(PROJDIR)/srcs
BLD_DIR := $(PROJDIR)/bld
INC_DIR = $(SRC_DIR)/incs

# if you create a new folder with source files, it should be in the srcs
# folder, and you should put its name here:
DIRS = . utils config config/analyser server CGI http http/response \
	http/response/get http/response/post http/response/delete

NAME_TEST = $(BLD_DIR)/utest_app
UTEST_DIR = test/unit
UT_DIRS = $(UTEST_DIR) $(UTEST_DIR)/http $(UTEST_DIR)/utils \
          	$(UTEST_DIR)/config_analyser

INC_PARAMS = $(foreach d, $(INC_DIR), -I$d)

define generateRules
$(1): $$(basename $$(subst $(BLD_DIR),$(SRC_DIR),$(1))).cpp
	$(QUIET)printf "[%3u%%] \033[0;32mBuilding CXX object $$@\033[0;0m\n" \
	$(shell \
	echo "100000 * $(2) / ( $(3) * 1000 ) " | \
	bc )
	$(QUIET)$(CXX) -c $(DBG) $(6) $(5) -o $$@ $$< -MMD && \
	if [ $(2) = $(3) ] ; then \
		printf "[100%%] \033[01;32mBuilt target %s\033[0;0m\n" $(4); \
	fi
endef

define generateDirs
$(1):
	$(QUIET)echo Creating dir $$@
	$(QUIET)$(MKDIR) $$@ $(ERRIGNORE)
endef

# this will recursively expand the above DIRS into full path name
SRC_DIRS := $(foreach dir, $(DIRS), $(addprefix $(SRC_DIR)/, $(dir)))
# this will replace the above srcs/ part of the path with bld/
OBJ_DIRS := $(foreach dir, $(DIRS), $(addprefix $(BLD_DIR)/, $(dir)))

# this will recursively expand the above DIRS into full path name
UT_SRC_DIRS := $(foreach dir, $(UT_DIRS), $(addprefix $(SRC_DIR)/, $(dir)))
# this will replace the above srcs/ part of the path with bld/
UT_OBJ_DIRS := $(foreach dir, $(UT_DIRS), $(addprefix $(BLD_DIR)/, $(dir)))

# this will glob all cpp files inside the SRC_DIRS
SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
UT_SRCS := $(foreach dir,$(UT_SRC_DIRS),$(wildcard $(dir)/*.cpp))

.PHONY: all
all: $(NAME)

# this assigns the .o file names replacing the srcs part of the $(SRCS)
# path with bld and the .cpp part of the filename with .o
OBJS := $(foreach src, $(SRCS), $(subst $(SRC_DIR),$(BLD_DIR),$(basename $(src))).o)
UT_OBJS := $(foreach src, $(UT_SRCS), $(subst $(SRC_DIR),$(BLD_DIR),$(basename $(src))).o)

# I tweaked this rule that George made to build the .d files also inside the
# bld folder, from the .o files
DEPS := $(foreach obj, $(OBJS), $(patsubst %.o, %.d, $(obj)))
UT_DEPS := $(foreach obj, $(UT_OBJS), $(patsubst %.o, %.d, $(obj)))

# Generate rules to compile .o files

define generateBundle
$(1):
	$(foreach x, \
		$(shell for x in $(shell seq 1 $(words $(1))); \
			do \
				echo $$x; \
			done \
		), \
		$(eval \
			$(call generateRules, \
				$(word $(x),$(1)), \
				$(x), \
				$(words $(1)), \
				$(2), \
				$(3), \
				$(4) \
			) \
		) \
	)
endef

$(call generateBundle, $(OBJS), OBJS, $(INC_PARAMS), $(CXXFLAGS))

$(call generateBundle, $(UT_OBJS), UT_OBJS, $(INC_PARAMS) -I$(SRC_DIR)/incs/test, \
	$(filter-out -std=c++98,$(CXXFLAGS)) -std=c++11)

# Generate rules to create the dirs that will contain the .o files

$(foreach targetdir, $(OBJ_DIRS), $(eval $(call generateDirs, $(targetdir))))

$(foreach targetdir, $(UT_OBJ_DIRS), $(eval $(call generateDirs, $(targetdir))))

# Here I just changed the name of the OBJ_DIR since we now have several
$(NAME): $(OBJS)
	$(QUIET)printf "[100%%] \033[0;32mLinking CXX executable $@\033[0;0m\n"
	$(QUIET)$(CXX) $(CXXFLAGS) $(INC_PARAMS) -o $(NAME) $(OBJS)

# take a look at what does a .d file look like to understand this directive
-include $(DEPS)
-include $(UT_DEPS)

$(OBJS): | $(OBJ_DIRS)

$(UT_OBJS): | $(UT_OBJ_DIRS)

$(NAME_TEST): $(OBJS) $(UT_OBJS)
	$(QUIET)printf "[100%%] \033[0;32mLinking CXX executable $(NAME_TEST)\033[0;0m\n"
	$(QUIET)$(CXX) $(filter-out -std=c++98,$(CXXFLAGS)) -std=c++11 $(INC_PARAMS) \
	-I$(SRC_DIR)/incs/test -o $(BLD_DIR)/utest_app $(filter-out \
	$(BLD_DIR)/./main.o, $(OBJS)) $(UT_OBJS) -MMD

.PHONY: unit_test
unit_test: $(NAME_TEST)
	$(NAME_TEST)

.PHONY: integration_test
integration_test: all
	pytest --tb=short

.PHONY: test
test: unit_test integration_test

.PHONY: lint
lint:
	$(LINT) $(LINTFLAGS) .

.PHONY: ngxsyntax
ngxsyntax:
	cd $(SRC_DIR)/test/unit/config_analyser/nginx_docker && docker-compose up

.PHONY: clean
clean:
	$(RMF) $(BLD_DIR)

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)

.PHONY: re
re: fclean
	$(MAKE)
