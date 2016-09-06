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
#include <cstdio>

// stb_image_write
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WRITE_NO_STDIO
#include "stb_image_write.h"

// CeCe
#include "cece/core/Log.hpp"
#include "cece/core/UniquePtr.hpp"
#include "cece/core/Exception.hpp"
#include "cece/core/FileStream.hpp"
#include "cece/config/Configuration.hpp"
#include "cece/simulator/Simulation.hpp"
#include "cece/render/ImageData.hpp"

/* ************************************************************************ */

namespace cece {
namespace plugin {
namespace picture {

/* ************************************************************************ */

void Module::loadConfig(const config::Configuration& config)
{
    // Configure parent
    module::Module::loadConfig(config);

    // File name pattern
    setFilePattern(config.get("pattern", getFilePattern()));

    // Save iteration
    setSaveIteration(config.get("iteration", getSaveIteration()));

    // Store alpha channel.
    m_alpha = config.get("alpha", m_alpha);
}

/* ************************************************************************ */

void Module::storeConfig(config::Configuration& config) const
{
    module::Module::storeConfig(config);

    config.set("pattern", getFilePattern());
    config.set("iteration", getSaveIteration());
    config.set("alpha", m_alpha);
}

/* ************************************************************************ */

void Module::update()
{
    if (m_size == Zero)
        return;

    const auto stepNumber = getSimulation().getIteration();

    // Skip steps
    if (stepNumber % getSaveIteration() != 0)
        return;

    auto pattern = getFilePattern();
    auto pos = pattern.find("$1");

    // Replace placeholder
    if (pos != String::npos)
    {
#if defined(__MINGW32__) || defined(__MINGW64__)
        char buffer[64];
        std::sprintf(buffer, "%d", stepNumber);
        String str = buffer;
#else
        String str = std::to_string(stepNumber);
#endif
        pattern.replace(pos, 2, str);
    }

    FilePath filename;

// Use working directory on Mac OS X
#if __APPLE__ && __MACH__
    // Get working directory
    const char* dir = getenv("WORKING_DIR");

    // Add directory
    if (dir)
        filename /= dir;
#endif

    filename /= pattern;

    // Write image
    save(filename);

    Log::info("Image saved: ", pattern);
}

/* ************************************************************************ */

void Module::draw(render::Context& context)
{
#ifdef CECE_THREAD_SAFE
    // Lock access
    MutexGuard guard(m_mutex);
#endif

    m_size = context.getSize();
    m_data.resize(m_size.getHeight() * m_size.getWidth() * (m_alpha ? 4 : 3));

    // Get image data
    context.getData(m_data.data(), m_alpha);
}

/* ************************************************************************ */

void Module::save(const FilePath& filename)
{
    OutFileStream file(filename.string(), OutFileStream::binary);

    if (!file.is_open())
        throw InvalidArgumentException("Cannot open output file: " + filename.string());

    // Write function
    stbi_write_func* func = [] (void* context, void* data, int size) {
        reinterpret_cast<OutFileStream*>(context)->write(reinterpret_cast<const char*>(data), size);
    };

    DynamicArray<unsigned char> data_flip;
    bool alpha;
    unsigned int width;
    unsigned int height;
    int channel_count;

    // Copy data
    {
#ifdef CECE_THREAD_SAFE
        // Lock access
        MutexGuard guard(m_mutex);
#endif
        alpha = m_alpha;
        width = m_size.getWidth();
        height = m_size.getHeight();
        channel_count = alpha ? 4 : 3;

        // Image is vertically flipped
        data_flip.resize(m_data.size());

        const unsigned int line_size = width * channel_count;

        // Vertically flip lines
        for (unsigned int i = 0; i < height; ++i)
        {
            std::memcpy(
                data_flip.data() + i * line_size,
                m_data.data() + (height - i - 1) * line_size,
                line_size
            );
        }
    }

    if (!stbi_write_png_to_func(func, &file, width, height, channel_count, data_flip.data(), 0))
        throw RuntimeException("Unable to write a picture");
}

/* ************************************************************************ */

}
}
}

/* ************************************************************************ */
