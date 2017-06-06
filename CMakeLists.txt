# *********************************************************************
# NND32
#       You better run and hide.
# *********************************************************************
project(lp3_nnd32 CXX)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

add_library(Lp3_NND32
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dummy.cpp
)

target_include_directories(Lp3_NND32
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src)

target_link_libraries(Lp3_NND32 Lp3_Core Lp3_Sims Lp3_Gfx Lp3_Mix Lp3_Input)

install(TARGETS Lp3_NND32 EXPORT Lp3_NND32_Config
    ARCHIVE  DESTINATION lib
    LIBRARY  DESTINATION lib
    RUNTIME  DESTINATION bin)  # This is for Windows
install(DIRECTORY include/ DESTINATION include)

install(EXPORT Lp3_NND32_Config DESTINATION share/Lp3_NND32/cmake)

export(TARGETS Lp3_NND32 FILE Lp3_NND32_Config.cmake)

add_executable(FlipSIDE ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(FlipSIDE Lp3_NND32)

if (LP3_SETUP_FOLDERS)
    set_target_properties (Lp3_NND32 PROPERTIES FOLDER NND32)
    set_target_properties (FlipSIDE PROPERTIES FOLDER NND32)
endif()
