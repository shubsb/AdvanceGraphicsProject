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
#include "Boid.h"
#pragma once

class BoidManager {
public:
  std::vector <Boid> boids;

  BoidManager(int num_boids);

  glm::vec3 GetGroupAlignment(glm::vec3 centerPosition, float radius);
  glm::vec3 GetGroupPosition(glm::vec3 centerPosition, float radius);
  void UpdateBoids(float dt);
};
