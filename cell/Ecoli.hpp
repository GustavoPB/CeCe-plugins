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
#include <random>
#include "cece/core/Log.hpp"

#ifdef CECE_RENDER
#  include "cece/render/State.hpp"
#  include "cece/render/Object.hpp"
#  include "cece/render/Context.hpp"
#  include "DrawableEcoli.hpp"
#endif

// Plugin
#include "CellBase.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace cell {

/* ************************************************************************ */

/**
 * @brief Ecoli representation.
 */
class Ecoli : public CellBase
{

// Public Ctors & Dtors
public:


    /**
     * @brief Constructor.
     *
     * @param simulation
     * @param type       Cell type.
     */
    explicit Ecoli(simulator::Simulation& simulation, String typeName = "cell.Ecoli",
                   object::Object::Type type = object::Object::Type::Dynamic) noexcept;


    /**
     * @brief Destructor.
     */
    ~Ecoli();


// Public Accessors
public:


    /**
     * @brief Returns if Ecoli has bud.
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

    units::Time getLifeTime()
    {
        return lifeTime;
    }

    //Toxine - Antitoxine Drive Behavior
    int getPromoterLibrary() 
    {
        return m_promoter_library; 
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

    void setLifeTime (units::Time value)
    {
        lifeTime = value;
    }

    /// Toxine - Anttoxine driven Behavior
    void setPromoter (int promoter_library) {
        std::random_device g_rd;
        std::default_random_engine eng(g_rd());
        std::uniform_int_distribution<int> unif_dist(0, promoter_library);
        m_promoter = unif_dist(eng);
    }

    void setPromoterLibrary(int value) 
    {
        m_promoter_library = value; 
    }


// Public Operations
public:


    /**
     * @brief Update ecoli state.
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
     * @brief Create new ecoli bud.
     */
    void budCreate();


    /**
     * @brief Release bud ecoli.
     */
    void budRelease();


#ifdef CECE_RENDER

    /**
     * @brief Render ecoli.
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
     * @brief Update ecoli physics shape.
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

// Private Data Members
private:

    /// If ecoli has a bud.
    bool m_hasBud = false;


    /**
     * @brief ecoli bud.
     */
    struct
    {
        /// Angle.
        units::Angle angle = Zero;

        /// Bud volume.
        units::Volume volume = Zero;

        /// Last shape update radius.
        units::Length lastRadius = Zero;
    } m_bud;

    /// Volume that is needed to bud creation.
    units::Volume m_volumeBudCreate = units::um3(12);

    /// Volume (bud) that is needed to bud release.
    units::Volume m_volumeBudRelease = units::um3(10);

#ifdef CECE_RENDER
    /// Render object for Ecoli.
    /// Shared between all instances, it's same for all instances.
    render::ObjectSharedPtr<DrawableEcoli> m_renderObject;

    /// Render state.
    render::State<RenderState> m_drawableState;
#endif

    /// Last shape update radius.
    units::Length m_lastRadius = Zero;

    /// If shape must be updated.
    bool m_shapeForceUpdate = false;

    /// Life time
    units::Time lifeTime = Zero;

    /// Toxine - Anttoxine driven Behavior
    
    //// Promoter library range
    int m_promoter_library = 0;

    //// Assigned promoter
    int m_promoter = 0; 

    //// Antitoxine generated amount 
    //int q_antitoxine = Zero; // TO REVIEW: see if being defined is needed

};

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
