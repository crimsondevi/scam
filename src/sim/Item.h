#pragma once

#include "ScamInterface.h"

namespace Scam {

class ScamSim;

enum ItemType {
  UNDEFINED,
  INSTANT,
  DURATION,
};

class Item : public ScamInterfaceItem {
public:
  void ActivateItem();
  void DeactivateItem();
  void ToggleItem();
  bool TryUseItem(ScamSim& sim); // return true if item should be consumed (removed)

protected:
  virtual void ApplyItem(ScamSim& sim) = 0; // Actual item logic
  
  ItemType type = INSTANT;

private:
  bool is_active = false;
};

class Item_HypeCampaign : public Item {
public:
  ScamInterfaceData GetInterfaceData() const override;
  void ApplyItem(ScamSim& sim) override;
};

} // namespace Scam
