#include "Triangle.h"

Triangle::Triangle(GLint index, GLfloat fluidDensity, GLfloat dragCoefficient,
	glm::vec3* startingAirVelocity,
	Particle* particle1, Particle* particle2, Particle* particle3) : index(index),
	fluidDensity(fluidDensity), dragCoefficient(dragCoefficient), 
	P1(particle1), P2(particle2), P3(particle3){
	this->airVelocity = startingAirVelocity;
}

Triangle::~Triangle() {

}

/*
* Computes the aerodynamic drag force acting on this
* triangle made up of three particles.
*/
void Triangle::ComputeForce() {
	// find velocity relative to airflow
	glm::vec3 surfaceVelocity = (P1->getVelocity() + P2->getVelocity() + P3->getVelocity()) / 3.0f;
	glm::vec3 relVelocity = surfaceVelocity - *(this->airVelocity);

	// find the normal of this triangle
	glm::vec3 p1Top2 = P2->getPosition() - P1->getPosition();
	glm::vec3 p1Top3 = P3->getPosition() - P1->getPosition();
	glm::vec3 crossProduct = glm::cross(p1Top2, p1Top3);
	glm::vec3 triangNormal = glm::normalize(crossProduct);

	// find the cross-sectional area of triangle, as seen from airflow 
	// direction
	GLfloat triangArea = (0.5f) * glm::length(crossProduct);
	GLfloat crossArea = triangArea * (glm::dot(relVelocity, triangNormal));

	// find the final aerodynamic drag force
	GLfloat relVelLengthSquared = glm::pow(glm::length(relVelocity), 2.0f);
	glm::vec3 aeroForce = (-0.5f) * this->fluidDensity * relVelLengthSquared * 
		this->dragCoefficient * crossArea * triangNormal;

	// apply this force equally to all three particles
	glm::vec3 forceThird = (1.0f / 3.0f) * aeroForce;
	P1->ApplyForce(forceThird);
	P2->ApplyForce(forceThird);
	P3->ApplyForce(forceThird);
}

Particle* Triangle::getPar1() {
	return P1;
}

Particle* Triangle::getPar2() {
	return P2;
}

Particle* Triangle::getPar3() {
	return P3;
}

void Triangle::computeNormal() {
	glm::vec3 normal = getNormal();

	P1->addNormal(normal);
	P2->addNormal(normal);
	P3->addNormal(normal);
}

glm::vec3 Triangle::getNormal() {
	// find the normal of this triangle
	glm::vec3 p1Top2 = P2->getPosition() - P1->getPosition();
	glm::vec3 p1Top3 = P3->getPosition() - P1->getPosition();
	glm::vec3 crossProduct = glm::cross(p1Top2, p1Top3);
	glm::vec3 triangNormal = glm::normalize(crossProduct);

	return triangNormal;
}

glm::vec3 Triangle::getPos1() {
	return P1->getPosition();
}

glm::vec3 Triangle::getPos2() {
	return P2->getPosition();
}

glm::vec3 Triangle::getPos3() {
	return P3->getPosition();
}
