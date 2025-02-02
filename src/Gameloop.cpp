#include "Gameloop.h"

#include "Dashboard.h"
#include "FrameCounter.h"
#include "Prelude.h"
#include "sim/Simulation.h"

#include <glad/glad.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <imgui_internal.h>
#include <thread>

namespace Scam {

void main_loop(SDL_Window* window) {
  FrameCounter frame_counter;
  bool show_settings_panel = false;
  Settings settings;
  bool quit = false;

  std::unique_ptr<Dashboard> dashboard = std::make_unique<Dashboard>(settings);
  std::unique_ptr<ScamSim> scam_sim = std::make_unique<ScamSim>();

  {
    auto coins = GetAvailableCoins();
    scam_sim->StartNewCoin(std::move(coins[0]));
    scam_sim->StepSimulation();
  }

  float simulation_timer = 0.f;
  bool game_over = false;
  bool try_again = false;

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

        static int last_speed_multiplier = 1;

        switch (e.key.key) {
        case SDLK_1:
          dashboard->SetSpeedMultiplier(1);
          break;
        case SDLK_2:
          dashboard->SetSpeedMultiplier(4);
          break;
        case SDLK_3:
          dashboard->SetSpeedMultiplier(16);
          break;
        case SDLK_SPACE:
          if (dashboard->GetSpeedMultiplier() > 0) {
            last_speed_multiplier = dashboard->GetSpeedMultiplier();
            dashboard->SetSpeedMultiplier(0);
          } else {
            dashboard->SetSpeedMultiplier(last_speed_multiplier);
          }
          break;
        case SDLK_R:
          try_again = true;
          break;
        }
      }
    }

    if (try_again) {
      try_again = false;

      game_over = false;
      simulation_timer = 0.f;

      dashboard = std::make_unique<Dashboard>(settings);
      scam_sim = std::make_unique<ScamSim>();

      {
        auto coins = GetAvailableCoins();
        scam_sim->StartNewCoin(std::move(coins[0]));
        scam_sim->StepSimulation();
      }
    }

    if (scam_sim && !game_over) {
      const auto update_delta_time =
          static_cast<float>(frame_counter.GetLastDeltaTime() * dashboard->GetSpeedMultiplier());
      simulation_timer += update_delta_time;

      while (simulation_timer > .4f) {
        simulation_timer -= .4f;
        scam_sim->StepSimulation();

        for (const auto& event : scam_sim->GetEvents()) {
          if (event->day == scam_sim->GetCurrentStep() && event->type == EventType::Audit &&
              scam_sim->HasBubbleBurst()) {
            game_over = true;
          }
        }
      }
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

    if (game_over) {
      ImGui::OpenPopup("Game Over");

      if (ImGui::BeginPopupModal("Game Over", nullptr, ImGuiWindowFlags_Modal | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Game Over");
        if (ImGui::Button("Try Again")) {
          try_again = true;
        }
        ImGui::EndPopup();
      }
    }

    // Dashboard
    {
      dashboard->ApplySettings(settings);
      dashboard->Update(static_cast<float>(frame_counter.GetLastDeltaTime()), *scam_sim);
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
