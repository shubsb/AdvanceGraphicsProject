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

#include "ShaderProgram.h"
#include "ObjMesh.h"
#include "BezierCurveRoad.h"

#define SCALE_FACTOR 2.0f

int width, height;

GLuint programId;
GLenum positionBufferId;


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
glm::vec3 eyePosition(0, 10, 10);
bool animateLight = true;
bool rotateObject = true;


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
   glBindVertexArray(VAO[0]);
   glDrawArrays(GL_LINE_STRIP, 0, road.getNumCurves()*road.getNumVertecies());

   //platform
   glBindVertexArray(VAO[1]);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, platformEBO);
   glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, (void*)0);

	 glutSwapBuffers();
}

static void reshape(int w, int h) {
   glViewport(0, 0, w, h);

   width = w;
   height = h;
}

static void drag(int x, int y) {
}

static void mouse(int button, int state, int x, int y) {
}

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

  glutMainLoop();
  return 0;
}
