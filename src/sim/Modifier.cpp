#include "Simulation.h"

#include <iostream>

namespace Scam {

// SLOW HYPE
Modifier_SlowHype::Modifier_SlowHype() {
  steps_left += 90;
}
ScamInterfaceData Modifier_SlowHype::GetInterfaceData() const {
  return {"Slow Hype", "Slow increase of hype over 90 days."};
}
void Modifier_SlowHype::ApplyModifier(CoinState& state) {
  state.hype += .1f;
}

// HYPE MARKET
Modifier_HypeMarket::Modifier_HypeMarket() {
  steps_left = 10; // 10 day hype
}
ScamInterfaceData Modifier_HypeMarket::GetInterfaceData() const {
  return {"Hype Market", "Increase hype over time..."};
}
void Modifier_HypeMarket::ApplyModifier(CoinState& state) {
  state.hype += .5;
}

// CRAZY MARKET
Modifier_CrazyMarket::Modifier_CrazyMarket() {
  steps_left = 10;
}
ScamInterfaceData Modifier_CrazyMarket::GetInterfaceData() const {
  return {"Crazy Market", "Increases price volatility..."};
}
void Modifier_CrazyMarket::ApplyModifier(CoinState& state) {
  state.volatility += 5.f;
}

} // namespace Scam
