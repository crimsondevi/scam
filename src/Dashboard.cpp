#include "Dashboard.h"

#include "Prelude.h"
#include "SoundSystem.h"
#include "sim/Simulation.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <implot.h>
#include <implot_internal.h>
#include <vector>

namespace ImGui {

void TextCenter(std::string_view view) {
  float font_size = ImGui::GetFontSize() * static_cast<float>(view.size()) / 2.f;
  ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2 - font_size + (font_size / 2));
  ImGui::Text("%s", view.data());
}

} // namespace ImGui

namespace Scam {

Dashboard::Dashboard(ImFont* big_font) : big_font(big_font) {
  window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;

  sound_system = std::make_unique<SoundSystem>();

  sound_system->PlayMusic();

  test_texture.LoadFromFile(std::filesystem::current_path() / "data" / "bitcoin.png");
}

void Dashboard::Update(ScamSim& scam_sim) {
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
    ImGui::TextCenter(scam_sim.GetCoinState().coin->name);
    ImGui::TextCenter(scam_sim.GetCoinState().coin->code);
    ImGui::PopFont();

    std::string real_money_str = std::format("Cash - ${:.2f}", scam_sim.GetRealMoney());
    ImGui::SeparatorText(real_money_str.c_str());
    std::string fake_money_str =
        std::format("{} - {:.2f}", scam_sim.GetCoinState().coin->code, scam_sim.GetFakeMoney());
    ImGui::SeparatorText(fake_money_str.c_str());

    ImGui::SeparatorText("Market Value");
    ImGui::PushFont(big_font);
    ImGui::TextCenter(std::format("${:.2f}", scam_sim.GetCoinState().value));
    ImGui::PopFont();
    ImGui::TextCenter(std::format("Trend: {:+.4f}", scam_sim.GetCoinState().value_delta));

    ImGui::SeparatorText("Hype");
    float hype = scam_sim.GetCoinState().hype;
    ImGui::SliderFloat("##Hype", &hype, -1.f, 1.f, "%.2f", ImGuiSliderFlags_ReadOnly);

    ImGui::SeparatorText("Volatility");
    float volatility = scam_sim.GetCoinState().volatility;
    ImGui::SliderFloat("##Volatility", &volatility, 0.f, 10.f, "%.2f", ImGuiSliderFlags_ReadOnly);

    ImGui::PopItemWidth();
    ImGui::End();
  }

  // Update graph

  if (scam_sim.GetCurrentStep() == 0) {
    x_data.emplace_back(updated_days);
    y_data.emplace_back(scam_sim.GetCoinState().value);
  }

  while (scam_sim.GetCurrentStep() > updated_days) {
    updated_days++;

    x_data.emplace_back(updated_days);
    y_data.emplace_back(scam_sim.GetCoinState().value);

    for (const auto& event : scam_sim.GetEvents()) {
      if (event->type == EventType::Shop && updated_days == event->day) {
        LOG_INFO("Event occurred on day: {}", event->day);
        pre_event_speed_multiplier = speed_multiplier;
        speed_multiplier = 0;
        current_event = event.get();
      }
    }
  }

  if (current_event) {
    ImGui::OpenPopup("Event");

    if (ImGui::BeginPopupModal("Event", nullptr, ImGuiWindowFlags_Modal | ImGuiWindowFlags_AlwaysAutoResize)) {
      bool bought = false;
      size_t chosen_item_index = -1;

      for (int i = 0; i < current_event->items.size(); i++) {
        const auto& item = *current_event->items[i];

        const std::string item_label = std::format("Buy##{}", i);

        ImGui::BeginGroup();
        if (TextureData texture_data; test_texture.GetTextureData(texture_data)) {
          ImGui::Image((ImTextureID)(intptr_t)texture_data.tex, ImVec2(64.f, 64.f));
        }
        ImGui::Text("%s", item.GetInterfaceData().name.c_str());
        if (ImGui::Button(item_label.c_str())) {
          chosen_item_index = i;
          bought = true;
        }
        ImGui::EndGroup();

        if (ImGui::IsItemHovered()) {
          if (ImGui::BeginTooltip()) {
            ImGui::Text("%s", item.GetInterfaceData().description.c_str());
            ImGui::EndTooltip();
          }
        }
      }
      if (bought) {
        auto item = std::move(current_event->items.at(chosen_item_index));
        scam_sim.AddItem(std::move(item));
        current_event = nullptr;
        speed_multiplier = pre_event_speed_multiplier;
      }
      ImGui::End();
    }
  }

  // Market window
  {
    ImGui::SetNextWindowClass(&window_class);
    ImGui::Begin("Market");
    if (ImPlot::BeginPlot("Market", ImVec2(-1, -1), ImPlotAxisFlags_AutoFit)) {
      float x_min = std::max(x_data.back() - 365.f / 2.f, 0.f);
      float x_max = x_data.back() + 365.f / 2.f;
      float y_min = std::max(scam_sim.GetCoinState().value - 500.f, -20.f);
      float y_max = *std::max_element(y_data.begin() + std::max(y_data.size()-200.f, 0.f), y_data.end()) + 200.f;
      ImPlot::SetupAxes("Day", "Stonks", ImPlotAxisFlags_None, ImPlotAxisFlags_Opposite);
      if (speed_multiplier > 0) {
        ImPlot::SetupAxisLimits(ImAxis_X1, x_min, x_max, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, y_min, y_max, ImGuiCond_Always);
      }
      ImPlot::SetNextLineStyle(ImVec4(.6f, .4f, .1f, 1.f), 2.f);
      ImPlot::PlotLine("BUBL", x_data.data(), y_data.data(), (int)x_data.size());
      ImPlot::SetNextLineStyle(ImVec4(1.f, 1.f, 1.f, 1.f), 2.f);
      ImPlot::PlotInfLines("##Today", &x_data.back(), 1);
      ImPlot::PlotText("Today", x_data.back(), 0.f, ImVec2(16.f, -80.f), ImPlotTextFlags_Vertical);

      const float bubble_threshold = scam_sim.GetBubbleThreshold();
      ImPlot::PlotInfLines("##Bubble Threshold", &bubble_threshold, 1, ImPlotInfLinesFlags_Horizontal);
      ImPlot::PlotText("Bubble Threshold", x_data.back(), bubble_threshold, ImVec2(0.f, 16.f));

      for (const auto& event : scam_sim.GetEvents()) {
        if (event->day >= x_min && event->day <= x_max) {
          const std::string day_name = std::format("##event_day_{}", event->day);
          const auto day = static_cast<float>(event->day);
          auto line_color = event->type == EventType::Audit ? ImVec4(1.f, 0.f, 0.f, 1.f) : ImVec4(0.f, 1.f, 0.f, 1.f);
          ImPlot::SetNextLineStyle(line_color, 2.f);
          ImPlot::PlotInfLines(day_name.c_str(), &day, 1);
          ImPlot::PlotText(event->name.c_str(), day, 0.f, ImVec2(16.f, 0.f), ImPlotTextFlags_Vertical);
        }
      }

      ImPlot::EndPlot();
    }
    ImGui::End();
  }

  // Action window
  {
    ImGui::SetNextWindowClass(&window_class);
    ImGui::Begin("Action");

    ImGui::Text("Day: %u", scam_sim.GetCurrentStep());

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

    if (ImGui::Button("Buy", {200.f, 100.f})) {
      if (scam_sim.AddTradeOrder(1.f)) {
        sound_system->PlaySound(SoundCue::Click);
      }
    }

    ImGui::SameLine();

    if (ImGui::Button("Sell", {200.f, 100.f})) {
      if (scam_sim.AddTradeOrder(-1.f)) {
        sound_system->PlaySound(SoundCue::Click);
      }
    }

    ImGui::SameLine();

    // if (ImGui::Button("Dump")) {
    //   sound_system->PlaySound(SoundCue::Purchase);
    // }

    ImGui::PopFont();
    ImGui::EndGroup();

    button_group_size = ImGui::GetItemRectSize();

    ImGui::End();
  }
}

void Dashboard::ApplySettings(const Settings& settings) {
  sound_system->SetMusicVolume(settings.volume_music);
  sound_system->SetSoundVolume(settings.volume_sound);
}

int Dashboard::GetSpeedMultiplier() const {
  return speed_multiplier;
}

} // namespace Scam
