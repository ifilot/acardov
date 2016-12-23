/**************************************************************************
 *   tile.h  --  This file is part of Acardov.                            *
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

#ifndef _TILE_H
#define _TILE_H

#include <vector>
#include <glm/glm.hpp>

class Tile {
private:
    glm::vec3 pos;
    std::vector<Tile*> neighbours;
    unsigned int id;
    unsigned int size;
    size_t memory_offset;

public:
    Tile(const glm::vec3 _pos);

    inline const glm::vec3& get_pos() const {
        return this->pos;
    }

    inline size_t get_memory_offset() const {
        return this->memory_offset;
    }

    inline unsigned int get_size() const {
        return this->size;
    }

    inline void add_neighbour(Tile* tile) {
        this->neighbours.push_back(tile);
    }

    inline void set_id(unsigned int _id) {
        this->id = _id;
    }

    inline void set_memory_offset(size_t _memory_offset) {
        this->memory_offset = _memory_offset;
    }

    inline void set_size(unsigned int _size) {
        this->size = _size;
    }

private:
};

#endif //_TILE_H
