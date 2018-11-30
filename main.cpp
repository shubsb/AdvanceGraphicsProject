#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <GL/glew.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "apis/stb_image.h"

#include "ShaderProgram.h"
#include "ObjMesh.h"
#include "trackball.hpp"
#include "BezierCurveRoad.h"
#include "Boid.h"
#include "BoidManager.h"

#define NUM_SHADERS 4
#define SHADER_SWITCH_DELAY 5000

#define NUM_SKYBOXES 3
#define SKYBOX_SWITCH_DELAY 20000

int width, height;

GLuint programId;
GLenum positionBufferId;

bool animateShaders = false;

GLuint skyboxProgramId;
GLuint skybox_vao = 00;
GLuint skybox_vbo = 0;

unsigned int skyboxTextures[NUM_SKYBOXES];
unsigned int skyboxIndex;
int lastSkyboxTime = 0;
bool animateSkyboxes = false;

const int BOIDS_COUNT = 200;
BoidManager* manager;
float deltaTime = 0.0f;
float prevTime = 0.0f;

void drawObject(glm::mat4 m, int numVertices, unsigned int vao, bool drawEbo, unsigned int ebo,GLenum mode);
//Road(Cubic Bezier Curve)
unsigned int VBO[6], VAO[6];

unsigned int roadVBO,roadVAO;
//platform
GLuint platformTextureId;

unsigned int platformEBO = 0;

unsigned int carEBO = 0;
unsigned int tireL_EBO = 0;
unsigned int tireR_EBO = 0;
unsigned int boid_EBO = 0;

BezierCurveRoad road;

//the xyz coordiantes the make up the path of the road
std::vector<glm::vec3> vertexPositionData;

//vertices used to make the triangles to make the road aroud the beizier curve
std::vector<glm::vec3> verticesVector;

//used to find out the numVertices in an object when loading it
unsigned int numVertices, numVerticiesCar, numVerticiesTireL, numVerticiesTireR, numVerticesBoid;

glm::vec3 carPosition(0.0f,0.0f,0.0f);

float angle = 0.0f;
float lightOffsetY = 0.0f;
//glm::vec3 eyePosition(0, 10, 10);
bool animateLight = true;
bool rotateObject = true;

float theta = -0.4f;
float phi = -1.0f;
float cameraRadius = 100.0f;

static unsigned int createCubemap(std::vector<std::string> filenames);


static GLuint createTexture(std::string filename) {
  int width, height, numChannels;
  unsigned char *bitmap = stbi_load(filename.c_str(), &width, &height, &numChannels, 4);

  GLuint textureId;
  glGenTextures(1, &textureId);

  // make this texture active
  glBindTexture(GL_TEXTURE_2D, textureId);

  // specify the functions to use when shrinking/enlarging the texture image
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // send the data to OpenGL
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);

  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0);

  // we don't need the bitmap data any longer
  stbi_image_free(bitmap);
  return textureId;
}

static void createSkybox(void) {

  float skyboxPositions[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
  };
  unsigned int numVertices = 36;

  glGenVertexArrays(1, &skybox_vao);
  glGenBuffers(1, &skybox_vbo);
  glBindVertexArray(skybox_vao);
  glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
  glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(float) * 3, skyboxPositions, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
}

static void setupRoad(){
  vertexPositionData = road.getPath();
  verticesVector = road.getRoad();
 //std::cout << "got path" << std::endl;
  glBindVertexArray(VAO[0]);
  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * road.getNumCurves()* road.getNumVertecies() * 3, &vertexPositionData[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(roadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, roadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)  *verticesVector.size()*3,&verticesVector[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void *)0);
  glEnableVertexAttribArray(0);
}

static void setupCar(){

}

float leftTiereCenterX = 0;
float leftTiereCenterY = 0;
float leftTiereCenterZ = 0;

float rightTiereCenterX = 0;
float rightTiereCenterY = 0;
float rightTiereCenterZ = 0;



static void createGeomentry(void) {
  glGenVertexArrays(6, VAO);
  glGenBuffers(6, VBO);
  glGenVertexArrays(1, &roadVAO);
  glGenBuffers(1, &roadVBO);
  setupRoad();

    ObjMesh mesh, carMesh, tireMeshL, tireMeshR, boidMesh;

  //load the platform(ground)
  {
    mesh.load("Models/cube.obj", true, true);

    numVertices = mesh.getNumIndexedVertices();
    std::vector<float> platformData = mesh.getData();
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER,VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(GL_FLOAT)*8, &platformData[0], GL_STATIC_DRAW);
    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*8, (void*)0);
    glEnableVertexAttribArray(0);

    //normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*8, (void*)(sizeof(GL_FLOAT)*3));
    glEnableVertexAttribArray(1);

    //texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*8, (void*)0);
    glEnableVertexAttribArray(2);

    //indeces for platform
    unsigned int* indexData = mesh.getTriangleIndices();
    int numTriangles = mesh.getNumTriangles();

    glGenBuffers(1, &platformEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, platformEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numTriangles * 3, indexData, GL_STATIC_DRAW);
    //platformTextureId = createTexture("textures/planks.jpg");
  }


    carMesh.load("Models/Nissan_Body.obj", true, true);
    numVerticiesCar = carMesh.getNumIndexedVertices();
    std::vector<float> carData = carMesh.getData();
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, numVerticiesCar * sizeof(GL_FLOAT)*8, &carData[0],GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GL_FLOAT)*8, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(GL_FLOAT)*8, (void*)(sizeof(GL_FLOAT)*3));
    glEnableVertexAttribArray(1);

    unsigned int* indexDataCar = carMesh.getTriangleIndices();
    int numTrianglesCar = carMesh.getNumTriangles();

    glGenBuffers(1,&carEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, carEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numTrianglesCar * 3, indexDataCar, GL_STATIC_DRAW);

    tireMeshL.load("Models/Nissan_TireL.obj", true, true);
    numVerticiesTireL = tireMeshL.getNumIndexedVertices();
    std::vector<float> LTireData = tireMeshL.getData();
    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, numVerticiesTireL * sizeof(GL_FLOAT)*8, &LTireData[0],GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GL_FLOAT)*8, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(GL_FLOAT)*8, (void*)(sizeof(GL_FLOAT)*3));
    glEnableVertexAttribArray(1);

    unsigned int* indexDataTireL = tireMeshL.getTriangleIndices();
    int numTrianglesTireL = tireMeshL.getNumTriangles();

    glGenBuffers(1,&tireL_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tireL_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numTrianglesTireL * 3, indexDataTireL, GL_STATIC_DRAW);

    tireMeshR.load("Models/Nissan_TireR.obj", true, true);
    numVerticiesTireR = tireMeshR.getNumIndexedVertices();
    std::vector<float> RTireData = tireMeshR.getData();
    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, numVerticiesTireR * sizeof(GL_FLOAT)*8, &RTireData[0],GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GL_FLOAT)*8, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(GL_FLOAT)*8, (void*)(sizeof(GL_FLOAT)*3));
    glEnableVertexAttribArray(1);

    Vector3* tiereData = tireMeshL.getIndexedPositions();

      for(int k=0; k<numVerticiesTireL;k++){
        leftTiereCenterX += tiereData[k].x;
        leftTiereCenterY += tiereData[k].y;
        leftTiereCenterZ += tiereData[k].z;
      }
      leftTiereCenterX /= numVerticiesTireL;
      leftTiereCenterY /= numVerticiesTireL;
      leftTiereCenterZ /= numVerticiesTireL;

      tiereData = tireMeshR.getIndexedPositions();

        for(int k=0; k<numVerticiesTireR;k++){
          rightTiereCenterX += tiereData[k].x;
          rightTiereCenterY += tiereData[k].y;
          rightTiereCenterZ += tiereData[k].z;
        }
        rightTiereCenterX /= numVerticiesTireR;
        rightTiereCenterY /= numVerticiesTireR;
        rightTiereCenterZ /= numVerticiesTireR;

    unsigned int* indexDataTireR = tireMeshR.getTriangleIndices();
    int numTrianglesTireR = tireMeshR.getNumTriangles();

    glGenBuffers(1,&tireR_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tireR_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numTrianglesTireR * 3, indexDataTireR, GL_STATIC_DRAW);


    boidMesh.load("Models/bat.obj", true, true);
    numVerticesBoid = boidMesh.getNumIndexedVertices();
    std::vector<float> boidData = boidMesh.getData();
    glBindVertexArray(VAO[5]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, numVerticesBoid * sizeof(GL_FLOAT)*8, &boidData[0],GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GL_FLOAT)*8, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(GL_FLOAT)*8, (void*)(sizeof(GL_FLOAT)*3));
    glEnableVertexAttribArray(1);

    unsigned int* indexDataBoid = boidMesh.getTriangleIndices();
    int numTrianglesBoid = boidMesh.getNumTriangles();

    glGenBuffers(1,&boid_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boid_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numTrianglesBoid * 3, indexDataBoid, GL_STATIC_DRAW);


}

int duration =0;
int carPositionIndex =0;
glm::vec4 lightPos;

static void update(void) {
    int time = glutGet(GLUT_ELAPSED_TIME);

    int time2 = glutGet(GLUT_ELAPSED_TIME)/100;
    deltaTime = time2 - prevTime;
	  prevTime = time2;
    manager->UpdateBoids(deltaTime);

    if((time - duration) >  0.01) {
      carPosition = vertexPositionData[carPositionIndex];
      carPositionIndex++;

      angle += 20.0f;

      if(carPositionIndex >= vertexPositionData.size()-1){
        carPositionIndex =0;
      }
      if(angle >= 360.0f) {
        angle = 0.0f;
      }
      duration = time;
    }

    glm::vec4 lightPosDir[2] = {
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)
    };

    float lightIntensity[2]{
      1.0f,
      1.0f
    };

    GLuint lightposId = glGetUniformLocation(programId, "u_lightPosDir");
    glUniform4fv(lightposId, 2, &lightPosDir[0][0]);

    GLuint intensityId = glGetUniformLocation(programId, "u_lightIntensity");
    glUniform1fv(intensityId, 2, &lightIntensity[0]);


    if (animateSkyboxes) {


     if (duration > SKYBOX_SWITCH_DELAY) {
        skyboxIndex++;

        if (skyboxIndex >= NUM_SKYBOXES) {
           skyboxIndex = 0;
        }

        lastSkyboxTime = time;
     }
   }

    glutPostRedisplay();
}
glm::vec3 carColor = glm::vec3(0.976, 0.003, 0.192);
glm::vec3 platformColor = glm::vec3(0.094f, 0.447f, 0.074f);
glm::vec3 tireColor= glm::vec3(0.0f, 0.0f, 0.0f);

static void render(void) {
  int skyboxTexture = skyboxTextures[skyboxIndex];

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // activate our shader program
	//glUseProgram(programId);

   // turn on depth buffering
   glEnable(GL_DEPTH_TEST);

   float aspectRatio = (float)width / (float)height;
   glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);


   // view matrix - orient everything around our preferred view
   glm::mat4 view = glm::lookAt(
      eyePosition,
      glm::vec3(0,0,0),    // where to look
      glm::vec3(0,1,0)     // up
   );

   // model matrix: translate, scale, and rotate the model
   glm::mat4 baseMatrix = glm::mat4(1.0f);

   // int modelMatrix = glGetUniformLocation(programId,"model");
   // glUniformMatrix4fv(modelMatrix,1,GL_FALSE,glm::value_ptr(model));

   GLuint numlightsId = glGetUniformLocation(programId, "u_numLights");
   glUniform1i(numlightsId, 2);

   int viewMatrix = glGetUniformLocation(programId,"view");
   glUniformMatrix4fv(viewMatrix,1,GL_FALSE,glm::value_ptr(view));

   int projectionMatrix = glGetUniformLocation(programId,"projection");
   glUniformMatrix4fv(projectionMatrix,1,GL_FALSE,glm::value_ptr(projection));

   // the position of our camera/eye
   GLuint eyePosId = glGetUniformLocation(programId, "u_cameraPos");
   glUniform3f(eyePosId, eyePosition.x, eyePosition.y, eyePosition.z);

   // the position of our light
   //glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)
   GLuint lightPosId = glGetUniformLocation(programId, "u_LightPos");
   glUniform3f(lightPosId, 0,0,0);
   //std:: cout << eyePosition.x << " "<< eyePosition.y << " " << eyePosition.z << std::endl;
   // the colour of our object

   // the shininess of the object's surface
   GLuint shininessId = glGetUniformLocation(programId, "u_shininess");
   glUniform1f(shininessId, 25);

   {
    // draw the cube map sky box
    glBindVertexArray(skybox_vao);
    //texture sampler - a reference to the texture we've previously created
    GLuint skyboxTextureId  = glGetUniformLocation(skyboxProgramId, "u_TextureSampler");
    glActiveTexture(GL_TEXTURE0);  // texture unit 0
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    glUniform1i(skyboxTextureId, 0);

    glUseProgram(skyboxProgramId);

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glDisable(GL_CULL_FACE);

    // set model-view matrix

    GLuint skyboxMVMatrixId = glGetUniformLocation(skyboxProgramId, "u_MVMatrix");
    glUniformMatrix4fv(skyboxMVMatrixId, 1, GL_FALSE, &view[0][0]);

    // set projection matrix
    GLuint skyboxProjMatrixId = glGetUniformLocation(skyboxProgramId, "u_PMatrix");
    glUniformMatrix4fv(skyboxProjMatrixId, 1, GL_FALSE, &projection[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }

   glUseProgram(programId);
   glDepthMask(GL_TRUE);
   glEnable(GL_DEPTH_TEST);

   int modelMatrixZoom = glGetUniformLocation(programId,"model");
   glm::mat4 zoom = baseMatrix;
   zoom = glm::scale(zoom, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
   glUniformMatrix4fv(modelMatrixZoom,1,GL_FALSE,glm::value_ptr(zoom));


   glm::mat4 model = zoom;

    {   //road

      glm::mat4 roadMatrix = zoom;

      //draw the path for the car using bezier curve
      roadMatrix = glm::translate(roadMatrix, glm::vec3(0.0,1.0,0.0));
      drawObject(roadMatrix,road.getNumCurves()*road.getNumVertecies(),VAO[0],false,-1,GL_LINE_STRIP);

      /*
       *draws the road on the bezier curve but having some trouble with it
       *some of the bezier curves roads dont come out clean
       *so not displaying it
      */
      glBindVertexArray(roadVAO);
      //drawObject(roadMatrix,road.getNumNormals()*road.getNumCurves()*3,roadVAO,false,-1,GL_TRIANGLES);
    }

    {    //platform
    GLuint colourId = glGetUniformLocation(programId, "u_color");
    glUniform3fv(colourId, 1, &platformColor[0]);
    glm::mat4 platform = zoom;
    platform = glm::rotate(platform, glm::radians(-90.0f), glm::vec3(1, 0, 0)); // rotate about the z-axis
    //model = glm::translate(model, glm::vec3(1,1,60));
    platform = glm::scale(platform, glm::vec3(100.0f, 100.0f, 1.0f));
    drawObject(platform,numVertices,VAO[1],true,platformEBO,GL_TRIANGLES);
  }

  { //car
    GLuint colourId = glGetUniformLocation(programId, "u_color");
      glUniform3fv(colourId, 1, &carColor[0]);
    //Car
    model = zoom;
    model = glm::translate(model, carPosition);
    model = glm::translate(model,glm::vec3(0.0f,1.2f,0.0f));
    //model = glm::rotate(model,glm::radians(90.0f),glm::vec3(0.0f,1.0f,0.0f));
    //model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
    drawObject(model,numVerticiesCar,VAO[2],true,carEBO,GL_TRIANGLES);
}

{    //Tires
    GLuint colourId = glGetUniformLocation(programId, "u_color");
    glUniform3fv(colourId, 1, &tireColor[0]);
    //Back Left Tire
    glm::mat4 tireBL = glm::translate(model, glm::vec3(0.58f, -0.352f, -0.9f));
     tireBL = glm::translate(tireBL, glm::vec3(0.0f,leftTiereCenterY,leftTiereCenterZ));
    tireBL = glm::rotate(tireBL,glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
     tireBL = glm::translate(tireBL, glm::vec3(0.0f,-leftTiereCenterY,-leftTiereCenterZ));
    drawObject(tireBL,numVerticiesTireL,VAO[3],true,tireL_EBO, GL_TRIANGLES);

    //Front Left Tire
    glm::mat4 tireFL = glm::translate(model, glm::vec3(0.58f, -0.352f, 1.88f));
    tireFL = glm::translate(tireFL, glm::vec3(0.0f,leftTiereCenterY,leftTiereCenterZ));
    tireFL = glm::rotate(tireFL,glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
    tireFL = glm::translate(tireFL, glm::vec3(0.0f,-leftTiereCenterY,-leftTiereCenterZ));
    drawObject(tireFL,numVerticiesTireL,VAO[3],true,tireL_EBO, GL_TRIANGLES);


    //front right tire
    glm::mat4 tireFR = glm::translate(model, glm::vec3(-0.8f, -0.222f, -0.73f));
    tireFR = glm::translate(tireFR, glm::vec3(0.0f,rightTiereCenterY,rightTiereCenterZ));
    tireFR = glm::rotate(tireFR,glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
    tireFR = glm::translate(tireFR, glm::vec3(0.0f,-rightTiereCenterY,-rightTiereCenterZ));
    drawObject(tireFR,numVerticiesTireR,VAO[4],true,tireR_EBO, GL_TRIANGLES);

    //back right tire
    glm::mat4 tireBR = glm::translate(model, glm::vec3(-0.8f, -0.222f, 2.06f));
    tireBR = glm::translate(tireBR, glm::vec3(0.0f,rightTiereCenterY,rightTiereCenterZ));
    tireBR = glm::rotate(tireBR,glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
    tireBR = glm::translate(tireBR, glm::vec3(0.0f,-rightTiereCenterY,-rightTiereCenterZ));
    drawObject(tireBR,numVerticiesTireR,VAO[4],true,tireR_EBO, GL_TRIANGLES);
  }

  {
    //bat
    for (int i =0;  i < manager->boids.size(); i++){
    	Boid b = manager->boids[i];

      glm::vec3 direction = glm::normalize(b.velocity);
      float angle = glm::atan(direction.z, direction.x);

      glm::mat4 bat = zoom;
      // bat = glm::translate(bat, glm::vec3(b.position.x+(0.5*i),b.position.y,b.position.z));
      bat = glm::translate(bat, b.position);
      bat = glm::translate(bat, glm::vec3(0.0f,15.0f,0.0f));
      bat = glm::rotate(bat, angle+glm::radians(-90.0f), glm::vec3(1.0f, 1.0f, 1.0f));
      bat = glm::scale(bat, glm::vec3(0.005f));
      drawObject(bat,numVerticesBoid,VAO[5],true,boid_EBO, GL_TRIANGLES);
    }
  }


  	glutSwapBuffers();
}

static void reshape(int w, int h) {
   glViewport(0, 0, w, h);

   width = w;
   height = h;
}

static void drawObject(glm::mat4 m, int numVertices, unsigned int vao, bool drawEBO, unsigned int ebo,GLenum mode){
  glm::mat4 model = m;
  int modelMatrix = glGetUniformLocation(programId,"model");
  glUniformMatrix4fv(modelMatrix,1,GL_FALSE,glm::value_ptr(model));
  if(drawEBO){
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(mode, numVerticiesCar, GL_UNSIGNED_INT, (void*)0);
  }else{
    glBindVertexArray(vao);
    glDrawArrays(mode, 0, numVertices);
  }
}

static void keyboard(unsigned char key, int x, int y) {
    std::cout << "Key pressed: " << key << std::endl;
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutCreateWindow("CSCI 4110u Shading in OpenGL");
  glutIdleFunc(&update);
  glutDisplayFunc(&render);
  glutReshapeFunc(&reshape);
  glutMotionFunc(&drag);
  glutMouseFunc(&mouse);
  glutKeyboardFunc(&keyboard);

  glewInit();
  if(!GLEW_VERSION_2_0) {
      std::cerr << "OpenGL 2.0 not available" << std::endl;
      return 1;
  }
  std::cout << "Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
  std::cout << "Using OpenGL " << glGetString(GL_VERSION) << std::endl;
  createGeomentry();
  ShaderProgram program;
  program.loadShaders("shaders/lit_vertex.glsl", "shaders/lit_fragment.glsl");
  programId = program.getProgramId();

  // determine the initial camera position
   eyePosition.x = cameraRadius * sin(theta) * cos(phi);
   eyePosition.y = cameraRadius * sin(theta) * sin(phi);
   eyePosition.z = cameraRadius * cos(theta);

   std::vector<std::string> filenames3;
   filenames3.push_back("IceRiver/posz.jpg");
   filenames3.push_back("IceRiver/negz.jpg");
   filenames3.push_back("IceRiver/posy.jpg");
   filenames3.push_back("IceRiver/negy.jpg");
   filenames3.push_back("IceRiver/negx.jpg");
   filenames3.push_back("IceRiver/posx.jpg");

   skyboxTextures[2] = createCubemap(filenames3);
   skyboxIndex = 2;
   glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

   // create the scene geometry
   createSkybox();

   // test out quaternions
   glm::quat rot(1.0f, 0.0f, 0.0f, 0.0f);
   float sqrtHalf = sqrt(0.5f);
   glm::quat rotx(sqrtHalf, sqrtHalf, 0.0f, 0.0f);  // 90 degrees about x
   glm::quat rotz(sqrtHalf, 0.0f, 0.0f, sqrtHalf);  // 90 degrees about z
   rot *= rotx;
   rot *= rotz;

   //BoidManger
   manager = new BoidManager(BOIDS_COUNT);

   ShaderProgram skyboxProgram;
   skyboxProgram.loadShaders("shaders/skybox_vertex.glsl", "shaders/skybox_fragment.glsl");
  	skyboxProgramId = skyboxProgram.getProgramId();

   // output some basic help
   std::cout << "Controls:" << std::endl;
   std::cout << "\tLeft click + drag - rotate camera" << std::endl;
   std::cout << "\tRight click + drag - zoom camera" << std::endl;
   std::cout << "\tr - Enable/disable object auto-rotation" << std::endl;
   std::cout << "\ts - Enable/disable shader auto-switching" << std::endl;
   std::cout << "\tb - Enable/disable environment auto-switching" << std::endl;


  glutMainLoop();
  return 0;
}

static unsigned int createCubemap(std::vector<std::string> filenames) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, numChannels;
    for (unsigned int i = 0; i < filenames.size(); i++) {
        unsigned char *data = stbi_load(filenames[i].c_str(), &width, &height, &numChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << filenames[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glActiveTexture(GL_TEXTURE0);

    return textureID;
}
