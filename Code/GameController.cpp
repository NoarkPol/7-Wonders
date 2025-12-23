#include "GameController.h"
#include "DataLoader.h"
#include "RulesEngine.h"
#include "ScoringManager.h"
#include "GameStateLogic.h" // Include concrete states
#include "GameCommands.h" // Include commands
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <cmath>

namespace SevenWondersDuel {

    GameController::GameController() {
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        rng.seed(seed);
        model = std::make_unique<GameModel>();
        // Initialize default state
        updateStateLogic(GameState::WONDER_DRAFT_PHASE_1);
    }

    GameController::~GameController() = default;

    void GameController::updateStateLogic(GameState newState) {
        switch (newState) {
            case GameState::WONDER_DRAFT_PHASE_1:
            case GameState::WONDER_DRAFT_PHASE_2:
                m_stateLogic = std::make_unique<WonderDraftState>();
                break;
            case GameState::AGE_PLAY_PHASE:
                m_stateLogic = std::make_unique<AgePlayState>();
                break;
            case GameState::WAITING_FOR_TOKEN_SELECTION_PAIR:
            case GameState::WAITING_FOR_TOKEN_SELECTION_LIB:
                m_stateLogic = std::make_unique<TokenSelectionState>();
                break;
            case GameState::WAITING_FOR_DESTRUCTION:
                m_stateLogic = std::make_unique<DestructionState>();
                break;
            case GameState::WAITING_FOR_DISCARD_BUILD:
                m_stateLogic = std::make_unique<DiscardBuildState>();
                break;
            case GameState::WAITING_FOR_START_PLAYER_SELECTION:
                m_stateLogic = std::make_unique<StartPlayerSelectionState>();
                break;
            case GameState::GAME_OVER:
                m_stateLogic = std::make_unique<GameOverState>();
                break;
        }
        if (m_stateLogic) {
            m_stateLogic->onEnter(*this);
        }
    }

    void GameController::loadData(const std::string& path) {
        std::vector<Card> cards;
        std::vector<Wonder> wonders;

        bool success = DataLoader::loadFromFile(path, cards, wonders);
        if (!success) {
            std::cerr << "CRITICAL ERROR: Failed to load game data from " << path << std::endl;
            exit(1);
        }
        model->populateData(std::move(cards), std::move(wonders));
    }

    void GameController::initializeGame(const std::string& jsonPath) {
        loadData(jsonPath);

        model->clearPlayers();
        model->addPlayer(std::make_unique<Player>(0, "Player 1"));
        model->addPlayer(std::make_unique<Player>(1, "Player 2"));

        model->setCurrentAge(0);
        model->setCurrentPlayerIndex(0);
        model->setWinnerIndex(-1);
        model->setVictoryType(VictoryType::NONE);
        model->clearLog();

        // 随机标记
        std::vector<ProgressToken> allTokens = {
            ProgressToken::AGRICULTURE, ProgressToken::URBANISM,
            ProgressToken::STRATEGY, ProgressToken::THEOLOGY,
            ProgressToken::ECONOMY, ProgressToken::MASONRY,
            ProgressToken::ARCHITECTURE, ProgressToken::LAW,
            ProgressToken::MATHEMATICS, ProgressToken::PHILOSOPHY
        };

        std::shuffle(allTokens.begin(), allTokens.end(), rng);

        model->getBoardMut()->setAvailableProgressTokens({});
        for(int i=0; i<5; ++i) {
            model->getBoardMut()->addAvailableProgressToken(allTokens[i]);
        }

        model->getBoardMut()->setBoxProgressTokens({});
        for(size_t i=5; i<allTokens.size(); ++i) {
            model->getBoardMut()->addBoxProgressToken(allTokens[i]);
        }

        model->addLog("[System] Game Initialized. Progress Tokens shuffled.");
    }

    void GameController::startGame() {
        setState(GameState::WONDER_DRAFT_PHASE_1);
        draftTurnCount = 0;
        initWondersDeck();
        dealWondersToDraft();
        model->addLog("[System] Game Started. Wonder Draft Phase 1.");
    }

    void GameController::initWondersDeck() {
        model->clearRemainingWonders();
        std::vector<Wonder*> temp = model->getPointersToAllWonders();
        std::shuffle(temp.begin(), temp.end(), rng);
        for (auto w : temp) {
            model->addToRemainingWonders(w);
        }
    }

    void GameController::dealWondersToDraft() {
        model->clearDraftPool();
        for (int i = 0; i < 4; ++i) {
            Wonder* w = model->backRemainingWonder();
            if (w) {
                model->addToDraftPool(w);
                model->popRemainingWonder();
            }
        }
    }

    // --- 流程控制 ---

    void GameController::setupAge(int age) {
        model->setCurrentAge(age);
        std::vector<Card*> deck = prepareDeckForAge(age);
        model->getBoardMut()->initPyramid(age, deck);
        setState(GameState::AGE_PLAY_PHASE);
        model->addLog("[System] Age " + std::to_string(age) + " Begins!");
    }

    void GameController::prepareNextAge() {
        // 检查是否结束 Age 3 -> 文官胜利
        if (model->getCurrentAge() == 3) {
            setState(GameState::GAME_OVER);
            model->setVictoryType(VictoryType::CIVILIAN);

            // 计算分数判定胜负
            int s1 = ScoringManager::calculateScore(*model->getPlayers()[0], *model->getPlayers()[1], *model->getBoard());
            int s2 = ScoringManager::calculateScore(*model->getPlayers()[1], *model->getPlayers()[0], *model->getBoard());

            if (s1 > s2) model->setWinnerIndex(0);
            else if (s2 > s1) model->setWinnerIndex(1);
            else {
                // [FIX] 平局判定：蓝卡分
                int blue1 = ScoringManager::calculateBluePoints(*model->getPlayers()[0], *model->getPlayers()[1]);
                int blue2 = ScoringManager::calculateBluePoints(*model->getPlayers()[1], *model->getPlayers()[0]);

                if (blue1 > blue2) {
                    model->setWinnerIndex(0);
                    model->addLog("[System] Score Tie! Player 1 wins by Civilian (Blue) Points.");
                } else if (blue2 > blue1) {
                    model->setWinnerIndex(1);
                    model->addLog("[System] Score Tie! Player 2 wins by Civilian (Blue) Points.");
                } else {
                    model->setWinnerIndex(-1); // Shared Victory / True Draw
                    model->addLog("[System] True Draw! (Scores and Blue Points identical)");
                }
            }
            return;
        }

        // 准备进入下一时代：判定谁决定先手
        int decisionMaker = -1;
        int pos = model->getBoard()->getMilitaryTrack().getPosition();

        if (pos > 0) decisionMaker = 1;
        else if (pos < 0) decisionMaker = 0;
        else decisionMaker = model->getCurrentPlayerIndex(); // 刚刚行动完的玩家

        model->setCurrentPlayerIndex(decisionMaker);
        setState(GameState::WAITING_FOR_START_PLAYER_SELECTION);

        model->addLog("[System] End of Age " + std::to_string(model->getCurrentAge()) +
                      ". " + model->getCurrentPlayer()->getName() + " chooses who starts next age.");
    }

    std::vector<Card*> GameController::prepareDeckForAge(int age) {
        std::vector<Card*> deck;
        std::vector<Card*> ageCards;
        std::vector<Card*> guildCards;

        for(const auto& c_const : model->getAllCards()) {
            // Get mutable pointer via model
            Card* c = model->findCardById(c_const.getId());
            if (c->getType() == CardType::GUILD) {
                guildCards.push_back(c);
            } else if (c->getAge() == age) {
                ageCards.push_back(c);
            }
        }

        std::shuffle(ageCards.begin(), ageCards.end(), rng);

        if (ageCards.size() > 3) {
            ageCards.resize(ageCards.size() - 3);
        }

        for (auto c : ageCards) deck.push_back(c);

        if (age == 3) {
            std::shuffle(guildCards.begin(), guildCards.end(), rng);
            if (guildCards.size() > 3) {
                guildCards.resize(3);
            }
            for (auto c : guildCards) deck.push_back(c);
            std::shuffle(deck.begin(), deck.end(), rng);
        }

        return deck;
    }

    void GameController::switchPlayer() {
        model->setCurrentPlayerIndex(1 - model->getCurrentPlayerIndex());
    }

    void GameController::onTurnEnd() {
        // 1. 检查胜利条件 (军事/科技)
        checkVictoryConditions();
        if (currentState == GameState::GAME_OVER) return;

        // 2. 检查是否时代结束 (金字塔空了)
        if (model->getRemainingCardCount() == 0) {
            prepareNextAge();
            return;
        }

        // 3. 正常回合切换
        if (extraTurnPending) {
            extraTurnPending = false;
            model->addLog(">> EXTRA TURN for " + model->getCurrentPlayer()->getName());
        } else {
            switchPlayer();
        }
    }

    // --- Actions ---
    // (Handled by GameCommands via CommandFactory)



    // --- 校验 ---

    ActionResult GameController::validateAction(const Action& action) {
        if (m_stateLogic) {
            return m_stateLogic->validate(action, *this);
        }
        return {false, 0, "State Logic not initialized"};
    }

    bool GameController::processAction(const Action& action) {
        ActionResult v = validateAction(action);
        if (!v.isValid) return false;

        auto cmd = CommandFactory::createCommand(action);
        if (cmd) {
            cmd->execute(*this);
            return true;
        }
        return false;
    }

    // --- 缺失函数实现 ---

    bool GameController::checkForNewSciencePairs(Player* p) {
        for (auto const& [sym, count] : p->getScienceSymbols()) {
            if (sym == ScienceSymbol::NONE) continue;

            if (count >= Config::SCIENCE_PAIR_COUNT) {
                if (p->getClaimedSciencePairs().find(sym) == p->getClaimedSciencePairs().end()) {
                    p->addClaimedSciencePair(sym);
                    setState(GameState::WAITING_FOR_TOKEN_SELECTION_PAIR);
                    model->addLog(p->getName() + " collected a Science Pair! Choose a Progress Token.");
                    return true;
                }
            }
        }
        return false;
    }

    void GameController::resolveMilitaryLoot(const std::vector<int>& lootEvents) {
        for (int amount : lootEvents) {
            if (amount > 0) {
                int loss = std::min(model->getPlayers()[1]->getCoins(), amount);
                model->getPlayers()[1]->payCoins(loss);
                model->addLog("[Military] Player 2 lost " + std::to_string(loss) + " coins!");
            } else {
                int loss = std::min(model->getPlayers()[0]->getCoins(), std::abs(amount));
                model->getPlayers()[0]->payCoins(loss);
                model->addLog("[Military] Player 1 lost " + std::to_string(loss) + " coins!");
            }
        }
    }

    void GameController::checkVictoryConditions() {
         VictoryResult result = RulesEngine::checkInstantVictory(*model->getPlayers()[0], *model->getPlayers()[1], *model->getBoard());
        if (result.isGameOver) {
            setState(GameState::GAME_OVER);
            model->setWinnerIndex(result.winnerIndex);
            model->setVictoryType(result.type);
        }
    }

    Card* GameController::findCardInPyramid(const std::string& id) {
        return model->findCardById(id);
    }

    Wonder* GameController::findWonderInHand(const Player* p, const std::string& id) {
        for(auto w : p->getUnbuiltWonders()) if (w->getId() == id) return w;
        return nullptr;
    }

    std::vector<int> GameController::moveMilitary(int shields, int playerId) {
        return model->getBoardMut()->moveMilitary(shields, playerId);
    }

    bool GameController::isDiscardPileEmpty() const {
        return model->getBoard()->getDiscardPile().empty();
    }

}
