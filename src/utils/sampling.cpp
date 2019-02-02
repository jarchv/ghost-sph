#include <glm/glm.hpp>
#include "sphere.hpp"

void setInitialPosition(Particle ParticleSystem[], int num_particles,float kf)
{
    for (int ip = 0; ip < num_particles; ip++)
    {
        ParticleSystem[ip].position.x = kf * ((ip%25)%5);
        ParticleSystem[ip].position.y = kf * ((ip/25  ));
        ParticleSystem[ip].position.z = kf * ((ip%25)/5);
        ParticleSystem[ip].velocity   = glm::vec3(0.0,2.0,0.0);
    }
}


