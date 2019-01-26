#include <iostream>
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
        
GLFWwindow* window;
        
#include "common/loadshader.hpp"
#include "common/controls.hpp"

#define PI 3.14159

GLfloat angle = -90;
int n=20;
float posX = 0, posY = 0, posZ = 0; 

int nV = 18;
static GLfloat *g_spherevertex_buffer_data = new GLfloat[nV * (n + 1) * (n + 1)];
static GLfloat *g_spherecolor_buffer_data  = new GLfloat[nV * (n + 1) * (n + 1)];

void Csphere(float radio){
    
	float px, py, pz;
	int i, j;
	float incO = 2 * PI / n;
	float incA = PI / n;
	//glBegin(GL_TRIANGLE_STRIP);
	for (i = 0; i <= n; i++){
		for (j = 0; j <= n; j++) {
			pz = cos(PI - (incA*j))*radio;
			py = sin(PI - (incA*j))*sin(incO*i)*radio;
			px = sin(PI - (incA*j))*cos(incO*i)*radio;
			//printf("%lf%lf%lf\n", px, py, pz);
            //g_spherevertex_buffer_data.push_back(px);
            //g_spherevertex_buffer_data.push_back(py);
            //g_spherevertex_buffer_data.push_back(pz);
            g_spherevertex_buffer_data[(i * (n+1) + j)*nV    ] = px;
            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 1] = py;
            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 2] = pz;
            
			//glVertex3f(px, py, pz);
			pz = cos(PI - (incA*j))*radio;
			py = sin(PI - (incA*j))*sin(incO*(i + 1))*radio;
			px = sin(PI - (incA*j))*cos(incO*(i + 1))*radio;
			//glVertex3f(px, py, pz);
            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 3] = px;
            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 4] = py;
            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 5] = pz;

			pz = cos(PI - (incA*(j + 1)))*radio;
			py = sin(PI - (incA*(j + 1)))*sin(incO*(i + 1))*radio;
			px = sin(PI - (incA*(j + 1)))*cos(incO*(i + 1))*radio;

            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 6] = px;
            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 7] = py;
            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 8] = pz;

			pz = cos(PI - (incA*(j + 1)))*radio;
			py = sin(PI - (incA*(j + 1)))*sin(incO*(i + 1))*radio;
			px = sin(PI - (incA*(j + 1)))*cos(incO*(i + 1))*radio;

            g_spherevertex_buffer_data[(i * (n+1) + j)*nV +  9] = px;
            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 10] = py;
            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 11] = pz;

			pz = cos(PI - (incA*(j + 1)))*radio;
			py = sin(PI - (incA*(j + 1)))*sin(incO*(i))*radio;
			px = sin(PI - (incA*(j + 1)))*cos(incO*(i))*radio;

            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 12] = px;
            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 13] = py;
            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 14] = pz;

			pz = cos(PI - (incA*(j)))*radio;
			py = sin(PI - (incA*(j)))*sin(incO*(i))*radio;
			px = sin(PI - (incA*(j)))*cos(incO*(i))*radio;

            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 15] = px;
            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 16] = py;
            g_spherevertex_buffer_data[(i * (n+1) + j)*nV + 17] = pz;

            for(int k = 0; k < nV/3; k++)
            {
                g_spherecolor_buffer_data[(i * (n+1) + j)*nV + k*3 + 0] = 0.0f;
                g_spherecolor_buffer_data[(i * (n+1) + j)*nV + k*3 + 1] = 0.5f;
                g_spherecolor_buffer_data[(i * (n+1) + j)*nV + k*3 + 2] = 0.45f;
            }
		}
	}
	//glEnd();
}


/*
void display(void){
    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glLoadIdentity();
	glRotatef(angle, 1.0, 0.0, 0.0);
	glScalef(2.0, 2.0, 2.0);

	//Multiply current matrix
	glTranslatef(posX, posY, posZ);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor4f(0.0, 0.0, 0.75, 0.5);
	glLineWidth(2.0);
	esfera(5);
	glPopMatrix();
	glfwSwapBuffers(window);
}
*/
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

int W = 720;
int H = 480;

void init(void)
{
    Csphere(0.05);

    std::cout << sizeof(g_spherevertex_buffer_data) << std::endl;
    std::cout << sizeof(g_vertex_buffer_data) << std::endl;
    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

    // Descartar los triangulos cuya normal no esta hacia la camara
    glDisable(GL_CULL_FACE);
    
	// Enable depth test
	//glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 
}


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




    init();
	
    
    
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
    glBufferData(GL_ARRAY_BUFFER, nV * (n + 1) * (n + 1) * sizeof(float),
                                         g_spherevertex_buffer_data ,
                                         GL_STATIC_DRAW); 

    GLuint spherecolor_buffer;

    glGenBuffers(1, &spherecolor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, spherecolor_buffer);
    glBufferData(GL_ARRAY_BUFFER, nV * (n + 1) * (n + 1) * sizeof(float),
                                         g_spherecolor_buffer_data ,
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

        int nTriangles = sizeof(g_color_buffer_data)/(3 * 3 * 4 );
        glDrawArrays(GL_TRIANGLES, 0, 3 * nTriangles);   

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        glUseProgram(programID);
        // 4th attribute buffer : colors
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

        // 5th attribute buffer : colors
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
     

        glDrawArrays(GL_TRIANGLES, 0, nV * (n + 1) * (n + 1));   
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        // Swap buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
          glfwWindowShouldClose(window)       == 0            );
    

	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteBuffers(1, &normalbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);
    
    glfwTerminate();
    
    return 0;
}
