#include "Engine.h"
#include "GameController.h"
#include "Model.h"

/**
 * ======================================================================================
 * [EDUCATIONAL NOTE] - Strategy Implementation
 * ======================================================================================
 * Here we implement the concrete behaviors defined in Engine.h.
 * Notice how each class only knows about its specific mechanic.
 */

namespace SevenWondersDuel {

void ProductionEffect::apply(Player *self, Player *opponent,
                             GameController *ctx) {
  // [DESIGN CHOICE] Passive Effect
  // Production effects typically don't "do" anything when card is built.
  // Instead, the Player::getResourceCount method (in Model.cpp) iterates over
  // cards and sums up resources. This is a "Pull" architecture (Model pulls
  // data from cards) rather than "Push".
}

void MilitaryEffect::apply(Player *self, Player *opponent,
                           GameController *ctx) {
  // [DESIGN CHOICE] Active Effect
  // Military effects immediately change game state (Board Position).
  int shields = shieldCount;
  if (self->hasBuff(ProgressToken::STRATEGY)) {
    shields++; // Strategy Token Logic: +1 Shield on all mil cards
  }

  // Context interaction: The Strategy (Effect) tells the Context
  // (Controller/Model) to update.
  bool victory = ctx->getModel()->board->militaryTrack.applyShields(
      shields, self, opponent);

  if (victory) {
    // [SOLID] - Dependency Inversion
    // The effect doesn't handle the "Win Screen". It just sets the state flag.
    ctx->getModel()->victoryType = VictoryType::MILITARY;
    ctx->getModel()->winnerIdx = self->id;
    ctx->setState(GameState::GAME_OVER);
  }
}

void ScienceEffect::apply(Player *self, Player *opponent, GameController *ctx) {
  self->addScienceSymbol(symbol);

  // [GAME LOGIC] Check for Pair
  // If we collected 2 identical symbols, we trigger a Progress Token selection.
  if (self->scienceSymbols[symbol] == 2) {
    // State Transition: The Strategy triggers a state change in the Context.
    ctx->setState(GameState::WAITING_FOR_TOKEN_SELECTION_PAIR);
  }
}

void CoinEffect::apply(Player *self, Player *opponent, GameController *ctx) {
  self->addCoins(amount);
}

// Guild Logic - Demonstrating complex scoring in Strategy
void GuildEffect::apply(Player *self, Player *opponent, GameController *ctx) {
  int count = 0;
  if (countWonders) {
    // Builders Guild: Most wonders built by one player
    int myW =
        self->builtWonders.size(); // self->unbuiltWonders unused for this rule
    int opW = opponent->builtWonders.size();
    count = std::max(myW, opW);
  } else {
    // Standard Guild: Count cards of specific color
    int myC = 0;
    int opC = 0;
    for (auto *c : self->builtCards)
      if (c->type == targetColor)
        myC++;
    for (auto *c : opponent->builtCards)
      if (c->type == targetColor)
        opC++;

    count = std::max(myC, opC);
  }

  if (coinsPerItem > 0) {
    self->addCoins(count * coinsPerItem);
  }
}

int GuildEffect::calculateScore(const Player *self,
                                const Player *opponent) const {
  int count = 0;
  if (countWonders) {
    int myW = self->builtWonders.size();
    int opW = opponent->builtWonders.size();
    count = std::max(myW, opW);
  } else {
    int myC = 0;
    int opC = 0;
    // Special case: Raw + Manufactured combined for Shipowners?
    // Simplification: targetColor is one type. If mixed, need logic.
    // Assuming simple guilds.
    for (auto *c : self->builtCards)
      if (c->type == targetColor)
        myC++;
    for (auto *c : opponent->builtCards)
      if (c->type == targetColor)
        opC++;
    count = std::max(myC, opC);
  }
  return count * pointsPerItem;
}

// Interactive Effects demonstrating State Machine interaction

void DestructionEffect::apply(Player *self, Player *opponent,
                              GameController *ctx) {
  // We set the parameter in Context (Context Object Pattern properties)
  ctx->destructionTargetType = targetColor;
  // And perform transition
  ctx->setState(GameState::WAITING_FOR_DESTRUCTION);
}

void LibraryEffect::apply(Player *self, Player *opponent, GameController *ctx) {
  ctx->setState(GameState::WAITING_FOR_TOKEN_SELECTION_LIB);
}

void ResurrectEffect::apply(Player *self, Player *opponent,
                            GameController *ctx) {
  ctx->setState(GameState::WAITING_FOR_DISCARD_BUILD);
}

void PlayAgainEffect::apply(Player *self, Player *opponent,
                            GameController *ctx) {
  // How this works: Controller::processTurn defaults to switching player.
  // It must check a flag or similar.
  // For this architecture demo, we assume Controller/Model captures this via
  // explicit signaling or checking the played card's effects. Ideally:
  // ctx->grantExtraTurn();
}

} // namespace SevenWondersDuel
