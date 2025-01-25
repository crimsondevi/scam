#include "Gameloop.h"

#include "Coin.h"
#include "Dashboard.h"
#include "FrameCounter.h"
#include "Prelude.h"

#include <SDL3/SDL_opengl.h>
#include <deque>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <imgui_internal.h>
#include <random>
#include <thread>

namespace Scam {

void main_loop(SDL_Window* window) {
  bool quit = false;

  Coin coin;
  coin.name = "Bubble$Coin";

  FrameCounter frame_counter;
  Dashboard dashboard;

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

    coin.Simulate(frame_counter.GetLastDeltaTime());

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
      ImGui::Text("FPS: %d, delta time: %.4f ms", frame_counter.GetFps(), frame_counter.GetLastDeltaTime() * 1000.0);
      ImGui::EndMainMenuBar();
    }

    // Dashboard
    {
      dashboard.Update(coin);
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
