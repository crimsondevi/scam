#pragma once

#include "Prelude.h"

#include <glad/glad.h>

namespace Scam {

struct ShaderData {
  GLuint program{};
};

class Shader {
public:
  Shader() = default;
  Shader(const Shader&) = delete;
  Shader& operator=(Shader&) = delete;
  Shader(const Shader&&) noexcept = delete;
  Shader& operator=(const Shader&&) noexcept = delete;

  Shader(Shader&& shader) noexcept
      : loaded(shader.loaded), program(shader.program), vert_source_file(std::move(shader.vert_source_file)),
        frag_source_file(std::move(shader.frag_source_file)), vert_last_file_time(shader.vert_last_file_time),
        frag_last_file_time(shader.frag_last_file_time) {
    shader.moving = true;
  }

  Shader& operator=(Shader&& shader) noexcept {
    loaded = shader.loaded;
    program = shader.program;
    vert_source_file = shader.vert_source_file;
    frag_source_file = shader.frag_source_file;
    vert_last_file_time = shader.vert_last_file_time;
    frag_last_file_time = shader.frag_last_file_time;
    shader.moving = true;
    return *this;
  }

  virtual ~Shader();

private:
  void Discard();

public:
  void Load(const std::string& in_vert_source_file, const std::string& in_frag_source_file);
  void Reload();

public:
  bool GetShaderData(ShaderData& out_shader_data) const;

private:
  bool loaded = false;
  bool moving = false;
  GLuint program{};

private:
  std::string vert_source_file;
  std::string frag_source_file;
  std::chrono::time_point<std::chrono::file_clock> vert_last_file_time;
  std::chrono::time_point<std::chrono::file_clock> frag_last_file_time;
};

class ShaderHandle {
private:
  bool valid = false;
  ShaderData shader_data;

public:
  ShaderHandle() = default;
  ShaderHandle(const Shader& shader) {
    valid = shader.GetShaderData(shader_data);
  }

public:
  bool GetShaderData(ShaderData& out_shader_data) const {
    if (valid) {
      out_shader_data = shader_data;
      return true;
    }
    return false;
  }
};

} // namespace Scam
