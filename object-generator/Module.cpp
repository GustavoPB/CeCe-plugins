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
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>

// CeCe
#include "cece/core/Log.hpp"
#include "cece/core/Assert.hpp"
#include "cece/core/Pair.hpp"
#include "cece/core/StringStream.hpp"
#include "cece/core/TimeMeasurement.hpp"
#include "cece/core/UnitIo.hpp"
#include "cece/object/Object.hpp"
#include "cece/simulator/Simulation.hpp"
#include "cece/simulator/TimeMeasurement.hpp"
#include "PID.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace object_generator {

/* ************************************************************************ */

namespace {

/* ************************************************************************ */

// Random engine
std::random_device g_rd;

/* ************************************************************************ */

std::default_random_engine g_gen(g_rd());

/* ************************************************************************ */

/**
 * @brief Parse list of active iterations.
 *
 * @param str
 *
 * @return
 */
DynamicArray<IterationRange> parseActive(String str)
{
    DynamicArray<IterationRange> res;

    InStringStream iss(std::move(str));

    while (true)
    {
        IterationType it;

        if (!(iss >> it))
            break;

        if (iss.peek() == '-')
        {
            IterationType itEnd;
            iss.ignore();
            iss >> itEnd;

            res.emplace_back(it, itEnd);
        }
        else
        {
            // Single item range
            res.emplace_back(it);
        }
    }

    return res;
}

/* ************************************************************************ */

/**
 * @brief Check if iteration is in range.
 *
 * @param list
 * @param it
 *
 * @return
 */
bool inRange(const DynamicArray<IterationRange>& list, IterationType it)
{
    // No limitation
    if (list.empty())
        return true;

    for (const auto& p : list)
    {
        if (p.inRange(it))
            return true;
    }

    return false;
}

/* ************************************************************************ */

}

/* ************************************************************************ */

/**
 * @brief Read distribution description.
 *
 * @param is
 * @param distr
 *
 * @return
 */
InStream& operator>>(InStream& is, ObjectDesc::Distributions& distr)
{
    for (unsigned int i = 0; i < distr.size(); ++i)
    {
        String name;
        is >> std::skipws >> name;

        if (name == "uniform")
            distr[i].type = Distribution::Type::Uniform;
        else if (name == "normal")
            distr[i].type = Distribution::Type::Normal;
        else
            throw InvalidArgumentException("Unknown distribution: " + name);

        is >> distr[i].parameters[0];
        is >> distr[i].parameters[1];
    }

    return is;
}

/* ************************************************************************ */

/**
 * @brief Write distribution description.
 *
 * @param os
 * @param distr
 *
 * @return
 */
OutStream& operator<<(OutStream& os, const ObjectDesc::Distributions& distr)
{
    for (unsigned int i = 0; i < distr.size(); ++i)
    {
        switch (distr[i].type)
        {
        case Distribution::Type::Uniform:
            os << "uniform"; break;
        case Distribution::Type::Normal:
            os << "normal"; break;
        default:
            throw InvalidArgumentException("Unknown distribution");
        }

        os << " " << distr[i].parameters[0];
        os << " " << distr[i].parameters[1];
        os << " ";
    }

    return os;
}

/* ************************************************************************ */

void Module::update()
{
    auto& simulation = getSimulation();

    auto _ = measure_time("object-generator", simulator::TimeMeasurement(simulation));

    // Get current iteration number
    const auto iteration = simulation.getIteration();

    // Foreach generated objects
    for (const auto& desc : m_objects)
    {
        // Skip inactive generators
        if (!inRange(desc.active, iteration))
            continue;

        //Looking after the density of an object stay steady
        double calculatedRate;
        if (desc.supervisedVolume != Zero)
        {
        	auto objectCount = simulation.getObjectCount(desc.className);
        	auto currentIteration = simulation.getIteration();
        	const auto currentObjDensity = objectCount/desc.supervisedVolume;
        	//Log::warning(currentObjDensity);

        	//PID Regulation
        	PID pid = PID(static_cast<double>(simulation.getTimeStep()),
        				  desc.pidParams.max,
						  desc.pidParams.min,
						  desc.pidParams.kp,
						  desc.pidParams.kd,
						  desc.pidParams.ki);
        	auto pidResult = pid.calculate(static_cast<double>(desc.steadyDensity), static_cast<double>(currentObjDensity));

        	//Print info if file is defined
        	if (desc.csvPath != "")
        	{
        		std::ofstream myfile (desc.csvPath, std::ios::app);
				if (myfile.is_open())
				  {
					myfile << currentIteration << " - " <<
							objectCount << " - " <<
							currentObjDensity << " - " <<
							pidResult << "\n";
					myfile.close();
				  }
        	}

        	calculatedRate = pidResult;
        }
        else
        {
        	// Create object number + probability
        	calculatedRate = desc.rate * simulation.getTimeStep();
        }

        // Number of created with 100% probability
        const auto baseCount = std::floor(calculatedRate);

        // Probability of the remaining object
        const auto probability = calculatedRate - baseCount;
        Assert(probability >= 0);
        Assert(probability <= 1);
        std::bernoulli_distribution distSpawn(probability);

        // Total number of spawned objects
        auto count = baseCount + (distSpawn(g_gen) ? 1 : 0);

        // Generate
        while (count--)
        {
            // Create object
            auto object = simulation.createObject(desc.className);
            Assert(object);

            units::PositionVector pos = Zero;

            // Generate position
            for (unsigned int i = 0; i < pos.getSize(); ++i)
            {
                const auto& distr = desc.distributions[i];

                if (distr.type == Distribution::Type::Uniform)
                {
                    std::uniform_real_distribution<RealType> dist(
                        distr.parameters[0].value(),
                        distr.parameters[1].value()
                    );

                    pos[i] = units::Length(dist(g_gen));
                }
                else if (distr.type == Distribution::Type::Normal)
                {
                    std::normal_distribution<RealType> dist(
                        distr.parameters[0].value(),
                        distr.parameters[1].value()
                    );

                    pos[i] = units::Length(dist(g_gen));
                }
                else
                {
                    Assert(false);
                }
            }

            object->configure(desc.config, simulation);
            object->setPosition(pos);
        }
    }

}

/* ************************************************************************ */

void Module::loadConfig(const config::Configuration& config)
{
    // Configure parent
    module::Module::loadConfig(config);

    auto& simulation = getSimulation();

    for (auto&& cfg : config.getConfigurations("object"))
    {
        ObjectDesc desc;
        desc.className = cfg.get("class");

        if(cfg.has("supervisedVolume") && cfg.has("steadyDensity"))
        {
        	desc.supervisedVolume = cfg.get<units::Volume>("supervisedVolume");
        	desc.steadyDensity = cfg.get<units::Density>("steadyDensity");
        	desc.pidParams.kp = cfg.get<double>("pidKp");
			desc.pidParams.ki = cfg.get<double>("pidKi");
			desc.pidParams.kd = cfg.get<double>("pidKd");
			desc.pidParams.max = cfg.get<double>("pidMax");
			desc.pidParams.min = cfg.get<double>("pidMin");
        	Log::warning("Value set to supervised volume. Object density control enabled");

        	//Print results on csv file
        	if(cfg.has("pathToCsvFile"))
        	{
        		desc.csvPath = cfg.get<String>("pathToCsvFile");
        		//Print headers
        		std::ofstream myfile (desc.csvPath);
        		if (myfile.is_open())
        		  {
        			myfile << "Supervised Volume: " << desc.supervisedVolume << "\n";
        			myfile << "Steady Density: " << desc.steadyDensity << "\n";
        			myfile << "PID configuration: " << "Kp: " << desc.pidParams.kp <<
        					" Kd: " << desc.pidParams.kd <<
							" Ki: " << desc.pidParams.ki <<
							" Max: " << desc.pidParams.max <<
							" Min: " << desc.pidParams.min << "\n";
        			myfile << "Simulation Time Step: " << simulation.getTimeStep() << "s\n\n";
        		    myfile << "Iteration - ObjectCount - CurrentDensity - PID_Output\n";
        		    myfile.close();
        		  }
        	}
        }
        else
        {
        	Log::warning("No value set to supervised volume. Object density control not enabled");
        	desc.supervisedVolume = Zero;
        	desc.steadyDensity = Zero;

        	//Looking for rate values instead
        	if (cfg.has("rate"))
			{
				desc.rate = cfg.get<ObjectDesc::SpawnRate>("rate");
			}
			else
			{
				// Backward compatibility
				Log::warning("[object-generator] 'probability' is deprecated, use 'rate' instead.");
				desc.rate = cfg.get<ObjectDesc::SpawnRate>("probability");
			}
        }

        if (cfg.has("distribution"))
        {
            desc.distributions  = cfg.get<ObjectDesc::Distributions>("distribution");
        }
        else if (cfg.has("size"))
        {
            const auto pos = cfg.get<units::PositionVector>("position");
            const auto size = cfg.get<units::PositionVector>("size");
            const auto half = size * 0.5;

            for (unsigned int i = 0; i < desc.distributions.size(); ++i)
            {
                desc.distributions[i].type = Distribution::Type::Uniform;
                desc.distributions[i].parameters[0] = pos[i] - half[i];
                desc.distributions[i].parameters[1] = pos[i] + half[i];
            }
        }
        else
        {
            const auto posMin = cfg.get<units::PositionVector>("position-min");
            const auto posMax = cfg.get<units::PositionVector>("position-max");

            for (unsigned int i = 0; i < desc.distributions.size(); ++i)
            {
                desc.distributions[i].type = Distribution::Type::Uniform;
                desc.distributions[i].parameters[0] = posMin[i];
                desc.distributions[i].parameters[1] = posMax[i];
            }
        }

        desc.active = parseActive(cfg.get("active", String{}));
        desc.config = cfg.toMemory();

        add(std::move(desc));
    }
}

/* ************************************************************************ */

void Module::storeConfig(config::Configuration& config) const
{
    module::Module::storeConfig(config);

    for (const auto& object : m_objects)
    {
        auto objectConfig = config.addConfiguration("object");

        objectConfig.set("class", object.className);
        objectConfig.set("rate", object.rate);
        objectConfig.set("distribution", object.distributions);
        //objectConfig.set("active", object.active);

        // TODO: store remaining config
        // desc.config
    }
}

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
