cmake_minimum_required(VERSION 3.16)
project(Mario)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(Mario
    src/main.cpp
    src/Map.cpp
    src/GameObject.cpp
    src/Movement.cpp
    src/LevelCreator.cpp
    src/GameController.cpp
)
