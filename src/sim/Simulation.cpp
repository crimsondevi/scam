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

  for (int i = 0; i < 5; i++) {
    auto items = std::vector<std::unique_ptr<Item>>();
    items.emplace_back(std::make_unique<Item_HypeCampaign>());
    items.emplace_back(std::make_unique<Item_CrazyCampaign>());
    items.emplace_back(std::make_unique<Item_HypeCampaign>());
    items.emplace_back(std::make_unique<Item_CrazyCampaign>());
    events.emplace_back(std::make_unique<Event>(Event{
        .day = 100 + 50 * i,
        .name = "Shop",
        .type = EventType::Shop,
        .items = std::move(items),
    }));
  }
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

  current_step++;
}

void ScamSim::ProcessTrade() {
  // TODO: trade
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
  coin_state->volatility = std::lerp(coin_state->volatility, 5.f, .01f);

  std::normal_distribution dist(coin_state->hype, coin_state->volatility);
  coin_state->value_delta = dist(rng);
  coin_state->value += coin_state->value_delta;
  coin_state->value = std::max(0.f, coin_state->value);
}

} // namespace Scam
