#pragma once
#include "openGlObject.h"
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <nico/window.h>
#include <nico/Shader.h>

#define PI 3.14f


namespace nico {
    /**
     * @class Camera
     * @brief create some 4x4 matrices to simulate a camera in a 3D world\n
     * this allow to display your 3D scene on a 2d screen
     */
    class Camera
    {
    public:
        /**
         * @brief create the camera with some default values
         * \param pos
         * \param pitch
         * \param yaw
         * \param fov
         */
        Camera(glm::vec3 pos = glm::vec3(0,0,0), float pitch = PI/2.0f, float yaw = 0, float fov = 45.0f);
        /**
         * @brief this take the values of the camera and update the matrices with them
         * the change of the camera will not be applied until you call this method
         * \param windowObject (needed to know the size of the window)
         */
        void calculateMatrix(Window *windowObject);
        /**
         * @brief this take the values of the camera and update the matrices with them
         * the change of the camera will not be applied until you call this method
         * \param width of the screen in pixel
         * \param height of the screen in pixel
         */
        void calculateMatrix(float width, float height);
        /**
         * @brief just modify the look vector with the yaw and the pitch of the camera
         */
        void updateLookWithYawAndPitch();
        /**
         * @brief set a projection and a view matrix in a shader and also a camera location vector
         * the uniform names in the shader are set by default but you can change them
         * \param shader_
         * \param projection_
         * \param view_
         * \param pos_
         */
        void sendToShader(Shader* shader_, const char* projection_ = "projection"
                                              , const char* view_ = "view"
                                              , const char*  pos_ = "viewPos");                                  
        /**
         * @brief allow to modif pitch and yaw (the view direction) with the mouse
         * \param win
         * \param mouseSensivity
         */
        void classicMouseControls(Window* win, double mouseSensivity);
        /**
         * @brief allow to modif the camera with z,q,s,d, space and left keys
         * \param win
         * \param speed
         */
        void classicKeyboardControls(Window* win, float speed);
        /**
         * @brief return if a point is in the field of view of the camera
         * \param vertexPosition
         * \param offset
         * \return 
         */
        bool isVertexInTheFieldOfView(glm::vec3 vertexLocation, float offset = 0.0f);
       
        //MODIFIERS
        /**
         * @brief change location of the camera
         * \param position_
         */
        void setPosition(glm::vec3 position_);
        /**
         * @brief change the direction of the view of the camera
         * \param look_
         */
        void setLook(glm::vec3 look_);
        /**
         * @brief set the up direction of the camera vec3(0, 1, 0) by default vec3(0, -1, 0) will look upside down
         * \param up_
         */
        void setUpDirection(glm::vec3 up_);
        /**
         * @brief this is the rotation around the x axis of the camera in radians
         * \param pitch_
         */
        void setPitch(float pitch_);
        /**
         * @brief this is the rotation around the y axis of the camera in radians
         * \param yaw_
         */
        void setYaw(float yaw_);
        /**
         * @brief set your own projection matrix
         * \param projection_
         */
        void setProjection(glm::mat4 projection_);
        /**
         * @brief set your own view matrix
         * \param view_
         */
        void setView(glm::mat4 view_);
        /**
         * @brief angle of view in degrees
         * \param fov_
         */
        void setFov(float fov_);
        /**
         * @brief all the fragments which have a distance less than ZNear are not displayed
         * so this method change  that ZNear value (by default equals to 0.1f)
         * \param zNear_
         */
        void setZNear(float zNear_);
        /**
         * @brief this is the maximun distance where fragment can be seen by this camera
         * don't put a too big value because this will provoke some performances issues
         * \param zFar_
         */
        void setZFar(float zFar_);

        //accessors :
        /**
         * @brief return the right direction relatively to the camera
         * \return 
         */
        glm::vec3 getRightDirection() const;
        /**
         * @brief return the location of the camera
         * \return 
         */
        glm::vec3 getPosition() const;
        /**
         * @brief return the direction of the view
         * \return 
         */
        glm::vec3 getLook() const;
        /**
         * @brief return the up direction
         * \return 
         */
        glm::vec3 getUpDirection() const;
        /**
         * @brief return Pitch
         * \return 
         */
        float getPitch() const;
        /**
         * @brief return yaw
         * \return 
         */
        float getYaw() const;
        /**
         * @brief return the projection matrix
         * \return 
         */
        glm::mat4 getProjection() const;
        /**
         * @brief return the view matrix
         * \return 
         */
        glm::mat4 getView() const;
        /**
         * @brief a little method to determine a (Model, View and Model) matrix
         * \param modelMatrix
         * \return 
         */
        glm::mat4 getMVP(glm::mat4& modelMatrix) const;
        /**
         * @brief return the field of view of the camera
         * \return 
         */
        float getFov() const;
        /**
         * @brief return the minimun Z of the camera
         * \return 
         */
        float getZNear() const;
        /**
         * @brief return the maximun Z of the camera
         * \return 
         */
        float getZFar() const;

    protected:
        glm::vec3 position;
        glm::vec3 look;
        glm::vec3 upDirection;

        float pitch;     //vertical look angle
        float yaw;       //horizontal look angle

        glm::mat4 projection;
        glm::mat4 view;

        float fov;
        float zNear;
        float zFar;

    };
}
