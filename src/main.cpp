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

#include "reactphysics3d/reactphysics3d.h"

#include "clogger.h"
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
#include "physics_debug_renderer.h"
#include "mesh_manager.h"
#include "player.h"

const int WIDTH = 1280;
const int HEIGHT = 720;

using namespace bullseye;

int main(int argc, char *argv[]) {
    CLOG_INFO("Starting Bullseye");

    app_settings::AppSettings app_settings { false,  false, true };

    CLOG_DEBUG("Initializing SDL");
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        CLOG_ERROR("Failed initializing SDL! Error: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_version sdl_version;
    SDL_GetVersion(&sdl_version);
    CLOG_DEBUG("SDL version: %d.%d.%d", sdl_version.major, sdl_version.minor, sdl_version.patch);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);   
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    CLOG_DEBUG("Initializing window");
    SDL_Window* window = SDL_CreateWindow("Bullseye", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == NULL) {
        CLOG_ERROR("Failed initializing window! Error: %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    {
        int window_w, window_h, window_x, window_y;

        SDL_GetWindowSize(window, &window_w, &window_h);
        SDL_GetWindowPosition(window, &window_x, &window_y);
        CLOG_DEBUG("Initialized window [posX=%d, posY=%d, width=%d, height=%d, title=%s]", window_x, window_y, window_w, window_h, SDL_GetWindowTitle(window));
    }

    CLOG_DEBUG("Initializing GL context");
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == NULL) {
        CLOG_ERROR("Failed initializing GL context! Error: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Enable vsync
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        CLOG_ERROR("Failed to initialize OpenGL context");
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
    ImGui_ImplOpenGL3_Init("#version 410");

    CLOG_DEBUG("OpenGL info [Vendor: %s, Renderer: %s, Version: %s]", glGetString(GL_VENDOR), 
        glGetString(GL_RENDERER), glGetString(GL_VERSION));

    srand(time(NULL));

    shader::ShaderManager shader_manager;
    shader_manager.load_shader("lightcube", "assets/shaders/light_cube_vert.glsl", "assets/shaders/light_cube_frag.glsl");
    shader_manager.load_shader("main", "assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
    shader_manager.load_shader("gun", "assets/shaders/gun_vert.glsl", "assets/shaders/gun_frag.glsl");
    shader_manager.load_shader("physics_debug", "assets/shaders/physics_debug_vert.glsl", "assets/shaders/physics_debug_frag.glsl");

    texture::TextureManager texture_manager;
    texture_manager.load_texture("grass", "assets/textures/grass.jpg");
    texture_manager.load_texture("metal", "assets/textures/metal.jpg");

    mesh::MeshManager mesh_manager;
    mesh_manager.load_mesh("plane", "assets/models/plane.obj", glm::vec3(5.f, 1.f, 5.f));
    mesh_manager.load_mesh("box", "assets/models/cube.obj");
    mesh_manager.load_mesh("gun", "assets/models/M4A1.obj", glm::vec3(0.016f, 0.016f, 0.016f));
    mesh_manager.load_mesh("bullet", "assets/models/cube.obj", glm::vec3(0.3f, 0.3f, 0.3f));

    rp3d::PhysicsCommon physics_common;
    rp3d::PhysicsWorld* physics_world = physics_common.createPhysicsWorld();
    rp3d::DefaultLogger* logger = physics_common.createDefaultLogger();
    uint32_t logLevel = static_cast<uint32_t>(static_cast<uint32_t>(rp3d::Logger::Level::Information) | static_cast<uint32_t>(rp3d::Logger::Level::Warning) | static_cast<uint32_t>(rp3d::Logger::Level::Error));
    logger->addStreamDestination(std::cout, logLevel, rp3d::DefaultLogger::Format::Text);
    physics_common.setLogger(logger);
    //world->setGravity(rp3d::Vector3(0.f, -1.f, 0.f));
    //world->setNbIterationsPositionSolver(24);

    CLOG_DEBUG("Initialized rp3d physics");

    render::PhysicsDebugRenderer physics_debug_renderer(physics_world);

    CLOG_DEBUG("Initialized rp3d physics debug renderer");

    entity::Entity plane("plane", glm::vec3(0.f, -3.f, 0.f), rp3d::Quaternion::identity(), entity::BodyType::RIGID);
    plane.set_mesh("plane");
    entity::Entity box("box", glm::vec3(0.f, 9.f, -5.f), rp3d::Quaternion::identity(), entity::BodyType::RIGID);
    box.set_mesh("box");
    entity::Entity box2("box2", glm::vec3(10.f, 8.f, -2.f), rp3d::Quaternion::identity(), entity::BodyType::RIGID);
    box2.set_mesh("box");

    entity::gun::Gun gun;

    plane.init_physics(physics_world, &physics_common, mesh_manager.get_mesh(plane.get_mesh_name()));
    box.init_physics(physics_world, &physics_common, mesh_manager.get_mesh(box.get_mesh_name()));
    box2.init_physics(physics_world, &physics_common, mesh_manager.get_mesh(box2.get_mesh_name()));


    entity::player::Player player(glm::vec3(-10.f, 0.f, 4.f), rp3d::Quaternion::identity());
    CLOG_DEBUG("PLayer name: %s", player.get_name());

    std::vector<entity::Entity> entities;
    entities.push_back(std::move(box));
    entities.push_back(std::move(box2));
    entities.push_back(std::move(plane));

    std::vector<entity::Entity> volatile_entities;
    volatile_entities.reserve(1024);

    std::vector<mesh::Mesh> light_cubes;
    light_cubes.push_back(mesh::Mesh("light", consts::SIMPLE_CUBE_VERTICES, sizeof(consts::SIMPLE_CUBE_VERTICES) / sizeof(float)));
    light_cubes.push_back(mesh::Mesh("light", consts::SIMPLE_CUBE_VERTICES, sizeof(consts::SIMPLE_CUBE_VERTICES) / sizeof(float)));

    camera::Camera camera(WIDTH, HEIGHT);

    // Init player collision
    rp3d::BoxShape* player_shape = physics_common.createBoxShape(rp3d::Vector3(0.5f, 0.9f, 0.5f));
    rp3d::RigidBody* player_physics_body = physics_world->createRigidBody(rp3d::Transform(rp3d::Vector3(camera.get_position()->x, camera.get_position()->y, camera.get_position()->z), rp3d::Quaternion::identity()));
    player_physics_body->addCollider(player_shape, rp3d::Transform(rp3d::Vector3(), rp3d::Quaternion::identity()));
    player_physics_body->enableGravity(false);
    player_physics_body->setType(rp3d::BodyType::KINEMATIC);

    const std::vector<std::string> skybox_texture_paths({
        "assets/textures/skybox/posx.jpg",
        "assets/textures/skybox/negx.jpg",
        "assets/textures/skybox/posy.jpg",
        "assets/textures/skybox/negy.jpg",
        "assets/textures/skybox/posz.jpg",
        "assets/textures/skybox/negz.jpg"
    });
    skybox::Skybox skybox(skybox_texture_paths, "assets/shaders/skybox_vert.glsl", "assets/shaders/skybox_frag.glsl");

    static int listbox_item_current = 0;

    std::unordered_map<SDL_Keycode, bool> pressed_keys;
    pressed_keys.reserve(128);

    typedef std::chrono::high_resolution_clock Clock;
    simple_timer::SimpleTimer frame_timer;

    // Time between updates in microseconds
    const uint64_t dt = static_cast<const uint64_t>(10 * 1000);

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
                        CLOG_DEBUG("Window resized [width=%d, height=%d]", event.window.data1, event.window.data2);
                    
                        glViewport(0, 0, event.window.data1, event.window.data2);
                    }
                    break;
                case SDL_KEYDOWN:
                    pressed_keys[event.key.keysym.sym] = true;

                    if (event.key.keysym.sym == SDLK_c) {
                        app_settings.camera_mouse_attached = !app_settings.camera_mouse_attached;
                    }
                    if (event.key.keysym.sym == SDLK_f) {
                       app_settings.camera_free_fly = !app_settings.camera_free_fly;
                    }
                     if (event.key.keysym.sym == SDLK_g) {
                         const float X = 25.f;
                         const float Y = 360.f;
                         float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / X));
                         float y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / X));
                         float z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / X));
                         float s = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / X)) / 10.f;

                         float q1 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / Y));
                         float q2 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / Y));
                         float q3 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / Y));

                         char namebuf[16];
                         snprintf(namebuf, 16, "%s%d", "box", entities.size());
                         rp3d::Quaternion q = rp3d::Quaternion(q1, q2, q3, 1.0f);
                         q.normalize();
                         entity::Entity box(namebuf, glm::vec3(x, y, z), q, entity::BodyType::RIGID);
                         box.set_mesh("box");
                         box.init_physics(physics_world, &physics_common, mesh_manager.get_mesh("box"));

                         entities.push_back(std::move(box));
                     }
                    break;  
                case SDL_KEYUP:
                    pressed_keys[event.key.keysym.sym] = false;
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

                        glm::vec3 bullet_starting_pos = glm::vec3(camera.get_position()->x + 0.1f, camera.get_position()->y + 0.1f, camera.get_position()->z + 0.1f);

                        entity::Entity bullet("bullet" + std::to_string(volatile_entities.size() + 1024), 
                            bullet_starting_pos, 
                            rp3d::Quaternion::fromEulerAngles(rp3d::Vector3(camera.get_front().x, camera.get_front().y, camera.get_front().z)),
                            entity::BodyType::RIGID);
                        bullet.set_mesh("bullet");
                        bullet.init_physics(physics_world, &physics_common, mesh_manager.get_mesh("bullet"), 0.1f);
                        bullet.set_force(camera.get_front() * 10.f);

                        volatile_entities.push_back(std::move(bullet));
                    }
                    break;
            }

            ImGui_ImplSDL2_ProcessEvent(&event);
        }

        // ===== Process movement input
        if (pressed_keys[SDLK_w] && pressed_keys[SDLK_d]) {
            camera.process_input(camera::MovementDirection::FRONT_RIGHT);
        } else if (pressed_keys[SDLK_w] && pressed_keys[SDLK_a]) {
            camera.process_input(camera::MovementDirection::FRONT_LEFT);
        } else if (pressed_keys[SDLK_s] && pressed_keys[SDLK_a]) {
            camera.process_input(camera::MovementDirection::BACK_RIGHT);
        } else if (pressed_keys[SDLK_s] && pressed_keys[SDLK_d]) {
            camera.process_input(camera::MovementDirection::BACK_LEFT);
        } else if (pressed_keys[SDLK_w]) {
            camera.process_input(camera::MovementDirection::FRONT);
        } else if (pressed_keys[SDLK_s]) {
            camera.process_input(camera::MovementDirection::BACK);
        } else if (pressed_keys[SDLK_a]) {
            camera.process_input(camera::MovementDirection::LEFT);
        } else if (pressed_keys[SDLK_d]) {
            camera.process_input(camera::MovementDirection::RIGHT);
        } else if (pressed_keys[SDLK_q]) {
            camera.process_input(camera::MovementDirection::UP);
        } else if (pressed_keys[SDLK_z]) {
            camera.process_input(camera::MovementDirection::DOWN);
        } else {
            camera.process_input(camera::MovementDirection::NONE);
        }

        // ===== App settings
        camera.update_settings(&app_settings);

        physics_debug_renderer.update_settings(&app_settings);

        // ===== Logic update
        const float dt_ms = dt / 1000000.f;
        while(accumulator >= dt) {
            camera.update(dt_ms);
            player_physics_body->setTransform(
                rp3d::Transform(rp3d::Vector3(camera.get_position()->x, camera.get_position()->y, camera.get_position()->z), 
                rp3d::Quaternion::fromEulerAngles(rp3d::Vector3(camera.get_front().x, camera.get_front().y, camera.get_front().z))));

            gun.update(dt_ms);
            
            for (auto &entity : entities) {
                entity.update(dt_ms);
            }
            for (auto& entity : volatile_entities) {
                entity.update(dt_ms);
            }

            physics_world->update(dt_ms);

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
        shader_manager.set_mat4("gun", "model", gun.get_model_matrix());
        mesh_manager.draw_mesh("gun");

        for (auto &entity : entities) {
            if (strcmp(entity.get_name(), "plane") == 0) {
                texture_manager.use_texture("grass", shader_manager.get_shader("main").get_id());
            }
            else {
                texture_manager.use_texture("metal", shader_manager.get_shader("main").get_id());
            }
         
            shader_manager.use_shader("main");
            shader_manager.set_mat4("main", "projection", proj);
            shader_manager.set_mat4("main", "view", view);
            shader_manager.set_vec3("main", "light_pos", light);
            shader_manager.set_vec3("main", "view_pos", *camera.get_position());
            shader_manager.set_vec3("main", "light_color", glm::vec3(1.f, 1.f, 1.f));
            shader_manager.set_vec3("main", "object_color", glm::vec3(0.1f, 0.5f, 0.3f)); 
            shader_manager.set_mat4("main", "model", entity.get_model_matrix(interp));
            
            mesh_manager.draw_mesh(entity.get_mesh_name());
        }

        // TODO: Consolidate that to one draw loop
        for (auto& entity : volatile_entities) {
            if (strcmp(entity.get_name(), "plane") == 0) {
                texture_manager.use_texture("grass", shader_manager.get_shader("main").get_id());
            }
            else {
                texture_manager.use_texture("metal", shader_manager.get_shader("main").get_id());
            }

            shader_manager.use_shader("main");
            shader_manager.set_mat4("main", "projection", proj);
            shader_manager.set_mat4("main", "view", view);
            shader_manager.set_vec3("main", "light_pos", light);
            shader_manager.set_vec3("main", "view_pos", *camera.get_position());
            shader_manager.set_vec3("main", "light_color", glm::vec3(1.f, 1.f, 1.f));
            shader_manager.set_vec3("main", "object_color", glm::vec3(0.1f, 0.5f, 0.3f));
            shader_manager.set_mat4("main", "model", entity.get_model_matrix(interp));

            mesh_manager.draw_mesh(entity.get_mesh_name());
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

        if (physics_world->getIsDebugRenderingEnabled()) {
            physics_debug_renderer.draw(shader_manager.get_shader("physics_debug"), camera, interp);
        }

        // Skybox
        skybox.draw(proj, view);

        // Debug GUI 
        const char* entities_names[1024];
        {
            assert(entities.size() <= 1024);
            uint32_t i = 0, j = 0;
            for (i = 0; i < entities.size(); i++) {
                entities_names[i] = entities[i].get_name();
            }
            for (j = 0; j < volatile_entities.size(); j++) {
                entities_names[i + j] = volatile_entities[j].get_name();
            }
        }

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
        ImGui::Spacing();
        ImGui::Checkbox("Physics debug", &app_settings.physics_debug_draw);
        ImGui::End();
        ImGui::Begin("Entities");
        ImGui::PushItemWidth(-1);
        ImGui::ListBox("", &listbox_item_current, entities_names, entities.size() + volatile_entities.size(), 12);
        ImGui::Separator();
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);

        last_render_time = (frame_timer.get_microseconds_since_start() / 1000.f) + update_time;
    }

    // Cleanup
    CLOG_DEBUG("Unloading managers");
    shader_manager.unload();
    texture_manager.unload();
    mesh_manager.unload();

    CLOG_DEBUG("Unloading entities");
    for (auto &entity : entities) {
        entity.unload(physics_world);
    }
    for (auto& entity : volatile_entities) {
        entity.unload(physics_world);
    }

    for (auto light_cube_mesh : light_cubes) {
        light_cube_mesh.unload();
    }

    gun.unload(physics_world);

    skybox.unload();

    CLOG_DEBUG("Unloading rp3d");
    physics_common.destroyPhysicsWorld(physics_world);

    CLOG_DEBUG("Shutting down ImGui");
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    CLOG_DEBUG("Shutting down GL context and SDL");
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    CLOG_INFO("Quitting Bullseye");

    return EXIT_SUCCESS;
}