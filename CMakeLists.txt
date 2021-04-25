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

# External libs provided by CMake modules
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# External libs sources 
add_subdirectory(${PROJECT_SOURCE_DIR}/3rdparty/glm) # GLM fortunately has its own CMake project, so we can just include and link it
add_subdirectory(${PROJECT_SOURCE_DIR}/3rdparty/reactphysics3d) # Same with Bullet 
set(GLAD_HEADERS 3rdparty/glad/glad/glad.h 3rdparty/glad/KHR/khrplatform.h)
set(GLAD_SOURCES 3rdparty/glad/glad.c)
set(IMGUI_HEADERS 3rdparty/imgui/imconfig.h 3rdparty/imgui/imgui_internal.h 3rdparty/imgui/imgui.h 3rdparty/imgui/imstb_rectpack.h 3rdparty/imgui/imstb_textedit.h 
    3rdparty/imgui/imstb_truetype.h 3rdparty/imgui/backends/imgui_impl_opengl3.h 3rdparty/imgui/backends/imgui_impl_sdl.h)
set(IMGUI_SOURCES 3rdparty/imgui/imgui.cpp 3rdparty/imgui/imgui_widgets.cpp 3rdparty/imgui/imgui_draw.cpp 3rdparty/imgui/imgui_tables.cpp
    3rdparty/imgui/backends/imgui_impl_opengl3.cpp 3rdparty/imgui/backends/imgui_impl_sdl.cpp)
set(TOBJL_HEADERS 3rdparty/tobjl/tiny_obj_loader.h)
set(TOBJL_SOURCES 3rdparty/tobjl/tiny_obj_loader.cc)
set(STB_HEADERS 3rdparty/stb/stb_image.h)

# Project sources
set(BULLSEYE_HEADERS include/logger.h include/shader.h include/math_utils.h include/camera.h include/simple_timer.h 
    include/mesh.h include/app_settings.h include/skybox.h include/gun.h include/entity.h 
    include/consts.h include/shader_manager.h include/texture_manager.h include/physics_debug_renderer.h)
set(BULLSEYE_SOURCES src/main.cpp src/shader.cpp src/camera.cpp src/simple_timer.cpp src/mesh.cpp 
    src/skybox.cpp src/gun.cpp src/entity.cpp src/shader_manager.cpp src/texture_manager.cpp src/physics_debug_renderer.cpp)

set(SOURCE_FILES 
    ${GLAD_HEADERS} ${GLAD_SOURCES}
    ${IMGUI_HEADERS} ${IMGUI_SOURCES}
    ${TOBJL_HEADERS} ${TOBJL_SOURCES}
    ${STB_HEADERS} 
    ${BULLSEYE_HEADERS} ${BULLSEYE_SOURCES})
    
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/3rdparty/imgui)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/3rdparty/glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/3rdparty/tobjl)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/3rdparty/glad)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/3rdparty/stb)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/3rdparty/reactphysics3d/include)

target_link_libraries(${PROJECT_NAME} glm reactphysics3d SDL2::Main ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS})
target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)