#ifndef CATAN_CATANRULES_H
#define CATAN_CATANRULES_H

#endif //CATAN_CATANRULES_H

#include "game_utility.h"

#define N_DEV_CARD 25

// Each of the development cards
#define KNIGHT 0
#define ROAD_BUILDING 1
#define YEAR_OF_PLENTY 2
#define MONOPOLY 3
#define VP_CARD 4

// Number of each development cards
#define N_KNIGHTS 14
#define N_ROAD_BUILDING 2
#define N_YEAR_OF_PLENTY 2
#define N_MONOPOLY 2
#define N_VP_CARD 5


#define N_BUILDINGS 20
#define N_SETTLEMENTS 5
#define N_CITIES 4
#define N_ROADS 15

#define N_PORTS 9
#define MYSTERY_PORT 0

#define N_VERTICES 54

#define N_RESOURCE 6
#define DESERT 0
#define FIELD 1         // Grain
#define FOREST 2        // Lumber
#define HILL 3          // Brick
#define MOUNTAIN 4      // Ore
#define PASTURE 5       // Wool

#define N_DESERT 1
#define N_FIELD 4
#define N_FOREST 4
#define N_HILL 3
#define N_MOUNTAIN 3
#define N_PASTURE 4

#define N_TILES 19

// constexpr tells compiler to evaluate at compile time i.e. these are global, constant, and static
constexpr static std::array<char, N_RESOURCE> types{DESERT, FIELD, FOREST, HILL, MOUNTAIN, PASTURE};
constexpr static std::array<char, N_RESOURCE> nTypes{N_DESERT, N_FIELD, N_FOREST, N_HILL, N_MOUNTAIN, N_PASTURE};

constexpr static std::array<int, N_RESOURCE> roadRecipe{0, 0, 1, 1, 0, 0};
constexpr static std::array<int, N_RESOURCE> settlementRecipe{0, 1, 1, 1, 0, 1};
constexpr static std::array<int, N_RESOURCE> cityRecipe{0, 2, 0, 0, 3, 0};
constexpr static std::array<int, N_RESOURCE> developmentCardRecipe{0, 1, 0, 0, 1, 1};

constexpr static std::array<Uint8, 3> RUM{123, 111, 131};
constexpr static std::array<Uint8, 3> RUSSET{156, 67, 0};
constexpr static std::array<Uint8, 3> BLUE{79, 166, 235};
constexpr static std::array<Uint8, 3> GREEN{81, 125, 25};
constexpr static std::array<Uint8, 3> YELLOW{240, 173, 0};
constexpr static std::array<std::array<Uint8, 3>, 5> allColors{RUM, RUSSET, BLUE, GREEN, YELLOW};

constexpr static std::array<char, N_TILES - 1> chitNums{5, 2, 6, 3, 8, 10, 9, 12, 11, 4, 8, 10, 9, 4, 5, 6, 3, 11};
