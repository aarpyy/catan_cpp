#include "Game.h"

#include <algorithm>


Game::Game(GameLoader *g, int r) {
    board = g->board;
    SDL_Log("Board is null: %d", (board == nullptr));
    gameLoader = g;
    frameRate = r;
    frameLimit = 1000 / frameRate;
    nDevelopmentCards = N_DEV_CARD;
    activePlayer = primaryVertex = secondaryVertex = -1;
    nPlayers = diceRoll = 0;
    player = nullptr;
    tick = SDL_GetTicks();
    running = debug = buildingRoad = buildingSettlement = buildingCity =
            buyingDevCard = diceRolled = robber = false;
    direction = 0;

    SDL_Log("Constructed everything so far");

    for (Player *p : g->players) {
        players.push_back(p);
        nPlayers++;
    }
}

Game::Game(GameLoader *g) : Game(g, defaultFrameRate) { }

Player *Game::nextPlayer() {
    Player *next;

    // If normal direction, just cycle through players
    if (!direction) {
        next = players[activePlayer];
        activePlayer = (activePlayer + 1) % nPlayers;
    }
    // If setup going forward, go until hit last player, then switch directions
    else if (direction == 1) {
        if (activePlayer == nPlayers) {
            activePlayer--;
            next = players[activePlayer];
            direction = -1;
        } else {
            next = players[activePlayer];
            activePlayer++;
        }
    }
    // If setup going backwards, go until first player, then switch to normal direction
    else if (direction == -1) {
        if (!activePlayer) {
            direction = 0;
            next = players[activePlayer];
            activePlayer++;
        } else {
            activePlayer--;
            next = players[activePlayer];
        }

    }
    SDL_SetCursor(next->cursor);
    return next;
}

bool Game::initialize() {
    return board->initializeWindowAndRenderer();
}

bool Game::initializePlayerTextures() {
    for (Player* p : players) {
        if (!p->loadTextures(board->renderer)) return false;
    }
    return true;
}

bool Game::initializeTTF() {
    if (!TTF_Init()) {
        return board->findFonts();
    } else {
        return false;
    }
}

bool Game::initializeBoard() {
    if (!board->initialize()) {
        return false;
    } else {
        board->initializeButtons();
        updateDisplay();
        return true;
    }
}

void Game::updateDisplay() {
    // Resets pixel data information from the renderer using the draw color
    SDL_SetRenderDrawColor(board->renderer,
                           harborColor[0],
                           harborColor[1],
                           harborColor[2],
                           SDL_ALPHA_OPAQUE);

    // Sets the background color of the renderer to a sea-like color
    SDL_RenderClear(board->renderer);

    // Draw new board
    board->draw();
    board->drawPlayerProperties(players);
    if (player != nullptr) board->drawResources(*player);

    if (primaryVertex != -1) {
        Vertex &vert = *board->vertices[primaryVertex];
        int half = Vertex::s_vert_size / 2;
        SDL_Rect active{vert.x - half, vert.y - half, Vertex::s_vert_size, Vertex::s_vert_size};

        SDL_SetRenderDrawColor(board->renderer, player->R, player->G, player->B, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(board->renderer, &active);
    }
    if (secondaryVertex != -1) {
        Vertex &vert = *board->vertices[secondaryVertex];
        int half = Vertex::s_vert_size / 2;
        SDL_Rect active{vert.x - half, vert.y - half, Vertex::s_vert_size, Vertex::s_vert_size};

        SDL_SetRenderDrawColor(board->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(board->renderer, &active);
    }

    if (diceRoll) board->drawRoll(diceRoll);

    if (buildingRoad) {
        board->drawActiveBuilding(player->roadTextures[1]);
    } else if (buildingSettlement) {
        board->drawActiveBuilding(player->settlementTexture);
    } else if (buildingCity) {
        board->drawActiveBuilding(player->cityTexture);
    } else if (buyingDevCard) {
        board->drawActiveBuilding(board->devCardBack);
    }
}

void Game::updateAndRender() {
    updateDisplay();
    SDL_RenderPresent(board->renderer);

    // Update time since last render s.t. the framerate is limited to macro FPS
    checkFrameTime();

    for (Player* p : players) {
        if (p->victoryPoints == 3) {
            SDL_Log("Player %d wins!", p->id);
            close();
        }
    }
}

void Game::checkFrameTime() {
    int tick_delta = (int)(SDL_GetTicks() - tick);
    if (tick_delta < frameLimit) {
        SDL_Delay(frameLimit - tick_delta);
    }
    tick = SDL_GetTicks();
}

void Game::roll() {
    if (diceRolled) {
        SDL_Log("End turn first!");
        return;
    }
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(1, 6);
    diceRoll = distribution(generator) + distribution(generator);
    diceRolled = true;

    if (diceRoll == 7) {
        robber = true;
        for (Tile* &tile : board->tiles) {
            tile->hasRobber = false;
        }
        return;
    }

    Vertex *vertex;
    Tile *tile;
    for (Player* p : players) {

        // For each vertex owned by player
        for (int &v : p->buildings) {
            vertex = board->vertices[v];

            // For each tile that that vertex touches
            for (int &t : vertex->tiles) {
                tile = board->tiles[t];
                if (!tile->hasRobber && (tile->chitNum == diceRoll)) {

                    /*
                     * If not robber on it, then add 1 for settlement and 2 for city to that players
                     * list of resources of that type
                     */
                    p->resources[tile->type] += vertex->buildingValue();
                }
            }
        }
    }
}

/*
 * This function does not check to see if there are enough total cards, only checks to see if
 * there are enough of the randomly chosen card. Thus, it is possible that there are no cards
 * left and this function will still return a card (VP_CARD), so check before calling this
 * function if there are any total cards left
 */
int Game::drawCard() {
    // Generator for random development card draw
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(1, 25);

    nDevelopmentCards--;
    int card = distribution(generator);
    if (card <= 14) {
        return KNIGHT;
    } else if (card <= 16) {
        return ROAD_BUILDING;
    } else if (card <= 18) {
        return YEAR_OF_PLENTY;
    } else if (card <= 20) {
        return MONOPOLY;
    } else {
        return VP_CARD;
    }

}

bool Game::buildRoad() {
    if ((primaryVertex == -1) || (secondaryVertex == -1)) {
        SDL_Log("Need two selected vertices to build a road");
        return false;
    } else if (!board->vertices[primaryVertex]->isAdjacent(secondaryVertex)) {
        SDL_Log("Not adjacent!");
        return false;
    } else {
        if ((direction == 1) && player->firstRoad) {
            SDL_Log("You built your first road already!");
            return false;
        } else if ((direction == -1) && player->secondRoad) {
            SDL_Log("You already built your second road!");
            return false;
        }
        for (auto& x : player->roads) {
            if ((x.first == primaryVertex) && (x.second == secondaryVertex)) {
                SDL_Log("You already have a road there");
                return false;
            }
        }
        bool canBuild = false;
        if (player->hasBuilding(primaryVertex)) {
            canBuild = true;
        } else if (board->vertices[primaryVertex]->isEmpty()) {
            for (auto& x : player->roads) {
                if ((primaryVertex == x.first) || (primaryVertex == x.second)) {
                    canBuild = true;
                    break;
                }
            }
        }
        if (canBuild) {
            if (direction) {
                return player->setupRoad(primaryVertex, secondaryVertex);
            } else {
                return player->buildRoad(primaryVertex, secondaryVertex);
            }
        } else {
            SDL_Log("Can't build from empty vertex");
            return false;
        }
    }
}

bool Game::buildSettlement() {
    if (primaryVertex == -1) {
        SDL_Log("Need a selected vertex to build a settlement");
        return false;
    } else if (!board->vertices[primaryVertex]->isEmpty()) {
        return false;
    } else {
        if (direction == 1) {
            if (player->firstSettlement) {
                SDL_Log("End your turn!");
                return false;
            } else {
                return player->setupSettlement(primaryVertex);
            }
        } else if (direction == -1) {
            if (player->secondSettlement) {
                SDL_Log("End your turn (again)!");
                return false;
            } else {
                return player->setupSettlement(primaryVertex);
            }
        } else {
            return player->buildSettlement(primaryVertex);
        }
    }
}

void Game::start() {
    if (nPlayers < 2) {
        SDL_Log("Not enough players to start game\n");
    } else {
        if (!initializeBoard()) {
            SDL_Log("Error loading resources\n");
            close();
        } else {
            direction = 1;
            activePlayer = 0;
            player = nextPlayer();
            running = true;
            checkFrameTime();
            eventLoop();
        }
    }
}

void Game::clearActiveState() {
    primaryVertex = secondaryVertex = -1;
    buyingDevCard = buildingRoad = buildingSettlement = buildingCity = false;
    SDL_Log("Active state cleared");
}

void Game::eventLoop() {
    SDL_Event event;
    SDL_Point mouse;

    while (running && (SDL_WaitEvent(&event))) {
        switch (event.type) {
            case SDL_QUIT:
                SDL_Log("SDL_QUIT\n");
                running = false;
                break;
            case SDL_KEYDOWN:
                // SDL_Log("Key down: %s\n", SDL_GetKeyName(event.key.keysym.sym));
                handleKeyPress(event.key);
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:

                        // Adjust board so that the tile positions are with respect to new board size
                        board->resize(event.window.data1, event.window.data2);
                        board->resizeVertices();

                        updateDisplay();

                        // Show render and look for more events
                        SDL_RenderPresent(board->renderer);

                        // Update time since last render s.t. the framerate is limited to macro FPS
                        checkFrameTime();

                        // Give 100ms of delay after resizing
                        SDL_Delay(100);
                        break;
                    case SDL_WINDOWEVENT_CLOSE:
                        running = false;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                handleMouseDown();
                break;
            default:
                SDL_GetMouseState(&mouse.x, &mouse.y);
                // SDL_Log("Mouse at (%d, %d)\n", x, y);
        }
    }
}

void Game::handleMouseDown() {
    SDL_Point mouse;
    Uint32 buttons = SDL_GetMouseState(&mouse.x, &mouse.y);
    if (buttons & SDL_BUTTON_LMASK) {

        // updateDisplay();

        // If end turn button hit, move to next player
        if (encloses(*board->endTurnButton, mouse)) {
            if (robber) {
                SDL_Log("Move the robber first!");
            } else if ((direction == 1) && (!player->firstSettlement || !player->firstRoad)) {
                SDL_Log("Must build a settlement and road first!");
            } else if ((direction == -1) && (!player->secondSettlement || !player->secondRoad)) {
                SDL_Log("Must build second second settlement");
            } else if (diceRolled) {
                player = nextPlayer();
                diceRoll = 0;
                clearActiveState();
                diceRolled = false;
            } else if (direction) {
                player = nextPlayer();
                clearActiveState();
            } else {
                SDL_Log("You need to roll dice first!");
            }
        } else if (encloses(*board->rollDiceRect, mouse)) {
            if (!direction) {
                roll();
            }
        } else {
            bool clickedVertex = false;
            Vertex *vertex;
            for (int i = 0; i < N_VERTICES; i++) {
                vertex = board->vertices[i];
                if (vertex->in(mouse.x, mouse.y)) {
                    clickedVertex = true;
                    if (buildingRoad) {
                        secondaryVertex = i;
                    } else if (primaryVertex != i) {
                        primaryVertex = i;
                    }
                }
            }

            // If clicked anywhere besides a button or a vertex, all active states should be cleared
            if (!clickedVertex) {
                clearActiveState();
            }

            if (robber) {
                for (Tile* &tile : board->tiles) {
                    if ((tile->type != DESERT) && encloses(tile->x, tile->y, 40, 40, mouse.x, mouse.y)) {
                        tile->hasRobber = true;
                        robber = false;
                    }
                }
            }


        }
        updateAndRender();
    }
}

void Game::confirmUserAction() {
    if (buyingDevCard) {
        if (!player->buyDevelopmentCard()) {
            SDL_Log("Not enough resources to buy development card");
        }
    } else if (buildingRoad) {
        if (!buildRoad()) {
            SDL_Log("Unable to build that road");
        }
    } else if (buildingSettlement) {
        if (!buildSettlement()) {
            SDL_Log("Unable to build settlement");
        } else {
            board->vertices[primaryVertex]->buildSettlement();
            if (direction == -1) {
                for (int &i : board->vertices[primaryVertex]->tiles) {
                    player->resources[board->tiles[i]->type]++;
                    SDL_Log("Adding resource type %d to player id %d", board->tiles[i]->type,
                            player->id);
                }
            }
        }
    } else if (buildingCity) {
        if (primaryVertex == -1) {
            SDL_Log("Need a selected vertex to build a city");
        } else if (!board->vertices[primaryVertex]->hasSettlement()) {
            SDL_Log("Must build city at vertex with settlement");
        } else {
            bool built = false;
            for (int &i : player->buildings) {
                if (i == primaryVertex) {
                    player->buildCity();
                    board->vertices[primaryVertex]->buildCity();
                    built = true;
                    break;
                }
            }
            if (!built) SDL_Log("Not your vertex!");
        }
    }

    // After ENTER hit, no actions continue
    clearActiveState();
}

void Game::handleKeyPress(SDL_KeyboardEvent &keyEvent) {
    switch(keyEvent.keysym.sym) {

        // Esc to quit game
        case SDLK_ESCAPE:
            running = false;
            break;

        // d to buy development card
        case SDLK_d:
            buyingDevCard = true;
            buildingRoad = buildingCity = buildingSettlement = false;
            SDL_Log("Buying development card");
            break;

        // r to build road
        case SDLK_r:
            buildingRoad = true;
            buyingDevCard = buildingCity = buildingSettlement = false;
            SDL_Log("Building road");
            break;

        // s to build settlement
        case SDLK_s:
            buildingSettlement = true;
            buyingDevCard = buildingCity = buildingRoad = false;
            SDL_Log("Building settlement");
            break;

        // c to build city
        case SDLK_c:
            buildingCity = true;
            buyingDevCard = buildingSettlement = buildingRoad = false;
            SDL_Log("Building city");
            break;

        // Enter to confirm action
        case SDLK_RETURN:
            confirmUserAction();
            SDL_Log("Confirming action");
            break;

        default:
            break;
    }
    updateAndRender();
}

void Game::close() {
    // Allows freeing the allocated renderer and window data if necessary
    if (board->renderer) {
        SDL_DestroyRenderer(board->renderer);
        SDL_Log("Renderer destroyed\n");
    }
    if (board->window) {
        SDL_DestroyWindow(board->window);
        SDL_Log("Window destroyed\n");
    }

    // This saves game and frees everything
    gameLoader->saveAndClose();

    TTF_Quit();
    SDL_Quit();
}

int main() {
    GameLoader loader;

    if (!loader.runTitleScreen()) {
        return 3;
    } else if (!loader.getPlayerInfo()) {
        return 4;
    }

    // If want to jump into game no testing, uncomment this
    //    SDL_Init(SDL_INIT_VIDEO);
    //    loader.players.push_back(new Player(1, RUSSET));
    //    loader.players.push_back(new Player(2, BLUE));
    //    loader.board = new Board();

    Game game(&loader);

    SDL_Log("Constructed game");

    if (!game.initialize()) {
        SDL_Log("Error initializing board: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Log("Board initialized");
    if (!game.initializeTTF()) {
        SDL_Log("Error initializing TTF library: %s\n", SDL_GetError());
        return 2;
    }
    if (!game.initializePlayerTextures()) {
        SDL_Log("Error initializing player textures: %s", SDL_GetError());
        return 5;
    }

    SDL_Log("Starting game");
    game.start();
    game.close();

    return 0;
}
