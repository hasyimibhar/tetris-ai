OBJ_DIR=obj
SRC_DIR=src
OUT_DIR=bin

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

RM=rm -f
RMRF=rm -rf
CXX=g++
CXXFLAGS=-I$(SRC_DIR) -std=c++14 -Ofast

LDFLAGS=
LDLIBS=

all: tetris

tetris: $(OBJS) $(OUT_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(OUT_DIR)/tetris $(OBJS) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(OBJ_DIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

clean:
	$(RMRF) $(OBJ_DIR)
	$(RMRF) $(OUT_DIR)
