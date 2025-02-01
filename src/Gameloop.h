#pragma once

#include <SDL3/SDL.h>

namespace Scam {

struct Settings {
  float volume_music = .1f;
  float volume_sound = 1.f;
};

void main_loop(SDL_Window* window);

} // namespace Scam
