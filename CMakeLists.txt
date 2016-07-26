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

cmake_minimum_required(VERSION 3.1)

# ######################################################################### #

project(CeCe-plugins)

# ######################################################################### #

if (NOT TARGET CeCe)
    # Standalone build
    # TODO: find CeCe library
    add_subdirectory(CeCe)
    include_directories(
        CeCe
        ${CMAKE_CURRENT_BINARY_DIR}/CeCe
    )
endif ()

# ######################################################################### #

# Available plugins
set(CECE_PLUGINS
    agglutination
    background
    cell
    cell-python
    chemostat
    cytometry
    diffusion
    diffusion-python
    diffusion-streamlines
    fluorescence
    object-generator
    object-streamlines-generator
    obstacle
    obstacles-svg
    picture
    python
    stochastic-reactions
    streamlines
    streamlines-channel
    streamlines-python
    xml
)

# ######################################################################### #

##
## Get plugin project name.
##
function(plugin_project_name PLUGIN_NAME PROJECT_NAME)
    set(${PROJECT_NAME} "plugin-${PLUGIN_NAME}" PARENT_SCOPE)
endfunction ()

# ######################################################################### #

##
## Get plugins project names.
##
function(plugin_project_names PLUGIN_NAMES PROJECT_NAMES)
    set(NAMES "")

    foreach (PLUGIN_NAME ${PLUGIN_NAMES})
        plugin_project_name(${PLUGIN_NAME} PROJECT_NAME)
        list(APPEND NAMES ${PROJECT_NAME})
    endforeach ()

    set(${PROJECT_NAMES} ${NAMES} PARENT_SCOPE)
endfunction ()

# ######################################################################### #

##
## Build plugin helper function
##
## param: NAME      Plugin name.
## param: SOURCES   List of sources.
## param: LIBRARIES A list of required libraries.
## param: PLUGINS_REQUIRED A list of required plugins.
##
## Function uses PLUGINS_BUILTIN_${NAME} variable.
##
function(build_plugin NAME)

    # Get project name
    plugin_project_name(${NAME} FULLNAME)

    include(CMakeParseArguments)
    cmake_parse_arguments(ARG "EXPORT_HEADER" "" "SOURCES;LIBRARIES;DEPENDENCIES;PLUGINS_REQUIRED" ${ARGN})

    # Modify list of plugins (add prefixes)
    set(PLUGINS_DEPENDENCIES "")

    foreach (PLUGIN ${ARG_PLUGINS_REQUIRED})
        plugin_project_name(${PLUGIN} PLUGIN_NAME)
        list(APPEND PLUGINS_DEPENDENCIES "${PLUGIN_NAME}")
    endforeach ()

    # Create project
    project(${FULLNAME})

    # Shared module
    add_library(${PROJECT_NAME} SHARED ${ARG_SOURCES})

    # Setup dependencies
    if (${ARG_DEPENDENCIES})
        add_dependencies(${PROJECT_NAME}
            ${ARG_DEPENDENCIES}
        )
    endif ()

    # Link libraries
    target_link_libraries(${PROJECT_NAME}
        ${PLUGINS_DEPENDENCIES}
        ${ARG_LIBRARIES}
        CeCe
    )

    set_target_properties(${PROJECT_NAME} PROPERTIES
        INSTALL_RPATH "$ORIGIN"
        MACOSX_RPATH On
        CXX_STANDARD 11
        CXX_EXTENSIONS Off
        CXX_STANDARD_REQUIRED On
    )

    # Generate export header
    if (ARG_EXPORT_HEADER)
        include(GenerateExportHeader)
        generate_export_header(${FULLNAME}
            EXPORT_FILE_NAME "export.hpp"
            BASE_NAME "CECE_${FULLNAME}"
        )
    endif ()

endfunction()

# ######################################################################### #

# Build plugins option
foreach(PLUGIN_NAME ${CECE_PLUGINS})
    if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_NAME}")
        option(CECE_PLUGINS_BUILD_${PLUGIN_NAME} "Build plugin '${PLUGIN_NAME}'" On)

        if (CECE_PLUGINS_BUILD_${PLUGIN_NAME})
            add_subdirectory(${PLUGIN_NAME})
        endif ()
    endif()
endforeach()

# ######################################################################### #
