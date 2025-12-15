#ifndef SEVEN_WONDERS_DUEL_ACTION_H
#define SEVEN_WONDERS_DUEL_ACTION_H

#include "Global.h"

namespace SevenWondersDuel {

/**
 * [DESIGN PATTERN] - Command Pattern (Data Oriented Variant)
 * ==========================================================
 * CONTEXT: In many games, player input needs to be separated from execution
 * logic. PROBLEM: If we just call `model->buildCard("ID")` directly from UI, we
 * couple UI to Model. We also can't easily queue actions, validate them without
 * executing, or let AI simulate them. SOLUTION: Encapsulate a request as an
 * object (or struct).
 *
 * This `Action` struct acts as a "Command" or "DTO" (Data Transfer Object).
 * It allows:
 * 1. Decoupling: Agent generates Action -> Controller validates Action ->
 * Controller executes Action.
 * 2. AI Simulation: AI can generate 100 Actions, check which are valid, pick
 * the best one.
 */
struct Action {
  ActionType type;
  std::string targetCardId;
  std::string targetWonderId; // If building wonder

  // Optional params
  ProgressToken selectedToken = ProgressToken::NONE;
  ResourceType chosenResource = ResourceType::WOOD;
  int chosenPlayerIndex =
      -1; // For starting player selection or destruction target
};

/**
 * [SOLID] - Single Responsibility Principle
 * Result struct purely for validation feedback. It carries data, not behavior.
 */
struct ActionResult {
  bool isValid;
  int actualCost;    // Calculated cost including discounts
  std::string error; // Human readable error if invalid

  static ActionResult Success(int cost = 0) { return {true, cost, ""}; }
  static ActionResult Fail(const std::string &msg) { return {false, 0, msg}; }
};

} // namespace SevenWondersDuel

#endif // SEVEN_WONDERS_DUEL_ACTION_H
