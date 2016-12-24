/**************************************************************************
 *   piece.h  --  This file is part of Acardov.                           *
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

#ifndef _PIECE_H
#define _PIECE_H

#include "game/terrain/tile.h"
#include "core/shader.h"
#include "models/mesh.h"
#include "util/mathfunc.h"
#include "core/light.h"

class Piece {
private:
    std::vector<Mesh*> meshes;
    const Tile* tile;
    Shader* shader;
    std::vector<glm::vec3> colors;

public:
    Piece(Shader* _shader);

    void draw();

    inline void add_mesh(Mesh* mesh, const glm::vec3& color) {
        this->meshes.push_back(mesh);
        this->colors.push_back(color);
    }

    inline void set_tile(const Tile* _tile) {
        this->tile = _tile;
    }

private:
};

#endif // PIECE_H
