#include "Simulation.h"
#include "Item.h"
#include "Modifier.h"

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_time.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <random>

namespace Scam {

ScamSim::ScamSim() {
  SDL_Time time;
  SDL_GetCurrentTime(&time);
  std::default_random_engine generator(time);
  rng = std::mt19937(generator());
}

void ScamSim::StartNewCoin(std::unique_ptr<ScamCoin> new_coin) {
  coin_state = std::make_unique<CoinState>();
  coin_state->coin = std::move(new_coin);

  // initial hype modifier
  AddModifier(std::make_unique<Modifier_SlowHype>());

  auto items = std::vector<std::unique_ptr<Item>>();
  items.emplace_back(std::make_unique<Item_HypeCampaign>());
  items.emplace_back(std::make_unique<Item_CrazyCampaign>());
  items.emplace_back(std::make_unique<Item_DoubleMoney>());
  items.emplace_back(std::make_unique<Item_HalfThreshold>());
  events.emplace_back(std::make_unique<Event>(Event{
      .day = 100,
      .name = "Shop",
      .type = EventType::Shop,
      .items = std::move(items),
  }));
}

bool ScamSim::AddTradeOrder(float order) {
  const int max_buy = std::floor(real_money / coin_state->value);
  const int max_sell = std::floor(fake_money);
  if (order > max_buy || order < -max_sell) {
    return false;
  }

  if (player_actions.trade_wish < 0.f && order > 0.f || player_actions.trade_wish > 0.f && order < 0.f) {
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

  bubble_threshold += .25f;

  current_step++;
}

float ScamSim::GetBubbleThreshold() const {
  return bubble_threshold;
}

bool ScamSim::HasBubbleBurst() const {
  return coin_state->value <= bubble_threshold;
}

void ScamSim::ProcessTrade() {
  const int orders = player_actions.trade_wish;

  const int max_buy = std::floor(real_money / coin_state->value);
  const int max_sell = std::floor(fake_money);

  const int max_buy_orders = std::min(4, max_buy);
  const int max_sell_orders = std::min(4, max_sell);

  const int clamped_orders = orders < 0 ? std::max(-max_sell_orders, orders) : std::min(max_buy_orders, orders);

  coin_state->hype += static_cast<float>(clamped_orders) * (clamped_orders > 0 ? .1f : .25f);

  fake_money += static_cast<float>(clamped_orders);
  real_money -= static_cast<float>(clamped_orders) * coin_state->value;

  if (std::abs(player_actions.trade_wish) > 0 && std::abs(clamped_orders) == 0) {
    player_actions.trade_wish /= 2;
  } else {
    player_actions.trade_wish -= clamped_orders;
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
    auto items = std::vector<std::unique_ptr<Item>>();
    items.emplace_back(std::make_unique<Item_HypeCampaign>());
    items.emplace_back(std::make_unique<Item_CrazyCampaign>());
    items.emplace_back(std::make_unique<Item_DoubleMoney>());
    items.emplace_back(std::make_unique<Item_HalfThreshold>());
    events.emplace_back(std::make_unique<Event>(Event{
        .day = current_step + 200,
        .name = "Shop",
        .type = EventType::Shop,
        .items = std::move(items),
    }));
  }
}

} // namespace Scam
