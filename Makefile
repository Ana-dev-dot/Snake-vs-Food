
CC=gcc
CXX=g++
CC_FLAGS=-g -static -Wall -DSFML_STATIC
CXX_FLAGS=-g -static -std=c++17 -Wall -O2 -DSFML_STATIC

TARGET=sfSnakePro
BUILD_DIR=build
SRC_DIR= src src/screen src/element
SRCS = $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.cpp))
OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(SRCS))))
DEPS = $(OBJS:.o=.d)

INC_DIR = ./src D:/SFML-2.5.1/include
LIB_DIR = D:/SFML-2.5.1/lib
LIBS = sfml-graphics-s sfml-window-s sfml-audio-s sfml-system-s opengl32 freetype winmm gdi32 openal32 flac vorbisenc vorbisfile vorbis ogg

INC_FLAGS = $(addprefix -I,$(INC_DIR))
LIB_FLAGS = $(addprefix -L,$(LIB_DIR)) $(addprefix -l,$(LIBS))
LD_FLAGS  =

all: $(TARGET)
.PHONY: clean

$(TARGET): $(OBJS)
	$(CXX) $(CXX_FLAGS) $(OBJS) -o $@ $(LD_FLAGS) $(LIB_FLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXX_FLAGS) $(INC_FLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CC_FLAGS) $(INC_FLAGS) -c $< -o $@

clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@if exist $(TARGET).exe del $(TARGET).exe
	@echo $(OBJS)

-include $(DEPS)
