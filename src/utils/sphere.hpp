#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

#ifndef PARTICLE_STRUCT
#define PARTICLE_STRUCT
struct Particle{
    glm::vec3 position;
    glm::vec3 velocity;
};
#endif
void SphereBuffer(  float radio, 
                    float x0, 
                    float y0, 
                    float z0,
                    int   n,
                    int   nSphVtx,
                    GLfloat* g_spherevertex_buffer_data,
                    GLfloat* g_spherecolor_buffer_data);