#include "Renderer2D.hpp"

#include "GLHelper.hpp"
#include "Utilities.hpp"

#include <iostream>


Renderer2D::Renderer2D(float amplitude,
                       float waterLevel,
                       float eta,
                       glm::vec4 const& waterColor,
                       float viewDistance,
                       glm::vec3 const& lightDir):
            Renderer(amplitude, waterLevel, eta, waterColor, viewDistance, lightDir),
            _cornersBufferID(-1)
{
    /* Shaders loading */
    std::string fragment, vertex, utils;
    loadFile("shaders/utils.glsl", utils);

    loadFile("shaders/display2D.frag", fragment);
    loadFile("shaders/display2D.vert", vertex);
    searchAndReplace("__UTILS__", utils, fragment);
    if (!_displayShader.loadFromMemory(vertex, fragment)) {
        std::cerr << fragment << std::endl << std::endl;
        throw std::runtime_error("Renderer2D: unable to load shader");
    }

    /* Buffer allocation */
    std::vector<glm::vec2> corners(6);
    corners[0] = glm::vec2(-1,-1);
    corners[1] = glm::vec2(+1,+1);
    corners[2] = glm::vec2(-1,+1);
    corners[3] = glm::vec2(-1,-1);
    corners[4] = glm::vec2(+1,+1);
    corners[5] = glm::vec2(+1,-1);

    GLCHECK(glGenBuffers(1, &_cornersBufferID));
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _cornersBufferID));
    GLCHECK(glBufferData(GL_ARRAY_BUFFER, corners.size()*sizeof(glm::vec2), corners.data(), GL_STATIC_DRAW));

    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

Renderer2D::~Renderer2D()
{
    if (_cornersBufferID != (GLuint)(-1)) {
        GLCHECK(glDeleteBuffers(1, &_cornersBufferID));
    }
}

void Renderer2D::draw (sf::Texture const& heightmap,
                           sf::Texture const& groundTexture) const
{
    GLCHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    _displayShader.setParameter("heightmap", heightmap);
    _displayShader.setParameter("groundTexture", groundTexture);
    sf::Shader::bind(&_displayShader);

    /* First retrieve locations */
    GLuint shaderHandle = -1;
    GLuint cornerALoc = -1;
    GLuint cellSizeULoc = -1, amplitudeULoc = -1, levelULoc = -1, etaULoc = -1, waterColorULoc = -1, viewDistanceULoc = -1, lightDirULoc = -1;
//    try {
        shaderHandle = getShaderHandle(_displayShader, false);

        cellSizeULoc = getShaderUniformLoc(shaderHandle, "cellSize", false);
        amplitudeULoc = getShaderUniformLoc(shaderHandle, "amplitude", false);
        levelULoc = getShaderUniformLoc(shaderHandle, "waterLevel", false);
        etaULoc = getShaderUniformLoc(shaderHandle, "eta", false);
        waterColorULoc = getShaderUniformLoc(shaderHandle, "waterColor", false);
        viewDistanceULoc = getShaderUniformLoc(shaderHandle, "viewDistance", false);
        lightDirULoc = getShaderUniformLoc(shaderHandle, "normalizedLightDir", false);

        cornerALoc = getShaderAttributeLoc(shaderHandle, "corner", false);
//    } catch (std::exception const& e) {
//        std::cerr << "Renderer2D.draw: " << e.what() << std::endl;
//        return;
//    }

    glm::vec2 cellSize(1.f / static_cast<float>(heightmap.getSize().x),
                       1.f / static_cast<float>(heightmap.getSize().y));
    glm::vec4 color = getWaterColor();

    GLCHECK(glUniform2f(cellSizeULoc, cellSize.x, cellSize.y));
    GLCHECK(glUniform1f(amplitudeULoc, getAmplitude()));
    GLCHECK(glUniform1f(levelULoc, getWaterLevel()));
    GLCHECK(glUniform1f(etaULoc, getEta()));
    GLCHECK(glUniform4f(waterColorULoc, color.r, color.g, color.b, color.a));
    GLCHECK(glUniform1f(viewDistanceULoc, getViewDistance()));
    GLCHECK(glUniform3f(lightDirULoc, getLightDirection().x, getLightDirection().y, getLightDirection().z));

    /* Enabling corners coordinates buffer */
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _cornersBufferID));
    GLCHECK(glEnableVertexAttribArray(cornerALoc));
    GLCHECK(glVertexAttribPointer(cornerALoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));

    /*Actual drawing */
    GLCHECK(glDisable(GL_CULL_FACE));
    GLCHECK(glDisable(GL_DEPTH_TEST));
    GLCHECK(glDisable(GL_BLEND));
    GLCHECK(glDrawArrays(GL_TRIANGLES, 0, 6));

    /* Don't forget to unbind buffers */
    GLCHECK(glDisableVertexAttribArray(cornerALoc));
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    sf::Shader::bind(0);
}
