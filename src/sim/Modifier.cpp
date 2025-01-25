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

} // namespace Scam
