#include "Raytracer.h"

std::vector<glm::vec3>Lights {glm::vec3(0,3.5,5), glm::vec3(-10,3.5,15),glm::vec3(0,110,10)};
std::vector<glm::vec3> lightColours = {50.f * glm::vec3(1,1,1),50.f * glm::vec3(1,1,1),800.f * glm::vec3(1,1,1)};
glm::vec3 indirectLighting = 0.2f * glm::vec3(1,1,1);
ModelTriangle nullT = ModelTriangle();
int lbounces = 0;
float pi = 3.1415f; //Approximation

glm::vec3 Lighting(const RayTriangleIntersection& i,std::vector<ModelTriangle>& triangles);
// bool closestIntersection(glm::vec3 start, glm::vec3 dir,
//                          std::vector<ModelTriangle>& triangles,
//                          RayTriangleIntersection& intersection);
bool closestIntersection(glm::vec3 start, glm::vec3 dir,
                         std::vector<ModelTriangle>& triangles,
                         RayTriangleIntersection& intersection,
                         ModelTriangle self);
bool calculateIntersectionWithBounces(DrawingWindow window,
                         glm::vec3 start, glm::vec3 dir,
                         std::vector<ModelTriangle>& triangles,
                         RayTriangleIntersection& intersection);


glm::vec3 interpolateNormal(const RayTriangleIntersection& i){
  std::vector<std::vector<glm::vec3>>* BumpMap = i.intersectedTriangle.bump;
  TexturePoint textures[3] {i.intersectedTriangle.texture[0],i.intersectedTriangle.texture[1],i.intersectedTriangle.texture[2]};
  float e1Scale = i.e1; //v0 to v1
  float e2Scale = i.e2; //v0 to v2
  // std::cout << textures[0] << " " << textures[1] << std::endl;

  glm::vec2 d1 = (textures[1].point - textures[0].point);
  glm::vec2 d2 = (textures[2].point - textures[0].point);

  glm::vec2 e1 = e1Scale * d1;
  glm::vec2 e2 = e2Scale * d2;
  // std::cout << e1  << "  "  << e2 << std::endl;
  glm::vec2 Coord = textures[0].point +  e1+e2;

  Coord *= (*BumpMap).size();

  if (Coord.x < 0) Coord.x = 0;
  if (Coord.x > (*BumpMap).size()-1) Coord.x = (*BumpMap).size()-1;
  if (Coord.y < 0) Coord.y = 0;
  if (Coord.y > (*BumpMap).size()-1) Coord.y = (*BumpMap).size()-1;


  return (*BumpMap)[Coord.y][Coord.x];
}

glm::vec3 Lighting(const RayTriangleIntersection& i,std::vector<ModelTriangle>& triangles,glm::vec3 viewdir){
  RayTriangleIntersection nearestSurface;
  glm::vec3 lighting = indirectLighting;
  bool found = false;
  for(uint j = 0; j< Lights.size();j++){
    glm::vec3 r = Lights[j] - i.intersectionPoint;
    bool lfound = closestIntersection(i.intersectionPoint,glm::normalize(r),triangles,nearestSurface,i.intersectedTriangle);
    if ( lfound && nearestSurface.distance < glm::length(r) && !(nearestSurface.intersectedTriangle.colour.name == "Red")){
      continue; //Shadow 
    }
    else{
      glm::vec3 n; 
      if (!i.intersectedTriangle.isBump){
        n = i.intersectedTriangle.normal;
      }
      else{
        n = interpolateNormal(i);
      }
      float percent = std::max(glm::dot(glm::normalize(r),n),0.f);
      glm::vec3 diffuse = (lightColours[j] * (percent/(4*pi*glm::dot(r,r))));
      glm::vec3 reflected = r - 2*(glm::dot(r,n))*n; 
      glm::vec3 specular;
      float si = i.intersectedTriangle.colour.specularIndex;
      if (si != 0){
        specular = powf(std::max(glm::dot(glm::normalize(reflected),glm::normalize(viewdir)),0.f),si) * glm::vec3(1);
      }
      else{
        specular = glm::vec3(0);
      }
      // std::cout << specular << std::endl;
      found = true; //Diffuse
      lighting += specular  + diffuse;
    }
  }
  if (!found){
    return 0.5f * indirectLighting;
  }

  return lighting;
}

// Möller–Trumbore intersection
bool closestIntersection(glm::vec3 start, glm::vec3 dir,
                         std::vector<ModelTriangle>& triangles,
                         RayTriangleIntersection& intersection,
                         ModelTriangle self){
  float bestT = 1000;
  dir = glm::normalize(dir);
  for (unsigned int i = 0; i < triangles.size(); i++){
    glm::vec3 e1 = triangles[i].vertices[1] - triangles[i].vertices[0];
    glm::vec3 e2 = triangles[i].vertices[2] - triangles[i].vertices[0];
    // glm::vec3  b = start - triangle.vertices[0];
    glm::vec3 pvec = glm::cross(dir,e2);
    float det = glm::dot(e1,pvec);
    if (abs(det)<=0.01){
      continue;
    }
    float invDet = 1 / det; 


    // std::cout << u << std::endl;
    glm::vec3 tvec = start - triangles[i].vertices[0]; 
    float u = glm::dot(tvec,pvec) * invDet;
    if (u < 0 || u > 1) continue; 
    glm::vec3 qvec = glm::cross(tvec,e1);
    float v = glm::dot(dir,qvec) * invDet;
    if (v < 0 || u + v > 1) continue; 
    float t = glm::dot(e2,qvec) * invDet;
    if (t < bestT && t > 0 && triangles[i] != self){
      bestT = t;
      intersection = RayTriangleIntersection(start + t * dir,t,u,v,triangles[i]);
    }
  }
    return bestT < 1000;
}


glm::vec3 refract(glm::vec3 dir, glm::vec3 n, float n1, float n2){
  float cos1 = glm::dot(dir,n);
  float nr = n1/n2;
  return (nr * cos1 - sqrt(1-(nr*nr)*(1-(cos1*cos1))))*n -nr*dir;
}

bool mirror(glm::vec3 dir,
            std::vector<ModelTriangle>& triangles,
            RayTriangleIntersection& intersection){
  glm::vec3 n = intersection.intersectedTriangle.normal;
  dir = glm::normalize(dir);                     
  dir = dir - 2*(glm::dot(dir,n))*n;
  // THIS NEEDS Multiple bounces
  // if (calculateIntersectionWithBounces(window,intersection.intersectionPoint,dir,triangles,intersection,intersection.intersectedTriangle)) return true;
  if (closestIntersection(intersection.intersectionPoint,dir,triangles,intersection,intersection.intersectedTriangle)){
    return true;
  }
  return false;
}

bool glass(glm::vec3 dir,
            std::vector<ModelTriangle>& triangles,
            RayTriangleIntersection& intersection,
            std::string name){
  float r = 1.52f;
  glm::vec3 n = intersection.intersectedTriangle.normal;
  dir = glm::normalize(dir);
  dir = refract(dir,n,1,r);
  // std::cout << dir.x << dir.y << dir.z << std::endl;
  closestIntersection(intersection.intersectionPoint,dir,triangles,intersection,intersection.intersectedTriangle);
  dir = glm::normalize(dir);
  dir = refract(dir,n,r,1);
  while (intersection.intersectedTriangle.colour.name==name){
    if (!closestIntersection(intersection.intersectionPoint,dir,triangles,intersection,intersection.intersectedTriangle)){
      return false;
    }
  }
  return intersection.intersectedTriangle.colour.name!=name;
}

bool portal(glm::vec3 dir,
            std::vector<ModelTriangle>& triangles,
            RayTriangleIntersection& intersection,
            char PortalNumber){

  glm::vec3 cros;
  glm::mat3 vx  ;
  glm::mat3 r   ;
  for (uint i=0; i<triangles.size(); i++){
    if (triangles[i].colour.name[6] == PortalNumber && triangles[i] != intersection.intersectedTriangle){
      //Assume a correspondence between points
      float c = glm::dot(intersection.intersectedTriangle.normal,triangles[i].normal);
      if (c != -1){
        cros = glm::cross(-intersection.intersectedTriangle.normal,triangles[i].normal);
        vx   = glm::mat3(0.f,-cros.z,cros.y,cros.z,0.f,-cros.x,-cros.y,cros.x,0.f);
        r    = glm::mat3() + vx + (1/(1+c)) * vx * vx;
      }
      else{
        // If the direction is the opposite
        r = glm::mat3();
      }
      
      float u = glm::dot(intersection.intersectionPoint-intersection.intersectedTriangle.vertices[0],glm::normalize(intersection.intersectedTriangle.vertices[1] - intersection.intersectedTriangle.vertices[0]));
      float v = glm::dot(intersection.intersectionPoint-intersection.intersectedTriangle.vertices[0],glm::normalize(intersection.intersectedTriangle.vertices[2] - intersection.intersectedTriangle.vertices[0]));
      glm::vec3 nu = u * glm::normalize(triangles[i].vertices[1] - triangles[i].vertices[0]);
      glm::vec3 nv = v * glm::normalize(triangles[i].vertices[2] - triangles[i].vertices[0]);
      glm::vec3 start = triangles[i].vertices[0] +  nu + nv;
      // dir = -triangles[i].normal;
      // std::cout << u <<" " << v << std::endl;
      // std::cout << glm::dot(start,glm::cross((triangles[i].vertices[1] - triangles[i].vertices[0]),(triangles[i].vertices[2] - triangles[i].vertices[0]))) << std::endl;
      // return closestIntersection(start,dir,triangles,intersection,triangles[i]);
      return closestIntersection(start,r*dir,triangles,intersection,nullT);

    }
  }
  return false;
}


bool calculateIntersectionWithBounces(DrawingWindow window,
                         glm::vec3 start, glm::vec3 dir,
                         std::vector<ModelTriangle>& triangles,
                         RayTriangleIntersection& intersection){
  glm::vec3 n;

  if (lbounces == 0) return closestIntersection(start,dir,triangles,intersection,nullT);
  int llbounces = lbounces;
  if (closestIntersection(start,dir,triangles,intersection,nullT)){
    while (llbounces > 0){
      if (intersection.intersectedTriangle.colour.name == "Blue"){
        if (!mirror(dir,triangles,intersection)) return false;
      }
      else if (intersection.intersectedTriangle.colour.name == "Red"){
        if (!glass(dir,triangles,intersection,"Red")) return false;
      }
      else if (intersection.intersectedTriangle.colour.name.find("Portal") != std::string::npos ){
        if (!portal(dir,triangles,intersection,intersection.intersectedTriangle.colour.name[6])) return false;
      }
      llbounces--;
    }
    return true;
  }
  return false;
}

bool calculateIntersectionWithBounces(DrawingWindow window,
                         glm::vec3 start, glm::vec3 dir,
                         std::vector<ModelTriangle>& triangles,
                         RayTriangleIntersection& intersection,
                         ModelTriangle self){

  for (unsigned int i = 0; i<triangles.size(); i++){
    if (triangles[i] == self){
      triangles[i] = triangles.back();
      triangles.pop_back();
      break;
    }
  }
  return calculateIntersectionWithBounces(window,start,dir,triangles,intersection);
}

void simple(DrawingWindow window,std::vector<ModelTriangle>& model,int x, int y, Camera camera){
  RayTriangleIntersection intersection;
  glm::vec3 dir = glm::vec3(x-window.width/2,window.height/2-y,camera.focal) * camera.rotation;
  if (calculateIntersectionWithBounces(window,camera.position,dir,model,intersection)){
    window.setPixelColour(x,y,intersection.intersectedTriangle.colour.pack(Lighting(intersection,model,dir))); 
  }
}

void NbyNGrid(DrawingWindow window,std::vector<ModelTriangle>& model,int x, int y, float n, Camera camera){
  RayTriangleIntersection intersection;
  float offset = 1 / (2*n);
  glm::vec3 dir;
  Colour colour = Colour(0,0,0);
  int count = 0;
  for (int i = 1; i<n; i++){
    for (int j = 1; j<n; j++){
      dir = glm::vec3(x-window.width/2+(i * 1/n)-offset,window.height/2-y-(j*1/n)-offset,camera.focal) * camera.rotation;
      if (calculateIntersectionWithBounces(window,camera.position,dir,model,intersection)){
        colour = colour + Lighting(intersection,model,dir) * intersection.intersectedTriangle.colour;
        count++;
      }
    }
  }
  if (count != 0) window.setPixelColour(x,y,(colour/count).pack());
}

void NbyNChecker(DrawingWindow window,std::vector<ModelTriangle>& model,int x, int y, float n, Camera camera){
  RayTriangleIntersection intersection;
  float offset = 1 / (2*n);
  glm::vec3 dir;
  Colour colour = Colour(0,0,0);
  int count = 0;
  for (int i = 1; i<n; i++){
    for (int j = 1; j<n; j++){
      if ((i+j)%2 == 0){
      dir = glm::vec3(x-window.width/2+(i * 1/n)-offset,window.height/2-y-(j*1/n)-offset,camera.focal) * camera.rotation;
        if (calculateIntersectionWithBounces(window,camera.position,dir,model,intersection)){
          colour = colour + Lighting(intersection,model,dir) * intersection.intersectedTriangle.colour;
          count++;
        }
      }
    }
  }
  if (count != 0) window.setPixelColour(x,y,(colour/count).pack());
}

void samplePixels(DrawingWindow window,std::vector<ModelTriangle>& model,int x, int y, Camera camera, int SSMethod){
  switch (SSMethod){
    case 0: //Simple
      simple(window,model,x,y,camera);
      break;
    case 1: //2by2 grid
      NbyNGrid(window,model,x,y,2.f,camera);
      break;
    case 2: //4by4 Checker --THIS APPEARS BEST
      NbyNChecker(window,model,x,y,4.f,camera); 
      break;
    case 3: //4by4 grid
      NbyNGrid(window,model,x,y,4.f,camera);
      break;
    case 4: //8by8 Checker
      NbyNChecker(window,model,x,y,8.f,camera);
      break;
  }
}

void drawRaytraced(std::vector<Model>& world, DrawingWindow window, Camera camera,int SSMethod, int bounces){
  lbounces = bounces;
  // camera.updateFrustum(window.width,window.height);
  std::vector<ModelTriangle> faces;
  for (uint i = 0; i<world.size(); i++){
    faces.insert(faces.end(),world[i].faces.begin(),world[i].faces.end());
  }
  // faces = camera.cull(faces);
  #pragma omp parallel for
  for(int y=0; y<window.height ;y++) {
    for(int x=0; x<window.width ;x++) {
      samplePixels(window,faces,x,y,camera,SSMethod);
      }
    }
  }
