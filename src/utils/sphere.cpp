#include <math.h> 
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "sphere.hpp"

#define PI 3.14159

glm::vec3 getNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    glm::vec3 norm = glm::normalize(glm::cross(c - b, a - b));
    
    if(norm.y < 0) // or whatever direction up is
        norm = -norm; 
    return norm;
}

void SetSphereNormals(  GLfloat* g_spherevertex_buffer_data,
                        GLfloat* g_spherenormal_buffer_data,
                        int n)
{
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
    glm::vec3 normal;

    int nT = n * n/9;

    for (int i = 0; i < n * n * 2; i++)
    {
        a = glm::vec3(  g_spherevertex_buffer_data[(i*9+0)%9],
                        g_spherevertex_buffer_data[(i*9+1)%9],
                        g_spherevertex_buffer_data[(i*9+2)%9]);

        b = glm::vec3(  g_spherevertex_buffer_data[(i*9+3)%9],
                        g_spherevertex_buffer_data[(i*9+4)%9],
                        g_spherevertex_buffer_data[(i*9+5)%9]);
        c = glm::vec3(  g_spherevertex_buffer_data[(i*9+6)%9],
                        g_spherevertex_buffer_data[(i*9+7)%9],
                        g_spherevertex_buffer_data[(i*9+8)%9]);

        normal = getNormal(a,b,c);

        g_spherenormal_buffer_data[(i*9+0)%9] = normal.x;
        g_spherenormal_buffer_data[(i*9+1)%9] = normal.y;
        g_spherenormal_buffer_data[(i*9+2)%9] = normal.z;
        g_spherenormal_buffer_data[(i*9+3)%9] = normal.x;
        g_spherenormal_buffer_data[(i*9+4)%9] = normal.y;
        g_spherenormal_buffer_data[(i*9+5)%9] = normal.z;
        g_spherenormal_buffer_data[(i*9+6)%9] = normal.x;
        g_spherenormal_buffer_data[(i*9+7)%9] = normal.y;
        g_spherenormal_buffer_data[(i*9+8)%9] = normal.z;
    }  
}

void SphereBuffer(  float   radio, 
                    int     n,
                    int     nSphVtx,
                    GLfloat* g_spherevertex_buffer_data,
                    GLfloat* g_spherecolor_buffer_data){
    
	float px, py, pz;
	int i, j;
	float phi   = 2 * PI / ((float)n);
	float theta =     PI / ((float)n);
    glm::vec3   normalGenerated;
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

                g_spherevertex_buffer_data[(i * n + j)*nSphVtx + u    ] = px;
                g_spherevertex_buffer_data[(i * n + j)*nSphVtx + u + 1] = py;
                g_spherevertex_buffer_data[(i * n + j)*nSphVtx + u + 2] = pz;      
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

                g_spherevertex_buffer_data[(i * n + j)*nSphVtx + u    ] = px;
                g_spherevertex_buffer_data[(i * n + j)*nSphVtx + u + 1] = py;
                g_spherevertex_buffer_data[(i * n + j)*nSphVtx + u + 2] = pz;      
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