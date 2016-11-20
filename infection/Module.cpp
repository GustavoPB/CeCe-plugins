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
            c_bond.get<RealType>("disassociation-constant"),
            c_bond.get("pathogen"),
            c_bond.get("host"),
			c_bond.get<int>("max-offspring")
        });
    }

    if(config.has("info-file-path"))
    {
    	infoFilePath = config.get<String>("info-file-path");
		for(Bond bond: m_bonds)
		{
			//Print headers
			std::ofstream myfile (infoFilePath);
			if (myfile.is_open())
			  {
				myfile << "Pathogen: " << bond.pathogen << "\n";
				myfile << "Host: " << bond.host << "\n";
				myfile << "Max offspring: " << bond.maxOffspring << "\n\n";

				myfile << "Iteration - Pathogen Count - Fitness Average - Default F. Distance - F. Distance Average - Good Pathogen Ratio [%] - Good Pathogen Children Ratio [%]\n";
				myfile.close();
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
        bondConfig.set("disassociation-constant", bond.dConst);
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
    m_step = getSimulation().getTimeStep();

    auto _ = measure_time("infection", simulator::TimeMeasurement(getSimulation()));

    // Foreach pending bindings
    for (auto& p : m_bindings)
    {
        auto data = makeUnique<BoundData>();
        data->module = this;
        data->Kd = p.dConst;
        data->offspring = p.offspring;

        p.o1->createBound(*p.o2, std::move(data)); //o1 -> host, o2->pathogen. Thus bonded object is always pathogen
    }

    m_bindings.clear();

	//perform info printing each time a new pathogen is released
	auto currentIteration = getSimulation().getIteration();
	if(infoFilePath != "" && currentIteration != 1)
	{
		auto pathogenCount = getSimulation().getObjectCount("cell.Phage");
		int defaultDistance = 0;
		RealType goodPathogenCount = 0;
		RealType goodPathogenChildrenCount = 0;
		RealType fitnessAverage = 0;
		double fitnessDistanceAverage = 0;

		for (auto& object : getSimulation().getObjects("cell.Phage"))
		{
			auto phage = static_cast<plugin::cell::Phage*>(object.get());
			fitnessAverage += phage->getFitness();
			fitnessDistanceAverage += phage->getFitnessDistance();
			defaultDistance = phage->getBadFitnessDefaultDistance();
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
						defaultDistance << " - " <<
						fitnessDistanceAverage << " - " <<
						goodPathogenRatio << " - " <<
						goodPathogenChildrenRatio << "\n";
				myfile.close();
			  }
		}

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

            std::bernoulli_distribution dist(
                getDisassociationPropensity(m_step, data->Kd)
            );

            if (dist(g_gen))
            {
                CECE_ASSERT(bound.object);
                Log::debug("Released: ", cell->getId(), ", ", bound.object->getId());
                cell->removeBound(*bound.object);

                //Perform offspring
                {

                	auto offspring = data->offspring;
                	units::PositionVector destroyPos = Zero;
                	destroyPos[0] = units::Length(1000000);
                	destroyPos[1] = units::Length(1000000);
					auto phage = static_cast<plugin::cell::Phage*>(bound.object.get());
					auto hostPos = cell->getPosition();
					//Kill does not work properly
					//We program the killing by moving the cell out of the simulation frame
					cell->setPosition(destroyPos);
					for (unsigned int i = 0; i < offspring; i++)
                	{
						auto phageChild = phage->replicate();
						phageChild->setPosition(hostPos);
					}
					cell->setPosition(destroyPos);
                }

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

		//Cell casting
		auto host = is2Host ?
				static_cast<plugin::cell::CellBase*>(&o2) :
				static_cast<plugin::cell::CellBase*>(&o1);

		if (!host->isInfected())
		{
			host->setInfected(true);
			auto phage = is1Pathogen ?
					static_cast<plugin::cell::Phage*>(&o1) :
					static_cast<plugin::cell::Phage*>(&o2);

			auto fitnessDistance = phage->getFitnessDistance();
			auto phageAptitud = 1.0/fitnessDistance;
			std::bernoulli_distribution dist(phageAptitud);

			int offspring = 0;
			if(phageAptitud != std::numeric_limits<double>::infinity())
			{
				if(dist(g_gen))
				{
					double offspringBandwidth = 1.0/(double)m_bonds[i].maxOffspring;
					offspring = phageAptitud/offspringBandwidth;
				}
			}
			else
			{
				offspring = m_bonds[i].maxOffspring;
			}

			if(offspring != 0)
			{
				//Generamos enlace
				//Ensure that the first object is the host
				if(is1Pathogen)
				{
					Log::debug("Joined: ", o2.getId(), ", ", o1.getId());
					m_bindings.push_back(JointDef{&o2, &o1, m_bonds[i].dConst, offspring});
					//host->setInfected(true);
					continue;
				}
				else
				{
					Log::debug("Joined: ", o1.getId(), ", ", o2.getId());
					m_bindings.push_back(JointDef{&o1, &o2, m_bonds[i].dConst, offspring});
					//host->setInfected(true);
					continue;
				}
			}
			else
			{
				host->setInfected(false);
			}
		}


    }
}


/* ************************************************************************ */
}
}
}

/* ************************************************************************ */
