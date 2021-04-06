#include "SpringDamper.h"

/*
* Constructor.
*/
SpringDamper::SpringDamper(GLint index, GLfloat springConstant, GLfloat dampingConstant, 
	GLfloat restLength, Particle* particle1, Particle* particle2) : index(index),
	springConstant(springConstant), dampingConstant(dampingConstant), 
	restLength(restLength), P1(particle1), P2(particle2) {
}

SpringDamper::~SpringDamper() {

}

/*
* Computes forces from spring-damper connecting particles P1 and P2.
* Applies forces directly to each particle once done.
*/
void SpringDamper::ComputeForce() {
	// compute current length l & unit vector e
	glm::vec3 e = P1->getPosition() - P2->getPosition();
	GLfloat currentLength = glm::length(e);
	e = e/currentLength;

	// compute closing velocity
	GLfloat closeV = glm::dot((P1->getVelocity() - P2->getVelocity()), e);

	// compute final forces
	GLfloat springForce = (-springConstant) * (currentLength - restLength);
	GLfloat dampingForce = ((-dampingConstant) * closeV);
	GLfloat forceConst =  springForce + dampingForce;
	glm::vec3 force1 = forceConst * e;
	glm::vec3 force2 = -force1;

	// apply final forces to each particle
	P1->ApplyForce(force1);
	P2->ApplyForce(force2);
}