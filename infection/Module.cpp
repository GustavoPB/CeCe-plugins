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

// Declaration
#include "Module.hpp"

// C++
#include <random>
#include <algorithm>

// CeCe
#include "cece/core/Log.hpp"
#include "cece/core/constants.hpp"
#include "cece/core/TimeMeasurement.hpp"
#include "cece/config/Configuration.hpp"
#include "cece/simulator/TimeMeasurement.hpp"
#include "cece/simulator/Simulation.hpp"

// Plugins
#include "../cell/CellBase.hpp"
#include "../cell/Ecoli.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace infection {

/* ************************************************************************ */

namespace {

/* ************************************************************************ */

std::random_device g_rd;

/* ************************************************************************ */

std::default_random_engine g_gen(g_rd());

/* ************************************************************************ */

RealType getRelativeReceptorProportion(RealType radius, unsigned int numberOfRec)
{
    return 1 - std::exp(- numberOfRec / (4 * core::constants::PI * radius * radius));
}

/* ************************************************************************ */

RealType getAssociationPropensity(units::Time step,
    RealType radius1, RealType radius2,
    unsigned int numberOfRec1, unsigned int numberOfRec2,
    RealType Ka)
{
    return  getRelativeReceptorProportion(radius1, numberOfRec1) *
            getRelativeReceptorProportion(radius2, numberOfRec2) *
            (1 - std::exp(-Ka * step.value()));
}

/* ************************************************************************ */

RealType getDisassociationPropensity(units::Time step, RealType Kd)
{
    return 1 - std::exp(-Kd * step.value());
}

/* ************************************************************************ */

}

/* ************************************************************************ */

void Module::loadConfig(const config::Configuration& config)
{
    // Configure parent
    module::Module::loadConfig(config);

    for (auto&& c_bond : config.getConfigurations("bond"))
    {
        m_bonds.push_back(Bond{
            c_bond.get<RealType>("association-constant"),
            c_bond.get<RealType>("disassociation-constant"),
            c_bond.get("pathogen"),
            c_bond.get("host")
        });
    }
}

/* ************************************************************************ */

void Module::storeConfig(config::Configuration& config) const
{
    module::Module::storeConfig(config);

    // Foreach bonds
    for (const auto& bond : m_bonds)
    {
        auto bondConfig = config.addConfiguration("bond");
        bondConfig.set("association-constant", bond.aConst);
        bondConfig.set("disassociation-constant", bond.dConst);
        bondConfig.set("pathogen", bond.pathogen);
        bondConfig.set("host", bond.host);
    }
}

/* ************************************************************************ */

void Module::init()
{
    getSimulation().setContactListener(this);
}

/* ************************************************************************ */

void Module::update()
{
    // Store time step
    m_step = getSimulation().getTimeStep();

    auto _ = measure_time("infection", simulator::TimeMeasurement(getSimulation()));

    // Foreach pending bindings
    for (auto& p : m_bindings)
    {
        auto data = makeUnique<BoundData>();
        data->module = this;
        data->Kd = p.dConst;

        p.o1->createBound(*p.o2, std::move(data));
    }

    m_bindings.clear();

    // Foreach objects
    for (auto& object : getSimulation().getObjects())
    {
        if (!object->is<plugin::cell::CellBase>())
            continue;

        auto cell = object->cast<plugin::cell::CellBase>();

        for (const auto& bound : cell->getBounds())
        {
            if (bound.data == nullptr)
                continue;

            const auto data = static_cast<const BoundData*>(bound.data.get());

            if (data->guard != '@')
                continue;

            std::bernoulli_distribution dist(
                getDisassociationPropensity(m_step, data->Kd)
            );

            if (dist(g_gen))
            {
                CECE_ASSERT(bound.object);
                Log::debug("Released: ", cell->getId(), ", ", bound.object->getId());
                cell->removeBound(*bound.object);
            }
        }
    }
}

/* ************************************************************************ */

void Module::terminate()
{
    getSimulation().setContactListener(nullptr);
}

/* ************************************************************************ */

void Module::onContact(object::Object& o1, object::Object& o2)
{
	//Programamos escape si los objetos de que chocan no son del tipo especificado por configuración
    //if (!o1.is<plugin::cell::Ecoli>() || !o2.is<plugin::cell::Ecoli>())
      //  return;



    // Get cell objects
//    auto cell1 = o1.cast<plugin::cell::Ecoli>();
//    auto cell2 = o2.cast<plugin::cell::Ecoli>();

    // Get cells radiuses
//    const auto radius1 = o1.getShapes()[0].getCircle().radius;
//    const auto radius2 = o2.getShapes()[0].getCircle().radius;

	auto type1 = o1.getTypeName();
	auto type2 = o2.getTypeName();

    for (unsigned int i = 0; i < m_bonds.size(); i++)
    {
    	//Checking if the simulation definition allows the bond
    	auto typePathogen = m_bonds[i].pathogen;
    	auto typeHost = m_bonds[i].host;

    	//Checkin object 1 type
    	auto is1Pathogen = false;
    	if (type1 == typePathogen)
    	{
    		is1Pathogen = true;

    	} else
    	{
    		if (type1 != typeHost)
    			return; //the object ain't pathogen nor host
    	}

    	//Checkin object 2 type
		auto is2Host = false;
		if (type2 == typeHost)
		{
			is2Host = true;

		} else
		{
			if (type2 != typePathogen)
				return; //the object ain't pathogen nor host
		}

		//Checking if both are the same type
		if(is1Pathogen != is2Host)
			return;


//        std::bernoulli_distribution dist1(
//            getAssociationPropensity(
//                m_step, radius1.value(), radius2.value(),
//                cell1->getMoleculeCount(m_bonds[i].host), cell2->getMoleculeCount(m_bonds[i].pathogen),
//                m_bonds[i].aConst
//            )
//        );

/*
        if (dist1(g_gen))
        {
            Log::debug("Joined: ", o1.getId(), ", ", o2.getId());
            m_bindings.push_back(JointDef{&o1, &o2, m_bonds[i].dConst});
            continue;
        }
*/

//        std::bernoulli_distribution dist2(
//            getAssociationPropensity(
//                m_step, radius1.value(), radius2.value(),
//                cell2->getMoleculeCount(m_bonds[i].host), cell1->getMoleculeCount(m_bonds[i].pathogen),
//                m_bonds[i].aConst
//            )
//        );
/*
        if (dist2(g_gen))
        {
            Log::debug("Joined: ", o2.getId(), ", ", o1.getId());
            m_bindings.push_back(JointDef{&o2, &o1, m_bonds[i].dConst});
            continue;
        }
*/
    }
}

/* ************************************************************************ */
}
}
}

/* ************************************************************************ */
