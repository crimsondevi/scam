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

private:
  ImFont* big_font = nullptr;
  std::vector<float> x_data;
  std::vector<float> y_data;

private:
  ImGuiWindowClass window_class;

private:
  std::unique_ptr<SoundSystem> sound_system;

  Texture test_texture;
};

} // namespace Scam
