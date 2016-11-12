#ifndef RENDERER_HPP_INCLUDED
#define RENDERER_HPP_INCLUDED

#include "glm.hpp"


/* Base class for rendering water.
 * Stores all the visual caracteristics of the fluid:
 * - '0' level
 * - wave amplitude
 * - color
 * - refraction level
 * ...
 */
class Renderer
{
    /* This class is not supposed to be instanciated */
    protected:
        Renderer (float amplitude,
                  float waterLevel,
                  float eta,
                  glm::vec4 const& waterColor,
                  float viewDistance,
                  glm::vec3 const& lightDir);

    public:
        virtual ~Renderer() {}

        void setAmplitude(float amplitude);
        float getAmplitude() const;

        void setWaterLevel(float height);
        float getWaterLevel() const;

        void setEta (float eta);
        float getEta() const;

        void setWaterColor(glm::vec4 color);
        glm::vec4 const& getWaterColor() const;

        void setViewDistance(float distance);
        float getViewDistance() const;

        void setLightDirection (glm::vec3 lightDir);
        glm::vec3 const& getLightDirection() const;


    private:
        float _amplitude;
        float _waterLevel;

        float _eta;
        glm::vec4 _waterColor;
        float _viewDistance;

        glm::vec3 _lightDirection;
};

#endif // RENDERER_HPP_INCLUDED
