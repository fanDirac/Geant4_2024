cmake_minimum_required(VERSION 3.0)
project(Project)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)
add_executable(Project main.cc ${sources} ${headers})
target_link_libraries(Project ${Geant4_LIBRARIES} )