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
    this->geometry = std::unique_ptr<Geometry>(new Geometry(4));
    this->load_assets();
    this->load_shader();
}

void Planet::draw() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    this->shader->link_shader();

    const glm::mat4 mvp = Camera::get().get_projection() * glm::rotate(angle, glm::vec3(0,1,0)) * glm::scale(glm::vec3(5,5,5));
    this->shader->set_uniform("mvp", &mvp[0][0]);
    const unsigned int texture_slot = 0;
    this->shader->set_uniform("text", &texture_slot);

    this->geometry->draw();

    this->shader->unlink_shader();
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
}

void Planet::update(double dt) {
    angle += dt * 0.5;
}

void Planet::load_shader() {
    this->shader = std::unique_ptr<Shader>(new Shader("assets/shaders/planet"));

    this->shader->add_attribute(ShaderAttribute::POSITION, "position");
    this->shader->add_attribute(ShaderAttribute::NORMAL, "normal");
    this->shader->add_attribute(ShaderAttribute::TEXTURE_COORDINATE, "uv");
    this->shader->add_uniform(ShaderUniform::MAT4, "mvp", 1);
    this->shader->add_uniform(ShaderUniform::TEXTURE, "text", 1);

    glBindVertexArray(this->geometry->get_vao());
    this->shader->bind_uniforms_and_attributes();
    glBindVertexArray(0);
}

void Planet::load_assets() {
    this->load_texture("assets/png/tile_sand.png");
}

void Planet::load_texture(const std::string& filename) {
    glActiveTexture(GL_TEXTURE0);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    std::vector<uint8_t> pixels;
    png_uint_32 width, height;
    int col, bit_depth;

    PNG::load_image_buffer_from_png(AssetManager::get().get_root_directory() + filename,
                                    pixels,
                                    &width,
                                    &height,
                                    &col,
                                    &bit_depth,
                                    true);  // automatically invert the image

    switch(col) {
        case PNG_COLOR_TYPE_RGB:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &pixels[0]);
        break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
        break;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
}
