#include "sphere.hpp"
#include <glm/glm.hpp>
#include "container.hpp"
#include <vector>
#include <omp.h>

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

glm::vec3 kernelPoly6Grad(glm::vec3 delta, float h,float h_9)
{
    float factor = - 945.0 / (32.0 * PIV * h_9);
    float r      = glm::length(delta);
    return factor * (h * h - r * r) * glm::normalize(delta);
}

float kernelPoly6Laplacian(glm::vec3 delta, float h,float h_9)
{
    float factor = 315.0 / (64.0 * PIV * h_9);
    float r      = glm::length(delta);
    return factor * (h * h - r * r) * (7 * r * r - 3 * h * h);
}
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
                        float h_9,
                        float h_6, 
                        float K, 
                        float MU,
                        float SIGMA,
                        float MASS)
{
    float pressure_factor;

    glm::vec3 viscosity_factor;
    
    glm::vec3 colorFieldNormal;
    float     colorFieldLaplacian;

    float i_density   = ParticleSystem[ip].density;
    float n_density   = ParticleSystem[np].density;

    glm::vec3 delta   = i_pos - n_pos;
    pressure_factor   = -(MASS * MASS/(i_density * n_density));
    pressure_factor  *=  ((ParticleSystem[ip].pressure + ParticleSystem[np].pressure)/2.0);

    //pressure_factor  *= (ParticleSystem[ip].pressure / (i_density * i_density) + 
    //                     ParticleSystem[np].pressure / (ParticleSystem[np].density * ParticleSystem[np].density) );

    ParticleSystem[ip].force += pressure_factor * KernelGrad(delta, smoothing_scale,h_6);        

    viscosity_factor  = MASS * MASS/(i_density * n_density) * MU * (ParticleSystem[np].velocity - ParticleSystem[ip].velocity);
    //viscosity_factor  = MU * (MASS / ParticleSystem[np].density) * (ParticleSystem[np].velocity - ParticleSystem[ip].velocity);

    ParticleSystem[ip].force += viscosity_factor * KernelLaplacian(delta, smoothing_scale,h_6);

    colorFieldNormal    += MASS * kernelPoly6Grad(delta,smoothing_scale, h_9)/ParticleSystem[np].density;
    colorFieldLaplacian += MASS * kernelPoly6Laplacian(delta,smoothing_scale, h_9)/ParticleSystem[np].density;

    ParticleSystem[ip].force += (-SIGMA) * colorFieldLaplacian * glm::normalize(colorFieldNormal);
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
                         float smoothing_scale, 
                         float h_9,
                         float h_6,
                         float MASS)
{
    //float K     = 500.0f;    // CONSTANT OF SPRING
    float normalDist;
    glm::vec3 Vt_liquit;

    float Vt_normal_value;

    glm::vec3 i_pos;
    glm::vec3 n_pos;

    glm::vec3 gravity_force  = glm::vec3(0.0, -9.80665, 0.0) * MASS;
    //glm::vec3 gravity_force  = glm::vec3(0.0, 0, 0.0) * MASS;
    
    for (int ip = 0; ip < num_particles; ip++)
    {
        ParticleSystem[ip].force = gravity_force;

        //i_pos = ParticleSystem[ip].position;

        for (int ic = 0; ic < 5; ic ++)
        {
            glm::vec3 deltaC = ParticleSystem[ip].position - FluidContainer[ic].center;
            normalDist       = glm::dot(deltaC, FluidContainer[ic].normal);
            
            /*
            if (normalDist > 0 && normalDist <  RadiusMask)
            {   
                ParticleSystem[ip].position = ParticleSystem[ip].position + FluidContainer[ic].normal * (RadiusMask/2 - normalDist)*0.5f;
            }
            */
            if (normalDist < 0) // inner
            {
                ParticleSystem[ip].position = ParticleSystem[ip].position + FluidContainer[ic].normal * (abs(normalDist) + RadiusMask);
            }
            
            if (abs(normalDist) < RadiusMask)
            {
                Vt_normal_value  = glm::dot(ParticleSystem[ip].velocity, FluidContainer[ic].normal);
                Vt_liquit        = ParticleSystem[ip].velocity - Vt_normal_value * FluidContainer[ic].normal;
                ParticleSystem[ip].velocity = Vt_liquit;

                ParticleSystem[ip].density  += MASS * KernelFunction(abs(normalDist), smoothing_scale,h_9) * 10;
                
                
                if (normalDist > 0 && normalDist <  RadiusMask/2)
                {   
                    ParticleSystem[ip].position = ParticleSystem[ip].position + FluidContainer[ic].normal * (RadiusMask/2 - normalDist)*0.5f;
                }
                
                //ParticleSystem[ip].force += K * FluidContainer[ic].normal * MASS * (RadiusMask - abs(normalDist));
                
                float pressure_factor;
                float MU = 0.001f;

                glm::vec3 viscosity_factor;
                
                float i_density   = ParticleSystem[ip].density;

                glm::vec3 delta   = normalDist * FluidContainer[ic].normal;
                //pressure_factor   = - MASS * MASS * i_density;
                //pressure_factor  *= (ParticleSystem[ip].pressure  / (i_density * i_density) + 
                //                     0.0  / (i_density * i_density) );

                pressure_factor    = -(MASS * MASS / (i_density * ParticleSystem[ip].density0));
                pressure_factor   *=  ((ParticleSystem[ip].pressure + 0.0) / 2.0);  // pressure_j = K * ((p/p)**7 - 1)

                //std::cout << "Force 0: " << ParticleSystem[ip].force.y <<std::endl;
                ParticleSystem[ip].force += pressure_factor * KernelGrad(delta, smoothing_scale,h_6) * 1.0f;
                //std::cout << "Force 1: " << ParticleSystem[ip].force.y <<std::endl;
                viscosity_factor  = MU * (MASS / ParticleSystem[ip].density0) * (glm::vec3(0.0,0.0,0.0) - ParticleSystem[ip].velocity);

                ParticleSystem[ip].force += viscosity_factor * KernelLaplacian(delta, smoothing_scale,h_6);

                //std::cout << "Force 2: " << ParticleSystem[ip].force.y <<std::endl;
                
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
                     float h_6,
                     float k0,
                     float MU,
                     float sigma0,
                     float MASS)
{   
    double t_start, t_finish;

    glm::vec3 aceleration;


    float Mask            = Radius * 1.0;    
    float RadiusMask      = Radius + Mask;
    
    t_start  = omp_get_wtime();
    ContainerCollisions(ParticleSystem, FluidContainer, num_particles, RadiusMask, smoothing_scale, h_9, h_6,MASS);

    t_finish = omp_get_wtime();

    double time_elapsed = t_finish - t_start;
    
    //std::cout << "time elapsed 1 = " << time_elapsed * 1000.0 << " ms" << std::endl;

    glm::vec3 i_pos;
    glm::vec3 j_pos;
    glm::vec3 n_pos;

    float delta;
    float i_pressure;
   
    int   n_idx;

    std::vector<int> NEIGHBOURS;

    double t_start2 = omp_get_wtime();
#   pragma omp parallel for private(NEIGHBOURS, i_pressure, n_idx, delta, i_pos, j_pos, n_pos, aceleration) num_threads(8)
    for (int ip = 0; ip < num_particles; ip++)
    {
        i_pos = ParticleSystem[ip].position;
        ParticleSystem[ip].density = 1000.0;
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
        
        
        float newK;
        float newSigma;
        if (i_pos.y < 0.5)
        {
            newK     = k0 * 200;
            newSigma = sigma0 * 100;
        }

        else if (i_pos.y >= 0.5 < i_pos.y < 1)
        {
            newK     = k0;
            newSigma = sigma0;

        }
        else if (i_pos.y >= 1)
        {
            newK     = k0/2.0;
            newSigma = sigma0/2.0;           
        }

        i_pressure  = newK * (pow(ParticleSystem[ip].density/ParticleSystem[ip].density0, 7) - 1);
        ParticleSystem[ip].pressure = i_pressure;

        //if (i_pressure > 0.0)
        //    std::cout << "i_pressure = " << i_pressure << std::endl;
        t_start  = omp_get_wtime();
        for (int i_neighbour = 0; i_neighbour < NEIGHBOURS.size(); i_neighbour++)
        {
            n_idx = NEIGHBOURS[i_neighbour];
            n_pos = ParticleSystem[n_idx].position;

            AccumulateForces(ParticleSystem, ip, n_idx, i_pos, n_pos, smoothing_scale, h_9, h_6, newK, MU, newSigma, MASS);            
        }
        t_finish = omp_get_wtime();

        time_elapsed = t_finish - t_start;
        
        //std::cout << "time elapsed 2:1 = " << time_elapsed * 1000.0 * num_particles << " ms" << std::endl;
    }    
    
    double t_finish2 = omp_get_wtime();

    time_elapsed = t_finish2 - t_start2;
    
    //std::cout << "time elapsed 2 = " << time_elapsed * 1000.0 << " ms" << std::endl;

    for (int ip = 0; ip < num_particles; ip++)
    {
        aceleration  = ParticleSystem[ip].force * (1 /  MASS);
        ParticleSystem[ip].velocity += timeStep * aceleration;
        ParticleSystem[ip].position += ParticleSystem[ip].velocity * timeStep;
    }
    
    tSim += timeStep;
    
}
