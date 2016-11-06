#ifndef WATER_HPP_INCLUDED
#define WATER_HPP_INCLUDED

#include <array>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>


/* Class for simulating and displaying 2D water */
class Water
{
    public:
        Water (sf::Vector2u dimensions);

        /* Updates the water surface (Euler intergration) */
        void update (sf::Time const& time);

        /* Displays the water as seen from above, with diffraction and reflection */
        void draw (sf::Texture const& background,
                   sf::RenderTarget& target);

        /* Initializes the water to be still */
        void init ();

        /* Adds a small perturbation */
        void touch (sf::Vector2i pos);

    private:
        const sf::Vector2f _bufferSize;

        unsigned int _currentIndex;
        std::array<sf::RenderTexture, 2> _positions;
        std::array<sf::RenderTexture, 2> _velocities;

        sf::Shader _initPositionShader;
        sf::Shader _initVelocityShader;
        sf::Shader _updateVelocityShader;
        sf::Shader _updatePositionShader;
        sf::Shader _displayShader;
        sf::Shader _touchShader;
};

#endif // WATER_HPP_INCLUDED
