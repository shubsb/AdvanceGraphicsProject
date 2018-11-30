#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
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
#pragma once

class Boid {

private:
  float seperationFacotor = 0.4f;
  float cohesionFactor = 0.3f;
  float alignmentFactor = 0.3f;

public:
  int NUM_BOIDS = 10;
  glm::vec3 position;
  glm::vec3 velocity;

  Boid(glm::vec3 a_position): position(a_position){
    velocity = glm::normalize(a_position)*3.0f;
  };

  void CapVelocity(float maxSpeed);
  void ApplyCohesion(glm::vec3 cohesionVector);
  void ApplySeperation(glm::vec3 seperationVector);
  void ApplyAlignment(glm::vec3 alignmentVector);
};
