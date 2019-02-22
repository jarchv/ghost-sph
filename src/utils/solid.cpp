#include <iostream>
#include <glm/glm.hpp>

#include "solid.hpp"
#define  OBJPI 3.14159

void createObject(Solid solidSystem[], int num_objectparticles, int angleRes, float objRadius, std::vector<float> objCener)
{
    //float tempRadius;
    //float theta;
    //float phi;

    float posX;
    float posY;
    float posZ;

    //int angleRes = 20;

	float phi   = 2 * OBJPI / ((float)angleRes);
	float theta =     OBJPI / ((float)angleRes);
    
    int px, py, pz;

    assert (num_objectparticles == angleRes * angleRes);
	for (int i = 0; i < angleRes; i++){
		for (int j = 0; j < angleRes; j++) {
			posZ = cos(OBJPI - (theta*j))*objRadius            + objCener[2];;
			posY = sin(OBJPI - (theta*j))*sin(phi*i)*objRadius + objCener[1];;
			posX = sin(OBJPI - (theta*j))*cos(phi*i)*objRadius + objCener[0];;

            solidSystem[i*angleRes + j].position = glm::vec3(posX, posY, posZ);
            solidSystem[i*angleRes + j].normal   = glm::vec3(sin(OBJPI - (theta*j))*cos(phi*i)*objRadius, 
                                                sin(OBJPI - (theta*j))*sin(phi*i)*objRadius, 
                                                cos(OBJPI - (theta*j))*objRadius);  
        }
    }
    /*
    for (int i = 0; i < num_objectparticles; i++)
    {
        //tempRadius  = ((float)(rand()%1000) / 1000.0);
        tempRadius  = objRadius * 1.0;
        
        theta       = ((float)(rand()%1000) / 1000.0);
        theta      *= OBJPI;

        phi         = ((float)(rand()%1000) / 1000.0);
		phi        *= (2.0 * OBJPI);
        
        //std::cout << "theta : " << theta << std::endl;
        posX = cos(OBJPI - theta)*           tempRadius + objCener[0];
		posY = sin(OBJPI - theta)*sin(phi) * tempRadius + objCener[1];
		posZ = sin(OBJPI - theta)*cos(phi) * tempRadius + objCener[2];

        solidSystem[i].position = glm::vec3(posX, posY, posZ);
        solidSystem[i].normal   = glm::vec3(cos(OBJPI - theta), 
                                            sin(OBJPI - theta)*sin(phi), 
                                            sin(OBJPI - theta)*cos(phi));       
    }
    */
}