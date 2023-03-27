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

#ifndef TCUTILS_TIMECODESAMPLES_H
#define TCUTILS_TIMECODESAMPLES_H

#include <tcutils/Timecode.h>

namespace Dolby
{
    namespace TcUtils
    {
        class TimecodeSamples;
    }
}

/**
 * @brief Class to encapsulate a timecode and a number of remaining samples.
 * @details This class encapsulates Timecode plus Samples, where samples represents the remaining
 * fraction of a frame.
 */
class Dolby::TcUtils::TimecodeSamples
{
    Timecode mTimecode;
    Samples mRemainder;

public:
    /**
     * @brief Default constructor.
     * @details Constructor that leaves all values initialized at -1 or UNDEFINED.
     */
    TimecodeSamples();

    /**
     * @brief Construct from samples
     * @details Creates a Timecode for the frame this sample is in and store the remaining samples.
     */
    explicit TimecodeSamples(Framerate framerate, Samples samples, Samplerate samplerate);

    const Timecode& GetTimecode() const
    {
        return mTimecode;
    }

    Samples GetRemainder() const
    {
        return mRemainder;
    }

    /**
     * @brief Convert to string
     *
     * Write the timecode and samples to an std::string in format hh:mm:ss:ff.xxx (hh:mm:ss;ff.xxx
     * for drop-frame frame rate), where .xxx is to amount of samples
     *
     * @param showZeroSamples When set to false, show the timecode without samples (just like
     * Timecode) if the samples value is zero
     */
    std::string ToString(bool showZeroSamples = true) const;
};

#endif
