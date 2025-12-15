#ifndef SEVEN_WONDERS_DUEL_MODEL_H
#define SEVEN_WONDERS_DUEL_MODEL_H

#include "Card.h"
#include "Global.h"
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace SevenWondersDuel {

class Player;
class GameModel;

// 2.4 类 MilitaryTrack
class MilitaryTrack {
public:
  // Position relative to center. Range [-9, 9].
  int position = 0;
  // Loot tokens
  bool lootZones[4] = {true, true, true, true};

  bool applyShields(int count, Player *activePlayer, Player *opponent);
};

// --- 金字塔卡牌结构 (DAG) ---
struct PyramidSlot {
  int index;
  Card *card = nullptr;
  bool isFaceUp = false;
  bool isRemoved = false;
  int rowIndex;
  std::string id;
  std::vector<int> coveredByIndices;
  std::vector<int> coversIndices;
};

class CardPyramid {
public:
  std::vector<PyramidSlot> slots;
  void addSlot(int index, Card *c, bool faceUp, int row);
  void addDependency(int upperIndex, int lowerIndex);
  void removeCard(const std::string &cardId);
  PyramidSlot *getSlot(const std::string &cardId);
  std::vector<Card *> getAvailableCards() const;
};

// --- 游戏版图 ---
class Board {
public:
  MilitaryTrack militaryTrack;
  CardPyramid cardStructure;
  std::vector<ProgressToken> availableProgressTokens;
  std::vector<Card *> discardPile;
  void removeCard(const std::string &cardId);
  void destroyCard(Player *target, CardType type);
};

// --- 玩家类 ---
class Player {
public:
  int id;
  std::string name;
  int coins = 7;
  std::vector<Card *> builtCards;
  std::vector<Wonder *> builtWonders;
  std::vector<Wonder *> unbuiltWonders;
  std::map<ScienceSymbol, int> scienceSymbols;
  std::set<std::string> ownedChainSymbols;
  std::set<ProgressToken> activeBuffs;

  Player(int pid, std::string n) : id(pid), name(n) {}

  int getResourceCount(ResourceType t) const;
  bool hasChainSymbol(const std::string &tag) const;
  bool hasBuff(ProgressToken t) const;
  int calculateCost(const Card *card, const Player *opponent) const;
  int calculateWonderCost(const Wonder *wonder, const Player *opponent) const;
  void pay(int amount, Player *opponent);
  void addCoins(int amount);
  void loseCoins(int amount);
  void addCard(Card *card);
  void addWonder(Wonder *wonder);
  void buildWonder(Wonder *wonder, Card *tuck);
  void addScienceSymbol(ScienceSymbol s);
  int getVictoryPoints(const Player *opponent) const;
  int getMilitaryStrength() const;
};

// --- 游戏整体模型 (Data Root) ---
class GameModel {
public:
  std::array<std::unique_ptr<Player>, 2> players;
  std::unique_ptr<Board> board;
  int currentAge = 1;
  int currentPlayerIdx = 0;
  int winnerIdx = -1;
  VictoryType victoryType = VictoryType::NONE;
  std::vector<ProgressToken> unusedProgressTokens;
  std::vector<Wonder *> draftPool;

  GameModel();
  Player *getCurrentPlayer() const { return players[currentPlayerIdx].get(); }
  Player *getOpponent() const { return players[1 - currentPlayerIdx].get(); }
  void switchPlayer();
  bool checkImmediateVictory();
  void setupAge(int age, const std::vector<Card *> &deck);
  void eliminateEighthWonder();
  bool isWonderBuildLimitReached() const;
};

} // namespace SevenWondersDuel

#endif // SEVEN_WONDERS_DUEL_MODEL_H
