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

glm::vec3 Face::get_center() const {
    HalfEdge* edge = this->edge;
    glm::vec3 center = glm::vec3(0,0,0);
    unsigned int count = 0;
    do {
        count++;
        center += edge->get_vertex()->get_pos();
        edge = edge->get_next();
    } while (edge != this->edge);

     return center / (float)count;
}

Vertex::Vertex(const glm::vec3& _pos) {
    this->pos = _pos;
    this->flag_new = false;
}

HalfEdge::HalfEdge(Vertex* _vertex) {
    this->vertex = _vertex;

    this->pair = NULL;
    this->next = NULL;
    this->face = NULL;
    this->flag_has_splitted = false;
    this->flag_new = false;
}

Geometry::Geometry(unsigned int nr_subdivisions) {
    this->generate_icosahedron();
    for(unsigned int i=0; i<nr_subdivisions; i++) {
        this->subdivide();
    }
    this->load_vertices_dual_gpu();
}

void Geometry::generate_square() {
    /****************#
    #                #
    #   a---------d  #
    #   |\        |  #
    #   | \       |  #
    #   |  \   B  |  #
    #   |   \     |  #
    #   |    \    |  #
    #   |     \   |  #
    #   |  A   \  |  #
    #   |       \ |  #
    #   |        \|  #
    #   b---------c  #
    #                #
    *****************/

    // create vertices
    Vertex* a = new Vertex(glm::vec3(-1,1,0));
    Vertex* b = new Vertex(glm::vec3(-1,-1,0));
    Vertex* c = new Vertex(glm::vec3(1,-1,0));
    Vertex* d = new Vertex(glm::vec3(1,1,0));

    // create half edges
    HalfEdge* ab = new HalfEdge(a);
    HalfEdge* ba = new HalfEdge(b);
    HalfEdge* bc = new HalfEdge(b);
    HalfEdge* cb = new HalfEdge(c);
    HalfEdge* cd = new HalfEdge(c);
    HalfEdge* dc = new HalfEdge(d);
    HalfEdge* da = new HalfEdge(d);
    HalfEdge* ad = new HalfEdge(a);
    HalfEdge* ac = new HalfEdge(a);
    HalfEdge* ca = new HalfEdge(c);

    // create faces
    Face* A = new Face(ca);
    Face* B = new Face(ac);

    // connect pointers
    ab->set_next(bc)->set_pair(ba)->set_face(A);
    bc->set_next(ca)->set_pair(cb)->set_face(A);
    ca->set_next(ab)->set_pair(ac)->set_face(A);

    ac->set_next(cd)->set_pair(ca)->set_face(B);
    cd->set_next(da)->set_pair(dc)->set_face(B);
    da->set_next(ac)->set_pair(ad)->set_face(B);

    // add vertices, edges and faces to vectors
    this->vertices.emplace_back(a);
    this->vertices.emplace_back(b);
    this->vertices.emplace_back(c);
    this->vertices.emplace_back(d);

    this->edges.emplace_back(ab);
    this->edges.emplace_back(ba);
    this->edges.emplace_back(bc);
    this->edges.emplace_back(cb);
    this->edges.emplace_back(cd);
    this->edges.emplace_back(dc);
    this->edges.emplace_back(da);
    this->edges.emplace_back(ad);
    this->edges.emplace_back(ac);
    this->edges.emplace_back(ca);

    this->faces.emplace_back(A);
    this->faces.emplace_back(B);
}

void Geometry::generate_tetra_triangle() {
    // create vertices
    Vertex* a = new Vertex(glm::vec3(-1,1,0));
    Vertex* b = new Vertex(glm::vec3(0,-std::sqrt(2.0f),0));
    Vertex* c = new Vertex(glm::vec3(1,1,0));
    Vertex* d = new Vertex((a->get_pos() + b->get_pos()) / 2.0f);
    Vertex* e = new Vertex((b->get_pos() + c->get_pos()) / 2.0f);
    Vertex* f = new Vertex((a->get_pos() + c->get_pos()) / 2.0f);

    // create half edges
    // triangle A
    HalfEdge* ad = new HalfEdge(a);
    HalfEdge* da = new HalfEdge(d);
    HalfEdge* df = new HalfEdge(d);
    HalfEdge* fd = new HalfEdge(f);
    HalfEdge* fa = new HalfEdge(f);
    HalfEdge* af = new HalfEdge(a);

    // triangle B
    HalfEdge* db = new HalfEdge(d);
    HalfEdge* bd = new HalfEdge(b);
    HalfEdge* be = new HalfEdge(b);
    HalfEdge* eb = new HalfEdge(e);
    HalfEdge* ed = new HalfEdge(e);
    HalfEdge* de = new HalfEdge(d);

    // triangle C
    HalfEdge* ec = new HalfEdge(e);
    HalfEdge* ce = new HalfEdge(c);
    HalfEdge* cf = new HalfEdge(c);
    HalfEdge* fc = new HalfEdge(f);
    HalfEdge* fe = new HalfEdge(f);
    HalfEdge* ef = new HalfEdge(e);

    // create faces
    Face* A = new Face(df);
    Face* B = new Face(ed);
    Face* C = new Face(fe);
    Face* D = new Face(ef);

    // connect pointers
    ad->set_next(df)->set_pair(da)->set_face(A);
    df->set_next(fa)->set_pair(fd)->set_face(A);
    fa->set_next(ad)->set_pair(af)->set_face(A);
    da->set_pair(ad);
    fd->set_pair(df);
    af->set_pair(fa);

    db->set_next(be)->set_pair(bd)->set_face(B);
    be->set_next(ed)->set_pair(eb)->set_face(B);
    ed->set_next(db)->set_pair(de)->set_face(B);
    bd->set_pair(db);
    eb->set_pair(be);
    de->set_pair(ed);

    ec->set_next(cf)->set_pair(ce)->set_face(C);
    cf->set_next(fe)->set_pair(fc)->set_face(C);
    fe->set_next(ec)->set_pair(ef)->set_face(C);
    ce->set_pair(ec);
    fc->set_pair(cf);
    ef->set_pair(fe);

    de->set_next(ef)->set_face(D);
    ef->set_next(fd)->set_face(D);
    fd->set_next(de)->set_face(D);

    // add vertices, edges and faces to vectors
    this->vertices.emplace_back(a);
    this->vertices.emplace_back(b);
    this->vertices.emplace_back(c);
    this->vertices.emplace_back(d);
    this->vertices.emplace_back(e);
    this->vertices.emplace_back(f);

    this->edges.emplace_back(ad);
    this->edges.emplace_back(da);
    this->edges.emplace_back(df);
    this->edges.emplace_back(fd);
    this->edges.emplace_back(fa);
    this->edges.emplace_back(af);

    // triangle B
    this->edges.emplace_back(db);
    this->edges.emplace_back(bd);
    this->edges.emplace_back(be);
    this->edges.emplace_back(eb);
    this->edges.emplace_back(ed);
    this->edges.emplace_back(de);

    // triangle C
    this->edges.emplace_back(ec);
    this->edges.emplace_back(ce);
    this->edges.emplace_back(cf);
    this->edges.emplace_back(fc);
    this->edges.emplace_back(fe);
    this->edges.emplace_back(ef);

    this->faces.emplace_back(A);
    this->faces.emplace_back(B);
    this->faces.emplace_back(C);
    this->faces.emplace_back(D);
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
    this->vertices.emplace_back(new Vertex(glm::vec3( 0,  b, -a)));
    this->vertices.emplace_back(new Vertex(glm::vec3( b,  a,  0)));
    this->vertices.emplace_back(new Vertex(glm::vec3(-b,  a,  0)));
    this->vertices.emplace_back(new Vertex(glm::vec3( 0,  b,  a)));
    this->vertices.emplace_back(new Vertex(glm::vec3( 0, -b,  a)));
    this->vertices.emplace_back(new Vertex(glm::vec3(-a,  0,  b)));
    this->vertices.emplace_back(new Vertex(glm::vec3( 0, -b, -a)));
    this->vertices.emplace_back(new Vertex(glm::vec3( a,  0, -b)));
    this->vertices.emplace_back(new Vertex(glm::vec3( a,  0,  b)));
    this->vertices.emplace_back(new Vertex(glm::vec3(-a,  0, -b)));
    this->vertices.emplace_back(new Vertex(glm::vec3( b, -a,  0)));
    this->vertices.emplace_back(new Vertex(glm::vec3(-b, -a,  0)));

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

        HalfEdge* edge1 = new HalfEdge(this->vertices[id1].get());
        HalfEdge* edge2 = new HalfEdge(this->vertices[id2].get());
        HalfEdge* edge3 = new HalfEdge(this->vertices[id3].get());
        this->add_face(edge1);

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

void Geometry::load_vertices_dual_gpu() {
    std::vector<glm::vec3> verts;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;

    for(auto&& vertex: vertices) {
        HalfEdge* edge = vertex->get_edge();

        unsigned int count = 0;
        glm::vec3 prev_pos = glm::vec3(0,0,0);

        do {
            count++;

            glm::vec3 pos = edge->get_face()->get_center();

            // add a vector of this face to the list
            if(count > 1) {
                verts.push_back(vertex->get_pos());
                verts.push_back(prev_pos);
                verts.push_back(pos);
            }

            // store current position for next iteration
            prev_pos = pos;
            edge = edge->get_pair()->get_next();
        } while (edge != vertex->get_edge());

        // add the closing triangle
        verts.push_back(vertex->get_pos());
        verts.push_back(prev_pos);
        verts.push_back(vertex->get_edge()->get_face()->get_center());

        for(unsigned int i=0; i<count * 3; i++) {
            indices.push_back(indices.size());
        }

        if(count == 5) {
            for(unsigned int i=0; i<count * 3; i++) {
                uvs.push_back(glm::vec2(0,0));
            }
            continue;
        }

        std::vector<glm::vec2> vert_hexagon = {
            glm::vec2(0.48f, 0.98f),
            glm::vec2(0.02f, 1.0f - 35.0f/140.0f - 0.02f),
            glm::vec2(0.02f, 1.0f - 104.0f/140.0f + 0.02f),
            glm::vec2(0.48f, 0.02f),
            glm::vec2(0.98f, 1.0f - 104.0f/140.0f + 0.02f),
            glm::vec2(0.98f, 1.0f - 35.0f/140.0f - 0.02f)
        };

        for(unsigned int i=0; i<count; i++) {
            uvs.push_back(glm::vec2(0.5f,0.5f));
            uvs.push_back(vert_hexagon[(i)%6]);
            uvs.push_back(vert_hexagon[(i+1)%6]);
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
    glBufferData(GL_ARRAY_BUFFER, verts.size() * 3 * sizeof(float), &verts[0][0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * 3 * sizeof(float), &uvs[0][0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Geometry::subdivide() {
    // reset all edges and vertices
    for(auto&& edge: edges) {
        edge->reset_new();
        edge->reset_splitted();
    }

    for(auto&& vertex: vertices) {
        vertex->reset_new();
    }

    // loop over all edges and split them
    const unsigned int nr_edges = this->edges.size();
    for(unsigned int i=0; i<nr_edges; i++) {
        this->split_edge(this->edges[i].get());
    }

    // loop over all edges and flip them if they connect a new with an old vertex
    for(auto&& edge: edges) {
        Vertex* a = edge->get_vertex();

        // if an edge has no pair just continue (boundary edge)
        if(edge->get_pair() == NULL) {
            continue;
        }
        Vertex* b = edge->get_pair()->get_vertex();

        if(edge->is_new()) {
            if((a->is_new() && !b->is_new()) ||
               (!a->is_new() && b->is_new())) {
                this->flip_edge(edge.get());
            }
        }
    }
}

void Geometry::flip_edge(HalfEdge* edge) {
    /*************************
    #    c             c     #
    #   /|\           / \    #
    #  / | \         / B \   #
    # a A|B d  ==>  a-----d  #
    #  \ | /         \ A /   #
    #   \|/           \ /    #
    #    b             b     #
    #                        #
    *************************/

    // get edges
    HalfEdge* bc = edge;
    HalfEdge* cb = edge->get_pair();

    // check if this is a boundary edge, if so, return
    if(bc->get_next() == NULL || cb->get_next() == NULL) {
        return;
    }

    HalfEdge* bd = cb->get_next();
    HalfEdge* db = bd->get_pair();
    HalfEdge* ca = bc->get_next();
    HalfEdge* ac = ca->get_pair();
    HalfEdge* ab = ca->get_next();
    HalfEdge* ba = ab->get_pair();
    HalfEdge* dc = bd->get_next();
    HalfEdge* cd = dc->get_pair();

    // get faces
    Face* A = bc->get_face();
    Face* B = cb->get_face();

    // get vertices
    Vertex* a = ab->get_vertex();
    Vertex* b = bc->get_vertex();
    Vertex* c = cb->get_vertex();
    Vertex* d = dc->get_vertex();

    // move vertices
    HalfEdge* ad = cb;
    HalfEdge* da = bc;

    // reconnect pointers half edges
    ad->set_next(dc)->set_face(B);
    dc->set_next(ca)->set_face(B);
    ca->set_next(ad)->set_face(B);

    da->set_next(ab)->set_face(A);
    ab->set_next(bd)->set_face(A);
    bd->set_next(da)->set_face(A);

    // set new vertices the new edges emanate from
    ad->set_vertex(a);
    da->set_vertex(d);

    // reconnect pointers faces
    A->set_edge(da);
    B->set_edge(ad);

    // reconnect pointers vertices
    a->set_edge(ad);
    b->set_edge(bd);
    c->set_edge(ca);
    d->set_edge(da);
}

void Geometry::split_edge(HalfEdge* edge) {
    // if this edge has already been splitted in a previous operation,
    // just return this function
    if(edge->has_splitted()) {
        return;
    }

    // Figure: Graphical representation of edge splitting
    /*************************
    #    c             c     #
    #   /|\           /|\    #
    #  / | \         /A|B\   #
    # a A|B d  ==>  a--m--d  #
    #  \ | /         \C|D/   #
    #   \|/           \|/    #
    #    b             b     #
    #                        #
    *************************/
    // get edges
    HalfEdge* bc = edge;
    HalfEdge* cb = edge->get_pair();

    bool has_A = bc->get_face() ? true : false;
    bool has_B = cb->get_face() ? true : false;

    HalfEdge *ca = NULL, *ac = NULL, *ab = NULL, *ba = NULL, *bd = NULL, *db = NULL;
    HalfEdge *dc = NULL, *cd = NULL, *am = NULL, *ma = NULL, *md = NULL, *dm = NULL;
    Vertex *a = NULL, *b = NULL, *c = NULL, *d = NULL;
    Face *A = NULL, *B = NULL, *C = NULL, *D = NULL;

    if(has_A) {
        ca = bc->get_next();
        ac = ca->get_pair();
        ab = ca->get_next();
        ba = ab->get_pair();

        a = ab->get_vertex();
        A = bc->get_face();
    }

    if(has_B) {
        bd = cb->get_next();
        db = bd->get_pair();
        dc = bd->get_next();
        cd = dc->get_pair();

        d = dc->get_vertex();
        B = cb->get_face();
    }

    // get vertices
    b = bc->get_vertex();
    c = cb->get_vertex();

    // create new vertex
    Vertex* m = new Vertex(glm::normalize((b->get_pos() + c->get_pos())/2.0f));
    m->set_new();

    // introduce new vertices
    HalfEdge* mc = new HalfEdge(m);
    HalfEdge* cm = new HalfEdge(c);

    if(has_A) {
        am = new HalfEdge(a);
        ma = new HalfEdge(m);
        am->set_new();
        ma->set_new();
    }

    if(has_B) {
        md = new HalfEdge(m);
        md->set_new();
        dm = new HalfEdge(d);
        dm->set_new();
    }

    // reset vertices bc and cb to bm and mb
    HalfEdge* bm = bc;
    HalfEdge* mb = cb;
    mb->set_vertex(m); // set new point where mb is originating from

    // set the new Vertex* next
    // Face A
    if(has_A) {
        mc->set_next(ca)->set_pair(cm);
        ca->set_next(am)->set_pair(ac);
        am->set_next(mc)->set_pair(ma);
    }

    // Face B
    if(has_B) {
        md->set_next(dc)->set_pair(dm);
        dc->set_next(cm)->set_pair(cd);
        cm->set_next(md)->set_pair(mc);
    }

    // Face C
    if(has_A) {
        ma->set_next(ab)->set_pair(am);
        ab->set_next(bm)->set_pair(ba);
        bm->set_next(ma)->set_pair(mb);
    }

    // Face D
    if(has_B) {
        bd->set_next(dm)->set_pair(db);
        dm->set_next(mb)->set_pair(md);
        mb->set_next(bd)->set_pair(bm);
    }

    // set faces
    if(has_A) {
        A->set_edge(mc);
        C = new Face(bm);
    }

    if(has_B) {
        B->set_edge(cm);
        D = new Face(mb);
    }

    /*************************
    #    c             c     #
    #   /|\           /|\    #
    #  / | \         /A|B\   #
    # a A|B d  ==>  a--m--d  #
    #  \ | /         \C|D/   #
    #   \|/           \|/    #
    #    b             b     #
    #                        #
    *************************/

    // connect faces to half edges
    if(has_A) {
        am->set_face(A);
        mc->set_face(A);
        ca->set_face(A);
    }

    if(has_B) {
        md->set_face(B);
        dc->set_face(B);
        cm->set_face(B);
    }

    if(has_A) {
        ma->set_face(C);
        ab->set_face(C);
        bm->set_face(C);
    }

    if(has_B) {
        bd->set_face(D);
        dm->set_face(D);
        mb->set_face(D);
    }

    // update the vertices
    if(has_A) {
        a->set_edge(ab);
    }
    b->set_edge(bd);
    if(has_B) {
        d->set_edge(dc);
    }
    c->set_edge(ca);
    m->set_edge(mc);

    // finally place all new vertices, halfedges and faces in the vectors
    this->vertices.emplace_back(m);

    // remember; bm and mb are made using bc and cb
    if(has_A) {
        this->edges.emplace_back(am);
        this->edges.emplace_back(ma);
        this->faces.emplace_back(C);
    }
    if(has_B) {
        this->edges.emplace_back(md);
        this->edges.emplace_back(dm);
        this->faces.emplace_back(D);
    }
    this->edges.emplace_back(mc);
    this->edges.emplace_back(cm);

    // designate that this edge and it's pair have been splitted
    bc->set_splitted();
    cb->set_splitted();
}

Geometry::~Geometry() {
    glBindVertexArray(0);
    glDeleteBuffers(4, this->vbo);
    glDeleteVertexArrays(1, &this->vao);
}
