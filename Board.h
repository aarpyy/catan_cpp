#ifndef BOARD_H
#define BOARD_H

#include "Player.h"

#include <map>

class Board {

    TTF_Font *SansLight, *SansMed, *SansBold;
    int fontSize, buttonBuffer, minWidth, minHeight;
    double menuFraction;

    std::vector<SDL_Texture*> textures{};

    SDL_Texture *endTurnTexture, *buildRoadTexture, *buildSettlementTexture,
    *buildCityTexture, *buyDevelopmentCardTexture, *confirmTexture, *cityTexture,
    *settlementTexture, *building, *rollDice, *resourceMenu, *robberTexture, *vpTexture, *portsTexture;

public:

    int w, h;
    SDL_Renderer *renderer;
    SDL_Window *window;

    SDL_Texture *devCardBack;

    SDL_Rect *endTurnButton, *buildOptionsRect, *buildRoadRect,
    *buildSettlementRect, *buildCityRect, *buyDevelopmentCardRect, *confirmRect, *buildingRect,
    *rollDiceRect, *resourceMenuRect, *vpRect, *portsRect, *robberRect;

    /*
     * explicit keyword prevents the following instance:
     *
     * if a function takes argument type Board (ex. foo (Board b) {})
     * and is given an int * instead of a Board, C++ will implicitly convert
     * int * board into a Board by sending it into the Board constructor. Adding
     * explicit keyword makes sure that a Board is only created when
     * Board() explicitly is called.
     */

    // SDL textures to be displayed for each tile
    std::map<char, SDL_Texture*> tileTextures{};
    // Texture for each chit number (2 - 12)
    std::map<char, SDL_Texture*> chitTextures{};
    std::map<char, SDL_Texture*> diceRolls{};

    std::array<Tile*, N_TILES> tiles{};
    std::array<Vertex*, N_VERTICES> vertices{};
    std::array<int, N_RESOURCE> bitBoard{};
    std::array<char, N_PORTS> ports{};
    std::array<SDL_Rect*, N_PORTS> portsRects{};

    Board();
    explicit Board(std::array<int, N_RESOURCE> &bit_board);
    void *operator new(size_t size);
    void operator delete(void *ptr);

    static std::array<int, 6> & randBoard();

    bool findTextures();
    bool findFonts();
    void initializePorts();
    void initializeMenu();
    void initializeButtons();
    bool initializeWindowAndRenderer();
    bool initialize();

    void resizeTiles();
    void resize(int windowWidth, int windowHeight);
    void resizeMenu() const;
    void resizeButtons() const;
    void resizeVertices() const;

    void drawMenu();
    void drawResources(Player &player);
    void drawButtons();
    void drawRoll(int roll);
    void drawActiveBuilding(SDL_Texture *texture) const;
    void drawAdjacent(Vertex &vertex, Player &player);
    void drawRoad(SDL_Point &first, SDL_Point &second, Player *player) const;
    void drawPlayerProperties(const std::vector<Player*>& players);
    void draw();

    void freeBoard();

    std::array<int, N_RESOURCE> getBitBoard() const;

};


#endif //BOARD_H
