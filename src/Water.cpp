#include "Water.hpp"

#include "Utilities.hpp"
#include "GLHelper.hpp"

#include <stdexcept>
#include <string>
#include <iostream>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>


Water::Water(sf::Vector2u dimensions, float propagation, float friction, float elasticity):
            _friction (friction),
            _propagation (propagation),
            _elasticity (elasticity),
            _currentIndex (0)
{
    /* Textures allocation */
    for (sf::RenderTexture& renderTexture : _buffers) {
        if (!renderTexture.create(dimensions.x, dimensions.y)) {
            throw std::runtime_error("Water: unable to create position buffer");
        }
        renderTexture.setSmooth(true);
    }

    if (!_heightmap.create(256,256)) {
            throw std::runtime_error("Water: unable to create position buffer");
    }
    _heightmap.setSmooth(true);

    /* Shaders loading */
    std::string fragment, vertex, utils;
    loadFile("shaders/utils.glsl", utils);

    loadFile("shaders/init.frag", fragment);
    searchAndReplace("__UTILS__", utils, fragment);
    if (!_initShader.loadFromMemory(fragment, sf::Shader::Fragment)) {
        std::cerr << fragment << std::endl << std::endl;
        throw std::runtime_error("Water: unable to load init shader");
    }

    loadFile("shaders/update.frag", fragment);
    searchAndReplace("__UTILS__", utils, fragment);
    if (!_updateShader.loadFromMemory(fragment, sf::Shader::Fragment)) {
        std::cerr << fragment << std::endl << std::endl;
        throw std::runtime_error("Water: unable to load update shader");
    }

    loadFile("shaders/touch.frag", fragment);
    searchAndReplace("__UTILS__", utils, fragment);
    if (!_touchShader.loadFromMemory(fragment, sf::Shader::Fragment)) {
        std::cerr << fragment << std::endl << std::endl;
        throw std::runtime_error("Water: unable to load touch shader");
    }

    loadFile("shaders/generateHeightmap.frag", fragment);
    searchAndReplace("__UTILS__", utils, fragment);
    if (!_generateHeightmapShader.loadFromMemory(fragment, sf::Shader::Fragment)) {
        std::cerr << fragment << std::endl << std::endl;
        throw std::runtime_error("Water: unable to load heightmap generation shader");
    }

    init();
}

Water::~Water ()
{
}

sf::Vector2u Water::getGridSize() const
{
    return _buffers[0].getSize();
}

void Water::generateHeightmap()
{
    sf::RenderStates noBlending(sf::BlendNone);
    sf::Vector2f heightmapSize(_heightmap.getSize().x, _heightmap.getSize().y);
    sf::Vector2f cellSize(1.f / heightmapSize.x, 1.f / heightmapSize.y);

    sf::RectangleShape square(heightmapSize);

    noBlending.shader = &_generateHeightmapShader;
    _generateHeightmapShader.setParameter("grid", _buffers[_currentIndex].getTexture());
    _generateHeightmapShader.setParameter("cellSize", cellSize);
    _heightmap.clear();
    _heightmap.draw (square, noBlending);
    _heightmap.display();
}

sf::Texture const& Water::getHeightmap () const
{
    return _heightmap.getTexture();
}

void Water::update (float time)
{
    unsigned int nextIndex = (_currentIndex + 1) % 2;

    float dt = time * 10.f;

    sf::RenderStates noBlending(sf::BlendNone);
    sf::Vector2f bufferSize(getGridSize().x, getGridSize().y);
    sf::Vector2f cellSize(1.f / bufferSize.x, 1.f / bufferSize.y);
    sf::RectangleShape square(bufferSize);

    /* Update velocities */
    noBlending.shader = &_updateShader;
    _updateShader.setParameter("oldGrid", _buffers[_currentIndex].getTexture());
    _updateShader.setParameter("cellSize", cellSize);
    _updateShader.setParameter("dt", dt);
    _updateShader.setParameter("c", _propagation);
    _updateShader.setParameter("k", _elasticity);
    _updateShader.setParameter("f", _friction);
    _buffers[nextIndex].clear();
    _buffers[nextIndex].draw (square, noBlending);
    _buffers[nextIndex].display();

    _currentIndex = nextIndex;
}

void Water::init()
{
    /* Blending disabled, all four components replaced */
    sf::RenderStates noBlending(sf::BlendNone);
    sf::Vector2f bufferSize(getGridSize().x, getGridSize().y);
    sf::RectangleShape square(bufferSize);

    /* Positions */
    noBlending.shader = &_initShader;
    for (sf::RenderTexture &texture : _buffers) {
        texture.clear();
        texture.draw (square, noBlending);
        texture.display();
    }
}

void Water::touch(sf::Vector2f mousePos, float radius, float extremum)
{
    unsigned int nextIndex = (_currentIndex + 1) % 2;

    /* Blending disabled, all four components replaced */
    sf::RenderStates noBlending(sf::BlendNone);
    sf::Vector2f gridSize(getGridSize().x, getGridSize().y);
    sf::Vector2f cellSize(1.f / gridSize.x, 1.f / gridSize.y);

    sf::RectangleShape square(gridSize);

    noBlending.shader = &_touchShader;
    _touchShader.setParameter("oldGrid", _buffers[_currentIndex].getTexture());
    _touchShader.setParameter("cellSize", cellSize);
    _touchShader.setParameter("extremum", extremum);
    _touchShader.setParameter("radius", radius);
    _touchShader.setParameter("coords", mousePos);
    _buffers[nextIndex].clear();
    _buffers[nextIndex].draw (square, noBlending);
    _buffers[nextIndex].display();

    _currentIndex = nextIndex;
}
