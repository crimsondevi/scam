#pragma once

#include "ScamInterface.h"

namespace Scam {

struct CoinState;

class Modifier : public ScamInterfaceItem {
public:
  virtual void ApplyModifier(CoinState& state) = 0;
  
  int steps_left = 0;
};

class Modifier_HypeMarket : public Modifier {
public:
  Modifier_HypeMarket();
  ScamInterfaceData GetInterfaceData() const override;
  void ApplyModifier(CoinState& state) override;
};

class Modifier_CrazyMarket : public Modifier {
public:
  Modifier_CrazyMarket();
  ScamInterfaceData GetInterfaceData() const override;
  void ApplyModifier(CoinState& state) override;
};

} // namespace Scam
