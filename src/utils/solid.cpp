#include <iostream>
#include <glm/glm.hpp>

#include "solid.hpp"
#define  OBJPI 3.14159

void createObject(Solid solidSystem[], int num_objectparticles, float objRadius, std::vector<float> objCener)
{
    float tempRadius;
    float theta;
    float phi;

    float posX;
    float posY;
    float posZ;

    for (int i = 0; i < num_objectparticles; i++)
    {
        //tempRadius  = ((float)(rand()%1000) / 1000.0);
        tempRadius  = objRadius;
        
        theta       = ((float)(rand()%1000) / 1000.0);
        theta      *= OBJPI;

        phi         = ((float)(rand()%1000) / 1000.0);
		phi        *= (2 * OBJPI);

        posX = cos(OBJPI - theta)*           tempRadius + objCener[0];
		posY = sin(OBJPI - theta)*sin(phi) * tempRadius + objCener[1];
		posZ = sin(OBJPI - theta)*cos(phi) * tempRadius + objCener[2];

        solidSystem[i].position = glm::vec3(posX, posY, posZ);       
    }
}