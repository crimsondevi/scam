#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include <SDL3/SDL_time.h>
#include <fstream>
#include <spdlog/spdlog.h>

namespace Scam {

#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)

inline ImVec2 ConstrainToAspectRatio(const ImVec2& size, float target_aspect_ratio) {
  float aspect_ratio = size.x / size.y;
  float x = aspect_ratio < target_aspect_ratio ? size.x : (size.y * target_aspect_ratio);
  float y = aspect_ratio > target_aspect_ratio ? size.y : (size.x / target_aspect_ratio);
  return {x, y};
}

inline bool LoadFileAsString(const std::string& filename, std::string& out_string) {
  std::ifstream stream(filename);
  std::stringstream buffer;
  buffer << stream.rdbuf();
  out_string = buffer.str();
  return true;
}

extern SDL_Time start_time;

inline double GetTimeSinceStart() {
  SDL_Time current_time;
  SDL_GetCurrentTime(&current_time);
  const SDL_Time elapsed_time = current_time - start_time;
  constexpr double ns = 1000.0 * 1000.0 * 1000.0;
  return static_cast<double>(elapsed_time) / ns;
}

} // namespace Scam
