#pragma once

#include "Coin.h"

#include <imgui.h>
#include <random>

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
};

} // namespace Scam
