#include "Utils.h"
#include "ModelTriangle.h"
#include "CanvasTriangle.h"
#include "EventHandler.h"
#include "RayTriangleIntersection.h"
#include "DrawingWindow.h"
#include "Camera.h"
#include "Raytracer.h"
#include "Rasteriser.h"
#include <glm/glm.hpp>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;
using namespace glm;

#define WIDTH 640
#define HEIGHT 480

// #define WIDTH 960
// #define HEIGHT 720

void draw();
void update();

unordered_map<string,Colour> materials = readMTL("scene.mtl");
vector<vector<uint32_t>> image = readPPM("texture.ppm");
Model scene = Model(readOBJ("scene.obj",materials,1,1));
DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
vector<vector<uint32_t>> back = readPPM("ppm/bluebelt.ppm");
vector<vector<uint32_t>> tiger = readPPM("logo/texture.ppm");
vector<vector<glm::vec3>> swirl = readBump("swirl.ppm");
vector<vector<glm::vec3>> wall = readBump("wall.ppm");

unordered_map<string,Colour> logomaterials = readMTL("logo/materials.mtl");
// Centered on origin
Model logo = Model(readOBJ("logo/logo.obj",logomaterials,0.02,299),vec3(-5.6,0,0));
/////////////////////////////////////////////////////////////////////////////////////////////////////////
vector<vector<uint32_t>> rockTexture = readPPM("blueStones/blue1.ppm");
unordered_map<string,Colour> rockmaterials = readMTL("blueStones/blueStone16.mtl");
Model rock = Model(readOBJ("blueStones/blueStone16.obj",rockmaterials,2,500),vec3(0,0,0));

vector<vector<uint32_t>> rockTexture2 = readPPM("blueStones/blue2.ppm");
unordered_map<string,Colour> rockmaterials2 = readMTL("blueStones/blueStone12.mtl");
Model rock2 = Model(readOBJ("blueStones/blueStone12.obj",rockmaterials2,2,500),vec3(0,0,0));

vector<vector<uint32_t>> rockTexture3 = readPPM("blueStones/blue3.ppm");
unordered_map<string,Colour> rockmaterials3 = readMTL("blueStones/blueStone3.mtl");
Model rock3 = Model(readOBJ("blueStones/blueStone3.obj",rockmaterials3,2,500),vec3(0,0,0));

vector<vector<uint32_t>> rockTexture4 = readPPM("blueStones/blue4.ppm");
unordered_map<string,Colour> rockmaterials4 = readMTL("blueStones/blueStone1.mtl");
Model rock4 = Model(readOBJ("blueStones/blueStone1.obj",rockmaterials4,2,500),vec3(0,0,0));

vector<Model> world = {scene,logo,rock,rock2,rock3,rock4};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void animation();
void rockUpdate();
void start();
void logoRotate();
void orbit();

Camera camera = Camera(vec3(0,2,-6),mat3(1.0f),HEIGHT/2);
// Camera camera = Camera(vec3(0,106,-16),mat3(1.0f),HEIGHT/2);
//this camera pos overlooks the room

// Distance from centre of orbit
float orbitDist =  length(camera.position - vec3(0,2,1));
int renderer = 0;
int fna = 1;
int bounces = 0;
int sequence = 0;
int sloop = 0;
int SSMethod;
bool animate = false;

int main(int argc, char* argv[])
{
  logo.transform(glm::vec3(-10,0,8),0,0,0);
  rock.velocity = glm::vec3( (-2)/10, (-2)/(10), -(0.2+(rand()%20)/10));
  rock2.velocity = glm::vec3( (-2)/10, (-2)/(10), -(0.2+(rand()%20)/10));
  rock3.velocity = glm::vec3( (-2)/10, (-2)/(10), -(0.2+(rand()%20)/10));
  rock4.velocity = glm::vec3( (-2)/10, (-2)/(10), -(0.2+(rand()%20)/10));


  for(unsigned int i = 0; i<scene.faces.size(); i++){
    if (scene.faces[i].nameBump == "swirl.ppm"){
      scene.faces[i].bump = &swirl;
    }
    else if (scene.faces[i].nameBump == "wall.ppm"){
      scene.faces[i].bump = &wall;
    }
  }

  for(unsigned int i = 0; i<logo.faces.size(); i++){
    if( logo.faces[i].isTexture){
      logo.faces[i].image = &tiger;
    }
  }
  for(unsigned int i = 0; i<rock.faces.size(); i++){
    if( rock.faces[i].isTexture){
      rock.faces[i].image = &rockTexture;
    }
  }
  for(unsigned int i = 0; i<rock2.faces.size(); i++){
    if( rock2.faces[i].isTexture){
      rock2.faces[i].image = &rockTexture2;
    }
  }
  for(unsigned int i = 0; i<rock3.faces.size(); i++){
    if( rock3.faces[i].isTexture){
      rock3.faces[i].image = &rockTexture3;
    }
  }
  for(unsigned int i = 0; i<rock4.faces.size(); i++){
    if( rock4.faces[i].isTexture){
      rock4.faces[i].image = &rockTexture4;
    }
  }

  // start();
  // logo_.texture(tiger);
  // rock.texture(rockTexture);
  SDL_Event event;
  while(true){
    // We MUST poll for events - otherwise the window will freeze !
    if(window.pollForInputEvents(&event)) handleEvent(event,window,camera,image,renderer,SSMethod,fna,bounces,animate);
    update();
    window.clearPixels();
    window.clearDepth();
    drawBack(window,WIDTH,HEIGHT,back);
    // logoRotate();
    draw();
    // Need to render the frame at the end, or nothing actually gets shown on the screen !
    window.renderFrame();
  }
}

void draw()
{
  // orbit();
  switch (renderer){
    case 0:
      drawWireframe(world,window,camera);
      break;

    case 1:

      drawRasterised(world,window,camera);
      break;

    case 2:

      drawRaytraced(world,window,camera,SSMethod,bounces);
      // drawRasterised(world,window,camera,rockTexture);
      break;

  }
}
/*
// void orbit(){
//   drawRaytraced(model,window,camera,SSMethod,bounces);
//   camera.lookat(glm::vec3(0,2,1));
//   camera.right(0.1);

//   // Ensures we have an   char filename[50];
//   sprintf(filename,"captures/%d.ppm",fna);
//   writePPM(filename, window);orbit rather than spiral
//   float distfromcentre = length(camera.position - vec3(0,2,1));
//   float scale = distfromcentre/orbitDist;
//   camera.forward(scale - 1);

//   // Write output
//   char filename[50];
//   sprintf(filename,"captures/%d.ppm",fna);
//   writePPM(filename, window);
//   fna++;
// }*/


void rockUpdate(){
  if (rock.shift.z <= -10){
    rock.shift = glm::vec3( rand()%20-10,   rand()%20-10,  (50+rand()%20));
    rock.velocity = glm::vec3( (-2)/10, (-2)/(10), -(0.2+(rand()%20)/10));
  }
  rock.update(false);
  rock.transform(vec3(0,0,0),(rand()%10)/100.0,(rand()%10)/100.0,(rand()%10)/100.0);
  world[2] = rock;

  if (rock2.shift.z <= -10){
    rock2.shift = glm::vec3( rand()%20-10,   rand()%20-10,  (50+rand()%20));
    rock2.velocity = glm::vec3( (-2)/10, (-2)/(10), -(0.2+(rand()%20)/10));
  }
  rock2.update(false);
  rock2.transform(vec3(0,0,0),(rand()%10)/100.0,(rand()%10)/100.0,(rand()%10)/100.0);
  world[3] = rock2;

  if (rock3.shift.z <= -10){
    rock3.shift = glm::vec3( rand()%20-10,   rand()%20-10,  (50+rand()%20));
    rock3.velocity = glm::vec3( (-2)/10, (-2)/(10), -(0.2+(rand()%20)/10));
  }
  rock3.update(false);
  rock3.transform(vec3(0,0,0),(rand()%10)/100.0,(rand()%10)/100.0,(rand()%10)/100.0);
  world[4] = rock3;

  if (rock4.shift.z <= -10){
    rock4.shift = glm::vec3( rand()%20-10,   rand()%20-10,  (50+rand()%20));
    rock4.velocity = glm::vec3( (-2)/10, (-2)/(10), -(0.2+(rand()%20)/10));
  }
  rock4.update(false);
  rock4.transform(vec3(0,0,0),(rand()%10)/100.0,(rand()%10)/100.0,(rand()%10)/100.0);
  world[5] = rock4;
}



void animation(){
  scene.update(true);
  world[0] = scene;
  logo.transform(vec3(0,0,0),0,0.02,0);
  logo.update(true);
  world[1] = logo;

  // Model temp;
  rockUpdate();
}
void update()
{
  animation();
  if (animate){
    switch(sequence){
      // Start in box
      //   camera.position = glm::vec3(0,1,0);
      case 0:
        camera.position = glm::vec3(0,101,10);
        sequence++;
        break;

      case 1:
        if (sloop < 25){
          camera.updateRotation(0,0.0628,0);
          sloop++;
        }
        else {sequence++;sloop=0;}
        break;

      case 2:
        if (camera.position.x > -9.3){
          camera.forward(0.2);
        }
        else sequence++;
        break;

      case 3:
        camera.position = glm::vec3(9.3,101,10);
        sequence++;
        break;

      case 4:
        if (camera.position.x > -9.3){
          camera.forward(0.2);
        }
        else sequence++;
        break;

      case 5:
        camera.position = glm::vec3(9.3,101,10);
        sequence++;
        break;

      case 6:
        if (camera.position.x >0){
          camera.forward(0.2);
        }
        else sequence++;
        break;

      case 7:
        if (sloop < 25){
          camera.updateRotation(0,0.0628,0);
          sloop++;
        }
        else {sequence++;sloop=0;}
        break;
      
      case 8:
        if (camera.position.z >0.4){
          camera.forward(0.2);
        }
        else sequence++;
        break;

      case 9:
        camera.position = glm::vec3(-10,1,23.5);
        sequence++;
        break;

      case 10:
        if (camera.position.z > 20){
          camera.forward(0.2);
        }
        else sequence++;
        break;

      case 11:
        sloop++;
        if (sloop > 120){
          sequence++;sloop=0;
        }
        break;

      case 12:
        std::exit(0);


      // case 4:
      //   if (sloop < 25){
      //     camera.updateRotation(0,-0.0601,0);
      //     sloop++;
      //   }
      //   else {sequence++;sloop=0;}
      //   break;

      // case 5:
      //   if (camera.position.z < 23.5){
      //     camera.forward(0.2);
      //   }
      //   else sequence++;
      //   break;

      // case 6:
      //   sequence++;
      //   break;

      // case 7:
      //   if (camera.position.z < 10){
      //     camera.forward(0.2);
      //   }
      //   else{
      //     sequence++;
      //   }
      //   break;

      // case 8:
      //   if (sloop < 25){
      //     camera.updateRotation(0,-0.1204,0);
      //     sloop++;
      //   }
      //   else {sequence++;sloop=0;}
      //   break;
      // case 9:


    }
    char filename[50];
    sprintf(filename,"captures/%d.ppm",fna);
    writePPM(filename, window);
    fna++;
  }
  // Function for performing animation (shifting artifacts or moving the camera)
    // camera.right(0.1);
}
