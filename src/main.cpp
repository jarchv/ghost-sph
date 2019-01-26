#include <iostream>
#include <algorithm>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
        
GLFWwindow* window;
        
#include "common/loadshader.hpp"
#include "common/controls.hpp"


struct Particle
{
    glm::vec3 pos, speed;
    unsigned char r,g,b,a;
    float size, angle, weight;
    float life;
    float cameradistance;

    bool operator<(const Particle& that) const{
        return this->cameradistance > that.cameradistance;
    }
};

const int MaxParticles = 100;

Particle ParticlesContainer[MaxParticles];

int LastUsedParticle = 0;

// Find particles in container
int FindUnusedParticle()
{
    for(int i = LastUsedParticle; i < MaxParticles; i++)
    {
        if (ParticlesContainer[i].life < 0)
        {
            LastUsedParticle = i;
            return i;
        }
    }

    for(int i = 0; i < LastUsedParticle; i++)
    {
        if(ParticlesContainer[i].life < 0)
        {
            LastUsedParticle = i;
            return i;
        }
    }

    return 0;
}

void SortParticles()
{
    std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}
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
    0.2f,  0.2f,  0.5f,
    0.2f,  0.2f,  0.5f,
    0.2f,  0.2f,  0.5f,

    0.2f,  0.2f,  0.5f,
    0.2f,  0.2f,  0.5f,
    0.2f,  0.2f,  0.5f,

//  Walls

//  Front 0
    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,

    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,

//  Lateral 0
    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,

    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,

//  Lateral 1
    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,

    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,

//  Front 1
    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,

    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,
    0.2f,  0.2f,  0.4f,

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
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

	// Enable depth test
	//glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Descartar los triangulos cuya normal no esta hacia la camara
    //glEnable(GL_CULL_FACE);

    
    
    GLuint VertexArrayID;
    glGenBuffers(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = LoadShaders( "../src/vertexShader.glsl", 
                                    "../src/fragmentShader.glsl" );

	static GLfloat* g_particule_position_size_data = new GLfloat[MaxParticles * 4];
	static GLubyte* g_particule_color_data         = new GLubyte[MaxParticles * 4];

	for(int i=0; i<MaxParticles; i++){
		ParticlesContainer[i].life = -1.0f;
		ParticlesContainer[i].cameradistance = -1.0f;
    }

	// The VBO containing the 4 vertices of the particles.
	// Thanks to instancing, they will be shared by all particles.
	static const GLfloat g_vertexparticle_buffer_data[] = { 
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f,
	};
	GLuint billboard_vertex_buffer;
	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertexparticle_buffer_data), g_vertexparticle_buffer_data, GL_STATIC_DRAW);

	GLuint particles_position_buffer;
	glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	GLuint particles_color_buffer;
	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
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


    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	// For speed computation
	double lastTime  = glfwGetTime();
    double startTime = lastTime; 
    int nbFrames = 0;

    // Enable blending
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
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
        glClear(GL_COLOR_BUFFER_BIT);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Calcular la MVP matrix para las entradas del mouse y teclado
        computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix  = getProjectionMatrix();
		glm::mat4 ViewMatrix        = getViewMatrix();
		glm::mat4 ModelMatrix       = glm::mat4(1.0);
        glm::mat4 MVP               = ProjectionMatrix * ViewMatrix * ModelMatrix;      
          
        GLuint MatrixID      = glGetUniformLocation(programID, "MVP");
        GLuint ViewMatrixID  = glGetUniformLocation(programID, "V");
        GLuint ModelMatrixID = glGetUniformLocation(programID, "M");     
        

        glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);
        int newparticles = (int)(delta*10000.0);

        if (newparticles > (int)(0.016f*10000.0))
            newparticles = (int)(0.016f*10000.0);

		for(int i=0; i<newparticles; i++){
			int particleIndex = FindUnusedParticle();
			ParticlesContainer[particleIndex].life = 5.0f; // This particle will live 5 seconds.
			ParticlesContainer[particleIndex].pos = glm::vec3(0,0,-20.0f);

			float spread = 1.5f;
			glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
			// Very bad way to generate a random direction; 
			// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
			// combined with some user-controlled parameters (main direction, spread, etc)
			glm::vec3 randomdir = glm::vec3(
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f
			);
			
			ParticlesContainer[particleIndex].speed = maindir + randomdir*spread;


			// Very bad way to generate a random color
			ParticlesContainer[particleIndex].r = rand() % 256;
			ParticlesContainer[particleIndex].g = rand() % 256;
			ParticlesContainer[particleIndex].b = rand() % 256;
			ParticlesContainer[particleIndex].a = (rand() % 256) / 3;

			ParticlesContainer[particleIndex].size = (rand()%1000)/2000.0f + 0.1f;
			
		}

		int ParticlesCount = 0;
		for(int i=0; i<MaxParticles; i++){

			Particle& p = ParticlesContainer[i]; // shortcut

			if(p.life > 0.0f){

				// Decrease life
				p.life -= delta;
				if (p.life > 0.0f){

					// Simulate simple physics : gravity only, no collisions
					p.speed += glm::vec3(0.0f,-9.81f, 0.0f) * (float)delta * 0.5f;
					p.pos += p.speed * (float)delta;
					p.cameradistance = glm::length( p.pos - CameraPosition );
					//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

					// Fill the GPU buffer
					g_particule_position_size_data[4*ParticlesCount+0] = p.pos.x;
					g_particule_position_size_data[4*ParticlesCount+1] = p.pos.y;
					g_particule_position_size_data[4*ParticlesCount+2] = p.pos.z;
												   
					g_particule_position_size_data[4*ParticlesCount+3] = p.size;
												   
					g_particule_color_data[4*ParticlesCount+0] = p.r;
					g_particule_color_data[4*ParticlesCount+1] = p.g;
					g_particule_color_data[4*ParticlesCount+2] = p.b;
					g_particule_color_data[4*ParticlesCount+3] = p.a;

				}else{
					// Particles that just died will be put at the end of the buffer in SortParticles();
					p.cameradistance = -1.0f;
				}

				ParticlesCount++;

			}
		}

        SortParticles();


		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);
        // Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glm::vec3 lightPos = glm::vec3(10,6,10);
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
            1,          // attibute 1, must match the layout in the shader
            3,          // size
            GL_FLOAT,   // type
            GL_FALSE,   // normalizerd?
            0,          // stride
            (void*)0    // array buffer offset
        );

        // 3th attribute buffer : colors
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

        // 4th attribute buffer : colors
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
        glVertexAttribPointer(
            3,          // attibute 2, must match the layout in the shader
            3,          // size
            GL_FLOAT,   // type
            GL_FALSE,   // normalizerd?
            0,          // stride
            (void*)0    // array buffer offset
        );

        // 5th attribute buffer : colors
        glEnableVertexAttribArray(4);
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glVertexAttribPointer(
            4,          // attibute 2, must match the layout in the shader
            3,          // size
            GL_FLOAT,   // type
            GL_FALSE,   // normalizerd?
            0,          // stride
            (void*)0    // array buffer offset
        );

        // 6th attribute buffer : colors
        glEnableVertexAttribArray(5);
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glVertexAttribPointer(
            5,          // attibute 2, must match the layout in the shader
            3,          // size
            GL_UNSIGNED_BYTE,   // type
            GL_TRUE,   // normalizerd?
            0,          // stride
            (void*)0    // array buffer offset
        );

        // Draw the triangle
        // Starting from vertex 0; 3 vertices total -> 1 triangle

        int nTriangles = sizeof(g_color_buffer_data)/(3 * 3 * 4 );
        glDrawArrays(GL_TRIANGLES, 0, 3 * nTriangles);         

		//glVertexAttribDivisor(3, 0); // particles vertices : always reuse the same 4 vertices -> 0
		//glVertexAttribDivisor(4, 1); // positions : one per quad (its center)                 -> 1
        //glVertexAttribDivisor(5, 1); // color : one per quad     
        //glDrawArraysInstanced(GL_TRIANGLE_STRIP, 3, 4, ParticlesCount);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
        glDisableVertexAttribArray(4);

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
