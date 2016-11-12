#include "Renderer.hpp"

#include "Utilities.hpp"

#include <algorithm>


Renderer::Renderer(float amplitude,
                   float waterLevel,
                   float eta,
                   glm::vec4 const& waterColor,
                   float viewDistance,
                   glm::vec3 const& lightDir):
            _amplitude(amplitude),
            _waterLevel(waterLevel),
            _eta(eta),
            _waterColor(waterColor),
            _viewDistance(viewDistance),
            _lightDirection(glm::normalize(lightDir))
{
}

void Renderer::setAmplitude(float amplitude)
{
    _amplitude = amplitude;
}
float Renderer::getAmplitude() const
{
    return _amplitude;
}

void Renderer::setWaterLevel(float height)
{
    _waterLevel = height;
}
float Renderer::getWaterLevel() const
{
    return _waterLevel;
}

void Renderer::setEta (float eta)
{
    _eta = eta;
}
float Renderer::getEta() const
{
    return _eta;
}

void Renderer::setWaterColor(glm::vec4 color)
{
    _waterColor[0] = clamp(0.f, 1.f, color[0]);
    _waterColor[1] = clamp(0.f, 1.f, color[1]);
    _waterColor[2] = clamp(0.f, 1.f, color[2]);
    _waterColor[3] = clamp(0.f, 1.f, color[3]);
}

glm::vec4 const& Renderer::getWaterColor() const
{
    return _waterColor;
}

void Renderer::setViewDistance(float distance)
{
    _viewDistance = std::max(0.f, distance);
}
float Renderer::getViewDistance() const
{
    return _viewDistance;
}

void Renderer::setLightDirection (glm::vec3 lightDir)
{
    _lightDirection = glm::normalize(lightDir);
}
glm::vec3 const& Renderer::getLightDirection() const
{
    return _lightDirection;
}
