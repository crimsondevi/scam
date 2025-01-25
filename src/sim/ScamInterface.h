#pragma once

#include <string>

struct ScamInterfaceData {
  std::string name;
  std::string description;
};

class ScamInterfaceItem {
public:
  virtual ~ScamInterfaceItem() = default;
  virtual ScamInterfaceData GetInterfaceData() const = 0;
};
