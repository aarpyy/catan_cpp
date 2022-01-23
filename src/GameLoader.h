#ifndef GAMELOADER_H
#define GAMELOADER_H

#include <iostream>
#include <fstream>

#include "Board.h"

/*
 * GameLoader class can take in either a filename to load from or a board to load from.
 * If it does not receive a board it makes one. If it does not receive a savefile it saves to
 * its input file.
 */

class GameLoader {

public:

    std::string save_file;
    std::vector<Player*> players;
    std::array<int, N_RESOURCE> board{};

    GameLoader();

    bool runPlayerSelect();

    void randBoard();

    void save() const;
    void quit() const;
};


#endif //GAMELOADER_H
