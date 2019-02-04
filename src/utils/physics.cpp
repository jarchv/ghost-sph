#include "sphere.hpp"
#include <glm/glm.hpp>
#include "container.hpp"

#define PIV 3.14159

/*
* Navier-Stokes equations:
* =======================
* 
* dv/dt = (1 / density) * [- grad(p) + mu * grad**2(v) + f]
*
* dv/dt = (1 / density) * [ presure + viscocity + external force]
*
* external foce = gravity + others
* 
* Pressure : fuilds move higher to lower pressure, so to compute it we
*            will use the equation of state:
*
*       pV = nRT
*       p  = nRT * (1 / V)
*       p  = k * (density_f - density_0)
*
*       ** Pressure wants to equalize the density **
* 
* 
*  
*/


/*
* Schoenberg (1946) B-Spline:
* ===========================
* Gaussian shaped kernel
*/
float KernelFunction(float delta, float smoothing_scale)
{
    float sigma;        // Normalization factor
    float d      = 3.0; // Tree dimensions
    float q      = delta / smoothing_scale;

    if (q >= 0 && q < 1)
    {
        return 0.25 * (2.0 - q) * (2.0 - q) * (2.0 - q) - 
                      (1.0 - q) * (1.0 - q) * (1.0 - q);
    }
    else if (q >= 1 && q < 2)
    {
        return 0.25 * (2.0 - q) * (2.0 - q) * (2.0 - q); 
    }
    else
    {
        return 0.0;
    }
    
}
void DensityEstimator(Particle ParticleSystem[], int num_particles, float smoothing_scale, float MASS)
{
    glm::vec3 i_pos;
    glm::vec3 j_pos;
    
    float     delta;
    
    for(int ip = 0; ip < num_particles; ip++)
    {
        ParticleSystem[ip].density = 0.0;
        for(int jp = 0; jp < num_particles; jp++)
        {
            if (ip != jp)
            {
                i_pos = ParticleSystem[ip].position;
                j_pos = ParticleSystem[jp].position;

                delta = glm::distance(i_pos, j_pos);

                ParticleSystem[ip].density += MASS * KernelFunction(delta, smoothing_scale);
            }
        }
    }
}

void ContainerCollisions(Particle  ParticleSystem[],
                         Container FluidContainer[], 
                         int num_particles, 
                         float RadiusMask, 
                         float MASS)
{
    float K     = 5.0f;    // CONSTANT OF SPRING

    glm::vec3 gravity_force  = glm::vec3(0.0, -9.80665, 0.0) * MASS;

    for (int ip = 0; ip < num_particles; ip++)
    {
        ParticleSystem[ip].force = gravity_force;
        for (int ic = 0; ic < 5; ic ++)
        {
            glm::vec3 delta  = ParticleSystem[ip].position - FluidContainer[ic].center;
            float normalDist = glm::dot(delta, FluidContainer[ic].normal);
                       
            if (normalDist < RadiusMask)
            {
                ParticleSystem[ip].force += (RadiusMask - normalDist) * FluidContainer[ic].normal * K;
            }
        }
    }    
}

void SimulatePhysics(Particle ParticleSystem[], 
                     Container FluidContainer[], 
                     float&tSim, float& v0, 
                     int num_particles, 
                     float timeStep, 
                     float Radius)
{   
    glm::vec3 aceleration;

    float MASS            = 0.001f;   // MASS OF A DROP = 50e-6 kg (Set 20 drops)
    float Mask            = 0.02;
    float RadiusMask      = Radius + Mask;
    float smoothing_scale = 0.01;
    float VOLUME          = (4.0 / 3.0) * PIV * RadiusMask * RadiusMask * RadiusMask;

    ContainerCollisions(ParticleSystem, FluidContainer, num_particles, RadiusMask, MASS);
    DensityEstimator(ParticleSystem, num_particles, smoothing_scale, MASS);

    for (int ip = 0; ip < num_particles; ip++)
    {
        aceleration  = ParticleSystem[ip].force * (1 /  MASS);
        ParticleSystem[ip].position += ParticleSystem[ip].velocity * timeStep + aceleration * timeStep * timeStep * 0.5f;
        ParticleSystem[ip].velocity += timeStep * aceleration;
        if (ParticleSystem[ip].density > 0)
        {
            std::cout << "Density (" << ip << ") = " << ParticleSystem[ip].density << ", MASS  = " << ParticleSystem[ip].density * VOLUME <<std::endl;
        }
    }

    tSim += timeStep;
}
