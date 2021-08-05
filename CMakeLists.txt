cmake_minimum_required(VERSION 3.13)

project(bullseye)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_SUPPRESS_REGENERATION true)

# External libs information (for keepsake, not used)
set(IMGUI_VERSION "1.82")
set(GLM_VERSION "0.9.9.8")
set(TOBJ_VERSION "2.0.0")
set(REACTPHYSICS3D_VERSION "c273e7cfe580037c6cb7073b0050e0ce7d6da231")
set(STB_IMAGE_VERSION "2.26")
set(CLOGGER_VERSION "0.6")
set(GLAD_VERSION "0.1.34")

# External libs provided by CMake modules
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# Project sources
set(BULLSEYE_SOURCES src/main.cpp src/shader.cpp src/camera.cpp src/simple_timer.cpp src/mesh.cpp 
    src/skybox.cpp src/gun.cpp src/entity.cpp src/shader_manager.cpp src/texture_manager.cpp src/mesh_manager.cpp src/physics_debug_renderer.cpp
    src/player.cpp)
    
add_executable(${PROJECT_NAME} ${BULLSEYE_SOURCES})

# Libraries
# GLM & RP3D
add_subdirectory(${PROJECT_SOURCE_DIR}/3rdparty/glm) # GLM fortunately has its own CMake project, so we can just include and link it
add_subdirectory(${PROJECT_SOURCE_DIR}/3rdparty/reactphysics3d) # Same with RP3D

# stbimage
set(STB_HEADERS 3rdparty/stb/stb_image.h)
add_library(stb_image OBJECT ${STB_HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/stb)

# glad
set(GLAD_SOURCES 3rdparty/glad/glad.c)
add_library(glad ${GLAD_SOURCES})
target_include_directories(glad PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/glad)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/glad)

# clogger
set(CLOGGER_SOURCES 3rdparty/clogger/clogger.cpp)
add_library(clogger ${CLOGGER_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/clogger)

# tobjl
set(TOBJL_SOURCES 3rdparty/tobjl/tiny_obj_loader.cc)
add_library(tobjl ${TOBJL_SOURCES})
target_include_directories(tobjl PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/tobjl)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/tobjl)

# imgui
set(IMGUI_SOURCES 3rdparty/imgui/imgui.cpp 3rdparty/imgui/imgui_widgets.cpp 3rdparty/imgui/imgui_draw.cpp 3rdparty/imgui/imgui_tables.cpp
    3rdparty/imgui/backends/imgui_impl_opengl3.cpp 3rdparty/imgui/backends/imgui_impl_sdl.cpp)
add_library(imgui ${IMGUI_SOURCES})
target_include_directories(imgui PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/imgui ${OPENGL_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/3rdparty/glad ${SDL2_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/imgui)
target_link_libraries(imgui ${OPENGL_LIBRARIES} glad SDL2::Main)
target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

# Global include
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})

# Link all of this
target_link_libraries(${PROJECT_NAME} 
    SDL2::Main 
    ${OPENGL_LIBRARIES} 
    glad 
    glm 
    stb_image 
    clogger 
    tobjl 
    imgui 
    reactphysics3d 
    ${CMAKE_DL_LIBS})
