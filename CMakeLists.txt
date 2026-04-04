cmake_minimum_required(VERSION 3.10)
project(StratTank)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3.0 COMPONENTS Graphics Audio Window System REQUIRED)

set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Entity.cpp
    src/Tank.cpp
    src/PlayerTank.cpp
    src/AllyTank.cpp
    src/EnemyTank.cpp
    src/Fort.cpp
    src/Projectile.cpp
    src/Heart.cpp
    src/ParticleSystem.cpp
    src/WaveManager.cpp
    src/HUD.cpp
    src/WorldConfig.cpp
    src/CostMap.cpp
)

add_executable(StratTank ${SOURCES})

target_link_libraries(StratTank PRIVATE SFML::Graphics SFML::Audio SFML::Window SFML::System)

target_include_directories(StratTank PRIVATE include)