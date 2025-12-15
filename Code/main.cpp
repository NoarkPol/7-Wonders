#include "GameController.h"
#include <iostream>
#include <memory>

/**
 * ======================================================================================
 * [EDUCATIONAL NOTE] - The Entry Point (Dependency Injection)
 * ======================================================================================
 * [Design Pattern] - Dependency Injection (DI)
 * --------------------------------------------
 * `main` acts as the "Composition Root".
 * It creates the dependencies (`GameView`, `MockDataLoader`) and injects them
 * into the `GameController`.
 */

using namespace SevenWondersDuel;

int main() {
  std::cout << "Starting 7 Wonders Duel (Educational Version)...\n";

  // 1. Create Dependencies
  auto view = std::make_unique<GameView>();
  auto loader = std::make_unique<MockDataLoader>();

  // 2. Inject Dependencies (Constructor Injection)
  GameController game(std::move(view), std::move(loader));

  // 3. Configure Agents
  // For demo purposes, we set P1 to Human and P2 to Random AI
  game.setAgent(0, new HumanAgent());
  game.setAgent(1, new AI_RandomAgent());

  // 4. Start Facade
  game.initializeGame();
  game.runGameLoop();

  return 0;
}
