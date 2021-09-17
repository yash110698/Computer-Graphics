#pragma once

#include "Utils.h"
#include "Colour.h"
#include "DrawingWindow.h"
#include "Camera.h"
#include "glm/glm.hpp"

// Camera is passed as a reference
void handleEvent(SDL_Event event, DrawingWindow window, Camera& camera, vector<vector<uint32_t>> image, int& renderer,int& SSMethod, int& fna,int& fancy,bool& animate);