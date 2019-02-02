#include "sphere.hpp"
#include <glm/glm.hpp>

void SimulatePhysics(Particle ParticleSystem[], float&tSim, float& v0, int num_particles, float timeStep)
{   
    glm::vec3 gravity = glm::vec3(0.0, -9.80665, 0.0);

    for (int ip = 0; ip < num_particles; ip++)
    {
        ParticleSystem[ip].position += ParticleSystem[ip].velocity * timeStep + gravity * timeStep * timeStep * 0.5f;
        ParticleSystem[ip].velocity += timeStep * gravity;
    }

    tSim += timeStep;
}