CXX := g++
INC := -lncurses -lpthread
SRC := src/
OBJ := obj/

CPPFILES=$(wildcard $(SRC)*.cpp)
OBJFILES=$(CPPFILES:.cpp=.o)

res.out: $(OBJFILES)
	$(CXX) $(INC) -o $@ $^ -g

%.o: %.cpp
	$(CXX) -c -o $@ $< -g

clean:
	rm *.o *.out