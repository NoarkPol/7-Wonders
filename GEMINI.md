# Seven Wonders Duel 项目结构重构与规范化指南

## 1. 目标概述
将当前扁平的项目结构重构为标准的 C++ 工程结构，实现类声明（.h）与实现（.cpp）的严格分离，并统一全案代码风格。

## 2. 目录结构设计
请按以下结构重新组织文件：
- `include/SevenWondersDuel/`: 存放所有头文件 (.h)。
- `src/`: 存放所有源文件 (.cpp)。
- `data/`: 存放 `gamedata.json`。
- `root/`: 存放 `main.cpp` 和 `CMakeLists.txt`。

## 3. 重构准则
### A. 声明与实现分离
- **禁止内联实现**：除模板类或极简单的 Getter/Setter 外，所有函数体必须从 `.h` 移至 `.cpp`。
- **头文件保护**：统一使用 `#ifndef ... #define ... #endif` 宏保护，格式为 `SEVEN_WONDERS_DUEL_[FILENAME]_H`。
- **前向声明**：在头文件中尽可能使用前向声明（Forward Declaration）代替 `#include`，以减少编译依赖。

### B. 代码格式化规范
- **命名风格**：
  - 类名：`PascalCase` (如 `GameController`)
  - 函数名：`camelCase` (如 `validateAction`)
  - 变量名：`camelCase`，成员变量加 `m_` 前缀 (如 `m_currentPlayer`)
  - 常量：`kPascalCase` 或 `UPPER_SNAKE_CASE`
- **缩进与空格**：
  - 使用 4 个空格缩进。
  - 大括号遵循 K&R 或侧重清晰的换行风格。
  - 运算符两侧保留空格，函数参数逗号后保留空格。

### C. 依赖管理
- **包含路径**：所有内部头文件的包含方式统一改为 `#include "SevenWondersDuel/FileName.h"`。
- **CMake 更新**：重构 `CMakeLists.txt`，确保 `target_include_directories` 指向新的 `include/` 目录，并正确链接 `src/` 下的所有文件。

## 4. 执行任务清单 (优先级顺序)
1. **结构迁移**：创建目录并将文件归类。
2. **逻辑剥离**：扫描所有 `.h` 文件，将实现代码迁移到对应的 `.cpp`。
3. **格式化与修正**：应用命名规范，并修复所有因目录变更导致的 `#include` 错误。
4. **编译配置**：更新 `CMakeLists.txt` 并确保项目可正常构建。