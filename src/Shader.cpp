#include "Shader.h"

#include <filesystem>
#include <glad/glad.h>

namespace Scam {

Shader::~Shader() {
  Discard();
}

void Shader::Discard() {
  if (loaded && !moving) {
    LOG_INFO("Discarding shader: {} / {}", vert_source_file, frag_source_file);

    glDeleteProgram(program);

    loaded = false;
  }
}

void Shader::Load(const std::string& in_vert_source_file, const std::string& in_frag_source_file) {
  vert_source_file = in_vert_source_file;
  frag_source_file = in_frag_source_file;

  vert_last_file_time = std::filesystem::last_write_time(vert_source_file);
  frag_last_file_time = std::filesystem::last_write_time(frag_source_file);

  GLuint vert_shader{};
  GLuint frag_shader{};

  // Compile vertex shader

  LOG_INFO("Loading vert shader: {}", vert_source_file);

  if (std::string source; LoadFileAsString(vert_source_file, source)) {
    const char* shader_code = source.c_str();

    int success{};
    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &shader_code, nullptr);
    glCompileShader(vert_shader);
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetShaderInfoLog(vert_shader, sizeof(infoLog), nullptr, infoLog);
      LOG_ERROR("Failed to compile vertex shader: {}", infoLog);
      return;
    }
  }

  // Compile fragment shader

  LOG_INFO("Loading frag shader: {}", frag_source_file);

  if (std::string source; LoadFileAsString(frag_source_file, source)) {
    const char* shader_code = source.c_str();

    int success{};
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &shader_code, nullptr);
    glCompileShader(frag_shader);
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetShaderInfoLog(frag_shader, sizeof(infoLog), nullptr, infoLog);
      LOG_ERROR("Failed to compile fragment shader: {}", infoLog);
      return;
    }
  }

  // Link shaders
  {
    int success{};
    GLuint new_program = glCreateProgram();
    glAttachShader(new_program, vert_shader);
    glAttachShader(new_program, frag_shader);
    glLinkProgram(new_program);
    glGetProgramiv(new_program, GL_LINK_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetProgramInfoLog(new_program, sizeof(infoLog), nullptr, infoLog);
      LOG_ERROR("Failed to link shaders: {}", infoLog);
      return;
    }

    Discard();

    program = new_program;
  }

  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  loaded = true;
}

bool Shader::GetShaderData(ShaderData& out_shader_data) const {
  if (loaded) {
    out_shader_data.program = program;
    return true;
  }
  return false;
}

void Shader::Reload() {
  auto vert_file_time = std::filesystem::last_write_time(vert_source_file);
  auto frag_file_time = std::filesystem::last_write_time(frag_source_file);

  if (vert_file_time > vert_last_file_time || frag_file_time > frag_last_file_time) {
    Load(vert_source_file, frag_source_file);
  }
}

} // namespace clover
