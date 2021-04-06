#include "Cloth.h"

#include "Window.h"

/*
* Constructor for a piece of fabric. 
* Make sure particlesL/W is > 1 and odd
* 
* clothLength: desired length for fabric in OpenGL units (we use as 1 m)
* clothWidth: desired width for fabric in OpenGL units
* particlesL: desired number of particles across length
* particlesW: desired number of particles across width
* topLeftPos: top left position of cloth
* clothMass: default mass of this whole cloth
* randomness: randomness factor for each particle's position
*/
Cloth::Cloth(GLfloat clothLength, GLfloat clothWidth, GLint particlesL, 
	GLint particlesW, glm::vec3 topLeftPos, GLfloat clothMass, 
	GLfloat randomness) : clothLength(clothLength), clothWidth(clothWidth),
	topLeftPos(topLeftPos) {
	// make sure inputs are useful

	
	/* initialize particles ======================================*/

	// calculate mass for each particle
	this->totalParticles = particlesL * particlesW;
	this->particleMass = clothMass / (this->totalParticles);
	this->particlesW = particlesW;

	// get spacing of particles
	GLfloat spacingL = clothLength / particlesL;
	GLfloat spacingW = clothWidth / particlesW;

	// index to keep track of each particle
	GLint currIndex = 0;

	// for each particle row
	for (unsigned int row = 0; row < particlesL; row++) {
		// move position downwards each row by spacing
		glm::vec3 downDirection = glm::vec3(0.0f, -1.0f, 0.0f);
		glm::vec3 rowPos = topLeftPos + (row * spacingL * downDirection);

		// for each particle column
		for (unsigned int column = 0; column < particlesW; column++) {
			// move position to right each row by spacing
			glm::vec3 rightDirection = glm::vec3(1.0f, 0.0f, 0.01f);
			glm::vec3 currPosition = rowPos + (column * spacingW * rightDirection);

			// TODO: randomize position from input factor
			
			// create new particle at position
			Particle* currParticle = new Particle(currIndex, currPosition, particleMass);
			currIndex++;
			
			// push it
			particles.push_back(currParticle);

			// fixate if it's the first row of particles
			if (row == 0) {
				currParticle->Fixate();
			}
		}
	}

	//TwAddVarRW(Window::bar, "Cloth Top", TW_TYPE_DIR3F, &topRowPos, "Cloth Top");

	// reset index counter
	currIndex = 0;

	/* initialize springdampers between them =====================*/

	// for every row
	for (unsigned int row = 0; row < particlesL; row ++) {

		// for every column
		for (unsigned int column = 0; column < particlesW; column ++) {
			// get particle at this index
			GLint entry = row * particlesW + column;
			Particle* currParticle = particles[entry];

			// get bottom/right/bottom-right/top-right particles if in range
			Particle* botP = (entry + particlesW) < totalParticles ? 
				particles[entry + particlesW] : nullptr;
			Particle* rightP = (entry + 1) < totalParticles ?
				particles[entry + 1] : nullptr;
			Particle* botRightP = (entry + particlesW + 1 < totalParticles) ?
				particles[entry + particlesW + 1] : nullptr;
			Particle* topRightP = (entry - particlesW + 1) >= 0 ?
				particles[entry - particlesW + 1] : nullptr;

			GLfloat springConst = 1.0001f;
			GLfloat dampingConst = 0.50001f;
			//GLfloat restLength = spacingL;

			// create spring-dampers for existing particles
			if (botP) {
				GLfloat dist = glm::distance(currParticle->getPosition(), botP->getPosition());
				SpringDamper* currSD = new SpringDamper(currIndex, springConst, dampingConst,
					dist, currParticle, botP);

				springDampers.push_back(currSD);

				currIndex++;
			}
			if (botRightP) {
				GLfloat dist = glm::distance(currParticle->getPosition(), botRightP->getPosition());
				SpringDamper* currSD = new SpringDamper(currIndex, springConst, dampingConst,
					dist, currParticle, botRightP);

				springDampers.push_back(currSD);

				currIndex++;
			}
			if (rightP) {
				GLfloat dist = glm::distance(currParticle->getPosition(), rightP->getPosition());
				SpringDamper* currSD = new SpringDamper(currIndex, springConst, dampingConst,
					dist, currParticle, rightP);

				springDampers.push_back(currSD);

				currIndex++;
			}
			if (topRightP) {
				GLfloat dist = glm::distance(currParticle->getPosition(), topRightP->getPosition());
				SpringDamper* currSD = new SpringDamper(currIndex, springConst, dampingConst,
					dist, currParticle, topRightP);

				springDampers.push_back(currSD);

				currIndex++;
			}
		}
	}

	// reset currIndex
	currIndex = 0;

	/* initialize more spring-dampers for bending force ===========*/

	/* initialize triangles from particles =======================*/

	this->airVelocity = glm::vec3(0.0f);
	TwAddVarRW(Window::bar, "Wind Speed", TW_TYPE_DIR3F, &airVelocity, "Wind Speed");

	// for every row except last
	for (unsigned int row = 0; row < particlesL - 1; row++) {
		//for every column except last
		for (unsigned int column = 0; column < particlesW - 1; column++) {
			// get particles at this index
			GLint entry = row * particlesW + column;
			Particle* currP = particles[entry];

			// get surrounding particles for two triangles connected to currP
			Particle* botP = particles[entry + particlesW];
			Particle* botRightP = particles[entry + particlesW + 1];
			Particle* rightP = particles[entry + 1];

			// CONSTANTS
			GLfloat fluid = 1.225f;
			GLfloat drag = 1.2f;
			glm::vec3* airV = &this->airVelocity; //glm::vec3(0.9f, 0.0f, 1.2f);

			// create first triangle and push it
			Triangle* botTriang = new Triangle(currIndex, fluid, drag,
				airV, currP, botP, botRightP);
			triangles.push_back(botTriang);

			//indices.push_back(currIndex);
			indices.push_back(botTriang->P1->index);
			indices.push_back(botTriang->P2->index);
			indices.push_back(botTriang->P3->index);

			currIndex++;

			// create second triangle and push it
			Triangle* rightTriang = new Triangle(currIndex, fluid, drag,
				airV, currP, botRightP, rightP);
			triangles.push_back(rightTriang);

			//indices.push_back(currIndex);
			indices.push_back(rightTriang->P1->index);
			indices.push_back(rightTriang->P2->index);
			indices.push_back(rightTriang->P3->index);

			currIndex++;
		}
	}

	// reset currIndex
	currIndex = 0;
	
	/* initialize OpenGL/glsm stuff ======================================*/

	for (Particle* p : particles) { p->resetNormal(); }
	for (Triangle* t : triangles) { t->computeNormal(); }

	for (Particle* p : particles) {
		p->normalizeNormal();

		positions.push_back(p->getPosition());
		normals.push_back(p->getNormal());

		//std::cout << p->getPosition().x << ", " << p->getPosition().y << ", " << p->getPosition().z << ", " << std::endl;
	}

	//std::cout << "Sweeped" << std::endl;

	// Model matrix.
	this->model = glm::mat4(1.0f);

	// The color of the cloth.
	this->color = glm::vec3(0.0f, 1.0f, 1.0f);

	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_positions);
	glGenBuffers(1, &VBO_normals);

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind to the first VBO - We will use it to store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO - We will use it to store the normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Generate EBO, bind the EBO to the bound VAO and send the data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Unbind the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

Cloth::~Cloth() {
	for(Particle * p : particles) {
		delete p;
	}

	for (SpringDamper* sd : springDampers) {
		delete sd;
	}

	for (Triangle* t : triangles) {
		delete t;
	}

	// Delete the VBOs and the VAO.
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &VBO_normals);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

void Cloth::Update() {
	//std::cout << "Updating" << std::endl;

	//model = glm::rotate(glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Apply oversampling (sample 10 times per frame to stabilize simulation)
	GLfloat timeStep = 1.0f / 40.0f;	// Adjust
	GLfloat oversampleFactor = 0100.0f;	// Adjust
	GLfloat newDeltaTime = timeStep / oversampleFactor;

	

	for (unsigned int i = 0; i < oversampleFactor; i++) {
		this->ComputeForce(newDeltaTime);
		// Integrate Motion 
		for (Particle* p : particles) {
			// Compute forces and apply them
			
			p->Integrate(newDeltaTime);
		}
	}

	for (Particle* p : particles) { p->resetNormal(); }
	for (Triangle* t : triangles) { t->computeNormal(); }

	unsigned int currIndex = 0;
	for (Particle* p : particles) { 
		p->normalizeNormal();

		positions[currIndex] = p->getPosition();
		normals[currIndex] = p->getNormal();

		//std::cout << p->getPosition().x << ", " << p->getPosition().y << ", " << p->getPosition().z << ", " << std::endl;

		currIndex++;
	}

	//std::cout << "Sweeped" << std::endl;
}

	

void Cloth::Draw(const glm::mat4& viewProjMtx, GLuint shader) {
	// actiavte the shader program 
	glUseProgram(shader);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// get the locations and send the uniforms to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

	// Bind the VAO
	glBindVertexArray(VAO);

	// Bind to the first VBO - We will use it to store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO - We will use it to store the normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// draw the points using triangles, indexed with the EBO
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, positions.size());

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

/*
* Takes care of computing all forces (gravity, spring-dampers, bending, 
* aerodynamic drag) to each particle.
* 
* deltaTime: the size of the time step to take when integrating motion
*/
void Cloth::ComputeForce(GLfloat deltaTime) {
	// Apply gravity to each particle
	for (Particle* p : particles) {
		// remember our units are 1 unit = 1 m. So 9.8 m for g
		glm::vec3 gravityForce = p->mass * glm::vec3(0.0f, -09.8f, 0.0f);
		p->ApplyForce(gravityForce);
	}

	// apply each spring-damper's force
	for (SpringDamper* sd : springDampers) {
		sd->ComputeForce();
	}

	// apply each aerodynamic force
	for (Triangle* t : triangles) {
		t->ComputeForce();
	}
}