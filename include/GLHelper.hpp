#ifndef GLHELPER_HPP_INCLUDED
#define GLHELPER_HPP_INCLUDED

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Shader.hpp>

#include "glm.hpp"


void gl_CheckError(const char* file, unsigned int line, const char* expression);

#ifdef DEBUG
    #define GLCHECK(expr) do { expr; gl_CheckError(__FILE__, __LINE__, #expr); } while (false)
#else
    #define GLCHECK(expr) (expr)
#endif // DEBUG


GLuint getShaderHandle (sf::Shader const& shader, bool throwExcept=false);

GLuint getShaderUniformLoc (GLuint shaderHandle, std::string const& name, bool throwExcept=false);

GLuint getShaderAttributeLoc (GLuint shaderHandle, std::string const& name, bool throwExcept=false);

/* Only computes the 4 sides of a cube. */
void computeCube (std::vector<glm::vec3>& vertices,
                  std::vector<glm::vec3>& normals);

#endif // GLHELPER_HPP_INCLUDED
