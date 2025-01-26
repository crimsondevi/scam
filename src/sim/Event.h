#pragma once

#include <string>

namespace Scam {

enum EventType {
  None,
  Shop,
  Mod,
};

class Event {
public:
  int day{};
  std::string name;
  EventType type{};
};

} // namespace Scam
