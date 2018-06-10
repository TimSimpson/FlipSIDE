# *********************************************************************
# NND3d
#       You better run and hide.
# *********************************************************************
project(lp3_nnd3d CXX)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
include(CMakePackageConfigHelpers)

find_package(Lp3_Core)
find_package(Lp3_Input)
find_package(Lp3_Gfx)
find_package(Lp3_Mix)
find_package(Lp3_Sims)

add_library(Lp3_NND3d
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/Game.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/Game.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/GameState.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/GameState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/GameOverScreen.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/GameOverScreen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/IntroStory.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/IntroStory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/NowLoading.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/NowLoading.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/PlayerData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/PlayerData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/SelectScreen.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/SelectScreen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/TitleScreen.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/TitleScreen.cpp


    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/BaseScreen.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/BaseScreen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/Camera.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/Camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/CharacterSprite.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/CharacterSprite.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/EntityManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/EntityManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/LegacyGame.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/LegacyGame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/World.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/World.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/procs/CharacterProc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/procs/CharacterProc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/procs/CinemaProc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/procs/CinemaProc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/procs/CinemaType.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/procs/CinemaType.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/procs/Enemies.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/procs/Enemies.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/procs/PlayerProc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/stage/procs/PlayerProc.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/AnimationFrame.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AnimationFrame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Input.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Input.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Sound.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Random.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Random.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Vb.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Vb.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VbFile.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VbFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Vertex.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Vertex.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/view.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/view/View.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/view/View.cpp
)

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
    target_sources(Lp3_NND3d PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/SoundEms.cpp)
else()
    target_sources(Lp3_NND3d PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/Sound.cpp)
endif()

target_include_directories(Lp3_NND3d
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src)

target_link_libraries(Lp3_NND3d lp3::core lp3::sims lp3::gfx lp3::mix lp3::input)

add_executable(FlipSIDE ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(FlipSIDE Lp3_NND3d)

if (LP3_SETUP_FOLDERS)
    set_target_properties (Lp3_NND3d PROPERTIES FOLDER NND3d)
    set_target_properties (FlipSIDE PROPERTIES FOLDER NND3d)
endif()
