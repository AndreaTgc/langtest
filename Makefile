# === Project settings ===
TARGET      := language
SRC_DIR     := src
BIN_DIR     := bin
OBJ_DIR     := $(BIN_DIR)/obj

# === Tools & flags ===
CC          := gcc
CFLAGS      := -Wall -Wextra -std=c99 -g
LDFLAGS     := 
FMT_FLAGS   := --options=.astylerc --suffix=none

# === Sources & objects ===
SRC_FILES   := $(wildcard $(SRC_DIR)/*/*.c)
OBJ_FILES   := $(patsubst $(SRC_DIR)/*/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# === Default target ===
all: $(BIN_DIR)/$(TARGET)

# === Link executable ===
$(BIN_DIR)/$(TARGET): $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS)

# === Compile each source incrementally ===
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# === Directory creation ===
$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

# === Convenience targets ===
.PHONY: run clean fmt

run: all
	@./$(BIN_DIR)/$(TARGET)

clean:
	@rm -rf $(BIN_DIR)

fmt:
	astyle $(FMT_FLAGS) $(SRC_DIR)/*/*.c $(SRC_DIR)/*/*.h