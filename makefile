CC = g++
FLAGS = -I/usr/include -I./include -Wall -pedantic
LIBS = -lGL -lglfw -lGLEW -lassimp

BUILD_DIR = build

DEPS = $(wildcard *.h)
SRC_FILES = $(wildcard *.cpp)
OBJS_FILES = $(SRC_FILES:.cpp=.o)

OBJS = $(addprefix build/, $(OBJS_FILES))

$(BUILD_DIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(FLAGS) $(LIBS)

thingy: $(OBJS)
	$(CC) -o $@ $^ $(FLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -r $(BUILD_DIR)/*.o
