#include "GameController.h"
#include "GameView.h"
#include "Agent.h"
#include <iostream>
#include <memory>
#include <limits>

using namespace SevenWondersDuel;

int main() {
    // 1. 初始化
    GameView view;
    GameController game;

    // 加载数据 (请确保 gamedata.json 存在于运行目录)
    game.initializeGame("../gamedata.json");

    // 2. 游戏配置菜单
    view.renderMainMenu();

    std::unique_ptr<IPlayerAgent> agent1;
    std::unique_ptr<IPlayerAgent> agent2;

    int modeChoice;
    std::cin >> modeChoice;

    // IMPORTANT: Clear the buffer after reading int to prevent prompt skipping
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (modeChoice == 1) {
        agent1 = std::make_unique<HumanAgent>();
        agent2 = std::make_unique<HumanAgent>();
    } else if (modeChoice == 3) {
        agent1 = std::make_unique<RandomAIAgent>();
        agent2 = std::make_unique<RandomAIAgent>();
    } else {
        // Default: Human vs AI
        agent1 = std::make_unique<HumanAgent>();
        agent2 = std::make_unique<RandomAIAgent>();
    }

    // 3. 开始游戏
    game.startGame();

    // 4. 主循环
    while (game.getState() != GameState::GAME_OVER) {
        const auto& model = game.getModel();

        // 渲染逻辑优化：
        // 如果当前是人类玩家，promptHumanAction 内部会处理渲染，所以这里不需要 renderGame
        // 但如果是 AI 玩家，我们需要在这里渲染以便观看
        // 或者，为了简单统一，我们在 AI 行动前渲染一次。
        IPlayerAgent* currentAgent = (model.currentPlayerIndex == 0) ? agent1.get() : agent2.get();

        if (!currentAgent->isHuman()) {
            view.renderGameForAI(model); // 使用 AI 专用渲染接口 (其实就是 renderGame)
            view.printTurnInfo(model.getCurrentPlayer());
        }

        // 获取决策
        bool actionSuccess = false;
        while (!actionSuccess) {
            // 如果是 HumanAgent，promptHumanAction 会负责清屏、渲染、报错循环
            // 如果是 RandomAI，它直接返回 Action
            Action action = currentAgent->decideAction(game, view);

            // 逻辑验证 (扣钱/规则校验)
            ActionResult val = game.validateAction(action);

            if (game.processAction(action)) {
                actionSuccess = true;
                // 成功执行后，清除错误信息 (如果有残留)
                view.clearLastError();

                if (!currentAgent->isHuman()) {
                    // AI 动作后稍微停顿或打印 (已在 Agent.cpp 中 sleep)
                }
            } else {
                // 动作逻辑失败 (例如钱不够)
                if (currentAgent->isHuman()) {
                    // 将错误信息注入 View，并在下一次循环的 promptHumanAction 中显示
                    view.setLastError("Action Failed: " + val.message);
                    // 循环继续，promptHumanAction 会再次被调用，并渲染此错误
                } else {
                    // [Fix] GameView 移除了 printError，对于 AI 的严重逻辑错误，直接使用标准错误流输出
                    std::cerr << "\033[1;31m[CRITICAL] AI attempted invalid action: " << val.message << "\033[0m" << std::endl;
                    actionSuccess = true; // Skip to prevent infinite loop
                }
            }
        }
    }

    // 5. 游戏结束
    view.renderGameForAI(game.getModel()); // 最后一帧

    std::cout << "\n=========================================\n";
    std::cout << "              GAME OVER                  \n";
    std::cout << "=========================================\n";

    const auto& model = game.getModel();
    if (model.winnerIndex != -1) {
        std::cout << "WINNER: " << model.players[model.winnerIndex]->name << "!\n";
        std::string vType;
        switch(model.victoryType) {
            case VictoryType::MILITARY: vType = "Military Supremacy"; break;
            case VictoryType::SCIENCE: vType = "Scientific Supremacy"; break;
            case VictoryType::CIVILIAN: vType = "Civilian Victory (Points)"; break;
            default: vType = "Unknown";
        }
        std::cout << "Victory Type: " << vType << "\n";

        // 显示分数详情
        if (model.victoryType == VictoryType::CIVILIAN) {
             std::cout << "Final Scores:\n";
             std::cout << "  " << model.players[0]->name << ": " << model.players[0]->getScore(*model.players[1]) << "\n";
             std::cout << "  " << model.players[1]->name << ": " << model.players[1]->getScore(*model.players[0]) << "\n";
        }
    }

    return 0;
}