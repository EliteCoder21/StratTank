# StratTank - 2D Strategy Tank Game

<div align="center">
  <img src="docs/screenshots/gameplay.png" alt="StratTank Gameplay" width="800"/>
  <p><em>A top-down strategy tank game where you command your forces against enemy waves</em></p>
</div>

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Building](#building)
- [Gameplay](#gameplay)
  - [Controls](#controls)
  - [Game Entities](#game-entities)
  - [Wave System](#wave-system)
- [Screenshots](#screenshots)
- [Architecture](#architecture)
  - [Class Hierarchy](#class-hierarchy)
  - [C++ Concepts](#c-concepts-used)
  - [Design Patterns](#design-patterns)
- [License](#license)

---

## Overview

**StratTank** is a 2D top-down strategy tank game built in C++ using the SFML (Simple and Fast Multimedia Library). Command your tank alongside allied units to battle increasingly difficult waves of enemies through tactical positioning, strategic targeting, and coordinated attacks.

**Author:** Aaryan Pawar  
**License:** MIT License  
**Build System:** CMake (C++17)  
**Dependencies:** SFML 3.0

---

## Features

- **Player Tank Control** - WASD movement with mouse aiming and shooting
- **Ally Commands** - Issue move/attack orders to allied tanks
- **Multiple Enemy Types** - Light tanks, heavy tanks, and fortified positions
- **Wave-Based Progression** - 7+ waves with escalating difficulty
- **Strategic Gameplay** - Barriers, positioning, and tactical combat
- **Visual Effects** - Particle systems for explosions, muzzle flashes, and smoke
- **HUD System** - Real-time stats, wave information, and command feedback
- **Minimap** - Track all units on the battlefield

---

## Building

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler (GCC, Clang, MSVC)
- SFML 3.0 (Graphics, Audio, Window, System modules)

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/yourusername/StratTank.git
cd StratTank

# Create build directory
mkdir build
cd build

# Generate build files
cmake ..

# Build the project
cmake --build . --config Release

# Run the game
./StratTank        # Linux/macOS
StratTank.exe      # Windows
```

### Dependencies (Windows)

If running on Windows, ensure the following DLLs are in the same directory as the executable:

```
libsfml-graphics-3.dll
libsfml-window-3.dll
libsfml-system-3.dll
libsfml-audio-3.dll
libsfml-network-3.dll
libgcc_s_seh-1.dll
libstdc++-6.dll
libwinpthread-1.dll
```

---

## Gameplay

### Controls

| Key | Action |
|-----|--------|
| `W` `A` `S` `D` | Move player tank |
| `Mouse Move` | Aim turret |
| `Left Click` | Shoot |
| `Right Click` | Command selected ally / Select ally |
| `1` `2` `3` | Select ally tank 1, 2, or 3 |
| `Space` | Select all allies |
| `Tab` | Cycle through allies |
| `Shift + Right Click` | Command all allies |
| `Escape` | Pause game |

### Game Entities

#### Player Tank
- **Health:** 100 HP
- **Speed:** 150 units/sec
- **Damage:** 15 per shot
- **Fire Rate:** 3.0 shots/sec
- **Color:** Olive green (`#4a7c23`)

#### Ally Tanks
- **Health:** 60 HP
- **Speed:** 120 units/sec
- **Damage:** 12 per shot
- **Fire Rate:** 3.0 shots/sec
- **Color:** Darker green (`#3d6b1e`)

#### Enemy Types

| Type | Health | Speed | Damage | Fire Rate | Color |
|------|--------|-------|--------|-----------|-------|
| **Light Tank** | 30 HP | 120 | 10 | 2.5/sec | Saddle brown (`#8b4513`) |
| **Heavy Tank** | 80 HP | 60 | 20 | 1.5/sec | Dark red-brown (`#5c2a2a`) |
| **Fort** | 200 HP | 0 | 25 | 1.5/sec | Gray (`#4a4a4a`) |

#### Special Elements

- **Health Pickups (Hearts):** Spawn every 10 seconds, heal 25 HP
- **Fort Bombs:** 200px radius, 500 damage, 10-second cooldown

### Wave System

| Wave | Light Tanks | Heavy Tanks | Forts |
|------|-------------|-------------|-------|
| 1 | 3 | 0 | 0 |
| 2 | 2 | 1 | 0 |
| 3 | 4 | 2 | 0 |
| 4 | 3 | 2 | 2 |
| 5 | 5 | 3 | 2 |
| 6 | 6 | 4 | 3 |
| 7 | 8 | 5 | 4 |

---

## Screenshots

<div align="center">

### Main Gameplay
![Gameplay](docs/screenshots/gameplay.png)
*The battlefield with player tank, allies, and incoming enemies*

### Combat
![Combat](docs/screenshots/combat.png)
*Strategic combat against enemy tanks and forts*

### HUD Display
![HUD](docs/screenshots/hud.png)
*In-game HUD showing health, wave number, score, and enemy base status*

### Victory Screen
![Victory](docs/screenshots/victory.png)
*Victory screen after clearing all waves*

### Defeat Screen
![Defeat](docs/screenshots/defeat.png)
*Game over screen when the player is defeated*

</div>

---

## Architecture

### Class Hierarchy

```
Entity (Abstract Base)
├── Tank (Abstract)
│   ├── PlayerTank
│   ├── AllyTank
│   └── EnemyTank
├── Fort
├── Projectile
└── Heart
```

### C++ Concepts Used

#### Object-Oriented Programming

- **Inheritance:** Base `Entity` class with derived game objects (Tank, Fort, Projectile, Heart)
- **Polymorphism:** Virtual methods for `update()` and `render()` (Entity.h:30-31)
- **Encapsulation:** Private members with public interfaces throughout

#### Smart Pointers (Game.h:42-49)

```cpp
std::unique_ptr<PlayerTank> player;
std::vector<std::unique_ptr<AllyTank>> allies;
std::vector<std::unique_ptr<EnemyTank>> enemies;
std::vector<std::unique_ptr<Fort>> forts;
std::list<std::unique_ptr<Projectile>> projectiles;
std::vector<std::unique_ptr<Heart>> hearts;
```

#### Function Objects / Callbacks (Tank.h:35-37)

```cpp
std::function<void(float, float, float, int, bool)> projectileCallback;
std::function<void(float, float, float)> bombCallback;
```

#### Enums (Entity.h:8-23)

```cpp
enum class GameState { Playing, Paused, Victory, Defeat };
enum class EntityType { Player, Ally, EnemyLight, EnemyHeavy, Fort, ... };
```

#### World Configuration

The world bounds are dynamically set based on the actual window size using `WorldConfig`:
- `WorldConfig::WIDTH` - World width (set from window size)
- `WorldConfig::HEIGHT` - World height (set from window size)
- `WorldConfig::MARGIN` - Boundary margin (20.0f)

#### Standard Library Containers

- `std::vector` - Dynamic arrays for entities, barriers, waves
- `std::list` - Linked list for projectiles (efficient insertion/removal)
- `std::unique_ptr` - Automatic memory management (RAII)

### Design Patterns

#### Game Loop Pattern (Game.cpp)

```cpp
void Game::run() {
    while (window.isOpen()) {
        processEvents();
        if (state == GameState::Playing) {
            update(deltaTime);
        }
        render();
    }
}
```

#### Observer/Callback Pattern

Function callbacks for projectile spawning and bomb attacks registered through constructors.

#### Factory Pattern

Wave spawning via `WaveManager` creates enemy types dynamically.

#### RAII (Resource Acquisition Is Initialization)

Smart pointers and font loading in `HUD.cpp` ensure proper resource cleanup.

### Key Implementation Files

| File | Purpose |
|------|---------|
| `Game.cpp` | Main game loop, state management, collision detection |
| `Tank.cpp` | Base tank movement, steering, obstacle avoidance |
| `AllyTank.cpp` | Ally AI behavior |
| `EnemyTank.cpp` | Enemy AI, flanking, fleeing |
| `Fort.cpp` | Fort bomb system, turret AI |
| `WaveManager.cpp` | Wave spawning logic |
| `ParticleSystem.cpp` | Visual effects |
| `HUD.cpp` | User interface rendering |
| `WorldConfig.cpp` | Dynamic world bounds configuration |

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

<div align="center">
  <p>Built with C++17 and SFML</p>
</div>
