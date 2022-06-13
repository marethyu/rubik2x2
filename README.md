# Interactive Pocket Cube

Try the live demo [here](https://marethyu.github.io/rubik2x2)!

Built with C++ and SDL2 only. The handmade 3D software renderer used in this project can be found [here](https://github.com/marethyu/mgl).

## Build instructions

Make sure you have g++, GNU Make, SDL2, and Emscripten installed.

Run `Make exe` to create an executable program. Use `Make all` (`Make test` if you want to test) to generate a HTML file.

To test the generated HTML file locally, run `python -m http.server` and go to http://localhost:8000/index.html

## Controls

- Left mouse button + drag = rotate the whole cube
- Right mouse button + drag = rotate one of the cube layers
- s key = scramble the cube
