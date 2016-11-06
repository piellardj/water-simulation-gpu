#include "Water.hpp"

#include "Utilities.hpp"

#include <stdexcept>
#include <string>
#include <iostream>

#include <SFML/Graphics/RectangleShape.hpp>


Water::Water(sf::Vector2u dimensions):
            _currentIndex (0)
{
    for (sf::RenderTexture& renderTexture : _buffers) {
        if (!renderTexture.create(dimensions.x, dimensions.y)) {
            throw std::runtime_error("Water: unable to create position buffer");
        }
        renderTexture.setSmooth(true);
    }

    std::string fragment, utils;
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

sf::Vector2u Water::getBufferSize() const
{
    return _buffers[0].getSize();
}

void Water::update (sf::Time const& time)
{
    unsigned int nextIndex = (_currentIndex + 1) % 2;

    float dt = time.asSeconds() * 10.f;

    sf::RenderStates noBlending(sf::BlendNone);
    sf::Vector2f bufferSize(getBufferSize().x, getBufferSize().y);
    sf::RectangleShape square(bufferSize);

    /* Update velocities */
    noBlending.shader = &_updateShader;
    _updateShader.setParameter("oldBuffer", _buffers[_currentIndex].getTexture());
    _updateShader.setParameter("bufferSize", bufferSize);
    _updateShader.setParameter("dt", dt);
    _buffers[nextIndex].clear();
    _buffers[nextIndex].draw (square, noBlending);
    _buffers[nextIndex].display();

    _currentIndex = nextIndex;
}

void Water::draw (sf::Texture const& background, sf::RenderTarget& target) const
{
    /* Blending disabled, all four components replaced */
    sf::RenderStates noBlending(sf::BlendNone);
    sf::Vector2f targetSize(target.getSize().x, target.getSize().y);
    sf::Vector2f bufferSize(getBufferSize().x, getBufferSize().y);
    sf::RectangleShape square(targetSize);

    /* Positions */
    noBlending.shader = &_displayShader;
    _displayShader.setParameter("buffer", _buffers[_currentIndex].getTexture());
    _displayShader.setParameter("bufferSize", targetSize);
    _displayShader.setParameter("tilesTexture", background);

    target.draw (square, noBlending);
}

void Water::init()
{
    /* Blending disabled, all four components replaced */
    sf::RenderStates noBlending(sf::BlendNone);
    sf::Vector2f bufferSize(getBufferSize().x, getBufferSize().y);
    sf::RectangleShape square(bufferSize);

    /* Positions */
    noBlending.shader = &_initShader;
    for (sf::RenderTexture &texture : _buffers) {
        texture.clear();
        texture.draw (square, noBlending);
        texture.display();
    }
}

void Water::touch(sf::Vector2f mousePos, float radius, float strength)
{
    unsigned int nextIndex = (_currentIndex + 1) % 2;

    /* Blending disabled, all four components replaced */
    sf::RenderStates noBlending(sf::BlendNone);
    sf::Vector2f bufferSize(getBufferSize().x, getBufferSize().y);
    sf::RectangleShape square(bufferSize);

    noBlending.shader = &_touchShader;
    _touchShader.setParameter("oldBuffer", _buffers[_currentIndex].getTexture());
    _touchShader.setParameter("bufferSize", bufferSize);
    _touchShader.setParameter("coords", mousePos);
    _touchShader.setParameter("strength", strength);
    _touchShader.setParameter("radius", radius);
    _buffers[nextIndex].clear();
    _buffers[nextIndex].draw (square, noBlending);
    _buffers[nextIndex].display();

    _currentIndex = nextIndex;
}
