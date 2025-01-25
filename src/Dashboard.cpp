#include "Dashboard.h"

#include "SoundSystem.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <implot.h>
#include <vector>

namespace Scam {

Dashboard::Dashboard() {
  const ImGuiIO& io = ImGui::GetIO();
  ImFontConfig font_cfg;
  font_cfg.SizePixels = 52.f;
  big_font = io.Fonts->AddFontDefault(&font_cfg);

  for (int i = 0; i < 365; i++) {
    x_data.emplace_back(i);
    y_data.emplace_back(0);
  }

  window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;

  sound_system = std::make_unique<SoundSystem>();

  sound_system->PlayMusic();
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
    ImGui::DockBuilderSplitNode(dock_id_up, ImGuiDir_Left, .25f, &dock_id_up_left, &dock_id_up_right);

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

    ImGui::SeparatorText("Currency");
    ImGui::PushFont(big_font);
    ImGui::Text("%s", coin.name.c_str());
    ImGui::PopFont();

    ImGui::SeparatorText("Market Value");
    ImGui::PushFont(big_font);
    ImGui::Text("$%.2f", coin.stonks);
    ImGui::PopFont();
    ImGui::Text("Trend: %+f", coin.stonks_delta);

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

  y_data.erase(y_data.begin());
  y_data.emplace_back(coin.stonks);

  // Market window
  {
    ImGui::SetNextWindowClass(&window_class);
    ImGui::Begin("Market");
    if (ImPlot::BeginPlot("Market", ImVec2(-1, -1))) {
      ImPlot::SetupAxes(nullptr, nullptr, 0, 0);
      ImPlot::SetupAxisLimits(ImAxis_X1, 0.f, 365.f, ImGuiCond_Always);
      ImPlot::SetupAxisLimits(ImAxis_Y1,
                              *std::min_element(y_data.begin(), y_data.end()),
                              *std::max_element(y_data.begin(), y_data.end()),
                              ImPlotCond_Always);
      ImPlot::PlotLine("Stonks", x_data.data(), y_data.data(), (int)x_data.size());
      ImPlot::EndPlot();
    }
    ImGui::End();
  }
  // Action window
  {
    ImGui::SetNextWindowClass(&window_class);
    ImGui::Begin("Action");

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

} // namespace Scam