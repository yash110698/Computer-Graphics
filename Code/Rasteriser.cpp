#include "Rasteriser.h"

CanvasPoint project(glm::vec3 point, Camera camera, int width, int height)
{
  glm::vec3 d = camera.rotation * (point - camera.position);
  int x = round(camera.focal * (d.x / d.z));
  int y = round(camera.focal * (d.y / d.z));
  CanvasPoint projected = CanvasPoint(x + width / 2, height / 2 - y, 1 / d.z);
  //passing depth of vertices
  if (projected.x<0) projected.x = 0;
  else if (projected.x>width-1) projected.x = width-1;
  if (projected.y<0) projected.y = 0;
  else if (projected.y>height-1) projected.y = height-1;
  return projected;
}

std::vector<CanvasPoint> clip(DrawingWindow window, Camera camera, std::vector<glm::vec3> &points,std::vector<TexturePoint> &tp)
{
  std::vector<CanvasPoint> projected;
  std::vector<Plane> frustum = camera.getFrustum();
  for (uint i = 0; i < frustum.size(); i++)
  {
      // glm::vec3 point = camera.rotation * (points[j] - camera.position);
    vector<glm::vec3> out;
    vector<TexturePoint> outTP;
    vector<glm::vec3> in;
    vector<TexturePoint> inTP;
    for (uint j=0; j < 3; j++)
    {
      if (frustum[i].distance(points[j]) <  0){
        out.push_back(points[j]);
        outTP.push_back(tp[j]);
      // std::cout << "Clipped at " << i << " DOT: " <<  frustum[i].distance(points[j]) << std::endl;
      }
      else{
        in.push_back(points[j]);
        inTP.push_back(tp[j]);
      }
    }
    // Completely out of range
    if (out.size() == 3){
      return projected;
    }
    if (out.size() == 2){
      for( int k = 0; k<2; k++){
        glm::vec3 np = glm::normalize(in[0] - out[k]);
        glm::vec2 npTP;
        if (inTP[0].point - outTP[k].point == glm::vec2(0)){
          npTP = glm::vec2(0);
        }
        else{
          npTP = glm::normalize(inTP[0].point - outTP[k].point);
        }
        float scale = frustum[i].distance(out[k])  / glm::dot(frustum[i].normal,np);
        float tpscale = (scale/glm::length(in[0]-out[k])) * glm::length(inTP[0].point - outTP[k].point);
        glm::vec3 newP = out[k] - (scale * np);
        TexturePoint newTP = TexturePoint(outTP[k].point - (tpscale * npTP));
        // std::cout << "This should be 0: (" << frustum[i].distance(new1) << ")" << new1.x <<"," << new1.y <<","<< new1.z << std::endl;
        std::replace(points.begin(),points.end(),out[k],newP);
        std::replace(tp.begin(),tp.end(),outTP[k],newTP);
      }
    }
    if (out.size() == 1){
      std::vector<glm::vec3> newPoints;
      std::vector<TexturePoint> newTPs;

      for( int k = 0; k<2; k++){
        glm::vec3 np = glm::normalize(in[k] - out[0]);
        glm::vec2 npTP;
        if (inTP[k].point - outTP[0].point == glm::vec2(0)){
          npTP = glm::vec2(0);
        }
        else{
          npTP = glm::normalize(inTP[k].point - outTP[0].point);
        }
        float scale = frustum[i].distance(out[0])  / glm::dot(frustum[i].normal,np);
        float tpscale = (scale/glm::length(in[k]-out[0])) * glm::length(inTP[k].point - outTP[0].point);

        glm::vec3 newP = out[0] - (scale * np);
        TexturePoint newTP = TexturePoint(outTP[0].point - (tpscale * npTP));
        newPoints.push_back(newP);
        newTPs.push_back(newTP);

      }
      //Recursion time
      vector<glm::vec3> left = {in[0], in[1], newPoints[1]};
      vector<TexturePoint> leftTP = {inTP[0], inTP[1], newTPs[1]};
      vector<glm::vec3> right = {newPoints[0],newPoints[1], in[0]};
      vector<TexturePoint> rightTP = {newTPs[0], newTPs[1], inTP[0]};
      vector<CanvasPoint> Pleft = clip(window,camera,left,leftTP);
      vector<CanvasPoint> Pright = clip(window,camera,right,rightTP);
      Pleft.insert(Pleft.end(),Pright.begin(),Pright.end());
      return Pleft;

    }
  }
  CanvasPoint temp;
  for (int i = 0; i<3; i++){
    temp = project(points[i], camera, window.width, window.height);
    // std::cout << tp[i] << std::endl;
    temp.texturePoint = tp[i];
    projected.push_back(temp);
  }
  return projected;
}


void drawWireframe(std::vector<Model>& world, DrawingWindow window, Camera camera)
{
  camera.updateFrustum(window.width,window.height);
  std::vector<ModelTriangle> faces;
  for (uint i = 0; i<world.size(); i++){
    faces.insert(faces.end(),world[i].faces.begin(),world[i].faces.end());
  }
  // #pragma omp parallel for
  for(uint j =0; j<faces.size(); j++){
    std::vector<glm::vec3> vertices = {faces[j].vertices[0], faces[j].vertices[1], faces[j].vertices[2]};
    std::vector<TexturePoint> tp = {faces[j].texture[0], faces[j].texture[1], faces[j].texture[2]};

    std::vector<CanvasPoint> projections = clip(window, camera, vertices, tp);
    if (projections.size() != 0)
    {
      for(uint k=0; k<projections.size(); k+=3){
        stroked(window, projections[k], projections[k+1], projections[k+2], faces[j].colour);
      }
    }
  }
}

void drawRasterised(std::vector<Model>& world, DrawingWindow window, Camera camera)
{
  camera.updateFrustum(window.width,window.height);
  std::vector<ModelTriangle> faces;
  for (uint i = 0; i<world.size(); i++){
    faces.insert(faces.end(),world[i].faces.begin(),world[i].faces.end());
  }
  #pragma omp parallel for
  for(uint j =0; j<faces.size(); j++){
    std::vector<glm::vec3> vertices = {faces[j].vertices[0], faces[j].vertices[1], faces[j].vertices[2]};
    std::vector<TexturePoint> tp = {faces[j].texture[0], faces[j].texture[1], faces[j].texture[2]};

    std::vector<CanvasPoint> projections = clip(window, camera, vertices, tp);
    if (projections.size() != 0)
    {
      for(uint k=0; k<projections.size(); k+=3)
      {

        if(faces[j].isTexture)
        {
          // filled(window, projections[k], projections[k+1], projections[k+2], model[i].colour);
          texturedTriangle(window,faces[j].image,projections[k], projections[k+1], projections[k+2]);
        }
        else{
          filled(window, projections[k], projections[k+1], projections[k+2], faces[j].colour);

        }
      }
    }
  }
}
