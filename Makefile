CXX := g++
INC := -lncurses -lpthread
SRCDIR := src
OUT := ultima.out
EXEC := exec
FLAGS := 

CPPFILES=$(wildcard $(SRCDIR)/*.cpp)
OBJFILES=$(CPPFILES:.cpp=.o)

all: $(OUT)

$(OUT): $(OBJFILES)
	$(CXX) $(INC) -o $@ $^ -g

%.o: %.cpp
	$(CXX) $(FLAGS) -c -o $@ $< -g

run: $(OUT)
	$(EXEC) ./$(OUT)

.PHONY: clean
clean:
	rm -rf $(SRCDIR)/*.o *.out