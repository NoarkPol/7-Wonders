#ifndef SEVEN_WONDERS_DUEL_GAMECONTROLLER_H
#define SEVEN_WONDERS_DUEL_GAMECONTROLLER_H

#include "IDataLoader.h"
#include "Model.h"
#include "View.h"
#include <map>
#include <memory>
#include <vector>

/**
 * ======================================================================================
 * [EDUCATIONAL NOTE] - The Controller Layer (Facade & Mediator)
 * ======================================================================================
 * [DESIGN PATTERN] - Facade Pattern
 * ---------------------------------
 * The `GameController` provides a simplified interface to the complex
 * subsystems (Model, Engine, View). The `main.cpp` only calls `runGameLoop()`.
 *
 * [DESIGN PATTERN] - Mediator Pattern
 * -----------------------------------
 * Components (View, Agents, Model) do not talk to each other directly.
 * - View doesn't modify Model -> It sends Input to Controller.
 * - Agents don't execute Moves -> They send Actions to Controller.
 * - Model doesn't draw itself -> Controller tells View to draw.
 * The Controller centralizes the communication.
 *
 * [DESIGN PATTERN] - State Pattern (Machine)
 * ------------------------------------------
 * The Game Loop is a State Machine driven by `currentState`.
 */

namespace SevenWondersDuel {

class GameController {
public:
  // [DEPENDENCY INJECTION]
  // We inject the "Eyes" (View) and the "World Builder" (Loader)
  GameController(std::unique_ptr<GameView> v, std::unique_ptr<IDataLoader> l);
  ~GameController() = default;

  // Configuration
  void setAgent(int playerId, IPlayerAgent *agent);

  // [FACADE] - One method to start everything
  void initializeGame();
  void runGameLoop();

  // Helper getters for strategies (IEffects) to access context
  GameModel *getModel() { return model.get(); }
  void setState(GameState s) { currentState = s; }

  // Context Parameters for States
  // (Similar to "Blackboard" pattern where context shares data)
  CardType destructionTargetType = CardType::MANUFACTURED;

private:
  // subsystems
  std::unique_ptr<GameModel> model;
  std::unique_ptr<GameView> view;
  std::unique_ptr<IDataLoader> loader;

  // Agent Registry
  std::map<int, IPlayerAgent *> agents;

  // State Machine Core
  GameState currentState = GameState::WONDER_DRAFT_PHASE_1;
  bool isRunning = true;

  // Internal Logic
  void processTurn();
  ActionResult validateAction(const Action &action, Player *p);
  void executeAction(const Action &action, Player *p);

  // State Handlers
  void handleWonderDraft();
  void triggerIdling(); // Handles Waiting States
  void handleAgeEnd();
};

} // namespace SevenWondersDuel

#endif // SEVEN_WONDERS_DUEL_GAMECONTROLLER_H
