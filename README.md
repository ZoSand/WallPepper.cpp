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

#### DLLs

You may have to copy the DLLs listed below from

```
%USERPROFILE%/AppData/Local/JetBrains/Toolbox/apps/CLion/ch-1/{CLion version}/bin/mingw/bin.
```

to the build directory.

|         DLL         |      Description      |
|:-------------------:|:---------------------:|
| libgcc_s_seh-1.dll  |  GCC runtime library  |
|   libstdc++-6.dll   |  C++ runtime library  |
| libwinpthread-1.dll | POSIX threads library |

### Or install from [Releases](https://github.com/ZoSand/WallPepper.cpp/releases)

## Usage

### Script API

See [API.md](API.md)

### Run the program

#### Command line arguments

|   Argument    |       Description       |                       Argument                        |
|:-------------:|:-----------------------:|:-----------------------------------------------------:|
| -s, --script  | Path to the script file | if the script is not in the InstallDir/Scripts folder |
|  -h, --help   |        Show help        |                                                       |
| -v, --version |      Show version       |                                                       |
|  -d, --debug  |       Debug mode        |                                                       |
| -e, --engine  |    Engine selection*    |                 one of OpenGl/Vulkan                  |
| -c, --config  |   Custom config path    |            C:/Path/to/custom/config.cfg**             |

*Can be configured in the application settings
**See [Config.md](Config.md)