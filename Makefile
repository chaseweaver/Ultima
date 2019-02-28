COMPILER := g++
CFLAGS := -Wall -Wextra -Wpedantic
LDFLAGS :=

SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst src%,obj%, $(patsubst %.cpp,%.o,$(SOURCES)))
EXEC := exec

all: $(EXEC)
	./$(EXEC)

$(OBJECTS): $(SOURCES)
	$(COMPILER) $(CFLAGS) -c $< -o $@

$(EXEC): $(OBJECTS)
	$(COMPILER) $(CFLAGS) $(OBJECTS) -o $(EXEC)

clean:
	rm $(EXEC) $(OBJECTS)
