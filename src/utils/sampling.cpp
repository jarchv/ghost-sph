#include <glm/glm.hpp>
#include "sphere.hpp"

void setInitialPosition(Particle ParticleSystem[], int num_particles, int lcube, float SphereDist)
{
    int sq_lcube = lcube * lcube;
    for (int ip = 0; ip < num_particles; ip++)
    {
        ParticleSystem[ip].position.x = SphereDist * ((ip%sq_lcube)%lcube) + 0.0;
        ParticleSystem[ip].position.y = SphereDist * ((ip/sq_lcube      )) + 2.0;
        ParticleSystem[ip].position.z = SphereDist * ((ip%sq_lcube)/lcube  + 2.0);
        ParticleSystem[ip].velocity   = glm::vec3(0.0,0.01,0.0);
        ParticleSystem[ip].force      = glm::vec3(0.0,0.0,0.0);
    }
}