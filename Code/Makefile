PROJECT_NAME = Renderer

# Define the names of key files
SOURCE_FILE = $(PROJECT_NAME).cpp
OBJECT_FILE = build/$(PROJECT_NAME).o
EXECUTABLE = $(PROJECT_NAME)
# There is definitely a better way than this
WINDOW_SOURCE = libs/sdw/DrawingWindow.cpp
WINDOW_OBJECT = build/DrawingWindow.o
UTILS_SOURCE = libs/sdw/Utils.cpp
UTILS_OBJECT = build/Utils.o
COLOUR_SOURCE = libs/sdw/Colour.cpp
COLOUR_OBJECT = build/Colour.o
CANVASPOINT_SOURCE = libs/sdw/CanvasPoint.cpp
CANVASPOINT_OBJECT = build/CanvasPoint.o
TEXTUREPOINT_SOURCE = libs/sdw/TexturePoint.cpp
TEXTUREPOINT_OBJECT = build/TexturePoint.o
MODELTRIANGLE_SOURCE = libs/sdw/ModelTriangle.cpp
MODELTRIANGLE_OBJECT = build/ModelTriangle.o

# Project Objects
CAMERA_SOURCE = Camera.cpp
CAMERA_OBJECT = build/Camera.o
EVENTHANDLER_SOURCE = EventHandler.cpp
EVENTHANDLER_OBJECT = build/EventHandler.o
RAYTRACER_SOURCE = Raytracer.cpp
RAYTRACER_OBJECT = build/Raytracer.o
RASTERISER_SOURCE = Rasteriser.cpp
RASTERISER_OBJECT = build/Rasteriser.o

# Build settings
COMPILER = g++
COMPILER_OPTIONS = -c -pipe -Wall -std=c++11
DEBUG_OPTIONS = -ggdb -g3
FUSSY_OPTIONS = -Werror -pedantic
SANITIZER_OPTIONS = -O1 -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer
SPEEDY_OPTIONS = -Ofast -funsafe-math-optimizations -march=native -fopenmp
LINKER_OPTIONS =

# Set up flags
SDW_COMPILER_FLAGS := -I./libs/sdw
GLM_COMPILER_FLAGS := -I./libs/glm
# If you have a manual install of SDL, you might not have sdl2-config. Compiler flags should be something like: -I/usr/local/include/SDL2 -D_THREAD_SAFE
SDL_COMPILER_FLAGS := $(shell sdl2-config --cflags)
# If you have a manual install of SDL, you might not have sdl2-config. Linker flags should be something like: -L/usr/local/lib -lSDL2
SDL_LINKER_FLAGS := $(shell sdl2-config --libs)
SDW_LINKER_FLAGS := $(WINDOW_OBJECT) $(UTILS_OBJECT) $(CANVASPOINT_OBJECT) ${TEXTUREPOINT_OBJECT} $(COLOUR_OBJECT) ${MODELTRIANGLE_OBJECT}
PROJECT_LINKER_FLAGS := $(RAYTRACER_OBJECT) $(CAMERA_OBJECT) $(EVENTHANDLER_OBJECT) $(RASTERISER_OBJECT)
default: diagnostic

# Rule to help find errors (when you get a segmentation fault)
# NOTE: Needs the "Address Sanitizer" library to be installed in order to work (might not work on lab machines !)
diagnostic: window utils canvaspoint texturepoint
	$(COMPILER) $(COMPILER_OPTIONS) $(FUSSY_OPTIONS) $(SANITIZER_OPTIONS) -o $(OBJECT_FILE) $(SOURCE_FILE) $(SDL_COMPILER_FLAGS) $(SDW_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS)
	$(COMPILER) $(LINKER_OPTIONS) $(FUSSY_OPTIONS) $(SANITIZER_OPTIONS) -o $(EXECUTABLE) $(OBJECT_FILE) $(SDW_LINKER_FLAGS) $(SDL_LINKER_FLAGS) $(PROJECT_LINKER_FLAGS)
	./$(EXECUTABLE)

# Rule to compile and link for production release
production: window utils canvaspoint texturepoint
	$(COMPILER) $(COMPILER_OPTIONS) -o $(OBJECT_FILE) $(SOURCE_FILE) $(SDL_COMPILER_FLAGS) $(SDW_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS)
	$(COMPILER) $(LINKER_OPTIONS) -o $(EXECUTABLE) $(OBJECT_FILE) $(SDW_LINKER_FLAGS) $(SDL_LINKER_FLAGS) $(PROJECT_LINKER_FLAGS)
	./$(EXECUTABLE)

# Rule to compile and link for use with a debugger
debug: window utils canvaspoint texturepoint
	$(COMPILER) $(COMPILER_OPTIONS) $(DEBUG_OPTIONS) -o $(OBJECT_FILE) $(SOURCE_FILE) $(SDL_COMPILER_FLAGS) $(SDW_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS)
	$(COMPILER) $(LINKER_OPTIONS) $(DEBUG_OPTIONS) -o $(EXECUTABLE) $(OBJECT_FILE) $(SDW_LINKER_FLAGS) $(SDL_LINKER_FLAGS) $(PROJECT_LINKER_FLAGS)
	./$(EXECUTABLE)

# Rule to build for high performance executable
speedy: raytracer rasteriser camera event utils window canvaspoint texturepoint colour modeltriangle
	$(COMPILER) $(COMPILER_OPTIONS) $(SPEEDY_OPTIONS) -o $(OBJECT_FILE) $(SOURCE_FILE) $(SDL_COMPILER_FLAGS) $(SDW_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS)
	$(COMPILER) $(LINKER_OPTIONS) $(SPEEDY_OPTIONS) -o $(EXECUTABLE) $(OBJECT_FILE) $(SDW_LINKER_FLAGS) $(SDL_LINKER_FLAGS) $(PROJECT_LINKER_FLAGS)
	./$(EXECUTABLE)

# Rule for building the DisplayWindow
window:
	$(COMPILER) $(COMPILER_OPTIONS) -o $(WINDOW_OBJECT) $(WINDOW_SOURCE) $(SDL_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS)

utils:
	$(COMPILER) $(COMPILER_OPTIONS) -o $(UTILS_OBJECT) $(UTILS_SOURCE) $(SDL_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS)

canvaspoint:
	$(COMPILER) $(COMPILER_OPTIONS) -o $(CANVASPOINT_OBJECT) $(CANVASPOINT_SOURCE) $(SDL_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS)

texturepoint:
	$(COMPILER) $(COMPILER_OPTIONS) -o $(TEXTUREPOINT_OBJECT) $(TEXTUREPOINT_SOURCE) $(SDL_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS)

modeltriangle:
	$(COMPILER) $(COMPILER_OPTIONS) -o $(MODELTRIANGLE_OBJECT) $(MODELTRIANGLE_SOURCE) $(SDL_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS)

colour:
	$(COMPILER) $(COMPILER_OPTIONS) $(SPEEDY_OPTIONS) -o $(COLOUR_OBJECT) $(COLOUR_SOURCE) $(SDL_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS)

camera:
	$(COMPILER) $(COMPILER_OPTIONS) -o $(CAMERA_OBJECT) $(CAMERA_SOURCE) $(SDL_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS)

event:
	$(COMPILER) $(COMPILER_OPTIONS) -o $(EVENTHANDLER_OBJECT) $(EVENTHANDLER_SOURCE) $(SDL_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS) $(SDW_COMPILER_FLAGS)

raytracer:
	$(COMPILER) $(COMPILER_OPTIONS) $(SPEEDY_OPTIONS) -o $(RAYTRACER_OBJECT) $(RAYTRACER_SOURCE) $(SDL_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS) $(SDW_COMPILER_FLAGS)

rasteriser:
	$(COMPILER) $(COMPILER_OPTIONS) $(SPEEDY_OPTIONS) -o $(RASTERISER_OBJECT) $(RASTERISER_SOURCE) $(SDL_COMPILER_FLAGS) $(GLM_COMPILER_FLAGS) $(SDW_COMPILER_FLAGS)

# Files to remove during clean
clean:
	rm $(OBJECT_FILE)
	rm $(EXECUTABLE)
