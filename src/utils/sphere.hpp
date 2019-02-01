#include <GL/glew.h>
#include <iostream>

void SphereBuffer(  float radio, 
                    float x0, 
                    float y0, 
                    float z0,
                    int   n,
                    int   nSphVtx,
                    GLfloat* g_spherevertex_buffer_data,
                    GLfloat* g_spherecolor_buffer_data);


void setInitialPosition(float*  spherePos, 
                        int     num_particles,
                        float   kf);