#pragma once

#include "SpringDamper.h"
#include "Triangle.h"

// forward declare
class Window;

class Cloth
{
private:
	// VAO/VBOs/EBO
	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO;

	glm::mat4 model;
	glm::vec3 color;

	// information vectors for VBOs and EBO
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;

	// lists of actual particles' data
	
	std::vector<SpringDamper*> springDampers;
	std::vector<SpringDamper*> bendingForces;
	std::vector<Triangle*> triangles;

	// cloth logistic general data
	GLfloat clothLength, clothWidth;
	glm::vec3 topLeftPos;
	GLfloat particleMass;
	GLfloat totalParticles;
	

public:
	std::vector<Particle*> particles;
	glm::vec3 topRowPos;
	GLfloat particlesW;

	glm::vec3 airVelocity;

	// constructor for a piece of fabric
	Cloth(GLfloat clothLength, GLfloat clothWidth, GLint particlesL,
		GLint particlesW, glm::vec3 topLeftPos, GLfloat clothMass,
		GLfloat randomness);
	// TODO: create more constructors if we want to do rope/etc
	~Cloth();

	void Update();
	void Draw(const glm::mat4& viewProjMtx, GLuint shader);

	void ComputeForce(GLfloat deltaTime);
};

