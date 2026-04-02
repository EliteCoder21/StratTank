# StratTank - Strategy Tank Game Specification

## Project Overview
- **Project Name**: StratTank
- **Type**: 2D Strategy Tank Game
- **Core Functionality**: Top-down tank game where player controls a main tank, commands ally tanks, and battles enemy formations
- **Target Users**: Casual gamers who enjoy strategy and action games

## Technology Stack
- C++ with SFML (Simple and Fast Multimedia Library)
- CMake for build system

## UI/UX Specification

### Layout Structure
- **Game Canvas**: Full window (1280x720 default)
- **HUD**: Top-left corner for player stats, bottom for command panel
- **Minimap**: Top-right corner (200x150px)

### Visual Design
- **Color Palette**:
  - Background/Ground: `#2d4a1c` (dark grass green)
  - Player Tank: `#4a7c23` (olive green)
  - Ally Tanks: `#3d6b1e` (darker green)
  - Light Tank Enemy: `#8b4513` (saddle brown)
  - Heavy Tank Enemy: `#5c2a2a` (dark red-brown)
  - Fort: `#4a4a4a` (gray) with `#2a2a2a` (darker gray) base
  - Player Projectiles: `#ffd700` (gold)
  - Enemy Projectiles: `#ff4500` (orange-red)
  - UI Background: `#1a1a1a` (near black) with `#333333` (dark gray) borders
  - Text: `#ffffff` (white)

- **Typography**:
  - Primary Font: Default SFML font
  - HUD Text Size: 16px
  - Title Size: 24px

- **Visual Effects**:
  - Tank tracks leave temporary trail
  - Muzzle flash on shooting
  - Explosion particles on death
  - Selection circle around selected units

### Components
- **Tank Sprites**: Simple geometric shapes with turret indication
- **Fort**: Square structure with rotating turret
- **Projectiles**: Small circles with trail
- **Selection Ring**: Dashed circle animation
- **Command Indicator**: Arrow pointing to attack target

## Gameplay Specification

### Player Tank
- Movement: WASD keys
- Aim: Mouse position (turret rotates toward cursor)
- Shoot: Left mouse button
- Speed: 150 units/sec
- Health: 100 HP
- Fire Rate: 2 shots/sec

### Ally Tanks
- Types: 2 light tanks, 1 heavy tank
- Movement: Follow player or move to waypoint
- Attack: Player right-clicks enemy to command ally to attack
- Commands:
  - Right-click on ground: Move to position
  - Right-click on enemy: Attack that enemy
  - Hold Shift + right-click: All allies follow/move
- AI: Auto-attack nearest enemy if no command given

### Enemy Types

#### Light Tank
- Health: 30 HP
- Speed: 120 units/sec
- Damage: 10 per shot
- Fire Rate: 1.5 shots/sec
- Behavior: Fast, flanking movements
- Color: Light brown

#### Heavy Tank
- Health: 80 HP
- Speed: 60 units/sec
- Damage: 20 per shot
- Fire Rate: 0.8 shots/sec
- Behavior: Slow, aggressive, prioritizes closest target
- Color: Dark red-brown

#### Fort
- Health: 200 HP
- Speed: 0 (stationary)
- Damage: 25 per shot
- Fire Rate: 0.5 shots/sec
- Behavior: Stationary, rotates turret to nearest enemy
- Color: Gray with visible turret

### Wave System
- Wave 1: 3 light tanks
- Wave 2: 2 light tanks, 1 heavy tank
- Wave 3: 4 light tanks, 2 heavy tanks
- Wave 4: 2 forts, 3 light tanks, 2 heavy tanks
- Waves continue with increasing difficulty

### Controls
- `W/A/S/D`: Move player tank
- `Mouse Move`: Aim turret
- `Left Click`: Shoot
- `Right Click`: Command ally (when ally selected) or select ally
- `1/2/3`: Select ally tank 1/2/3
- `Space`: Select all allies
- `Tab`: Cycle through allies
- `Escape`: Pause menu

### Game States
1. **Playing**: Active gameplay
2. **Paused**: Game paused, shows menu
3. **Victory**: All waves cleared
4. **Defeat**: Player tank destroyed

## Technical Architecture

### Classes
- `Game`: Main game loop, state management
- `Entity`: Base class for all game objects
- `Tank`: Base class for tanks (player, ally, enemy)
- `PlayerTank`: User-controlled tank
- `AllyTank`: AI-controlled friendly tank
- `EnemyTank`: AI-controlled enemy (base for light/heavy)
- `Fort`: Stationary enemy structure
- `Projectile`: Bullet object
- `ParticleSystem`: Visual effects
- `HUD`: User interface rendering
- `WaveManager`: Enemy spawn management

## Acceptance Criteria
1. Player can move tank with WASD and aim with mouse
2. Player can shoot with left click
3. Ally tanks follow commands (move/attack)
4. All three enemy types spawn and behave differently
5. Wave system progresses through multiple waves
6. HUD displays health, score, wave number
7. Game ends on victory (all waves) or defeat (player death)
8. Visual feedback for shots, hits, and deaths
9. At least 30 FPS performance