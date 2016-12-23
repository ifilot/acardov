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

#include "core/mouse.h"

/**
 * @brief       mouse constructor
 */
Mouse::Mouse() {

}

/*
 * @brief Calculate the direction of the ray cast
 */
void Mouse::calculate_ray() {
    Camera::get().calculate_ray(this->cur_pos, &this->ray_origin, &this->ray_vector);
}

/*
 * @brief Perform these actions when left button is pressed
 */
void Mouse::button_left_press() {
    if(!(this->drag_view || this->drag_select)) {
        this->button_left_pos = this->cur_pos;
        return;
    }

    if(this->drag_view) {
        glm::vec3 va = Camera::get().calculate_arcball_vector(this->button_left_pos);
        glm::vec3 vb = Camera::get().calculate_arcball_vector(this->cur_pos);
        Camera::get().set_view_arcball(va, vb);
        return;
    }
}

/*
 * @brief Perform actions when left button is released
 */
void Mouse::button_left_release() {
    Camera::get().apply_arcball_rotation();
    this->drag_view = false;
}
