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

Vertex::Vertex(const glm::vec3& _pos) {
    this->pos = _pos;
}

Triangle::Triangle(Vertex* v1, Vertex* v2, Vertex* v3) {
    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;
}

Geometry::Geometry() {
    this->load_sphere_coordinates(2);
}

void Geometry::load_sphere_coordinates(unsigned int tesselation_level) {
    static const float radius = 1.0f;
    static const float sqrt5 = std::sqrt(5.0f);
    static const float phi = (1.0f + sqrt5) * 0.5f; // "golden ratio"
    // ratio of edge length to radius
    static const float ratio = std::sqrt(10.0f + (2.0f * sqrt5)) / (4.0f * phi);
    static const float a = (radius / ratio) * 0.5;
    static const float b = (radius / ratio) / (2.0f * phi);

    // define the vertices of the icosahedron
    std::vector<glm::vec3> vertices{
        glm::vec3( 0,  b, -a),
        glm::vec3( b,  a,  0),
        glm::vec3(-b,  a,  0),
        glm::vec3( 0,  b,  a),
        glm::vec3( 0, -b,  a),
        glm::vec3(-a,  0,  b),
        glm::vec3( 0, -b, -a),
        glm::vec3( a,  0, -b),
        glm::vec3( a,  0,  b),
        glm::vec3(-a,  0, -b),
        glm::vec3( b, -a,  0),
        glm::vec3(-b, -a,  0)
    };

    // define the faces of the icosahedron
    std::vector<unsigned int> triangles{
        0, 1, 2, 3, 2, 1, 3, 4, 5, 3, 8, 4, 0, 6, 7, 0, 9, 6, 4, 10,
        11, 6, 11, 10, 2, 5, 9, 11, 9, 5, 1, 7, 8, 10, 8, 7, 3, 5, 2,
        3, 1, 8, 0, 2, 9, 0, 7, 1, 6, 9, 11, 6, 10, 7, 4, 11, 5, 4, 8, 10
    };

    std::vector<unsigned int> new_triangles;
    for(unsigned int j=0; j<tesselation_level; j++) {
        new_triangles.resize(0);
        unsigned int size = triangles.size();
        for(unsigned int i=0; i<size; i+=3) {
            // calculate center of the vertices of each triangle
            const glm::vec3 center1 = glm::normalize((vertices[triangles[i]] + vertices[triangles[i+1]]) / 2.0f);
            const glm::vec3 center2 = glm::normalize((vertices[triangles[i]] + vertices[triangles[i+2]]) / 2.0f);
            const glm::vec3 center3 = glm::normalize((vertices[triangles[i+1]] + vertices[triangles[i+2]]) / 2.0f);

            // place the new vertices in the vector
            vertices.push_back(center1); // 0-1
            const unsigned int a = vertices.size() - 1;
            vertices.push_back(center2); // 0-2
            const unsigned int b = vertices.size() - 1;
            vertices.push_back(center3); // 1-2
            const unsigned int c = vertices.size() - 1;

            // add the new triangles
            new_triangles.push_back(triangles[i]);
            new_triangles.push_back(a);
            new_triangles.push_back(b);

            new_triangles.push_back(triangles[i+1]);
            new_triangles.push_back(c);
            new_triangles.push_back(a);

            new_triangles.push_back(triangles[i+2]);
            new_triangles.push_back(b);
            new_triangles.push_back(c);

            new_triangles.push_back(a);
            new_triangles.push_back(c);
            new_triangles.push_back(b);
        }
        // set the new triangles on the old triangles
        triangles = new_triangles;
    }

    // create seperate triangles, normals and colors
    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> cols;
    std::vector<unsigned int> indices;

    boost::random::mt19937 rng;
    boost::random::uniform_real_distribution<> dist(0.0, 1.0);

    for(unsigned int i=0; i<triangles.size(); i+=3) {
        verts.push_back(vertices[triangles[i]]);
        verts.push_back(vertices[triangles[i+1]]);
        verts.push_back(vertices[triangles[i+2]]);

        const glm::vec3 col = glm::vec3((float)dist(rng), (float)dist(rng), (float)dist(rng));
        cols.push_back(col);
        cols.push_back(col);
        cols.push_back(col);

        indices.push_back(i);
        indices.push_back(i+1);
        indices.push_back(i+2);
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

void Geometry::draw() {
    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, this->nr_vertices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
