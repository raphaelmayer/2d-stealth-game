# Tactical Squad

Tactical Squad is an top down tactical shooter game, that is built with C++, CMake, SDL2 and Catch2.

![alt text for screen readers](/assets/mainmenu_background.png "Text to show on mouseover")

## Controls

| Action                                                                                          | Key(s)      |
|-------------------------------------------------------------------------------------------------|-------------|
| Selection				                                                                          | LEFT MOUSE  |
| Movement / Firing			                                                                      | RIGHT MOUSE |
| Navigating menus                                                                     		      | W, A, S, D  |
| Toggle in-game menu                                                                             | ESCAPE      |
| Select menu item / Advance text <br/> 							                              | ENTER       |


## How to build
The build will be built in a separate build folder in the root directory. The binaries will be put into ```build/bin```.

The basic requirements to build this project are:

- CMake 3.14+
- A C++20 compatible compiler
- Git

The main CMake targets are:
- ```Tactical_Squad```: runs the main application
- ```Tactical_Squad_Tests```: runs all test suites
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
cmake --build build --target Tactical_Squad
cmake --build build --target Tactical_Squad_Tests
cmake --build build --target benchmark_ecs
```

To use an IDE, such as Xcode:

```bash
cmake -S . -B xbuild -GXcode
cmake --open xbuild
```

## TODO
### Features 
- implement actual AI state machine
- Sprint feature
- Stances? Crouch, Prone
- design and implement a dedicated `AnimationSystem`
- switch to bounding box collision?
- implement player AI for engaging? I.e. move into range and LOS

### Low priority
- State machines: We need a generic state machine class to simplify a couple of areas:
	- Weapon firing state
	- AI alert state
	- ...
- Still using SDL specific types for mouse and keycodes.
- integrate ImGUI?

## Ideas for sound engine
- add SoundLibrary class to hold all sound chunks (in game context)
