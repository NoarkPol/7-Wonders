#include "View.h"
#include <iomanip>
#include <iostream>

namespace SevenWondersDuel {

// --- GameView ---
void GameView::renderMessage(const std::string &msg) {
  std::cout << "[GAME] " << msg << std::endl;
}

void GameView::renderPlayer(const Player *p, bool isActive) {
  std::cout << (isActive ? "--> " : "    ") << p->name << " (ID:" << p->id
            << ")\n";
  std::cout << "    Coins: " << p->coins
            << " | VP (approx): " << p->getVictoryPoints(nullptr) << "\n";
  std::cout << "    Resources: ";
  // Simple scan to show resources? Hard without iterating built cards.
  // For debugging, just show card count.
  std::cout << "Built Cards: " << p->builtCards.size()
            << " | Wonders: " << p->builtWonders.size() << "\n";
  std::cout << "    Science: ";
  for (auto &pair : p->scienceSymbols)
    if (pair.second > 0)
      std::cout << (int)pair.first << "(" << pair.second << ") ";
  std::cout << "\n";
}

void GameView::renderMilitary(const MilitaryTrack *track) {
  std::cout << "    Military Track Position: " << track->position << "\n";
  std::cout << "    Loot Zones: " << (track->lootZones[0] ? "[P1-2]" : "[X]")
            << " " << (track->lootZones[1] ? "[P1-5]" : "[X]") << " "
            << (track->lootZones[2] ? "[P2-2]" : "[X]") << " "
            << (track->lootZones[3] ? "[P2-5]" : "[X]") << "\n";
}

void GameView::renderBoard(const Board *board) {
  std::cout << "\n=== BOARD ===\n";
  renderMilitary(&board->militaryTrack);
  std::cout << "    Pyramid (Available Cards):\n";
  auto avail = board->cardStructure.getAvailableCards();
  for (auto *c : avail) {
    std::cout << "      [" << c->id << "] Cost: " << c->cost.coins << "C\n";
  }
  std::cout << "    Discard Pile Size: " << board->discardPile.size() << "\n\n";
}

void GameView::renderDraft(const std::vector<Wonder *> &wonders) {
  std::cout << "\n=== WONDER DRAFT ===\n";
  for (size_t i = 0; i < wonders.size(); ++i) {
    std::cout << i << ". " << wonders[i]->name << "\n";
  }
}

void GameView::renderGameState(const GameModel &model) {
  std::cout << "\n--------------------------------------------------\n";
  std::cout << "AGE " << model.currentAge << "\n";
  renderPlayer(model.players[0].get(), model.currentPlayerIdx == 0);
  renderPlayer(model.players[1].get(), model.currentPlayerIdx == 1);
  renderBoard(model.board.get());
  std::cout << "--------------------------------------------------\n";
}

void GameView::renderGameOver(const GameModel &model) {
  std::cout << "\n!!! GAME OVER !!!\n";
  std::cout << "Winner: Player " << model.winnerIdx << "\n";
  if (model.victoryType == VictoryType::MILITARY)
    std::cout << "Type: MILITARY SUPREMACY\n";
  else if (model.victoryType == VictoryType::SCIENCE)
    std::cout << "Type: SCIENCE SUPREMACY\n";
  else if (model.victoryType == VictoryType::CIVILIAN)
    std::cout << "Type: CIVILIAN VICTORY\n";
}

// --- Agents ---
// Human
// --- Agents ---
// Human
Action HumanAgent::decideNextMove(const GameModel &model) {
  std::cout << "\n[Your Turn] Choose action:\n";
  std::cout
      << "1. Build Card\n2. Discard Card (for coins)\n3. Build Wonder\n> ";
  int choice;
  if (!(std::cin >> choice)) { // Input validation
    std::cin.clear();
    std::cin.ignore(1000, '\n');
    return {ActionType::DISCARD_FOR_COINS}; // fallback
  }

  Action action;
  if (choice == 1)
    action.type = ActionType::BUILD_CARD;
  else if (choice == 2)
    action.type = ActionType::DISCARD_FOR_COINS;
  else if (choice == 3)
    action.type = ActionType::BUILD_WONDER;
  else
    action.type = ActionType::DISCARD_FOR_COINS; // Default safe op

  if (action.type == ActionType::BUILD_WONDER) {
    std::cout << "Enter Wonder ID: ";
    std::cin >> action.targetWonderId;
    std::cout << "Enter Card ID to tuck: ";
    std::cin >> action.targetCardId;
  } else {
    std::cout << "Enter Card ID: ";
    std::cin >> action.targetCardId;
  }
  return action;
}

int HumanAgent::decideStartingPlayer(const GameModel &model) {
  std::cout << "You have military disadvantage. Choose starting player "
               "(0=You, 1=Opponent): ";
  int c;
  std::cin >> c;
  return c;
}

ProgressToken
HumanAgent::selectProgressToken(const std::vector<ProgressToken> &options,
                                const GameModel &model) {
  std::cout << "Select Progress Token (index): ";
  for (size_t i = 0; i < options.size(); ++i)
    std::cout << i << ". " << (int)options[i] << " ";
  int c;
  std::cin >> c;
  if (c >= 0 && c < options.size())
    return options[c];
  return options[0];
}

std::string HumanAgent::selectCardToDestroy(const std::vector<Card *> &targets,
                                            const GameModel &model) {
  std::cout << "Select card to destroy (ID): ";
  std::string s;
  std::cin >> s;
  return s;
}

std::string
HumanAgent::selectCardFromDiscard(const std::vector<Card *> &targets,
                                  const GameModel &model) {
  std::cout << "Select card from discard (ID): ";
  std::string s;
  std::cin >> s;
  return s;
}

// --- AI Agent Implementation ---
// [STRATEGY PATTERN] - A different implementation of IPlayerAgent.
// This allows swapping "Brains" without changing "Body" (GameController).

Action AI_RandomAgent::decideNextMove(const GameModel &model) {
  // [AI LOGIC]
  // A smarter AI would:
  // 1. Get all legal actions (GameController::getLegalActions)
  // 2. Evaluate state after each action (Minimax / MCTS)
  // 3. Return best action.

  // This Random AI is a placeholder.
  // It relies on "Validation" to fail if it picks a bad card, then
  // randomizes. In a real impl, we should scan
  // `model.board->cardStructure.getAvailableCards()`.

  // ... Implementation (truncated for brevity)
  Action a;
  a.type = ActionType::BUILD_CARD; // Default attempt
  return a;
}

int AI_RandomAgent::decideStartingPlayer(const GameModel &model) {
  return 0;
} // Always me
ProgressToken
AI_RandomAgent::selectProgressToken(const std::vector<ProgressToken> &options,
                                    const GameModel &m) {
  return options.empty() ? ProgressToken::NONE : options[0];
}
std::string AI_RandomAgent::selectCardToDestroy(const std::vector<Card *> &t,
                                                const GameModel &m) {
  return t.empty() ? "" : t[0]->id;
}
std::string AI_RandomAgent::selectCardFromDiscard(const std::vector<Card *> &t,
                                                  const GameModel &m) {
  return t.empty() ? "" : t[0]->id;
}

} // namespace SevenWondersDuel
