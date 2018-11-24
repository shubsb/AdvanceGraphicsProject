#include <GL\glew.h>
#include <GL\freeglut.h>
#include <GL\glut.h>

#include <iostream>
#include "ShaderProgram.h"
#include "Object.h"

using namespace std;

GLuint programID;

static void createGeometry(void) {
	Object mesh;
	mesh.load("bugatti.obj", true, true);
	std::cout << "Finished loadingb car .obj file." << std::endl;
}
void changeViewport(int w, int h) {
	glViewport(0,0,w,h);
}

static void render(void) {
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);
	glutSwapBuffers();
}


int main(int argc, char** argv) {
	//initialize glut
	glutInit(&argc, argv);
  glutInitContextVersion(4,0);
  //glutInitContextFlag(GLUT_FORWARD_COMPATIBLE);
  glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Graphics Project");

	glutDisplayFunc(&render);
	glutReshapeFunc(&changeViewport);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr,"GLEW error");
		return 1;
	}
	std::cout << "Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Using OpenGL " << glGetString(GL_VERSION) << std::endl;

	ShaderProgram program;
	program.loadShaders("shaders/flat_vertex.glsl", "shaders/flat_fragment.glsl");
	programID = program.getProgramId();
	glutMainLoop();

}
