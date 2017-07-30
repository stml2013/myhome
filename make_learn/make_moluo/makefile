
CC = gcc
CXX_FLAGS += -g -Wall -Wextra
ASM_FLAGS = -S
COV_FLAGS = -fprofile-arcs -ftest-coverage  
LINK_GCOV = -lgcov   

TARGET_FILE = test
CPP_FILE = $(TARGET_FILE).c
PCM_FILE = $(TARGET_FILE).i
ASM_FILE = $(TARGET_FILE).s
OBJ_FILE = $(TARGET_FILE).o #obj_file
EXE_FILE = $(TARGET_FILE)

TARGET = $(EXE_FILE) $(OBJ_FILE) $(ASM_FILE) $(PCM_FILE)
CLEAN_TARGET = -rm -f $(TARGET)

all: $(TARGET)
$(TARGET):
$(EXE_FILE): $(OBJ_FILE)
	$(CC) $^ -o $@
$(OBJ_FILE): $(ASM_FILE)
	$(CC) -c $^ -o $@
$(ASM_FILE): $(PCM_FILE)
	$(CC) $(ASM_FLAGS) $^ -o $@
$(PCM_FILE): $(CPP_FILE)
	$(CC) -E $^ -o $@
	@echo $(CPP_FILE)

.PHONY:clean
clean:
	$(CLEAN_TARGET)
	
	
