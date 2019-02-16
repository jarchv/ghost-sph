#include <math.h> 
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "sphere.hpp"

#define PI 3.14159

glm::vec3 getNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    glm::vec3 norm = glm::normalize(glm::cross(b - a,c - a));
    
    //if(norm.y < 0) // or whatever direction up is
    //    norm = -norm; 
    return norm;
}

void SetSphereNormals(  GLfloat* g_spherevertex_buffer_data,
                        GLfloat* g_spherenormal_buffer_data,
                        int angleRes)
{
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
    glm::vec3 normal;

    for (int i = 0; i < angleRes * angleRes * 18; i+=9)
    {
        a = glm::vec3(  g_spherevertex_buffer_data[i+0],
                        g_spherevertex_buffer_data[i+1],
                        g_spherevertex_buffer_data[i+2]);

        b = glm::vec3(  g_spherevertex_buffer_data[i+3],
                        g_spherevertex_buffer_data[i+4],
                        g_spherevertex_buffer_data[i+5]);
        c = glm::vec3(  g_spherevertex_buffer_data[i+6],
                        g_spherevertex_buffer_data[i+7],
                        g_spherevertex_buffer_data[i+8]);

        normal = getNormal(a,b,c);

        g_spherenormal_buffer_data[i+0] = normal.x;
        g_spherenormal_buffer_data[i+1] = normal.y;
        g_spherenormal_buffer_data[i+2] = normal.z;
        g_spherenormal_buffer_data[i+3] = normal.x;
        g_spherenormal_buffer_data[i+4] = normal.y;
        g_spherenormal_buffer_data[i+5] = normal.z;
        g_spherenormal_buffer_data[i+6] = normal.x;
        g_spherenormal_buffer_data[i+7] = normal.y;
        g_spherenormal_buffer_data[i+8] = normal.z;
    }  
}

void SphereBuffer(  float   radio, 
                    int     angleRes,
                    int     nSphVtx,
                    std::vector<float> sphereColor,
                    GLfloat* g_spherevertex_buffer_data,
                    GLfloat* g_spherecolor_buffer_data,
                    std::vector<float> sphereCenter){
    
	float px, py, pz;
	int i, j;
	float phi   = 2 * PI / ((float)angleRes);
	float theta =     PI / ((float)angleRes);
    glm::vec3   normalGenerated;
	for (i = 0; i < angleRes; i++){
		for (j = 0; j < angleRes; j++) {
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

                g_spherevertex_buffer_data[(i * angleRes + j)*nSphVtx + u    ] = px + sphereCenter[0];
                g_spherevertex_buffer_data[(i * angleRes + j)*nSphVtx + u + 1] = py + sphereCenter[1];
                g_spherevertex_buffer_data[(i * angleRes + j)*nSphVtx + u + 2] = pz + sphereCenter[2];      
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

                g_spherevertex_buffer_data[(i * angleRes + j)*nSphVtx + u    ] = px + sphereCenter[0];
                g_spherevertex_buffer_data[(i * angleRes + j)*nSphVtx + u + 1] = py + sphereCenter[1];
                g_spherevertex_buffer_data[(i * angleRes + j)*nSphVtx + u + 2] = pz + sphereCenter[2];      
            }

            for(int k = 0; k < nSphVtx/3; k++)
            {
                g_spherecolor_buffer_data[(i * angleRes + j)*nSphVtx + k*3 + 0] = sphereColor[0];
                g_spherecolor_buffer_data[(i * angleRes + j)*nSphVtx + k*3 + 1] = sphereColor[1];
                g_spherecolor_buffer_data[(i * angleRes + j)*nSphVtx + k*3 + 2] = sphereColor[2];
            }
		}
	}
}