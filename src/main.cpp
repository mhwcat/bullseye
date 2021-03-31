#include <cstdlib>
#include <cstdint>
#include <cstring>

#include "SDL.h"
#include "SDL_keycode.h"
#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include "logger.h"
#include "shader.h"
#include "camera.h"
#include "simple_timer.h"
#include "mesh.h"
#include "app_settings.h"

const int WIDTH = 1280;
const int HEIGHT = 720;

using namespace bullseye;

int main(int argc, char *argv[]) {
    logger::info("Starting Bullseye");

    app_settings::AppSettings app_settings { .camera_mouse_attached = false, .camera_free_fly = true };

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
    SDL_Window* window = SDL_CreateWindow("Bullseye", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
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
    //glDepthFunc(GL_LEQUAL);
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

    shader::Shader shader("main");
    shader.load_vertex_shader("assets/shaders/vertex.glsl");
    shader.load_fragment_shader("assets/shaders/fragment.glsl");
    shader.link_shaders();   

    std::vector<mesh::Mesh> meshes;
    meshes.push_back(mesh::Mesh("assets/models/cube.obj"));

    camera::Camera camera(WIDTH, HEIGHT);

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
            }

            ImGui_ImplSDL2_ProcessEvent(&event);
        }
        
        // ===== App settings
        camera.update_settings(&app_settings);

        // ===== Logic update
        while(accumulator >= dt) {
            camera.update(dt / 1000000.f);

            time_elapsed += dt;
            accumulator -= dt;
        }

        interp = (float) accumulator / (float) dt;

        update_time = frame_timer.get_microseconds_since_start() / 1000.f;

        // ===== Rendering
        frame_timer.start();

        glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 proj = camera.get_perspective_matrix();
        shader.set_mat4("perspective", proj);

        glm::mat4 view = camera.get_view_matrix(interp);
        shader.set_mat4("view", view);

        glm::vec3 light = glm::vec3(2.4f, 2.4f, -1.7f);
        shader.set_vec3("u_light", light);

        shader.set_float("time", (float) time_elapsed);

        for (auto mesh : meshes) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.f, 0.f, -5.f));
            shader.set_mat4("model", model);

            mesh.draw(shader);
        }

        // Debug GUI rendering
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
        ImGui::Checkbox("Locked", &app_settings.camera_mouse_attached);
        ImGui::Checkbox("Free fly", &app_settings.camera_free_fly);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);

        last_render_time = frame_timer.get_microseconds_since_start() / 1000.f;
    }

    // @TODO: Where to delete these?
    // glDeleteVertexArrays(1, &vao);
    // glDeleteBuffers(1, &vbo);
    
    shader.delete_program();

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