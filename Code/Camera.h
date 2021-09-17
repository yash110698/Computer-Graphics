#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "libs/sdw/ModelTriangle.h"
#include <math.h>       /* tan */
#include <iostream>

class Plane
{
  public:
    glm::vec3 normal;
    glm::vec3 point;

  Plane(glm::vec3 n, glm::vec3 p);
  float distance(glm::vec3 p);
};

class Camera
{
  private: 
    float near;
    float far;
  public:
    glm::vec3 position;
    glm::mat3 rotation;
    float focal;
    std::vector<Plane> frustum;
    std::vector<ModelTriangle> cull(std::vector<ModelTriangle>);
  
    
  Camera();
  Camera(glm::vec3 p, glm::mat3 r, float f);
  void updateRotation(float X, float Y, float Z);
  void updateFrustum(int width, int height);
  std::vector<Plane> getFrustum();
  void lookat(glm::vec3 point);
  void forward(float amount);
  void right(float amount);
  void up(float amount);

};
