#ifndef SEVEN_WONDERS_DUEL_IDATALOADER_H
#define SEVEN_WONDERS_DUEL_IDATALOADER_H

#include "Card.h"
#include "Global.h"
#include <vector>

/**
 * ======================================================================================
 * [EDUCATIONAL NOTE] - The Factory / Builder Pattern Layer
 * ======================================================================================
 * PROBLEM:
 * We need to create hundreds of complex Card objects.
 * Hardcoding them in `GameController` or `main` is messy.
 * Reading them from JSON/XML is robust but requires an adapter.
 *
 * SOLUTION:
 * `IDataLoader` is an Abstract Factory.
 * It provides an interface to "Get the Deck".
 * We can implement `JsonDataLoader` or `MockDataLoader` (Generic code).
 */

namespace SevenWondersDuel {

// Abstract Factory interface
class IDataLoader {
public:
  virtual ~IDataLoader() = default;

  // Factory Method 1: Produce Cards
  virtual std::vector<Card *> loadCards(int age) = 0;

  // Factory Method 2: Produce Wonders
  virtual std::vector<Wonder *> loadWonders() = 0;

  // Factory Method 3: Produce Tokens
  virtual std::vector<ProgressToken> loadProgressTokens() = 0;
};

// Concrete Factory (Hardcoded Data for MVP/Testing)
class MockDataLoader : public IDataLoader {
public:
  std::vector<Card *> loadCards(int age) override;
  std::vector<Wonder *> loadWonders() override;
  std::vector<ProgressToken> loadProgressTokens() override;
};

} // namespace SevenWondersDuel

#endif // SEVEN_WONDERS_DUEL_IDATALOADER_H
