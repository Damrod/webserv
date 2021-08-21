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
