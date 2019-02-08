#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

#ifndef PARTICLE
#define PARTICLE

struct Particle{
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 force;
    float     density0 = 997.0;
    float     density  = 997.0;
    float     pressure;
};

#endif
void SphereBuffer(  float radio, 
                    int   n,
                    int   nSphVtx,
                    GLfloat* g_spherevertex_buffer_data,
                    GLfloat* g_spherecolor_buffer_data);
                
void SetSphereNormals(  GLfloat* g_spherevertex_buffer_data,
                        GLfloat* g_spherenormal_buffer_data,
                        int n);