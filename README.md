# Bullseye
Simple 3D shooting range simulator written in C++ using OpenGL. Work in progress.
## Building
### Prerequisities for all platforms
* [SDL2](https://www.libsdl.org/index.php)
* [CMake](https://cmake.org/) (>= 3.13)
* GPU and drivers with support for at least OpenGL 4.3  

### Windows
Use CMake (min. 3.13) to generate Visual Studio solution. You might have to manually set `SDL2_PATH` to your local SDL2 development library path. SDL2.dll have to be copied to output build directory.  
Tested on Windows 10, CMake 3.19.6 and Visual Studio 2019 16.9.1.
### Linux 
Install SDL2. For Ubuntu-based distros:
```bash
sudo apt update
sudo apt install libsdl2-dev
```
Generate makefile and build project:
```bash
mkdir build/ && cd build/  
cmake ../ && make
```
Tested on Ubuntu 20.04, CMake 3.16.3 and gcc 9.3.0.
### MacOS 
*TODO*
### Post build (all platforms)
Copy assets to build directory (*TODO: Automate this step*):
```bash
cp -R assets build/
```