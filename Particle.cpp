#include "Particle.h"


/*
* Constructor.
* position: given position to start at
* mass: given mass constant
*/
Particle::Particle(GLint index, glm::vec3 position, GLfloat mass) : 
	index(index), position(position), mass(mass) {
	// Initialize variables
	this->velocity = glm::vec3(0.0f);
	this->acceleration = glm::vec3(0.0f);

	this->momentum = glm::vec3(0.0f);
	this->force = glm::vec3(0.0f);

	this->normal = glm::vec3(0.0f);

	this->fixed = false;

	this->planePos = glm::vec3(0.0f, -4.5f, 0.0f);
	this->planeNorm = glm::vec3(0.0f, 1.0f, 0.0f);
}

Particle::~Particle() {

}

void Particle::ApplyForce(glm::vec3& f) {
	if(!fixed) this->force += f;
}

/*
* Method that computes Forward Euler Integration on this particle
* 
* deltaTime: the size of the time step to take forward in time
*/
void Particle::Integrate(GLfloat deltaTime) {
	// TODO: make sure this is right logic
	
	// if this particle is fixed, don't do anything to it
	if (fixed) return;

	glm::vec3 oldAcc = acceleration;
	glm::vec3 oldVel = velocity;
	glm::vec3 oldPos = position;

	// compute acceleration from all forces added up already
	this->acceleration = (1.0f / this->mass) * this->force;
	// compute velocity at (i+1) from acceleration at i times a time step
	this->velocity += this->acceleration * deltaTime;
	// compute position from velocity at (i+1) times a time step
	this->position += this->velocity * deltaTime;

	this->collisionHandler();

	//reset the forces of this particle
	this->force = glm::vec3(0.0f);
}

void Particle::collisionHandler() {
	if (!detectCollision()) return;

	GLfloat restitution = 0.5f;
	glm::vec3 impulseJ = (-1.0f) * (1 + restitution) * (this->mass * this->velocity * this->planeNorm);

	glm::vec3 normV = glm::dot(this->velocity, planeNorm) * planeNorm;
	glm::vec3 tanV = this->velocity - normV;

	GLfloat dynamicFriction = 0.75;
	impulseJ += (-tanV) * (dynamicFriction * glm::abs(impulseJ));
	this->velocity = (1.0f / mass) * impulseJ;
}

bool Particle::detectCollision() {
	GLfloat distToPlane = glm::dot((position - planePos), planeNorm);

	if (distToPlane < 0) return true;

	return false;
}

/*
* Call this method to make this particle fixed
*/
void Particle::Fixate() {
	this->fixed = true;
}

/*
* Updates position of those fixed particles to move around cloth
* 
* distToMove: distance to move this particle
*/
void Particle::updateFixedPos(glm::vec3 distToMove) {
	this->position += distToMove;
}