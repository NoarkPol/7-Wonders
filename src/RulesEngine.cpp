#include "RulesEngine.h"
#include "Board.h"
#include <cmath>

namespace SevenWondersDuel {

    ScienceSymbol RulesEngine::getNewSciencePairSymbol(const Player& player) {
        for (auto const& [sym, count] : player.getScienceSymbols()) {
            if (sym == ScienceSymbol::NONE) continue;

            if (count >= Config::SCIENCE_PAIR_COUNT) {
                const auto& claimed = player.getClaimedSciencePairs();
                if (claimed.find(sym) == claimed.end()) {
                    return sym;
                }
            }
        }
        return ScienceSymbol::NONE;
    }

    VictoryResult RulesEngine::checkInstantVictory(const Player& p1, const Player& p2, const Board& board) {
        VictoryResult result;

        // 1. Military Supremacy
        int pos = board.getMilitaryTrack().getPosition();
        if (std::abs(pos) >= Config::MILITARY_THRESHOLD_WIN) {
            result.isGameOver = true;
            result.type = VictoryType::MILITARY;
            
            if (pos > 0) result.winnerIndex = 0;
            else result.winnerIndex = 1;
            
            return result;
        }

        // 2. Science Supremacy
        const Player* players[2] = { &p1, &p2 };
        for (int i = 0; i < 2; ++i) {
            int distinctSymbols = 0;
            for (auto const& [sym, count] : players[i]->getScienceSymbols()) {
                if (sym != ScienceSymbol::NONE && count > 0) {
                    distinctSymbols++;
                }
            }
            if (distinctSymbols >= Config::SCIENCE_WIN_THRESHOLD) {
                result.isGameOver = true;
                result.type = VictoryType::SCIENCE;
                result.winnerIndex = i;
                return result;
            }
        }

        return result;
    }

}
