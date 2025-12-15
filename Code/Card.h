#ifndef SEVEN_WONDERS_DUEL_CARD_H
#define SEVEN_WONDERS_DUEL_CARD_H

#include "Engine.h"
#include "Global.h"

/**
 * ======================================================================================
 * [EDUCATIONAL NOTE] - Card Definition (Composite Pattern Idea)
 * ======================================================================================
 * [DESIGN PATTERN] - Composition over Inheritance
 * -----------------------------------------------
 * PROBLEM:
 * If we used inheritance: `class MilitaryCard : public Card`, `class
 * ScienceCard : public Card`. What if a card gives both Military AND Science?
 * `class MilitaryScienceCard`? This leads to "Class Explosion".
 *
 * SOLUTION:
 * Use Composition. A Card "HAS A" list of Effects.
 * It does not matter what the effects are. The Card is just a container.
 *
 * [DESIGN PATTERN] - Composite idea
 * ---------------------------------
 * When we ask `card.getPoints()`, the Card asks all its children (Effects)
 * "What are your points?" and sums them up.
 * The client treats the Card and its Effects uniformly.
 */

namespace SevenWondersDuel {

// Simple Data Struct for Cost
struct ResourceCost {
  int coins = 0;
  std::map<ResourceType, int> resources;
};

class Card {
public:
  std::string id;
  std::string name;
  int age;
  CardType type;
  ResourceCost cost;

  std::string chainCost;   // Free if player has this symbol (e.g. "Lamp")
  std::string chainSymbol; // This card provides this symbol (e.g. "Mask")

  // [SOLID] - Open/Closed Principle
  // We can add any number of effects without changing Card class structure.
  std::vector<std::shared_ptr<IEffect>> effects;

  Card(const std::string &id, int age, CardType type)
      : id(id), name(id), age(age), type(type) {}

  // [COMPOSITE PATTERN]
  // The Leaf nodes are IEffects. The Composite node is Card.
  // The operation `getPoints` is delegated to all children.
  int getPoints(const Player *self, const Player *opponent) const {
    int total = 0;
    for (const auto &eff : effects)
      total += eff->calculateScore(self, opponent);
    return total;
  }

  // Delegate "build" trigger to strategies
  void onBuild(Player *self, Player *opponent, GameController *ctx);
};

class Wonder {
public:
  std::string id;
  std::string name;
  ResourceCost cost;
  std::vector<std::shared_ptr<IEffect>> effects;

  // State of the Wonder
  bool isBuilt = false;
  Card *tuckedCard = nullptr;

  Wonder(const std::string &id, const std::string &name) : id(id), name(name) {}

  int getPoints(const Player *self, const Player *opponent) const {
    int total = 0;
    for (const auto &eff : effects)
      total += eff->calculateScore(self, opponent);
    return total;
  }

  void onBuild(Player *self, Player *opponent, GameController *ctx);
};

} // namespace SevenWondersDuel

#endif // SEVEN_WONDERS_DUEL_CARD_H
