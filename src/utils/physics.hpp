#include <iostream>
#include "container.hpp"

void SimulatePhysics(Particle ParticleSystem[], 
                     Container FluidContainer[], 
                     float&tSim, float& v0, 
                     int num_particles, 
                     float timeStep, 
                     float Radius,
                     float smoothing_scale,
                     float h_9,
                     float h_6 );