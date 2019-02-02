#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
        
GLFWwindow* window;
        
#include "common/loadshader.hpp"
#include "common/controls.hpp"

#include "utils/sphere.hpp"
#include "utils/container.hpp"
#include "utils/sampling.hpp"
#include "utils/physics.hpp"

int num_particles  = 125;
float *spherePos   = new float[num_particles*3];

int W = 720;
int H = 480;

float tSim     = 0.0;
float v0       = 0.0;
float timeStep = 0.005;

void init(void)
{
    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

    // Descartar los triangulos cuya normal no esta hacia la camara
    glDisable(GL_CULL_FACE);
    
	// Enable depth test
	//glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	//glDepthFunc(GL_LESS); 
}


int main(int argc, char** argv)
{

    if (!glfwInit()){
        std::cout << "Initialization GLFW library failed!" <<std::endl; 
        return -1;
    }

    // Setting hint for glfwCreateWindow
    glfwWindowHint(GLFW_SAMPLES                 , 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR   , 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR   , 0);

    window = glfwCreateWindow(W, H, "Ghost SPH", NULL, NULL);

    if (!window){
        std::cout << "Window or OpenGL context creation failed!" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Make the context of our window the main context on the current thread
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

    init();

    // n: angles per theta/phi
    int n           = 10;
    int nSphVtx     = 18;
    int sphereSize  = nSphVtx * n * n;

    static GLfloat *g_spherevertex_buffer_data = new GLfloat[sphereSize];
    static GLfloat *g_spherecolor_buffer_data  = new GLfloat[sphereSize];
    
    SphereBuffer(0.1, 0.0, 2.0, 0.5, n, nSphVtx,    g_spherevertex_buffer_data, 
                                                    g_spherecolor_buffer_data);

    std::cout << sphereSize <<std::endl;
    std::cout << g_spherevertex_buffer_data[0] <<std::endl;
    GLuint VertexArrayID;
    glGenBuffers(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = LoadShaders( "../src/vertexShader.glsl", 
                                    "../src/fragmentShader.glsl" );

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

    GLuint normalbuffer;

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_normal_buffer_data),
                                         g_normal_buffer_data ,
                                         GL_STATIC_DRAW);   


    GLuint spherebuffer;

    glGenBuffers(1, &spherebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, spherebuffer);
    glBufferData(GL_ARRAY_BUFFER, sphereSize * sizeof(float),
                                         g_spherevertex_buffer_data ,
                                         GL_STATIC_DRAW); 

    GLuint spherecolor_buffer;

    glGenBuffers(1, &spherecolor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, spherecolor_buffer);
    glBufferData(GL_ARRAY_BUFFER, sphereSize * sizeof(float),
                                         g_spherecolor_buffer_data ,
                                         GL_STATIC_DRAW); 

    glUseProgram(programID);
    GLuint TransparentID = glGetUniformLocation(programID, "Transparent");
    GLuint LightID       = glGetUniformLocation(programID, "LightPosition_worldspace");
    
        
    GLuint MatrixID      = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID  = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");     

	// For speed computation
	double lastTime  = glfwGetTime();
    double startTime = lastTime; 
    int nbFrames = 0;

    // Enable blending
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setInitialPosition(spherePos, num_particles, 0.2);
    float fall = 0.0;

    do {
        double currentTime = glfwGetTime();
        double delta = currentTime - startTime;
        startTime = currentTime;
        nbFrames++;

		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame\n", 1000.0/double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
            
        }

        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Calcular la MVP matrix para las entradas del mouse y teclado
        computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix  = getProjectionMatrix();
		glm::mat4 ViewMatrix        = getViewMatrix();
		glm::mat4 ModelMatrix       = glm::mat4(1.0);
        glm::mat4 MVP               = ProjectionMatrix * ViewMatrix * ModelMatrix;   
        

        // Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glm::vec3 lightPos = glm::vec3(10,6,10);
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
        glUniform1f(TransparentID,0.3);
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
            1,          // attibute 1, must match the layout in the shader
            3,          // size
            GL_FLOAT,   // type
            GL_FALSE,   // normalizerd?
            0,          // stride
            (void*)0    // array buffer offset
        );

        // 3th attribute buffer : normal
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(
            2,          // attibute 2, must match the layout in the shader
            3,          // size
            GL_FLOAT,   // type
            GL_FALSE,   // normalizerd?
            0,          // stride
            (void*)0    // array buffer offset
        );

        int nTriangles = sizeof(g_color_buffer_data)/(3 * 3 * 4 );
        glDrawArrays(GL_TRIANGLES, 0, 3 * nTriangles);   

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);



        glUseProgram(programID);

        glUniform1f(TransparentID,0.8);

        // 1th attribute buffer : sphere
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, spherebuffer);
        glVertexAttribPointer(
            0,          // attibute 2, must match the layout in the shader
            3,          // size
            GL_FLOAT,   // type
            GL_FALSE,   // normalizerd?
            0,          // stride
            (void*)0    // array buffer offset
        );

        // 2th attribute buffer : colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, spherecolor_buffer);
        glVertexAttribPointer(
            1,          // attibute 2, must match the layout in the shader
            3,          // size
            GL_FLOAT,   // type
            GL_FALSE,   // normalizerd?
            0,          // stride
            (void*)0    // array buffer offset
        );

        // Draw the triangle
        // Starting from vertex 0; 3 vertices total -> 1 triangle
        collisions(spherePos, tSim, v0, num_particles, timeStep);

        for(size_t i = 0; i < num_particles; i++)
        {
            ModelMatrix       = glm::translate(glm::mat4(1.0),glm::vec3((float)spherePos[i*3 + 0],
                                                                        (float)spherePos[i*3 + 1],
                                                                        (float)spherePos[i*3 + 2]));
            MVP               = ProjectionMatrix * ViewMatrix * ModelMatrix;   
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, sphereSize);   
        
        }
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        // Swap buffer
        glfwSwapBuffers(window);
        glfwPollEvents();

        fall-= 0.01;
    }

    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
          glfwWindowShouldClose(window)       == 0            );
    

	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &spherebuffer);
    glDeleteBuffers(1, &spherecolor_buffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);
    
    glfwTerminate();
    
    return 0;
}
