#include "Item.h"
#include "Simulation.h"

namespace Scam {

void Item::ActivateItem() {
  is_active = true;
}

void Item::DeactivateItem() {
  is_active = false;
}

void Item::ToggleItem() {
  if (is_active)
    DeactivateItem();
  else
    ActivateItem();
}

bool Item::TryUseItem(ScamSim& sim) {
  if (!is_active)
    return false;
  ApplyItem(sim);
  return true;
}

ScamInterfaceData Item_HypeCampaign::GetInterfaceData() const {
  return {"Hype Campaign", "Start a hype campaign that will take us to the moon!"};
}

void Item_HypeCampaign::ApplyItem(ScamSim& sim) {
  sim.AddModifier(std::make_unique<Modifier_HypeMarket>());
}

ScamInterfaceData Item_CrazyCampaign::GetInterfaceData() const {
  return {"Crazy Campaign", "Start a crazy campaign that will destabilize the markets!"};
}

void Item_CrazyCampaign::ApplyItem(ScamSim& sim) {
  sim.AddModifier(std::make_unique<Modifier_CrazyMarket>());
}

} // namespace Scam
