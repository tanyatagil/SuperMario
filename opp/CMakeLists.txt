cmake_minimum_required(VERSION 3.16)
project(Mario)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Curses REQUIRED)

include_directories(include ${CURSES_INCLUDE_DIR})

add_executable(Mario
    src/main.cpp
    src/Map.cpp
    src/GameObject.cpp
    src/Movement.cpp
    src/LevelCreator.cpp
    src/GameController.cpp
    src/Brick.cpp
    src/Coin.cpp
    src/Enemy.cpp
    src/FinishBrick.cpp
    src/Mario.cpp
    src/QuestionBrick.cpp
)

target_link_libraries(Mario PRIVATE ${CURSES_LIBRARIES})
