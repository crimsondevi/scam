#pragma once

#include "Gameloop.h"
#include "SoundSystem.h"
#include "Texture.h"
#include "sim/Simulation.h"

#include <imgui.h>

namespace Scam {

enum class Action {
  None,
  Buy,
  Sell
};

class Dashboard {
public:
  explicit Dashboard(ImFont* big_font);

public:
  void Update(float delta_time, ScamSim& scam_sim);

public:
  void ApplySettings(const Settings& settings);

public:
  [[nodiscard]] int GetSpeedMultiplier() const;
  void SetSpeedMultiplier(int in_speed_multiplier);

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

  Event* current_event = nullptr;
  int pre_event_speed_multiplier = 1;

  int combo_multiplier = 1;
  Action combo_action = Action::None;
  float combo_timer = 0.f;
  float combo_timer_reset_threshold = 1.f;
};

} // namespace Scam
