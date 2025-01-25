#include "Gameloop.h"
#include "Prelude.h"

#include <SDL3/SDL_opengl.h>
#include <deque>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
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

  uint32_t GetFps() const {
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
  std::default_random_engine generator;
  std::mt19937 mt(generator());
  std::normal_distribution dist(0.f, 1.f);

  for (int i = 0; i < 365; i++) {
    x_data.emplace_back(i);
    y_data.emplace_back(0);
  }

  FrameCounter frame_counter;

  float timer = 0.f;
  float stonks = 100.f;

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

    timer += frame_counter.GetLastDeltaTime();

    if (timer > .01f) {
      timer -= .01;

      stonks += dist(mt);

      y_data.erase(y_data.begin());
      y_data.emplace_back(stonks);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui::BeginMainMenuBar();
    ImGui::Text("FPS: %d, last delta time: %.4f ms", frame_counter.GetFps(), frame_counter.GetLastDeltaTime() * 1000.0);
    ImGui::EndMainMenuBar();

    ImGui::ShowDemoWindow();
    ImPlot::ShowDemoWindow();

    ImGui::Begin("Test");
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

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);

    std::this_thread::yield();
  }
}

} // namespace Scam
