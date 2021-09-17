#pragma once

#include "CanvasPoint.h"
#include "ModelTriangle.h"
#include "Colour.h"
#include "Camera.h"
#include "DrawingWindow.h"
#include "Utils.h"
#include <vector>
#include <glm/glm.hpp>


void drawWireframe(std::vector<Model>& model,DrawingWindow window, Camera camera);


// void drawRasterised(std::vector<ModelTriangle> model,DrawingWindow window, Camera camera);
void drawRasterised(std::vector<Model>& model,DrawingWindow window, Camera camera);
