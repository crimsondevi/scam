#include "Coin.h"

#include <SDL3/SDL.h>
#include <random>

namespace Scam {

Coin::Coin() {
  SDL_Time time;
  SDL_GetCurrentTime(&time);
  std::default_random_engine generator(time);
  rng = std::mt19937(generator());
}

void Coin::Simulate(float delta_time) {
  total_timer += delta_time;
  hype_update_timer += delta_time;
  simulation_timer += delta_time;

  if (hype_update_timer > .5f) {
    hype_update_timer -= .5f;

    std::normal_distribution dist(0.f, 1.f);
    hype = dist(rng);
  }

  if (simulation_timer > .1f) {
    simulation_timer -= .1f;

    volatility = std::abs(std::sin(total_timer * .5f)) * 10.f;

    {
      std::normal_distribution dist(hype, volatility);
      stonks_delta = dist(rng);
      stonks += stonks_delta;
    }

    days++;
  }
}

} // namespace Scam
