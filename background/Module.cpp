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

// STB
#define STB_IMAGE_IMPLEMENTATION
//#define STBI_NO_JPEG
//#define STBI_NO_PNG
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_SIMD // __cpu_model issue
#include "stb_image.h"

// CeCe
#include "cece/core/VectorRange.hpp"
#include "cece/core/InStream.hpp"
#include "cece/render/Color.hpp"
#include "cece/config/Configuration.hpp"
#include "cece/simulator/Simulation.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace background {

/* ************************************************************************ */

void Module::loadConfig(const config::Configuration& config)
{
    // Configure parent
    module::Module::loadConfig(config);

    // Get image name
    setImageName(config.get("image"));
}

/* ************************************************************************ */

void Module::storeConfig(config::Configuration& config) const
{
    module::Module::storeConfig(config);

    config.set("image", getImageName());
}

/* ************************************************************************ */

void Module::init()
{
    // Get image resource
    auto file = getSimulation().getResource(getImageName());

    if (!file)
        throw RuntimeException("Image '" + getImageName() + "' not found");

    // Define callbacks
    const stbi_io_callbacks callbacks{
        // read
        [] (void* user, char* data, int size) -> int {
            return reinterpret_cast<InOutStream*>(user)->read(data, size).gcount();
        },
        // skip
        [] (void* user, int n) -> void {
            reinterpret_cast<InOutStream*>(user)->ignore(n);
        },
        // eof
        [] (void* user) -> int {
            return reinterpret_cast<InOutStream*>(user)->eof();
        }
    };

    // Load image
    int x, y, n;
    unsigned char* data = stbi_load_from_callbacks(&callbacks, file.get(), &x, &y, &n, 0);

    // Store size and channels
    m_size = Vector<unsigned int>(x, y);
    m_channels = n;

    // Allocate memory
    m_data.resize(x * y * n);

    // Copy data
    std::memcpy(m_data.data(), data, m_data.size());

    stbi_image_free(data);
}

/* ************************************************************************ */

void Module::draw(render::Context& context)
{
    if (!m_drawable)
    {
        m_drawable.create(context, m_size);

        // Copy data
        for (auto&& c : range(m_size))
        {
            render::Color pixel;

            // Pointer
            const auto ptr = m_data.data() + m_channels * ((m_size.getHeight() - c.getY() - 1) * m_size.getWidth() + c.getX());

            switch (m_channels)
            {
            case 1:
                pixel = render::Color::fromUchar(ptr[0], ptr[0], ptr[0]);
                break;

            case 2:
                pixel = render::Color::fromUchar(ptr[0], ptr[0], ptr[0], ptr[1]);
                break;

            case 3:
                pixel = render::Color::fromUchar(ptr[0], ptr[1], ptr[2]);
                break;

            case 4:
                pixel = render::Color::fromUchar(ptr[0], ptr[1], ptr[2], ptr[3]);
                break;
            }

            m_drawable->set(c, pixel);
        }
    }

    // Draw color grid
    context.matrixPush();
    context.matrixScale(getSimulation().getWorldSize() / units::Length(1));
    context.colorPush();
    context.enableAlpha();
    m_drawable->draw(context);
    context.disableAlpha();
    context.colorPop();
    context.matrixPop();
}

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
