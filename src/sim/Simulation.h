#pragma once

#include "Item.h"
#include "Modifier.h"

#include <memory>
#include <vector>

namespace Scam {

struct SimSettings {
  // float time_multiplier = 1.f;
};

struct CoinState {
  std::string name = "Bubble$Coin";
  float value = 1.f;
  float hype = 0.f;
  float volatility = 0.f;
  float total = 100000;
};

struct PlayerActions {
  float trade_wish = 0.f;
};

class ScamSim {
public:
  const SimSettings& GetSimSettings() const { return settings; }
  const CoinState& GetCoinState() const { return *coin_state; }
  const std::vector<std::unique_ptr<Modifier>>& GetModifiers() const { return modifiers; }
  const std::vector<std::unique_ptr<Item>>& GetItems() const { return items; }
  int GetMaxItems() const { return max_items; }
  
  void StartNewCoin(/*TODO: take in coin type*/);

  // TODO: add more player interaction

  template<class T>
  void AddModifier() {
    modifiers.push_back(std::make_unique<T>());
  }

  template<class T>
  void AddItem() {
    items.push_back(std::make_unique<T>());
  }

  void StepSimulation();

private:
  void ProcessTrade();
  void UseItems();
  void ApplyModifiers();
  void UpdateCoin();

  SimSettings settings;
  PlayerActions player_actions;
  std::unique_ptr<CoinState> coin_state;
  std::vector<std::unique_ptr<Modifier>> modifiers;
  std::vector<std::unique_ptr<Item>> items;
  int max_items = 5;

  // current step of the simulation, days by default but does not matter
  int current_step = 0;
};

} // namespace Scam
