# FourX economy simulator

With this project I tried to find out how difficult it actually was to create a real-time economy simulator. The eventual goal of FourX is to simulate all kinds of relations (i.e factions, tax, police).
The graphics are currently lacking a bit, but I believe that a simple star shader will greatly improve it. Currently I'm using SDL and C++ and apparently standalone SDL doesn't support shaders, so I'll eventually have to use the OpenGL variant of SDL.

## Installation

1. Clone the repo
```bash 
git clone https://github.com/svrem/4x
```
2. Install the SDL dependencies

```bash
# <3 https://gist.github.com/aaangeletakis/3187339a99f7786c25075d4d9c80fad5
#install sdl2
sudo apt install libsdl2-dev libsdl2-2.0-0 -y;

#install sdl image 
sudo apt install libjpeg-dev libwebp-dev libtiff5-dev libsdl2-image-dev libsdl2-image-2.0-0 -y;

#install sdl ttf
sudo apt install libfreetype6-dev libsdl2-ttf-dev libsdl2-ttf-2.0-0 -y;
```

3. Run the CMake script
```bash
cmake .
cmake --build .
```
