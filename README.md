# GP2-shooter: A High-Performance 2D Shooter Game

## Overview
GP2-shooter is a 2D shooter game developed with a focus on low-level system interactions and efficient gameplay mechanics. Built directly on the SDL2 framework, this project demonstrates proficiency in fundamental game development concepts, custom physics implementations, and cross-platform build processes.

## Key Technical Highlights
*   **Low-Level Graphics & Input**: Direct utilization of **SDL2** for rendering, event handling, and audio management, showcasing a deep understanding of the core functionalities of a minimal game development framework.
*   **High-Performance Math & Physics**: Implementation of custom mathematical utilities, including a **Vector2** class, for precise entity positioning and movement. Advanced collision detection (e.g., OBB - Oriented Bounding Box, Circle-based collisions) is integrated, alongside foundational physics principles such as gravity, forces, and velocity management.
*   **Cross-Platform Development**: Engineered for compatibility across multiple operating systems, specifically **Linux**, **macOS**, and **Windows**, managed through a robust **Makefile** system.
*   **Modular Architecture**: The game features a component-based design, separating concerns into distinct modules like `AnimatedSprite`, `BasicAI`, `BuffItem`, `Bullet`, `Character`, `HitBox`, `InputHandler`, and `Obstacle` for maintainability and scalability.

## Architecture
The project's architecture, particularly the relationships between core components, is illustrated in the following class diagram:

```mermaid
classDiagram
    ICollidable <|.. Entity
    ICollidable <|.. Obstacle
    IUpdatable <|.. Entity
    IUpdatable <|.. Buff
    Buff <|-- CharBuff
    Buff <|-- BulletBuff
	HitBox <|-- Rect
	HitBox <|-- Circle
    Entity <|-- Bullet
    Entity <|-- Character
    Obstacle <|-- Wall
    Obstacle <|-- BlackHole
    InputHandler o-- Character
    BasicAI o-- Character

    class IUpdatable {
        <<interface>>
        + update(..) void*
    }
    class ICollidable {
        <<interface>>
        + collide(object: Rect) void*
    }
    class Buff {
	    <<abstract>>
	    - _duration: float
	    - _timer: float
	    - _activated: boolean
		+ update(delta_time: float) void
		+ activate() void*
		+ is_activated() boolean
		+ deactivate() void*
    }
    class CharBuff {
	    - _type: Enum[CharBuffType]
	    - _char: Character
		+ activate() void
		+ deactivate() void
    }
    class BulletBuff {
	    - _type: Enum[BulletType]
	    - _char: Character
		+ activate() void
		+ deactivate() void
    }
    class HitBox {
	    <<abstract>>
	    + is_collide(hitbox: HitBox) boolean*
    }
    class Circle {
	    - _radius: float
	    - _local_pos: Vector2
	    + is_collide(hitbox: HitBox) boolean
    }
    class Rect {
	    - _rect: SDL_Rect
	    - _local_pos: Vector2
	    + is_collide(hitbox: HitBox) boolean
    }
    class Entity {
	    # _position: Vector2
		# _sprite: Texture
		# _speed: float
		# _force: Vector2
	    # _hitbox_list: Vector~HitBox~*
        + get_position() Vector2
        + get_sprite() Texture
		+ collide(object: ICollidable) void*
    }
    class Obstacle {
		<<abstract>>
        # _hitbox_list: Vector~HitBox~*
        # _position: Vector2
        # _sprite: Texture
        + get_position() Vector2
        + get_sprite() Texture
        + collide(object: Rect) void*
    }
    class InputHandler {
	    - _activated_char: Character
		- _char_list: Vector~Char~
	    + handle_event(event) void
    }
    class BasicAI {
		- _ai_char: Character
	      ...
    }
    class Character {
        - _health: float
	    - _gun_sprite: Texture
	    - _activated: boolean
        - _gun_type: Enum[GunType]
	    - _buff_list: Vector~CharBuff~
        - _gun_buffed: Enum[BulletBuffType]
        + get_position() Vector2
        + get_sprite() Texture
        + get_collision() Rect
	    + shoot(...) void
	    + move(dx, dy) void
	    + update(...) void
        + collide(object: Rect) void
		+ add_buff(buff: CharBuff, buff_type) void
		+ remove_buff(buff: CharBuff, buff_type) void
		+ change_bullet_buff(bullet_buff: BulletBuff, buff_type) void
    }
	class Bullet {
        - _damage: float
		- _directiom: Vector2
		- _buffed: Enum[BulletBuffType]
        + get_position() Vector2
        + get_sprite() Texture
        + get_collision() Rect
		+ update(...) void
        + collide(object: Rect) void
	}
    class Wall {
        + collide(object: Rect) void
    }
    class BlackHole {
        + collide(object: Rect) void
    }
```

## Building and Running

### Requirements

To build and run this project, you will need:
*   A **C++17 compatible compiler** (e.g., GCC, Clang).
*   The **`make` build tool**.

Additionally, you need the **SDL2 development libraries**: **SDL2**, **SDL2_image**, and **SDL2_ttf**.
Here's how to set them up for your operating system:

#### Linux (Debian/Ubuntu-based)
```bash
sudo apt-get update
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
```

#### macOS (using Homebrew)
```bash
brew install sdl2 sdl2_image sdl2_ttf
```

#### Windows
For Windows, the necessary SDL2 libraries are conveniently included within the `win-deps` directory of this project. You do *not* need to install them separately.

However, to compile and run the project:
*   Install **MinGW-w64** (or another GCC-compatible compiler suite).
*   Install **Git for Windows**, which includes **Git Bash** and the `make` utility.
*   **Recommendation**: Perform all `make` commands within a **Git Bash** or **MSYS2 terminal** to ensure proper execution.

### Building the Project
The project utilizes a `Makefile` for streamlined compilation across different platforms.

```bash
make all
```

### Running the Game
After a successful build, you can run the game:

```bash
make run
```
On Windows, this command will also ensure that required DLLs are copied to the executable directory before launching the game.
