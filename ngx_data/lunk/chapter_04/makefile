CXX = gcc
CXXFLAGS += -g -Wall -Wextra

TARGET_FILE = travel_dir.c

SRC_FILE = $(TARGET_FILE).c
OBJ_FILE = $(TARGET_FILE).o
EXE_FILE = $(TARGET_FILE)

TARGETS = $(EXE_FILE)

CLEANUP = rm -i -f $(TARGETS)  *.o

all:$(TARGETS)
$(EXE_FILE): $(OBJ_FILE)
	$(CXX) $(CXXFLAGS) $^ -o $@
$(): $(SRC_FILE)
	$(CXX) -c $(CXXFLAGS) $<

.PHONLY:clean
clean:
	$(CLEANUP) 
