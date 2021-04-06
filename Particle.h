#pragma once

#include "core.h"

class Particle
{
private:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	

	glm::vec3 momentum;
	glm::vec3 force;
	
	// whether particle is fixed or not
	bool fixed;

	glm::vec3 normal;

	glm::vec3 planePos, planeNorm;

public:
	
	const GLfloat mass;

	const GLint index;	// keeps track of what particle this is

	Particle(GLint index, glm::vec3 position, GLfloat mass);
	~Particle();

	void ApplyForce(glm::vec3& f);
	void Integrate(GLfloat deltaTime);
	void Fixate();

	glm::vec3 getPosition() { return position; }
	glm::vec3 getVelocity() { return velocity; }
	glm::vec3 getNormal() { return normal; }

	void updateFixedPos(glm::vec3 distToMove);
	void addNormal(glm::vec3 norm) { normal += norm; }
	void resetNormal() { normal = glm::vec3(0.0f); }
	void normalizeNormal() { normal = glm::normalize(normal); }

	void collisionHandler();
	bool detectCollision();
};

