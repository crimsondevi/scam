#pragma once

#include <imgui.h>
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

} // namespace Scam
