/**************************************************************************
 *   geometry.h  --  This file is part of Acardov.                        *
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

#ifndef _VERTEX_H
#define _VERTEX_H

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>

class Triangle; // forward declaration

class Vertex {
private:
    std::vector<Triangle*> triangles;
    glm::vec3 pos;

public:
    Vertex(const glm::vec3& _pos);

private:
};

class Triangle {
private:
    const Triangle* neighbours[3];
    const Vertex* vertices[3];

public:
    Triangle(Vertex* v1, Vertex* v2, Vertex* v3);

    inline const Vertex* get_vertex_ptr(unsigned int id) const {
        return this->vertices[id];
    }

private:
};

class Geometry {
private:
    GLuint vao;
    GLuint vbo[4];
    unsigned int nr_vertices;

public:
    Geometry();

    void draw();

    inline GLuint get_vao() const {
        return this->vao;
    }

private:
    void load_sphere_coordinates(unsigned int tesselation_level);
};

#endif //_VERTEX_H
