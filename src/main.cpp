#include <cstdlib>
#include "SDL.h"
#include "glad/glad.h"

#include "logger.h"
#include "shader.h"

const int WIDTH = 1280;
const int HEIGHT = 720;

using namespace bullseye;

int main(int argc, char *argv[]) {
    logger::info("Starting Bullseye!");

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

    SDL_Window* window = SDL_CreateWindow("Bullseye", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == NULL) {
        logger::error("Failed initializing window! Error: %s", SDL_GetError());
        return EXIT_FAILURE;
    }
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

    logger::debug("OpenGL info [Vendor: %s, Renderer: %s, Version: %s]", glGetString(GL_VENDOR), 
        glGetString(GL_RENDERER), glGetString(GL_VERSION));

    shader::Shader shader("main");
    shader.load_vertex_shader("assets/shaders/vertex.glsl");
    shader.load_fragment_shader("assets/shaders/fragment.glsl");

    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertex_shader_src = shader.get_vertex_shader_src();
    glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
    glCompileShader(vertex_shader);

    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        logger::error("Failed to compile vertex shader: %s", info_log);
    }

    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragment_shader_src = shader.get_fragment_shader_src();
    glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        logger::error("Failed to compile fragment shader: %s", info_log);
    }

    uint32_t shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        logger::error("Failed to link shader program: %s", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    }; 

    uint32_t vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    bool running = true;
    while (running) {
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
            }
        }

        glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        int color_loc = glGetUniformLocation(shader_program, "in_color");
        glUniform4f(color_loc, 0.3f, 0.2f, 0.0f, 0.1f);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader_program);

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    logger::info("Quitting Bullseye!");

    return EXIT_SUCCESS;
}