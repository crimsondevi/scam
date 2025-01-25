#pragma once

#include <SDL3_mixer/SDL_mixer.h>
#include <filesystem>
#include <map>

namespace Scam {

enum SoundCue {
  Click,
  Close,
  Hover,
  Open,
  Purchase,
};

class SoundSystem {
public:
  SoundSystem();
  virtual ~SoundSystem();

public:
  void PlayMusic();
  void PlaySound(const SoundCue& sound_cue);

public:
  void LoadSample(SoundCue sound_cue, const std::filesystem::path& path);

private:
  Mix_Music* music_sample;
  std::map<SoundCue, Mix_Chunk*> samples;
};

} // namespace Scam
