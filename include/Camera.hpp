#ifndef CAMERA_HPP_INCLUDED
#define CAMERA_HPP_INCLUDED

#include "glm.hpp"


/* Trackball camera class, computing viewMatrix annd projectionMatrix.
 *
 * Position is defined by a latitude, a longitutde and a distance from focusPoint.
 */
class Camera
{
    public:
        Camera (glm::vec3 focusPoint=glm::vec3(0,0,0),
                float distance=2.f,
                float latitude=3.1415f/8.f,
                float longitude=-3.1415f/4.f);

        /* In radians */
        void setFov (float Fov);
        float getFov () const;

        void setAspectRatio(float width, float height);
        float getAspectRatio () const;

        void setFocusPoint (glm::vec3 focusPoint);
        glm::vec3 const& getFocusPoint () const;

        /* Positive, clamped to 0 otherwise */
        void setDistance (float distance);
        float getDistance () const;

        /* In radians */
        void setLatitude (float latitude);
        float getLatitude () const;

        /* In radians, between [-PI/2, PI/2], clamped otherwise*/
        void setLongitude (float longitude);
        float getLongitude () const;

        glm::vec3 getPosition () const;

        /* Returns projectionMatrix * viewMatrix */
        glm::mat4 getMatrix () const;

    private:
        void computeProjectionMatrix();
        void computeViewMatrix();


    private:
        float _FoV;
        float _aspectRatio;
        const float _nearClipping;
        const float _farClipping;

        const glm::vec3 _cameraUp;
        glm::vec3 _focusPoint;
        float _distance;
        float _latitude;
        float _longitude;

        glm::mat4 _viewMatrix;
        glm::mat4 _projectionMatrix;
};

#endif // CAMERA_HPP_INCLUDED
