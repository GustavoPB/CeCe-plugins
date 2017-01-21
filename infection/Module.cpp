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
#include <iostream>
#include <fstream>

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
#include "../cell/Phage.hpp"

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

//void performOffspring(plugin::cell::Phage& pathogen, int offspring, units::PositionVector hostPos)
//{
//	for (unsigned int i = 0; i < offspring; i++)
//	{
//		auto child = getSimulation().createObject(pathogen.getTypeName());
//		auto phageChild = static_cast<plugin::cell::Phage*>(child.get());
//
//		//Transmit phage properties
//		phageChild->setPosition(hostPos);
//		phageChild->setFitness(pathogen.getFitness());
//		phageChild->setVolume(pathogen.getVolume());
//		phageChild->setGoodFitnessValue(pathogen.getGoodFitnessValue());
//		phageChild->setMoleculeCount("BFP", 10000);
//	}
//}

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
        	c_bond.get<RealType>("probability-of-infection"),
            c_bond.get("pathogen"),
            c_bond.get("host"),
			c_bond.get<int>("max-offspring"),
			c_bond.get<units::Time>("eclipse-time"),
			c_bond.get<units::Time>("ppr"),
        });
    }

    if(config.has("info-file-path"))
    {
    	infoFilePath = config.get<String>("info-file-path");
    	trackedPathogen = config.get<String>("tracked-pathogen");
		for(Bond bond: m_bonds)
		{
			if(bond.pathogen == trackedPathogen)
			{
				//Print headers
				std::ofstream myfile (infoFilePath);
				if (myfile.is_open())
				  {
					myfile << "Pathogen: " << bond.pathogen << "\n";
					myfile << "Host: " << bond.host << "\n";
					myfile << "Max offspring: " << bond.maxOffspring << "\n\n";

					myfile << "Iteration - Pathogen Count - Fitness Average - Default F. Target - F. Distance Average - Good Pathogen Ratio [%] - Good Pathogen Children Ratio [%]\n";
					myfile.close();
				  }
			}
		}
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
        bondConfig.set("probability-of-infection", bond.probOfInfection);
        bondConfig.set("pathogen", bond.pathogen);
        bondConfig.set("host", bond.host);
        bondConfig.set("max-offspring", bond.maxOffspring);
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
    m_step = getSimulation().getIteration()*getSimulation().getTimeStep();

    auto _ = measure_time("infection", simulator::TimeMeasurement(getSimulation()));

    // Foreach pending bindings
    for (auto& p : m_bindings)
    {
        auto data = makeUnique<BoundData>();
        data->module = this;
        data->offspring = p.offspring;
        data->releaseDelay = p.eclipseTime;
        data->ppr = p.ppr;
        data->timeToRelease = p.eclipseTime + m_step;

        p.o1->createBound(*p.o2, std::move(data)); //o1 -> host, o2->pathogen. Thus bonded object is always pathogen
    }

    m_bindings.clear();

	//perform info printing each time a new pathogen is released
    printSimulationInfo(trackedPathogen);

	// Foreach objects
    for (auto& object : getSimulation().getObjects())
    {
    	//De momento solo hay celulas
        //if (!object->is<plugin::cell::CellBase>())
        //    continue;

        auto cell = static_cast<plugin::cell::CellBase*>(object.get());

        for (const auto& bound : cell->getBounds())
        {
            if (bound.data == nullptr)
                continue;

            const auto data = static_cast<const BoundData*>(bound.data.get());

            if (data->guard != '@')
                continue;

			//Perform offspring
			auto offspring = data->offspring;
			bool releaseOffspring = m_step > data->timeToRelease;

			if (offspring != 0 && releaseOffspring)
			{
				auto phage = static_cast<plugin::cell::Phage*>(bound.object.get());
				auto hostPos = cell->getPosition();

				for (unsigned int i = 0; i < offspring; i++)
				{
					auto phageChild = phage->replicate();
					phageChild->mutate();
					phageChild->setPosition(hostPos);
				}

				{
					auto updatedData = makeUnique<BoundData>();
					updatedData->module = this;
					updatedData->offspring = data->offspring;
					updatedData->releaseDelay = data->ppr;
					updatedData->ppr = data->ppr;
					updatedData->timeToRelease = data->releaseDelay + m_step;

					CECE_ASSERT(bound.object);
					cell->removeBound(*bound.object);

					cell->createBound(*bound.object.get(), std::move(updatedData));
				}

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
	auto type1 = o1.getTypeName();
	auto type2 = o2.getTypeName();

    for (unsigned int i = 0; i < m_bonds.size(); i++)
    {
    	//Checking if the simulation definition allows the bond
    	auto typePathogen = m_bonds[i].pathogen;
    	auto typeHost = m_bonds[i].host;
    	auto eclipseTime = m_bonds[i].eclipseTime;
    	auto ppr = m_bonds[i].ppr;

    	//Checking object 1 type
    	auto is1Pathogen = false;
    	if (type1 == typePathogen)
    	{
    		is1Pathogen = true;

    	} else
    	{
    		if (type1 != typeHost)
    			return; //the object ain't pathogen nor host
    	}

    	//Checking object 2 type
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

		//Cell type casting
		auto host = is2Host ?
				static_cast<plugin::cell::CellBase*>(&o2) :
				static_cast<plugin::cell::CellBase*>(&o1);

		std::bernoulli_distribution associationDistribution(m_bonds[i].probOfInfection);

		if (associationDistribution(g_gen) && !host->isInfected())
		{
			auto phage = is1Pathogen ?
					static_cast<plugin::cell::Phage*>(&o1) :
					static_cast<plugin::cell::Phage*>(&o2);

			//Check if search time is reached
			if (!phage->IsInfective())
				return;

			host->setInfected(true);

			auto fitnessDistance = phage->getFitnessDistance();
			auto phageAptitud = 1.0/fitnessDistance;

			int offspring = 0;
			if(phageAptitud != std::numeric_limits<double>::infinity())
			{
				double offspringBandwidth = 1.0/(double)m_bonds[i].maxOffspring;
				offspring = phageAptitud/offspringBandwidth;
			}
			else // In case Distance equals 0 so the solution is exactly the target
			{
				offspring = m_bonds[i].maxOffspring;
			}

			//Generate bond
			//Ensure that the first object is the host
			if(is1Pathogen)
			{
				Log::debug("Joined: ", o2.getId(), ", ", o1.getId());
				m_bindings.push_back(JointDef{&o2, &o1, offspring, eclipseTime, ppr});
				//host->setInfected(true);
				continue;
			}
			else
			{
				Log::debug("Joined: ", o1.getId(), ", ", o2.getId());
				m_bindings.push_back(JointDef{&o1, &o2, offspring, eclipseTime, ppr});
				//host->setInfected(true);
				continue;
			}

		}
    }
}


void Module::printSimulationInfo(String pathogenType)
{
	auto currentIteration = getSimulation().getIteration();
	if(infoFilePath != "" && currentIteration != 1)
	{
		auto pathogenCount = getSimulation().getObjectCount(pathogenType);
		int fitnessTarget = 0;
		RealType goodPathogenCount = 0;
		RealType goodPathogenChildrenCount = 0;
		RealType fitnessAverage = 0;
		double fitnessDistanceAverage = 0;

		for (auto& object : getSimulation().getObjects(pathogenType))
		{
			auto phage = static_cast<plugin::cell::Phage*>(object.get());
			fitnessAverage += phage->getFitness();
			fitnessDistanceAverage += phage->getFitnessDistance();
			fitnessTarget = phage->getGoodFitnessValue();
			auto goodFitnessRange = phage->getFitness() < (phage->getGoodFitnessValue() + phage->getGoodFitnessAmplitude());
			if (goodFitnessRange)
			{
				goodPathogenCount++;
				if(phage->isChild())
					goodPathogenChildrenCount++;
			}
		}
		fitnessAverage /= pathogenCount;
		fitnessDistanceAverage /= pathogenCount;
		RealType goodPathogenRatio = goodPathogenCount/pathogenCount*100;
		RealType goodPathogenChildrenRatio = goodPathogenChildrenCount/pathogenCount*100;
		//print
		{
			std::ofstream myfile (infoFilePath, std::ios::app);
			if (myfile.is_open())
			  {
				myfile << currentIteration << " - " <<
						pathogenCount << " - " <<
						fitnessAverage << " - " <<
						fitnessTarget << " - " <<
						fitnessDistanceAverage << " - " <<
						goodPathogenRatio << " - " <<
						goodPathogenChildrenRatio << "\n";
				myfile.close();
			  }
		}
	}
}


/* ************************************************************************ */
}
}
}

/* ************************************************************************ */
