/**************************************************************************
 *   mesh.h  --  This file is part of Acardov.                            *
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

#ifndef _MESH_H
#define _MESH_H

#include <vector>
#include <fstream>
#include <iostream>
#include <string>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <GL/glew.h>

#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "core/asset_manager.h"

class Mesh {
private:
    std::vector<glm::vec3> positions;                   //!< vector holding positions
    std::vector<glm::vec3> normals;                     //!< vector holding vertex normals
    std::vector<glm::vec2> texture_coordinates;         //!< vector holding texture coordinates
    std::vector<unsigned int> indices;                  //!< vector holding set of indices

    GLuint vao;
    GLuint vbo[3];

public:

    /**
     * @brief      Mesh constructor
     */
    Mesh();

    /**
     * @brief      Mesh constructor method using an input file
     *
     * @param[in]  filename  The filename
     */
    Mesh(const std::string& filename);

    /**
     * @brief      draw the mesh
     */
    void draw() const;

    /**
     * @brief      center the vertex coordinates around the origin in model space
     */
    void center();

    inline void load_vao() const {
        glBindVertexArray(this->vao);
    }

    ~Mesh();

private:

    /**
     * @brief      load Mesh from an .obj file
     *
     * @param[in]  filename  The filename
     */
    void load_mesh_from_obj_file(const std::string& filename);

    void load_on_gpu();
};


#endif //_MESH_H
