#pragma once

#include <SDL3/SDL.h>

namespace Scam {

class FrameCounter {
public:
  FrameCounter();

public:
  void Update();

public:
  [[nodiscard]] double GetLastDeltaTime() const;
  [[nodiscard]] uint32_t GetFps() const;

private:
  SDL_Time last_tick = 0;
  SDL_Time delta_time = 0;
  uint32_t fps = 0;
  uint32_t frame_counter = 0;
  SDL_Time frame_timer = 0;
};

} // namespace Scam
