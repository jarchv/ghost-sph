#include <glm/glm.hpp>
#include "sphere.hpp"

void setInitialPosition(Particle ParticleSystem[], int num_particles, int lcube, float SphereDist)
{
    int sq_lcube = lcube * lcube;
    for (int ip = 0; ip < num_particles; ip++)
    {
        ParticleSystem[ip].position.x = SphereDist * ((ip%sq_lcube)%lcube) + 0.0;
        ParticleSystem[ip].position.y = SphereDist * ((ip/sq_lcube      )) + 1.0;
        ParticleSystem[ip].position.z = SphereDist * ((ip%sq_lcube)/lcube  + 1.0);
        ParticleSystem[ip].velocity   = glm::vec3(0.0,0.0,0.0);
        ParticleSystem[ip].force      = glm::vec3(0.0,0.0,0.0);
    }
}

void setCubeRandomly(Particle ParticleSystem[], int num_particles, float lcube, float x0, float y0, float z0)
{
    float epsilonX;
    float epsilonY;
    float epsilonZ;
    for (int ip = 0; ip < num_particles; ip++)
    {
        epsilonX = ((float)(rand()%1000) / 1000.0);
        epsilonY = ((float)(rand()%1000) / 1000.0);
        epsilonZ = ((float)(rand()%1000) / 1000.0);
        //std::cout << "epsilon = " << epsilon << std::endl;
        ParticleSystem[ip].position.x = epsilonX * lcube - lcube/2.0 + x0;//+ 0.0;
        ParticleSystem[ip].position.y = epsilonY * lcube - lcube/2.0 + y0;
        ParticleSystem[ip].position.z = epsilonZ * lcube - lcube/2.0 + z0;
        ParticleSystem[ip].velocity   = glm::vec3(0.0,0.0,0.0);
        ParticleSystem[ip].force      = glm::vec3(0.0,0.0,0.0);
    }
}