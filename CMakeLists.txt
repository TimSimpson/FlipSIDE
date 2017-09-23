# *********************************************************************
# NND3d
#       You better run and hide.
# *********************************************************************
project(lp3_nnd3d CXX)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

add_library(Lp3_NND3d
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/BaseScreen.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/BaseScreen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/Camera.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/Camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/CharacterProc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/CharacterProc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/CharacterSprite.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/CharacterSprite.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/CinemaType.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/CinemaType.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/EntityManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/EntityManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/Game.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/Game.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/GameState.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/GameState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/GameOverScreen.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/GameOverScreen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/IntroStory.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/IntroStory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/LegacyGame.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/LegacyGame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/NowLoading.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/NowLoading.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/PlayerData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/PlayerData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/SelectScreen.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/SelectScreen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/TitleScreen.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/TitleScreen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/World.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/World.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/procs/CinemaProc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/procs/CinemaProc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/procs/PlayerProc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/procs/PlayerProc.cpp

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

target_link_libraries(Lp3_NND3d Lp3_Core Lp3_Sims Lp3_Gfx Lp3_Mix Lp3_Input)

install(TARGETS Lp3_NND3d EXPORT Lp3_NND3d_Config
    ARCHIVE  DESTINATION lib
    LIBRARY  DESTINATION lib
    RUNTIME  DESTINATION bin)  # This is for Windows
install(DIRECTORY include/ DESTINATION include)

install(EXPORT Lp3_NND3d_Config DESTINATION share/Lp3_NND3d/cmake)

export(TARGETS Lp3_NND3d FILE Lp3_NND3d_Config.cmake)

add_executable(FlipSIDE ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(FlipSIDE Lp3_NND3d)

if (LP3_SETUP_FOLDERS)
    set_target_properties (Lp3_NND3d PROPERTIES FOLDER NND3d)
    set_target_properties (FlipSIDE PROPERTIES FOLDER NND3d)
endif()
