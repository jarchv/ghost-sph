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

#include "utils/sampling.hpp"
#include "utils/physics.hpp"
#include "utils/container.hpp"
#include "utils/solid.hpp"

const int LCUBE          = 12;
const int num_particles  = LCUBE * LCUBE * LCUBE;

Particle  ParticleSystem[num_particles];
Container FluidContainer[5];

float K               = 10.0f;
float MU              = 100.0f;
float SIGMA           = 0.000;
float MASS            = 0.12f;          // MASS OF A DROP = 50e-6 kg (Set 20 drops)

int windowSizeW       = 720;
int windowSizeH       = 480;

float tSim            = 0.0;
float v0              = 0.0;
float timeStep        = 0.01;

float particleCubeSize = 0.5;
float cubePartPosX0    = 0.0;
float cubePartPosY0    = 4.5;
float cubePartPosZ0    = 1.0;


float particleRadius   = 0.05;
float smoothing_scale  = particleRadius * 3.0;

float h_9           = smoothing_scale * smoothing_scale * smoothing_scale *
                      smoothing_scale * smoothing_scale * smoothing_scale * 
                      smoothing_scale * smoothing_scale * smoothing_scale;

float h_6           = smoothing_scale * smoothing_scale * smoothing_scale * 
                      smoothing_scale * smoothing_scale * smoothing_scale;

// angleRes: angles per theta/phi


int angleRes                      = 10;
int nSphVtx                       = 18;
int sphereSizeRes                 = nSphVtx * angleRes * angleRes;
//std::vector<float> particleColor  = {0.0, 0.5, 0.45};
std::vector<float> particleColor  = {0.0, 0.5, 0.95};
std::vector<float> particleCenter = {0.0, 0.0, 0.0};

/*
* Setting Object:
* ===============
*/

float objRadius                   = 0.7;
int objAngleRes                   = 40;
int objectSizeRes                 = nSphVtx * objAngleRes * objAngleRes;
std::vector<float> objectColor    = {0.95, 0.1, 0.15};
std::vector<float> objectCenter   = {0.0, 3.0, 1.0};

const int obj_num_particles       = 1000;

Solid solidSystem[obj_num_particles];

void init(void)
{
    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

    // Descartar los triangulos cuya normal no esta hacia la camara
    //glDisable(GL_CULL_FACE);
        
	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

    // Enable blending
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    window = glfwCreateWindow(windowSizeW , windowSizeH, "Ghost SPH", NULL, NULL);

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
    glfwSetCursorPos(window, windowSizeW/2, windowSizeH/2);

    init();

    static GLfloat *g_spherevertex_buffer_data = new GLfloat[sphereSizeRes];
    static GLfloat *g_spherecolor_buffer_data  = new GLfloat[sphereSizeRes];
    static GLfloat *g_spherenormal_buffer_data = new GLfloat[sphereSizeRes];

    static GLfloat *g_objectvertex_buffer_data = new GLfloat[objectSizeRes];
    static GLfloat *g_objectcolor_buffer_data  = new GLfloat[objectSizeRes];
    static GLfloat *g_objectnormal_buffer_data = new GLfloat[objectSizeRes];

    SphereBuffer(particleRadius, angleRes, nSphVtx, particleColor, g_spherevertex_buffer_data, 
                                            g_spherecolor_buffer_data, particleCenter);


    SetSphereNormals(   g_spherevertex_buffer_data, 
                        g_spherenormal_buffer_data,
                        angleRes);

    SphereBuffer(objRadius, objAngleRes, nSphVtx, objectColor,  g_objectvertex_buffer_data, 
                                                g_objectcolor_buffer_data, objectCenter);


    SetSphereNormals(   g_objectvertex_buffer_data, 
                        g_objectnormal_buffer_data,
                        objAngleRes);

    createObject(solidSystem, obj_num_particles, objRadius, objectCenter);

    std::cout << "num of particles : " << num_particles <<std::endl;
    GLuint VertexArrayID;
    glGenBuffers(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = LoadShaders( "../src/vertexShader.glsl", 
                                    "../src/fragmentShader.glsl" );

    /*
    * Container
    * ============================================================== 
    */

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


    /*
    * Particle
    * ============================================================== 
    */

    GLuint spherebuffer;

    glGenBuffers(1, &spherebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, spherebuffer);
    glBufferData(GL_ARRAY_BUFFER, sphereSizeRes * sizeof(float),
                                         g_spherevertex_buffer_data ,
                                         GL_STATIC_DRAW); 

    GLuint spherecolor_buffer;

    glGenBuffers(1, &spherecolor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, spherecolor_buffer);
    glBufferData(GL_ARRAY_BUFFER, sphereSizeRes * sizeof(float),
                                         g_spherecolor_buffer_data ,
                                         GL_STATIC_DRAW); 

    GLuint spherenormal_buffer;

    glGenBuffers(1, &spherenormal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, spherenormal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sphereSizeRes * sizeof(float),
                                         g_spherenormal_buffer_data ,
                                         GL_STATIC_DRAW);   

    /*
    * Object
    * ======
    * 
    */

    GLuint objectbuffer;

    glGenBuffers(1, &objectbuffer);
    glBindBuffer(GL_ARRAY_BUFFER,   objectbuffer);
    glBufferData(GL_ARRAY_BUFFER,   objectSizeRes * sizeof(float),
                                    g_objectvertex_buffer_data ,
                                    GL_STATIC_DRAW); 

    GLuint objectcolor_buffer;

    glGenBuffers(1, &objectcolor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, objectcolor_buffer);
    glBufferData(GL_ARRAY_BUFFER,   objectSizeRes * sizeof(float),
                                    g_objectcolor_buffer_data ,
                                    GL_STATIC_DRAW); 

    GLuint objectnormal_buffer;

    glGenBuffers(1, &objectnormal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, objectnormal_buffer);
    glBufferData(GL_ARRAY_BUFFER, objectSizeRes * sizeof(float),
                                         g_objectnormal_buffer_data ,
                                         GL_STATIC_DRAW);  
    
    /*
    * Transparent input parameter
    * ===========================
    */
    glUseProgram(programID);
    GLuint TransparentID = glGetUniformLocation(programID, "Transparent");
    GLuint LightID       = glGetUniformLocation(programID, "LightPosition_worldspace");
    
        
    GLuint MatrixID      = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID  = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");     

	// For speed computation
	double lastTime  = glfwGetTime();
    double startTime = lastTime; 
    int    nbFrames  = 0;

    setContainer(FluidContainer);
    
    /*
    * Setting Initial Position for the cube of particles
    * ===================================================
    *  
    *  Center = (cubePartPosX0, cubePartPosY0, cubePartPosZ0)
    * 
    */
    setCubeRandomly(ParticleSystem, num_particles, particleCubeSize, cubePartPosX0, cubePartPosY0, cubePartPosZ0);

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
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        /*
        * Container:
        * ============================
        */    
        
        glUniform1f(TransparentID,0.2);
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
        
        /*
        * Object:
        * ============================
        */

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);

        glUniform1f(TransparentID,1.0);

        // 1th attribute buffer : sphere
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, objectbuffer);
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
        glBindBuffer(GL_ARRAY_BUFFER, objectcolor_buffer);
        glVertexAttribPointer(
            1,          // attibute 2, must match the layout in the shader
            3,          // size
            GL_FLOAT,   // type
            GL_FALSE,   // normalizerd?
            0,          // stride
            (void*)0    // array buffer offset
        );

        // 3th attribute buffer : colors
        
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, objectnormal_buffer);
        glVertexAttribPointer(
            2,          // attibute 2, must match the layout in the shader
            3,          // size
            GL_FLOAT,   // type
            GL_FALSE,   // normalizerd?
            0,          // stride
            (void*)0    // array buffer offset
        );

        glDrawArrays(GL_TRIANGLES, 0, objectSizeRes/3 );  

        /*
        * Particles:
        * ============================
        */
         
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        //glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);

        glUniform1f(TransparentID,1.0);

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

        // 3th attribute buffer : colors
        
        glBindBuffer(GL_ARRAY_BUFFER, spherenormal_buffer);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2,          // attibute 2, must match the layout in the shader
            3,          // size
            GL_FLOAT,   // type
            GL_FALSE,   // normalizerd?
            0,          // stride
            (void*)0    // array buffer offset
        );
        
        // Draw the triangle
        // Starting from vertex 0; 3 vertices total -> 1 triangle
        
        SimulatePhysics(ParticleSystem, 
                        solidSystem, 
                        FluidContainer, 
                        tSim, 
                        v0, 
                        num_particles, 
                        obj_num_particles, 
                        timeStep, 
                        particleRadius, 
                        smoothing_scale, 
                        h_9, h_6, K, MU, SIGMA, MASS);

        for(size_t i = 0; i < num_particles; i++)
        {
            ModelMatrix       = glm::translate(glm::mat4(1.0),glm::vec3(ParticleSystem[i].position.x,
                                                                        ParticleSystem[i].position.y,
                                                                        ParticleSystem[i].position.z));
            MVP               = ProjectionMatrix * ViewMatrix * ModelMatrix;   
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, sphereSizeRes/3 );   
        
        }
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        // Swap buffer
        glfwSwapBuffers(window);
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    }

    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
          glfwWindowShouldClose(window)       == 0            );
    


	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteBuffers(1, &normalbuffer);
    
    glDeleteBuffers(1, &spherebuffer);
    glDeleteBuffers(1, &spherecolor_buffer);
    glDeleteBuffers(1, &spherenormal_buffer);

    glDeleteBuffers(1, &objectbuffer);
    glDeleteBuffers(1, &objectcolor_buffer);
    glDeleteBuffers(1, &objectnormal_buffer);

    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);
    
    glfwTerminate();

    delete [] g_spherevertex_buffer_data;
    delete [] g_spherecolor_buffer_data;
    delete [] g_spherenormal_buffer_data;

    delete [] g_objectvertex_buffer_data;
    delete [] g_objectcolor_buffer_data;
    delete [] g_objectnormal_buffer_data;

    return 0;
}
