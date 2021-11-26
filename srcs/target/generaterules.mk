# this assigns the .o file names replacing the srcs part of the $(SRCS)
# path with bld and the .cpp part of the filename with .o
OBJS := $(foreach src, $(SRCS), $(subst $(SRC_DIR),$(BLD_DIR),$(basename $(src))).o)

# I tweaked this rule that George made to build the .d files also inside the
# bld folder, from the .o files
DEPS := $(foreach obj, $(OBJS), $(patsubst %.o, %.d, $(obj)))

# Generate rules to compile .o files

$(foreach obj, $(OBJS), $(eval $(call generateRules, $(obj))))

# Generate rules to create the dirs that will contain the .o files

$(foreach targetdir, $(OBJ_DIRS), $(eval $(call generateDirs, $(targetdir))))

# Here I just changed the name of the OBJ_DIR since we now have several
$(NAME):  $(OBJ_DIRS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(INC_PARAMS) -o $(NAME) $(OBJS)
