cmake_minimum_required(VERSION 3.20)
project(catan_cpp)

set(CMAKE_CXX_STANDARD 14)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SDL2)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_LIBRARIES})

find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_TTF_INCLUDE_DIRS} ${SDL2_TTF_LIBRARIES})

file(GLOB CATAN_SRC
        "src/*.h"
        "src/*.cpp")

add_executable(${PROJECT_NAME}
        ${CATAN_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
        include ${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})
