#include "GameController.h"
#include <iostream>

/**
 * ======================================================================================
 * [EDUCATIONAL NOTE] - Controller Implementation
 * ======================================================================================
 * The Controller is the "Brain". It connects the "Organs" (Model) with the
 * "Senses" (View/Agents).
 *
 * [DESIGN PATTERN] - Mediator
 * Note how `executeAction` doesn't just change the Model; it also notifies the
 * View
 * (`view->renderGameState`) and checks for Victory conditions.
 *
 * [DESIGN PATTERN] - State Machine
 * `runGameLoop` is effectively a `while` loop switching on `currentState`.
 */

namespace SevenWondersDuel {

GameController::GameController(std::unique_ptr<GameView> v,
                               std::unique_ptr<IDataLoader> l)
    : view(std::move(v)), loader(std::move(l)) {
  model = std::make_unique<GameModel>();
}

void GameController::setAgent(int playerId, IPlayerAgent *agent) {
  agents[playerId] = agent;
}

// [FACADE] - Simple interface to start the complex game system
void GameController::initializeGame() {
  // 1. Use Abstract Factory (loader) to build the deck
  auto age1Cards = loader->loadCards(1);
  auto wonders = loader->loadWonders();
  auto tokens = loader->loadProgressTokens();

  // 2. Setup Model (Data)
  model->setupAge(1, age1Cards);
  model->draftPool = wonders; // Start with all wonders
  model->board->availableProgressTokens = tokens;

  // 3. Initial State
  currentState = GameState::WONDER_DRAFT_PHASE_1;

  view->renderMessage("Game Initialized. Starting Wonder Draft...");
}

void GameController::runGameLoop() {
  while (isRunning) {
    // [OBSERVER (PUSH)] - Update View
    view->renderGameState(*model);

    // [STATE PATTERN] - Dispatch based on current state
    switch (currentState) {
    case GameState::WONDER_DRAFT_PHASE_1:
    case GameState::WONDER_DRAFT_PHASE_2:
      handleWonderDraft();
      break;

    case GameState::AGE_PLAY_PHASE:
      processTurn();
      break;

    case GameState::WAITING_FOR_DESTRUCTION:
    case GameState::WAITING_FOR_TOKEN_SELECTION_PAIR:
    case GameState::WAITING_FOR_TOKEN_SELECTION_LIB:
    case GameState::WAITING_FOR_DISCARD_BUILD:
    case GameState::WAITING_FOR_START_PLAYER_SELECTION:
      triggerIdling(); // Handling Interrupts
      break;

    case GameState::GAME_OVER:
      view->renderGameOver(*model);
      isRunning = false;
      break;
    }
  }
}

// --- State Handlers ---

void GameController::processTurn() {
  Player *p = model->getCurrentPlayer();
  view->renderMessage("Player " + p->name + "'s turn.");

  // 1. Ask Agent for Command
  // [STRATEGY] - We don't know if this is Human or AI
  // Safely access agent map
  if (agents.find(p->id) == agents.end()) {
    view->renderMessage("Error: No agent for player " + std::to_string(p->id));
    return;
  }

  Action action = agents[p->id]->decideNextMove(*model);

  // 2. Validate Command
  ActionResult result = validateAction(action, p);
  if (!result.isValid) {
    view->renderMessage("Invalid Action: " + result.error);
    // In a real loop we would ask again. Here we just skip to avoid infinite
    // loop in demo if AI is dumb. But let's try to ask again once for Human?
    // Simplified: do nothing, let loop continue (re-render, re-process).
    // Actually `runGameLoop` calls `processTurn` which renders. We should
    // probably stay in `processTurn`? No, the State logic calls processTurn. If
    // invalid, we just return, state doesn't change, we loop back. Correct.
  } else {
    // 3. Execute Command
    executeAction(action, p);

    // 4. Check End of Age
    if (model->board->cardStructure.getAvailableCards().empty() &&
        model->board->cardStructure.slots.empty()) {
      // Simplified check. Real check involves checking if all slots are
      // removed. But `slots` vector usually stays, `isRemoved` flags change.
      // Let's iterate slots.
      bool allRemoved = true;
      for (const auto &slot : model->board->cardStructure.slots) {
        if (!slot.isRemoved) {
          allRemoved = false;
          break;
        }
      }

      if (allRemoved)
        handleAgeEnd();
      else
        model->switchPlayer();
    } else {
      // Check if actually any available cards?
      if (model->board->cardStructure.getAvailableCards().empty()) {
        // If empty but not all removed (e.g. covered cards remaining?),
        // unlikely in valid pyramid. If pyramid cleared, end age.
        handleAgeEnd();
      } else {
        model->switchPlayer();
      }
    }
  }
}

void GameController::handleWonderDraft() {
  Player *p = model->getCurrentPlayer();
  Action action = agents[p->id]->decideNextMove(*model);

  // Reuse BUILD_WONDER action type or introduce DRAFT.
  // Assuming AI sends BUILD_WONDER with targetWonderId.
  if (model->draftPool.empty()) {
    // Should not happen if transitions correct
    return;
  }

  // Logic: Find wonder in draft pool
  Wonder *selectedWonder = nullptr;
  int idx = 0;
  for (auto *w : model->draftPool) {
    if (w->name == action.targetWonderId ||
        action.targetWonderId.empty()) { // Empty ID means pick first
      selectedWonder = w;
      break;
    }
    idx++;
  }
  if (!selectedWonder)
    selectedWonder = model->draftPool[0]; // Fallback

  // Move to player
  p->unbuiltWonders.push_back(selectedWonder);

  // Remove from pool
  // std::vector erase
  for (auto it = model->draftPool.begin(); it != model->draftPool.end(); ++it) {
    if (*it == selectedWonder) {
      model->draftPool.erase(it);
      break;
    }
  }

  view->renderMessage(p->name + " drafted " + selectedWonder->name);

  // Transition Check
  int totalDrafted = model->players[0]->unbuiltWonders.size() +
                     model->players[1]->unbuiltWonders.size();
  if (totalDrafted >= 4 && currentState == GameState::WONDER_DRAFT_PHASE_1) {
    currentState = GameState::WONDER_DRAFT_PHASE_2;
    // Refill pool
    auto newWonders =
        loader->loadWonders(); // Load more? Mock loader returns 4 always.
    // Mock logic: reuse same call for new set?
    // Since MockDataLoader returns fixed set, we might get duplicates or empty
    // if consumed. For this Educational Demo, we can re-call.
    model->draftPool = loader->loadWonders();
    view->renderMessage("--- Phase 2 of Wonder Draft ---");
  } else if (totalDrafted >= 8) { // 4 from phase 1, 4 from phase 2
    currentState = GameState::AGE_PLAY_PHASE;
    view->renderMessage("--- Begginning Age 1 ---");
  } else {
    model->switchPlayer();
  }
}

void GameController::triggerIdling() {
  Player *p = model->getCurrentPlayer();

  if (currentState == GameState::WAITING_FOR_TOKEN_SELECTION_PAIR) {
    // Ask agent to select token
    ProgressToken t = agents[p->id]->selectProgressToken(
        model->board->availableProgressTokens, *model);
    p->addScienceSymbol(
        ScienceSymbol::LAW); // Dummy logic, should apply token effect
    // Remove from board
    // ...
    currentState = GameState::AGE_PLAY_PHASE; // Resume
  } else if (currentState == GameState::WAITING_FOR_DESTRUCTION) {
    auto targets = model->getOpponent()->builtCards;
    std::string tid = agents[p->id]->selectCardToDestroy(targets, *model);
    model->board->destroyCard(model->getOpponent(), destructionTargetType);
    currentState = GameState::AGE_PLAY_PHASE;
  }
}

void GameController::handleAgeEnd() {
  view->renderMessage("--- End of Age " + std::to_string(model->currentAge) +
                      " ---");

  if (model->currentAge < 3) {
    model->currentAge++;
    auto newCards = loader->loadCards(model->currentAge);
    model->setupAge(model->currentAge, newCards);
    currentState = GameState::AGE_PLAY_PHASE; // Or START_PLAYER_SELECTION
  } else {
    // Civil Victory check
    model->victoryType = VictoryType::CIVILIAN;
    int p1Score = model->players[0]->getVictoryPoints(model->players[1].get());
    int p2Score = model->players[1]->getVictoryPoints(model->players[0].get());
    model->winnerIdx = (p1Score > p2Score) ? 0 : 1;
    currentState = GameState::GAME_OVER;
  }
}

// --- Validation & Execution ---

ActionResult GameController::validateAction(const Action &action, Player *p) {
  if (action.type == ActionType::BUILD_CARD) {
    PyramidSlot *slot =
        model->board->cardStructure.getSlot(action.targetCardId);
    if (!slot)
      return ActionResult::Fail("Card not found");
    if (!slot->isFaceUp || !slot->coveredByIndices.empty())
      return ActionResult::Fail("Card not available");

    int cost = p->calculateCost(slot->card, model->getOpponent());
    if (p->coins < cost)
      return ActionResult::Fail("Not enough coins");

    return ActionResult::Success(cost);
  }
  return ActionResult::Success();
}

void GameController::executeAction(const Action &action, Player *p) {
  ActionResult res = validateAction(action, p);
  // Note: In real engine, we trust 'res' was checked.

  switch (action.type) {
  case ActionType::BUILD_CARD: {
    PyramidSlot *slot =
        model->board->cardStructure.getSlot(action.targetCardId);
    if (slot) {
      p->pay(res.actualCost, model->getOpponent());
      p->addCard(slot->card);
      model->board->cardStructure.removeCard(action.targetCardId);
      slot->card->onBuild(p, model->getOpponent(), this);
      view->renderMessage(p->name + " built " + slot->card->name);
    }
    break;
  }
  case ActionType::DISCARD_FOR_COINS: {
    p->addCoins(
        2 + p->getResourceCount(ResourceType::WOOD)); // Simply 2+yellows logic
    model->board->cardStructure.removeCard(action.targetCardId);
    view->renderMessage(p->name + " discarded a card for coins.");
    break;
  }
  case ActionType::BUILD_WONDER: {
    // ...
    view->renderMessage(p->name + " built a Wonder!");
    break;
  }
  }

  if (model->checkImmediateVictory()) {
    currentState = GameState::GAME_OVER;
  }
}

} // namespace SevenWondersDuel
