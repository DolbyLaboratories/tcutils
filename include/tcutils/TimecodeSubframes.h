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

#ifndef TCUTILS_TIMECODESUBFRAMES_H
#define TCUTILS_TIMECODESUBFRAMES_H

#include <tcutils/Timecode.h>

namespace Dolby
{
    namespace TcUtils
    {
        class TimecodeSubframes;
    }
}

/**
 * @brief Class to encapsulate a timecode and a number of remaining subframes.
 * @details This class encapsulates Timecode and 'subframes', a representation of the remaining
 * fraction of a frame with a given denominator (typically 80 or 100). The subframe is rounded away
 * from the frame-boundary, so that only the exact frame boundary is represented with 0 subframes.
 */
class Dolby::TcUtils::TimecodeSubframes
{
    Timecode mTimecode;
    int mSubframesNum;
    int mSubframesDenom;

public:
    /**
     * @brief Default constructor.
     *
     * Constructor that leaves all values initialized at -1 or UNDEFINED.
     */
    TimecodeSubframes();

    /**
     * @brief Construct from timecode and given subframe numerator and denominator
     */
    explicit TimecodeSubframes(Timecode timecode, int subframesNum, int subframesDenom);

    /**
     * @brief Construct from samples
     *
     * Creates a Timecode for the frame this sample is in and express the remainder
     * in subframes, rounding away from to nearest frame boundary, so that only
     * the sample that matches the frame boundary exactly results in zero subframes.
     */
    explicit TimecodeSubframes(Framerate framerate,
                               Samples samples,
                               Samplerate samplerate,
                               int subframesDenom);

    const Timecode& GetTimecode() const
    {
        return mTimecode;
    }

    int GetSubframesNum() const
    {
        return mSubframesNum;
    }

    int GetSubframesDenom() const
    {
        return mSubframesDenom;
    }

    /**
     * @brief Convert to string
     *
     * Write the timecode and subframes to an std::string in format hh:mm:ss:ff.xxx (hh:mm:ss;ff.xxx
     * for drop-frame frame rate), where .xxx is to amount of subframes
     *
     * @param showZeroSubframes When set to false, show the timecode without subframes (just like
     * Timecode) if the subframes value is zero
     */
    std::string ToString(bool showZeroSubframes = true) const;
};

#endif
