/* ************************************************************************ */
/* Georgiev Lab (c) 2015-2016                                               */
/* ************************************************************************ */
/* Department of Cybernetics                                                */
/* Faculty of Applied Sciences                                              */
/* University of West Bohemia in Pilsen                                     */
/* ************************************************************************ */
/*                                                                          */
/* This file is part of CeCe.                                               */
/*                                                                          */
/* CeCe is free software: you can redistribute it and/or modify             */
/* it under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or        */
/* (at your option) any later version.                                      */
/*                                                                          */
/* CeCe is distributed in the hope that it will be useful,                  */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with CeCe.  If not, see <http://www.gnu.org/licenses/>.            */
/*                                                                          */
/* ************************************************************************ */

// This must be first
#include "Python.hpp"

// CeCe
#include "cece/core/Exception.hpp"
#include "cece/plugin/definition.hpp"
#include "cece/plugin/Api.hpp"
#include "cece/plugin/Repository.hpp"

// Plugin
#include "Module.hpp"
#include "Object.hpp"
#include "Program.hpp"
#include "Initializer.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace python {

/* ************************************************************************ */

void init_stdout(void);
void init_core(void);
void init_simulator(void);

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */

/**
 * @brief Python modules initialization table.
 */
static const struct _inittab INIT_TABLE[] = {
    {const_cast<char*>("cppout"),    cece::plugin::python::init_stdout},
    {const_cast<char*>("core"),      cece::plugin::python::init_core},
    {const_cast<char*>("simulator"), cece::plugin::python::init_simulator},
    {NULL, NULL}
};

/* ************************************************************************ */

using namespace cece;

/* ************************************************************************ */

class PythonApi : public plugin::Api
{

    /**
     * @brief On plugin load.
     *
     * @param repository Plugin repository.
     */
    void onLoad(plugin::Repository& repository) const override
    {
        if (PyImport_ExtendInittab(const_cast<struct _inittab*>(INIT_TABLE)) != 0)
            throw RuntimeException("Unable to initialize Python import table");

        // Initialize Python interpreter
        Py_Initialize();

        repository.registerApi(this).
            registerInitializer<plugin::python::Initializer>("python").
            registerModule<plugin::python::Module>("python").
            registerProgram<plugin::python::Program>("python")
        ;
    }


    /**
     * @brief On plugin unload.
     *
     * @param repository Plugin repository.
     */
    void onUnload(plugin::Repository& repository) const override
    {
        plugin::Api::onUnload(repository);

        Py_Finalize();
    }

};

/* ************************************************************************ */

CECE_DEFINE_PLUGIN(python, PythonApi)

/* ************************************************************************ */
