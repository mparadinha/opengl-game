CC = g++
INCLUDE_DIRS = -I/usr/include -I./include
FLAGS = $(INCLUDE_DIRS) -Wall -pedantic
LIBS = -lGL -lglfw -lGLEW -lassimp

BUILD_DIR = build

DEPS = $(wildcard *.h)
SRC_FILES = $(wildcard *.cpp)
OBJS_FILES = $(SRC_FILES:.cpp=.o)

OBJS = $(addprefix $(BUILD_DIR)/, $(OBJS_FILES))

$(BUILD_DIR)/%.o: %.cpp $(DEPS)
	mkdir -p $(BUILD_DIR)
	$(CC) -c -o $@ $< $(FLAGS) $(LIBS)

thingy: $(OBJS)
	$(CC) -o $@ $^ $(FLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -r $(BUILD_DIR)/*.o
