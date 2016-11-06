#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>

#include <GL/glew.h>

#include "Water.hpp"

int main()
{
    /* Creation of the fixed dimensions window and the OpenGL 3+ context */
    sf::ContextSettings openGLContext(0, 0, 0, //no depth, no stencil, no antialiasing
                                      3, 0, //openGL 3.0 requested
                                      sf::ContextSettings::Default);
    sf::RenderWindow window(sf::VideoMode(512, 512), "2D water",
                            sf::Style::Titlebar | sf::Style::Close,
                            openGLContext);
    window.setVerticalSyncEnabled(true);

    /* Checking if the requested OpenGL version is available */
    std::cout << "openGL version: " << window.getSettings().majorVersion << "." << window.getSettings().minorVersion << std::endl << std::endl;
    if (window.getSettings().majorVersion < 3) {
        std::cerr << "This program requires at least OpenGL 3.0" << std::endl << std::endl;
        return EXIT_FAILURE;
    }
    if (!sf::Shader::isAvailable()) {
        std::cerr << "This program requires support for OpenGL shaders." << std::endl << std::endl;
        return EXIT_FAILURE;
    }
    glewInit();

    /* Loading of the image */
    sf::Texture tilesTexture;
    if (!tilesTexture.loadFromFile("rc/tiles.png"))
        throw std::runtime_error("unable to open pic");
    tilesTexture.setSmooth(true);
    tilesTexture.setRepeated(true);

    Water water(window.getSize());

    float total = 0.f;
    int loops = 0;
    sf::Clock fpsCounter, clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                break;
                case sf::Event::KeyReleased:
                    if (event.key.code == sf::Keyboard::R) {
                        water.init();
                    }
                break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left)
                        water.touch(sf::Mouse::getPosition(window));
                break;
                case sf::Event::MouseMoved:
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                        water.touch(sf::Mouse::getPosition(window));
                break;
                default:
                    break;
            }
        }

        sf::Time elapsedTime = clock.getElapsedTime();
        clock.restart();
       // std::cout << elapsedTime.asSeconds() << std::endl;

        window.clear(sf::Color::Green);
        water.update(elapsedTime);
        water.draw(tilesTexture, window);
        window.display();

       // std::cout << 1.f / clock.getElapsedTime().asSeconds() << std::endl;
        ++loops;

    }

    std::cout << "average fps: " << static_cast<float>(loops) / fpsCounter.getElapsedTime().asSeconds() << std::endl;

    return EXIT_SUCCESS;
}
