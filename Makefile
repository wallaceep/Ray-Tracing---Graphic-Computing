APP_NAME = raytracer

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

TARGET = $(BIN_DIR)/$(APP_NAME)

CXX = g++
CXXFLAGS = -Wall -Wextra -g -std=c++17 -I$(INC_DIR)

SRCS = $(wildcard $(SRC_DIR)/*.cpp)

OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

ifeq ($(OS),Windows_NT)
	# Comandos Windows
	MKDIR_OBJ = if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	MKDIR_BIN = if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	RM = del /Q /S
	FIX_PATH = $(subst /,\,$1)
	EXEC_CMD = $(TARGET)
else
	# Comandos Linux/Mac
	MKDIR_OBJ = mkdir -p $(OBJ_DIR)
	MKDIR_BIN = mkdir -p $(BIN_DIR)
	RM = rm -rf
	FIX_PATH = $1
	EXEC_CMD = ./$(TARGET)
endif


all: directories $(TARGET)

directories:
	@$(MKDIR_OBJ)
	@$(MKDIR_BIN)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)
	@echo "Build completo: $(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(call FIX_PATH,$(OBJ_DIR)/*.o)
	$(RM) $(call FIX_PATH,$(TARGET).exe)
	$(RM) $(call FIX_PATH,$(TARGET))

ARGS = $(filter-out run,$(MAKECMDGOALS))

run: all
	@echo "Rodando $(APP_NAME) de $(BIN_DIR)..."
	$(EXEC_CMD) $(ARGS)

%:
	@:

.PHONY: all clean run directories