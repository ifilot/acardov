/****************************************************************************
#                                                                           #
#    INORGANIC MATERIALS CHEMISTRY CONFIDENTIAL                             #
#                                                                           #
#    Copyright 2016 Inorganic Materials Chemistry                           #
#                   Eindhoven University of Technology (TU/e)               #
#                                                                           #
#    All Rights Reserved.                                                   #
#                                                                           #
#    NOTICE:  All information contained herein is, and remains              #
#    the property of Inorganic Materials Chemistry (TU/e) and its suppliers,#
#    if any.  The intellectual and technical concepts contained             #
#    herein are proprietary to Inorganic Materials Chemistry (TU/e)         #
#    and its suppliers and may be covered by U.S. and Foreign Patents,      #
#    patents in process, and are protected by trade secret or copyright law.#
#    Dissemination of this information or reproduction of this Materials    #
#    is strictly forbidden unless prior written permission is obtained      #
#    from Inorganic Materials Chemistry (TU/e).                             #
#                                                                           #
#    Authors: Ivo Filot       <i.a.w.filot@tue.nl>                          #
#             Emiel Hensen    <e.j.m.hensen@tue.nl>                         #
#                                                                           #
*****************************************************************************/

/**
 * @file camera.h
 * @brief Header file for camera class
 *
 * @author Ivo Filot
 *
 * @date 2016-06-12
 */

#ifndef _CAMERA_H
#define _CAMERA_H

#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <algorithm>

#include "screen.h"

/**
 * @class Camera class
 * @brief class handling the camera
 */
class Camera {
private:
    glm::mat4 projection;               //!< perspective matrix
    glm::mat4 view;                     //!< view matrix
    glm::mat4 base_view;                //!< view matrix

    glm::vec3 base_view_direction;      //!< viewing direction of the camera
    glm::vec3 base_position;            //!< position of the camera in world space
    glm::vec3 base_up;                  //!< up vector of the camera

    glm::vec3 view_direction;           //!< viewing direction of the camera
    glm::vec3 position;                 //!< position of the camera in world space
    glm::vec3 look_at;                  //!< looking position in world space
    glm::vec3 up;                       //!< up vector of the camera

    glm::mat4 drag_rotation;            //!< matrix holding current rotation of arcball dragging

    float distance;                     //!< distance of the camera to the origin
    float cam_angle;                    //!< angle of the camera
    float m_aspect;                     //!< aspect ratio of the window

public:

    /**
     * @brief       get a reference to the camera object
     *
     * @return      reference to the camera object (singleton pattern)
     */
    static Camera& get() {
        static Camera camera_instance;
        return camera_instance;
    }

    //*************************
    // GETTERS
    //*************************

    /**
     * @brief       get current view matrix
     *
     * @return      view matrix
     */
    inline const glm::mat4& get_view() const {
        return this->view;
    }

    /**
     * @brief       get current projection matrix
     *
     * @return      projection matrix
     */
    inline const glm::mat4& get_projection() const {
        return this->projection;
    }

    /**
     * @brief       get current camera position
     *
     * @return      position
     */
    inline const glm::vec3& get_position() const {
        return this->position;
    }

    /**
     * @brief       get current look at vector
     *
     * @return      look at vector
     */
    inline const glm::vec3& get_look_at() const {
        return this->look_at;
    }

    /**
     * @brief       get current view direction
     *
     * @return      view direction
     */
    inline const glm::vec3& get_view_direction() const {
        return this->view_direction;
    }

    /**
     * @brief       get current up vector
     *
     * @return      up vector
     */
    inline const glm::vec3& get_up() const {
        return this->up;
    }

    /**
     * @brief       get current right vector
     *
     * @return      up vector
     */
    glm::vec3 get_right() const {
        return glm::cross(glm::normalize(this->up), glm::normalize(this->view_direction));
    }

    //*************************
    // SETTERS
    //*************************

    /**
     * @brief       set the projection matrix
     *
     * @param       projection matrix
     * @return      void
     */
    inline void set_projection(const glm::mat4& _projection) {
        this->projection = _projection;
    }

    /**
     * @brief       set the camera distance
     *
     * @param       distance of the camera to look_at point
     * @return      void
     */
    inline void set_distance(float _distance) {
        this->distance = _distance;
        this->set_view_top();
    }

    /**
     * @brief       update the camera perspective matrix
     *
     * @return      void
     */
    void update();

    /**
     * @brief       set the aspect ratio of the screen (changes projection matrix)
     *
     * @return      void
     */
    void set_aspect_ratio(const float& aspect_ratio);

    /**
     * @brief       set look_at position
     *
     * @param[in]   look_at position
     */
    void set_look_at(const glm::vec3& _look_at);

    /**
     * @brief      rotate the camera in the clock-wise direction
     *
     * @return     void
     */
    void rotate(const glm::vec3& drot);

    /**
     * @brief       translate the camera in the clock-wise direction
     *
     * @return      void
     */
    void translate(const glm::vec3& trans);

    /**
     * @brief       zoom in / out
     *
     * @return      void
     */
    void zoom(double delta_z);

    /**
     * @brief       calculate a ray originating based on mouse position and current view
     *
     * @param       mouse position
     * @param       pointer to vector holding ray origin
     * @param       pointer to vector holding ray direction
     * @return      void
     */
    void calculate_ray(const glm::vec2& mouse_position, glm::vec3* ray_origin, glm::vec3* ray_direction);

    /**
     * @brief       calculate the vector on the arcball using mouse position
     *
     * @param       mouse position
     * @return      vector on the arcball
     */
    glm::vec3 calculate_arcball_vector(const glm::vec2& mouse_position);

    /**
     * @brief       set the current view based on two arcball vectors
     *
     * this function lets the user use the mouse to rotate the view
     *
     * @param       arcball vector 1
     * @param       arcball vector 2
     * @return      void
     */
    void set_view_arcball(const glm::vec3& va, const glm::vec3& vb);

    /**
     * @brief       confirm the current arcball rotation
     *
     * this function is executed when the user releases the mouse button
     *
     * @return      void
     */
    void apply_arcball_rotation();

    /**
     * @brief      reset the rotation angle
     *
     * @return     void
     */
    void reset_angle();

    /**
     * @brief      orient the camera to the bottom position
     *
     * @return     void
     */
    void set_view_bottom();

    /**
     * @brief      orient the camera to the top position
     *
     * @return     void
     */
    void set_view_top();

    /**
     * @brief      orient the camera to the front position
     *
     * @return     void
     */
    void set_view_front();

    /**
     * @brief      orient the camera to the right position
     *
     * @return     void
     */
    void set_view_right();

    /**
     * @brief      orient the camera to the back position
     *
     * @return     void
     */
    void set_view_back();

    /**
     * @brief      orient the camera to the left position
     *
     * @return     void
     */
    void set_view_left();

    /**
     * @brief      set camera position and up direction
     *
     * @param      camera position
     * @param      up direction
     * @return     void
     */
    void set_camera_position(const glm::vec3& _position, const glm::vec3& _up);

private:
    /**
     * @brief       camera constructor
     *
     * @return      camera instance
     */
    Camera();

    /**
     * @brief      orient the camera based on view and up vectors
     *
     * @param      view direction
     * @param      up direction
     * @return     void
     */
    void set_camera_orientation(const glm::vec3& _view_dir, const glm::vec3& _up);

    Camera(Camera const&)          = delete;
    void operator=(Camera const&)  = delete;
};

#endif // _CAMERA_H
