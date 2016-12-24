/**************************************************************************
 *   game.cpp  --  This file is part of Acardov.                          *
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

#include "game.h"

Game::Game() {
    Planet::get();

    this->meshes.emplace_back(std::unique_ptr<Mesh>(new Mesh("assets/models/piece_base_hexagon.obj")));
    this->meshes.emplace_back(std::unique_ptr<Mesh>(new Mesh("assets/models/piece_base_circle.obj")));
    this->meshes.emplace_back(std::unique_ptr<Mesh>(new Mesh("assets/models/piece_gears.obj")));
    this->meshes.emplace_back(std::unique_ptr<Mesh>(new Mesh("assets/models/piece_cube.obj")));

    this->shader = std::unique_ptr<Shader>(new Shader("assets/shaders/piece"));
    this->shader->add_attribute(ShaderAttribute::POSITION, "position");
    this->shader->add_attribute(ShaderAttribute::NORMAL, "normal");
    this->shader->add_uniform(ShaderUniform::MAT4, "mvp", 1);
    this->shader->add_uniform(ShaderUniform::MAT4, "view", 1);
    this->shader->add_uniform(ShaderUniform::VEC3, "light", 1);
    this->shader->add_uniform(ShaderUniform::VEC3, "color", 1);

    this->meshes[0]->load_vao();
    this->shader->bind_uniforms_and_attributes();
    glBindVertexArray(0);

    this->add_piece(86, 0);
    this->add_piece(200, 0);
    this->add_piece(12, 0);
    this->add_piece(88, 1);
    this->add_piece(201, 1);
    this->add_piece(13, 1);
}

void Game::draw() {
    Planet::get().draw();

    this->shader->link_shader();

    for(auto& piece: pieces) {
        piece.draw();
    }

    this->shader->unlink_shader();
}


void Game::update(double dt) {
    Planet::get().update(dt);
}

void Game::add_piece(unsigned int tile_id, unsigned int type) {
    this->pieces.push_back(Piece(this->shader.get()));

    switch(type) {
        case 0:
            this->pieces.back().add_mesh(this->meshes[0].get(), glm::vec3(0.5f, 0.5f, 0.5f));
            this->pieces.back().add_mesh(this->meshes[2].get(), glm::vec3(1.0f, 0.5f, 0.5f));
        break;
        case 1:
            this->pieces.back().add_mesh(this->meshes[1].get(), hex2col("f7a147"));
            this->pieces.back().add_mesh(this->meshes[3].get(), glm::vec3(0.0f, 0.0f, 0.0f));
        break;
    }

    this->pieces.back().set_tile(Planet::get().get_tile(tile_id));
}
