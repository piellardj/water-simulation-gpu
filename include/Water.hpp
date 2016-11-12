#ifndef WATER_HPP_INCLUDED
#define WATER_HPP_INCLUDED

#include <array>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>


/* Class for simulating water surface.
 * The surface is divided into a grid of cells,
 * the thinner the more precise the simulation is.
 *
 * Each cell has two caracteristics:
 * - vertical position
 * - vertical speed
 *
 * A cell is subject to 2 forces:
 * - spring bringing back the particle to equilibrium.
 *  (controlled with elasticity parameter)
 * - springs making the cell align with its neighbours
 *  (controlled with propagation parameter)
 *
 * The water is meant to be displayed with the RenderX  classes.
 *
 * This class can export the surface with the generate/getHeightmap methods.
 * Internally it uses another format for more precision.
 */
class Water
{
    public:
        Water (sf::Vector2u gridSize,
               float propagation=20.f,
               float friction=0.99f,
               float elasticity=0.4);
        ~Water();

        sf::Vector2u getGridSize() const;

        /* Exports the water surface in a texture:
         * - RGB channels store the normal
         * - A channel stores the height */
        void generateHeightmap();
        sf::Texture const& getHeightmap () const;

        /* Updates the water surface (Euler integration) */
        void update (float time);

        /* Initializes the water to be still. */
        void init ();

        /* Adds a small perturbation.
         * - pos is the center of the perturbation, normalized in grid coordinates
         * - radius is the maximum effect radius, normalized in grid coordinates
         * - extremum is expected to be in [-1,1] */
        void touch (sf::Vector2f pos, float radius=0.1f, float extremum=0.9f);


    private:
        float _friction;
        float _propagation;
        float _elasticity;

        unsigned int _currentIndex;
        std::array<sf::RenderTexture, 2> _buffers;

        sf::Shader _initShader;
        sf::Shader _updateShader;
        sf::Shader _touchShader;

        sf::RenderTexture _heightmap;
        sf::Shader _generateHeightmapShader;
};

#endif // WATER_HPP_INCLUDED
