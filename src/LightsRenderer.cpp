#include "LightsRenderer.hpp"

#include "GLHelper.hpp"
#include "Utilities.hpp"

#include <iostream>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>


LightsRenderer::LightsRenderer (unsigned int quality,
                                float amplitude,
                                float waterLevel,
                                float eta,
                                glm::vec3 const& lightDir):
            Renderer::Renderer(amplitude, waterLevel, eta, glm::vec4(1), 2.f, lightDir),
            _partPerPixel(2),
            _intensity(0.2f / static_cast<float>(_partPerPixel*_partPerPixel)),
            _particlesGridBufferID(-1),
            _particlesGridSize(_partPerPixel*quality, _partPerPixel*quality),
            _particleSize(1.f)
{
    /* Texture allocation */
    if (!_rawLights.create(quality, quality)) {
            throw std::runtime_error("LightsRenderer: unable to create buffer");
    }
    _rawLights.setSmooth(true);

    if (!_processedLights.create(_rawLights.getSize().x, _rawLights.getSize().y)) {
            throw std::runtime_error("LightsRenderer: unable to create buffer");
    }
    _processedLights.setSmooth(true);
    _processedLights.setRepeated(true);

    /* Shaders loading */
    std::string fragment, vertex, utils;
    loadFile("shaders/utils.glsl", utils);

    loadFile("shaders/computeLights.frag", fragment);
    loadFile("shaders/computeLights.vert", vertex);
    searchAndReplace("__UTILS__", utils, fragment);
    searchAndReplace("__UTILS__", utils, vertex);
    if (!_computeLightsShader.loadFromMemory(vertex, fragment)) {
        std::cerr << fragment << std::endl << std::endl;
        throw std::runtime_error("LightsRenderer: unable to load compute lights shader");
    }

    loadFile("shaders/processLights.frag", fragment);
    if (!_processLightsShader.loadFromMemory(fragment, sf::Shader::Fragment)) {
        std::cerr << fragment << std::endl << std::endl;
        throw std::runtime_error("LightsRenderer: unable to load adjust light ground shader");
    }


    /* Allocation of the buffer storing the particles initial position */
    std::vector<glm::vec2> lightParticles(_particlesGridSize.x*_particlesGridSize.y);
    for (unsigned int iX = 0 ; iX < _particlesGridSize.x ; ++iX) {
        for (unsigned int iY = 0 ; iY < _particlesGridSize.y ; ++iY)  {
            unsigned int index = iX*_particlesGridSize.y + iY;
            lightParticles[index] = glm::vec2(static_cast<float>(iX) / static_cast<float>(_particlesGridSize.x),
                                              static_cast<float>(iY) / static_cast<float>(_particlesGridSize.y));
        }
    }

    GLCHECK(glGenBuffers(1, &_particlesGridBufferID));

    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _particlesGridBufferID));
    GLCHECK(glBufferData(GL_ARRAY_BUFFER, lightParticles.size()*sizeof(glm::vec2), lightParticles.data(), GL_STATIC_DRAW));

    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

LightsRenderer::~LightsRenderer()
{
    if (_particlesGridBufferID != (GLuint)(-1)) {
        GLCHECK(glDeleteBuffers(1, &_particlesGridBufferID));
    }
}

void LightsRenderer::reset()
{
    _rawLights.clear(sf::Color::Black);
    _processedLights.clear(sf::Color::Black);
}

sf::Texture const& LightsRenderer::getTexture() const
{
    return _processedLights.getTexture();
}

void LightsRenderer::update (sf::Texture const& heightmap)
{
    computeRawLights(heightmap);
    computeProcessedLights();
}

void LightsRenderer::computeRawLights(sf::Texture const& heightmap)
{
    _rawLights.setActive(true);
    GLCHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    GLCHECK(glViewport(0, 0, _rawLights.getSize().x, _rawLights.getSize().y));

    sf::Vector2f cellSizeOnBuffer(1.f / static_cast<float>(heightmap.getSize().x),
                                  1.f / static_cast<float>(heightmap.getSize().x));

    _computeLightsShader.setParameter("heightmap", heightmap);
    sf::Shader::bind(&_computeLightsShader);

    /* First retrieve locations */
    GLuint shaderHandle = -1;
    GLuint posALoc = -1;
    GLuint cellSizeULoc = -1, intensityULoc = -1, amplitudeULoc = -1, waterLevelULoc = -1, etaULoc = -1, lightDirULoc = -1;
//    try {
        shaderHandle = getShaderHandle(_computeLightsShader, false);

        cellSizeULoc = getShaderUniformLoc(shaderHandle, "cellSize", false);
        intensityULoc = getShaderUniformLoc(shaderHandle, "intensity", false);
        amplitudeULoc = getShaderUniformLoc(shaderHandle, "amplitude", false);
        waterLevelULoc = getShaderUniformLoc(shaderHandle, "waterLevel", false);
        etaULoc = getShaderUniformLoc(shaderHandle, "eta", false);
        lightDirULoc = getShaderUniformLoc(shaderHandle, "normalizedLightDir", false);

        posALoc = getShaderAttributeLoc(shaderHandle, "pos", false);
//    } catch (std::exception const& e) {
//        std::cerr << "LightsRenderer.udpate: " << e.what() << std::endl;
//        return;
//    }

    glm::vec2 cellSize(1.f / static_cast<float>(heightmap.getSize().x),
                       1.f / static_cast<float>(heightmap.getSize().y));
    glm::vec3 lightDir = getLightDirection();

    GLCHECK(glUniform2f(cellSizeULoc, cellSize.x, cellSize.y));
    GLCHECK(glUniform1f(intensityULoc, _intensity));
    GLCHECK(glUniform1f(amplitudeULoc, getAmplitude()));
    GLCHECK(glUniform1f(waterLevelULoc, getWaterLevel()));
    GLCHECK(glUniform1f(etaULoc, getEta()));
    GLCHECK(glUniform3f(lightDirULoc, lightDir.x, lightDir.y, lightDir.z));

    /* Enabling coordinates buffer */
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _particlesGridBufferID));
    GLCHECK(glEnableVertexAttribArray(posALoc));
    GLCHECK(glVertexAttribPointer(posALoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));

    /*Actual drawing */
    GLCHECK(glPointSize(_particleSize));
    GLCHECK(glDisable(GL_CULL_FACE));
    GLCHECK(glDisable(GL_DEPTH_TEST));
    GLCHECK(glEnable(GL_BLEND));
    GLCHECK(glBlendFunc(GL_ONE, GL_ONE));
    GLCHECK(glDrawArrays(GL_POINTS, 0, _particlesGridSize.x*_particlesGridSize.y));

    /* Don't forget to unbind buffers */
    GLCHECK(glDisableVertexAttribArray(posALoc));
    GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    sf::Shader::bind(0);

    _rawLights.display();
}

void LightsRenderer::computeProcessedLights()
{
    sf::Vector2f texSize = sf::Vector2f(_processedLights.getSize().x, _processedLights.getSize().y);
    sf::RenderStates renderStates (sf::BlendNone);
    renderStates.shader = &_processLightsShader;
    _processLightsShader.setParameter("rawLightsTexture", _rawLights.getTexture());
    _processLightsShader.setParameter("textureSize", texSize);
    _processLightsShader.setParameter("shift", 0.3f);
    _processLightsShader.setParameter("stretch", 2.f);

    sf::RectangleShape square(texSize);

    _processedLights.clear();
    _processedLights.draw(square, renderStates);
    _processedLights.display();
}
