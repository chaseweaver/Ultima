CXX := g++
INC := -lncurses -lpthread
SRCDIR := src
OUT := ultima.out
EXEC := exec

CPPFILES=$(wildcard $(SRCDIR)/*.cpp)
OBJFILES=$(CPPFILES:.cpp=.o)

all: $(OUT)

$(OUT): $(OBJFILES)
	$(CXX) $(INC) -o $@ $^ -g

%.o: %.cpp
	$(CXX) -c -o $@ $< -g

run: $(OUT)
	$(EXEC) ./$(OUT)

.PHONY: clean
clean:
	rm -rf $(SRCDIR)/*.o *.out