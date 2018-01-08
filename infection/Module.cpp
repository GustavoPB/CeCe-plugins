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

	if(config.has("print-interval"))
    {
		setPrintInterval(config.get<int>("print-interval"));
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
					myfile << "Prob. of Infection: " << bond.probOfInfection << "\n";
					myfile << "Max offspring: " << bond.maxOffspring << "\n\n";

					myfile << "Time [hours], Pathogen Count, Fitness Average, Default F. Target, F. Distance Average, Good Pathogen Ratio [%], Good Pathogen Children Ratio [%], Host Residence Time [h], Phage Residence Time [h]\n";
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
    auto _ = measure_time("infection", simulator::TimeMeasurement(getSimulation()));
	m_step = getSimulation().getIteration()*getSimulation().getTimeStep();

	//perform info printing each time a new pathogen is released
    printSimulationInfo("Ecoli", trackedPathogen);

    // Foreach pending bindings
    for (auto& p : m_bindings)
    {
		auto phage = static_cast<plugin::cell::Phage*>(p.o2.get());
		auto data = makeUnique<BoundData>();

		data->module = this;
		data->singlePhageProductionRate = p.singlePhageProductionRate;
		phage->setTimeToRelease(data->singlePhageProductionRate);

		p.o1->createBound(*p.o2, std::move(data)); //o1 -> host, o2->pathogen. Thus bonded object is always pathogen
    }
    m_bindings.clear();

	// Foreach objects. Look for host minimizes the calcules
    for (auto& object : getSimulation().getObjects("Ecoli"))
    {
    	//De momento solo hay celulas
        //if (!object->is<plugin::cell::CellBase>())
        //    continue;

        auto cell = static_cast<object::Object*>(object.get());
        for (const auto& bound : cell->getBounds())
        {
            if (bound.data == nullptr)
                continue;

            const auto data = static_cast<const BoundData*>(bound.data.get());

            if (data->guard != '@')
                continue;

			auto phage = static_cast<plugin::cell::Phage*>(bound.object.get());

			//Perform offspring
			auto currentTimeToRelease = phage->getTimeToRelease() - getSimulation().getTimeStep();
			phage->setTimeToRelease(currentTimeToRelease);
			bool releaseOffspring = phage->getTimeToRelease() <= Zero;
			if (releaseOffspring)
			{
				CECE_ASSERT(bound.object);

				//Guard that prevent from crashing
				auto pos = phage->getPosition();
				auto world = getSimulation().getWorldSize();
				auto phageX = pos.getX();
				auto phageY = pos.getY();
				auto worldX = world.getX()/2 -units::Length(20);
				auto worldY = world.getY()/2 -units::Length(20);
				
				if((phageX >= worldX || phageX <= -worldX) && phageY >= worldY)
				{
					cell->removeBound(*bound.object);
					auto ecoli = static_cast<plugin::cell::Ecoli*>(cell);
					ecoli->addMolecules("RFP", 10000);
					continue;
				}

				int queue = std::abs(data->singlePhageProductionRate / getSimulation().getTimeStep());
				phage->queueForReplication(queue);

				phage->setTimeToRelease(data->singlePhageProductionRate);
		
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
					continue; //the object ain't pathogen nor host
			}

			//Checking object 2 type
			auto is2Host = false;
			if (type2 == typeHost)
			{
				is2Host = true;

			} else
			{
				if (type2 != typePathogen)
					continue; //the object ain't pathogen nor host
			}

			//Checking if both are the same type
			if(is1Pathogen != is2Host)
				continue;
		
			//Cell type casting
			auto host = is2Host ?
					static_cast<plugin::cell::CellBase*>(&o2) :
					static_cast<plugin::cell::CellBase*>(&o1);
			//Phage type casting
			auto phage = is1Pathogen ?
					static_cast<plugin::cell::Phage*>(&o1) :
					static_cast<plugin::cell::Phage*>(&o2);
			//Check if phage is infective - phage can only infect one bacteria
			if (!phage->IsInfective())
				return;
			
			auto probOfInfection = phage->getGiiiAmount();
			std::bernoulli_distribution associationDistribution(probOfInfection);

			if (associationDistribution(g_gen) && !host->isInfected())
			{
				host->setInfected(true);
				phage->disableInfection();

				//Calculate fitness according to Promoter - TransFactor Distance
				/// Note: do not assign fitness to phage to not affect basic infection system
				auto phageToxineFitness = calculateFitness(host->getPromoter(), phage->getTransFactor());
				Log::warning("Fitness host - phage: ", host->getPromoter()," : ", phage->getTransFactor());

				//Calculate Antitoxine Amount
				host->generateAntitoxine(phageToxineFitness, phage->getToxineMaximum());

				//Evaluate Toxine - Antitoxine balance
				auto toxinePenalty = host->checkToxineBalance(phage->getToxineAmount());

				//Update growth rate or kill cell
				if (!host->updateGrowthRate(toxinePenalty)) //like disabling cell: used when updateGrowthRate kills cell by toxine
				{
					host->addMolecules("RFP", 10000);
					return; //TOREVIEW: infective phage is also disabled
				}

				//Calculate Offspring according to fitness
				//TOREVIEW: implement a selector - adapt to run basic infection protocol
				auto singlePhageProductionRate = 
				CalculeSinglePhageProductionRate(
					//phage->getFitnessDistance(), //TOREVIEW: adapt to run basic infection protocol
					phageToxineFitness,
					m_bonds[i].maxOffspring,
					ppr
				);

				Log::warning("Phage release rate: ", singlePhageProductionRate, " mins");

				if (singlePhageProductionRate == Zero)
				{
					host->setInfected(false);
					phage->disableInfection();
					return;
				}
					
				///Generate bond: ensure that the first object is the host
				if(is1Pathogen)
				{
					Log::debug("Joined: ", o2.getId(), ", ", o1.getId());
					m_bindings.push_back(JointDef{&o2, &o1, singlePhageProductionRate});
				}
				else
				{
					Log::debug("Joined: ", o1.getId(), ", ", o2.getId());
					m_bindings.push_back(JointDef{&o1, &o2, singlePhageProductionRate});
				}
				return;

			}
			else
			{
				return;
			}
		}
    }

units::Time Module::CalculeSinglePhageProductionRate(RealType phageFitness, int maxOffspring, units::Time ppr)
{
	RealType offspring = 0.0;
	units::Time result = Zero;

	if(phageFitness >= 0)
	{
		offspring = phageFitness * (RealType)maxOffspring;
	}
	else // If Distance equals 0 then the offspring is exactly the fitness target
	{
		offspring = (RealType)maxOffspring;
	}

	result = offspring < 1 ?
		Zero :
		ppr/offspring;

	return result;
}

RealType Module::calculateFitness(int promoter, int transcriptionFactor)
{
	int distance = abs(promoter - transcriptionFactor);
	// When fitness equals -1, then maximum promoter - transcription factor match
	RealType fitness = distance == 0 ?
		-1 :
		1.0/distance;
	return fitness;
}

void Module::printSimulationInfo(String hostType, String pathogenType)
{
	auto currentIteration = getSimulation().getIteration();
	auto shouldPrint = currentIteration % getPrintInterval() == 0;

	if(infoFilePath != "" && currentIteration != 1 && shouldPrint)
	{
		auto pathogenCount = getSimulation().getObjectCount(pathogenType);
		auto hostCount = getSimulation().getObjectCount(hostType);
		int fitnessTarget = 0;
		RealType goodPathogenCount = 0;
		RealType goodPathogenChildrenCount = 0;
		RealType fitnessAverage = 0;
		units::Time simulationMinutes;
		double fitnessDistanceAverage = 0;
		units::Time pathogenTotalResidenceTime = Zero;
		units::Time averagePathogenResidenceTime = Zero;
		units::Time hostTotalResidenceTime = Zero;
		units::Time averageHostResidenceTime = Zero; 

		for (auto& object : getSimulation().getObjects(hostType))
		{
			auto host = static_cast<plugin::cell::Ecoli*>(object.get());
			hostTotalResidenceTime += host->getLifeTime();
		}

		for (auto& object : getSimulation().getObjects(pathogenType))
		{
			auto phage = static_cast<plugin::cell::Phage*>(object.get());
			fitnessAverage += phage->getFitness();
			fitnessDistanceAverage += phage->getFitnessDistance();
			fitnessTarget = phage->getGoodFitnessValue();
			auto goodFitnessRange = phage->getFitness() < (phage->getGoodFitnessValue() + phage->getGoodFitnessAmplitude());
			pathogenTotalResidenceTime += phage->getLifeTime();

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
		simulationMinutes = (currentIteration * getSimulation().getTimeStep()) / 60;
		averagePathogenResidenceTime = pathogenTotalResidenceTime / pathogenCount;
		averagePathogenResidenceTime /= 60;
		averageHostResidenceTime = hostTotalResidenceTime / hostCount;
		averageHostResidenceTime /= 60;

		//print
		{
			std::ofstream myfile (infoFilePath, std::ios::app);
			if (myfile.is_open())
			  {
				myfile << simulationMinutes << ", " <<
						pathogenCount << ", " <<
						fitnessAverage << ", " <<
						fitnessTarget << ", " <<
						fitnessDistanceAverage << ", " <<
						goodPathogenRatio << ", " <<
						goodPathogenChildrenRatio << ", " <<
						averageHostResidenceTime << "," <<
						averagePathogenResidenceTime << "\n";
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
