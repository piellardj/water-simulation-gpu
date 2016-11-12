#ifndef RENDERER3D_HPP_INCLUDED
#define RENDERER3D_HPP_INCLUDED

#include "Renderer.hpp"
#include "Camera.hpp"

#include <GL/glew.h>
#include "glm.hpp"
#include <SFML/OpenGL.hpp>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>


/* Class for rendering water in 3D,
 * with refraction, underwater lights, reflection, specularity.
 *
 * The scene can be changed with the getCamera() method.
*/
class Renderer3D: public Renderer
{
    public:
        Renderer3D(unsigned int quality,
                   float amplitude=0.3f,
                   float waterLevel=0.5f,
                   float eta=0.8f,
                   glm::vec4 const& waterColor=glm::vec4(.1,.1,.6,1),
                   float viewDistance=2.f,
                   glm::vec3 const& lightDir=glm::vec3(1,-1,-1));
        virtual ~Renderer3D();

        /* Disable copy constructor and assignment operator */
        Renderer3D (Renderer3D const& original) = delete;
        Renderer3D& operator= (Renderer3D const& original) = delete;

        /* Displays the water in the active OpenGL context.
         * Bottom left corner is expected to be (-1,-1),
         * top right (1,1).
         *
         * The heightmap must be in the format:
         * - RGB: normal for amplitude=1
         * - A: relative height for amplitude 1
         */
        void draw (sf::Texture const& heightmap,
                   sf::Texture const& groundTexture,
                   sf::Texture const& lightsTexture) const;

        Camera& getCamera();

    private:
        void drawSurface (sf::Texture const& heightmap,
                          sf::Texture const& groundTexture,
                          sf::Texture const& lightsTexture) const;

        void drawCube (sf::Texture const& heightmap,
                       sf::Texture const& groundTexture,
                       sf::Texture const& lightsTexture) const;


    private:
        GLuint _surfaceGridPosBufferID;
        GLuint _surfaceGridIndexBufferID;
        sf::Vector2u _surfaceGridSize;

        GLuint _cubeVertBufferID;
        GLuint _cubeNormBufferID;

        Camera _camera;

        mutable sf::Shader _displaySurfaceShader;
        mutable sf::Shader _displayCubeShader;
};

#endif // RENDERER3D_HPP_INCLUDED
