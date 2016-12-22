/**************************************************************************
 *   planet.cpp  --  This file is part of Acardov.                        *
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

#include "planet.h"

Planet::Planet() {
    this->geometry = std::unique_ptr<Geometry>(new Geometry());
    this->load_shader();
}

void Planet::draw() {
    this->shader->link_shader();

    const glm::mat4 mvp = Camera::get().get_projection() * glm::rotate(angle, glm::vec3(0,1,0)) * glm::scale(glm::vec3(5,5,5));
    this->shader->set_uniform("mvp", &mvp[0][0]);

    this->geometry->draw();

    this->shader->unlink_shader();
}

void Planet::update(double dt) {
    angle += dt * 0.5;
}

void Planet::load_shader() {
    this->shader = std::unique_ptr<Shader>(new Shader("assets/shaders/planet"));

    this->shader->add_attribute(ShaderAttribute::POSITION, "position");
    this->shader->add_attribute(ShaderAttribute::NORMAL, "normal");
    this->shader->add_attribute(ShaderAttribute::COLOR, "color");
    this->shader->add_uniform(ShaderUniform::MAT4, "mvp", 1);

    glBindVertexArray(this->geometry->get_vao());
    this->shader->bind_uniforms_and_attributes();
    glBindVertexArray(0);
}
