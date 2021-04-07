#ifndef BULLSEYE_HUD_H
#define BULLSEYE_HUD_H

#include "shader.h"

namespace bullseye::hud {
    class Hud {
        public:
            Hud();
            void draw();
            
        private:
            uint32_t vao;
            
            void init();
            void draw_crosshair();
    };
}

#endif