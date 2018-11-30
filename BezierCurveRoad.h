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
    int numCurves = 6;

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

    glm::vec3 controlPoints[24]={

      glm::vec3(15.0f,0.0f,-25.0f),
      glm::vec3(15.0f,0.0f,-25.0f),
      glm::vec3(15.0f,0.0f,-20.0f),
      glm::vec3(15.0f,0.0f, 15.0f),

      glm::vec3(15.0f,0.0f,15.0f),
      glm::vec3(15.0f,0.0f,20.0f),
      glm::vec3(10.0f,0.0f,20.0f),
      glm::vec3(5.0f,0.0f,20.0f),

      glm::vec3(5.0f,0.0f,20.0f),
      glm::vec3(0.0f,0.0f,20.0f),
      glm::vec3(-5.0f,0.0f,20.0f),
      glm::vec3(-5.0f,0.0f,10.0f),

      glm::vec3(-5.0f,0.0f,10.0f),
      glm::vec3(-5.0f,0.0f,5.0f),
      glm::vec3(-5.0f,0.0f,1.0f),
      glm::vec3(-5.0f,0.0f,-5.0f),

      glm::vec3(-5.0f,0.0f,-5.0f),
      glm::vec3(-5.0f,0.0f,-10.0f),
      glm::vec3(-5.0f,0.0f,-15.0f),
      glm::vec3(-5.0f,0.0f,-20.0f),

      glm::vec3(-5.0f,0.0f,-20.0f),
      glm::vec3(-5.0f,0.0f,-25.0f),
      glm::vec3(5.0f,0.0f,-25.0f),
      glm::vec3(15.0f,0.0f,-25.0f)
    };

    //create road/path using cubic bezier curves for the car to follow
    void createRoad();

    //add triangles to draw road along path
    void AddTriangle(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC);
    //get points on bezier curve
    glm::vec3 getPoint(float t,int curve);

    //get tangent to curve used to calculate normals to form triangle
    glm::vec3 getFirstDerivativePoint(float t, int curve);

  public:
    BezierCurveRoad();
    std::vector<glm::vec3> getPath();
    std::vector<glm::vec3> getRoad();
    int getNumCurves();
    int getNumVertecies();
    int getNumNormals();


};
