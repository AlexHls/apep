# Apep

[![forthebadge](https://forthebadge.com/images/badges/60-percent-of-the-time-works-every-time.svg)](https://forthebadge.com)
[![forthebadge](https://forthebadge.com/images/badges/works-on-my-machine.svg)](https://forthebadge.com)

C++ learning project. A simple 2D hydrodynamics simulation.
Includes a simple window creation using GLFW and OpenGL.
Allows to change simulation parameters in real time.

## Dependencies

- GLFW 3.3 (https://www.glfw.org/), install via package manager or build from source
- OpenGL 3.3 (https://www.opengl.org/), should be installed by default

## Build

```bash
git clone https://github.com/alexhls/apep.git
cd apep
git submodule update --init --recursive

mkdir build
cd build
cmake ..
make
```

## Run

```bash
./apep
```

for the 2D hydrodynamics simulation or

```bash
./demo
```

for the demo window

## WIP

This project is still a work in progress. Most edge cases are not handled and the code is not optimized.

## Directory sturcture

src/ - source code

- app/ - Everything related to the window creation
- hydro/ - Everything related to the hydrodynamics simulation
- utils/ - Utility functions