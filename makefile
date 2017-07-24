CC = g++
INCLUDE_DIRS = -I/usr/include -I./include
FLAGS = $(INCLUDE_DIRS) -Wall -pedantic
LIBS = -lGL -ldl -lglfw -lassimp

BUILD_DIR = build

DEPS = $(wildcard *.h)
SRC_FILES = $(wildcard *.cpp)
OBJS_FILES = $(SRC_FILES:.cpp=.o)

OBJS = $(addprefix $(BUILD_DIR)/, $(OBJS_FILES))

$(BUILD_DIR)/%.o: %.cpp $(DEPS) make_build_dir
	$(CC) -c -o $@ $< $(FLAGS) $(LIBS)

thingy: $(OBJS)
	$(CC) -o $@ $^ $(FLAGS) $(LIBS)

.PHONY: clean make_build_dir

clean:
	rm -r $(BUILD_DIR)/*.o

make_build_dir:
	mkdir -p $(BUILD_DIR)
