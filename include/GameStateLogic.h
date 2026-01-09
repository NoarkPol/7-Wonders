#ifndef SEVEN_WONDERS_DUEL_GAMESTATELOGIC_H
#define SEVEN_WONDERS_DUEL_GAMESTATELOGIC_H

#include "Global.h"

namespace SevenWondersDuel {

    class GameController; // Forward declaration

    /**
     * @brief 游戏状态逻辑基类 (State Pattern Interface)
     * 定义了特定游戏状态下动作验证的接口。
     * 每个具体的游戏阶段 (如轮抽、主时代、等待选择等) 都有一个对应的子类。
     */
    class IGameStateLogic {
    public:
        virtual ~IGameStateLogic() = default;
        
        /**
         * @brief 进入该状态时的初始化逻辑
         */
        virtual void onEnter(GameController& controller);

        /**
         * @brief 验证当前状态下的动作是否合法
         * @param action 玩家尝试执行的动作
         * @param controller 游戏控制器上下文
         * @return 验证结果 (包含是否通过及错误信息)
         */
        virtual ActionResult validate(const Action& action, GameController& controller) = 0;
    };

    /**
     * @brief 1. 奇迹轮抽状态
     * 玩家从桌面上的 4 张奇迹中选择 1 张。
     */
    class WonderDraftState : public IGameStateLogic {
    public:
        ActionResult validate(const Action& action, GameController& controller) override;

    };

    /**
     * @brief 2. 时代主流程状态
     * 游戏最主要的状态。玩家可以从金字塔拿牌进行：建造、弃牌换钱、建造奇迹。
     */
    class AgePlayState : public IGameStateLogic {
    public:
        ActionResult validate(const Action& action, GameController& controller) override;
    };

    /**
     * @brief 3. 等待选择科技标记
     * 触发条件：凑齐一对科技符号，或建造了图书馆奇迹。
     */
    class TokenSelectionState : public IGameStateLogic {
    public:
        ActionResult validate(const Action& action, GameController& controller) override;
    };

    /**
     * @brief 4. 等待摧毁卡牌
     * 触发条件：建造了某些具有摧毁效果的奇迹或卡牌 (如 Zeus 雕像)。
     */
    class DestructionState : public IGameStateLogic {
    public:
        ActionResult validate(const Action& action, GameController& controller) override;
    };

    /**
     * @brief 5. 等待弃牌堆复活
     * 触发条件：建造了陵墓奇迹 (Mausoleum)，允许从弃牌堆免费建造一张卡。
     */
    class DiscardBuildState : public IGameStateLogic {
    public:
        ActionResult validate(const Action& action, GameController& controller) override;
    };

    /**
     * @brief 6. 等待选择先手
     * 触发条件：时代结束时，军事优势方决定下一时代谁先手。
     */
    class StartPlayerSelectionState : public IGameStateLogic {
    public:
        ActionResult validate(const Action& action, GameController& controller) override;
    };
    
    /**
     * @brief 7. 游戏结束
     * 此时不再接受任何操作。
     */
    class GameOverState : public IGameStateLogic {
    public:
        ActionResult validate(const Action& action, GameController& controller) override;
    };


}

#endif // SEVEN_WONDERS_DUEL_GAMESTATELOGIC_H