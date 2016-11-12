#ifndef LIGHTSRENDERER_HPP_INCLUDED
#define LIGHTSRENDERER_HPP_INCLUDED

#include "Renderer.hpp"

#include <GL/glew.h>
#include "glm.hpp"
#include <SFML/OpenGL.hpp>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>


/* Class for computing light rays hitting the ground.
 * They are coming from above the surface and refracted in the water.
 *
 * To do so this class computes the trajectories of "light particles"
 * uniformly spread on the surface to the ground underwater.
 *
 */
class LightsRenderer: public Renderer
{
    public:
        LightsRenderer (unsigned int quality,
                        float amplitude=0.3f,
                        float waterLevel=0.5f,
                        float eta=0.8f,
                        glm::vec3 const& lightDir=glm::vec3(0,0,-1));
        virtual ~LightsRenderer();

        /* Disable copy constructor and assignment operator */
        LightsRenderer (LightsRenderer const& original) = delete;
        LightsRenderer& operator= (LightsRenderer const& original) = delete;

        void reset();

        /* Computes the raw lights and then processes them.
         * The resulting texture can be accessed by getTexture().
         *
         * The heightmap must be in the format:
         * - RGB: normal for amplitude=1
         * - A: relative height for amplitude 1
         */
        void update (sf::Texture const& heightmap);

        /* Returns the final lights texture. */
        sf::Texture const& getTexture() const;

    private:
        /* Computes the light hitting the ground.
         * The resulting texture still has to be processed.*/
        void computeRawLights (sf::Texture const& heightmap);

        /* Post-processes the raw lights: luminosity range adjustments */
        void computeProcessedLights();


    private:
        unsigned int _partPerPixel;
        float _intensity;

        GLuint _particlesGridBufferID;
        sf::Vector2u _particlesGridSize;
        float _particleSize;

        sf::RenderTexture _rawLights; //lights only
        sf::RenderTexture _processedLights; //post-processed lights

        sf::Shader _computeLightsShader;
        sf::Shader _processLightsShader;
};

#endif // LIGHTSRENDERER_HPP_INCLUDED
