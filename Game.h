#ifndef CATAN_GAME_H
#define CATAN_GAME_H

#include "Board.h"
#include "GameLoader.h"

#include <chrono>
#include <random>

#define defaultFrameRate 45


class Game {
    // Number of development cards
//    std::array<char, N_DEV_CARD> globalDevelopmentCards{N_KNIGHTS, N_ROAD_BUILDING,
//                                                        N_YEAR_OF_PLENTY, N_MONOPOLY, N_VP_CARD};

    Board *board;
    GameLoader *gameLoader;

    std::vector<Player*> players;
    Player *player;
    int frameRate, frameLimit, activePlayer, nPlayers, nDevelopmentCards, primaryVertex, secondaryVertex;
    std::array<Uint8, 3> renderColor{0, 84, 119};
    std::array<Uint8, 3> harborColor{74, 183, 197};
    Uint32 tick;
    bool running, buildingRoad, buildingSettlement, buildingCity, buyingDevCard, diceRolled, robber;
    int diceRoll;

    /*
     * Represents order in which players get their turns. 0 for standard. 1 for setup moving from
     * first to last player, and -1 for setup moving from last player to first
     */
    char direction;

public:
    bool debug;

    Game(GameLoader *g, int r);
    explicit Game(GameLoader *g);

    Player *nextPlayer();

    bool initialize();
    bool initializePlayerTextures();
    bool initializeTTF();
    bool initializeBoard();
    void updateDisplay();
    void updateAndRender();
    void checkFrameTime();

    void roll();
    int drawCard();
    bool buildRoad();
    bool buildSettlement();

    void start();
    void clearActiveState();
    void eventLoop();
    void confirmUserAction();
    void handleMouseDown();
    void handleKeyPress(SDL_KeyboardEvent &keyEvent);
    void close();
};


#endif //CATAN_GAME_H
