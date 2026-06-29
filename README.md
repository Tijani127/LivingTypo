# Living Typo

A particle-based living typography sandbox. Type text — particles spring into place to form the letters. Move your mouse through them to scatter them.

Built with C++20 and SFML 2.6.

## How it works

Text is rendered to an offscreen texture. Every bright pixel becomes a particle with a spring force pulling it toward its screen position. The mouse adds a repulsion force. The result is organic, fluid text that responds to touch.

## Controls

| Key | Action |
|-----|--------|
| Type | Add characters |
| Backspace | Delete last character |
| R | Scatter particles randomly |
| Escape | Quit |

## Build

Requires CMake 3.20+, Ninja, a C++20 compiler.

```bash
cmake -B build -G Ninja -DCMAKE_CXX_COMPILER=g++
cmake --build build
./build/LivingTypo.exe
```

SFML is fetched automatically via CMake FetchContent.

