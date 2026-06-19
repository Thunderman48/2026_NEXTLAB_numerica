# Make 10 — Educational Game

A lightweight, offline math game for kids. Tap flying number bubbles to fill the empty slot and make **10** when added to the number already shown.

Built with **C** and **[raylib](https://www.raylib.com/)** for minimal size and low hardware requirements. No internet connection required.

## How to play

1. Press **PLAY** on the home screen.
2. The left slot shows a random number from **1 to 9**.
3. Tap a flying bubble to put its number in the right slot.
4. If the two numbers add up to **10**, you earn **10 points** and a new round starts.
5. If the sum is wrong, try again — the left number stays, the right slot clears.
6. Your best score is saved locally in `save.dat`.

## Requirements

- **CMake** 3.14+
- **C compiler** (MinGW-w64 or MSVC on Windows)
- **Git** (CMake downloads raylib automatically on first build)

### Minimum device specs

- ~512 MB RAM
- OpenGL 2.1 (desktop) or OpenGL ES 2.0 (Android)
- Mouse or touchscreen

## Build on Windows

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Run the game:

```bash
build\Release\make10.exe
```

Or, with a single-config generator (MinGW):

```bash
build\make10.exe
```

Copy `make10.exe` to a USB drive — no installer or internet needed.

## Build for Android (APK)

1. Install [Android NDK](https://developer.android.com/ndk) and the Android SDK.
2. Follow the [raylib Android guide](https://github.com/raysan5/raylib/wiki/Working-for-Android).
3. Cross-compile this project using the same `src/` files with the Android CMake toolchain.
4. Sign and sideload the APK. The game runs fully offline after install.

## Project structure

```
src/
  main.c       — app entry, home/playing state machine
  game.c       — slots, scoring, round logic
  bubble.c     — flying bubbles (movement, hit-test, draw)
  ui.c         — home screen, slots, score display
  save.c       — local high-score file
  constants.h  — colors, sizes, game constants
```

## License

2026 NEXTLAB club — educational use.
