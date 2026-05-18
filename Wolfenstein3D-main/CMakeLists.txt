cmake_minimum_required(VERSION 3.16)

project(Wolfenstein3D VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(wolf3d
    src/main.cpp
    src/Game.cpp
    src/Raycaster.cpp
    src/Player.cpp
    src/Map.cpp
    src/Monster.cpp
)

target_include_directories(wolf3d PRIVATE include)

if(TARGET SDL2::SDL2)
    target_link_libraries(wolf3d PRIVATE SDL2::SDL2)
else()
    target_link_libraries(wolf3d PRIVATE SDL2::SDL2-static)
endif()

if(TARGET SDL2_image::SDL2_image)
    target_link_libraries(wolf3d PRIVATE SDL2_image::SDL2_image)
elseif(TARGET SDL2_image::SDL2_image-static)
    target_link_libraries(wolf3d PRIVATE SDL2_image::SDL2_image-static)
endif()

if(TARGET SDL2::SDL2main)
    target_link_libraries(wolf3d PRIVATE SDL2::SDL2main)
endif()
