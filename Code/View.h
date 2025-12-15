#ifndef SEVEN_WONDERS_DUEL_VIEW_H
#define SEVEN_WONDERS_DUEL_VIEW_H

#include "Action.h"
#include "Model.h"

/**
 * ======================================================================================
 * [EDUCATIONAL NOTE] - The View Layer & Agent Interface
 * ======================================================================================
 * RESPONSIBILITY:
 * 1. UI Rendering: PURE Output. No game logic.
 * 2. Input Handling: Defines contract for Human/AI agents.
 *
 * [DESIGN PATTERN] - Strategy Pattern (for Agents)
 * ------------------------------------------------
 * PROBLEM: We want the game to support Human vs Human, Human vs AI, or AI vs
 * AI. SOLUTION: Usage of `IPlayerAgent`. Controller holds `IPlayerAgent*`. It
 * calls `decideNextMove()`. It doesn't care if the agent calculates MCTS,
 * prompts `cin`, or rolls a dice.
 *
 * [DESIGN PATTERN] - Observer Pattern (Simplified Push)
 * ----------------------------------------------------
 * `GameView::renderGameState(model)` is called by Controller whenever state
 * changes. This is the "Update" method in the Observer pattern.
 */

namespace SevenWondersDuel {

// 4.3 智能体接口 (IPlayerAgent) - Strategy Pattern
// Provides abstraction for decision making.
class IPlayerAgent {
public:
  virtual ~IPlayerAgent() = default;

  // Core Decision: "What do you want to do given this board state?"
  virtual Action decideNextMove(const GameModel &model) = 0;

  // Auxiliary Decisions (Sub-decisions needed during specific phases)
  virtual int decideStartingPlayer(const GameModel &model) = 0;
  virtual ProgressToken
  selectProgressToken(const std::vector<ProgressToken> &options,
                      const GameModel &model) = 0;
  virtual std::string
  selectCardToDestroy(const std::vector<Card *> &targets,
                      const GameModel &model) = 0; // Return ID
  virtual std::string selectCardFromDiscard(const std::vector<Card *> &targets,
                                            const GameModel &model) = 0;
};

// Human Implementation (Console Input)
class HumanAgent : public IPlayerAgent {
public:
  Action decideNextMove(const GameModel &model) override;
  int decideStartingPlayer(const GameModel &model) override;
  ProgressToken selectProgressToken(const std::vector<ProgressToken> &options,
                                    const GameModel &model) override;
  std::string selectCardToDestroy(const std::vector<Card *> &targets,
                                  const GameModel &model) override;
  std::string selectCardFromDiscard(const std::vector<Card *> &targets,
                                    const GameModel &model) override;
};

// AI Implementation (Random Strategy for Demo)
class AI_RandomAgent : public IPlayerAgent {
public:
  Action decideNextMove(const GameModel &model) override;
  int decideStartingPlayer(const GameModel &model) override;
  ProgressToken selectProgressToken(const std::vector<ProgressToken> &options,
                                    const GameModel &model) override;
  std::string selectCardToDestroy(const std::vector<Card *> &targets,
                                  const GameModel &model) override;
  std::string selectCardFromDiscard(const std::vector<Card *> &targets,
                                    const GameModel &model) override;
};

// 3. View (MVC)
// Pure rendering Logic.
class GameView {
public:
  void renderGameState(const GameModel &model);
  void renderMessage(const std::string &msg);
  void renderGameOver(const GameModel &model);

private:
  void renderPlayer(const Player *p, bool isActive);
  void renderBoard(const Board *board);
  void renderMilitary(const MilitaryTrack *track);
  void renderDraft(const std::vector<Wonder *> &wonders);
};

} // namespace SevenWondersDuel

#endif // SEVEN_WONDERS_DUEL_VIEW_H
