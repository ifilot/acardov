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
    this->geometry->load_vertices_dual_gpu(&this->vao_tiles, &this->vbo_tiles[0], &this->nr_vertices);
    this->geometry->load_lines_dual_gpu(&this->vao_lines, &this->vbo_lines[0], &this->nr_lines);
    this->geometry->load_tiles(&this->tiles);
    this->load_assets();
    this->load_shaders();

    this->set_poles();
}

void Planet::draw() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // draw all tiles
    this->shader_tiles->link_shader();

    const glm::mat4 mvp_tiles = Camera::get().get_projection() * Camera::get().get_view();
    this->shader_tiles->set_uniform("mvp", &mvp_tiles[0][0]);

    glBindVertexArray(this->vao_tiles);
    glDrawElements(GL_TRIANGLES, this->nr_vertices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    this->shader_tiles->unlink_shader();

    // draw all lines
    this->shader_lines->link_shader();

    static const float s = 1.001f;
    const glm::mat4 mvp_lines = mvp_tiles * glm::scale(glm::vec3(s,s,s));
    this->shader_lines->set_uniform("mvp", &mvp_lines[0][0]);

    glBindVertexArray(this->vao_lines);
    glDrawElements(GL_LINES, this->nr_lines, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    this->shader_lines->unlink_shader();

    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
}

void Planet::update(double dt) {

}

void Planet::load_shaders() {
    // load shader for the tiles
    this->shader_tiles = std::unique_ptr<Shader>(new Shader("assets/shaders/planet"));

    this->shader_tiles->add_attribute(ShaderAttribute::POSITION, "position");
    this->shader_tiles->add_attribute(ShaderAttribute::NORMAL, "normal");
    this->shader_tiles->add_attribute(ShaderAttribute::COLOR, "color");
    this->shader_tiles->add_uniform(ShaderUniform::MAT4, "mvp", 1);

    glBindVertexArray(this->vao_tiles);
    this->shader_tiles->bind_uniforms_and_attributes();
    glBindVertexArray(0);

    // load shader for the lines
    this->shader_lines = std::unique_ptr<Shader>(new Shader("assets/shaders/lines"));

    this->shader_lines->add_attribute(ShaderAttribute::POSITION, "position");
    this->shader_lines->add_uniform(ShaderUniform::MAT4, "mvp", 1);

    glBindVertexArray(this->vao_lines);
    this->shader_lines->bind_uniforms_and_attributes();
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

void Planet::set_poles() {
    glBindVertexArray(this->vao_tiles);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_tiles[2]);

    std::vector<glm::vec3> data;
    for(unsigned int i=0; i<18; i++) {
        data.push_back(hex2col("d2dadc"));
    }

    for(auto&& tile: tiles) {
        if(std::abs(tile->get_pos()[2]) > 0.9f) {
            glBufferSubData(GL_ARRAY_BUFFER, tile->get_memory_offset() * 9 * sizeof(float), tile->get_size() * 9 * sizeof(float), &data[0][0]);
        }
    }

    glBindVertexArray(0);
}

Planet::~Planet() {
    glBindVertexArray(0);
    glDeleteBuffers(4, this->vbo_tiles);
    glDeleteVertexArrays(1, &this->vao_tiles);

    glDeleteBuffers(4, this->vbo_lines);
    glDeleteVertexArrays(1, &this->vao_lines);
}
