
# CeCe plugins

CeCe is a platform for advanced simulation of cell communication in dynamic environment.

This repository contains only plugins, for whole project see [https://github.com/GeorgievLab/CeCe](https://github.com/GeorgievLab/CeCe)

There are some plugins which are available as a part of the CeCe.

* [agglutination](agglutination/doc/manual.md)
* [background](background/doc/manual.md)
* [cell](cell/doc/manual.md)
* [cell-python](cell-python/doc/manual.md)
* [diffusion](diffusion/doc/manual.md)
* [diffusion-python](diffusion-python/doc/manual.md)
* [diffusion-streamlines](diffusion-streamlines/doc/manual.md)
* [chemostat](chemostat/doc/manual.md)
* [object-generator](object-generator/doc/manual.md)
* [obstacle](obstacle/doc/manual.md)
* [obstacles-svg](obstacles-svg/doc/manual.md)
* [picture](picture/doc/manual.md)
* [python](python/doc/manual.md)
* [stochastic-reactions](stochastic-reactions/doc/manual.md)
* [streamlines](streamlines/doc/manual.md)
* [streamlines-channel](streamlines-channel/doc/manual.md)
* [streamlines-python](streamlines-python/doc/manual.md)
* [xml](xml/doc/manual.md)

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
* [CMake](https://cmake.org) v3.0 or newer.
* [Boost C++ Libraries](http://www.boost.org) - Filesystem, System, Any, Tribool and Variant.
* [Python](http://www.python.org) v2.7
* [LibPNG](https://github.com/glennrp/libpng)
* [Zlib](https://github.com/madler/zlib)