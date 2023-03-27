// BSD 3-Clause License
//
// Copyright (c) 2022-2023, Dolby International AB.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions
// and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of
// conditions and the following disclaimer in the documentation and/or other materials provided with
// the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be used to
// endorse or promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
// WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef DOLBY_TCUTILS_CONVERT_H
#define DOLBY_TCUTILS_CONVERT_H

#include <tcutils/Framerate.h>
#include <tcutils/Types.h>

namespace Dolby
{
    namespace TcUtils
    {
        class Convert;
    }
}

/**
 * @brief Utility class with static functions for different conversions.
 * @details See README.md for a detailing description of DAMF-time.
 */
class Dolby::TcUtils::Convert
{
public:
    /**
     * @brief Convert real-time in seconds to DAMF-time in seconds using the given framerate.
     * @details Note: If the source time is represented in samples, use the corresponding function
     * instead, as it takes sample precision into account. Using seconds=samples/samplerate is not
     * the same using samples directly.
     */
    static DAMFSeconds ToDAMFSeconds(Framerate framerate, Seconds seconds);

    /**
     * @brief Convert samples at the given samplerate to DAMF-time in seconds using the given
     * framerate.
     */
    static DAMFSeconds ToDAMFSeconds(Framerate framerate, Samples samples, Samplerate sr);

    /**
     * @brief Convert DAMF-time in seconds to real-time in seconds using the given framerate.
     */
    static Seconds ToSeconds(Framerate framerate, DAMFSeconds damfSeconds);
    /**
     * @brief Convert real-time in seconds to samples using the given samplerate.
     */
    static Samples ToSamples(Seconds seconds, Samplerate sr);
    /**
     * @brief Convert DAMF-time in seconds to samples using the given framerate and samplerate.
     */
    static Samples ToSamples(Framerate framerate, DAMFSeconds damfSeconds, Samplerate sr);
};

#endif
