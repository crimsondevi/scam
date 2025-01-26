#include "Gameloop.h"

#include "Coin.h"
#include "Dashboard.h"
#include "FrameCounter.h"
#include "sim/Simulation.h"

#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <imgui_internal.h>
#include <thread>

namespace Scam {

void main_loop(SDL_Window* window) {
  bool quit = false;

  float simulation_timer = 0.f;

  ScamSim scam_sim;
  auto coins = GetAvailableCoins();
  scam_sim.StartNewCoin(std::move(coins[0]));
  // scam_sim.StartNewCoin(std::make_unique<Coin_Bubble>());
  // scam_sim.AddModifier<Modifier_HypeMarket>();
  scam_sim.StepSimulation();

  FrameCounter frame_counter;
  Dashboard dashboard;
  Settings settings;

  bool show_settings_panel = false;

  while (!quit) {
    frame_counter.Update();

    SDL_Event e;
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

    const float update_delta_time =
        static_cast<float>(frame_counter.GetLastDeltaTime() * dashboard.GetSpeedMultiplier());
    simulation_timer += update_delta_time;

    if (simulation_timer > .1f) {
      simulation_timer -= .1f;
      scam_sim.StepSimulation();
    }

    // Begin frame
    {
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplSDL3_NewFrame();
      ImGui::NewFrame();

      const ImGuiIO& io = ImGui::GetIO();
      glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
      glClearColor(0.f, 0.f, 0.f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT);
    }

    // Main menu
    {
      ImGui::BeginMainMenuBar();
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Settings")) {
          show_settings_panel = true;
        }
        ImGui::EndMenu();
      }
      ImGui::Separator();
      ImGui::Text("FPS: %d", frame_counter.GetFps());
      ImGui::Separator();
      ImGui::Text("Delta Time: %.4f ms", frame_counter.GetLastDeltaTime() * 1000.0);
      ImGui::EndMainMenuBar();
    }

    if (show_settings_panel) {
      ImGui::Begin("Settings", &show_settings_panel, ImGuiWindowFlags_AlwaysAutoResize);
      ImGui::SeparatorText("Audio");
      ImGui::SliderFloat("Music", &settings.volume_music, 0.f, 1.f);
      ImGui::SliderFloat("Sound", &settings.volume_sound, 0.f, 1.f);
      ImGui::End();
    }

    // Dashboard
    {
      dashboard.ApplySettings(settings);
      dashboard.Update(scam_sim);
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
