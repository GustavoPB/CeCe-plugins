:: ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: ::
:: Georgiev Lab (c) 2015-2016                                                ::
:: ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: ::
:: Department of Cybernetics                                                 ::
:: Faculty of Applied Sciences                                               ::
:: University of West Bohemia in Pilsen                                      ::
:: ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: ::
::                                                                           ::
:: This file is part of CeCe.                                                ::
::                                                                           ::
:: CeCe is free software: you can redistribute it and/or modify              ::
:: it under the terms of the GNU General Public License as published by      ::
:: the Free Software Foundation, either version 3 of the License, or         ::
:: (at your option) any later version.                                       ::
::                                                                           ::
:: CeCe is distributed in the hope that it will be useful,                   ::
:: but WITHOUT ANY WARRANTY; without even the implied warranty of            ::
:: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             ::
:: GNU General Public License for more details.                              ::
::                                                                           ::
:: You should have received a copy of the GNU General Public License         ::
:: along with CeCe.  If not, see <http://www.gnu.org/licenses/>.             ::
::                                                                           ::
:: ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: ::

@echo on

md vendor
pushd vendor

:: Clone dependencies
git clone https://github.com/madler/zlib.git -b v1.2.8 zlib
git clone https://github.com/glennrp/libpng -b libpng16 libpng

:: Set MinGW path
SET PATH=%PATH:C:\Program Files\Git\usr\bin;=%
SET PATH=C:\MinGW\bin;%PATH%

::::::::::::::::

:: Zlib
pushd zlib

md build
pushd build

:: Configure
cmake -G "%GENERATOR%" ^
    -DCMAKE_BUILD_TYPE=Release ^
    .. ^
    || EXIT /B 1

:: Build
cmake --build . --config Release || EXIT /B 1

:: Install
cmake --build . --target install --config Release || EXIT /B 1

popd
popd

::::::::::::::::

:: LibPNG
pushd libpng

md build
pushd build

SET PF=Program Files (x86)
IF "%GENERATOR%" == "Visual Studio 14 2015 Win64" SET PF=Program Files

:: Configure
cmake -G "%GENERATOR%" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DPNG_SHARED=Off ^
    -DZLIB_INCLUDE_DIR="C:\%PF%\zlib\include" ^
    -DZLIB_LIBRARY="C:\%PF%\zlib\lib\zlibstatic.lib" ^
    .. ^
    || EXIT /B 1

:: Build
cmake --build . --config Release || EXIT /B 1

:: Install
cmake --build . --target install --config Release || EXIT /B 1

popd
popd

:: vendor
popd

@echo off

:: ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: ::

