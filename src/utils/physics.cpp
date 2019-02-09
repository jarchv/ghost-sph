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
    if (r >= 0 && r <= h)
    {
        float factor = (45.0 / (PIV * h_6)) * (h - r) * (h - r);
        return  - factor * glm::normalize(delta);
    }
    else 
    {   
        return glm::vec3(0.0,0.0,0.0);
    }
    
}
void  ForceEstimator(Particle ParticleSystem[], int num_particles, float smoothing_scale, float MASS)
{
    float h_6 = smoothing_scale * smoothing_scale * smoothing_scale * 
                smoothing_scale * smoothing_scale * smoothing_scale;
    
    float i_pressure;
    float j_pressure;

    glm::vec3 i_pos;
    glm::vec3 j_pos;
    
    glm::vec3 delta;

    float K  = 500.0f;
    float MU = 0.000001f;
    float pressure_factor;
    glm::vec3 viscosity_factor;

    glm::vec3 pressure_force;
    glm::vec3 viscosity_force;
    for(int ip = 0; ip < num_particles; ip++)
    {
        float i_density = ParticleSystem[ip].density;
        pressure_force  = glm::vec3(0.0, 0.0, 0.0);
        viscosity_force  = glm::vec3(0.0, 0.0, 0.0);

        i_pos       = ParticleSystem[ip].position;
        //i_pressure  = (ParticleSystem[ip].density - ParticleSystem[ip].density0) * K;

        /*
        * Incompressibility:
        * =================
        * 
        *  Computing pressure from density using the Tait equation
        */
        i_pressure  = K * (pow(ParticleSystem[ip].density/ParticleSystem[ip].density0, 7) - 1);
        for(int jp = 0; jp < num_particles; jp++)
        {
            if (ip != jp)
            {
                j_pos       = ParticleSystem[jp].position;       
                //j_pressure  = (ParticleSystem[jp].density - ParticleSystem[jp].density0) * K;
                j_pressure  = K * (pow(ParticleSystem[jp].density/ParticleSystem[jp].density0, 7) - 1);
                delta = i_pos - j_pos;
                
                /*
                * Pressure:
                * =========
                * 
                *   Pressure_Force = -mi * sum(mj * (pj/rhoj**2 + pi/rhoi**2)) * Grad(W(delta))  
                * 
                */
                pressure_factor   = - MASS * MASS * ParticleSystem[ip].density;
                pressure_factor  *= (i_pressure / (i_density * i_density) + 
                                    j_pressure / (ParticleSystem[jp].density * ParticleSystem[jp].density) );
                pressure_force   += pressure_factor * KernelGrad(delta, smoothing_scale,h_6);

                /*
                * Viscosity
                * =========
                * 
                *  Viscosity_Force = - mu * sum((mj/rhoj)* (vj - vi) * Laplacian(W(delta)))
                * 
                */

                viscosity_factor  = MU * (MASS / ParticleSystem[jp].density) * (ParticleSystem[jp].velocity - ParticleSystem[ip].velocity);
                viscosity_force  += viscosity_factor * KernelLaplacian(delta, smoothing_scale,h_6);
            }
        }
        ParticleSystem[ip].force += pressure_force + viscosity_force;
    }   
}
float KernelFunction(float delta, float h, float h_9)
{
    // Poly6 Kernel
    

    if (delta >= 0 && delta <= h)
    {
        return (365.0 / (64.0 * PIV * h_9)) * (h * h - delta * delta) * 
                                              (h * h - delta * delta) * 
                                              (h * h - delta * delta);
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
    
    float     h_9 = smoothing_scale * smoothing_scale * smoothing_scale *
                    smoothing_scale * smoothing_scale * smoothing_scale * 
                    smoothing_scale * smoothing_scale * smoothing_scale;
    
    for(int ip = 0; ip < num_particles; ip++)
    {
        for(int jp = 0; jp < num_particles; jp++)
        {
            if (ip != jp)
            {
                i_pos = ParticleSystem[ip].position;
                j_pos = ParticleSystem[jp].position;

                delta = glm::distance(i_pos, j_pos);

                ParticleSystem[ip].density += MASS * KernelFunction(delta, smoothing_scale,h_9);
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
    float K     = 10.0f;    // CONSTANT OF SPRING
    float normalDist;
    float v_comp_normal;
    //glm::vec3 gravity_force  = glm::vec3(0.0, -9.80665, 0.0) * MASS;
    glm::vec3 gravity_force  = glm::vec3(0.0, 0, 0.0) * MASS;
    
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
                    ParticleSystem[ip].velocity -= FluidContainer[ic].normal * v_comp_normal * 1.0f;
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
                     float Radius)
{   
    glm::vec3 aceleration;

    float MASS            = 0.001f;          // MASS OF A DROP = 50e-6 kg (Set 20 drops)
    float Mask            = Radius * 0.5;    
    float RadiusMask      = Radius + Mask;
    float smoothing_scale = RadiusMask * 3;
    float VOLUME          = (4.0 / 3.0) * PIV * RadiusMask * RadiusMask * RadiusMask;

    ContainerCollisions(ParticleSystem, FluidContainer, num_particles, RadiusMask, MASS);
    DensityEstimator(ParticleSystem, num_particles, smoothing_scale, MASS);
    ForceEstimator(ParticleSystem, num_particles, smoothing_scale, MASS);

    for (int ip = 0; ip < num_particles; ip++)
    {
        aceleration  = ParticleSystem[ip].force * (1 /  MASS);
        ParticleSystem[ip].position += ParticleSystem[ip].velocity * timeStep + aceleration * timeStep * timeStep * 0.5f;
        ParticleSystem[ip].velocity += timeStep * aceleration;
    }

    tSim += timeStep;
}
