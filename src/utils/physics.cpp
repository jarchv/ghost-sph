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

    float K  = 100000.0f;
    float MU = 0.0002f;
    float pressure_factor;
    glm::vec3 viscocity_factor;

    glm::vec3 pressure_force;
    glm::vec3 viscocity_force;
    for(int ip = 0; ip < num_particles; ip++)
    {
        float i_density = ParticleSystem[ip].density;
        pressure_force  = glm::vec3(0.0, 0.0, 0.0);
        viscocity_force  = glm::vec3(0.0, 0.0, 0.0);

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

                // sample implementation value: pi/roi2 + pj/roj2

                pressure_factor   = - MASS * MASS * ParticleSystem[ip].density;
                pressure_factor  *= (i_pressure / (i_density * i_density) + 
                                    j_pressure / (ParticleSystem[jp].density * ParticleSystem[jp].density) );
                pressure_force   += pressure_factor * KernelGrad(delta, smoothing_scale,h_6);


                viscocity_factor  = MU * MASS * (ParticleSystem[jp].velocity - ParticleSystem[ip].velocity) / ParticleSystem[jp].density;
                viscocity_force  += viscocity_factor * KernelLaplacian(delta, smoothing_scale,h_6);
            }
        }
        //std::cout << "i : " << ip <<" | before : " << ParticleSystem[ip].force.x << ", pressure : " << pressure_force.x << std::endl;
        //std::cout << "i : " << ip <<" | before : " << ParticleSystem[ip].force.y << ", pressure : " << pressure_force.y << std::endl;
        //std::cout << "i : " << ip <<" | before : " << ParticleSystem[ip].force.z << ", pressure : " << pressure_force.z << std::endl;
        ParticleSystem[ip].force += pressure_force + viscocity_force;
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
    float K     = 1.0f;    // CONSTANT OF SPRING

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
    float smoothing_scale = 0.3;
    float VOLUME          = (4.0 / 3.0) * PIV * RadiusMask * RadiusMask * RadiusMask;

    ContainerCollisions(ParticleSystem, FluidContainer, num_particles, RadiusMask, MASS);
    DensityEstimator(ParticleSystem, num_particles, smoothing_scale, MASS);
    ForceEstimator(ParticleSystem, num_particles, smoothing_scale, MASS);

    for (int ip = 0; ip < num_particles; ip++)
    {
        aceleration  = ParticleSystem[ip].force * (1 /  MASS);
        aceleration -= glm::normalize(ParticleSystem[ip].velocity) * 3.5f;
        //std::cout << glm::normalize(ParticleSystem[ip].velocity).x * 0.1f << std::endl;
        //std::cout << glm::normalize(ParticleSystem[ip].velocity).y * 0.1f << std::endl;
        //std::cout << glm::normalize(ParticleSystem[ip].velocity).z * 0.1f << std::endl;
        //break;
        aceleration *= 0.8;
        ParticleSystem[ip].position += ParticleSystem[ip].velocity * timeStep + aceleration * timeStep * timeStep * 0.5f;
        ParticleSystem[ip].velocity += timeStep * aceleration;
        //if (ParticleSystem[ip].density > 0)
        //{
        //    std::cout << "Density (" << ip << ") = " << ParticleSystem[ip].density << ", MASS  = " << ParticleSystem[ip].density * VOLUME <<std::endl;
        //}
    }

    tSim += timeStep;
}
