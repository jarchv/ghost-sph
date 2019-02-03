#include <iostream>
#include "container.hpp"

void SimulatePhysics(   Particle ParticleSystem[], 
                        Container FluidContainer[], 
                        float& tSim,
                        float& v0,
                        int num_particles,
                        float timeStep,
                        float Radius);