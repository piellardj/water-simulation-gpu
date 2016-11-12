#ifndef RENDERER2D_HPP_INCLUDED
#define RENDERER2D_HPP_INCLUDED

#include "Renderer.hpp"

#include <GL/glew.h>
#include "glm.hpp"
#include <SFML/OpenGL.hpp>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>


/* Class for rendering water in 2D, as seen from above,
 * with refraction, reflection, specularity.
*/
class Renderer2D: public Renderer
{
    public:
        Renderer2D(float amplitude=0.2f,
                   float waterLevel=0.5f,
                   float eta=0.8f,
                   glm::vec4 const& waterColor=glm::vec4(.1,.1,.8,1),
                   float viewDistance=2.f,
                   glm::vec3 const& lightDir=glm::vec3(-1,-1,-4));
        virtual ~Renderer2D();

        /* Disable copy constructor and assignment operator */
        Renderer2D (Renderer2D const& original) = delete;
        Renderer2D& operator= (Renderer2D const& original) = delete;

        /* Displays the water in the active OpenGL context.
         * Bottom left corner is expected to be (-1,-1),
         * top right (1,1).
         *
         * The heightmap must be in the format:
         * - RGB: normal for amplitude=1
         * - A: relative height for amplitude 1
         */
        void draw (sf::Texture const& heightmap,
                   sf::Texture const& groundTexture) const;


    private:
        GLuint _cornersBufferID;

        mutable sf::Shader _displayShader;
};

#endif // RENDERER2D_HPP_INCLUDED
