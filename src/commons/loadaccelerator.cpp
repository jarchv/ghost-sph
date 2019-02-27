#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include "loadaccelerator.hpp"

GLuint LoadComputeShader(const char* compute_file_path)
{
    // Create the shaders

    GLuint ComputeShaderID  = glCreateShader(GL_COMPUTE_SHADER);
    
    // Read the Compute Shader code from the file
    
    std::string ComputeShaderCode;
    std::ifstream ComputeShaderStream(compute_file_path, std::ios::in);
    
    if (ComputeShaderStream.is_open())
    {
        std::stringstream sstr;
        sstr << ComputeShaderStream.rdbuf();
        ComputeShaderCode = sstr.str();
        ComputeShaderStream.close();
    } else
    {
        printf("Imposible to open %s\n", compute_file_path);
        getchar();
        return 0;
    }

    GLint result = GL_FALSE;
    int InfoLogLength;

    // Compile Compute Shader
    printf("Compiling shader : %s\n", compute_file_path);
    char const* ComputeSourcePointer  = ComputeShaderCode.c_str();
    glShaderSource(ComputeShaderID, 1, &ComputeSourcePointer, NULL);
    glCompileShader(ComputeShaderID);

    // Check Compute Shader
    glGetShaderiv(ComputeShaderID, GL_COMPILE_STATUS, &result);
    

    if (!result)
    {
        glGetShaderiv(ComputeShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::vector<char> ComputeShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(ComputeShaderID, InfoLogLength, NULL, &ComputeShaderErrorMessage[0]);
        printf("Error: Compiler log:\n%s\n", &ComputeShaderErrorMessage[0]);

        return 0;
    }
    


    // Linking the program

    printf("\nLinking program...\n\n");
    GLuint ProgramID = glCreateProgram();

    glAttachShader(ProgramID, ComputeShaderID);    
    glLinkProgram(ProgramID);


    // Check the program

    glGetProgramiv(ProgramID, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::vector<char> ProgramErrorMessage(InfoLogLength);
        glGetProgramInfoLog(ProgramID, InfoLogLength, &InfoLogLength, &ProgramErrorMessage[0]);
        std::cerr<< &ProgramErrorMessage[0] << std::endl;
    }

    glDetachShader(ProgramID, ComputeShaderID);
    glDeleteShader(ComputeShaderID);

    return ProgramID;
}
