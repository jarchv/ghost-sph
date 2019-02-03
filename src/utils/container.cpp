#include <glm/glm.hpp>
#include "container.hpp"

void setContainer(Container FluidContainer[])
{
    FluidContainer[0].center = glm::vec3( 0.0, 0.0, 1.0); // floor
    FluidContainer[1].center = glm::vec3(-2.0, 1.0, 1.0); // left
    FluidContainer[2].center = glm::vec3( 2.0, 1.0, 1.0); // right
    FluidContainer[3].center = glm::vec3( 0.0, 1.0, 0.0); // back
    FluidContainer[4].center = glm::vec3( 0.0, 1.0, 2.0); // front

    // Normal Direction : Internal

    FluidContainer[0].normal = glm::vec3( 0.0, 1.0, 0.0); // floor
    FluidContainer[1].normal = glm::vec3( 1.0, 0.0, 0.0); // left
    FluidContainer[2].normal = glm::vec3(-1.0, 0.0, 0.0); // right
    FluidContainer[3].normal = glm::vec3( 0.0, 0.0, 1.0); // back
    FluidContainer[4].normal = glm::vec3( 0.0, 0.0,-1.0); // front
}
