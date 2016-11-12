#include "GLHelper.hpp"

#include <stdexcept>
#include <string>
#include <iostream>

#include <SFML/OpenGL.hpp>


void gl_CheckError(const char* file, unsigned int line, const char* expression)
{
    GLenum errorCode = glGetError();
    while (errorCode != GL_NO_ERROR) {
        std::string fileString = file;
        std::string error = "Unknown error";
        std::string description  = "No description";

        switch (errorCode) {
            case GL_INVALID_ENUM:
            {
                error = "GL_INVALID_ENUM";
                description = "An unacceptable value has been specified for an enumerated argument.";
                break;
            }

            case GL_INVALID_VALUE:
            {
                error = "GL_INVALID_VALUE";
                description = "A numeric argument is out of range.";
                break;
            }

            case GL_INVALID_OPERATION:
            {
                error = "GL_INVALID_OPERATION";
                description = "The specified operation is not allowed in the current state.";
                break;
            }

            case GL_STACK_OVERFLOW:
            {
                error = "GL_STACK_OVERFLOW";
                description = "This command would cause a stack overflow.";
                break;
            }

            case GL_STACK_UNDERFLOW:
            {
                error = "GL_STACK_UNDERFLOW";
                description = "This command would cause a stack underflow.";
                break;
            }

            case GL_OUT_OF_MEMORY:
            {
                error = "GL_OUT_OF_MEMORY";
                description = "There is not enough memory left to execute the command.";
                break;
            }

//            case GLEXT_GL_INVALID_FRAMEBUFFER_OPERATION:
//            {
//                error = "GL_INVALID_FRAMEBUFFER_OPERATION";
//                description = "The object bound to FRAMEBUFFER_BINDING is not \"framebuffer complete\".";
//                break;
//            }
        }

        // Log the error
        std::cerr << "OpenGL error in "
                  << fileString.substr(fileString.find_last_of("\\/") + 1) << "(" << line << ")."
                  << "\nExpression:\n   " << expression
                  << "\nError description:\n   " << error << "\n   " << description << "\n"
                  << std::endl;
    }
}


GLuint getShaderHandle (sf::Shader const& shader, bool throwExcept)
{
    GLuint shaderID = -1;
    GLCHECK(shaderID = shader.getNativeHandle());
    if (shaderID == (GLuint)(-1) && throwExcept)
        throw std::runtime_error("Error: unable to find shader handle");

    return shaderID;
}

GLuint getShaderUniformLoc (GLuint shaderHandle, std::string const& name, bool throwExcept)
{
    GLuint uniformID = -1;
    GLCHECK(uniformID = glGetUniformLocation(shaderHandle, name.c_str()));
    if (uniformID == (GLuint)(-1) && throwExcept)
        throw std::runtime_error("Error: unable to find uniform " + name);

    return uniformID;
}

GLuint getShaderAttributeLoc (GLuint shaderHandle, std::string const& name, bool throwExcept)
{
    GLuint attributeID = -1;
    GLCHECK(attributeID = glGetAttribLocation(shaderHandle, name.c_str()));
    if (attributeID == (GLuint)(-1) && throwExcept)
        throw std::runtime_error("Error: unable to find attribute " + name);

    return attributeID;
}

void computeCube (std::vector<glm::vec3>& vertices,
                  std::vector<glm::vec3>& normals)
{

    vertices.resize(4*3*2);
    vertices[0] = glm::vec3(0, 0, 0);
    vertices[1] = glm::vec3(1, 0, 1);
    vertices[2] = glm::vec3(0, 0, 1);
    vertices[3] = glm::vec3(0, 0, 0);
    vertices[4] = glm::vec3(1, 0, 0);
    vertices[5] = glm::vec3(1, 0, 1);

    vertices[6+0] = glm::vec3(1, 0, 0);
    vertices[6+1] = glm::vec3(1, 1, 1);
    vertices[6+2] = glm::vec3(1, 0, 1);
    vertices[6+3] = glm::vec3(1, 0, 0);
    vertices[6+4] = glm::vec3(1, 1, 0);
    vertices[6+5] = glm::vec3(1, 1, 1);

    vertices[12+0] = glm::vec3(1, 1, 0);
    vertices[12+1] = glm::vec3(0, 1, 0);
    vertices[12+2] = glm::vec3(0, 1, 1);
    vertices[12+3] = glm::vec3(1, 1, 0);
    vertices[12+4] = glm::vec3(0, 1, 1);
    vertices[12+5] = glm::vec3(1, 1, 1);

    vertices[18+0] = glm::vec3(0, 1, 0);
    vertices[18+1] = glm::vec3(0, 0, 1);
    vertices[18+2] = glm::vec3(0, 1, 1);
    vertices[18+3] = glm::vec3(0, 1, 0);
    vertices[18+4] = glm::vec3(0, 0, 0);
    vertices[18+5] = glm::vec3(0, 0, 1);

    normals.resize(4*3*2);
    for (unsigned int i = 0 ; i < normals.size() ; ++i) {
        int face = i / 6u;
        if (face == 0)
            normals[i] = glm::vec3(0,-1,0);
        else if (face == 1)
            normals[i] = glm::vec3(1,0,0);
        else if (face == 2)
            normals[i] = glm::vec3(0,1,0);
        else
            normals[i] = glm::vec3(-1,0,0);
    }
}
