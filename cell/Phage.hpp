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

// CeCe
#include "cece/config.hpp"
#include "cece/core/UnitIo.hpp"
#include <random>
#include "cece/core/Log.hpp"

#ifdef CECE_RENDER
#  include "cece/render/State.hpp"
#  include "cece/render/Object.hpp"
#  include "cece/render/Context.hpp"
#  include "DrawablePhage.hpp"
#endif

// Plugin
#include "CellBase.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace cell {

/* ************************************************************************ */

/**
 * @brief Phage representation.
 */
class Phage : public CellBase
{

// Public Ctors & Dtors
public:


    /**
     * @brief Constructor.
     *
     * @param simulation
     * @param type       Cell type.
     */
    explicit Phage(simulator::Simulation& simulation, String typeName = "cell.Phage",
                   object::Object::Type type = object::Object::Type::Dynamic) noexcept;


    /**
     * @brief Destructor.
     */
    ~Phage();


// Public Accessors
public:


    /**
     * @brief Returns if Phage has bud.
     *
     * @return
     */
    bool hasBud() const noexcept
    {
        return m_hasBud;
    }


    /**
     * @brief Return bud volume.
     *
     * @return
     */
    units::Volume getVolumeBud() const noexcept
    {
        if (hasBud())
            return m_bud.volume;
        else
            return Zero;
    }


    /**
     * @brief Return bud current angle.
     *
     * @return
     */
    units::Angle getAngleBud() const noexcept
    {
        if (hasBud())
            return m_bud.angle;
        else
            return Zero;
    }


    /**
     * @brief Return volume when buds are produced.
     *
     * @return
     */
    units::Volume getVolumeBudCreate() const noexcept
    {
        return m_volumeBudCreate;
    }


    /**
     * @brief Return volume (bud) when buds are released.
     *
     * @return
     */
    units::Volume getVolumeBudRelease() const noexcept
    {
        return m_volumeBudRelease;
    }

    //Fitness values

    double getGoodFitnessProportion() const noexcept
	{
    	return goodFitnessProp;
	}

    int getBadFitnessDefaultDistance() const noexcept
	{
		return badFitnessDefaultDistance;
	}

    int getGoodFitnessValue() const noexcept
	{
		return goodFitnessValue;
	}

    int getBadFitnessAmplitude() const noexcept
	{
		return badFitnessAmplitude;
	}

    int getGoodFitnessAmplitude() const noexcept
	{
		return goodFitnessAmplitude;
	}

    int getFitness() const noexcept
    {
    	return fitness;
    }

    double getFitnessDistance()
    {
    	return fitnessDistance;
    }

    bool isChild()
    {
    	return isChildPhage;
    }

    //Infection methods
	bool IsInfective()
	{
		return isInfective;
	}

	units::Time getSearchTime()
	{
		return searchTime;
	}

    units::Time getTimeToRelease ()
    {
        return timeToRelease;
    }

    units::Time getLifeTime()
    {
        return lifeTime;
    }

    /// Toxine - Antintoxine driven behavior
    int getGiiiAmount() 
    {
        return m_q_giii;
    }

    int getTransFactorLibrary()
    {
        return m_trasnfactor_library;
    }

// Public Accessors
public:


    /**
     * @brief Set bud volume.
     *
     * @param volume
     */
    void setVolumeBud(units::Volume volume) noexcept
    {
        if (!hasBud() && volume != Zero)
            budCreate();

        m_bud.volume = std::move(volume);
    }


    /**
     * @brief Set bud angle.
     *
     * @param angle
     */
    void setAngleBud(units::Angle angle) noexcept
    {
        if (!hasBud() && angle != Zero)
            budCreate();

        m_bud.angle = std::move(angle);
    }


    /**
     * @brief Set volume when buds are produced.
     *
     * @param volume
     */
    void setVolumeBudCreate(units::Volume volume) noexcept
    {
        m_volumeBudCreate = std::move(volume);
    }


    /**
     * @brief Set volume (bud) when buds are released.
     *
     * @param volume
     */
    void setVolumeBudRelease(units::Volume volume) noexcept
    {
        m_volumeBudRelease = std::move(volume);
    }

    ///Fitness methods

    void setGoodFitnessProportion(double value) noexcept
	{
		goodFitnessProp = value;
	}

    void setBadFitnessDefaultDistance(int value) noexcept
	{
    	badFitnessDefaultDistance = value;
	}

    void setGoodFitnessValue(int value) noexcept
	{
    	goodFitnessValue = value;
	}

    void setBadFitnessAmplitude(int value) noexcept
	{
    	badFitnessAmplitude = value;
	}

    void setGoodFitnessAmplitude(int value) noexcept
	{
    	goodFitnessAmplitude = value;
	}

    void setFitness(int value)
    {
    	fitness = value;
    }

    void setFitnessDistance(double value)
    {
    	fitnessDistance = value;
    }

    void setChild()
    {
    	isChildPhage = true;
    }

    //Infection methods
    void enableInfection()
    {
    	isInfective = true;
    	addMolecules("GFP", 100000);
    }

    void disableInfection()
    {
    	isInfective = false;
        removeMolecules("GFP", 100000);
    }

    void setSearchTime(units::Time value)
    {
    	searchTime = value;
    	currentSearchTime = value;
    }

    void setTimeToRelease (units::Time value)
    {
        timeToRelease = value;
    }

    void setLifeTime (units::Time value)
    {
        lifeTime = value;
    }

    /// Toxine - Anttoxine driven Behavior

    void setToxineAmount (int value)
    {
        m_q_toxine = value;
    }

    void setGiiiAmount(int probability) 
    {
        m_q_giii = probability;
    }

    void setTransFactorLibrary (int value)
    {
        m_trasnfactor_library = value;
    }

    void initTranscriptionFactor (int tp_library) {
        std::random_device g_rd;
        std::default_random_engine eng(g_rd());
        std::uniform_int_distribution<int> unif_dist(0, tp_library);
        m_transfactor = unif_dist(eng);
    }

    void initToxineBehavior (int max_q_toxine, int tp_library, int good_f_proportion) {
        int assigned_q_toxine = 0;
        int assigned_q_giii = 0;
        int assigned_fitness = 0;

        initTranscriptionFactor(tp_library);

        std::random_device g_rd;
        // Distribuimos fitness en función de "good-fitness-proportion"
        std::default_random_engine eng(g_rd());
        std::bernoulli_distribution bern_dist(good_f_proportion);

        if (bern_dist(eng))
        {
            //Asignar buen fitness
            assigned_q_toxine = 0;
            assigned_q_giii = 100;
            assigned_fitness = generateGoodFitness();
        }
        else
        {
            //Asignar mal fitness
            assigned_q_toxine = max_q_toxine;
            assigned_q_giii = 0;
            assigned_fitness = generateBadFitness();
        }
        setToxineAmount(assigned_q_toxine);
        setGiiiAmount(assigned_q_giii);
        setFitness(assigned_fitness);
    }

// Public Operations
public:


    /**
     * @brief Update phage state.
     *
     * @param dt Time step.
     */
    void update(units::Time dt) override;


    /**
     * @brief Configure object.
     *
     * @param config
     * @param simulation
     */
    void configure(const config::Configuration& config, simulator::Simulation& simulation) override;


    /**
     * @brief Create new phage bud.
     */
    void budCreate();


    /**
     * @brief Release bud phage.
     */
    void budRelease();

    int calculateFitness();

    int generateGoodFitness();

    int generateBadFitness();

    /**
     * @brief Method for replication
     */

    ViewPtr<plugin::cell::Phage> replicate();

    /**
     * @brief Mutation ratio according to a certain probability
     */
    void mutate();

    /**
	 * @brief We check if the phage is able to infect a bacteria considering its search time
	 */
    void checkSearchTime(units::Time dt)
    {
    	this->currentSearchTime -= dt;
    	if (this->currentSearchTime <= Zero)
    	{
    		this->enableInfection();
    	}
    }

    void queueForReplication(int queue)
    {
        shouldReplicate = true;
        replicationQueue = queue;
    }

#ifdef CECE_RENDER

    /**
     * @brief Render phage.
     *
     * @param context
     */
    virtual void draw(render::Context& context) override;


    /**
     * @brief Store current state for drawing.
     * State should be stored in back state because the front state is
     * used for rendering.
     * Drawing state should contain data that can be modified during update()
     * call and are used for rendering.
     */
    void drawStoreState() override;


    /**
     * @brief Swap render state.
     * Calling this function should be guarded by mutex for all modules
     * to ensure all modules are in same render state.
     * Function should be fast because otherwise it will block rendering.
     */
    void drawSwapState() override;

#endif


// Protected Operations
protected:


    /**
     * @brief Update phage physics shape.
     */
    void updateShape();


// Private Structures
private:

#ifdef CECE_RENDER
    struct RenderState
    {
        units::PositionVector position;
        units::Length radius;
        units::Angle angle;
        units::Length budRadius;
        render::Color color;
        render::Color idColor;
    };
#endif

 struct MutationConf
    {
        units::Time Time;
        RealType Probability;
        RealType Amplitude;
    };

// Private Data Members
private:


    /// If phage has a bud.
    bool m_hasBud = false;

    /// List of mutation distribution throught time
    DynamicArray<MutationConf> m_mutations;

    /**
     * @brief phage bud.
     */
    struct
    {
        /// Angle.
        units::Angle angle = Zero;

        /// Bud volume.
        units::Volume volume = Zero;

        /// Fake Volume
        units::Volume volumeFake = Zero; //TODELETE

        /// Last shape update radius.
        units::Length lastRadius = Zero;
    } m_bud;

    /// Volume that is needed to bud creation.
    units::Volume m_volumeBudCreate = units::um3(12);

    /// Volume (bud) that is needed to bud release.
    units::Volume m_volumeBudRelease = units::um3(10);

#ifdef CECE_RENDER
    /// Render object for Phage.
    /// Shared between all instances, it's same for all instances.
    render::ObjectSharedPtr<DrawablePhage> m_renderObject;

    /// Render state.
    render::State<RenderState> m_drawableState;
#endif

    /// Last shape update radius.
    units::Length m_lastRadius = Zero;

    /// If shape must be updated.
    bool m_shapeForceUpdate = false;

    ///Fitness parameters
    double goodFitnessProp = 0.0;

    int badFitnessDefaultDistance = 0;

    int goodFitnessValue = 0;

    int badFitnessAmplitude = 0;

    int goodFitnessAmplitude = 0;

    ///Cell fitness value
    int fitness = 0;

    bool isChildPhage = false;

    double fitnessDistance = 0;

    ///Infection parameters
    bool isInfective = false;

    units::Time searchTime = Zero;

    units::Time currentSearchTime = Zero;

    units::Time timeToRelease = Zero;

    //Life time
    units::Time lifeTime = Zero;

    bool shouldReplicate = false;

    int replicationQueue = 0;

    //// Promoter library range
    int m_transfactor = 0;

    int m_q_toxine = 0; 

    int m_q_giii = 0;

    int m_trasnfactor_library = 0;

};

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
