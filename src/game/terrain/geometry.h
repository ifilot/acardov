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
#include <memory>
#include <map>
#include <unordered_set>
#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>

class HalfEdge; // forward declaration

class Face {
private:
    HalfEdge* edge;

public:
    Face(HalfEdge* _edge);

    inline HalfEdge* get_edge() {
        return this->edge;
    }

private:
};

class Vertex {
private:
    glm::vec3 pos;
    HalfEdge* edge;

public:
    Vertex(const glm::vec3& _pos);

    inline void set_edge(HalfEdge* _edge) {
        this->edge = _edge;
    }

    inline HalfEdge* get_edge() const {
        return this->edge;
    }

    inline const glm::vec3& get_pos() const {
        return this->pos;
    }

private:
};

class HalfEdge {
private:
    Vertex* vertex;
    HalfEdge* pair;
    Face* face;
    HalfEdge* next;

public:
    HalfEdge(Vertex* _vertex);

    inline HalfEdge* set_pair(HalfEdge* _pair) {
        this->pair = _pair;
        return this;
    }

    inline HalfEdge* set_face(Face* _face) {
        this->face = _face;
        return this;
    }

    inline HalfEdge* set_next(HalfEdge* _next) {
        this->next = _next;
        return this;
    }

    inline Vertex* get_vertex() {
        return this->vertex;
    }

    inline HalfEdge* get_pair() {
        return this->pair;
    }

    inline Face* get_face() {
        return this->face;
    }

    inline HalfEdge* get_next() {
        return this->next;
    }

private:
};

class Geometry {
private:
    GLuint vao;
    GLuint vbo[4];
    unsigned int nr_vertices;

    std::vector<std::unique_ptr<Vertex> > vertices;
    std::vector<std::unique_ptr<Face> > faces;
    std::vector<std::unique_ptr<HalfEdge> > edges;

public:
    Geometry();

    void draw();

    inline GLuint get_vao() const {
        return this->vao;
    }

private:
    void generate_icosahedron();

    void subdivide();

    void add_face(HalfEdge* _edge);

    void load_vertices_gpu();
};

#endif //_VERTEX_H
