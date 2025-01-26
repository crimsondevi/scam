#include "Simulation.h"
#include "Item.h"
#include "Modifier.h"

#include <algorithm>
#include <cstdlib>

namespace Scam {

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
    }
    else {
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
    }
    else {
      ++it;
    }
  }
}

void ScamSim::UpdateCoin() {
  // TODO: implement the actual simulation
  float change = 120.f - static_cast<float>(rand() % 200);
  change += 100 * coin_state->hype;
  const float newValue = coin_state->value + change;
  coin_state->value = std::max(0.f, newValue);
}

} // namespace Scam
