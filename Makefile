# Compile Flags
CC          = g++
LD          = g++
CCFLAGS     = -Wall -Wextra -std=c++17
OPT			= -O0
DEPFLAGS    = -MP -MD
THREADS		= -pthread

# Programs
PROG_CLIENT	= client
PROG_SERVER = server

# Folders
SRC_DIR		= ./src
BUILD_DIR	= ./build
BIN_DIR     = ./bin
INCLUDE_DIR	=. ./include

# CPP, Sources, Dependencies and Object files
CPP_LIST_CLIENT = client.cpp tcp_client.cpp 
SRC_LIST_CLIENT = $(patsubst %.cpp,$(SRC_DIR)/%.cpp,$(CPP_LIST_CLIENT))
OBJ_LIST_CLIENT = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_LIST_CLIENT))
DEP_LIST_CLIENT = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.d,$(SRC_LIST_CLIENT))

CPP_LIST_SERVER = server.cpp tcp_server.cpp
SRC_LIST_SERVER = $(patsubst %.cpp,$(SRC_DIR)/%.cpp,$(CPP_LIST_SERVER))
OBJ_LIST_SERVER = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_LIST_SERVER))
DEP_LIST_SERVER = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.d,$(SRC_LIST_SERVER))

INCLUDES = $(foreach dir, $(INCLUDE_DIR), $(addprefix -I, $(dir)))
-include $(DEP_LIST_CLIENT)
-include $(DEP_LIST_SERVER)

# Prettier Makefile
NO_COLOR=$(shell printf "%b" "\033[0m")
OK_COLOR=$(shell printf "%b" "\033[32;01m")
OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)

.PHONY: all clean $(PROG_CLIENT) $(PROG_SERVER)

all: $(BIN_DIR)/$(PROG_CLIENT) $(BIN_DIR)/$(PROG_SERVER)

$(BIN_DIR)/$(PROG_CLIENT): $(OBJ_LIST_CLIENT)
	@echo "Linking the target $(PROG_CLIENT) in $(BIN_DIR)"
	$(LD) -o $@ $^ $(THREADS)
	@echo ""

$(BIN_DIR)/$(PROG_SERVER): $(OBJ_LIST_SERVER)
	@echo "Linking the target $(PROG_CLIENT) in $(BIN_DIR)"
	$(LD) -o $@ $^ $(THREADS)
	@echo ""

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $< in $(BUILD_DIR) $(OK_STRING)"
	@$(CC) $(CCFLAGS) -g $(INCLUDES) $(OPT) $(DEPFLAGS) -c -o $@ $<

clean:
	rm -f $(BIN_DIR)/$(PROG_CLIENT) $(BIN_DIR)/$(PROG_SERVER) $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d