# sphere-lighting

This program renders a sphere that reflects light using Phong illumination. The sphere's location and the direction of the light can be adjusted through user input.

Download the [latest release](https://github.com/Varulli/sphere-lighting/releases/latest) or [build it yourself](https://github.com/Varulli/sphere-lighting#build-instructions).

![sphere-lighting-demo](https://github.com/user-attachments/assets/33b25bb4-be36-4100-af6a-c2518d467b67)

## Build Instructions

### Requirements
- Windows 7+
- C compiler
- CMake 3.5+

### Steps
```
git clone https://github.com/Varulli/sphere-lighting.git
cd sphere-lighting
mkdir build
cd build
cmake ..
cmake --build .
```

## Controls

W/A/S/D: Move sphere up/left/down/right

Q/E: Move sphere away/towards viewport

I/J/K/L: Rotate light source up/left/down/right

0: Quit
