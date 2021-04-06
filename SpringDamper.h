#pragma once

#include "Particle.h"

class SpringDamper
{
private:


public:
	const GLfloat springConstant;
	const GLfloat dampingConstant;
	GLfloat restLength;

	// pointers to each particle
	Particle *P1, *P2;

	GLint index;	// keeps track of which SpringDamper this is

	SpringDamper(GLint index, GLfloat springConstant, GLfloat dampingConstant,
		GLfloat restLength, Particle* particle1, Particle* particle2);
	~SpringDamper();

	void ComputeForce();

};

