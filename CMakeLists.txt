cmake_minimum_required(VERSION 3.13)

project(bullseye)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_SUPPRESS_REGENERATION true)

if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()

# External libs included 
set(IMGUI_HEADERS include/imgui/imconfig.h include/imgui/imgui_internal.h include/imgui/imgui.h include/imgui/imstb_rectpack.h include/imgui/imstb_textedit.h 
    include/imgui/imstb_truetype.h include/imgui/imgui_impl_opengl3.h include/imgui/imgui_impl_sdl.h)
set(IMGUI_SOURCES src/imgui/imgui.cpp src/imgui/imgui_widgets.cpp src/imgui/imgui_draw.cpp src/imgui/imgui_tables.cpp
    src/imgui/imgui_impl_opengl3.cpp src/imgui/imgui_impl_sdl.cpp)
set(TOBJL_HEADERS include/tobjl/tiny_obj_loader.h)
set(TOBJL_SOURCES src/tobjl/tiny_obj_loader.cpp)
set(STB_HEADERS include/stb/stb_image.h)

set(SOURCE_FILES 
    include/logger.h include/shader.h include/math_utils.h include/camera.h include/simple_timer.h include/mesh.h include/app_settings.h include/skybox.h include/gun.h include/entity.h include/consts.h ${IMGUI_HEADERS} ${TOBJL_HEADERS} ${STB_HEADERS}
    src/glad.c src/main.cpp src/shader.cpp src/camera.cpp src/simple_timer.cpp src/mesh.cpp src/skybox.cpp src/gun.cpp src/entity.cpp ${IMGUI_SOURCES} ${TOBJL_SOURCES})
    
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR} PUBLIC ${OPENGL_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR} PUBLIC ${GLM_INCLUDE_DIRS})

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLM REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} SDL2::Main ${OPENGL_LIBRARIES} ${GLM_LIBRARIES} ${CMAKE_DL_LIBS})
target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)