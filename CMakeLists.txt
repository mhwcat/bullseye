cmake_minimum_required(VERSION 3.13)

project(bullseye)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_SUPPRESS_REGENERATION true)

set(SOURCE_FILES 
    include/logger.h include/shader.h include/structs.h include/camera.h
    src/main.cpp src/shader.cpp src/camera.cpp src/glad.c)
    
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR} PUBLIC ${OPENGL_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR} PUBLIC ${GLM_INCLUDE_DIRS})

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLM REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} SDL2::Main ${OPENGL_LIBRARIES} ${GLM_LIBRARIES} ${CMAKE_DL_LIBS})
