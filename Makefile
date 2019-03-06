COMPILER := g++
CFLAGS := -Wall -Wextra -Wpedantic
LDFLAGS :=

SOURCES := src/Ultima.cpp src/UI.cpp src/Semaphore.cpp src/Scheduler.cpp
#$(wildcard src/*.cpp)
EXEC := exec

all: $(EXEC)
	./$(EXEC)

$(EXEC): #$(SOURCES)
	$(COMPILER) $(CFLAGS) $(SOURCES) -o $(EXEC)

clean:
	rm $(EXEC)
