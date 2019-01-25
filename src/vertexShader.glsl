#version 330 core

/*
location = 0, refers to the buffer we use to feed the VertexPosition_modelspace
attibrute.

Each vertex can have numerous attributes : A position, one or several colours, 
one or several texture coordinates, lots of other things. 
OpenGL doesn’t know what a colour is : it just sees a vec3. 
So we have to tell him which buffer corresponds to which input. 
We do that by setting the layout to the same value as the first parameter 
to glVertexAttribPointer. The value “0” is not important, 
it could be 12 (but no more than glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &v) ), 
the important thing is that it’s the same number on both sides.

“vertexPosition_modelspace” could have any other name. It will contain
the position of the vertex for each run of the vertex shader.

“in” means that this is some input data. Soon we’ll see the “out” keyword.

*/

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
//layout(location = 1) in vec2 vertexUV;

// Output data: will be interpolated for each fragment
out vec3 fragmentColor;
//out vec2 UV;

// Values that stay constant for whole mesh
uniform mat4 MVP;

void main()
{
    // gl_Position.xyz = vertexPosition_modelspace;
    // gl_Position.w   = 1.0;

    // Output position of the vertex, in clip space : MVP * position
    gl_Position   = MVP * vec4(vertexPosition_modelspace, 1);

    // vertexColor
    fragmentColor = vertexColor;
    
    // UV of vertex. No special space for this one.
    //UV = vertexUV; 
}