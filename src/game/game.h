/**************************************************************************
 *   game.h  --  This file is part of Acardov.                            *
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

#ifndef _GAME_H
#define _GAME_H

#include "game/terrain/planet.h"
#include "models/mesh.h"
#include "core/shader.h"
#include "core/light.h"
#include "game/piece.h"

class Game {
private:
    std::vector<std::unique_ptr<Mesh> > meshes;
    std::unique_ptr<Shader> shader;
    std::vector<Piece> pieces;

public:
    /**
     * @fn          get
     *
     * @brief       get a reference to the game
     *
     * @return      reference to the game object (singleton pattern)
     */
    static Game& get() {
        static Game gameinstance;
        return gameinstance;
    }

    void draw();

    void update(double dt);

    void add_piece(unsigned int tile_id, unsigned int type);

private:
    Game();

    // Singleton pattern
    Game(Game const&)          = delete;
    void operator=(Game const&)  = delete;
};

#endif //_GAME_H
