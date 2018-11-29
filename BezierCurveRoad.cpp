#include "BezierCurveRoad.h"


BezierCurveRoad::BezierCurveRoad(){
  std::cout << "Creating Path" << std::endl;
  createRoad();
}
void BezierCurveRoad::createRoad(){

  //the control points for the bezier curves
  glm::vec3 controlPoints[]={

    //curve 1
    glm::vec3(-35.0f,0.0f,-35.0f),
    glm::vec3(-10.0f,0.0f,-35.0f),
    glm::vec3(10.0f,0.0f,-35.0f),
    glm::vec3(25.0f,0.0f,-35.0f),

    //curve 2
    glm::vec3(25.0f,0.0f,-35.0f),
    glm::vec3(30.0f,0.0f,-35.0f),
    glm::vec3(45.0f,0.0f,-20.0f),
    glm::vec3(45.0f,0.0f,-15.0f)
  };

  float x, y, z;

  //used to keep track of the indexes of the control points that make up each bezier curve
  int p0 = 0;
  int p1 = 1;
  int p2 = 2;
  int p3 = 3;

  for(int i = 0; i< cpTotal; i+=4){
    p0 = i;
    p1 = i + 1;
    p2 = i + 2;
    p3 = i + 3;
    for (float t = 0.0f; t <= 1.0f; t += 1.0f / (float)numVertices){
      x = pow((1 - t), 3)*controlPoints[p0].x + pow((1 - t), 2) * 3 * t*controlPoints[p1].x + (1 - t) * 3 * pow(t, 2)*controlPoints[p2].x + pow(t, 3)*controlPoints[p3].x;
			y = pow((1 - t), 3)*controlPoints[p0].y + pow((1 - t), 2) * 3 * t*controlPoints[p1].y + (1 - t) * 3 * pow(t, 2)*controlPoints[p2].y + pow(t, 3)*controlPoints[p3].y;
			z = pow((1 - t), 3)*controlPoints[p0].z + pow((1 - t), 2) * 3 * t*controlPoints[p1].z + (1 - t) * 3 * pow(t, 2)*controlPoints[p2].z + pow(t, 3)*controlPoints[p3].z;

      glm::vec3 xyz(x,y,z);
      vertexPositionData.push_back(xyz);
    }
  }
}

std::vector<glm::vec3> BezierCurveRoad::getPath(){
  return vertexPositionData;
}

int BezierCurveRoad::getNumCurves(){
  return numCurves;
}

int BezierCurveRoad::getNumVertecies(){
  return numVertices;
}
