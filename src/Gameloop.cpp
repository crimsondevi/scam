#include "Gameloop.h"
#include "Prelude.h"

#include <SDL3/SDL_opengl.h>
#include <deque>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <imgui_internal.h>
#include <implot.h>
#include <random>
#include <thread>

namespace Scam {

class FrameCounter {
public:
  FrameCounter() {
    SDL_GetCurrentTime(&last_tick);
  }

public:
  void Update() {
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

  [[nodiscard]] double GetLastDeltaTime() const {
    return static_cast<double>(delta_time) / (1000.0 * 1000.0 * 1000.0);
  }

  [[nodiscard]] uint32_t GetFps() const {
    return fps;
  }

private:
  SDL_Time last_tick = 0;
  SDL_Time delta_time = 0;
  uint32_t fps = 0;
  uint32_t frame_counter = 0;
  SDL_Time frame_timer = 0;
};

void main_loop(SDL_Window* window) {
  bool quit = false;

  SDL_Event e;

  std::vector<float> x_data;
  std::vector<float> y_data;
  SDL_Time time;
  SDL_GetCurrentTime(&time);
  std::default_random_engine generator(time);
  std::mt19937 mt(generator());

  for (int i = 0; i < 365; i++) {
    x_data.emplace_back(i);
    y_data.emplace_back(0);
  }

  ImGuiWindowClass window_class;
  window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;

  FrameCounter frame_counter;

  float total_timer = 0.f;
  float hype_update_timer = 0.f;
  float simulation_timer = 0.f;

  struct Coin {
    std::string name;
    float stonks = 100.f;
    float stonks_delta = 0.f;
    float hype = 0.f;
    float volatility = 0.f;
  };

  Coin coin;
  coin.name = "Bubble$Coin";

  ImGuiIO& io = ImGui::GetIO();
  ImFontConfig font_cfg;
  font_cfg.SizePixels = 52.f;
  auto big_font = io.Fonts->AddFontDefault(&font_cfg);

  while (!quit) {
    frame_counter.Update();

    while (SDL_PollEvent(&e) != 0) {
      ImGui_ImplSDL3_ProcessEvent(&e);

      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      }
      if (e.type == SDL_EVENT_KEY_DOWN) {
        if (e.key.key == SDLK_ESCAPE) {
          quit = true;
        }
      }
    }

    total_timer += static_cast<float>(frame_counter.GetLastDeltaTime());

    hype_update_timer += static_cast<float>(frame_counter.GetLastDeltaTime());
    simulation_timer += static_cast<float>(frame_counter.GetLastDeltaTime());

    if (hype_update_timer > .5f) {
      hype_update_timer -= .5f;

      std::normal_distribution dist(0.f, 1.f);
      coin.hype = dist(mt);
    }

    if (simulation_timer > .01f) {
      simulation_timer -= .01;

      coin.volatility = std::abs(std::sin(total_timer * .5f)) * 10.f;

      {
        std::normal_distribution dist(coin.hype, coin.volatility);
        coin.stonks_delta = dist(mt);
        coin.stonks += coin.stonks_delta;
      }

      y_data.erase(y_data.begin());
      y_data.emplace_back(coin.stonks);
    }

    // Begin frame
    {
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplSDL3_NewFrame();
      ImGui::NewFrame();

      glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
      glClearColor(0.f, 0.f, 0.f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT);
    }

    // Main menu
    {
      ImGui::BeginMainMenuBar();
      ImGui::Text("FPS: %d, delta time: %.4f ms", frame_counter.GetFps(), frame_counter.GetLastDeltaTime() * 1000.0);
      ImGui::EndMainMenuBar();
    }

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
      ImGui::SliderFloat("##Hype", &coin.hype, -1.f, 1.f, "%.2f", ImGuiSliderFlags_ReadOnly);

      ImGui::SeparatorText("Volatility");
      ImGui::SliderFloat("##Volatility", &coin.volatility, 0.f, 10.f, "%.2f", ImGuiSliderFlags_ReadOnly);

      ImGui::PopItemWidth();
      ImGui::End();
    }

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
      ImGui::Button("Buy");
      ImGui::SameLine();
      ImGui::Button("Sell");
      ImGui::SameLine();
      ImGui::Button("Dump");
      ImGui::PopFont();
      ImGui::EndGroup();

      button_group_size = ImGui::GetItemRectSize();

      ImGui::End();
    }

    // End frame
    {
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      SDL_GL_SwapWindow(window);
    }

    std::this_thread::yield();
  }
}

} // namespace Scam
