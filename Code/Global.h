#ifndef SEVEN_WONDERS_DUEL_GLOBAL_H
#define SEVEN_WONDERS_DUEL_GLOBAL_H

#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <set>
#include <string>
#include <vector>

/**
 * ======================================================================================
 * [EDUCATIONAL NOTE] - Global Definitions
 * ======================================================================================
 * This file serves as the "Common Language" of the project domain.
 * By defining strict Enums instead of using raw integers (Magic Numbers) or
 * strings, we enforce type safety and make the code self-documenting.
 *
 * 1. SCOPE: Enclosed in namespace `SevenWondersDuel` to avoid pollution of
 * global namespace.
 * 2. C++11 FEATURES: usage of `enum class` (Scoped Enums) prevents name
 * clashes.
 */

namespace SevenWondersDuel {

// 资源类型 (规则 1.6)
// [SOLID] Single Responsibility: This enum is ONLY responsible for listing
// Resource Types.
enum class ResourceType {
  WOOD,
  STONE,
  CLAY, // 棕色原料
  PAPER,
  GLASS // 灰色加工品
};

// 卡牌类型 (规则 1.6)
// Used by Factory Pattern (DataLoader) to construct cards.
enum class CardType {
  RAW_MATERIAL, // 棕色
  MANUFACTURED, // 灰色
  CIVILIAN,     // 蓝色
  SCIENTIFIC,   // 绿色
  COMMERCIAL,   // 黄色
  MILITARY,     // 红色
  GUILD,        // 紫色 (时代III专用)
  WONDER        // 奇迹
};

// 科学符号类型 (规则 1.7)
enum class ScienceSymbol {
  GLOBE,   // 地球仪
  TABLET,  // 石板
  MORTAR,  // 研钵(烧杯)
  COMPASS, // 圆规
  WHEEL,   // 齿轮
  QUILL,   // 羽毛笔
  LAW      // 法律 (发展标记提供)
};

/**
 * [DESIGN PATTERN] - State Pattern (Basis)
 * ----------------------------------------
 * While this is just an Enum, it represents the backbone of the State Machine
 * implemented in GameController. Instead of using boolean flags like
 * `isDrafting = true`, `isWaitingForDestroy = true`,
 * we define explicit states. This makes the game flow deterministic.
 */
enum class GameState {
  // --- 准备阶段 ---
  WONDER_DRAFT_PHASE_1, // 第一轮奇迹轮抽 (4张)
  WONDER_DRAFT_PHASE_2, // 第二轮奇迹轮抽 (4张)

  // --- 正常回合 ---
  AGE_PLAY_PHASE, // 玩家行动阶段 (建牌/卖牌/建奇迹)

  // --- 中断/交互状态 (Interrupts) ---
  // These states handle complex card effects that pause the normal game flow.
  WAITING_FOR_TOKEN_SELECTION_PAIR,   // 凑齐2个相同符号，从5个公开标记中选1
  WAITING_FOR_TOKEN_SELECTION_LIB,    // 亚历山大图书馆效应
  WAITING_FOR_DESTRUCTION,            // 宙斯/竞技场效应
  WAITING_FOR_DISCARD_BUILD,          // 摩索拉斯陵墓效应
  WAITING_FOR_START_PLAYER_SELECTION, // 时代交替

  // --- 结算 ---
  GAME_OVER
};

// 玩家行动指令类型
// Part of Command Pattern (Action struct)
enum class ActionType {
  DRAFT_WONDER,          // 轮抽奇迹
  BUILD_CARD,            // 建造卡牌 (支付资源/金币/连锁)
  DISCARD_FOR_COINS,     // 弃牌换钱 (规则: 2 + 黄卡数)
  BUILD_WONDER,          // 建造奇迹 (作为次级目标，需垫一张时代卡)
  SELECT_PROGRESS_TOKEN, // 选择发展标记
  SELECT_DESTRUCTION,    // 指定摧毁目标
  SELECT_FROM_DISCARD,   // 从弃牌堆捞牌
  CHOOSE_STARTING_PLAYER // 决定下一时代先手
};

// 发展标记 (规则 1.3)
enum class ProgressToken {
  NONE,
  AGRICULTURE,  // 农业
  URBANISM,     // 都市化
  STRATEGY,     // 战略
  THEOLOGY,     // 宗教
  ECONOMY,      // 经济
  MASONRY,      // 砖石
  ARCHITECTURE, // 建筑
  LAW,          // 法律
  MATHEMATICS,  // 数学
  PHILOSOPHY    // 哲学
};

// 胜利类型
enum class VictoryType { NONE, MILITARY, SCIENCE, CIVILIAN };

} // namespace SevenWondersDuel

#endif // SEVEN_WONDERS_DUEL_GLOBAL_H
