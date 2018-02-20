CC = g++
INCLUDE_DIRS = -I/usr/include -I./include
FLAGS = $(INCLUDE_DIRS) -Wall -pedantic
LIBS = -ldl -lglfw -lassimp
SRC_DIRS = . systems
HEADER_DIRS = . systems components

BUILD_DIR = build

DEPS = $(wildcard *.h)
SRC_FILES = $(wildcard *.cpp) $(wildcard systems/*.cpp)
OBJS_FILES = $(SRC_FILES:.cpp=.o)

OBJS = $(addprefix $(BUILD_DIR)/, $(OBJS_FILES))

$(BUILD_DIR)/%.o: %.cpp $(DEPS) make_build_dir
	$(CC) -c -o $@ $< $(FLAGS) $(LIBS)

all:
	python build.py

clean:
	python build.py clean

force:
	python build.py force

.PHONY: clean make_build_dir

make_build_dirs:
	$(foreach src_dir, $(SRC_DIRS), mkdir -p $(src_dir)/$(BUILD_DIR);)

#thingy: $(OBJS)
#	$(CC) -o $@ $^ $(FLAGS) $(LIBS)

#clean:
#	rm -r $(BUILD_DIR)/*.o
