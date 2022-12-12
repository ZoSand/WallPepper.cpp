# WallPepper.cpp

## Description

Wallpepper is a program that allows you to set a wallpaper from a user script (written in LUA), which offers the user a
wide range of possibilities.

## Installation

### Install from source

#### Dependencies

* CMake
* C++ 20 compiler (GCC 10.2.0 or higher)
* Lua 5.4.0 or higher
* Vulkan SDK
* OpenGL 4.5 or higher

I used CLion to build the project, but you should be able to use any other IDE or build it manually.

### Or install from [Releases](https://github.com/ZoSand/WallPepper.cpp/releases)

## Usage

### Create a script

#### Example script

```lua
```

#### Script API

See [API.md](API.md)

### Run the program

#### Command line arguments

|   Argument    |                            Description                            |
|:-------------:|:-----------------------------------------------------------------:|
| -s, --script  | Path to the script file (if not in the InstallDir/Scripts folder) |