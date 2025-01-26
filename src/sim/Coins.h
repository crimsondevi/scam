#pragma once
#include <memory>
#include <string>
#include <vector>

namespace Scam {

struct ScamCoin {
  std::string name;
  std::string code;
};

struct Coin_Bubble : ScamCoin {
  Coin_Bubble() {
    name = "Bubble$Coin";
    code = "BUBL";
  }
};

struct Coin_Lunar : ScamCoin {
  Coin_Lunar() {
    name = "Lunar";
    code = "LUNA";
  }
};

struct Coin_Bitcoinect : ScamCoin {
  Coin_Bitcoinect() {
    name = "Bitcoinect";
    code = "BITCON";
  }
};

struct Coin_Avocado : ScamCoin {
  Coin_Avocado() {
    name = "Avocado";
    code = "AVO";
  }
};

struct Coin_Cacticoin : ScamCoin {
  Coin_Cacticoin() {
    name = "Cacticoin";
    code = "CACTI";
  }
};

inline std::vector<std::unique_ptr<ScamCoin>> GetAvailableCoins() {
  std::vector<std::unique_ptr<ScamCoin>> coins;
  coins.emplace_back(std::make_unique<Coin_Bubble>());
  coins.emplace_back(std::make_unique<Coin_Lunar>());
  coins.emplace_back(std::make_unique<Coin_Bitcoinect>());
  coins.emplace_back(std::make_unique<Coin_Avocado>());
  coins.emplace_back(std::make_unique<Coin_Cacticoin>());
  return coins;
}

} // namespace Scam
