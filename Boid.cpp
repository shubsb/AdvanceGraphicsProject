#include "Boid.h"

void Boid::CapVelocity(float maxSpeed){
	if(glm::length(velocity) > maxSpeed){
		velocity = glm::normalize(velocity)*maxSpeed;
	}
}

  void Boid::ApplyCohesion(glm::vec3 cohesionVector) { velocity += cohesionVector * cohesionFactor; }
  void Boid::ApplySeperation(glm::vec3 seperationVector) { velocity += seperationFacotor*seperationVector; }
  void Boid::ApplyAlignment(glm::vec3 alignmentVector) { velocity += alignmentVector*alignmentFactor; }
