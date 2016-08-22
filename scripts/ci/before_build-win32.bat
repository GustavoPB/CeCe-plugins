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

SET PF=Program Files (x86)
IF "%GENERATOR%" == "Visual Studio 14 2015 Win64" SET PF=Program Files

:: Shared configuration arguments
SET ARGS=^
    -DCMAKE_BUILD_TYPE=Release ^
    -DPNG_PNG_INCLUDE_DIR="C:\%PF%\libpng\include" ^
    -DPNG_LIBRARY_RELEASE="C:\%PF%\libpng\lib\libpng16_static.lib" ^
    -DZLIB_INCLUDE_DIR="C:\%PF%\zlib\include" ^
    -DZLIB_LIBRARY="C:\%PF%\zlib\lib\zlibstatic.lib"

md build
pushd build

:: Configure project
cmake -G "%GENERATOR%" %ARGS% .. || EXIT /B 1

popd

@echo off

:: ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: ::
