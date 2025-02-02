#pragma once

#include "Coins.h"
#include "Event.h"
#include "Item.h"
#include "Modifier.h"

#include "SDL3/SDL_time.h"
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
  int trade_wish = 0;
};

class ScamSim {
public:
  explicit ScamSim(SDL_Time time);

public:
  [[nodiscard]] const CoinState& GetCoinState() const {
    return *coin_state;
  }

  [[nodiscard]] const std::vector<std::unique_ptr<Modifier>>& GetModifiers() const {
    return modifiers;
  }

  [[nodiscard]] const std::vector<std::unique_ptr<Item>>& GetItems() const {
    return items;
  }

  [[nodiscard]] const std::vector<std::unique_ptr<Event>>& GetEvents() const {
    return events;
  }

  [[nodiscard]] int GetMaxItems() const {
    return max_items;
  }

  [[nodiscard]] uint32_t GetCurrentStep() const {
    return current_step;
  }

  [[nodiscard]] double GetRealMoney() const {
    return real_money;
  }

  [[nodiscard]] double GetFakeMoney() const {
    return fake_money;
  }

  void StartNewCoin(std::unique_ptr<ScamCoin> new_coin);
  bool AddTradeOrder(int order);
  void AddModifier(std::unique_ptr<Modifier> modifier);
  void AddItem(std::unique_ptr<Item> item);
  void StepSimulation();

  [[nodiscard]] int GetTradeWish() const;
  [[nodiscard]] int GetProcessedTrades() const;
  [[nodiscard]] std::pair<double, double> GetMaxBuySell() const;
  [[nodiscard]] std::pair<double, double> GetMaxBuySellOrders() const;
  [[nodiscard]] double GetBubbleThreshold() const;
  [[nodiscard]] bool HasBubbleBurst() const;

private:
  void ProcessTrade();
  void UseItems();
  void ApplyModifiers();
  void UpdateCoin();

private:
  PlayerActions player_actions;
  std::unique_ptr<CoinState> coin_state;
  std::vector<std::unique_ptr<Modifier>> modifiers;
  std::vector<std::unique_ptr<Item>> items;
  std::vector<std::unique_ptr<Event>> events;

  int max_items = 5;
  double fake_money = 0.0;
  double real_money = 100.0;
  double bubble_threshold = .1f;
  double bubble_threshold_target = .1f;
  int processed_trades = 0;

  // current step of the simulation, days by default but does not matter
  int current_step = 0;

  std::default_random_engine rng;

private:
  friend class Item;
  friend class Item_DoubleMoney;
  friend class Item_HalfThreshold;
};

} // namespace Scam
