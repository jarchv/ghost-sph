#include <GL/glew.h>
#include <glm/glm.hpp>

#ifndef CONTAINER
#define CONTAINER

static const GLfloat g_vertex_buffer_data[] = 
{
//  Floor
    -2.0f, 0.0f,  0.0f,
    -2.0f, 0.0f,  2.0f,
     2.0f, 0.0f,  0.0f,

     2.0f, 0.0f,  0.0f,
    -2.0f, 0.0f,  2.0f,
     2.0f, 0.0f,  2.0f,

//  Front 0
    -2.0f, 0.0f, 0.0f,
     2.0f, 0.0f, 0.0f,
     2.0f, 2.0f, 0.0f,

     2.0f, 2.0f, 0.0f,
    -2.0f, 2.0f, 0.0f,
    -2.0f, 0.0f, 0.0f,

//  Lateral 0
     2.0f, 0.0f, 0.0f,
     2.0f, 2.0f, 0.0f,
     2.0f, 2.0f, 2.0f,

     2.0f, 2.0f, 2.0f,
     2.0f, 0.0f, 2.0f,
     2.0f, 0.0f, 0.0f,

//  Lateral 1
    -2.0f, 0.0f, 0.0f,
    -2.0f, 2.0f, 2.0f,
    -2.0f, 2.0f, 0.0f,

    -2.0f, 2.0f, 2.0f,
    -2.0f, 0.0f, 0.0f,
    -2.0f, 0.0f, 2.0f,


//  Front 1
    -2.0f, 0.0f, 2.0f,
     2.0f, 0.0f, 2.0f,
     2.0f, 2.0f, 2.0f,

     2.0f, 2.0f, 2.0f,
    -2.0f, 2.0f, 2.0f,
    -2.0f, 0.0f, 2.0f,

};

static const GLfloat g_color_buffer_data[] = {

//  Floor
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,

//  Walls

//  Front 0
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,

//  Lateral 0
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,

//  Lateral 1
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,

//  Front 1
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,
    0.6f,  0.6f,  0.6f,

};

static const GLfloat g_normal_buffer_data[] = {

//  Floor
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,

    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
//  Walls

//  Front 0
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
//  Lateral 0
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
//  Lateral 1
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,

//  Front 1
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
};

struct Container
{
     glm::vec3 center;
     glm::vec3 normal;
};

#endif

void setContainer(Container FluidContainer[]);