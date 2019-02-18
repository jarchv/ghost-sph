#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#ifndef SOLID
#define SOLID

struct Solid{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 velocity = glm::vec3(0.0,0.0,0.0);
    glm::vec3 force    = glm::vec3(0.0,0.0,0.0);
    float     density0 = 1000.0;
    float     density  = 1000.0;
    float     pressure;
};

#endif

void createObject(Solid solidSystem[], int num_objectparticles, float objRadius, std::vector<float> objCener);