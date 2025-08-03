CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread -O2
TARGET = future_promise_demo
SOURCES = base_task.cpp main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = base_task.h promise_task.h #user_api.h

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

debug: CXXFLAGS += -g -DDEBUG
debug: clean $(TARGET)

.SECONDARY: $(OBJECTS)
