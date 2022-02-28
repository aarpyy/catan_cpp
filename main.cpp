#include <algorithm>

#include "Board.h"
#include "GameLoader.h"

#define FPS 45                      // Max framerate of game
#define frametime (1000 / FPS)      // Minimum time (ms) each frame must be displayed for until new one can be rendered

void flush(SDL_Renderer *renderer, std::array<Uint8, 3> render_color) {
    // Resets pixel data information from the renderer using the draw color
    SDL_SetRenderDrawColor(renderer,
                           render_color[0],
                           render_color[1],
                           render_color[2],
                           SDL_ALPHA_OPAQUE);

    // Sets the background color of the renderer to a sea-like color
    SDL_RenderClear(renderer);
}

void refresh(SDL_Renderer *renderer, std::array<Uint8, 3> render_color, Board *board) {
    // Flush renderer with render-color
    flush(renderer, render_color);

    // Draw new board
    board->draw(renderer);
}

void update(Uint32 *tick) {
    Uint32 tick_delta = SDL_GetTicks() - *tick;
    if (tick_delta < frametime) {
        SDL_Delay(frametime - tick_delta);
    }
    *tick = SDL_GetTicks();
}

int main(int argc, char **argv) {
    Board *board;
    GameLoader *game;

    /*
     * Flags are retrieved if they exist are as follows:
     *
     * -o specifies an output file i.e. the file that the game data will be saved in
     * -l specifies the input file i.e. the file that the game data will be read from
     */
    std::string in, out;
    std::string o_flag = "-o";
    std::string l_flag = "-l";
    if (argc > 1) {
        int i;
        for (i = 1; argv[i]; i++) {
            if (argv[i] == o_flag) {
                i++;
                out = argv[i];
            } else if (argv[i] == l_flag) {
                i++;
                in = argv[i];
            }
        }
    }

    // Use saves folder as default save location. Currently, nothing to stop it overwriting an existing file name.
    // Also, nothing currently checking to see if infile actually exists.
    if (out.empty()) out = "save";
    out.insert(0, "saves/");

    if (in.empty()) {
        board = new Board();
        game = new GameLoader(board, out);
    } else {
        in.insert(0, "saves/");
        game = new GameLoader(in, out);
        board = game->board;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        Uint32 window_flags, tick, buttons;
        std::array<Uint8, 3> render_color{0, 84, 119};

        // Integers for x, y coords of mouse button
        int window_width, window_height;
        SDL_Point mouse;

        // Creates an SDL window used to display pixel data to the user
        SDL_Window* window = nullptr;

        /*
         * Creates an SDL renderer used to collate textures and then render them
         * simultaneously within the window with which it is associated
         */
        SDL_Renderer* renderer = nullptr;

        SDL_DisplayMode DM;
        SDL_GetCurrentDisplayMode(0, &DM);

        // Integers for window size
        window_width = (int)(0.90 * DM.w);
        window_height = (int)(0.90 * DM.h);

        // Sets SDL-window to be resizable

        window_flags = SDL_WINDOW_RESIZABLE;

        if (!SDL_CreateWindowAndRenderer(window_width, window_height,
                                         window_flags, &window, &renderer)) {

            // Boolean for if game has been quit
            SDL_bool done = SDL_FALSE;

            // Set blend mode to none
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

            board->initialize(window_width, window_height);
            // Flush and re-render board
            refresh(renderer, render_color, board);
            SDL_RenderPresent(renderer);

            // Get time since last render display
            tick = SDL_GetTicks();

            SDL_Event event;
            while ((!done) && (SDL_WaitEvent(&event))) {
                switch (event.type) {
                    case SDL_QUIT:
                        std::cout << "Quit" << std::endl;
                        done = SDL_TRUE;
                        break;
                    case SDL_KEYDOWN:
                        std::cout << "Key down" << std::endl;
                        if (event.key.keysym.sym == SDLK_ESCAPE) done = SDL_TRUE;
                        break;
                    case SDL_WINDOWEVENT:
                        switch (event.window.event) {
                            case SDL_WINDOWEVENT_RESIZED:

                                // Adjust board so that the tile positions are with respect to new board size
                                board->resize(event.window.data1, event.window.data2);
                                board->resizeVertices();

                                // Flush renderer with render-color
                                flush(renderer, render_color);

                                // Draw new board
                                board->draw(renderer);

                                // Show render and look for more events
                                SDL_RenderPresent(renderer);

                                // Update time since last render s.t. the framerate is limited to macro FPS
                                update(&tick);

                                // Give 100ms of delay after resizing
                                SDL_Delay(100);
                                break;
                            case SDL_WINDOWEVENT_CLOSE:
                                done = SDL_TRUE;
                                break;
                            case SDL_WINDOWEVENT_SHOWN:
                                SDL_Log("Window %d shown", event.window.windowID);
                                break;
                            case SDL_WINDOWEVENT_HIDDEN:
                                SDL_Log("Window %d hidden", event.window.windowID);
                                break;
                            case SDL_WINDOWEVENT_EXPOSED:
                                // SDL_Log("Window %d exposed", event.window.windowID);
                                break;
                            case SDL_WINDOWEVENT_MOVED:
//                                SDL_Log("Window %d moved to %d,%d",
//                                        event.window.windowID, event.window.data1,
//                                        event.window.data2);
                                break;
                            case SDL_WINDOWEVENT_SIZE_CHANGED:
//                                SDL_Log("Window %d size changed to %dx%d",
//                                        event.window.windowID, event.window.data1,
//                                        event.window.data2);
                                break;
                            case SDL_WINDOWEVENT_MINIMIZED:
                                SDL_Log("Window %d minimized", event.window.windowID);
                                break;
                            case SDL_WINDOWEVENT_MAXIMIZED:
                                SDL_Log("Window %d maximized", event.window.windowID);
                                break;
                            case SDL_WINDOWEVENT_RESTORED:
                                SDL_Log("Window %d restored", event.window.windowID);
                                break;
                            case SDL_WINDOWEVENT_ENTER:
                                 SDL_Log("Mouse entered window %d", event.window.windowID);
                                break;
                            case SDL_WINDOWEVENT_LEAVE:
                                SDL_Log("Mouse left window %d", event.window.windowID);
                                break;
                            case SDL_WINDOWEVENT_FOCUS_GAINED:
                                 SDL_Log("Window %d gained keyboard focus", event.window.windowID);
                                break;
                            case SDL_WINDOWEVENT_FOCUS_LOST:
                                 SDL_Log("Window %d lost keyboard focus", event.window.windowID);
                                break;
                            #if SDL_VERSION_ATLEAST(2, 0, 5)
                                case SDL_WINDOWEVENT_TAKE_FOCUS:
                                    // SDL_Log("Window %d is offered a focus", event.window.windowID);
                                    break;
                                case SDL_WINDOWEVENT_HIT_TEST:
                                    // SDL_Log("Window %d has a special hit test", event.window.windowID);
                                    break;
                            #endif
                            default:
                                SDL_Log("Window %d got unknown event %d",
                                        event.window.windowID, event.window.event);
                                break;
                        }
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        buttons = SDL_GetMouseState(&mouse.x, &mouse.y);
                        if (buttons & SDL_BUTTON_LMASK) {
                            refresh(renderer, render_color, board);

                            for (Vertex* const& vertex : board->vertices) {
                                if (vertex->in(mouse)) {
                                    SDL_Log("Mouse position enclosed in vertex at %dx%d\n", mouse.x, mouse.y);

                                    SDL_Rect mouse_pos;
                                    mouse_pos.x = mouse.x - Vertex::s_vert_size / 2;
                                    mouse_pos.y = mouse.y - Vertex::s_vert_size / 2;
                                    mouse_pos.w = mouse_pos.h = Vertex::s_vert_size;

                                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
                                    SDL_RenderFillRect(renderer, &mouse_pos);

                                    // Draw square at adjacent vertex -- for testing
                                    for (int const& i : vertex->vertices) {
                                        SDL_Rect adj_vert;
                                        adj_vert.x = board->vertices[i]->x - Vertex::s_vert_size / 2;
                                        adj_vert.y = board->vertices[i]->y - Vertex::s_vert_size / 2;
                                        adj_vert.w = adj_vert.h = Vertex::s_vert_size;

                                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
                                        SDL_RenderFillRect(renderer, &adj_vert);
                                    }

                                    // Draw square at adjacent tile -- for testing
                                    for (int const& i : vertex->tiles) {
                                        SDL_Rect adj_tile;
                                        adj_tile.x = board->tiles[i]->x - Vertex::s_vert_size / 2;
                                        adj_tile.y = board->tiles[i]->y - Vertex::s_vert_size / 2;
                                        adj_tile.w = adj_tile.h = Vertex::s_vert_size;

                                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
                                        SDL_RenderFillRect(renderer, &adj_tile);
                                    }

                                    SDL_RenderPresent(renderer);

                                    // Delay so that square is visible
                                    SDL_Delay(200);

                                    refresh(renderer, render_color, board);
                                    SDL_RenderPresent(renderer);

                                    // Update time since last render s.t. the framerate is limited to macro FPS
                                    update(&tick);
                                }
                            }
                        }
                        break;
                    default:
                        SDL_GetMouseState(&mouse.x, &mouse.y);
                        // SDL_Log("Mouse at (%d, %d)\n", x, y);
                }
            }
        }
        else {
            SDL_Log("Error initializing window and renderer: %s\n", SDL_GetError());
        }

        // Allows freeing the allocated renderer and window data if necessary
        if (renderer) {
            std::cout << "Renderer destroyed" << std::endl;
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            std::cout << "Window destroyed" << std::endl;
            SDL_DestroyWindow(window);
        }
    }
    else {
        SDL_Log("Error initializing video: %s\n", SDL_GetError());
    }


    SDL_Quit();
    game->save();   // Write out to save file the game data

    delete board;
    delete game;
    return 0;
}
