/**************************************************************************
 *   piece.cpp  --  This file is part of Acardov.                         *
 *                                                                        *
 *   Copyright (C) 2016, Ivo Filot                                        *
 *                                                                        *
 *   Acardov is free software: you can redistribute it and/or modify      *
 *   it under the terms of the GNU General Public License as published    *
 *   by the Free Software Foundation, either version 3 of the License,    *
 *   or (at your option) any later version.                               *
 *                                                                        *
 *   Acardov is distributed in the hope that it will be useful,           *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#include "piece.h"

Piece::Piece(Shader* _shader) {
    this->shader = _shader;
    this->tile = NULL;
}

void Piece::draw() {
    if(this->tile == NULL) {
        return;
    }

    const glm::mat4 rot = get_rotation_matrix(glm::vec3(0,1,0), tile->get_pos());
    const glm::mat4 model = glm::translate(tile->get_pos()) * rot * glm::scale(glm::vec3(0.03f, 0.03f, 0.03f));

    const glm::mat4 view = Camera::get().get_view();
    const glm::mat4 projection = Camera::get().get_projection();

    glm::mat4 mvp = projection * view * model;

    this->shader->set_uniform("mvp", &mvp[0][0]);
    this->shader->set_uniform("view", &view[0][0]);
    this->shader->set_uniform("light", &Light::get().get_position()[0]);

    for(unsigned int i=0; i<this->meshes.size(); i++) {
        this->shader->set_uniform("color", &this->colors[i][0]);
        this->meshes[i]->draw();
    }
}
