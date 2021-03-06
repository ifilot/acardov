/**************************************************************************
 *   mathfunc.cpp  --  This file is part of Acardov.                      *
 *                                                                        *
 *   Copyright (C) 2016, Ivo Filot                                        *
 *                                                                        *
 *   Netris is free software: you can redistribute it and/or modify       *
 *   it under the terms of the GNU General Public License as published    *
 *   by the Free Software Foundation, either version 3 of the License,    *
 *   or (at your option) any later version.                               *
 *                                                                        *
 *   Netris is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#include "mathfunc.h"

glm::vec3 hex2col(const std::string& strhex) {
    const unsigned int r = std::stoul(strhex.substr(0,2), NULL, 16);
    const unsigned int g = std::stoul(strhex.substr(2,2), NULL, 16);
    const unsigned int b = std::stoul(strhex.substr(4,2), NULL, 16);

    return glm::vec3((float)r / 255.f, (float)g / 255.f, (float)b / 255.f);
}

glm::mat4 get_rotation_matrix(const glm::vec3& v1, const glm::vec3& v2) {
    const float dotprod = glm::dot(v1, v2);

    // check for parallel or anti-parallel configurations
    glm::mat4 rot(1.0f);
    glm::vec3 axis_angle;
    if(fabs(dotprod) > 0.999f) { // axes are parallel or anti-parallel
        if(dotprod < -0.5f) { // anti-parallel --> rotate system by 180 degrees
            const float angle = -(float)M_PI;
            axis_angle = glm::vec3(0, 1, 0);
            rot = glm::rotate(glm::mat4(1.0), angle, axis_angle);
        } // else parallel --> do nothing
    } else { // neither parallel or anti-parallel --> normal behavior
        const float angle = std::acos(dotprod);
        axis_angle = glm::normalize(glm::cross(v1, v2));
        rot = glm::rotate(glm::mat4(1.0), angle, axis_angle);
    }

    return rot;
}
