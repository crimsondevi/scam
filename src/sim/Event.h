#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Scam {

enum EventType {
  None,
  Shop,
  Mod,
  Audit,
};

class Event {
public:
  int day{};
  std::string name;
  EventType type{};
  std::vector<std::unique_ptr<class Item>> items;
};

} // namespace Scam
