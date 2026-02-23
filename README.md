# cpp-particle-sim-
A simple interactive particle simulation written in C++20 using SFML and
a rudimentary test to engage with an learn multi threading.

## Controls
- **P**: spawn proton at mouse cursor
- **N**: spawn neutron at mouse cursor
- **E**: spawn electron at mouse cursor

## Build (vcpkg)
1. Install SFML via vcpkg:
   - `vcpkg install sfml`

2. Configure and build:
   - `cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake`
   - `cmake --build build -j`

3. Run:
   - `./build/particle_sim`

## License
MIT (c) 2026 Aschwyn
