#include "BezierCurveRoad.h"


//the control points for the bezier curves
BezierCurveRoad::BezierCurveRoad(){
  std::cout << "Creating Path" << std::endl;
  createRoad();
}

void BezierCurveRoad::AddTriangle(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC)
{
	verticesVector.push_back(pointA);
	verticesVector.push_back(pointB);
	verticesVector.push_back(pointC);

	normalsVector.push_back(glm::vec3(0,1,0));
	normalsVector.push_back(glm::vec3(0, 1, 0));
	normalsVector.push_back(glm::vec3(0, 1, 0));

	uvsVector.push_back(glm::vec2(pointA.x, pointA.z));
	uvsVector.push_back(glm::vec2(pointB.x, pointB.z));
	uvsVector.push_back(glm::vec2(pointC.x, pointC.z));
}

glm::vec3 BezierCurveRoad::getPoint(float t,int curve) {
	int cp0 = curve * 4;
	int cp1 = cp0 + 1;
	int cp2 = cp0 + 2;
	int cp3 = cp0 + 3;

	float x = pow((1 - t), 3)*controlPoints[cp0].x + pow((1 - t), 2) * 3 * t*controlPoints[cp1].x + (1 - t) * 3 * pow(t, 2)*controlPoints[cp2].x + pow(t, 3)*controlPoints[cp3].x;
	float y = pow((1 - t), 3)*controlPoints[cp0].y + pow((1 - t), 2) * 3 * t*controlPoints[cp1].y + (1 - t) * 3 * pow(t, 2)*controlPoints[cp2].y + pow(t, 3)*controlPoints[cp3].y;
	float z = pow((1 - t), 3)*controlPoints[cp0].z + pow((1 - t), 2) * 3 * t*controlPoints[cp1].z + (1 - t) * 3 * pow(t, 2)*controlPoints[cp2].z + pow(t, 3)*controlPoints[cp3].z;

	glm::vec3 point(x,y,z);

	return point;
}

glm::vec3 BezierCurveRoad::getFirstDerivativePoint(float t, int curve) {
	int cp0 = curve * 4;
	int cp1 = cp0 + 1;
	int cp2 = cp0 + 2;
	int cp3 = cp0 + 3;

	float x = -3 * pow((1 - t), 2)*controlPoints[cp0].x + (3 * pow((1 - t), 2) * controlPoints[cp1].x - 6 * t * (1 - t))*controlPoints[cp1].x - 3 * pow(t, 2) *controlPoints[cp2].x + 6 * t*(1 - t) *controlPoints[cp2].x + (3 * pow(t, 2))*controlPoints[cp3].x;
	float y = -3 * pow((1 - t), 2)*controlPoints[cp0].y + (3 * pow((1 - t), 2) * controlPoints[cp1].y - 6 * t * (1 - t))*controlPoints[cp1].y - 3 * pow(t, 2) *controlPoints[cp2].y+ 6 * t*(1 - t) *controlPoints[cp2].y + (3 * pow(t, 2))*controlPoints[cp3].y;
	float z = -3 * pow((1 - t), 2)*controlPoints[cp0].z + (3 * pow((1 - t), 2) * controlPoints[cp1].z - 6 * t * (1 - t))*controlPoints[cp1].z - 3 * pow(t, 2) *controlPoints[cp2].z + 6 * t*(1 - t) *controlPoints[cp2].z + (3 * pow(t, 2))*controlPoints[cp3].z;

	glm::vec3 point(x, y, z);

	return point;
}

void BezierCurveRoad::createRoad(){
  glm::vec3 pointOnCurve;
	glm::vec3 tangentOnCurve;
	glm::vec3 normalOnCurve;

	glm::vec3 nextPointOnCurve;
	glm::vec3 nextTangentOnCurve;
	glm::vec3 nextNormalOnCurve;

	int pickPoint;
	//make bezier curve
	for (int j = 0; j < numCurves; j++) {
		pickPoint = 0;
		for (float t = 0.0f; t <= 1.0f; t += 1.0f / (float)numVertices) {
			pointOnCurve = getPoint(t,j);

			if (pickPoint == 0) {
				vertexPositionData2.push_back(pointOnCurve);
				tangentOnCurve = glm::normalize(getFirstDerivativePoint(t, j));
				vertexTangentData.push_back(tangentOnCurve);
			}
			pickPoint++;
			if (pickPoint == 100) {
				pickPoint = 0;
			}
			vertexPositionData.push_back(pointOnCurve);
		}
	}

	int a =0;
	numNormal = vertexPositionData2.size()*2;

	for (int j = 0; j < vertexPositionData2.size()-1; j++) {
		//std::cout << "j: " << j<<std::endl;
		//pick a point on the curve
		pointOnCurve = vertexPositionData2[j];
		//tangent for that point
		tangentOnCurve = vertexTangentData[j];
		//find normal for that point
		normalOnCurve = glm::cross(glm::vec3(0, 1, 0), tangentOnCurve);

		//pick the next point on the curve
		nextPointOnCurve = vertexPositionData2[j + 1];  
		//tangent for that point
		nextTangentOnCurve = vertexTangentData[j + 1];
		//find normal for that point
		nextNormalOnCurve = glm::cross(glm::vec3(0, 1, 0), nextTangentOnCurve);
		a++;
		AddTriangle(pointOnCurve + normalOnCurve, pointOnCurve - normalOnCurve, nextPointOnCurve + nextNormalOnCurve);
		a++;
		AddTriangle(pointOnCurve - normalOnCurve, nextPointOnCurve + nextNormalOnCurve, nextPointOnCurve - nextNormalOnCurve);
	}

	verticesVector.push_back(nextPointOnCurve+nextNormalOnCurve);

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

int BezierCurveRoad::getNumNormals(){
  return numNormal;
}

std::vector<glm::vec3> BezierCurveRoad::getRoad(){
  return verticesVector;
}
