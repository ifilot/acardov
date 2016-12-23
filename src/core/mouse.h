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

#ifndef _MOUSE_H
#define _MOUSE_H

#include <iostream>
#include <memory>
#include <algorithm>

#include "core/camera.h"
#include "core/screen.h"
#include "core/shader.h"

/**
 * @class Mouse class
 *
 * @brief class handling mouse actions such as raycasting
 *
 */
class Mouse {
private:
    glm::vec3 ray_vector;                 //!< normalized raycast vector
    glm::vec3 ray_origin;                 //!< raycast origin

    bool drag_select;                     //!< whether mouse is selecting
    bool drag_view;                       //!< whether mouse is rotating camera

    glm::vec2 button_left_pos;            //!< mouse position when left button was pressed
    glm::vec2 button_middle_pos;          //!< mouse position when left button was pressed

    glm::vec2 cur_pos;                    //!< current cursor position
    glm::vec2 cur_pos_sw;                 //!< current cursor position with origin in SW position

public:
    /**
     * @fn          get
     *
     * @brief       get a reference to the mouse
     *
     * @return      reference to the mouse object (singleton pattern)
     */
    static Mouse& get() {
        static Mouse mouse_instance;
        return mouse_instance;
    }

    /*
     * @brief Calculate the direction of the ray cast
     */
    void calculate_ray();

    /*
     * @brief Perform these actions when left button is pressed
     */
    void button_left_press();

    /*
     * @brief Perform these actions when left button is pressed
     */
    void button_middle_press();

    /*
     * @brief Perform actions when left button is released
     */
    void button_left_release();

    /*
     * @brief Perform actions when left button is released
     */
    void button_middle_release();

    inline void set_drag_view(bool drag) {
        this->drag_view = drag;
    }

    /*
     * @brief Obtain the normalized ray vector
     *
     * @return normalized ray vector
     */
    inline const glm::vec3& get_ray_vector() const {
        return this->ray_vector;
    }

    inline const glm::vec3& get_ray_origin() const {
        return this->ray_origin;
    }

    inline const glm::vec2& get_button_left_pos() const {
        return this->button_left_pos;
    }

    inline const glm::vec2& get_button_middle_pos() const {
        return this->button_middle_pos;
    }

    inline const glm::vec2& get_cursor() const {
        return this->cur_pos;
    }

    inline const glm::vec2& get_cursor_sw() const {
        return this->cur_pos_sw;
    }

    inline glm::vec2 get_delta_movement_left() const {
        return (this->button_left_pos - this->cur_pos);
    }

    /*
     * @brief Update the current cursor position
     *
     * @param[in] xpos      current x position of the cursor on the screen
     * @param[in] xpos      current y position of the cursor on the screen
     */
    inline void set_cursor(double xpos, double ypos) {
        this->cur_pos[0] = (float)xpos;
        this->cur_pos[1] = (float)ypos;
        this->cur_pos_sw[0] = (float)xpos;
        this->cur_pos_sw[1] = (float)Screen::get().get_height() - (float)ypos;
    }

private:
    /**
     * @brief       mouse constructor
     */
    Mouse();

    Mouse(Mouse const&)          = delete;
    void operator=(Mouse const&)  = delete;
};

#endif //_MOUSE_H
