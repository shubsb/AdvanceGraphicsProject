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

    int numNormal =0;

    //total number of control points
    int cpTotal = numCurves*4;

    std::vector<glm::vec3> vertexPositionData;
    std::vector<glm::vec3> vertexPositionData2;
    std::vector<glm::vec3> vertexTangentData;
    std::vector<glm::vec3> vertexPositionData3;

    std::vector<glm::vec3> verticesVector;
    std::vector<glm::vec3> normalsVector;
    std::vector<glm::vec2> uvsVector;

    glm::vec3 controlPoints[16]={
      //curve 1
      // glm::vec3(-35.0f,0.0f,-35.0f),
      // glm::vec3(-35.0f,0.0f,-10.0f),
      // glm::vec3(-35.0f,0.0f,10.0f),
      // glm::vec3(-35.0f,0.0f,25.0f),

      //curve 2
      // glm::vec3(25.0f,0.0f,-35.0f),
      // glm::vec3(30.0f,0.0f,-35.0f),
      // glm::vec3(45.0f,0.0f,-20.0f),
      // glm::vec3(45.0f,0.0f,-15.0f)

      glm::vec3(15.0f,0.0f,-35.0f),
      glm::vec3(15.0f,0.0f,-35.0f),
      glm::vec3(15.0f,0.0f,-20.0f),
      glm::vec3(15.0f,0.0f, 15.0f),

      glm::vec3(15.0f,0.0f,15.0f),
      glm::vec3(15.0f,0.0f,15.0f),
      glm::vec3(15.0f,0.0f,15.0f),
      glm::vec3(15.0f,0.0f,15.0f),

      glm::vec3(15.0f,0.0f,15.0f),
      glm::vec3(15.0f,0.0f,15.0f),
      glm::vec3(15.0f,0.0f,15.0f),
      glm::vec3(15.0f,0.0f,15.0f),

      glm::vec3(15.0f,0.0f,15.0f),
      glm::vec3(15.0f,0.0f,20.0f),
      glm::vec3(10.0f,0.0f,25.0f),
      glm::vec3(5.0f,0.0f,30.0f)
    };

    //create road/path using cubic bezier curves for the car to follow
    void createRoad();
    void AddTriangle(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC);
    glm::vec3 getPoint(float t,int curve);
    glm::vec3 getFirstDerivativePoint(float t, int curve);

  public:
    BezierCurveRoad();
    std::vector<glm::vec3> getPath();
    std::vector<glm::vec3> getRoad();
    int getNumCurves();
    int getNumVertecies();
    int getNumNormals();


};
