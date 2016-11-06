#include "Water.hpp"

#include "Utilities.hpp"

#include <stdexcept>
#include <string>
#include <iostream>

#include <SFML/Graphics/RectangleShape.hpp>


Water::Water(sf::Vector2u dimensions):
            _bufferSize(dimensions.x, dimensions.y),
            _currentIndex (0)
{
    for (sf::RenderTexture& renderTexture : _positions) {
        if (!renderTexture.create(dimensions.x, dimensions.y)) {
            throw std::runtime_error("Water: unable to create position buffer");
        }
    }
    for (sf::RenderTexture& renderTexture : _velocities) {
        if (!renderTexture.create(dimensions.x, dimensions.y)) {
            throw std::runtime_error("Water: unable to create position buffer");
        }
    }

    std::string fragment, utils;
    loadFile("shaders/utils.glsl", utils);

    loadFile("shaders/initPosition.frag", fragment);
    searchAndReplace("__UTILS__", utils, fragment);
    if (!_initPositionShader.loadFromMemory(fragment, sf::Shader::Fragment)) {
        std::cerr << fragment << std::endl << std::endl;
        throw std::runtime_error("Water: unable to load init position shader");
    }

    loadFile("shaders/initVelocity.frag", fragment);
    searchAndReplace("__UTILS__", utils, fragment);
    if (!_initVelocityShader.loadFromMemory(fragment, sf::Shader::Fragment)) {
        std::cerr << fragment << std::endl << std::endl;
        throw std::runtime_error("Water: unable to load init velocity shader");
    }

    loadFile("shaders/updatePosition.frag", fragment);
    searchAndReplace("__UTILS__", utils, fragment);
    if (!_updatePositionShader.loadFromMemory(fragment, sf::Shader::Fragment)) {
        std::cerr << fragment << std::endl << std::endl;
        throw std::runtime_error("Water: unable to load update position shader");
    }

    loadFile("shaders/updateVelocity.frag", fragment);
    searchAndReplace("__UTILS__", utils, fragment);
    if (!_updateVelocityShader.loadFromMemory(fragment, sf::Shader::Fragment)) {
        std::cerr << fragment << std::endl << std::endl;
        throw std::runtime_error("Water: unable to load update velocity shader");
    }

    loadFile("shaders/display.frag", fragment);
    searchAndReplace("__UTILS__", utils, fragment);
    if (!_displayShader.loadFromMemory(fragment, sf::Shader::Fragment)) {
        std::cerr << fragment << std::endl << std::endl;
        throw std::runtime_error("Water: unable to load display shader");
    }

    loadFile("shaders/touch.frag", fragment);
    searchAndReplace("__UTILS__", utils, fragment);
    if (!_touchShader.loadFromMemory(fragment, sf::Shader::Fragment)) {
        std::cerr << fragment << std::endl << std::endl;
        throw std::runtime_error("Water: unable to load touch shader");
    }

    init();
}

void Water::update (sf::Time const& time)
{
    unsigned int nextIndex = (_currentIndex + 1) % 2;

    float dt = time.asSeconds() * 10.f;

    sf::RenderStates noBlending(sf::BlendNone);
    sf::RectangleShape square(_bufferSize);

    /* Update velocities */
    noBlending.shader = &_updateVelocityShader;
    _updateVelocityShader.setParameter("oldVelocities", _velocities[_currentIndex].getTexture());
    _updateVelocityShader.setParameter("positions", _positions[_currentIndex].getTexture());
    _updateVelocityShader.setParameter("bufferSize", _bufferSize);
    _updateVelocityShader.setParameter("dt", dt);
    _velocities[nextIndex].clear();
    _velocities[nextIndex].draw (square, noBlending);
    _velocities[nextIndex].display();

    /* Update positions */
    noBlending.shader = &_updatePositionShader;
    _updatePositionShader.setParameter("velocities", _velocities[nextIndex].getTexture());
    _updatePositionShader.setParameter("oldPositions", _positions[_currentIndex].getTexture());
    _updatePositionShader.setParameter("bufferSize", _bufferSize);
    _updatePositionShader.setParameter("dt", dt);
    _positions[nextIndex].clear();
    _positions[nextIndex].draw (square, noBlending);
    _positions[nextIndex].display();

    _currentIndex = nextIndex;
}

void Water::draw (sf::Texture const& background, sf::RenderTarget& target)
{
    /* Blending disabled, all four components replaced */
    sf::RenderStates noBlending(sf::BlendNone);
    sf::RectangleShape square(_bufferSize);

    /* Positions */
    noBlending.shader = &_displayShader;
    _displayShader.setParameter("positions", _positions[_currentIndex].getTexture());
    _displayShader.setParameter("bufferSize", _bufferSize);
    _displayShader.setParameter("tilesTexture", background);

    target.draw (square, noBlending);
}

void Water::init()
{
    /* Blending disabled, all four components replaced */
    sf::RenderStates noBlending(sf::BlendNone);
    sf::RectangleShape square(_bufferSize);

    /* Positions */
    noBlending.shader = &_initPositionShader;
    for (sf::RenderTexture &texture : _positions) {
        texture.clear();
        texture.draw (square, noBlending);
        texture.display();
    }

    /* Velocities */
    noBlending.shader = &_initVelocityShader;
    for (sf::RenderTexture &texture : _velocities) {
        texture.clear();
        texture.draw (square, noBlending);
        texture.display();
    }
}

void Water::touch(sf::Vector2i mousePos)
{
    unsigned int nextIndex = (_currentIndex + 1) % 2;

    /* Blending disabled, all four components replaced */
    sf::RenderStates noBlending(sf::BlendNone);
    sf::RectangleShape square(_bufferSize);

    noBlending.shader = &_touchShader;
    _touchShader.setParameter("oldPositions", _positions[_currentIndex].getTexture());
    _touchShader.setParameter("bufferSize", _bufferSize);
    _touchShader.setParameter("coords", sf::Vector2f(mousePos.x, _bufferSize.y-mousePos.y));
    _touchShader.setParameter("strength", 0.3f);
    _touchShader.setParameter("radius", 20.f);
    _positions[nextIndex].clear();
    _positions[nextIndex].draw (square, noBlending);
    _positions[nextIndex].display();

    _currentIndex = nextIndex;
}
