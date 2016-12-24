/**************************************************************************
 *   mesh.cpp  --  This file is part of Acardov.                          *
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

#include "mesh.h"

/**
 * @brief      Mesh constructor method using an input file
 *
 * @param[in]  filename  The filename
 */
Mesh::Mesh(const std::string& filename) {
    // read mesh data from file
    this->load_mesh_from_obj_file(filename);

    // load mesh on GPU (so that it can be rendered)
    this->load_on_gpu();
}

/**
 * @brief      draw the mesh
 */
void Mesh::draw() const {
    // load the vertex array
    glBindVertexArray(this->vao);

    // draw the mesh using the indices
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);

    // after this command, any commands that use a vertex array will
    // no longer work
    glBindVertexArray(0);
}

/**
 * @brief      center the vertex coordinates around the origin in model space
 */
void Mesh::center() {
    double sum_x = 0.0;
    double sum_y = 0.0;
    double sum_z = 0.0;

    for(unsigned int i=0; i<this->positions.size(); i++) {
        sum_x += this->positions[i].x;
        sum_y += this->positions[i].y;
        sum_z += this->positions[i].z;
    }

    double n_items = (double)this->positions.size();
    glm::vec3 center(sum_x / n_items,
                     sum_y / n_items,
                     sum_z / n_items);

    for(unsigned int i=0; i<this->positions.size(); i++) {
        this->positions[i] -= center;
    }
}

Mesh::~Mesh() {

}

/**
 * @brief      load Mesh from an .obj file
 *
 * @param[in]  filename  The filename
 */
void Mesh::load_mesh_from_obj_file(const std::string& filename) {
    // open file
    std::ifstream f;
    f.open((AssetManager::get().get_root_directory() + filename).c_str());

    if(!f.is_open()) {
        std::cerr << "Could not open file: " + filename << std::endl;
        std::cerr << "Exiting!" << std::endl;
        exit(-1);
    }

    boost::regex v_line("v\\s+([0-9.-]+)\\s+([0-9.-]+)\\s+([0-9.-]+).*");
    boost::regex vt_line("vt\\s+([0-9.-]+)\\s+([0-9.-]+).*");
    boost::regex vn_line("vn\\s+([0-9.-]+)\\s+([0-9.-]+)\\s+([0-9.-]+).*");
    boost::regex f1_line("f\\s+([0-9]+)\\/([0-9]+)\\/([0-9]+)\\s+([0-9]+)\\/([0-9]+)\\/([0-9]+)\\s+([0-9]+)\\/([0-9]+)\\/([0-9]+).*");
    boost::regex f2_line("f\\s+([0-9]+)\\/\\/([0-9]+)\\s+([0-9]+)\\/\\/([0-9]+)\\s+([0-9]+)\\/\\/([0-9]+).*");

    std::vector<glm::vec3> _positions;
    std::vector<glm::vec2> _texture_coordinates;
    std::vector<glm::vec3> _normal_coordinates;
    std::vector<unsigned int> _position_indices;
    std::vector<unsigned int> _texture_indices;
    std::vector<unsigned int> _normal_indices;

    std::string line;
    while(getline(f, line)) {
        boost::smatch what1;

        if (boost::regex_match(line, what1, v_line)) {
            glm::vec3 pos(boost::lexical_cast<float>(what1[1]),
                          boost::lexical_cast<float>(what1[2]),
                          boost::lexical_cast<float>(what1[3]));
            _positions.push_back(pos);
        }

        if (boost::regex_match(line, what1, vt_line)) {
            glm::vec2 tex(boost::lexical_cast<float>(what1[1]),
                          boost::lexical_cast<float>(what1[2]));
            _texture_coordinates.push_back(tex);
        }

        if (boost::regex_match(line, what1, vn_line)) {
            glm::vec3 normal(boost::lexical_cast<float>(what1[1]),
                             boost::lexical_cast<float>(what1[2]),
                             boost::lexical_cast<float>(what1[3]));
            _normal_coordinates.push_back(normal);
        }

        if (boost::regex_match(line, what1, f1_line)) {
            _position_indices.push_back(boost::lexical_cast<unsigned int>(what1[1]) - 1);
            _position_indices.push_back(boost::lexical_cast<unsigned int>(what1[4]) - 1);
            _position_indices.push_back(boost::lexical_cast<unsigned int>(what1[7]) - 1);

            _texture_indices.push_back(boost::lexical_cast<unsigned int>(what1[2]) - 1);
            _texture_indices.push_back(boost::lexical_cast<unsigned int>(what1[5]) - 1);
            _texture_indices.push_back(boost::lexical_cast<unsigned int>(what1[8]) - 1);

            _normal_indices.push_back(boost::lexical_cast<unsigned int>(what1[3]) - 1);
            _normal_indices.push_back(boost::lexical_cast<unsigned int>(what1[6]) - 1);
            _normal_indices.push_back(boost::lexical_cast<unsigned int>(what1[9]) - 1);
        }

        if (boost::regex_match(line, what1, f2_line)) {
            _position_indices.push_back(boost::lexical_cast<unsigned int>(what1[1]) - 1);
            _position_indices.push_back(boost::lexical_cast<unsigned int>(what1[3]) - 1);
            _position_indices.push_back(boost::lexical_cast<unsigned int>(what1[5]) - 1);

            _normal_indices.push_back(boost::lexical_cast<unsigned int>(what1[2]) - 1);
            _normal_indices.push_back(boost::lexical_cast<unsigned int>(what1[4]) - 1);
            _normal_indices.push_back(boost::lexical_cast<unsigned int>(what1[6]) - 1);
        }
    }

    if(_position_indices.size() > 0 && _texture_indices.size() > 0) {
        for(unsigned int i=0; i<_position_indices.size(); i++) {
            this->indices.push_back(i);

            this->positions.push_back(_positions[_position_indices[i]]);
            this->texture_coordinates.push_back(_texture_coordinates[_texture_indices[i]]);
            this->normals.push_back(_normal_coordinates[_normal_indices[i]]);
        }
    }

    if(_position_indices.size() > 0 && _texture_indices.size() == 0) {
        for(unsigned int i=0; i<_position_indices.size(); i++) {
            this->indices.push_back(i);

            this->positions.push_back(_positions[_position_indices[i]]);
            this->normals.push_back(_normal_coordinates[_normal_indices[i]]);
        }
    }

    std::cout << "Model summary: " << filename << std::endl;
    std::cout << "Positions: " << this->positions.size() << std::endl;
    std::cout << "Normals: " << this->normals.size() << std::endl;
}

void Mesh::load_on_gpu() {
    if(this->indices.size() == 0) {
        std::cerr << "Cannot load mesh with 0 indices on GPU!" << std::endl;
        std::cerr << "Did you read a mesh file before loading it?" << std::endl;
        exit(-1);
    }

    // load vao and vbo
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);
    glGenBuffers(3, this->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, this->positions.size() * 3 * sizeof(float), &this->positions[0][0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, this->normals.size() * 3 * sizeof(float), &this->normals[0][0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbo[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}
