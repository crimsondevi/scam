#include "Simulation.h"
#include "Item.h"
#include "Modifier.h"

#include <SDL3/SDL_stdinc.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <random>

namespace Scam {

ScamSim::ScamSim(SDL_Time time) : rng(std::default_random_engine(static_cast<uint32_t>(time))) {}

void ScamSim::StartNewCoin(std::unique_ptr<ScamCoin> new_coin) {
  coin_state = std::make_unique<CoinState>();
  coin_state->coin = std::move(new_coin);

  // initial hype modifier
  AddModifier(std::make_unique<Modifier_SlowHype>());

  auto shop_items = std::vector<std::unique_ptr<Item>>();
  shop_items.emplace_back(std::make_unique<Item_HypeCampaign>());
  shop_items.emplace_back(std::make_unique<Item_CrazyCampaign>());
  shop_items.emplace_back(std::make_unique<Item_DoubleMoney>());
  shop_items.emplace_back(std::make_unique<Item_HalfThreshold>());
  events.emplace_back(std::make_unique<Event>(Event{
      .day = 100,
      .name = "Shop",
      .type = EventType::Shop,
      .items = std::move(shop_items),
  }));
}

bool ScamSim::AddTradeOrder(int order) {
  const auto [max_buys, max_sells] = GetMaxBuySell();

  if (order > max_buys || order < -max_sells) {
    return false;
  }

  if (player_actions.trade_wish < 0 && order > 0 || player_actions.trade_wish > 0 && order < 0) {
    player_actions.trade_wish = order;
  } else {
    player_actions.trade_wish += order;
  }
  return true;
}

void ScamSim::AddModifier(std::unique_ptr<Modifier> modifier) {
  modifiers.emplace_back(std::move(modifier));
}

void ScamSim::AddItem(std::unique_ptr<Item> item) {
  items.emplace_back(std::move(item));
}

void ScamSim::StepSimulation() {
  ProcessTrade();
  UseItems();
  ApplyModifiers();
  UpdateCoin();

  for (const auto& event : events) {
    if (event->type == EventType::Audit && event->day == current_step) {
      bubble_threshold_target = std::lerp(bubble_threshold_target, coin_state->value, .5f);
    }
  }

  bubble_threshold = std::lerp(bubble_threshold, bubble_threshold_target, .1f);

  current_step++;
}

int ScamSim::GetTradeWish() const {
  return player_actions.trade_wish;
}

int ScamSim::GetProcessedTrades() const {
  return processed_trades;
}

std::pair<double, double> ScamSim::GetMaxBuySell() const {
  return std::make_pair(std::floor(real_money / coin_state->value), std::floor(fake_money));
}

std::pair<double, double> ScamSim::GetMaxBuySellOrders() const {
  return std::make_pair(std::max(1.0, static_cast<double>(std::floor(std::log(current_step)))),
                        std::max(1.0, static_cast<double>(std::floor(std::log(current_step)))));
}

double ScamSim::GetBubbleThreshold() const {
  return bubble_threshold;
}

bool ScamSim::HasBubbleBurst() const {
  return coin_state->value <= bubble_threshold;
}

void ScamSim::ProcessTrade() {
  const double orders = player_actions.trade_wish;

  const auto [max_buys, max_sells] = GetMaxBuySell();
  const auto [max_buy_orders, max_sell_orders] = GetMaxBuySellOrders();

  const double min_orders = std::max(-max_sell_orders, -max_sells);
  const double max_orders = std::min(max_buy_orders, max_buys);
  const double clamped_orders = std::clamp(orders, min_orders, max_orders);

  coin_state->hype += static_cast<float>(clamped_orders) * (clamped_orders > 0 ? .15f : .25f);

  fake_money += static_cast<float>(clamped_orders);
  real_money -= static_cast<float>(clamped_orders) * coin_state->value;

  if (std::abs(player_actions.trade_wish) > 0 && std::abs(clamped_orders) == 0) {
    processed_trades = 0;
    player_actions.trade_wish = 0;
  } else {
    processed_trades = static_cast<int>(clamped_orders);
    player_actions.trade_wish -= processed_trades;
  }
}

void ScamSim::UseItems() {
  for (auto it = items.begin(); it != items.end();) {
    Item* item = it->get();
    if (item->TryUseItem(*this)) {
      it = items.erase(it);
    } else {
      ++it;
    }
  }
}

void ScamSim::ApplyModifiers() {
  for (auto it = modifiers.begin(); it != modifiers.end();) {
    Modifier* modifier = it->get();
    modifier->ApplyModifier(*coin_state);

    if (modifier->steps_left-- == 0) {
      it = modifiers.erase(it);
    } else {
      ++it;
    }
  }
}

void ScamSim::UpdateCoin() {
  // coin_state->hype += -.1f;

  if (coin_state->hype > 0.f) {
    coin_state->hype -= .1f + coin_state->hype / 200.f;
  } else {
    // slower decay when hype is negative
    coin_state->hype -= .1f * (1.f / std::max(1.f, abs(coin_state->hype)));
  }

  const float volatility_target = coin_state->hype < 0.f ? 20.f : 10.f;
  coin_state->volatility = std::lerp(coin_state->volatility, volatility_target, .025f);
  // coin_state->volatility = std::lerp(coin_state->volatility, abs(coin_state->hype) + 10.f, .005f);

  std::normal_distribution dist(coin_state->hype, coin_state->volatility * 2.f);
  coin_state->value_delta = dist(rng);
  coin_state->value += coin_state->value_delta;
  coin_state->value = std::max(0.01f, coin_state->value);

  if ((current_step + 365) % 365 == 0) {
    events.emplace_back(std::make_unique<Event>(Event{
        .day = current_step + 365,
        .name = "Audit",
        .type = EventType::Audit,
        .items = {},
    }));
  }

  if ((current_step + 200) % 400 == 0) {
    auto shop_items = std::vector<std::unique_ptr<Item>>();
    shop_items.emplace_back(std::make_unique<Item_HypeCampaign>());
    shop_items.emplace_back(std::make_unique<Item_CrazyCampaign>());
    shop_items.emplace_back(std::make_unique<Item_DoubleMoney>());
    shop_items.emplace_back(std::make_unique<Item_HalfThreshold>());
    events.emplace_back(std::make_unique<Event>(Event{
        .day = current_step + 200,
        .name = "Shop",
        .type = EventType::Shop,
        .items = std::move(shop_items),
    }));
  }
}

} // namespace Scam
