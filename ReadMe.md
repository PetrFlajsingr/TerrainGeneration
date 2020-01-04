# Terrain generation

gcc-9 required (c++20)

Required libraries:
* `freeglut3-dev` - for text rendering
* `SDL2` - correct version for geGL required (2.0.9)
* `libfreetype6-dev` - for text rendering
* `libsndio-dev`
* `libdbus-1-dev`
* `libibus-1.0-dev`

To build the project:

Set `SDL2_DIR`.

`mkdir build && cd build`

`cmake ..`

Modify CMakeLists.txt of freetypeglpp to use system GLUT in case of a failure.

`cmake ..`

`make`

To run the project:

`./TerrainGeneration <path to config.json file>`
