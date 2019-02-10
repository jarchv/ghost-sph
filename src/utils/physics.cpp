#include "sphere.hpp"
#include <glm/glm.hpp>
#include "container.hpp"
#include <vector>

#define PIV 3.14159

/*
* Navier-Stokes equations:
* =======================
* 
* dv/dt = (1 / density) * [- grad(p) + mu * grad**2(v) + f]
*
* dv/dt = (1 / density) * [ presure + viscosity + external force]
*
*/

/*
* Schoenberg (1946) B-Spline:
* ===========================
* Gaussian shaped kernel
*/

float KernelLaplacian(glm::vec3 delta, float h, float h_6)
{
    float r = glm::length(delta);
    return  (45.0 / (PIV * h_6 )) * (h - r);
}

glm::vec3 KernelGrad(glm::vec3 delta, float h, float h_6)
{
    // Gradient spinky kernel

    float r = glm::length(delta);
    float factor = (45.0 / (PIV * h_6)) * (h - r) * (h - r);
    return  - factor * glm::normalize(delta);   
}

float KernelFunction(float delta, float h, float h_9)
{
    // Poly6 Kernel
    
    return (365.0 / (64.0 * PIV * h_9)) * (h * h - delta * delta) * 
                                          (h * h - delta * delta) * 
                                          (h * h - delta * delta);
}

void AccumulateForces(  Particle ParticleSystem[], 
                        int ip, 
                        int np, 
                        glm::vec3 i_pos, 
                        glm::vec3 n_pos, 
                        float smoothing_scale, 
                        float h_6, 
                        float K, 
                        float MASS)
{
    float pressure_factor;
    float MU = 0.000001f;

    glm::vec3 viscosity_factor;
    
    float i_density   = ParticleSystem[ip].density;
    glm::vec3 delta   = i_pos - n_pos;
    pressure_factor   = - MASS * MASS * i_density;
    pressure_factor  *= (ParticleSystem[ip].pressure / (i_density * i_density) + 
                         ParticleSystem[np].pressure / (ParticleSystem[np].density * ParticleSystem[np].density) );

    ParticleSystem[ip].force += pressure_factor * KernelGrad(delta, smoothing_scale,h_6);

    viscosity_factor  = MU * (MASS / ParticleSystem[np].density) * (ParticleSystem[np].velocity - ParticleSystem[ip].velocity);

    ParticleSystem[ip].force += viscosity_factor * KernelLaplacian(delta, smoothing_scale,h_6);
}

void DensityEstimator(Particle ParticleSystem[], int ip, glm::vec3 i_pos, glm::vec3 n_pos, float smoothing_scale, float h_9, float MASS)
{
    float delta = glm::distance(i_pos, n_pos);
    ParticleSystem[ip].density += MASS * KernelFunction(delta, smoothing_scale,h_9);
}

void ContainerCollisions(Particle  ParticleSystem[],
                         Container FluidContainer[], 
                         int num_particles, 
                         float RadiusMask, 
                         float MASS)
{
    float K     = 10.0f;    // CONSTANT OF SPRING
    float normalDist;
    float v_comp_normal;
    glm::vec3 gravity_force  = glm::vec3(0.0, -9.80665, 0.0) * MASS;
    //glm::vec3 gravity_force  = glm::vec3(0.0, 0, 0.0) * MASS;
    
    for (int ip = 0; ip < num_particles; ip++)
    {
        ParticleSystem[ip].force = gravity_force;
        for (int ic = 0; ic < 5; ic ++)
        {
            glm::vec3 delta  = ParticleSystem[ip].position - FluidContainer[ic].center;
            normalDist = abs(glm::dot(delta, FluidContainer[ic].normal));
                       
            if (normalDist < RadiusMask)
            {
                // Ghost Particles in Solids
                ParticleSystem[ip].velocity  = ParticleSystem[ip].velocity * 0.5f;
                
                v_comp_normal                = glm::dot(ParticleSystem[ip].velocity, FluidContainer[ic].normal);

                if (v_comp_normal < 0)
                {
                    ParticleSystem[ip].velocity -= FluidContainer[ic].normal * v_comp_normal * 1.5f;
                    //std::cout << ParticleSystem[ip].velocity.y << std::endl;
                }   
            }
        }
    }    
}

void SimulatePhysics(Particle ParticleSystem[], 
                     Container FluidContainer[], 
                     float&tSim, float& v0, 
                     int num_particles, 
                     float timeStep, 
                     float Radius,
                     float smoothing_scale,
                     float h_9,
                     float h_6 )
{   
    glm::vec3 aceleration;

    float MASS            = 0.001f;          // MASS OF A DROP = 50e-6 kg (Set 20 drops)
    float Mask            = Radius * 0.5;    
    float RadiusMask      = Radius + Mask;

    ContainerCollisions(ParticleSystem, FluidContainer, num_particles, RadiusMask, MASS);

    glm::vec3 i_pos;
    glm::vec3 j_pos;
    glm::vec3 n_pos;

    float delta;
    float i_pressure;
   
    int   n_idx;

    std::vector<int> NEIGHBOURS;
    

    float K  = 500.0f;
    for (int ip = 0; ip < num_particles; ip++)
    {
        i_pos = ParticleSystem[ip].position;
        NEIGHBOURS.clear();
        for (int jp = 0; jp < num_particles; jp++)
        {
            if (ip != jp)
            {
                j_pos = ParticleSystem[jp].position;
                delta = glm::distance(i_pos, j_pos);

                if (abs(delta) < smoothing_scale)
                {
                    NEIGHBOURS.push_back(jp);
                }
            }

        }
        for (int i_neighbour = 0; i_neighbour < NEIGHBOURS.size(); i_neighbour++)
        {
            n_idx = NEIGHBOURS[i_neighbour];
            n_pos = ParticleSystem[n_idx].position;

            DensityEstimator(ParticleSystem, ip, i_pos, n_pos, smoothing_scale, h_9, MASS);

            
        }
        i_pressure  = K * (pow(ParticleSystem[ip].density/ParticleSystem[ip].density0, 7) - 1);
        ParticleSystem[ip].pressure = i_pressure;

        for (int i_neighbour = 0; i_neighbour < NEIGHBOURS.size(); i_neighbour++)
        {
            n_idx = NEIGHBOURS[i_neighbour];
            n_pos = ParticleSystem[n_idx].position;

            AccumulateForces(ParticleSystem, ip, n_idx, i_pos, n_pos, smoothing_scale, h_6, K, MASS);            
        }
    }    

    for (int ip = 0; ip < num_particles; ip++)
    {
        aceleration  = ParticleSystem[ip].force * (1 /  MASS);
        ParticleSystem[ip].position += ParticleSystem[ip].velocity * timeStep + aceleration * timeStep * timeStep * 0.5f;
        ParticleSystem[ip].velocity += timeStep * aceleration;
    }

    tSim += timeStep;
}
