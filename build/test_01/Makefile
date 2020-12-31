# Compiler options
CC=g++
CFLAGS=-c -Wall -O2 -I ../../src
LDFLAGS=
EXECUTABLE_NAME=testes

# Folders
SRC=src
BIN=bin
OBJ=bin/obj/a/a

# Files
SOURCE_FILES=$(shell find ../../ -maxdepth 3 -name "*.cpp")
EXECUTABLE_FILES = $(EXECUTABLE_NAME:%=$(BIN)/%)
OBJECT_FILES     = $(SOURCE_FILES:%.cpp=$(OBJ)/%.o)

build: $(EXECUTABLE_FILES)

clean:
	rm -r -f $(BIN)

.PHONY: build clean

$(EXECUTABLE_FILES): $(OBJECT_FILES)
	@$(CC) $(LDFLAGS) -o $@ $^
	@echo "Build successful!"

$(OBJECT_FILES): $(OBJ)/%.o: %.cpp
	@echo Compiling $<
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $<
