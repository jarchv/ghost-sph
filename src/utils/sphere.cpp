#include <math.h> 
#include <GL/glew.h>

#include "sphere.hpp"

#define PI 3.14159

void SphereBuffer(  float   radio, 
                    float   x0, 
                    float   y0, 
                    float   z0,
                    int     n,
                    int     nSphVtx,
                    GLfloat* g_spherevertex_buffer_data,
                    GLfloat* g_spherecolor_buffer_data){
    
	float px, py, pz;
	int i, j;
	float phi   = 2 * PI / ((float)n);
	float theta =     PI / ((float)n);

	for (i = 0; i < n; i++){
		for (j = 0; j < n; j++) {
			pz = cos(PI - (theta*j))*radio;
			py = sin(PI - (theta*j))*sin(phi*i)*radio;
			px = sin(PI - (theta*j))*cos(phi*i)*radio;

            int di = 0;
            int dj = 0;
            
            // Bottom Triangle
            
            for (int u = 0; u < nSphVtx/2; u+=3)
            {
                di =  u >= 3? 1: 0;
                dj =  u >= 6? 1: 0;

                pz = cos(PI - theta*(j + dj))*radio;
                py = sin(PI - theta*(j + dj))*sin(phi*(i + di))*radio;
                px = sin(PI - theta*(j + dj))*cos(phi*(i + di))*radio;          

                g_spherevertex_buffer_data[(i * n + j)*nSphVtx + u    ] = x0 + px;
                g_spherevertex_buffer_data[(i * n + j)*nSphVtx + u + 1] = y0 + py;
                g_spherevertex_buffer_data[(i * n + j)*nSphVtx + u + 2] = z0 + pz;      
            }

            di = 0;
            dj = 0;

            // Top Triangle

            for (int u = nSphVtx/2; u < nSphVtx; u+=3)
            {
                di =  u < 12? 1: 0;
                dj =  u < 15? 1: 0;

                pz = cos(PI - theta*(j + dj))*radio;
                py = sin(PI - theta*(j + dj))*sin(phi*(i + di))*radio;
                px = sin(PI - theta*(j + dj))*cos(phi*(i + di))*radio;          

                g_spherevertex_buffer_data[(i * n + j)*nSphVtx + u    ] = x0 + px;
                g_spherevertex_buffer_data[(i * n + j)*nSphVtx + u + 1] = y0 + py;
                g_spherevertex_buffer_data[(i * n + j)*nSphVtx + u + 2] = z0 + pz;      
            }

            for(int k = 0; k < nSphVtx/3; k++)
            {
                g_spherecolor_buffer_data[(i * n + j)*nSphVtx + k*3 + 0] = 0.0f;
                g_spherecolor_buffer_data[(i * n + j)*nSphVtx + k*3 + 1] = 0.5f;
                g_spherecolor_buffer_data[(i * n + j)*nSphVtx + k*3 + 2] = 0.45f;
            }
		}
	}
}