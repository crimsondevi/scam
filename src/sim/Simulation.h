#pragma once

#include "Coins.h"
#include "Event.h"
#include "Item.h"
#include "Modifier.h"

#include <memory>
#include <random>
#include <vector>

namespace Scam {

struct CoinState {
  std::shared_ptr<ScamCoin> coin;
  float value = 1.f;
  float value_delta = 0.f;
  float hype = 0.f;
  float volatility = .1f;
  float total = 100000;
};

struct PlayerActions {
  float trade_wish = 0.f;
};

class ScamSim {
public:
  ScamSim();

public:
  const CoinState& GetCoinState() const {
    return *coin_state;
  }
  const std::vector<std::unique_ptr<Modifier>>& GetModifiers() const {
    return modifiers;
  }
  const std::vector<std::unique_ptr<Item>>& GetItems() const {
    return items;
  }
  const std::vector<std::unique_ptr<Event>>& GetEvents() const {
    return events;
  }
  int GetMaxItems() const {
    return max_items;
  }
  [[nodiscard]] int GetCurrentStep() const {
    return current_step;
  }
  float GetRealMoney() const {
    return real_money;
  }
  float GetFakeMoney() const {
    return fake_money;
  }

  void StartNewCoin(std::unique_ptr<ScamCoin> new_coin);
  bool AddTradeOrder(float order);
  void AddModifier(std::unique_ptr<Modifier> modifier);
  void AddItem(std::unique_ptr<Item> item);
  void StepSimulation();

  [[nodiscard]] float GetBubbleThreshold() const;
  [[nodiscard]] bool HasBubbleBurst() const;

private:
  void ProcessTrade();
  void UseItems();
  void ApplyModifiers();
  void UpdateCoin();

  PlayerActions player_actions;
  std::unique_ptr<CoinState> coin_state;
  std::vector<std::unique_ptr<Modifier>> modifiers;
  std::vector<std::unique_ptr<Item>> items;
  std::vector<std::unique_ptr<Event>> events;

public:
  int max_items = 5;
  float fake_money = 0.f;
  float real_money = 100.f;
  float bubble_threshold = .1f;

  // current step of the simulation, days by default but does not matter
  int current_step = 0;

  std::mt19937 rng;
};

} // namespace Scam
