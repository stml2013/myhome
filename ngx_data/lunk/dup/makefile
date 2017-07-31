CXX = gcc
CXXFLAGS = -g -Wall -Wextra

TARGETS_FILE = dup

SRC_FILE = $(TARGETS_FILE).c
OBJ_FILE = $(TARGETS_FILE).o
EXE_FILE = $(TARGETS_FILE)

CLEANUP = rm -i -f $(TARGETS) *.o

all: $(TARGETS)
$(EXT_FILE): $(OBJ_FILE)
	$(CXX) $(CXXFLAGS) $^ -o $@
$(OBJ_FILE): $(SRC_FILE)
	$(CXX) -c $(CXXFLAGS) $<

.PHONY:clean
clean:
	$(CLEANUP) 
