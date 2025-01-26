#include "Simulation.h"
#include "../Prelude.h"
#include "Item.h"
#include "Modifier.h"

#include <algorithm>
#include <cstdlib>

namespace Scam {

void ScamSim::StartNewCoin(std::unique_ptr<ScamCoin> new_coin) {
  coin_state = std::make_unique<CoinState>();
  coin_state->coin = std::move(new_coin);
  events.emplace_back(std::make_unique<Event>(Event{
      .day = 100,
      .name = "Shop",
      .type = EventType::Shop,
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

  current_step++;
}

void ScamSim::ProcessTrade() {
  float order = player_actions.trade_wish;

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
  // TODO: implement the actual simulation
  float change = 10.f - static_cast<float>(rand() % 20);
  change += 10 * coin_state->hype;
  const float newValue = coin_state->value + change;
  coin_state->value = std::max(0.f, newValue);
}

} // namespace Scam
