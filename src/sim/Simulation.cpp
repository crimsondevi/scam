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
      .day = 50,
      .name = "Shop",
      .type = EventType::Shop,
      .items = std::move(items),
  }));

}
bool ScamSim::AddTradeOrder(float order) {
  const float new_order = player_actions.trade_wish + order;

  if (new_order > 0.f) {
    if (new_order * coin_state->value <= real_money) {
      player_actions.trade_wish = new_order;
      return true;
    }
  } else if (new_order < 0.f) {
    if (abs(new_order) <= fake_money) {
      player_actions.trade_wish = new_order;
      return true;
    }
  }

  return false;
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
  float order = player_actions.trade_wish;
  coin_state->hype += order;

  fake_money += order;
  real_money += order * coin_state->value * -1.f;

  player_actions.trade_wish = 0.f;
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
  coin_state->hype += -.1f;
  coin_state->volatility = std::lerp(coin_state->volatility, abs(coin_state->hype) + 10.f, .005f);

  std::normal_distribution dist(coin_state->hype, coin_state->volatility);
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
