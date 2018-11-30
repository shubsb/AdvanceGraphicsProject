#include "BoidManager.h"

	BoidManager::BoidManager(int a_num_boids){
    for (int i = 0; i < a_num_boids; i++){
			glm::vec3 startingPosition = glm::vec3(cos((float)i), sin((float)i), 0.0f);
			Boid boid =  Boid(startingPosition);
			boids.push_back(boid);
		}
	}

  glm::vec3 BoidManager::GetGroupAlignment(glm::vec3 centerPosition, float radius){
		glm::vec3 averageAlignment = glm::vec3(0.0f);
		float count = 0.0f;

		for(int i = 0; i < boids.size(); i++){
			if (glm::length(boids[i].position - centerPosition) < radius){
				glm::vec3 direction = glm::normalize(boids[i].velocity);
				averageAlignment += direction;
				count += 1.0f;
			}
		}
		if (count == 0.0f) return glm::vec3(0.0f);
		return averageAlignment/count;
  }

  glm::vec3 BoidManager::GetGroupPosition(glm::vec3 centerPosition, float radius){
		glm::vec3 averagePosition = glm::vec3(0.0f);
		float count = 0.0f;

		for(int i = 0; i < boids.size(); i++){
			if (glm::length(boids[i].position - centerPosition) < radius){
				averagePosition += boids[i].position;
				count += 1.0f;
			}
		}
		if (count == 0.0f) return glm::vec3(0.0f);
		return averagePosition/count;
	}

	void BoidManager::UpdateBoids(float dt){
		for(int i =0; i < boids.size(); i++){
			boids[i].CapVelocity(3.0f);

			glm::vec3 alignmentVector = GetGroupAlignment(boids[i].position, 1.0f);
			glm::vec3 seperationVector = boids[i].position - GetGroupPosition(boids[i].position, 1.0f);
			glm::vec3 cohesionVector = GetGroupPosition(boids[i].position, 1.0f) - boids[i].position;

			boids[i].ApplySeperation(seperationVector);
			boids[i].ApplyCohesion(cohesionVector);
			boids[i].ApplyAlignment(alignmentVector);

			boids[i].position += boids[i].velocity*dt;

      if (boids[i].position.x > 10.0f){
				boids[i].position.x = -10.0f;
			}
			if (boids[i].position.x < -10.0f){
				boids[i].position.x = 10.0f;
			}
			if (boids[i].position.y > 8.0f){
				boids[i].position.y = 6.0f;
			}
			if (boids[i].position.y < 6.0f){
				boids[i].position.y = 8.0f;
			}
		}
	}
