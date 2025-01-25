#pragma once

#include <random>
#include <string>

namespace Scam {

class Coin {
public:
  Coin();

public:
  void Simulate(float delta_time);

public:
  std::string name;
  std::string code;
  float stonks = 100.f;
  float stonks_delta = 0.f;
  float hype = 0.f;
  float volatility = 0.f;
  uint32_t days = 0;

private:
  float total_timer = 0.f;
  float hype_update_timer = 0.f;
  float simulation_timer = 0.f;

  std::mt19937 rng;
};

} // namespace Scam
