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

# this will recursively expand the above DIRS into full path name
SRC_DIRS := $(foreach dir, $(DIRS), $(addprefix $(SRC_DIR)/, $(dir)))
# this will replace the above srcs/ part of the path with bld/
OBJ_DIRS := $(foreach dir, $(DIRS), $(addprefix $(BLD_DIR)/, $(dir)))
