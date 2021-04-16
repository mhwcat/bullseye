#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cmath>

#include <SDL.h>
#include <SDL_keycode.h>
#include "glad/glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_opengl3.h"

// @TODO: Move this somewhere
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "logger.h"
#include "shader.h"
#include "shader_manager.h"
#include "camera.h"
#include "simple_timer.h"
#include "mesh.h"
#include "app_settings.h"
#include "skybox.h"
#include "gun.h"
#include "math_utils.h"
#include "entity.h"
#include "consts.h"
#include "texture_manager.h"

const int WIDTH = 1280;
const int HEIGHT = 720;

using namespace bullseye;

int main(int argc, char *argv[]) {
    logger::info("Starting Bullseye");

    app_settings::AppSettings app_settings { false,  false };

    logger::debug("Initializing SDL");
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        logger::error("Failed initializing SDL! Error: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_version sdl_version;
    SDL_GetVersion(&sdl_version);
    logger::debug("SDL version: %d.%d.%d", sdl_version.major, sdl_version.minor, sdl_version.patch);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);   
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    logger::debug("Initializing window");
    SDL_Window* window = SDL_CreateWindow("$$$ Cartel Birthday $$$", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == NULL) {
        logger::error("Failed initializing window! Error: %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    {
        int window_w, window_h, window_x, window_y;

        SDL_GetWindowSize(window, &window_w, &window_h);
        SDL_GetWindowPosition(window, &window_x, &window_y);
        logger::debug("Initialized window [posX=%d, posY=%d, width=%d, height=%d, title=%s]", window_x, window_y, window_w, window_h, SDL_GetWindowTitle(window));
    }

    logger::debug("Initializing GL context");
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == NULL) {
        logger::error("Failed initializing GL context! Error: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Enable vsync
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        logger::error("Failed to initialize OpenGL context");
        return EXIT_FAILURE;
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);

    // Load ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 430");

    logger::debug("OpenGL info [Vendor: %s, Renderer: %s, Version: %s]", glGetString(GL_VENDOR), 
        glGetString(GL_RENDERER), glGetString(GL_VERSION));

    shader::ShaderManager shader_manager;
    shader_manager.load_shader("lightcube", "assets/shaders/light_cube_vert.glsl", "assets/shaders/light_cube_frag.glsl");
    shader_manager.load_shader("main", "assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
    shader_manager.load_shader("gun", "assets/shaders/gun_vert.glsl", "assets/shaders/gun_frag.glsl");

    texture::TextureManager texture_manager;
    texture_manager.load_texture("grass", "assets/textures/grass.jpg");

    entity::Entity plane("plane", glm::vec3(0.f, -3.f, 0.f));
    plane.add_mesh_from_file("plane_mesh", "assets/models/plane.obj", glm::vec3(2.f));

    entity::Entity box("box", glm::vec3(0.f, 0.f, -5.f));
    box.add_mesh_from_file("box_mesh", "assets/models/cube.obj");
    entity::Entity box2("box2", glm::vec3(10.f, 3.f, -2.f));
    box2.add_mesh_from_file("box_mesh", "assets/models/cube.obj");

    entity::gun::Gun gun;
    gun.add_mesh_from_file("gun_mesh", "assets/models/M4A1.obj", glm::vec3(0.016f, 0.016f, 0.016f));

    std::vector<entity::Entity> entities;
    entities.push_back(std::move(box));
    entities.push_back(std::move(box2));
    entities.push_back(std::move(plane));

    std::vector<mesh::Mesh> light_cubes;
    light_cubes.push_back(mesh::Mesh("light", consts::SIMPLE_CUBE_VERTICES, sizeof(consts::SIMPLE_CUBE_VERTICES) / sizeof(float)));
    light_cubes.push_back(mesh::Mesh("light", consts::SIMPLE_CUBE_VERTICES, sizeof(consts::SIMPLE_CUBE_VERTICES) / sizeof(float)));

    camera::Camera camera(WIDTH, HEIGHT);

    const std::vector<std::string> skybox_texture_paths({
        "assets/textures/skybox/posx.jpg",
        "assets/textures/skybox/negx.jpg",
        "assets/textures/skybox/posy.jpg",
        "assets/textures/skybox/negy.jpg",
        "assets/textures/skybox/posz.jpg",
        "assets/textures/skybox/negz.jpg"
    });
    skybox::Skybox skybox(skybox_texture_paths, "assets/shaders/skybox_vert.glsl", "assets/shaders/skybox_frag.glsl");

    //entity::gun::Gun gun("assets/models/M4A1.obj", "assets/shaders/gun_vert.glsl", "assets/shaders/gun_frag.glsl");

    const char* entities_names[64];
    for (int i = 0; i < entities.size(); i++) {
        entities_names[i] = entities[i].get_name();
    }
    static int listbox_item_current = 1;

    typedef std::chrono::high_resolution_clock Clock;
    simple_timer::SimpleTimer frame_timer;

    // Time between updates in microseconds
    const uint64_t dt = static_cast<const uint64_t>(1 * 1000);

    uint64_t current_time = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(Clock::now().time_since_epoch()).count());
    uint64_t accumulator = 0, new_time = 0, loop_time = 0, time_elapsed = 0;
    float interp = 0.f, last_render_time = 0.f, update_time = 0.f;

    bool running = true;
    while (running) {
        // ===== Timer ops
        frame_timer.start();

        new_time = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(Clock::now().time_since_epoch()).count());
        loop_time = new_time - current_time;

        current_time = new_time;
        accumulator += loop_time;

        // Prevent accumulator reaching too high values when updating takes too long
        if(accumulator > 250000) {
            accumulator = 250000; 
        }

        // ===== Event handling
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        logger::debug("Window resized [width=%d, height=%d]", event.window.data1, event.window.data2);
                    
                        glViewport(0, 0, event.window.data1, event.window.data2);
                    }
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_w) {
                        camera.process_input(camera::MovementDirection::FRONT);
                    }
                    if (event.key.keysym.sym == SDLK_s) {
                        camera.process_input(camera::MovementDirection::BACK);
                    }     
                    if (event.key.keysym.sym == SDLK_a) {
                        camera.process_input(camera::MovementDirection::LEFT);
                    }
                    if (event.key.keysym.sym == SDLK_d) {
                        camera.process_input(camera::MovementDirection::RIGHT);
                    }  
                    if (event.key.keysym.sym == SDLK_q) {
                        camera.process_input(camera::MovementDirection::UP);
                    }
                    if (event.key.keysym.sym == SDLK_z) {
                        camera.process_input(camera::MovementDirection::DOWN);
                    }
                    if (event.key.keysym.sym == SDLK_c) {
                        app_settings.camera_mouse_attached = !app_settings.camera_mouse_attached;
                    }
                    if (event.key.keysym.sym == SDLK_f) {
                        app_settings.camera_free_fly = !app_settings.camera_free_fly;
                    }                    
                    break;  
                case SDL_KEYUP:
                    camera.process_input(camera::MovementDirection::NONE);   
                    break; 
                case SDL_MOUSEMOTION:
                    if (camera.is_mouse_attached()) {
                        camera.process_mouse_input((float) event.motion.xrel, (float) -event.motion.yrel);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // Discard click event if processed by ImGui
                    if (ImGui::GetIO().WantCaptureMouse) {
                        break;
                    }

                    if (event.button.button == SDL_BUTTON_LEFT) {
                        gun.shoot();
                    }
                    break;
            }

            ImGui_ImplSDL2_ProcessEvent(&event);
        }
        
        // ===== App settings
        camera.update_settings(&app_settings);

        // ===== Logic update
        while(accumulator >= dt) {
            camera.update(dt / 1000000.f);
            gun.update(dt / 1000000.f);
            
            for (auto &entity : entities) {
                entity.update(dt / 1000000.f);
            }

            time_elapsed += dt;
            accumulator -= dt;
        }

        interp = (float) accumulator / (float) dt;

        update_time = frame_timer.get_microseconds_since_start() / 1000.f;

        // ===== Rendering
        frame_timer.start();

        glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 proj = camera.get_perspective_matrix();
        glm::mat4 view = camera.get_view_matrix(interp);
        uint64_t movement = time_elapsed / 10000;

        glm::vec3 light = glm::vec3(9.f, 4.5f, (5.f * sin(math_utils::to_radians(movement)) + 2.f));

        shader_manager.use_shader("gun");
        shader_manager.set_mat4("gun", "projection", proj);
        shader_manager.set_mat4("gun", "view", view);
        shader_manager.set_vec3("gun", "light_pos", light);
        shader_manager.set_vec3("gun", "view_pos", *camera.get_position());
        shader_manager.set_vec3("gun", "light_color", glm::vec3(1.f, 1.f, 1.f));
        shader_manager.set_vec3("gun", "object_color", glm::vec3(0.1f, 0.1f, 0.1f));  
        gun.draw(shader_manager.get_shader("gun"), interp);

        for (auto &entity : entities) {
            if (strcmp(entity.get_name(), "plane") == 0) {
                texture_manager.use_texture("grass", shader_manager.get_shader("main").get_id());
            }

            shader_manager.use_shader("main");
            shader_manager.set_mat4("main", "projection", proj);
            shader_manager.set_mat4("main", "view", view);
            shader_manager.set_vec3("main", "light_pos", light);
            shader_manager.set_vec3("main", "view_pos", *camera.get_position());
            shader_manager.set_vec3("main", "light_color", glm::vec3(1.f, 1.f, 1.f));
            shader_manager.set_vec3("main", "object_color", glm::vec3(0.1f, 0.5f, 0.3f)); 

            entity.draw(shader_manager.get_shader("main"), interp);
        }

        shader_manager.use_shader("lightcube");
        shader_manager.set_mat4("lightcube", "projection", proj);
        shader_manager.set_mat4("lightcube", "view", view);
        for (auto &light_cube_mesh : light_cubes) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, light);
            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
            shader_manager.set_mat4("lightcube", "model", model);

            light_cube_mesh.draw_light_cube(); 
        }

        // Skybox
        skybox.draw(proj, view);

        // Debug GUI 
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
        ImGui::Begin("Debug");
        ImGui::Text("Stats");
        ImGui::Separator();
        ImGui::Text("Size: [%.2f, %.2f]", ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
        ImGui::Text("Update: %.2f ms", update_time);
        ImGui::Text("Render: %.2f ms (%.2f FPS)", last_render_time, 1000.f / last_render_time);
        ImGui::Spacing();
        ImGui::Text("Camera");
        ImGui::Separator();
        ImGui::Text("Pos: [%.2f, %.2f, %.2f]", camera.get_position()->x, camera.get_position()->y, camera.get_position()->z);
        ImGui::Text("Yaw: %.2f", camera.get_yaw());
        ImGui::Text("Pitch: %.2f", camera.get_pitch());
        ImGui::Spacing();
        ImGui::Checkbox("Locked [C]", &app_settings.camera_mouse_attached);
        ImGui::Checkbox("Free fly [F]", &app_settings.camera_free_fly);
        ImGui::End();
        ImGui::Begin("Entities");
        ImGui::PushItemWidth(-1);
        ImGui::ListBox("", &listbox_item_current, entities_names, entities.size());
        ImGui::Separator();
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);

        last_render_time = frame_timer.get_microseconds_since_start() / 1000.f;
    }

    // Cleanup
    shader_manager.unload();
    texture_manager.unload();

    for (auto &entity : entities) {
        entity.unload();
    }

    for (auto light_cube_mesh : light_cubes) {
        light_cube_mesh.unload();
    }

    gun.unload();

    skybox.unload();

    logger::debug("Shutting down ImGui");
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    logger::debug("Shutting down GL context and SDL");
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    logger::info("Quitting Bullseye");

    return EXIT_SUCCESS;
}