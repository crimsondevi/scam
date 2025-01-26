#include "Simulation.h"

#include <iostream>

namespace Scam {

Modifier_HypeMarket::Modifier_HypeMarket() {
  steps_left = 10; // 10 day hype
}

ScamInterfaceData Modifier_HypeMarket::GetInterfaceData() const {
  return {"Hype Market", "Increase hype over time..."};
}

void Modifier_HypeMarket::ApplyModifier(CoinState& state) {
  state.hype += 1;
}

Modifier_CrazyMarket::Modifier_CrazyMarket() {
  steps_left = 10;
}

ScamInterfaceData Modifier_CrazyMarket::GetInterfaceData() const {
  return {"Crazy Market", "Increases price volatility..."};
}

void Modifier_CrazyMarket::ApplyModifier(CoinState& state) {
  state.volatility += 1;
}

} // namespace Scam
