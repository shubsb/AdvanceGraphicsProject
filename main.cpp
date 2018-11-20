#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include "ShaderProgram.h"

using namespace std;

int windowWidth = 1920;
int windowHeight = 1080;

GLuint programID;

void changeViewport(int w, int h) {
	glViewport(0,0,w,h);
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

	glutSwapBuffers();
}
int main(int argc, char** argv) {
	//initialize glut
	glutInit(&argc, argv);
  glutInitContextVersion(4,0);
  //glutInitContextFlag(GLUT_FORWARD_COMPATIBLE);
  glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(windowWidth, windowHeight);
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
