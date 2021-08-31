TEST0 := $(BLD_DIR)/utest_app

$(TEST0) : $(OBJ_DIRS) $(OBJ)
	make -C srcs/app/test

.PHONY: run
run: test
	$(BLD_DIR)/utest_app

.PHONY: test
test: $(TEST0)
