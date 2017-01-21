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

#pragma once

/* ************************************************************************ */

// C++
#include <utility>

// CeCe
#include "cece/config.hpp"
#include "cece/core/DynamicArray.hpp"
#include "cece/core/String.hpp"
#include "cece/core/Real.hpp"
#include "cece/core/ViewPtr.hpp"
#include "cece/module/Module.hpp"
#include "cece/object/ContactListener.hpp"
#include "cece/object/BoundData.hpp"
#include "cece/core/UnitIo.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace infection {

/* ************************************************************************ */

/**
 * @brief Module for infection.
 */
class Module : public module::Module, public object::ContactListener
{

// Public Ctors & Dtors
public:


    using module::Module::Module;


// Public Operations
public:


    /**
     * @brief Load module configuration.
     *
     * @param config Source configuration.
     */
    void loadConfig(const config::Configuration& config) override;


    /**
     * @brief Store module configuration.
     *
     * @param config Output configuration.
     */
    void storeConfig(config::Configuration& config) const override;


    /**
     * @brief Initialize module.
     */
    void init() override;


    /**
     * @brief Update module state.
     */
    void update() override;


    /**
     * @brief Terminate module.
     */
    void terminate() override;


    /**
     * @brief When two objects contact.
     *
     * @param o1 The first object.
     * @param o2 The second object.
     */
    void onContact(object::Object& o1, object::Object& o2) override;

    /*
     * @perform info printig
     */
    void printSimulationInfo(String pathogenType);

// Private Structures
private:


    /**
     * @brief Structure for storing bonds.
     */
    struct Bond
    {
    	RealType probOfInfection;
        String pathogen;
        String host;
        int maxOffspring;
        units::Time eclipseTime;
    };


    /**
     * @brief User data for joint.
     */
    struct BoundData : public object::BoundData
    {
        char guard = '@';
        ViewPtr<Module> module;
        int offspring;
        units::Time eclipseTime;
        units::Time timeToRelease;
    };


    /**
     * @brief Joint definition.
     */
    struct JointDef
    {
        ViewPtr<object::Object> o1;
        ViewPtr<object::Object> o2;
        int offspring;
        units::Time eclipseTime;
    };


// Private Data Members
private:

    /// List of bindings.
    DynamicArray<JointDef> m_bindings;

    /// Used time step.
    units::Time m_step;

    /// List of created bonds.
    DynamicArray<Bond> m_bonds;

    String infoFilePath;

    String trackedPathogen;
};

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
