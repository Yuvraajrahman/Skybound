# Android Build Notes

The desktop CMake project in this repository focuses on Windows and Linux. For Android, reuse raylib's official Gradle template and swap in the SkyBound sources.

## Quick Start

1. Install Android Studio and ensure the **NDK (r25 or newer)**, **CMake**, and **LLDB** are added via the SDK Manager.
2. Clone raylib with submodules (needed for the template assets):

   ```bash
   git clone --recursive https://github.com/raysan5/raylib.git
   ```

3. Open `raylib/projects/Android/raylib_android` in Android Studio.
4. Copy the `src/` directory from this SkyBound repo into the template's `app/src/main/cpp/` folder, replacing the example files.
5. Update `app/src/main/cpp/CMakeLists.txt` so it lists the SkyBound sources (`game.cpp`, `player.cpp`, etc.) and links against the bundled raylib library.
6. Sync Gradle and run `./gradlew assembleDebug` (or use **Run** inside Android Studio) to build and deploy.

## Input Overlay

The `PollInputState()` implementation includes simple gesture handling. Extend it with raylib's `Gesture` APIs or hook up UI buttons inside the Android template for more precise touch controls.

## Assets

Place Android-ready textures, sounds, and fonts under `app/src/main/assets/` and load them with raylib's `LoadTexture` / `LoadMusicStream` functions at runtime.


