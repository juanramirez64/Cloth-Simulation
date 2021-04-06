#pragma once

#include "Particle.h"

class Triangle
{
private:
	const GLfloat fluidDensity;
	const GLfloat dragCoefficient;
	glm::vec3* airVelocity;

public:
	// pointers to each particle
	Particle *P1, *P2, *P3;

	GLint index;	// keeps track of which Triangle this is

	Triangle(GLint index, GLfloat fluidDensity, GLfloat dragCoefficient,
		glm::vec3* startingAirVelocity,
		Particle* particle1, Particle* particle2, Particle* particle3);
	~Triangle();

	void ComputeForce();

	Particle* getPar1();
	Particle* getPar2();
	Particle* getPar3();

	void computeNormal();

	glm::vec3 getNormal();
	glm::vec3 getPos1();
	glm::vec3 getPos2();
	glm::vec3 getPos3();
};

