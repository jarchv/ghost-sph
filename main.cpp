#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

GLFWwindow* window;

#include "common/loadshader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"

static const GLfloat g_vertex_buffer_data[] = {

   -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,

    1.0f,  1.0f, -1.0f,
   -1.0f,  1.0f, -1.0f,
   -1.0f, -1.0f, -1.0f,    

   -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,

    1.0f, -1.0f, -1.0f,
   -1.0f, -1.0f, -1.0f,
   -1.0f, -1.0f,  1.0f,

};

static const GLfloat g_color_buffer_data[] = {
    0.0f,  0.0f,  0.8f,
    0.0f,  0.0f,  0.4f,
    0.0f,  0.0f,  0.8f,

    0.0f,  0.0f,  0.8f,
    0.0f,  0.0f,  0.4f,
    0.0f,  0.0f,  0.8f,

    0.0f,  0.8f,  0.0f,
    0.0f,  0.4f,  0.0f,
    0.0f,  0.8f,  0.0f,

    0.0f,  0.8f,  0.0f,
    0.0f,  0.4f,  0.0f,
    0.0f,  0.8f,  0.0f,
};

/*
static const GLfloat g_uv_buffer_data[] = { 
	0.000059f, 1.0f-0.000004f, 
	0.000103f, 1.0f-0.336048f, 
	0.335973f, 1.0f-0.335903f, 
	1.000023f, 1.0f-0.000013f, 
	0.667979f, 1.0f-0.335851f, 
	0.999958f, 1.0f-0.336064f, 
	0.667979f, 1.0f-0.335851f, 
	0.336024f, 1.0f-0.671877f, 
	0.667969f, 1.0f-0.671889f, 
	1.000023f, 1.0f-0.000013f, 
    0.668104f, 1.0f-0.000013f,
    0.667979f, 1.0f-0.335851f,
};
*/

int main(int argc, char** argv)
{
    if (!glfwInit()){
        std::cout << "Initialization failed" <<std::endl; 
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);

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
    glfwSetCursorPos(window, 640/2, 480/2);
    

    // Dark blue background
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	// Enable depth test
	//glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	//glDepthFunc(GL_LESS); 

	// Descartar los triangulos cuya normal no esta hacia la camara
    glEnable(GL_CULL_FACE);

    
    
    GLuint VertexArrayID;
    glGenBuffers(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = LoadShaders( "vertexShader.glsl", 
                                    "fragmentShader.glsl" );

	// Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");


	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	// Or, for an ortho camera :
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// Camera matrix
	glm::mat4 View  = glm::lookAt(  glm::vec3(0,5,10), // Camera is at (0,5,10), in World Space
								    glm::vec3(0,0,0), // and looks at the origin
								    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                 );

	// Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4(1.0f);
    
    glm::mat4 ScalingMatrix   = glm::scale(glm::mat4(1.0f),glm::vec3(1.0f, 1.0f , 1.0f));
    //Model = ScalingMatrix * Model;
    
    
    glm::vec3 RotationAxis(0.0f, 0.0f, 1.0f);
    glm::mat4 RotationMatrix = glm::rotate(Model, (glm::mediump_float)M_PI * 0.0f, RotationAxis );

    //Model = RotationMatrix * Model;
    
    glm::mat4 TranslateMatrix = glm::translate( Model, glm::vec3(0.0f, 0.0f, 0.0f));
    Model = TranslateMatrix * RotationMatrix * ScalingMatrix * Model;
    
	// Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around    
    
    
    GLuint Texture   = loadBMP("uvtemplate.bmp");
    GLuint TextureID = glGetUniformLocation(programID, "TextureSampler");
    
    
    
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

    
    /*
    GLuint uvbuffer;

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data),
                                         g_uv_buffer_data ,
                                         GL_STATIC_DRAW);
    */
    do {
        glClear(GL_COLOR_BUFFER_BIT);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Calcular la MVP matrix para las entradas del mouse y teclado
        computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;        
		
        // Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

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

        glDrawArrays(GL_TRIANGLES, 0, 3 * 4);         
        
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
