CXX         := g++
SRC_DIR     := src
OBJ_DIR     := obj
BUILD_DIR   := build
SRC_FILES   := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES   := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
INCLUDE     := -lncurses -lpthread
FLAGS       := -g # -Wall -Wextra -Wreorder
ENVIRONMENT := $(shell basename $(abspath $(dir $$PWD)))

$(BUILD_DIR)/$(ENVIRONMENT): $(OBJ_FILES)
	$(CXX) $(INCLUDE) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(FLAGS) -c -o $@ $<

run:
	./$(BUILD_DIR)/$(ENVIRONMENT)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)/*.o $(BUILD_DIR)/*