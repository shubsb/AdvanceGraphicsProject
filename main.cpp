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

#define NUM_SHADERS 4
#define SHADER_SWITCH_DELAY 5000

#define NUM_SKYBOXES 3
#define SKYBOX_SWITCH_DELAY 20000

int width, height;

GLuint programId;
GLenum positionBufferId;

bool animateShaders = false;

GLuint skyboxProgramId;

GLuint skybox_vbo = 0;

unsigned int skyboxTextures[NUM_SKYBOXES];
unsigned int skyboxIndex;
int lastSkyboxTime = 0;
bool animateSkyboxes = false;


//Road(Cubic Bezier Curve)
unsigned int VBO[3], VAO[3];

//platform
unsigned int platformPositionVAO = 0;
unsigned int platformTextureCoordsVAO = 0;
unsigned int platformNormalVAO = 0;

unsigned int platformPositionVBO = 0;
unsigned int platformTextureCoordsVBO = 0;
unsigned int platformNormalVBO = 0;

unsigned int platformEBO = 0;

BezierCurveRoad road;
//the xyz coordiantes the make up the road/path
std::vector<glm::vec3> vertexPositionData;

//used to find out the numVertices in an object when loading it
unsigned int numVertices;

float angle = 0.0f;
float lightOffsetY = 0.0f;
//glm::vec3 eyePosition(0, 10, 10);
bool animateLight = true;
bool rotateObject = true;

float theta = -0.4f;
float phi = -1.0f;
float cameraRadius = 100.0f;

static unsigned int createCubemap(std::vector<std::string> filenames);

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

  glGenBuffers(1, &skybox_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
  glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(float) * 3, skyboxPositions, GL_STATIC_DRAW);
}

static void setupRoad(){
  vertexPositionData = road.getPath();

  glBindVertexArray(VAO[0]);
  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * road.getNumCurves()* road.getNumVertecies() * 3, &vertexPositionData[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void *)0);
  glEnableVertexAttribArray(0);
}

static void createGeomentry(void) {
  glGenVertexArrays(3, VAO);
  glGenBuffers(3, VBO);
  setupRoad();

  ObjMesh mesh;

  //load the platform(ground)
  mesh.load("Models/square.obj", true, true);

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
//indeces for platform
    unsigned int* indexData = mesh.getTriangleIndices();
    int numTriangles = mesh.getNumTriangles();

    glGenBuffers(1, &platformEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, platformEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numTriangles * 3, indexData, GL_STATIC_DRAW);

}

static void update(void) {
    int milliseconds = glutGet(GLUT_ELAPSED_TIME);

    if (animateSkyboxes) {
     int duration = milliseconds - lastSkyboxTime;

     if (duration > SKYBOX_SWITCH_DELAY) {
        skyboxIndex++;

        if (skyboxIndex >= NUM_SKYBOXES) {
           skyboxIndex = 0;
        }

        lastSkyboxTime = milliseconds;
     }
   }

    // rotate the shape about the y-axis so that we can see the shading
    if (rotateObject) {
      float degrees = (float)milliseconds / 80.0f;
      angle = degrees;
    }

    // move the light position over time along the x-axis, so we can see how it affects the shading
    if (animateLight) {
      float t = milliseconds / 1000.0f;
      lightOffsetY = sinf(t) * 100;
    }

    glutPostRedisplay();
}

static void render(void) {
  int skyboxTexture = skyboxTextures[skyboxIndex];

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // activate our shader program
	glUseProgram(programId);

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
   glm::mat4 model = glm::mat4(1.0f);
   model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0)); // rotate about the y-axis
   //model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0)); // rotate about the y-axis
  //  model = glm::scale(model, glm::vec3(10.0f, 10.0f, 0.0f));

   // model-view-projection matrix
   glm::mat4 mvp = projection * view * model;
   GLuint mvpMatrixId = glGetUniformLocation(programId, "u_MVPMatrix");
   glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);

   // model-view matrix
   glm::mat4 mv = view * model;
   GLuint mvMatrixId = glGetUniformLocation(programId, "u_MVMatrix");
   glUniformMatrix4fv(mvMatrixId, 1, GL_FALSE, &mv[0][0]);

   // the position of our camera/eye
   GLuint eyePosId = glGetUniformLocation(programId, "u_EyePosition");
   glUniform3f(eyePosId, eyePosition.x, eyePosition.y, eyePosition.z);

   // the position of our light
   //glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)
   GLuint lightPosId = glGetUniformLocation(programId, "u_LightPos");
   glUniform3f(lightPosId, 1, 8 + lightOffsetY, -2);

   // the colour of our object
   GLuint diffuseColourId = glGetUniformLocation(programId, "u_DiffuseColour");
   glUniform4f(diffuseColourId, 0.3, 0.2, 0.8, 1.0);

   // the shininess of the object's surface
   GLuint shininessId = glGetUniformLocation(programId, "u_Shininess");
   glUniform1f(shininessId, 25);

   glUseProgram(programId);

   //road
   // glBindVertexArray(VAO[0]);
   // glDrawArrays(GL_LINE_STRIP, 0, road.getNumCurves()*road.getNumVertecies());
   //
   // //platform
   // glBindVertexArray(VAO[1]);
   // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, platformEBO);
   // glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, (void*)0);

   // draw the cube map sky box

  // provide the vertex positions to the shaders
  GLint skyboxPositionAttribId = glGetAttribLocation(skyboxProgramId, "position");
  glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
  glEnableVertexAttribArray(skyboxPositionAttribId);
  glVertexAttribPointer(skyboxPositionAttribId, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  // texture sampler - a reference to the texture we've previously created
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

  glBindVertexArray(skyboxPositionAttribId);
  glDrawArrays(GL_TRIANGLES, 0, 36);

 // disable the attribute array
  glDisableVertexAttribArray(skyboxPositionAttribId);

	glutSwapBuffers();
}

static void reshape(int w, int h) {
   glViewport(0, 0, w, h);

   width = w;
   height = h;
}

// static void drag(int x, int y) {
// }
//
// static void mouse(int button, int state, int x, int y) {
// }

static void keyboard(unsigned char key, int x, int y) {
    std::cout << "Key pressed: " << key << std::endl;
    if (key == 'l') {
      animateLight = !animateLight;
    } else if (key == 'r') {
      rotateObject = !rotateObject;
    }
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
  program.loadShaders("shaders/flat_vertex.glsl", "shaders/flat_fragment.glsl");
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

static void createTexture(std::string filename) {
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
}
