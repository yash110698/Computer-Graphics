#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "ModelTriangle.h"
#include "RayTriangleIntersection.h"
#include "Camera.h"
#include "Colour.h"
#include <algorithm>
#include "libs/sdw/DrawingWindow.h"



void drawRaytraced(std::vector<Model>& world, DrawingWindow window, Camera camera,int SSMethod,int bounces);