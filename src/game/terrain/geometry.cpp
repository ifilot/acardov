/**************************************************************************
 *   vertex.cpp  --  This file is part of Acardov.                        *
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

#include "geometry.h"

Face::Face(HalfEdge* _edge) {
    this->edge = _edge;
}

Vertex::Vertex(const glm::vec3& _pos) {
    this->pos = _pos;
}

HalfEdge::HalfEdge(Vertex* _vertex) {
    this->vertex = _vertex;
}

Geometry::Geometry() {
    this->generate_icosahedron();
    this->load_vertices_gpu();
}

void Geometry::generate_icosahedron() {
    static const float radius = 1.0f;
    static const float sqrt5 = std::sqrt(5.0f);
    static const float phi = (1.0f + sqrt5) * 0.5f; // "golden ratio"
    // ratio of edge length to radius
    static const float ratio = std::sqrt(10.0f + (2.0f * sqrt5)) / (4.0f * phi);
    static const float a = (radius / ratio) * 0.5;
    static const float b = (radius / ratio) / (2.0f * phi);

    // define the vertices of the icosahedron
    std::vector<Vertex*> temp_vertices;
    temp_vertices.emplace_back(new Vertex(glm::vec3( 0,  b, -a)));
    temp_vertices.emplace_back(new Vertex(glm::vec3( b,  a,  0)));
    temp_vertices.emplace_back(new Vertex(glm::vec3(-b,  a,  0)));
    temp_vertices.emplace_back(new Vertex(glm::vec3( 0,  b,  a)));
    temp_vertices.emplace_back(new Vertex(glm::vec3( 0, -b,  a)));
    temp_vertices.emplace_back(new Vertex(glm::vec3(-a,  0,  b)));
    temp_vertices.emplace_back(new Vertex(glm::vec3( 0, -b, -a)));
    temp_vertices.emplace_back(new Vertex(glm::vec3( a,  0, -b)));
    temp_vertices.emplace_back(new Vertex(glm::vec3( a,  0,  b)));
    temp_vertices.emplace_back(new Vertex(glm::vec3(-a,  0, -b)));
    temp_vertices.emplace_back(new Vertex(glm::vec3( b, -a,  0)));
    temp_vertices.emplace_back(new Vertex(glm::vec3(-b, -a,  0)));

    std::vector<unsigned int> triangles{
        0, 1, 2, 3, 2, 1, 3, 4, 5, 3, 8, 4, 0, 6, 7, 0, 9, 6, 4, 10,
        11, 6, 11, 10, 2, 5, 9, 11, 9, 5, 1, 7, 8, 10, 8, 7, 3, 5, 2,
        3, 1, 8, 0, 2, 9, 0, 7, 1, 6, 9, 11, 6, 10, 7, 4, 11, 5, 4, 8, 10
    };

    // make temporary auxiliary object to store edge pairs in
    std::map<std::pair<unsigned int, unsigned int>, HalfEdge*> temp_edges;

    for(unsigned int i=0; i<triangles.size(); i+=3) {
        const unsigned int id1 = triangles[i];
        const unsigned int id2 = triangles[i+1];
        const unsigned int id3 = triangles[i+2];

        HalfEdge* edge1 = new HalfEdge(temp_vertices[id1]);
        HalfEdge* edge2 = new HalfEdge(temp_vertices[id2]);
        HalfEdge* edge3 = new HalfEdge(temp_vertices[id3]);
        this->add_face(edge1);

        this->vertices.emplace_back(temp_vertices[id1]);
        this->vertices.emplace_back(temp_vertices[id2]);
        this->vertices.emplace_back(temp_vertices[id3]);

        edge1->set_next(edge2)->set_face(this->faces.back().get());
        edge2->set_next(edge3)->set_face(this->faces.back().get());
        edge3->set_next(edge1)->set_face(this->faces.back().get());

        // add the new edges to the Edge structure
        temp_edges.emplace(std::pair<unsigned int, unsigned int>(id1, id2), edge1);
        temp_edges.emplace(std::pair<unsigned int, unsigned int>(id2, id3), edge2);
        temp_edges.emplace(std::pair<unsigned int, unsigned int>(id3, id1), edge3);

        // check if opposite half edge already exists in temp_edges
        auto it = temp_edges.find(std::pair<unsigned int, unsigned int>(id2, id1));
        if(it != temp_edges.end()) {
            it->second->set_pair(edge1);
            edge1->set_pair(it->second);
        }
        it = temp_edges.find(std::pair<unsigned int, unsigned int>(id3, id2));
        if(it != temp_edges.end()) {
            it->second->set_pair(edge2);
            edge2->set_pair(it->second);
        }
        it = temp_edges.find(std::pair<unsigned int, unsigned int>(id1, id3));
        if(it != temp_edges.end()) {
            it->second->set_pair(edge3);
            edge3->set_pair(it->second);
        }

        this->edges.emplace_back(edge1);
        this->edges.emplace_back(edge2);
        this->edges.emplace_back(edge3);
    }
}

void Geometry::add_face(HalfEdge* _edge) {
    this->faces.emplace_back(new Face(_edge));
}

void Geometry::draw() {
    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, this->nr_vertices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Geometry::load_vertices_gpu() {
    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> cols;
    std::vector<unsigned int> indices;

    boost::random::mt19937 rng;
    boost::random::uniform_real_distribution<> dist(0.0, 1.0);

    for(auto&& face: faces) {
        HalfEdge* edge = face->get_edge();
        const glm::vec3 col = glm::vec3((float)dist(rng), (float)dist(rng), (float)dist(rng));
        do {
            verts.push_back(edge->get_vertex()->get_pos());
            cols.push_back(col);
            edge = edge->get_next();
        } while (edge != face->get_edge());

        for(unsigned int i=0; i<3; i++) {
            indices.push_back(indices.size());
        }
    }
    this->nr_vertices = indices.size();

    // load vao and vbo
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);
    glGenBuffers(4, this->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * 3 * sizeof(float), &verts[0][0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * 3 * sizeof(float), &cols[0][0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, cols.size() * 3 * sizeof(float), &cols[0][0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Geometry::subdivide() {
    // create new objects to hold all new vertices, faces and half edges
    std::vector<std::unique_ptr<Vertex> > new_vertices;
    std::vector<std::unique_ptr<Face> > new_faces;
    std::vector<std::unique_ptr<HalfEdge> > new_edges;

    std::map<std::pair<unsigned int, unsigned int>, HalfEdge*> splitted_edges;
    std::map<std::pair<size_t, size_t>, HalfEdge*> temp_edges;

    // loop over all vertices
    for(auto&& vert: vertices) {
        // create new vertex pointer
        Vertex* current_vertex = new Vertex(vert->get_pos());
        new_vertices.emplace_back(current_vertex);
        const unsigned int id1 = new_vertices.size();

        // loop over all edges
        HalfEdge* edge = vert->get_edge();
        Vertex* prev_vertex = NULL;
        do {
            // get opposite vector
            auto vert2 = edge->get_vertex();

            // create a new vertex at the intersection of these two vertices
            //center_vertex = new Vertex(glm::normalize((current_vertex->get_pos() + vert2->get_pos()) / 2.0f));
            //prev_vertex = vertex;

            // we have three vertices, so we can make a new face and all half edges!
            if(prev_vertex) {

            }

            edge = edge->get_pair()->get_next();
        } while(edge != vert->get_edge());
    }
}
