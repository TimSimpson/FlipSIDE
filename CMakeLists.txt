# *********************************************************************
# NND3d
#       You better run and hide.
# *********************************************************************
project(lp3_nnd3d CXX)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

add_library(Lp3_NND3d
    ${CMAKE_CURRENT_SOURCE_DIR}/src/World.cpp
)

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
