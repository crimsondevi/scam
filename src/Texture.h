#pragma once

#include <filesystem>
#include <glad/glad.h>

namespace Scam {

struct TextureData {
  GLuint tex{};
  float w{};
  float h{};
};

class Texture {
public:
  Texture() = default;
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;
  Texture(const Texture&&) noexcept = delete;
  Texture& operator=(const Texture&&) noexcept = delete;

  Texture(Texture&& texture) noexcept : loaded(texture.loaded), texture_data(texture.texture_data) {
    texture.moving = true;
  }

  Texture& operator=(Texture&& texture) noexcept {
    loaded = texture.loaded;
    texture_data = texture.texture_data;
    texture.moving = false;
    return *this;
  }

  virtual ~Texture();

private:
  void Discard();

public:
  void LoadFromFile(const std::filesystem::path& path);

public:
  bool GetTextureData(TextureData& out_texture_data) const;

private:
  bool loaded = false;
  bool moving = false;
  TextureData texture_data;
};

} // namespace Scam
