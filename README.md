# Ants & Islands

Ants & Islands is an RPG exploration game, that is built with C++, CMake, SDL2 and Catch2.

![alt text for screen readers](/assets/mainmenu_background.png "Text to show on mouseover")

## Controls

| Action                                                                                          | Key(s)     |
|-------------------------------------------------------------------------------------------------|------------|
| Movement / Navigating menus                                                                     | W, A, S, D |
| Toggle in-game menu                                                                             | ESCAPE     |
| Select menu item / Advance text <br/> Interact / Collect item <br/> Consume item from inventory | ENTER      |


## How to build
The build will be built in a separate build folder in the root directory. The binaries will be put into ```build/bin```.

The basic requirements to build this project are:

- CMake 3.14+
- A C++20 compatible compiler
- Git

The main CMake targets are:
- ```Ants_And_Islands```: runs the main application
- ```testing_app```: runs all test suites
- ```benchmark_ecs```: runs the ECS benchmarks

### Windows
The easiest way to build this project on Windows is using Visual Studio with the C++ desktop development tools (available via the Visual Installer). 
When building with Visual Studio, the build folder will be in ```root/out/build```. The necessary steps include:

1. Install Visual Studio (with C++ desktop development tools)
2. Install git
3. Import project, let CMake scan and configure the project
4. Build the executable(s)

No further steps should be required.
 
*This has been tested on a fresh Windows 11 setup using Visual Studio 2022.*

### Ubuntu
The requirements on Ubuntu (and Linux in general) are a bit more involved.

1. ```sudo apt-get update```
2. ```sudo apt-get install g++ cmake git libfreetype6-dev libopusfile-dev libsndfile1-dev libopenmpt-modplug-dev libfluidsynth-dev```
3. Navigate to the root of the project folder
4. Follow the instructions under **Executing CMake manually**.

*This has been tested on a fresh Ubuntu 22.04 setup.*

### Executing CMake manually
To configure (has to be called twice on some systems):

```bash
cmake -S . -B build
```

Add `-GNinja` if you have Ninja.

To build all targets:

```bash
cmake --build build
```

To build specific target::

```bash
cmake --build build --target Ants_And_Islands
cmake --build build --target testing_app
cmake --build build --target benchmark_ecs
```

To use an IDE, such as Xcode:

```bash
cmake -S . -B xbuild -GXcode
cmake --open xbuild
```

## Work distribution
#### Raphael Mayer
- project setup & CMake
- /engine
- /ecs
- Game.hpp (excluding tilemap code and viewport code)
- GameStateManager.hpp
- SaveGameManager.hpp
- MenuStack.hpp and UI elements (/ui)
- InputSystem.hpp (handle movement)
- PhysicsSystem.hpp
- RenderSystem.hpp (excluding animation code)
- all tests for my code

#### Joshua Meyer
- /assets (designing map, initial game state, etc.)
- AudioSystem.hpp
- InteractionSystem.hpp
- ProgressSystem.hpp

## TODO
- Switch to new ECS version
	- Load ECS dynamically from Github with CMake [x]
	- Change `ECSManager` to `ECS` to switch to new version.
	- Remove old files.
- Pathfinding & Movement:
	- If `targetPosition` is blocked or unreachable we try to recalculate a path every iteration. We need to check, if `targetPosition` is reachable. If not, we could use a couple of strategies like finding the nearest reachable tile, resetting, etc.
- switch from TXT to TMX map files
- text drawing still leaks memory (see `engine.cpp`)
- need to free surface when loading a `Texture` in `Engine::loadTexture`
- need to adjust camera scrolling speed based on zoom level
- need to adjust debug system to work with floats. This requires some new float versions of existing engine draw methods. 
- Configure CMake SDL imports differently, so we can update the SDL2 version.

### Features 
- implement actual AI state machine
- implement a new, free-moving camera with zoom
- create and switch to a new testing map 
- Sprint feature
- Stances? Crouch, Prone

### Low priority
- handle window resizing
- UI rendering needs to be adjusted
- we broke `ProgressSystem` because it relies on `interactable.isTextVisible`
- Change CMake config for assets. Currently it is kind of cumbersome (see `src/CMakeLists.txt`)
