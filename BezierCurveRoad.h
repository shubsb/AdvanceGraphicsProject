#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderProgram.h"
#pragma once

class BezierCurveRoad{
  private:

    //how many verteces make up each bezier curve
    int numVertices = 1000;

    //how many cuves combined make up the path
    int numCurves = 4;

    //total number of control points
    int cpTotal = numCurves*4;



    //the xyz coordiantes the make up the road/path
    std::vector<glm::vec3> vertexPositionData;


    //create road/path using cubic bezier curves for the car to follow
    void createRoad();
  public:
    BezierCurveRoad();
    std::vector<glm::vec3> getPath();
    int getNumCurves();
    int getNumVertecies();

};
