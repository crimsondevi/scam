#include "Texture.h"

#include "Prelude.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Scam {

Texture::~Texture() {
  Discard();
}

void Texture::Discard() {
  if (loaded && !moving) {
    LOG_INFO("Discarding texture...");

    glDeleteTextures(1, &texture_data.tex);

    loaded = false;
  }
}

void Texture::LoadFromFile(const std::filesystem::path& in_path) {
  Discard();

  LOG_INFO("Loading texture from file: {}...", in_path.string());

  int w, h, comp;
  if (unsigned char* image_data = stbi_load(in_path.string().c_str(), &w, &h, &comp, STBI_rgb_alpha)) {
    texture_data.w = static_cast<float>(w);
    texture_data.h = static_cast<float>(h);

    glGenTextures(1, &texture_data.tex);
    glBindTexture(GL_TEXTURE_2D, texture_data.tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    loaded = true;
  }
}

bool Texture::GetTextureData(TextureData& out_texture_data) const {
  if (loaded) {
    out_texture_data.tex = texture_data.tex;
    out_texture_data.w = texture_data.w;
    out_texture_data.h = texture_data.h;
    return true;
  }
  return false;
}

} // namespace Scam
