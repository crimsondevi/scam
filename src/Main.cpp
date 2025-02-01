#include "Prelude.h"

#include "Gameloop.h"

#include <IconsFontAwesome6.h>
#include <SDL3/SDL.h>
#include <filesystem>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <implot.h>

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_Window* window = SDL_CreateWindow("Scam", 1920, 1080, SDL_WINDOW_OPENGL | SDL_EVENT_WINDOW_RESIZED);

  if (!window) {
    LOG_ERROR("Could not create scamming window: {}", SDL_GetError());
    return -1;
  }

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);

  if (!gl_context) {
    LOG_ERROR("Error: SDL_GL_CreateContext(): {}", SDL_GetError());
    return -1;
  }

  SDL_GL_MakeCurrent(window, gl_context);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
    LOG_ERROR("Failed to initialize GLAD");
    return -1;
  }

  SDL_GL_SetSwapInterval(1); // Enable vsync
  SDL_ShowWindow(window);

  // Setup Dear ImGui context

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  // Default font
  {
    ImFontConfig font_cfg;
    font_cfg.SizePixels = 36.f;
    const auto font_path = std::filesystem::current_path() / "data" / "m6x11plus.ttf";
    io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), font_cfg.SizePixels, &font_cfg);

    // Font awesome icon font
    {
      const float base_font_size = font_cfg.SizePixels;
      const float icon_font_size = .75f * base_font_size; // FontAwesome fonts need to have their sizes reduced

      static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
      ImFontConfig icons_config;
      icons_config.MergeMode = true;
      icons_config.PixelSnapH = true;
      icons_config.GlyphMinAdvanceX = icon_font_size;
      const auto fa_path = std::filesystem::current_path() / "data" / FONT_ICON_FILE_NAME_FAS;
      io.Fonts->AddFontFromFileTTF(fa_path.string().c_str(), icon_font_size, &icons_config, icons_ranges);
    }
  }

  // Big font
  {
    ImFontConfig font_cfg;
    font_cfg.SizePixels = 54.f;
    const auto font_path = std::filesystem::current_path() / "data" / "m6x11plus.ttf";
    io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), font_cfg.SizePixels, &font_cfg);

    // Font awesome icon font
    {
      const float base_font_size = font_cfg.SizePixels;
      const float icon_font_size = .75f * base_font_size; // FontAwesome fonts need to have their sizes reduced

      static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
      ImFontConfig icons_config;
      icons_config.MergeMode = true;
      icons_config.PixelSnapH = true;
      icons_config.GlyphMinAdvanceX = icon_font_size;
      const auto fa_path = std::filesystem::current_path() / "data" / FONT_ICON_FILE_NAME_FAS;
      io.Fonts->AddFontFromFileTTF(fa_path.string().c_str(), icon_font_size, &icons_config, icons_ranges);
    }
  }

  ImPlot::CreateContext();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends

  ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init("#version 330");

  // MAIN LOOP

  Scam::main_loop(window);

  // Cleanup

  ImPlot::DestroyContext();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DestroyContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
