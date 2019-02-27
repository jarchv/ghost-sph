#include <glm/glm.hpp>
#include <vector>
#include <omp.h>

#include "sphere.hpp"
#include "physics.hpp"
#include "sampling.hpp"

#define PIV 3.14159

/*
* Navier-Stokes equations:
* =======================
* 
* F = [ F_presure + F_viscosity + F_external]
*
* F_external = gravity
* 
* Presure  = Kgas * [(p / p0)**gamma - 1]
* 
* F_pressure  = - (Vi * Vj) * (Pressure_i + Pressure_j) * 0.5 * Grad(Kernel(ri - rj, h))
* F_viscosity = + (Vi * Vj) * (mu_i + mu_j) * 0.5 *(vj - vi) * Laplacian(Kernel(ri - rj, h))
* 
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

float KernelFunction(float delta, float h, float h_9)
{
    // Poly6 Kernel
    
    return (365.0 / (64.0 * PIV * h_9)) * (h * h - delta * delta) * 
                                          (h * h - delta * delta) * 
                                          (h * h - delta * delta);
}

float KernelLaplacian(glm::vec3 delta, float h, float h_6)
{
    float r = glm::length(delta);
    return  (45.0 / (PIV * h_6 )) * (h - r);
}

glm::vec3 KernelGrad(glm::vec3 delta, float h, float h_6)
{
    float r = glm::length(delta);

    float factor = (45.0 / (PIV * h_6)) * (h - r) * (h - r);
    return  - factor * glm::normalize(delta);   
}

void accumulateForces(  Particle    particleSystem[],
                        Solid       solidSystem[],
                        int         ip, 
                        int         np, 
                        glm::vec3   i_pos, 
                        glm::vec3   n_pos, 
                        float       smoothing_scale, 
                        float       h_9,
                        float       h_6, 
                        float       MU,
                        float       SIGMA,
                        float       MASS,
                        bool        isSolid)
{
    float pressure_factor;

    glm::vec3 viscosity_factor;
    
    glm::vec3 colorFieldNormal;
    float     colorFieldLaplacian;

    float i_density   = particleSystem[ip].density;
    
    float n_density;
    float n_pressure;

    glm::vec3 n_velocity = glm::vec3(0.0,0.0,0.0);

    if (isSolid == true)
    {
        n_density   = particleSystem[ip].density;
        n_pressure  = particleSystem[ip].pressure;
        float Vt_normal_value  = glm::dot(particleSystem[ip].velocity, solidSystem[np].normal);
        glm::vec3 Vt_liquit    = particleSystem[ip].velocity - Vt_normal_value * solidSystem[np].normal;
        
        //particleSystem[ip].velocity = Vt_liquit;
        n_velocity  = Vt_liquit;
    }

    else
    {    
        n_density   = particleSystem[np].density;
        n_pressure  = particleSystem[np].pressure;
        n_velocity  = particleSystem[np].velocity;
    }

    glm::vec3 delta   = i_pos - n_pos;
    pressure_factor   = -(MASS * MASS/(i_density * n_density));
    pressure_factor  *=  ((particleSystem[ip].pressure + n_pressure)/2.0);

    particleSystem[ip].force += pressure_factor * KernelGrad(delta, smoothing_scale,h_6);        

    viscosity_factor  = MASS * MASS/(i_density * n_density) * MU * (n_velocity - particleSystem[ip].velocity);
    //viscosity_factor  = MU * (MASS / particleSystem[np].density) * (particleSystem[np].velocity - particleSystem[ip].velocity);

    particleSystem[ip].force += viscosity_factor * KernelLaplacian(delta, smoothing_scale,h_6);

    colorFieldNormal    += MASS * kernelPoly6Grad(delta,smoothing_scale, h_9)/n_density;
    colorFieldLaplacian += MASS * kernelPoly6Laplacian(delta,smoothing_scale, h_9)/n_density;

    particleSystem[ip].force += (-SIGMA) * colorFieldLaplacian * glm::normalize(colorFieldNormal);
}



void ContainerCollisions(Particle  particleSystem[],
                         Container FluidContainer[], 
                         int num_particles, 
                         float RadiusMask,
                         float smoothing_scale, 
                         float h_9,
                         float h_6,
                         float MASS)
{
    float normalDist;
    glm::vec3 Vt_liquit;

    float Vt_normal_value;

    glm::vec3 i_pos;
    glm::vec3 n_pos;

    glm::vec3 gravity_force  = glm::vec3(0.0, -9.80665, 0.0) * MASS;
    //glm::vec3 gravity_force  = glm::vec3(0.0, 0, 0.0) * MASS;
    
    for (int ip = 0; ip < num_particles; ip++)
    {
        particleSystem[ip].force = gravity_force;

        //i_pos = particleSystem[ip].position;

        for (int ic = 0; ic < 5; ic ++)
        {
            glm::vec3 deltaC = particleSystem[ip].position - FluidContainer[ic].center;
            normalDist       = glm::dot(deltaC, FluidContainer[ic].normal);

            if (normalDist < 0) // inner
            {
                particleSystem[ip].position = particleSystem[ip].position + FluidContainer[ic].normal * (abs(normalDist) + RadiusMask);
            }
            
            if (abs(normalDist) < RadiusMask)
            {
                Vt_normal_value  = glm::dot(particleSystem[ip].velocity, FluidContainer[ic].normal);
                Vt_liquit        = particleSystem[ip].velocity - Vt_normal_value * FluidContainer[ic].normal;
                particleSystem[ip].velocity = Vt_liquit;

                particleSystem[ip].density  += MASS * KernelFunction(abs(normalDist), smoothing_scale,h_9) * 10;
                
                
                if (normalDist > 0 && normalDist <  RadiusMask/2)
                {   
                    particleSystem[ip].position = particleSystem[ip].position + FluidContainer[ic].normal * (RadiusMask/2 - normalDist)*0.5f;
                }
                
                //particleSystem[ip].force += K * FluidContainer[ic].normal * MASS * (RadiusMask - abs(normalDist));
                
                float pressure_factor;
                float MU = 0.001f;

                glm::vec3 viscosity_factor;
                
                float i_density   = particleSystem[ip].density;

                glm::vec3 delta   = normalDist * FluidContainer[ic].normal;
                pressure_factor   = -(MASS * MASS / (i_density * particleSystem[ip].density0));
                pressure_factor  *=  ((particleSystem[ip].pressure + 0.0) / 2.0);

                particleSystem[ip].force += pressure_factor * KernelGrad(delta, smoothing_scale,h_6) * 1.0f;
                
                viscosity_factor  = MU * (MASS / particleSystem[ip].density0) * (glm::vec3(0.0,0.0,0.0) - particleSystem[ip].velocity);

                particleSystem[ip].force += viscosity_factor * KernelLaplacian(delta, smoothing_scale,h_6);
            }   
        }
    }    
}

void DensityEstimator(Particle particleSystem[], int ip, glm::vec3 i_pos, glm::vec3 n_pos, float smoothing_scale, float h_9, float MASS)
{
    float delta = glm::distance(i_pos, n_pos);
    particleSystem[ip].density += MASS * KernelFunction(delta, smoothing_scale,h_9);
}

void SimulatePhysics(   Particle    particleSystem[],
                        Solid       solidSystem[],
                        Container   FluidContainer[], 
                        float&      tSim, 
                        int         num_particles,
                        int         num_solidparticles, 
                        float       timeStep, 
                        float       Radius,
                        float       smoothing_scale,
                        float       h_9,
                        float       h_6,
                        float       k0,
                        float       MU,
                        float       sigma0,
                        float       MASS)
{   
    double t_start, t_finish;

    glm::vec3 aceleration;


    float Mask            = Radius * 1.0;    
    float RadiusMask      = Radius + Mask;
    
    t_start  = omp_get_wtime();
    ContainerCollisions(particleSystem, FluidContainer, num_particles, RadiusMask, smoothing_scale, h_9, h_6,MASS);

    t_finish = omp_get_wtime();

    double time_elapsed = t_finish - t_start;
    
    //std::cout << "time elapsed 1 = " << time_elapsed * 1000.0 << " ms" << std::endl;

    glm::vec3 i_pos;
    glm::vec3 j_pos;
    glm::vec3 n_pos;
    glm::vec3 s_pos;

    float delta;
    float i_pressure;
   
    int   n_idx;

    std::vector<int> PNEIGHBOURS;
    std::vector<int> SNEIGHBOURS;

    double t_start2 = omp_get_wtime();
#   pragma omp parallel for private(PNEIGHBOURS, SNEIGHBOURS, i_pressure, n_idx, delta, i_pos, j_pos, n_pos, s_pos, aceleration) num_threads(8)
    for (int ip = 0; ip < num_particles; ip++)
    {
        i_pos = particleSystem[ip].position;
        particleSystem[ip].density = 1000.0;
        PNEIGHBOURS.clear();
        SNEIGHBOURS.clear();

        /*
        * Step 2: Compute Neighbours
        * ==========================
        */

        for (int jp = 0; jp < num_particles; jp++)
        {
            if (ip != jp)
            {
                j_pos = particleSystem[jp].position;
                delta = glm::distance(i_pos, j_pos);

                if (abs(delta) < smoothing_scale)
                {
                    PNEIGHBOURS.push_back(jp);
                }
            }

        }

        for (int sp = 0; sp < num_solidparticles; sp++)
        {
            s_pos = solidSystem[sp].position;
            delta = glm::distance(i_pos, s_pos);

            if (abs(delta) < smoothing_scale)
            {
                SNEIGHBOURS.push_back(sp);
            }
        }
        
        /*
        * Step 2: Compute Density
        * ========================
        */

        for (int i_neighbour = 0; i_neighbour < PNEIGHBOURS.size(); i_neighbour++)
        {
            n_idx = PNEIGHBOURS[i_neighbour];
            n_pos = particleSystem[n_idx].position;

            DensityEstimator(particleSystem, ip, i_pos, n_pos, smoothing_scale, h_9, MASS);
        }

        for (int s_neighbour = 0; s_neighbour < SNEIGHBOURS.size(); s_neighbour++)
        {
            n_idx = SNEIGHBOURS[s_neighbour];
            s_pos = solidSystem[n_idx].position;
            
            DensityEstimator(particleSystem, ip, i_pos, s_pos, smoothing_scale, h_9, MASS);
        }

        float newK;
        float newSigma;
        if (i_pos.y < 0.5)
        {
            newK     = k0 * 100;
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

        /*
        * Step 3: Compute Pressure
        * ========================
        */

        i_pressure  = newK * (pow(particleSystem[ip].density/particleSystem[ip].density0, 7) - 1);
        particleSystem[ip].pressure = i_pressure;

        //t_start  = omp_get_wtime();
        for (int i_neighbour = 0; i_neighbour < PNEIGHBOURS.size(); i_neighbour++)
        {
            n_idx = PNEIGHBOURS[i_neighbour];
            n_pos = particleSystem[n_idx].position;

            accumulateForces(particleSystem, solidSystem, ip, n_idx, i_pos, n_pos, smoothing_scale, h_9, h_6, MU*8.0, newSigma, MASS, false);            
        }

        for (int s_neighbour = 0; s_neighbour < SNEIGHBOURS.size(); s_neighbour++)
        {
            n_idx = SNEIGHBOURS[s_neighbour];
            s_pos = solidSystem[n_idx].position;
            //std::cout << "pos = " << s_pos.x << ", " << s_pos.y << ", " << s_pos.z << ", r = "<< sqrt(s_pos.x* s_pos.x + s_pos.y*s_pos.y + s_pos.z * s_pos.z) <<std::endl;
            accumulateForces(particleSystem, solidSystem, ip, n_idx, i_pos, s_pos, smoothing_scale*2, h_9, h_6, MU*4.0, newSigma, MASS, true);
            glm::vec3 deltaOBJ = particleSystem[ip].position - s_pos;
            float normalDist   = glm::dot(deltaOBJ, solidSystem[n_idx].normal);

            if (normalDist < 0) // inner
            {
                particleSystem[ip].position = particleSystem[ip].position + solidSystem[n_idx].normal * (abs(normalDist));
            }
            
        }
        //t_finish = omp_get_wtime();

        time_elapsed = t_finish - t_start;
        
        //std::cout << "time elapsed 2:1 = " << time_elapsed * 1000.0 * num_particles << " ms" << std::endl;
    }    
    
    double t_finish2 = omp_get_wtime();

    time_elapsed = t_finish2 - t_start2;
    
    //std::cout << "time elapsed 2 = " << time_elapsed * 1000.0 << " ms" << std::endl;

    for (int ip = 0; ip < num_particles; ip++)
    {
        aceleration  = particleSystem[ip].force * (1 /  MASS);
        particleSystem[ip].velocity += timeStep * aceleration;
        particleSystem[ip].position += particleSystem[ip].velocity * timeStep;
    }
    
    tSim += timeStep;
    
}

void setRandomPosition(Particle ParticleSystem[], float lcube, int ip, glm::vec3 C)
{
    float epsilonX = ((float)(rand()%1000) / 1000.0);
    float epsilonY = ((float)(rand()%1000) / 1000.0);
    float epsilonZ = ((float)(rand()%1000) / 1000.0);
    //std::cout << "epsilon = " << epsilon << std::endl;
    ParticleSystem[ip].position.x = epsilonX * lcube - lcube/2.0 + C.x;//+ 0.0;
    ParticleSystem[ip].position.y = epsilonY * lcube - lcube/2.0 + C.y;
    ParticleSystem[ip].position.z = epsilonZ * lcube - lcube/2.0 + C.z;
    ParticleSystem[ip].velocity  += glm::vec3(0.0,2.0,0.0);
    //ParticleSystem[ip].force      = glm::vec3(0.0,0.0,0.0);   
}


void goBack(Particle ParticleSystem[], int num_particles, float lcube, glm::vec3 C)
{
#   pragma omp parallel for shared(ParticleSystem) num_threads(4)
    for (int ip = 0; ip < num_particles; ip++)   
    {
        if (ParticleSystem[ip].position.y < 0.5f)
        {
            setRandomPosition(ParticleSystem, lcube, ip, C);
        }
    }
}