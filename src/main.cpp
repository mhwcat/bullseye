#include <cstdlib>

#include "logger.h"

#include "SDL.h"
#include "glad/glad.h"

const int WIDTH = 1280;
const int HEIGHT = 720;

int main(int argc, char *argv[]) {
    logger::info("Starting Bullseye!");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        logger::error("Failed initializing SDL! Error: %s", SDL_GetError());
    }

    SDL_version sdl_version;
    SDL_GetVersion(&sdl_version);
    logger::debug("SDL version: %d.%d.%d", sdl_version.major, sdl_version.minor, sdl_version.patch);

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

    bool running = true;
    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }

        glViewport(0, 0, WIDTH, HEIGHT);
        glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
    }

    SDL_Quit();
    
    logger::info("Quitting Bullseye!");

    return EXIT_SUCCESS;
}