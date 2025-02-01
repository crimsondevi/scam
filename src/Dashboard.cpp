#include "Dashboard.h"

#include "Prelude.h"
#include "SoundSystem.h"
#include "sim/Simulation.h"

#include <IconsFontAwesome6.h>

#include <imgui_internal.h>
#include <implot.h>
#include <vector>

namespace ImGui {

void TextCenter(std::string_view view) {
  ImVec2 size = ImGui::CalcTextSize(view.data(), nullptr, true);
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x / 2 - (size.x / 2));
  ImGui::Text("%s", view.data());
}

void ImagePadded(GLuint tex, float w, float h, ImVec2 padding) {
  auto size = ImGui::GetContentRegionAvail() - padding * 2.f;
  auto final_size = Scam::ConstrainToAspectRatio(size, w / h);
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x / 2.f - final_size.x / 2.f);
  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + padding.y);
  ImGui::Image((ImTextureID)(intptr_t)tex, final_size);
  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + padding.y);
}

} // namespace ImGui

namespace Scam {

Dashboard::Dashboard(const Settings& settings) {
  big_font = ImGui::GetIO().Fonts->Fonts[1];

  window_class = std::make_unique<ImGuiWindowClass>();
  window_class->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;

  sound_system = std::make_unique<SoundSystem>();
  sound_system->SetMusicVolume(settings.volume_music);
  sound_system->SetSoundVolume(settings.volume_sound);
  sound_system->PlayMusic();

  test_texture.LoadFromFile(std::filesystem::current_path() / "data" / "bitcoin.png");
}

void Dashboard::Update(float delta_time, ScamSim& scam_sim) {
  ImGuiID dockspace_id = ImGui::DockSpaceOverViewport();

  static bool init = false;

  if (!init) {
    init = true;

    ImGui::DockBuilderRemoveNodeChildNodes(dockspace_id);

    ImGuiID dock_id_up, dock_id_bottom;
    ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, .75f, &dock_id_up, &dock_id_bottom);

    ImGuiID dock_id_up_left, dock_id_up_right;
    ImGui::DockBuilderSplitNode(dock_id_up, ImGuiDir_Left, .2f, &dock_id_up_left, &dock_id_up_right);

    ImGuiID dock_id_bottom_left, dock_id_bottom_right;
    ImGui::DockBuilderSplitNode(dock_id_bottom, ImGuiDir_Left, .2f, &dock_id_bottom_left, &dock_id_bottom_right);

    ImGuiID dock_id_bottom_mid;
    const float x = .6f * (1.f / (1.f - .2f));
    ImGui::DockBuilderSplitNode(dock_id_bottom_right, ImGuiDir_Left, x, &dock_id_bottom_mid, &dock_id_bottom_right);

    ImGui::DockBuilderDockWindow("Status", dock_id_up_left);
    ImGui::DockBuilderDockWindow("Market", dock_id_up_right);
    ImGui::DockBuilderDockWindow("Wallet", dock_id_bottom_left);
    ImGui::DockBuilderDockWindow("Action", dock_id_bottom_mid);
    ImGui::DockBuilderDockWindow("Time", dock_id_bottom_right);

    ImGui::DockBuilderFinish(dockspace_id);
  }

  // Status window
  {
    ImGui::SetNextWindowClass(window_class.get());

    ImGui::Begin("Status");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    if (TextureData texture_data; test_texture.GetTextureData(texture_data)) {
      ImGui::ImagePadded(texture_data.tex, texture_data.w, texture_data.h, ImVec2(16.f, 4.f));
    }
    ImGui::PushFont(big_font);
    ImGui::TextCenter(scam_sim.GetCoinState().coin->name);
    ImGui::PopFont();

    ImGui::SeparatorText(ICON_FA_COINS " Value");
    ImGui::PushFont(big_font);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 0.f, 1.f));
    ImGui::TextCenter(std::format("${:.2f}", scam_sim.GetCoinState().value));
    ImGui::PopStyleColor();
    ImGui::PopFont();

    const ImVec4 trend_color =
        scam_sim.GetCoinState().value_delta > 0.f ? ImVec4(0.f, 1.f, 0.f, 1.f) : ImVec4(1.f, 0.f, 0.f, 1.f);
    ImGui::PushStyleColor(ImGuiCol_Text, trend_color);
    const char* icon = scam_sim.GetCoinState().value_delta > 0.f ? ICON_FA_ARROW_UP : ICON_FA_ARROW_DOWN;
    ImGui::TextCenter(std::format("Trend: {:+.2f} {}", scam_sim.GetCoinState().value_delta, icon));
    ImGui::PopStyleColor();

    ImGui::SeparatorText(ICON_FA_ARROW_TREND_UP " Hype");
    float hype = scam_sim.GetCoinState().hype;
    ImGui::SliderFloat("##Hype", &hype, -30.f, 30.f, "%.2f", ImGuiSliderFlags_ReadOnly);

    ImGui::SeparatorText(ICON_FA_BOLT " Volatility");
    float volatility = scam_sim.GetCoinState().volatility;
    ImGui::SliderFloat("##Volatility", &volatility, 0.f, 10.f, "%.2f", ImGuiSliderFlags_ReadOnly);

    ImGui::PopItemWidth();
    ImGui::End();
  }

  // Wallet window
  {
    ImGui::SetNextWindowClass(window_class.get());
    ImGui::Begin("Wallet");
    ImGui::SeparatorText(ICON_FA_WALLET " Wallet");
    ImGui::Text("Cash: $%.2f", scam_sim.GetRealMoney());
    ImGui::Text("%s: %.2f", scam_sim.GetCoinState().coin->code.c_str(), scam_sim.GetFakeMoney());
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

    if (ImGui::BeginPopupModal("Event",
                               nullptr,
                               ImGuiWindowFlags_Modal | ImGuiWindowFlags_AlwaysAutoResize |
                                   ImGuiWindowFlags_NoDecoration)) {
      bool bought = false;
      size_t chosen_item_index = -1;

      ImGui::BeginTable("Test", 4, ImGuiTableFlags_None, ImVec2(-1.f, -1.f));

      for (int i = 0; i < current_event->items.size(); i++) {
        ImGui::TableNextColumn();

        const auto& item = *current_event->items[i];

        const std::string item_label = std::format("Choose##{}", i);

        ImGui::BeginGroup();
        if (TextureData texture_data; test_texture.GetTextureData(texture_data)) {
          ImGui::ImagePadded((ImTextureID)(intptr_t)texture_data.tex, 128.f, 128.f, ImVec2(16.f, 4.f));
        }
        ImGui::TextCenter(item.GetInterfaceData().name);
        ImGui::Spacing();

        if (ImGui::Button(item_label.c_str(), ImVec2(192.f, 48.f))) {
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

      ImGui::EndTable();

      ImGui::End();

      if (bought) {
        auto item = std::move(current_event->items.at(chosen_item_index));
        scam_sim.AddItem(std::move(item));
        current_event = nullptr;
        speed_multiplier = pre_event_speed_multiplier;
      }
    }
  }

  // Market window
  {
    ImGui::SetNextWindowClass(window_class.get());
    ImGui::Begin("Market");
    if (ImPlot::BeginPlot(ICON_FA_MONEY_BILL_TREND_UP " Market", ImVec2(-1, -1), ImPlotAxisFlags_AutoFit)) {
      float x_min = std::max(x_data.back() - 365.f / 2.f, 0.f);
      float x_max = x_data.back() + 365.f / 2.f;
      float y_min = std::min(std::max(scam_sim.GetCoinState().value - 2000.f, -20.f),
                             static_cast<float>(scam_sim.GetBubbleThreshold()));
      const uint32_t offset = std::max(static_cast<int>(y_data.size()) - 200, 0);
      float y_max = *std::max_element(y_data.begin() + offset, y_data.end()) + 500.f;
      float y_center = std::lerp(y_max, y_min, .5f);

      ImPlot::SetupAxes(
          "Day", "Stonks", ImPlotAxisFlags_NoHighlight, ImPlotAxisFlags_Opposite | ImPlotAxisFlags_NoHighlight);
      if (speed_multiplier > 0) {
        ImPlot::SetupAxisLimits(ImAxis_X1, x_min, x_max, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, y_min, y_max, ImGuiCond_Always);
      }

      // Draw bubble threshold

      const double bubble_threshold = scam_sim.GetBubbleThreshold();
      ImPlot::SetNextLineStyle(ImVec4(1.f, 0.f, 0.f, 1.f), 2.f);
      ImPlot::PlotInfLines("##Bubble", &bubble_threshold, 1, ImPlotInfLinesFlags_Horizontal);
      ImPlot::PushStyleColor(ImPlotCol_InlayText, ImVec4(1.f, 0.f, 0.f, 1.f));
      ImPlot::PlotText("Bubble", x_data.back(), bubble_threshold, ImVec2(50.f, -30.f));
      ImPlot::PopStyleColor();

      // Draw events

      for (const auto& event : scam_sim.GetEvents()) {
        const auto day = static_cast<float>(event->day);
        if (day >= x_min && day <= x_max) {
          const std::string day_name = std::format("##event_day_{}", event->day);
          auto line_color = event->type == EventType::Audit ? ImVec4(1.f, 0.f, 1.f, 1.f) : ImVec4(0.f, 1.f, 0.f, 1.f);
          ImPlot::SetNextLineStyle(line_color, 2.f);
          ImPlot::PlotInfLines(day_name.c_str(), &day, 1);
          ImPlot::PlotText(event->name.c_str(), day, y_center, ImVec2(16.f, 0.f), ImPlotTextFlags_Vertical);
        }
      }

      // Draw stonks

      ImPlot::SetNextLineStyle(ImVec4(.8f, .6f, .1f, 1.f), 2.f);
      ImPlot::SetNextFillStyle(ImVec4(.8f, .6f, .1f, .2f));
      ImPlot::PlotLine(scam_sim.GetCoinState().coin->code.c_str(),
                       x_data.data(),
                       y_data.data(),
                       (int)x_data.size(),
                       ImPlotLineFlags_Shaded);
      ImPlot::SetNextLineStyle(ImVec4(1.f, 1.f, 1.f, 1.f), 2.f);
      ImPlot::PlotInfLines("##Today", &x_data.back(), 1);
      ImPlot::PlotText("Today", x_data.back(), y_max, ImVec2(50.f, 30.f));
      ImPlot::SetNextMarkerStyle(
          ImPlotMarker_Circle, 10.f, ImVec4(.8f, .6f, .1f, 1.f), 2.f, ImVec4(1.f, 1.f, 1.f, 1.f));
      ImPlot::PlotScatter("##Coin", x_data.data() + (x_data.size() - 1), y_data.data() + (x_data.size() - 1), 1);

      ImPlot::EndPlot();
    }
    ImGui::End();
  }

  // Time Window
  {
    ImGui::SetNextWindowClass(window_class.get());
    ImGui::Begin("Time");

    ImGui::BeginGroup();

    const auto day_label = std::format(ICON_FA_CALENDAR " Day {}", scam_sim.GetCurrentStep());
    ImGui::SeparatorText(day_label.c_str());

    ImGui::BeginGroup();
    ImGui::RadioButton(ICON_FA_PAUSE, &speed_multiplier, 0);
    ImGui::SameLine();
    ImGui::RadioButton(ICON_FA_PLAY, &speed_multiplier, 1);
    ImGui::SameLine();
    ImGui::RadioButton(ICON_FA_FORWARD, &speed_multiplier, 4);
    ImGui::SameLine();
    ImGui::RadioButton(ICON_FA_FORWARD_FAST, &speed_multiplier, 16);
    ImGui::EndGroup();

    ImGui::EndGroup();

    ImGui::End();
  }

  // Action window
  {
    ImGui::SetNextWindowClass(window_class.get());
    ImGui::Begin("Action");

    static ImVec2 button_group_size;
    static ImVec2 total_group_size;

    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - button_group_size.x) / 2.f);
    ImGui::SetCursorPosY((ImGui::GetContentRegionAvail().y - total_group_size.y) / 2.f);

    ImGui::BeginGroup();

    ImGui::PushItemWidth(button_group_size.x);
    int trade_wish = scam_sim.GetTradeWish();
    ImGui::SliderInt("##Trade Wish", &trade_wish, -30, 30, "%d", ImGuiSliderFlags_ReadOnly);

    int trade = scam_sim.GetProcessedTrades();
    const auto [max_buy_orders, max_sell_orders] = scam_sim.GetMaxBuySellOrders();
    ImGui::PushStyleColor(ImGuiCol_Border, trade > 0 ? ImVec4(0.f, 1.f, 0.f, 1.f) : ImVec4(1.f, 0.f, 0.f, 1.f));
    ImGui::SliderInt("##Trade",
                     &trade,
                     -static_cast<int>(max_sell_orders),
                     static_cast<int>(max_buy_orders),
                     "%d",
                     ImGuiSliderFlags_ReadOnly);
    ImGui::PopStyleColor();
    ImGui::PopItemWidth();

    ImGui::BeginGroup();
    ImGui::PushFont(big_font);

    ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
    if (ImGui::Button(ICON_FA_MONEY_BILLS " Buy", {250.f, 100.f})) {
      combo_multiplier = combo_action != Action::Buy ? 1 : combo_multiplier;

      if (scam_sim.AddTradeOrder(combo_multiplier)) {
        sound_system->PlaySound(SoundCue::Purchase);

        combo_multiplier += 1;
        combo_action = Action::Buy;
        combo_timer = combo_timer_reset_threshold;
      } else {
        sound_system->PlaySound(SoundCue::Fail);
      }
    }
    ImGui::PopItemFlag();

    ImGui::SameLine();

    ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
    if (ImGui::Button(ICON_FA_SACK_DOLLAR " Sell", {250.f, 100.f})) {
      combo_multiplier = combo_action != Action::Sell ? 1 : combo_multiplier;

      if (scam_sim.AddTradeOrder(-combo_multiplier)) {
        sound_system->PlaySound(SoundCue::Click);

        combo_multiplier += 1;
        combo_action = Action::Sell;
        combo_timer = combo_timer_reset_threshold;
      } else {
        sound_system->PlaySound(SoundCue::Fail);
      }
    }
    ImGui::PopItemFlag();

    combo_timer -= delta_time;

    if (combo_timer <= 0.f) {
      combo_timer = 0.f;
      combo_multiplier = 1;
    }

    ImGui::PopFont();

    ImGui::EndGroup();

    button_group_size = ImGui::GetItemRectSize();

    ImGui::EndGroup();

    total_group_size = ImGui::GetItemRectSize();

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

void Dashboard::SetSpeedMultiplier(int in_speed_multiplier) {
  speed_multiplier = in_speed_multiplier;
}

} // namespace Scam
