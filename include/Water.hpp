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

        sf::Vector2u getBufferSize() const;

        /* Updates the water surface (Euler intergration) */
        void update (sf::Time const& time);

        /* Displays the water as seen from above, with diffraction and reflection */
        void draw (sf::Texture const& background,
                   sf::RenderTarget& target) const;

        /* Initializes the water to be still */
        void init ();

        /* Adds a small perturbation.
         * WARNING pos is expected to be normalized */
        void touch (sf::Vector2f pos, float radius=20.f, float strength=0.9f);

    private:
        unsigned int _currentIndex;
        std::array<sf::RenderTexture, 2> _buffers;

        sf::Shader _initShader;
        mutable sf::Shader _displayShader;
        sf::Shader _updateShader;
        sf::Shader _touchShader;
};

#endif // WATER_HPP_INCLUDED
