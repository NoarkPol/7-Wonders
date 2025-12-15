#ifndef SEVEN_WONDERS_DUEL_ENGINE_H
#define SEVEN_WONDERS_DUEL_ENGINE_H

#include "Global.h"

/**
 * ======================================================================================
 * [EDUCATIONAL NOTE] - The Engine Layer (Strategy Pattern)
 * ======================================================================================
 * RESPONSIBILITY: Defines "WHAT cards do".
 * This layer is the heart of the game's extensibility.
 *
 * [DESIGN PATTERN] - Strategy Pattern (Polymorphism)
 * --------------------------------------------------
 * PROBLEM: Cards in 7 Wonders have vastly different effects.
 *          - Type A: Gives 2 Coins.
 *          - Type B: Gives 1 Shield.
 *          - Type C: Destroys an opponent's card.
 *          - Type D: Gives 1 point per yellow card.
 * WRONG WAY: `if (card.type == MILITARY) { ... } else if (card.type == COIN) {
 * ... }` This violates OCP (Open/Closed Principle). Adding a new effect
 * requires modifying core logic. RIGHT WAY: Define an interface `IEffect`
 * (Strategy). Each effect is a class. `Card` holds a list of `IEffect*`.
 *
 * [SOLID] - Open/Closed Principle (OCP)
 * You can add `class MeteorEffect : public IEffect` without changing a single
 * line of `GameController` or `Engine.h` mechanism.
 */

namespace SevenWondersDuel {

// Forward declarations to avoid circular includes
class Player;
class GameController;

/**
 * Interface for all game effects.
 * Functions:
 * 1. apply: Executed immediately when card is built (Command execution).
 * 2. calculateScore: Executed at end of game for scoring.
 */
class IEffect {
public:
  virtual ~IEffect() = default;
  virtual void apply(Player *self, Player *opponent, GameController *ctx) = 0;

  // Default impl: returns 0 points. Overridden by VictoryPointEffect,
  // GuildEffect, etc.
  virtual int calculateScore(const Player *self, const Player *opponent) const {
    return 0;
  }
};

// --------------------------------------------------
// Concrete Strategies
// --------------------------------------------------

// 1. Production Strategy (Simple Data Holder)
class ProductionEffect : public IEffect {
public:
  std::map<ResourceType, int> resources;
  ProductionEffect(std::map<ResourceType, int> r) : resources(r) {}

  // Production logic is usually passive (queried by Player::getResourceCount),
  // unlike active effects. So apply() is often empty unless it grants immediate
  // resources.
  void apply(Player *self, Player *opponent, GameController *ctx) override;
};

// 2. Military Strategy (Active Effect)
class MilitaryEffect : public IEffect {
public:
  int shieldCount;
  MilitaryEffect(int s) : shieldCount(s) {}
  void apply(Player *self, Player *opponent, GameController *ctx) override;
};

// 3. Science Strategy (Tagging Effect)
class ScienceEffect : public IEffect {
public:
  ScienceSymbol symbol;
  ScienceEffect(ScienceSymbol s) : symbol(s) {}
  void apply(Player *self, Player *opponent, GameController *ctx) override;
};

// 4. Coin Strategy (Immediate Gain)
class CoinEffect : public IEffect {
public:
  int amount;
  CoinEffect(int a) : amount(a) {}
  void apply(Player *self, Player *opponent, GameController *ctx) override;
};

// 5. Victory Point Strategy (Scoring Only)
class VictoryPointEffect : public IEffect {
public:
  int points;
  VictoryPointEffect(int p) : points(p) {}
  void apply(Player *self, Player *opponent, GameController *ctx) override {
  } // No immediate effect
  int calculateScore(const Player *self,
                     const Player *opponent) const override {
    return points;
  }
};

// 6. Guild Strategy (Complex Scoring)
// Calculates score based on dynamic game state (e.g., number of wonders built).
class GuildEffect : public IEffect {
public:
  bool countWonders;
  CardType targetColor;
  int pointsPerItem;
  int coinsPerItem; // Immediate coins usually

  // Constructor overloading for different Guild types
  GuildEffect(bool countW, int pts, int coins)
      : countWonders(countW), pointsPerItem(pts), coinsPerItem(coins),
        targetColor(CardType::GUILD) {} // guild unused
  GuildEffect(CardType color, int pts, int coins)
      : countWonders(false), targetColor(color), pointsPerItem(pts),
        coinsPerItem(coins) {}

  void apply(Player *self, Player *opponent, GameController *ctx) override;
  int calculateScore(const Player *self, const Player *opponent) const override;
};

// 7. State-Changing Strategies
// These effects trigger interruptions in the GameController state machine.

class DestructionEffect
    : public IEffect { // e.g. Constructs Garrison -> Destroy Opponent Grey Card
public:
  CardType targetColor; // Usually Grey or Brown
  DestructionEffect(CardType t = CardType::MANUFACTURED) : targetColor(t) {}
  void apply(Player *self, Player *opponent, GameController *ctx) override;
};

class LibraryEffect : public IEffect { // Progress Token selection
public:
  void apply(Player *self, Player *opponent, GameController *ctx) override;
};

class ResurrectEffect : public IEffect { // Mausoleum
public:
  void apply(Player *self, Player *opponent, GameController *ctx) override;
};

class PlayAgainEffect : public IEffect { // Wonder bonus
public:
  void apply(Player *self, Player *opponent, GameController *ctx) override;
};

} // namespace SevenWondersDuel

#endif // SEVEN_WONDERS_DUEL_ENGINE_H
