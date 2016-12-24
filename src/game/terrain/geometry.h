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

#define GLM_FORCE_RADIANS
#include <glm/gtx/string_cast.hpp>

#include "game/terrain/tile.h"
#include "util/mathfunc.h"

class HalfEdge; // forward declaration

class Face {
private:
    HalfEdge* edge;

public:
    /**
     * @brief       face constructor
     *
     * @param       pointer to half edge
     *
     * @return      face instance
     */
    Face(HalfEdge* _edge);

    /**
     * @brief       get an half edge of the face
     *
     * @return      half edge pointer
     */
    inline HalfEdge* get_edge() {
        return this->edge;
    }

    /**
     * @brief       set the edge of a face
     *
     * @param       half edge pointer
     *
     * @return      void
     */
    inline void set_edge(HalfEdge* _edge) {
        this->edge = _edge;
    }

    /**
     * @brief       get the center coordinate of a face
     *
     * @return      center position
     */
    glm::vec3 get_center() const;

private:
};

class Vertex {
private:
    glm::vec3 pos;
    HalfEdge* edge;
    unsigned int id;
    bool flag_new;

public:
    Vertex(const glm::vec3& _pos);

    inline void set_edge(HalfEdge* _edge) {
        this->edge = _edge;
    }

    inline void set_id(unsigned int _id) {
        this->id = _id;
    }

    inline HalfEdge* get_edge() const {
        return this->edge;
    }

    inline unsigned int get_id() const {
        return this->id;
    }

    inline const glm::vec3& get_pos() const {
        return this->pos;
    }

    inline void reset_new() {
        this->flag_new = false;
    }

    inline void set_new() {
        this->flag_new = true;
    }

    inline bool is_new() const {
        return this->flag_new;
    }

private:
};

class HalfEdge {
private:
    Vertex* vertex;                 //!< the vertex where this half edge is emanating from
    HalfEdge* pair;                 //!< opposite half edge
    Face* face;                     //!< face adjacent to this half edge
    HalfEdge* next;                 //!< next half edge on this face in counter-clockwise fashion
    bool flag_has_splitted;         //!< whether this half-edge has been splitted
    bool flag_new;                  //!< whether this half-edge is new

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

    inline void set_vertex(Vertex* _vertex) {
        this->vertex = _vertex;
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

    inline bool has_splitted() const {
        return this->flag_has_splitted;
    }

    inline void set_splitted() {
        this->flag_has_splitted = true;
    }

    inline void reset_splitted() {
        this->flag_has_splitted = false;
    }

    inline void reset_new() {
        this->flag_new = false;
    }

    inline void set_new() {
        this->flag_new = true;
    }

    inline bool is_new() const {
        return this->flag_new;
    }

private:
};

class Geometry {
private:
    std::vector<std::unique_ptr<Vertex> > vertices;     //!< vector holding all vertices
    std::vector<std::unique_ptr<Face> > faces;          //!< vector holding all faces
    std::vector<std::unique_ptr<HalfEdge> > edges;      //!< vector holding all half edges

public:
    /*
     * @brief   Geometry constructor class
     *
     * @param   Number of subdivision iterations to make the grid
     *
     * @return  Geometry instance
     */
    Geometry(unsigned int nr_subdivisions);

    /*
     * @brief   Load the vertices on the gpu of the half-edge data structure
     *
     * @param   Pointer to vertex attribute object
     * @param   Pointer to vertex buffer object array
     * @param   Pointer where number of indices is written to
     *
     * @return  void
     */
    void load_vertices_gpu(GLuint* vao, GLuint* vbo, unsigned int *nr);

    /*
     * @brief   Load the vertices on the gpu of the dual of the half-edge data structure
     *
     * @param   Pointer to vertex attribute object
     * @param   Pointer to vertex buffer object array
     * @param   Pointer where number of indices is written to
     *
     * @return  void
     */
    void load_vertices_dual_gpu(GLuint* vao, GLuint* vbo, unsigned int *nr);

    /*
     * @brief   Load the lines on the gpu of the dual of the half-edge data structure
     *
     * @param   Pointer to vertex attribute object
     * @param   Pointer to vertex buffer object array
     * @param   Pointer where number of indices is written to
     *
     * @return  void
     */
    void load_lines_dual_gpu(GLuint* vao, GLuint* vbo, unsigned int *nr);

    void load_tiles(std::vector<std::unique_ptr<Tile> > *tiles);

    // deconstructor
    ~Geometry() {}

private:
    void generate_icosahedron();

    void generate_square();

    void generate_tetra_triangle();

    void subdivide();

    void flip_edge(HalfEdge* edge);

    void split_edge(HalfEdge* edge);

    void add_vertex(Vertex* vertex);
};

#endif //_VERTEX_H
