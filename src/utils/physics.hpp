#include <iostream>

#include "container.hpp"
#include "solid.hpp"

void SimulatePhysics(   Particle    ParticleSystem[],
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
                        float       MASS);

void goBack(Particle ParticleSystem[], int num_particles, float lcube, glm::vec3 C);