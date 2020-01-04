# Terrain generation

Required libraries:
* `freeglut3-dev`
* `SDL2`


To build the project:

Set `SDL2_DIR`.

`mkdir build && cd build`

`cmake ..`

Modify CMakeLists.txt of freetypeglpp to use system GLUT after failure.

`cmake ..`

`make`

To run the project:

`./TerrainGeneration <path to config.json file>`