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
  Fail,
  GameOver,
};

struct Settings {
  float volume_music = .5f;
  float volume_sound = 1.f;
};

class SoundSystem {
public:
  SoundSystem();
  virtual ~SoundSystem();

public:
  void PlayMusic();
  void StopMusic();
  void PlaySound(const SoundCue& sound_cue);
  void SetMusicVolume(float volume);
  void SetSoundVolume(float volume);

public:
  void LoadSample(SoundCue sound_cue, const std::filesystem::path& path);

private:
  Mix_Music* music_sample;
  std::map<SoundCue, Mix_Chunk*> samples;
  int current_channel = 0;
};

} // namespace Scam
