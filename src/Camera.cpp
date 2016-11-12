#include "Camera.hpp"

#include "Utilities.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>


const float F_PI = static_cast<float>(M_PI);

Camera::Camera (glm::vec3 focusPoint, float distance, float latitude, float longitude):
            _FoV(F_PI/4.f),
            _aspectRatio(4.f/3.f),
            _nearClipping(0.1f),
            _farClipping(10.f),
            _cameraUp(glm::vec3(0,0,1)),
            _focusPoint (focusPoint),
            _distance (std::max(0.f, distance)),
            _latitude (clamp(-F_PI/2.f, F_PI/2.f, latitude)),
            _longitude (longitude)
{
    computeViewMatrix();
    computeProjectionMatrix();
}

void Camera::setFov (float FoV)
{
    _FoV = FoV;
    computeProjectionMatrix();
}
float Camera::getFov () const
{
    return _FoV;
}

void Camera::setAspectRatio(float width, float height)
{
    _aspectRatio = width / height;
    computeProjectionMatrix();
}

float Camera::getAspectRatio () const
{
    return _aspectRatio;
}

void Camera::setFocusPoint (glm::vec3 focusPoint)
{
    _focusPoint = focusPoint;
    computeViewMatrix();
}
glm::vec3 const& Camera::getFocusPoint () const
{
    return _focusPoint;
}

void Camera::setDistance (float distance)
{
    _distance = std::max(0.f, distance);
    computeViewMatrix();
}
float Camera::getDistance () const
{
    return _distance;
}

/* In radians */
void Camera::setLatitude (float latitude)
{
    _latitude = clamp(-F_PI/2.f, F_PI/2.f, latitude);
    computeViewMatrix();
}
float Camera::getLatitude () const
{
    return _latitude;
}

/* In radians, between [-PI/2, PI/2], clamped otherwise*/
void Camera::setLongitude (float longitude)
{
    _longitude = longitude;
    computeViewMatrix();
}
float Camera::getLongitude () const
{
    return _longitude;
}

glm::vec3 Camera::getPosition() const
{
    glm::vec3 position = glm::vec3(std::cos(_longitude)*std::cos(_latitude),
                                   std::sin(_longitude)*std::cos(_latitude),
                                   std::sin(_latitude));

    return getFocusPoint() + glm::normalize(position) * _distance;
}

/* Returns projectionMatrix * viewMatrix */
glm::mat4 Camera::getMatrix () const
{
    return _projectionMatrix * _viewMatrix;
}

void Camera::computeProjectionMatrix()
{
    _projectionMatrix = glm::perspective(_FoV, _aspectRatio, _nearClipping, _farClipping);
}

void Camera::computeViewMatrix()
{
    _viewMatrix = glm::lookAt(getPosition(), _focusPoint, _cameraUp);
}
