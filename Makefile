CXX = g++
#CXX = clang++
SRC_DIR = ./src
EXE = main
IMGUI_DIR = ./src/extern
BUILD_DIR = ./build

SOURCES = ./src/main.cpp 
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/imgui_impl_sdl2.cpp $(IMGUI_DIR)/imgui_impl_opengl3.cpp ./src/editor.cpp
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES)))) # $(addprefix $(BUILD_DIR)/, 
LINUX_GL_LIBS = -lGL -lGLEW -lGLU

CXXFLAGS = -std=c++11 -I$(IMGUI_DIR) -I$(IMGUI_DIR)
CXXFLAGS += -g -Wall -Wformat
LIBS =


LIBS += $(LINUX_GL_LIBS) -ldl `sdl2-config --libs`

CXXFLAGS += `sdl2-config --cflags`
CFLAGS = $(CXXFLAGS)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------


%.o:$(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $(BUILD_DIR)/$@ $<

%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $(BUILD_DIR)/$@ $<

%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $(BUILD_DIR)/$@ $<

all: $(EXE)
	@echo Build complete for $(EXE)

exec:
	$(CXX) -o $(EXE) $(addprefix $(BUILD_DIR)/, $(OBJS)) $(CXXFLAGS) $(LIBS)

editor:
	$(CXX) $(CXXFLAGS) -c -o $(BUILD_DIR)/editor.o $(SRC_DIR)/editor.cpp

main-exec:
	$(CXX) $(CXXFLAGS) -c -o $(BUILD_DIR)/main.o $(SRC_DIR)/main.cpp
	$(CXX) -o $(EXE) $(addprefix $(BUILD_DIR)/, $(OBJS)) $(CXXFLAGS) $(LIBS)

$(EXE): $(OBJS)
	$(CXX) -o  $@ $(addprefix $(BUILD_DIR)/, $^) $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) ./build/*.o
