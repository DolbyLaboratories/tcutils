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

#ifndef DOLBY_TCUTILS_FRAMEBOUNDARY_H
#define DOLBY_TCUTILS_FRAMEBOUNDARY_H

#include <tcutils/Framerate.h>
#include <tcutils/Types.h>

namespace Dolby
{
    namespace TcUtils
    {
        class FrameBoundary;
    }
}

/**
 * @brief Class with static functions to relate sample positions to frame boundaries at a given
 * sample rate and frame rate.
 */
class Dolby::TcUtils::FrameBoundary
{
public:
    /**
     * @brief Calculate the sample position of the frame this sample is in (rounding down).
     */
    static Samples Floor(Framerate framerate, Samples samples, Samplerate sr);

    /**
     * @brief Calculate the sample position of the frame this sample is closest to. If samples
     * is exactly in between, use the upper frame.
     */
    static Samples Round(Framerate framerate, Samples samples, Samplerate sr);

    /**
     * @brief Calculate the sample position of the frame this sample matches with, or the next
     * frame if the sample is not on the frame boundary.
     */
    static Samples Ceiling(Framerate framerate, Samples samples, Samplerate sr);

    /**
     * @brief Check if the sample position corresponds with a frame boundary.
     */
    static bool IsOnBoundary(Framerate framerate, Samples samples, Samplerate sr);
};

#endif
