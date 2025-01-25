#include "Dashboard.h"

#include "Prelude.h"
#include "SoundSystem.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <implot.h>
#include <vector>

namespace ImGui {

void TextCenter(std::string_view view) {
  float font_size = ImGui::GetFontSize() * static_cast<float>(view.size()) / 2.f;
  ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2 - font_size + (font_size / 2));
  ImGui::Text("%s", view.data());
}

} // namespace ImGui

namespace Scam {

Dashboard::Dashboard() {
  const ImGuiIO& io = ImGui::GetIO();
  ImFontConfig font_cfg;
  font_cfg.SizePixels = 52.f;
  big_font = io.Fonts->AddFontDefault(&font_cfg);

  window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;

  sound_system = std::make_unique<SoundSystem>();

  sound_system->PlayMusic();

  test_texture.LoadFromFile(std::filesystem::current_path() / "data" / "bitcoin.png");
}

void Dashboard::Update(const Coin& coin) {
  ImGuiID dockspace_id = ImGui::DockSpaceOverViewport();

  static bool init = false;

  if (!init) {
    init = true;

    ImGui::DockBuilderRemoveNodeChildNodes(dockspace_id);

    ImGuiID dock_id_up, dock_id_bottom;
    ImGuiID dock_id_up_left, dock_id_up_right;
    ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, .75f, &dock_id_up, &dock_id_bottom);
    ImGui::DockBuilderSplitNode(dock_id_up, ImGuiDir_Left, .2f, &dock_id_up_left, &dock_id_up_right);

    ImGui::DockBuilderDockWindow("Status", dock_id_up_left);
    ImGui::DockBuilderDockWindow("Market", dock_id_up_right);
    ImGui::DockBuilderDockWindow("Action", dock_id_bottom);

    ImGui::DockBuilderFinish(dockspace_id);
  }

  // Status window
  {
    ImGui::SetNextWindowClass(&window_class);

    ImGui::Begin("Status");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    if (TextureData texture_data; test_texture.GetTextureData(texture_data)) {
      auto size = ImGui::GetContentRegionAvail();
      auto final_size = ConstrainToAspectRatio(size, texture_data.w / texture_data.h);
      ImGui::Image((ImTextureID)(intptr_t)texture_data.tex, final_size);
    }
    ImGui::PushFont(big_font);
    ImGui::TextCenter(coin.name);
    ImGui::TextCenter(coin.code);
    ImGui::PopFont();

    ImGui::SeparatorText("Market Value");
    ImGui::PushFont(big_font);
    ImGui::TextCenter(std::format("${:.2f}", coin.stonks));
    ImGui::PopFont();
    ImGui::TextCenter(std::format("Trend: {:+.4f}", coin.stonks_delta));

    ImGui::SeparatorText("Hype");
    float hype = coin.hype;
    ImGui::SliderFloat("##Hype", &hype, -1.f, 1.f, "%.2f", ImGuiSliderFlags_ReadOnly);

    ImGui::SeparatorText("Volatility");
    float volatility = coin.volatility;
    ImGui::SliderFloat("##Volatility", &volatility, 0.f, 10.f, "%.2f", ImGuiSliderFlags_ReadOnly);

    ImGui::PopItemWidth();
    ImGui::End();
  }

  // Update graph

  if (coin.days == 0) {
    x_data.emplace_back(updated_days);
    y_data.emplace_back(coin.stonks);
  }

  if (coin.days > updated_days) {
    updated_days++;

    x_data.emplace_back(updated_days);
    y_data.emplace_back(coin.stonks);
  }

  // Market window
  {
    ImGui::SetNextWindowClass(&window_class);
    ImGui::Begin("Market");
    if (ImPlot::BeginPlot("Market", ImVec2(-1, -1), ImPlotAxisFlags_AutoFit)) {
      float x_min = std::max(x_data.back() - 365.f, 0.f);
      float x_max = x_data.back();
      ImPlot::SetupAxes("Day", "Stonks", ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_Opposite);
      if (speed_multiplier > 0) {
        ImPlot::SetupAxisLimits(ImAxis_X1, x_min, x_max, ImGuiCond_Always);
      }
      ImPlot::SetNextLineStyle(ImVec4(.6f, .4f, .1f, 1.f), 2.f);
      ImPlot::PlotLine("BUBL", x_data.data(), y_data.data(), (int)x_data.size());
      ImPlot::EndPlot();
    }
    ImGui::End();
  }

  // Action window
  {
    ImGui::SetNextWindowClass(&window_class);
    ImGui::Begin("Action");

    ImGui::Text("Day: %u", coin.days);

    ImGui::BeginGroup();
    ImGui::RadioButton("Pause", &speed_multiplier, 0);
    ImGui::SameLine();
    ImGui::RadioButton("1x", &speed_multiplier, 1);
    ImGui::SameLine();
    ImGui::RadioButton("4x", &speed_multiplier, 4);
    ImGui::SameLine();
    ImGui::RadioButton("16x", &speed_multiplier, 16);
    ImGui::EndGroup();

    static ImVec2 button_group_size;

    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - button_group_size.x) / 2.f);
    ImGui::SetCursorPosY((ImGui::GetContentRegionAvail().y - button_group_size.y) / 2.f);

    ImGui::BeginGroup();
    ImGui::PushFont(big_font);

    if (ImGui::Button("Buy")) {
      sound_system->PlaySound(SoundCue::Click);
    }

    ImGui::SameLine();

    if (ImGui::Button("Sell")) {
      sound_system->PlaySound(SoundCue::Click);
    }

    ImGui::SameLine();

    if (ImGui::Button("Dump")) {
      sound_system->PlaySound(SoundCue::Purchase);
    }

    ImGui::PopFont();
    ImGui::EndGroup();

    button_group_size = ImGui::GetItemRectSize();

    ImGui::End();
  }
}

int Dashboard::GetSpeedMultiplier() const {
  return speed_multiplier;
}

} // namespace Scam
