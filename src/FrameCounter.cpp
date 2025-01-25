#include "FrameCounter.h"

namespace Scam {

FrameCounter::FrameCounter() {
  SDL_GetCurrentTime(&last_tick);
}

void FrameCounter::Update() {
  SDL_Time current_tick;
  SDL_GetCurrentTime(&current_tick);
  delta_time = current_tick - last_tick;
  last_tick = current_tick;
  frame_timer += delta_time;

  frame_counter++;

  if (frame_timer >= 1000 * 1000 * 1000) {
    frame_timer -= 1000 * 1000 * 1000;
    fps = frame_counter;
    frame_counter = 0;
  }
}

double FrameCounter::GetLastDeltaTime() const {
  return static_cast<double>(delta_time) / (1000.0 * 1000.0 * 1000.0);
}

uint32_t FrameCounter::GetFps() const {
  return fps;
}

} // namespace Scam