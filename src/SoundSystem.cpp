#include "SoundSystem.h"

#include "Prelude.h"

#include <SDL3/SDL_audio.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <filesystem>

namespace Scam {

SoundSystem::SoundSystem() {
  Mix_OpenAudio(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

  // Load music
  {
    auto file = std::filesystem::current_path() / "data" / "A_Music.mp3";
    if (auto chunk = Mix_LoadMUS(file.string().c_str())) {
      LOG_INFO("Loaded audio sample: {}", file.string());
      music_sample = chunk;
    }
  }

  // Load samples
  {
    LoadSample(Click, std::filesystem::current_path() / "data" / "A_UI_Click_01.wav");
    LoadSample(Close, std::filesystem::current_path() / "data" / "A_UI_Close_01.wav");
    LoadSample(Hover, std::filesystem::current_path() / "data" / "A_UI_Hover_01.wav");
    LoadSample(Open, std::filesystem::current_path() / "data" / "A_UI_Open_01.wav");
    LoadSample(Purchase, std::filesystem::current_path() / "data" / "A_UI_Purchase_01.wav");
    LoadSample(Fail, std::filesystem::current_path() / "data" / "A_UI_Fail_01.wav");
    LoadSample(GameOver, std::filesystem::current_path() / "data" / "A_UI_GameOver_01.wav");
  }
}

SoundSystem::~SoundSystem() {
  SDL_CloseAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);

  if (music_sample) {
    Mix_FreeMusic(music_sample);
  }

  for (auto sample : samples) {
    if (sample.second) {
      Mix_FreeChunk(sample.second);
    }
  }
}

void SoundSystem::PlayMusic() {
  Mix_PlayMusic(music_sample, -1);
}

void SoundSystem::StopMusic() {
  Mix_PauseMusic();
}

void SoundSystem::PlaySound(const SoundCue& sound_cue) {
  if (const auto itr = samples.find(sound_cue); itr != samples.cend()) {
    current_channel = (current_channel + 1) % MIX_CHANNELS;
    Mix_PlayChannel(current_channel, itr->second, 0);
  }
}

void SoundSystem::LoadSample(SoundCue sound_cue, const std::filesystem::path& path) {
  if (auto chunk = Mix_LoadWAV(path.string().c_str())) {
    LOG_INFO("Loaded audio sample: {}", path.string());
    samples.insert({sound_cue, chunk});
  }
}

void SoundSystem::SetMusicVolume(float volume) {
  Mix_VolumeMusic(static_cast<int>(volume * MIX_MAX_VOLUME));
}

void SoundSystem::SetSoundVolume(float volume) {
  for (int i = 0; i < MIX_CHANNELS; i++) {
    Mix_Volume(i, static_cast<int>(volume * MIX_MAX_VOLUME));
  }
}

} // namespace Scam
