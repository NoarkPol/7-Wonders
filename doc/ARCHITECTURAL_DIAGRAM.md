---
config:
  layout: elk
  class:
    hideEmptyMembersBox: true
---
classDiagram
direction TB
class Legend {
Entities(核心实体) : Blue
Rules(游戏逻辑) : Green
Control(流程控制) : Orange
Factory(数据构建) : Purple
View(视图交互) : Red
}

    class GameModel {
    }

    class Player {
    }

    class Board {
    }

    class MilitaryTrack {
    }

    class CardPyramid {
    }

    class CardSlot {
    }

    class Card {
    }

    class Wonder {
    }

    class Action {
    }

    class ResourceCost {
    }

    class RulesEngine {
    }

    class ScoringManager {
    }

    class IEffect {
    }

    class EffectFactory {
    }

    class IGuildStrategy {
    }

    class GameController {
    }

    class IGameActions {
    }

    class IGameStateLogic {
    }

    class IGameCommand {
    }

    class CommandFactory {
    }

    class IPlayerAgent {
    }

    class IGameFactory {
    }

    class BaseGameFactory {
    }

    class CardBuilder {
    }

    class GameView {
    }

    class InputManager {
    }

    class RenderContext {
    }

    class ILogger {
    }

    GameModel "1" *-- "2" Player : contains
    GameModel "1" *-- "1" Board : contains
    GameModel *-- Wonder : owns
    Player o-- Card : owns
    Player o-- Wonder : owns
    Board *-- MilitaryTrack : contains
    Board *-- CardPyramid : contains
    CardPyramid *-- CardSlot : contains
    CardSlot ..> Card : references
    Card *-- ResourceCost : has
    Card o-- IEffect : has effects
    Wonder o-- IEffect : has effects
    Wonder *-- ResourceCost : has
    BaseGameFactory --|> IGameFactory : implements
    BaseGameFactory ..> CardBuilder : uses
    BaseGameFactory ..> Card : creates
    BaseGameFactory ..> Wonder : creates
    GameController --|> IGameActions : implements
    GameController --|> ILogger : implements
    GameController *-- GameModel : owns
    GameController *-- IGameStateLogic : current state
    GameController ..> Action : consumes
    IGameStateLogic ..> GameController : validates actions for
    IPlayerAgent ..> GameController : observes
    IPlayerAgent ..> Action : returns
    IPlayerAgent ..> GameView : uses
    IPlayerAgent ..> InputManager : uses
    InputManager ..> Action : produces
    InputManager *-- RenderContext : owns
    InputManager ..> GameView : uses
    CommandFactory ..> Action : consumes
    CommandFactory ..> IGameCommand : creates
    IGameCommand ..> GameController : executes
    IEffect ..> IGameActions : modifies state via
    IEffect ..> ILogger : logs via
    EffectFactory ..> IEffect : creates
    GameController ..> ScoringManager : uses
    GameController ..> RulesEngine : uses
    GameController ..> CommandFactory : uses to create commands
    ScoringManager ..> Player : reads
    ScoringManager ..> Board : reads
    RulesEngine ..> Player : reads
    RulesEngine ..> Board : reads
    IGuildStrategy ..> Player : reads
    GameView ..> RenderContext : uses
    GameView ..> GameModel : reads

	style Legend fill:#fff,stroke:#333,stroke-dasharray: 5 5

	class GameModel:::Entities
	class Player:::Entities
	class Board:::Entities
	class MilitaryTrack:::Entities
	class CardPyramid:::Entities
	class CardSlot:::Entities
	class Card:::Entities
	class Wonder:::Entities
	class Action:::Entities
	class ResourceCost:::Entities
	class RulesEngine:::Rules
	class ScoringManager:::Rules
	class IEffect:::Rules
	class EffectFactory:::Rules
	class IGuildStrategy:::Rules
	class GameController:::Control
	class IGameActions:::Control
	class IGameStateLogic:::Control
	class IGameCommand:::Control
	class CommandFactory:::Control
	class IPlayerAgent:::Control
	class IGameFactory:::Factory
	class BaseGameFactory:::Factory
	class CardBuilder:::Factory
	class GameView:::View
	class InputManager:::View
	class RenderContext:::View
	class ILogger:::View

	classDef Entities :,fill:#e3f2fd,stroke:#1565c0,stroke-width:1px
	classDef Rules :,fill:#e8f5e9,stroke:#2e7d32,stroke-width:1px
	classDef Control :,fill:#fff3e0,stroke:#ef6c00,stroke-width:1px
	classDef Factory :,fill:#f3e5f5,stroke:#7b1fa2,stroke-width:1px
	classDef View :,fill:#ffebee,stroke:#c62828,stroke-width:1px