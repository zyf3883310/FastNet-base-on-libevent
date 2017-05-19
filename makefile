INC_DIR:= ./LibBase/Include/ ./FastNetLib/Include/  ./TestServer/Include
SRC_DIR:= ./LibBase/Source/  ./FastNetLib/Source/  ./TestServer/Source
SRCS:=$(wildcard ./LibBase/Source/*.cpp)  $(wildcard ./FastNetLib/Source/*.cpp) $(wildcard ./TestServer/Source/*.cpp)
OBJS:= $(patsubst %.cpp, %.o, $(SRCS))
LIBS:= -lpthread -levent

CXX:=g++

CXXFLAGS:= -w -g -std=c++11 $(addprefix -I, $(INC_DIR)) $(LIBS) -Wno-deprecated

EXE:=./Bin/TestServer

$(EXE):$(OBJS)
	$(CXX) -o $(EXE) $(SRCS) $(CXXFLAGS)
	
clean:
	rm -rf $(EXE)
	rm -rf $(OBJS)
