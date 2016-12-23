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
 * @file camera.cpp
 * @brief Source file for camera class
 *
 * @author Ivo Filot
 *
 * @date 2016-06-12
 */

#include "camera.h"

/**
 * @brief       update the camera perspective matrix
 *
 * @return      void
 */
void Camera::update() {
    // make sure base position is never below 1.5 units
    if(glm::length(this->base_position) < 1.5f) {
        this->base_position = 1.5f * glm::normalize(this->base_position);
        this->base_view_direction = -glm::normalize(this->base_position);
    }

    this->projection = glm::perspective(glm::radians(this->cam_angle),
                                            this->m_aspect,
                                            0.1f,
                                            300.0f);

    this->up = (drag_rotation * glm::vec4(this->base_up, 0.0)).xyz();
    this->view_direction = (drag_rotation * glm::vec4(this->base_view_direction, 0.0)).xyz();
    this->position = (drag_rotation * glm::vec4(this->base_position, 1.0)).xyz();
    this->look_at = this->position + this->view_direction * this->distance;

    this->view = glm::lookAt(
                    this->position,              // cam pos
                    this->look_at,               // look at
                    this->up                     // up
                );
}

/**
 * @brief       set the camera aspect ratio
 *
 * @param       aspect ratio
 * @return      void
 */
void Camera::set_aspect_ratio(const float& aspect_ratio) {
    this->m_aspect = aspect_ratio;
}

/**
 * @brief       set look_at position
 *
 * @param       look_at position
 * @return      void
 */
void Camera::set_look_at(const glm::vec3& _look_at) {
    this->look_at = _look_at;
    this->view = glm::translate(this->view, -this->look_at);
}

/**
 * @brief       translate the camera in the clock-wise direction
 *
 * @return      void
 */
void Camera::translate(const glm::vec3& trans) {
    glm::mat3 rot_mat = glm::inverse(glm::mat3(this->view));

    this->base_position += rot_mat * trans;

    this->update();
}

/**
 * @brief      rotate the camera in the clock-wise direction
 *
 * @return     void
 */
void Camera::rotate(const glm::vec3& drot) {
    // rotate up around the view direction
    this->base_up = glm::mat3(glm::rotate(drot[1], this->base_view_direction)) * this->base_up;

    // rotate view direction around the up direction
    this->base_view_direction = glm::mat3(glm::rotate(drot[2], this->base_up)) * this->base_view_direction;

    // rotate up and view vectors around their cross product vector
    glm::vec3 rotation_vector = glm::cross(this->base_view_direction, this->base_up);
    this->base_view_direction = glm::mat3(glm::rotate(drot[0], glm::normalize(rotation_vector))) * this->base_view_direction;
    this->base_up = glm::mat3(glm::rotate(drot[0], glm::normalize(rotation_vector))) * this->base_up;

    this->update();
}

/**
 * @brief       zoom in / out
 *
 * @return      void
 */
void Camera::zoom(double delta_z) {
    this->translate(glm::vec3(0.0f, 0.0f, (float)delta_z));
}

/**
 * @brief       calculate a ray originating based on mouse position and current view
 *
 * @param       mouse position
 * @param       pointer to vector holding ray origin
 * @param       pointer to vector holding ray direction
 * @return      void
 */
void Camera::calculate_ray(const glm::vec2& mouse_position, glm::vec3* ray_origin, glm::vec3* ray_direction) {
    const glm::mat4 projection = this->get_projection();
    const glm::mat4 view = this->get_view();

    const float screen_width = Screen::get().get_width();
    const float screen_height = Screen::get().get_height();

    const glm::vec3 ray_nds = glm::vec3((2.0f * mouse_position[0]) / screen_width - 1.0f,
                                         1.0f - (2.0f * mouse_position[1]) / screen_height,
                                         1.0);

    const glm::vec4 ray_clip(ray_nds.xy(), -1.0, 1.0);

    glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
    ray_eye = glm::vec4(ray_eye.xy(), -1.0, 0.0);
    *ray_direction = glm::normalize((glm::inverse(view) * ray_eye).xyz());

    // the origin of the ray in perspective projection is simply the position
    // of the camera in world space
    *ray_origin = this->get_position();
}

/**
 * @brief       calculate the vector on the arcball using mouse position
 *
 * @param       mouse position
 * @return      vector on the arcball
 */
glm::vec3 Camera::calculate_arcball_vector(const glm::vec2& mouse_position) {
    glm::vec3 p = glm::vec3(1.0f * mouse_position[0] / Screen::get().get_width() * 2.0f - 1.0f,
                            1.0f * mouse_position[1] / Screen::get().get_height() * 2.0f - 1.0f,
                            0.0f);

    p.y = -p.y;

    float op_squared = p.x * p.x + p.y * p.y;

    if(op_squared <= 1) {
        p.z = std::sqrt(1 - op_squared);
    } else {
        p = glm::normalize(p);
    }

    return p;
}

/**
 * @brief       set the current view based on two arcball vectors
 *
 * this function lets the user use the mouse to rotate the view
 *
 * @param       arcball vector 1
 * @param       arcball vector 2
 * @return      void
 */
void Camera::set_view_arcball(const glm::vec3& va, const glm::vec3& vb) {
    float dotprod = glm::dot(va, vb);
    if (fabs(dotprod) > 0.9999f) { // parallel vector, don't do anything
        return;
    }
    float angle = std::acos(std::min(1.0f, dotprod)) / glm::length(this->distance);
    glm::vec3 axis_in_camera_coord = glm::normalize(glm::cross(vb, va));
    glm::vec3 axis_in_world_space = glm::normalize((glm::inverse(this->base_view) * glm::vec4(axis_in_camera_coord, 0.0)).xyz());
    this->drag_rotation = glm::rotate(glm::degrees(angle), axis_in_world_space);
}

/**
 * @brief       confirm the current arcball rotation
 *
 * this function is executed when the user releases the mouse button
 *
 * @return      void
 */
void Camera::apply_arcball_rotation() {
    this->base_up = this->up;
    this->base_view_direction = this->view_direction;
    this->base_position = this->position;
    this->drag_rotation = glm::mat4(1.0f);
    this->update();
    this->base_view = view;
}

/**
 * @brief      reset the rotation angle
 *
 * @return     void
 */
void Camera::reset_angle() {
    this->set_view_top();
}

/**
 * @brief      orient the camera to the bottom perspective
 *
 * @return     void
 */
void Camera::set_view_bottom() {
    this->set_camera_orientation(glm::vec3(0,0,1), glm::vec3(0,-1,0));
}

/**
 * @brief      orient the camera to the top perspective
 *
 * @return     void
 */
void Camera::set_view_top() {
    this->set_camera_orientation(glm::vec3(0,0,-1), glm::vec3(0,1,0));
}

/**
 * @brief      orient the camera to the right perspective
 *
 * @return     void
 */
void Camera::set_view_right() {
    this->set_camera_orientation(glm::vec3(-1,0,0), glm::vec3(0,0,1));
}

/**
 * @brief      orient the camera to the front perspective
 *
 * @return     void
 */
void Camera::set_view_front() {
    this->set_camera_orientation(glm::vec3(0,1,0), glm::vec3(0,0,1));
}

/**
 * @brief      orient the camera to the left perspective
 *
 * @return     void
 */
void Camera::set_view_left() {
    this->set_camera_orientation(glm::vec3(1,0,0), glm::vec3(0,0,1));
}

/**
 * @brief      orient the camera to the back perspective
 *
 * @return     void
 */
void Camera::set_view_back() {
    this->set_camera_orientation(glm::vec3(0,-1,0), glm::vec3(0,0,1));
}

/**
 * @brief      set camera position and up direction
 *
 * @param      camera position
 * @param      up direction
 * @return     void
 */
void Camera::set_camera_position(const glm::vec3& _position, const glm::vec3& _up) {
    this->distance = glm::length(_position);
    this->set_camera_orientation(-glm::normalize(_position), _up);
    this->update();
}

/**
 * @brief       camera constructor
 *
 * @return      camera instance
 */
Camera::Camera() {
    this->distance = 10.0f;
    this->view = glm::mat4(1.0f);
    this->projection = glm::mat4(1.0f);
    this->m_aspect = 1.0f;
    this->reset_angle();
}

/**
 * @brief      orient the camera based on view and up vectors
 *
 * @param      view direction
 * @param      up direction
 * @return     void
 */
void Camera::set_camera_orientation(const glm::vec3& _view_dir, const glm::vec3& _up) {
    this->cam_angle = 45.0f;
    this->base_view_direction = _view_dir;
    this->base_up = _up;
    this->base_position = -this->base_view_direction * this->distance;
    this->update();
    this->apply_arcball_rotation();
}
