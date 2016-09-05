
# CeCe plugins

[![Build Status](https://travis-ci.org/GeorgievLab/CeCe-plugins.svg?branch=master)](https://travis-ci.org/GeorgievLab/CeCe-plugins)
[![Build status](https://ci.appveyor.com/api/projects/status/nsh176yuvfyhvp3i/branch/master?svg=true)](https://ci.appveyor.com/project/NTSFka/cece-plugins/branch/master)

CeCe is a platform for advanced simulation of cell communication in dynamic environment.

This repository contains only plugins, for whole project see [https://github.com/GeorgievLab/CeCe](https://github.com/GeorgievLab/CeCe)

There are some plugins which are available as a part of the CeCe.

## Building from source

Building from source code is done by CMake (see Requirements).

### Linux & OSX

```bash
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
```

### Windows
```batch
md build
cd build
cmake -G "Visual Studio 14 2015" ..
cmake --build .
```

### Requirements

* C++11 compiler - GCC, Clang, MSVC.
* [CMake](https://cmake.org) v3.1 or newer.

### Optional Requirements

* [Python](http://www.python.org) v2.7
* [LibPNG](https://github.com/glennrp/libpng)
* [Zlib](https://github.com/madler/zlib)

## Third party libraries
Libraries that are part of source code.

* [pugixml](http://pugixml.org) (*xml* plugin).
* [nanosvg](https://github.com/memononen/nanosvg) (*obstacles-svg* plugin).
