#include "Renderer3D.hpp"

#include "GLHelper.hpp"
#include "Utilities.hpp"

#include <iostream>


Renderer3D::Renderer3D(unsigned int quality,
                       float amplitude,
                       float waterLevel,
                       float eta,
                       glm::vec4 const& waterColor,
                       float viewDistance,
                       glm::vec3 const& lightDir):
            Renderer(amplitude, waterLevel, eta, waterColor, viewDistance, lightDir),
            _surfaceGridPosBufferID(-1),
            _surfaceGridIndexBufferID(-1),
            _surfaceGridSize(std::max(2u,quality), std::max(2u,quality)),
            _cubeVertBufferID(-1),
            _cubeNormBufferID(-1),
            _camera(glm::vec3(0.5,0.5,0.25))
{
    /* Shaders loading */
    std::string fragment, vertex, utils;
    loadFile("shaders/utils.glsl", utils);

    loadFile("shaders/display3DSurface.frag", fragment);
    loadFile("shaders/display3DSurface.vert", vertex);
    searchAndReplace("__UTILS__", utils, fragment);
    searchAndReplace("__UTILS__", utils, vertex);
    if (!_displaySurfaceShader.loadFromMemory(vertex, fragment)) {
        std::cerr << fragment << std::endl << std::endl;
        throw std::runtime_error("Renderer3D: unable to load 3D surface display shader");
    }

    loadFile("shaders/display3DCube.frag", fragment);
    loadFile("shaders/display3DCube.vert", vertex);
    searchAndReplace("__UTILS__", utils, fragment);
    searchAndReplace("__UTILS__", utils, vertex);
    if (!_displayCubeShader.loadFromMemory(vertex, fragment)) {
        std::cerr << fragment << std::endl << std::endl;
        throw std::runtime_error("Renderer3D: unable to load 3D cube display shader");
    }


    /* Computing grid vertices */
    int nbPtX = _surfaceGridSize.x, nbPtY = _surfaceGridSize.y;
    float stepX = 1.f / static_cast<float>(nbPtX-1), stepY = 1.f / static_cast<float>(nbPtY-1);
    std::vector<glm::vec2> positions(nbPtX * nbPtY);
    for (int iX = 0 ; iX < nbPtX ; ++iX) {
        for (int iY = 0 ; iY < nbPtY ; ++iY) {
            int index = iX*nbPtY + iY;
            positions[index].x = static_cast<float>(iX) * stepX;
            positions[index].y = static_cast<float>(iY) * stepY;
        }
    }

    std::vector<glm::ivec3> indexes(2 * (nbPtX-1) * (nbPtY-1));
    for (int iX = 0 ; iX < nbPtX-1 ; ++iX) {
        for (int iY = 0 ; iY < nbPtY-1 ; ++iY) {
            int index = iX*(nbPtY-1) + iY;
            indexes[2*index+0] = glm::ivec3(iX,iX,iX+1)*nbPtY + glm::ivec3(iY+1, iY, iY);
            indexes[2*index+1] = glm::ivec3(iX,iX+1,iX+1)*nbPtY + glm::ivec3(iY+1, iY, iY+1);
        }
    }

    /* Computing cube vertices */
    std::vector<glm::vec3> cubeVertices, cubeNormals;
    computeCube(cubeVertices, cubeNormals);


    /* Allocation of buffers on GPU */
    GLCHECK(glGenBuffers(1, &_surfaceGridPosBufferID));
    GLCHECK(glGenBuffers(1, &_surfaceGridIndexBufferID));
    GLCHECK(glGenBuffers(1, &_cubeVertBufferID));
    GLCHECK(glGenBuffers(1, &_cubeNormBufferID));

    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _surfaceGridPosBufferID));
    GLCHECK(glBufferData(GL_ARRAY_BUFFER, positions.size()*sizeof(glm::vec2), positions.data(), GL_STATIC_DRAW));
    GLCHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _surfaceGridIndexBufferID));
    GLCHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size()*sizeof(glm::ivec3), indexes.data(), GL_STATIC_DRAW));
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _cubeVertBufferID));
    GLCHECK(glBufferData(GL_ARRAY_BUFFER, cubeVertices.size()*sizeof(glm::vec3), cubeVertices.data(), GL_STATIC_DRAW));
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _cubeNormBufferID));
    GLCHECK(glBufferData(GL_ARRAY_BUFFER, cubeNormals.size()*sizeof(glm::ivec3), cubeNormals.data(), GL_STATIC_DRAW));

    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

Renderer3D::~Renderer3D()
{
    if (_surfaceGridPosBufferID != (GLuint)(-1)) {
        GLCHECK(glDeleteBuffers(1, &_surfaceGridPosBufferID));
    }
    if (_surfaceGridIndexBufferID != (GLuint)(-1)) {
        GLCHECK(glDeleteBuffers(1, &_surfaceGridIndexBufferID));
    }

    if (_cubeVertBufferID != (GLuint)(-1)) {
        GLCHECK(glDeleteBuffers(1, &_cubeVertBufferID));
    }
    if (_cubeNormBufferID != (GLuint)(-1)) {
        GLCHECK(glDeleteBuffers(1, &_cubeNormBufferID));
    }
}

void Renderer3D::draw (sf::Texture const& heightmap,
                           sf::Texture const& groundTexture,
                           sf::Texture const& lightsTexture) const
{
    GLCHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    drawSurface(heightmap, groundTexture, lightsTexture);
    drawCube(heightmap, groundTexture, lightsTexture);
}

Camera& Renderer3D::getCamera ()
{
    return _camera;
}

void Renderer3D::drawSurface (sf::Texture const& heightmap,
                                  sf::Texture const& groundTexture,
                                  sf::Texture const& lightsTexture) const
{
    glm::vec3 eyePos = _camera.getPosition();
    glm::mat4 MVP = _camera.getMatrix();

    _displaySurfaceShader.setParameter("heightmap", heightmap);
    _displaySurfaceShader.setParameter("groundTexture", groundTexture);
    _displaySurfaceShader.setParameter("lightsTexture", lightsTexture);
    sf::Shader::bind(&_displaySurfaceShader);

    /* First retrieve locations */
    GLuint shaderHandle = -1;
    GLuint coordsALoc = -1;
    GLuint MVPULoc = -1, eyeULoc = -1, cellSizeULoc = -1, amplitudeULoc = -1, levelULoc = -1, etaULoc = -1, waterColorULoc = -1, viewDistanceULoc = -1, lightDirULoc = -1;
//    try {
        shaderHandle = getShaderHandle(_displaySurfaceShader);

        MVPULoc = getShaderUniformLoc(shaderHandle, "MVP");
        eyeULoc = getShaderUniformLoc(shaderHandle, "eyeWorldPos");
        cellSizeULoc = getShaderUniformLoc(shaderHandle, "cellSize");
        amplitudeULoc = getShaderUniformLoc(shaderHandle, "amplitude");
        levelULoc = getShaderUniformLoc(shaderHandle, "waterLevel");
        etaULoc = getShaderUniformLoc(shaderHandle, "eta");
        waterColorULoc = getShaderUniformLoc(shaderHandle, "waterColor");
        viewDistanceULoc = getShaderUniformLoc(shaderHandle, "viewDistance");
        lightDirULoc = getShaderUniformLoc(shaderHandle, "normalizedLightDir");

        coordsALoc = getShaderAttributeLoc(shaderHandle, "coordsOnHeightmap");
//    } catch (std::exception const& e) {
//        std::cerr << "Renderer3D.drawSurface: " << e.what() << std::endl;
//        return;
//    }

    glm::vec2 cellSize(1.f / static_cast<float>(heightmap.getSize().x),
                       1.f / static_cast<float>(heightmap.getSize().y));
    glm::vec4 color = getWaterColor();

    GLCHECK(glUniformMatrix4fv(MVPULoc, 1, GL_FALSE, &MVP[0][0]));
    GLCHECK(glUniform3f(eyeULoc, eyePos.x, eyePos.y, eyePos.z));
    GLCHECK(glUniform2f(cellSizeULoc, cellSize.x, cellSize.y));
    GLCHECK(glUniform1f(amplitudeULoc, getAmplitude()));
    GLCHECK(glUniform1f(levelULoc, getWaterLevel()));
    GLCHECK(glUniform1f(etaULoc, getEta()));
    GLCHECK(glUniform4f(waterColorULoc, color.r, color.g, color.b, color.a));
    GLCHECK(glUniform1f(viewDistanceULoc, getViewDistance()));
    GLCHECK(glUniform3f(lightDirULoc, getLightDirection().x, getLightDirection().y, getLightDirection().z));

    /* Enabling corners coordinates buffer */
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _surfaceGridPosBufferID));
    GLCHECK(glEnableVertexAttribArray(coordsALoc));
    GLCHECK(glVertexAttribPointer(coordsALoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));

    /*Actual drawing */
    GLCHECK(glEnable(GL_CULL_FACE));
    GLCHECK(glEnable(GL_DEPTH_TEST));
    GLCHECK(glDisable(GL_BLEND));
    GLCHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _surfaceGridIndexBufferID));
    GLCHECK(glDrawElements(GL_TRIANGLES, 6*(_surfaceGridSize.x-1)*(_surfaceGridSize.y-1), GL_UNSIGNED_INT, (void*)0));

    /* Don't forget to unbind buffers */
    GLCHECK(glDisableVertexAttribArray(coordsALoc));
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    sf::Shader::bind(0);
}

void Renderer3D::drawCube (sf::Texture const& heightmap,
                               sf::Texture const& groundTexture,
                               sf::Texture const& lightsTexture) const
{
    glm::vec3 eyePos = _camera.getPosition();
    glm::mat4 MVP = _camera.getMatrix();

    _displayCubeShader.setParameter("heightmap", heightmap);
    _displayCubeShader.setParameter("groundTexture", groundTexture);
    _displayCubeShader.setParameter("lightsTexture", lightsTexture);
    sf::Shader::bind(&_displayCubeShader);

    /* First retrieve locations */
    GLuint shaderHandle = -1;
    GLuint vertexALoc = -1, normalALoc = -1;
    GLuint MVPULoc = -1, eyeULoc = -1, amplitudeULoc = -1, levelULoc = -1, etaULoc = -1, waterColorULoc = -1, viewDistanceULoc = -1, lightDirULoc = -1;
//    try {
        shaderHandle = getShaderHandle(_displayCubeShader, false);

        MVPULoc = getShaderUniformLoc(shaderHandle, "MVP", false);
        eyeULoc = getShaderUniformLoc(shaderHandle, "eyeWorldPos", false);
        amplitudeULoc = getShaderUniformLoc(shaderHandle, "amplitude", false);
        levelULoc = getShaderUniformLoc(shaderHandle, "waterLevel", false);
        etaULoc = getShaderUniformLoc(shaderHandle, "eta", false);
        waterColorULoc = getShaderUniformLoc(shaderHandle, "waterColor", false);
        viewDistanceULoc = getShaderUniformLoc(shaderHandle, "viewDistance", false);
        lightDirULoc = getShaderUniformLoc(shaderHandle, "normalizedLightDir", false);

        vertexALoc = getShaderAttributeLoc(shaderHandle, "vertex", false);
        normalALoc = getShaderAttributeLoc(shaderHandle, "normal", false);
//    } catch (std::exception const& e) {
//        std::cerr << "Renderer3D.drawCube: " << e.what() << std::endl;
//        return;
//    }

    glm::vec2 cellSize(1.f / static_cast<float>(heightmap.getSize().x),
                       1.f / static_cast<float>(heightmap.getSize().y));
    glm::vec4 color = getWaterColor();

    GLCHECK(glUniformMatrix4fv(MVPULoc, 1, GL_FALSE, &MVP[0][0]));
    GLCHECK(glUniform3f(eyeULoc, eyePos.x, eyePos.y, eyePos.z));
    GLCHECK(glUniform1f(amplitudeULoc, getAmplitude()));
    GLCHECK(glUniform1f(levelULoc, getWaterLevel()));
    GLCHECK(glUniform1f(etaULoc, getEta()));
    GLCHECK(glUniform4f(waterColorULoc, color.r, color.g, color.b, color.a));
    GLCHECK(glUniform1f(viewDistanceULoc, getViewDistance()));
    GLCHECK(glUniform3f(lightDirULoc, getLightDirection().x, getLightDirection().y, getLightDirection().z));

    /* Enabling coordinates buffer */
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _cubeVertBufferID));
    GLCHECK(glEnableVertexAttribArray(vertexALoc));
    GLCHECK(glVertexAttribPointer(vertexALoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));

    /* Enabling normals buffer */
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _cubeNormBufferID));
    GLCHECK(glEnableVertexAttribArray(normalALoc));
    GLCHECK(glVertexAttribPointer(normalALoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));

//    /*Actual drawing */
    GLCHECK(glEnable(GL_CULL_FACE));
    GLCHECK(glEnable(GL_DEPTH_TEST));
    GLCHECK(glDisable(GL_BLEND));
    GLCHECK(glDrawArrays(GL_TRIANGLES, 0, 4*3*2));

    /* Don't forget to unbind buffers */
    GLCHECK(glDisableVertexAttribArray(vertexALoc));
    GLCHECK(glDisableVertexAttribArray(normalALoc));
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    sf::Shader::bind(0);
}
