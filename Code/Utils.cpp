#include "Card.h"
#include "Engine.h"
#include "IDataLoader.h"
#include <iostream>

namespace SevenWondersDuel {

// --- Card Effect Implementations moved here or inline in Engine.cpp/Card.h ---
// Since we used inline or Engine.cpp, we just need Card::onBuild and
// Wonder::onBuild if they were not defined. Engine.cpp defines IEffect
// implementations. Card.h defines Card class. But Card::onBuild /
// Wonder::onBuild might be missing definitions if they were in Utils.cpp
// before. Step 236 showed them in Utils.cpp. So we keep them.

void Card::onBuild(Player *self, Player *opponent, GameController *ctx) {
  for (auto &eff : effects) {
    eff->apply(self, opponent, ctx);
  }
}

void Wonder::onBuild(Player *self, Player *opponent, GameController *ctx) {
  for (auto &eff : effects) {
    eff->apply(self, opponent, ctx);
  }
}

// --- MockDataLoader Helper ---
// Helper to create card on heap and return pointer (ownership transferred to
// caller via vector, managed eventually by Board/Player/unique_ptr if we
// upgraded, but here raw pointers for MVP). Actually, data loader usually
// transfers ownership. We will allocate with `new`.
Card *makeProdCard(std::string id, int age, ResourceType rt, int count,
                   int coinCost = 0) {
  Card *c = new Card(id, age,
                     (rt == ResourceType::PAPER || rt == ResourceType::GLASS)
                         ? CardType::MANUFACTURED
                         : CardType::RAW_MATERIAL);
  c->cost.coins = coinCost;
  std::map<ResourceType, int> prod;
  prod[rt] = count;
  c->effects.push_back(std::make_shared<ProductionEffect>(prod));
  return c;
}

Card *makeVictoryCard(std::string id, int age, int points, CardType type) {
  Card *c = new Card(id, age, type);
  c->effects.push_back(std::make_shared<VictoryPointEffect>(points));
  return c;
}

std::vector<Card *> MockDataLoader::loadCards(int age) {
  std::vector<Card *> deck;
  // Minimal Set for MVP Age 1 (20 cards needed for pyramid)
  // We create duplicates for demo
  for (int i = 0; i < 5; ++i) {
    deck.push_back(makeProdCard("LumberYard_" + std::to_string(i), age,
                                ResourceType::WOOD, 1));
    deck.push_back(makeProdCard("ClayPool_" + std::to_string(i), age,
                                ResourceType::CLAY, 1));
    deck.push_back(makeProdCard("Quarry_" + std::to_string(i), age,
                                ResourceType::STONE, 1));
    deck.push_back(makeVictoryCard("Baths_" + std::to_string(i), age, 3,
                                   CardType::CIVILIAN));
  }
  // Just ensure we have 20+
  return deck;
}

std::vector<Wonder *> MockDataLoader::loadWonders() {
  std::vector<Wonder *> wonders;

  Wonder *w1 = new Wonder("ThePyramids", "The Pyramids");
  w1->effects.push_back(std::make_shared<VictoryPointEffect>(9));
  w1->cost.resources[ResourceType::STONE] = 3;
  w1->cost.resources[ResourceType::PAPER] = 1;
  wonders.push_back(w1);

  Wonder *w2 = new Wonder("TheGreatLibrary", "The Great Library");
  w2->effects.push_back(std::make_shared<VictoryPointEffect>(4));
  w2->effects.push_back(
      std::make_shared<ScienceEffect>(ScienceSymbol::TABLET)); // Random science
  w2->cost.resources[ResourceType::WOOD] = 3;
  w2->cost.resources[ResourceType::GLASS] = 1;
  wonders.push_back(w2);

  Wonder *w3 = new Wonder("TheSphinx", "The Sphinx");
  w3->effects.push_back(std::make_shared<VictoryPointEffect>(6));
  w3->effects.push_back(std::make_shared<PlayAgainEffect>());
  w3->cost.resources[ResourceType::STONE] = 3;
  wonders.push_back(w3);

  Wonder *w4 = new Wonder("TheMausoleum", "The Mausoleum");
  w4->effects.push_back(std::make_shared<VictoryPointEffect>(2));
  w4->effects.push_back(std::make_shared<ResurrectEffect>());
  wonders.push_back(w4);

  // Add more if needed for draft (need 4 for demo phase)
  return wonders;
}

std::vector<ProgressToken> MockDataLoader::loadProgressTokens() {
  return {ProgressToken::AGRICULTURE, ProgressToken::URBANISM,
          ProgressToken::STRATEGY, ProgressToken::LAW,
          ProgressToken::PHILOSOPHY};
}

} // namespace SevenWondersDuel
