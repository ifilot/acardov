/**************************************************************************
 *   planet.h  --  This file is part of Acardov.                          *
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

#ifndef _PLANET_H
#define _PLANET_H

#include <algorithm>
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "core/shader.h"
#include "core/camera.h"
#include "game/terrain/geometry.h"
#include "game/terrain/tile.h"
#include "util/pngfuncs.h"

class Planet {
private:
    std::unique_ptr<Shader> shader_tiles;
    std::unique_ptr<Shader> shader_lines;

    std::vector<std::unique_ptr<Tile> > tiles;

    float angle;
    std::unique_ptr<Geometry> geometry;
    GLuint texture_id;

    GLuint vao_tiles;
    GLuint vbo_tiles[4];
    unsigned int nr_vertices;

    GLuint vao_lines;
    GLuint vbo_lines[2];
    unsigned int nr_lines;

public:
    /**
     * @brief       get a reference to the camera object
     *
     * @return      reference to the camera object (singleton pattern)
     */
    static Planet& get() {
        static Planet planet_instance;
        return planet_instance;
    }

    void draw();

    void update(double dt);

    inline const Tile* get_tile(unsigned int id) {
        return this->tiles[id].get();
    }

    ~Planet();

private:
    Planet();

    void load_shaders();

    void load_assets();

    void load_texture(const std::string& filename);

    void set_poles();

    Planet(Planet const&)          = delete;
    void operator=(Planet const&)  = delete;
};

#endif //_PLANET_H
