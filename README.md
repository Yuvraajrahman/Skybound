# SkyBound

SkyBound is a cross-platform 2D platformer written in modern C++ and powered by [raylib](https://www.raylib.com/). It targets desktop (Windows/Linux) and Android using a single CMake-based build pipeline.

## Features

- Responsive player controller with jumping, gravity, and collision resolution.
- Moving and static platforms, patrolling enemies, and collectible coins.
- Basic HUD, pause, menu, and game-over screens.
- Camera tracking with dynamic zoom for extra motion flair.
- CMake project that fetches raylib automatically (or links against a system install).

## Repository Layout

```
SkyBound/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── src/
│   ├── main.cpp
│   ├── game.cpp/.h
│   ├── player.cpp/.h
│   ├── platform.cpp/.h
│   ├── enemy.cpp/.h
│   ├── coin.cpp/.h
│   ├── ui.cpp/.h
│   └── input.cpp/.h
├── assets/
│   ├── images/
│   ├── sounds/
│   └── fonts/
└── android/
    └── README.md (platform-specific notes)
```

## Prerequisites

| Platform | Requirements |
|----------|--------------|
| Windows  | Visual Studio 2019+ (Desktop C++ workload) or MSYS2/MinGW, CMake 3.16+, Git |
| Linux    | `build-essential` (or equivalent), CMake 3.16+, Git, X11/Mesa headers (see below) |
| Android  | Android Studio, Android NDK r25+, CMake + Ninja via the Android SDK Manager |

### Install raylib (optional)

By default, the project fetches raylib v5.0 at configure-time via CMake FetchContent. Disable that behavior and use a system install instead with:

```
cmake -S . -B build -DSKYBOUND_USE_SYSTEM_RAYLIB=ON
```

When `SKYBOUND_USE_SYSTEM_RAYLIB=ON`, ensure `raylib` is discoverable by CMake (`find_package(raylib 5.0 REQUIRED)`).

## Building (Desktop)

```bash
cmake -S . -B build
cmake --build build
```

Run the game from the build tree:

```bash
# Windows (PowerShell)
./build/Debug/SkyBound.exe

# Linux
./build/SkyBound
```

> **Note**: If your shell cannot find `cmake`, install it first or add it to `PATH` (Windows installer, MSYS2 `pacman -S cmake`, Ubuntu `sudo apt install cmake`, etc.).

### Linux dependencies

Ubuntu/Debian example:

```bash
sudo apt update
sudo apt install build-essential cmake git libasound2-dev mesa-utils \
    libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev
```

## Android Build

1. Install Android Studio, then add the CMake and NDK components via the SDK Manager.
2. Follow `android/README.md` to generate an Android Studio project using raylib's template.
3. Replace the template source with `src/` from this repository.
4. Build with `./gradlew assembleDebug` and install the resulting APK.

## Gameplay Controls

| Action | Desktop | Android (default template) |
|--------|---------|----------------------------|
| Move   | `A`/`D` or arrow keys | On-screen left/right buttons |
| Jump   | `Space`/`W`/`Up` | Tap center button |
| Pause  | `P` | Pause button |
| Restart| `R` | Pause menu button |

## Testing Checklist

- Verify player can traverse platforms, collect all coins, and reach new levels.
- Confirm collisions keep the player grounded without clipping.
- Ensure pause/resume/Game Over flows behave as expected.
- Target 60 FPS on desktop builds; profile using the built-in frame counter (`DrawFPS`).

## Next Steps

- Drop art/audio assets into the `assets/` subfolders and wire them up with `LoadTexture` / `LoadMusicStream`.
- Expand levels by adding more platform layouts or a level loader.
- Integrate save data or leaderboards for a polished release build.