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
#include "Renderer2D.hpp"


/* Returns relative mouse position in the window (in [0,1]x[0,1]) */
sf::Vector2f getRelativeMousePos (sf::Window const& window)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f pos(mousePos.x, window.getSize().y - mousePos.y);
    pos.x /= window.getSize().x;
    pos.y /= window.getSize().y;
    return pos;
}

int main()
{
    /* Creation of the windows and contexts */
    sf::ContextSettings openGL2DContext(0, 0, 0, //no depth, no stencil, no antialiasing
                                       3, 0, //openGL 3.0 requested
                                       sf::ContextSettings::Default);
    sf::RenderWindow window2D(sf::VideoMode(512,512), "2D water",
                              sf::Style::Titlebar | sf::Style::Close,
                              openGL2DContext);
    window2D.setVerticalSyncEnabled(true);


    /* Checking if the requested OpenGL version is available */
    std::cout << "openGL version: " << window2D.getSettings().majorVersion << "." << window2D.getSettings().minorVersion << std::endl << std::endl;
    if (window2D.getSettings().majorVersion < 3) {
        std::cerr << "This program requires at least OpenGL 3.0" << std::endl << std::endl;
        return EXIT_FAILURE;
    }
    if (!sf::Shader::isAvailable()) {
        std::cerr << "This program requires support for OpenGL shaders." << std::endl << std::endl;
        return EXIT_FAILURE;
    }
    glewInit();


    /* Creation of the simulation */
    float touchRadius = 0.03f;
    float touchExtremum = -0.9f;
    float friction = 0.99f;
    float propagation = 20.f;
    float elasticity = 0.7f;// 0.3f;
    sf::Vector2u gridSize(512, 512);
    Water water(gridSize, propagation, friction, elasticity);


    /* Creation of the renderers */
    float amplitude = 0.1f;
    float waterLevel = 0.5f;
    float eta = 0.8f;
    glm::vec4 waterColor(.1, .1, .6, 1);
    float viewDistance = 3.f;
    Renderer2D renderer2D (amplitude, waterLevel, eta, waterColor, viewDistance);

    sf::Texture groundTexture;
    if (!groundTexture.loadFromFile("rc/tiles.png"))
        throw std::runtime_error("unable to open rc/tiles.png");
    groundTexture.setSmooth(true);
    groundTexture.setRepeated(true);

    /* Main loop */
    int loops = 0;
    sf::Clock fpsCounter, clock;
    while (window2D.isOpen()) {
        sf::Event event;
        while (window2D.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window2D.close();
                break;
                case sf::Event::KeyReleased:
                    if (event.key.code == sf::Keyboard::R) {
                        water.init();
                    }
                break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left)
                        water.touch(getRelativeMousePos(window2D), touchRadius, touchExtremum);
                break;
                case sf::Event::MouseMoved:
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                        water.touch(getRelativeMousePos(window2D), touchRadius, touchExtremum*0.1f);
                break;
                default:
                    break;
            }
        }

        sf::Time elapsedTime = clock.getElapsedTime();
        clock.restart();

        /* Simulation */
        water.update(elapsedTime.asSeconds());
        water.generateHeightmap();

        /* Rendering */
        window2D.clear(sf::Color::Green);
        window2D.setActive(true);
        renderer2D.draw (water.getHeightmap(), groundTexture);
        window2D.display();

       // std::cout << 1.f / elapsedTime.asSeconds() << std::endl;
        ++loops;

    }

    std::cout << "average fps: " << static_cast<float>(loops) / fpsCounter.getElapsedTime().asSeconds() << std::endl;

    return EXIT_SUCCESS;
}
