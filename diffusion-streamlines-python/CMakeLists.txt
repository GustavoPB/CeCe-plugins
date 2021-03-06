# ######################################################################### #
# Georgiev Lab (c) 2015-2016                                                #
# ######################################################################### #
# Department of Cybernetics                                                 #
# Faculty of Applied Sciences                                               #
# University of West Bohemia in Pilsen                                      #
# ######################################################################### #
#                                                                           #
# This file is part of CeCe.                                                #
#                                                                           #
# CeCe is free software: you can redistribute it and/or modify              #
# it under the terms of the GNU General Public License as published by      #
# the Free Software Foundation, either version 3 of the License, or         #
# (at your option) any later version.                                       #
#                                                                           #
# CeCe is distributed in the hope that it will be useful,                   #
# but WITHOUT ANY WARRANTY; without even the implied warranty of            #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
# GNU General Public License for more details.                              #
#                                                                           #
# You should have received a copy of the GNU General Public License         #
# along with CeCe.  If not, see <http://www.gnu.org/licenses/>.             #
#                                                                           #
# ######################################################################### #

# Include Python API
find_package(PythonLibs 2.7)

if (NOT PYTHONLIBS_FOUND)
    message(WARNING "'diffusion-python' plugin requires Python library, plugin will not be built")
    set(CECE_PLUGINS_BUILD_diffusion-python_DISABLE Off PARENT_SCOPE)
    return ()
endif ()

# Include directories
include_directories(${PYTHON_INCLUDE_DIRS})

# ######################################################################### #

# Build plugin
build_plugin(diffusion-streamlines-python
    SOURCES
        Plugin.cpp
        wrappers/Module.cpp
    PLUGINS_REQUIRED
        diffusion-streamlines python
    LIBRARIES
        ${PYTHON_LIBRARIES}
)

# ######################################################################### #
