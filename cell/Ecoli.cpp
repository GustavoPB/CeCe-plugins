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
#include "Ecoli.hpp"

// C++
//#include <random>

// CeCe
#include "cece/core/Assert.hpp"
#include "cece/core/constants.hpp"
#include "cece/core/Shape.hpp"
#include "cece/core/UnitIo.hpp"
#include "cece/config/Configuration.hpp"
#include "cece/simulator/Simulation.hpp"
//#include "cece/core/Log.hpp"

#ifdef CECE_RENDER
#  include "cece/render/Color.hpp"
#endif

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace cell {

/* ************************************************************************ */

//static std::random_device g_rd;

/* ************************************************************************ */

Ecoli::Ecoli(simulator::Simulation& simulation, String typeName, object::Object::Type type) noexcept
    : CellBase(simulation, std::move(typeName), type)
#ifdef CECE_RENDER
    , m_renderObject()
#endif
{
    setVolume(units::um3(13));
    setDensity(units::kg(1200) / units::m3(1));

    // Max 2 shapes
    auto& shapes = getMutableShapes();
    shapes.reserve(2);
    shapes.push_back(Shape::makeCircle(calcRadius(getVolume())));

    //getBody()->SetAngularDamping(10);
}

/* ************************************************************************ */

Ecoli::~Ecoli()
{
    // Nothing to do
}

/* ************************************************************************ */

void Ecoli::update(units::Time dt)
{
    CellBase::update(dt);

    //Increment Life in time step
    setLifeTime(getLifeTime() + dt);


    auto pos = this->getPosition();
    auto world = getSimulation().getWorldSize();
    auto phageX = pos.getX();
    auto phageY = pos.getY();
    auto worldX = world.getX()/2 -units::Length(20);
    auto worldY = world.getY()/2 -units::Length(20);

    bool exit = false;
    
    if((phageX >= worldX || phageX <= -worldX) && phageY >= worldY)
    {
        this->setInfected(true);
        exit = true;
    }


	if ((getVolume() >= (getVolumeMax() - (units::Volume)0.1)) && !exit)
	{
		budRelease();
	}

    // Update cell shape
    updateShape();
}

/* ************************************************************************ */

void Ecoli::configure(const config::Configuration& config, simulator::Simulation& simulation)
{
    CellBase::configure(config, simulation);

    setVolumeBud(config.get("volume-bud", getVolumeBud()));
    setAngleBud(config.get("angle-bud", getAngleBud()));
    setVolumeBudCreate(config.get("volume-bud-create", getVolumeBudCreate()));
    setVolumeBudRelease(config.get("volume-bud-release", getVolumeBudRelease()));

    // Toxine - Antitoxine driven behaviour
    auto promoter_library = config.get<int>("promoter-library");
    setPromoter(promoter_library);

    // Update cell shape
    updateShape();
}

/* ************************************************************************ */

void Ecoli::budCreate()
{
    Assert(!hasBud());

//    std::default_random_engine eng(g_rd());
//    std::uniform_real_distribution<float> dist(0.f, 1.f);

    m_hasBud = true;
//    m_bud.angle = units::Angle(2 * constants::PI * dist(eng));
//
//    m_shapeForceUpdate = true;
}

/* ************************************************************************ */

void Ecoli::budRelease()
{

    auto splitedVolume = this->getVolumeMax()/2;
    this->setVolume(splitedVolume);

    // Calculate bud position
    const auto angle = getRotation();
    const auto offset = units::PositionVector(Zero, calcRadius(getVolume()) + calcRadius(splitedVolume)).rotated(-getAngleBud());

    const auto omega = getAngularVelocity();
    const auto center = getMassCenterPosition();

    // Change ecoli velocity
    if (omega != Zero)
    {
        setVelocity(getVelocity() + cross(omega, getPosition() - center));
        setAngularVelocity(omega);
    }

    // Get current position
    const auto posBud = getPosition() + offset.rotated(angle);
    const auto velocityBud = getVelocity() + cross(omega, getWorldPosition(offset) - center);

    // Release bud into the world
    auto newEcoli = getSimulation().createObject((String)getTypeName());
    auto bud = static_cast<plugin::cell::Ecoli*>(newEcoli.get());
    bud->setVolume(splitedVolume);
    bud->setPosition(posBud);
    bud->setVelocity(velocityBud);
    bud->setAngularVelocity(omega);
    bud->setPrograms(getPrograms().clone());
    bud->setDensity(getDensity());
    bud->setGrowthRate(getGrowthRate());
    bud->setCurrentGrowthRate(getGrowthRate());
    bud->setInfected(false);
    bud->setVolumeMax(getVolumeMax());
    bud->updateShape();

    // Split molecules between Ecoli and bud

    // Total volume
    const auto totalVolume = getVolume()*2;

    // Copy old state
    const auto molecules = getMolecules();

    // Foreach molecules
    for (const auto& p : molecules)
    {
        // Molecules per volume unit
        const auto concentration = p.second / totalVolume;

        // Set molecule count to Ecoli and bud
        setMoleculeCount(p.first, concentration * getVolume());
        bud->setMoleculeCount(p.first, concentration * bud->getVolume());
    }

    // Release bud
    m_hasBud = false;
    m_bud.volume = Zero;
    m_shapeForceUpdate = true;
}

/* ************************************************************************ */

#ifdef CECE_RENDER
void Ecoli::draw(render::Context& context)
{
    if (!m_renderObject)
        m_renderObject.create(context);

    const RenderState& state = m_drawableState.getFront();

    // Transform
    context.matrixPush();
    context.matrixTranslate(state.position);
    context.matrixRotate(state.angle);
    context.matrixScale(2 * state.radius.value());
    context.colorPush();
    context.enableAlpha();
    m_renderObject->draw(context, 0.5, 0.5 * (state.budRadius / state.radius), state.color, state.idColor);
    context.disableAlpha();
    context.colorPop();
    context.matrixPop();
}
#endif

/* ************************************************************************ */

#ifdef CECE_RENDER
void Ecoli::drawStoreState()
{
    RenderState& state = m_drawableState.getBack();

    state.position = getPosition();
    state.radius = calcRadius(getVolume());
    state.idColor = getIdentificationColor();

    if (hasBud())
    {
        state.angle = getRotation() - m_bud.angle;
        state.budRadius = calcRadius(m_bud.volume);
        state.color = calcFluorescentColor(getVolume() + m_bud.volume);
    }
    else
    {
        state.angle = getRotation();
        state.budRadius = Zero;
        state.color = calcFluorescentColor(getVolume());
    }
}
#endif

/* ************************************************************************ */

#ifdef CECE_RENDER
void Ecoli::drawSwapState()
{
    m_drawableState.swap();
}
#endif

/* ************************************************************************ */

void Ecoli::updateShape()
{
    static constexpr auto MIN_CHANGE = units::Length(0.10);

    // Alias for ecoli shapes
    auto& shapes = getMutableShapes();

    bool needs_update = m_shapeForceUpdate;

    /*if (hasBud())
    {
        const auto newRadius = calcRadius(getVolume());
        const auto newBudRadius = calcRadius(getVolumeBud());
        const auto oldBudRadius = m_bud.lastRadius;
        needs_update = needs_update || ((newBudRadius - oldBudRadius) > MIN_CHANGE);

        const auto radiusMin = MIN_CHANGE;
        const auto radiusRelease = calcRadius(getVolumeBudRelease());

        const auto c1 = (radiusRelease + radiusMin) / (radiusRelease - radiusMin);
        const auto c2 = newRadius - 2 * radiusMin * (1 + radiusMin / (radiusRelease - radiusMin));
        const auto c = c1 * newBudRadius + c2;

        const auto center = 0.9 * units::PositionVector(Zero, c).rotated(-m_bud.angle);

        if (shapes.size() != 2)
        {
            shapes.push_back(Shape::makeCircle(newBudRadius, center));
        }
        else
        {
            shapes[1].getCircle().radius = newBudRadius;
            shapes[1].getCircle().center = center;
        }
    }
    else
    {*/
        const auto newRadius = calcRadius(getVolume());
        const auto oldRadius = m_lastRadius;
        needs_update = needs_update || ((newRadius - oldRadius) > MIN_CHANGE);

        // Update main shape
        shapes.resize(1);
        Assert(shapes.size() == 1);
        shapes[0].getCircle().radius = newRadius;
 //   }

    if (!needs_update)
        return;

    // Initialize shapes
    initShapes();
    m_shapeForceUpdate = false;
}

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
