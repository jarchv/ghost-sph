#include <glm/glm.hpp>
#include "sphere.hpp"

void setInitialPosition(Particle ParticleSystem[], int num_particles,float kf)
{
    float MASS = 0.01;
    glm::vec3 gravityForce = glm::vec3(0.0, -9.80665, 0.0) * MASS;
    for (int ip = 0; ip < num_particles; ip++)
    {
        ParticleSystem[ip].position.x = kf * ((ip%25)%5);
        ParticleSystem[ip].position.y = kf * ((ip/25  )) + 2;
        ParticleSystem[ip].position.z = kf * ((ip%25)/5  + 1);
        ParticleSystem[ip].velocity   = glm::vec3(-2.0,0.0,1.0);
        ParticleSystem[ip].force      = gravityForce;//glm::vec3(0.0,0.0,0.0);
    }
}