#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera(glm::vec3 p, glm::mat3 r, float f)
{
    position = p;
    rotation = r;
    focal    = f;
    near = 1;
    far = 100;
    frustum = {};
}


void Camera::updateRotation(float X, float Y, float Z){
  glm::mat3 xrot = glm::mat3(1,0,0,0,cos(X),sin(X),0,-sin(X),cos(X));
  glm::mat3 yrot = glm::mat3(cos(Y),0,-sin(Y),0,1,0,sin(Y),0,cos(Y));
  glm::mat3 zrot = glm::mat3(cos(Z),sin(Z),0,-sin(Z),cos(Z),0,0,0,1);
  rotation = xrot * yrot * zrot * rotation;
}

void Camera::lookat(glm::vec3 point){
  glm::vec3 forward = glm::normalize(point-position);
  glm::vec3 right   = glm::cross(glm::vec3(0,1,0),forward);
  glm::vec3 up      = glm::cross(forward,right);

  rotation[0][0] = right.x;
  rotation[1][0] = right.y;
  rotation[2][0] = right.z;
  rotation[0][1] = up.x;
  rotation[1][1] = up.y;
  rotation[2][1] = up.z;
  rotation[0][2] = forward.x;
  rotation[1][2] = forward.y;
  rotation[2][2] = forward.z;
}

void Camera::updateFrustum(int width,int height){
  std::vector<Plane> f;
  glm::vec3 right =   glm::vec3(1,0,0) * rotation;
  glm::vec3 up =      glm::vec3(0,1,0) * rotation;
  glm::vec3 forward = glm::vec3(0,0,1) * rotation;

  glm::vec3 fc = position + forward * far;

  glm::vec3 nc = position + forward * near;

  glm::vec3 tmp;
  float fovy = 2 * std::atan(height/(focal*2));
  float aspect = (float)width/(float)height;

  float hh = std::tan(fovy/2) * near;
  float hw = hh * aspect;

// Good
  glm::vec3 NEAR = forward;
  glm::vec3 NEARP = nc;
  f.push_back(Plane(NEAR,NEARP));
  glm::vec3 FAR = -forward;
  glm::vec3 FARP = fc;
  f.push_back(Plane(FAR,FARP));

// Bad
  tmp = (nc + (up * hh)) - position;
  glm::vec3 TOP = -glm::normalize(glm::cross(tmp, right));
  glm::vec3 TOPP = (nc + (up * hh));
  // std::cout << TOP.x << "," << TOP.y << "," << TOP.z << std::endl;
  // std::cout << TOPP.x << "," << TOPP.y << "," << TOPP.z << std::endl;

  f.push_back(Plane(TOP,TOPP));


  tmp = (nc - (up * hh)) - position;
  glm::vec3 BOTTOM = -glm::normalize(glm::cross(right, tmp));
  glm::vec3 BOTTOMP = (nc - (up * hh));
  f.push_back(Plane(BOTTOM,BOTTOMP));


  tmp = (nc + (right * hw)) - position;
  glm::vec3 LEFT = glm::normalize(glm::cross(tmp, up));
  glm::vec3 LEFTP = (nc + (right * hw));
  f.push_back(Plane(LEFT,LEFTP));


  tmp = (nc - (right * hw)) - position;
  glm::vec3 RIGHT = glm::normalize(glm::cross(up, tmp));
  glm::vec3 RIGHTP = (nc - (right * hw));
  f.push_back(Plane(RIGHT,RIGHTP));

  frustum = f;
}

std::vector<ModelTriangle> Camera::cull(std::vector<ModelTriangle> faces){
  std::vector<ModelTriangle> clippedFaces;
  std::vector<Plane> frustum = getFrustum();
  for (unsigned int i = 0; i < faces.size(); i++){
    std::vector<glm::vec3> vertices {faces[i].vertices[0], faces[i].vertices[1], faces[i].vertices[2]};
    unsigned int out;
    bool store = true;
    for (unsigned int j = 0; j < frustum.size(); j++){
      out = 0;
      for(unsigned int k = 0; k < vertices.size();k++){
        if (frustum[j].distance(vertices[k]) <  0){
          out++;
        }
      }
      if (out == 3){
        store = false;
        break;
      }
    }
    if (store){
      clippedFaces.push_back(faces[i]);
    }
  }

  return clippedFaces;
}

std::vector<Plane> Camera::getFrustum(){
  return frustum;
}


void Camera::forward(float amount){
  position += (glm::vec3(0,0,amount) * rotation);
}
void Camera::right(float amount){
  position += (glm::vec3(amount,0,0) * rotation);
}
void Camera::up(float amount){
  position += (glm::vec3(0,amount,0) * rotation);
}

Plane::Plane(glm::vec3 n, glm::vec3 p){
  normal = n;
  point = p;
}
float Plane::distance(glm::vec3 p){
  float dist = glm::dot(normal, p-point);
  if (abs(dist) < 0.001) return 0;
  return dist;
}
