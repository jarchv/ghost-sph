#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

GLFWwindow* window;

#include "common/loadshader.hpp"
//#include "common/texture.hpp"
#include "common/controls.hpp"

static const GLfloat g_vertex_buffer_data[] = {


//  Floor
    -2.0f, 0.0f,  0.0f,
    -2.0f, 0.0f,  2.0f,
     2.0f, 0.0f,  0.0f,

     2.0f, 0.0f,  0.0f,
    -2.0f, 0.0f,  2.0f,
     2.0f, 0.0f,  2.0f,

//  Walls

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
    -2.0f, 2.0f, 0.0f,
    -2.0f, 2.0f, 2.0f,

    -2.0f, 2.0f, 2.0f,
    -2.0f, 0.0f, 2.0f,
    -2.0f, 0.0f, 0.0f,

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
    0.0f,  0.4f,  0.0f,
    0.0f,  0.4f,  0.0f,
    0.0f,  0.4f,  0.0f,

    0.0f,  0.4f,  0.0f,
    0.0f,  0.4f,  0.0f,
    0.0f,  0.4f,  0.0f,

//  Walls

//  Front 0
    0.0f,  0.0f,  0.4f,
    0.0f,  0.0f,  0.4f,
    0.0f,  0.0f,  0.4f,

    0.0f,  0.0f,  0.4f,
    0.0f,  0.0f,  0.4f,
    0.0f,  0.0f,  0.4f,

//  Lateral 0
    0.4f,  0.0f,  0.4f,
    0.4f,  0.0f,  0.4f,
    0.4f,  0.0f,  0.4f,

    0.4f,  0.0f,  0.4f,
    0.4f,  0.0f,  0.4f,
    0.4f,  0.0f,  0.4f,

//  Lateral 1
    0.4f,  0.0f,  0.4f,
    0.4f,  0.0f,  0.4f,
    0.4f,  0.0f,  0.4f,

    0.4f,  0.0f,  0.4f,
    0.4f,  0.0f,  0.4f,
    0.4f,  0.0f,  0.4f,

//  Front 1
    0.0f,  0.4f,  0.4f,
    0.0f,  0.4f,  0.4f,
    0.0f,  0.4f,  0.4f,

    0.0f,  0.4f,  0.4f,
    0.0f,  0.4f,  0.4f,
    0.0f,  0.4f,  0.4f,

};

int W = 720;
int H = 480;

int main(int argc, char** argv)
{
    if (!glfwInit()){
        std::cout << "Initialization failed" <<std::endl; 
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(W, H, "My Title", NULL, NULL);

    if (!window){
        std::cout << "Window or OpenGL context creation failer!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // Establer el mouse en el centro
    glfwPollEvents();
    glfwSetCursorPos(window, W/2, H/2);
    glDisable(GL_CULL_FACE);

    // Dark blue background
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	// Enable depth test
	//glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	//glDepthFunc(GL_LESS); 

	// Descartar los triangulos cuya normal no esta hacia la camara
    //glEnable(GL_CULL_FACE);

    
    
    GLuint VertexArrayID;
    glGenBuffers(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = LoadShaders( "../src/vertexShader.glsl", 
                                    "../src/fragmentShader.glsl" );

	// Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifiers in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give out vertices to OpenGL
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data),
                                         g_vertex_buffer_data ,
                                         GL_STATIC_DRAW       );

     
    GLuint colorbuffer;

    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data),
                                         g_color_buffer_data ,
                                         GL_STATIC_DRAW);   

    do {
        glClear(GL_COLOR_BUFFER_BIT);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Calcular la MVP matrix para las entradas del mouse y teclado
        computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix  = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);

        glm::vec4 lightPos = glm::vec4(4,4,1,0);
        
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;        
		
        GLuint MatrixID      = glGetUniformLocation(programID, "MVP");
        GLuint ViewMatrixID  = glGetUniformLocation(programID, "V");
        GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
        GLuint LightID       = glGetUniformLocation(programID, "LightPosition_worldspace");
        
        // Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,          // attibute 0, must match the layout in the shader
            3,          // size
            GL_FLOAT,   // type
            GL_FALSE,   // normalizerd?
            0,          // stride
            (void*)0    // array buffer offset
        );

        // 2nd attribute buffer : colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(
            1,          // attibute 0, must match the layout in the shader
            3,          // size
            GL_FLOAT,   // type
            GL_FALSE,   // normalizerd?
            0,          // stride
            (void*)0    // array buffer offset
        );

        
        // Draw the triangle
        // Starting from vertex 0; 3 vertices total -> 1 triangle

        int nTriangles = sizeof(g_color_buffer_data)/(3 * 3 * 4 );
        glDrawArrays(GL_TRIANGLES, 0, 3 * nTriangles);         
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);


        // Swap buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
          glfwWindowShouldClose(window)       == 0            );
    

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);
    
    glfwTerminate();
    
    return 0;
}
