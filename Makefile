CXX         := g++
SRC_DIR     := src
OBJ_DIR     := obj
BUILD_DIR   := build
SRC_FILES   := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES   := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
INCLUDE     := -lncurses -lpthread
FLAGS       := -g
ENVIRONMENT := $(shell basename $(abspath $(dir $$PWD)))
PRE_RUN     := mkdir -p build obj disk

$(BUILD_DIR)/$(ENVIRONMENT): $(OBJ_FILES)
	$(PRE_RUN)
	$(CXX) $(INCLUDE) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(FLAGS) -c -o $@ $<

run:
	./$(BUILD_DIR)/$(ENVIRONMENT)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)/*.o $(BUILD_DIR)/*

clean_obj:
	rm -rf $(OBJ_DIR)/*.o

clean_builds:
	rm -rf $(BUILD_DIR)/*