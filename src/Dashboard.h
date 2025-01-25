#pragma once

#include "Coin.h"
#include "SoundSystem.h"
#include "Texture.h"

#include <imgui.h>

namespace Scam {

class Dashboard {
public:
  Dashboard();

public:
  void Update(const Coin& coin);

public:
  [[nodiscard]] int GetSpeedMultiplier() const;

private:
  ImFont* big_font = nullptr;
  std::vector<float> x_data;
  std::vector<float> y_data;
  uint32_t updated_days = 0;

private:
  ImGuiWindowClass window_class;

private:
  std::unique_ptr<SoundSystem> sound_system;

  Texture test_texture;

  int speed_multiplier = 1;
};

} // namespace Scam
