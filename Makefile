NAME = webserv
CXX = clang++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98
LINT = cpplint
LINTFLAGS = --recursive
RMF = rm -rf

INC_DIR = include
SRC_DIR = src
OBJ_DIR = obj

_OBJ = main.o
OBJ := $(patsubst %, $(OBJ_DIR)/%, $(_OBJ))

_DEPS =
DEPS := $(patsubst %, $(INC_DIR)/%, $(_DEPS))

.PHONY: all
all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c -o $@ $<

$(OBJ): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

.PHONY: lint
lint:
	$(LINT) $(LINTFLAGS) .

.PHONY: clean
clean:
	$(RMF) $(OBJ_DIR)

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)

.PHONY: re
re: fclean all
