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

#ifndef DOLBY_TCUTILS_TIMEINSECONDS_H
#define DOLBY_TCUTILS_TIMEINSECONDS_H

#include <cstdint>

/**
 * @brief Strong types used by Timecode and utility classes.
 * @details We prefer using strong types instead of using scalars directly for clarity and because
 * it allows use to write constructors and do function overloading based on these types, instead of
 * having the resolt to static creator functions or function names containing the types explicitly.
 */

namespace Dolby
{
    namespace TcUtils
    {
        class Samplerate;
        class Samples;
        class Frames;
        class Seconds;
        class DAMFSeconds;
    }
}

/**
 * @brief Strong type class representing samplerate with double floating-point precision.
 */
class Dolby::TcUtils::Samplerate
{
    double mValue;

public:
    explicit Samplerate(double value)
    : mValue(value)
    {
    }
    double GetValue() const
    {
        return mValue;
    }
};

/**
 * @brief Strong type class representing an integer sample position.
 */
class Dolby::TcUtils::Samples
{
    int64_t mValue;

public:
    explicit Samples(int64_t value)
    : mValue(value)
    {
    }
    int64_t GetValue() const
    {
        return mValue;
    }
};

/**
 * @brief Strong type class representing an integer frame count.
 */
class Dolby::TcUtils::Frames
{
    int32_t mValue;

public:
    explicit Frames(int32_t value)
    : mValue(value)
    {
    }
    int32_t GetValue() const
    {
        return mValue;
    }
};

/**
 * @brief Strong type class representing real-time in seconds with double floating-point precision.
 */
class Dolby::TcUtils::Seconds
{
    double mValue;

public:
    explicit Seconds(double value)
    : mValue(value)
    {
    }
    double GetValue() const
    {
        return mValue;
    }
};

/**
 * @brief Strong type class representing DAMF-time in seconds with double floating-point precision.
 * @details DAMF-time, used in Dolby DAMF files, is a epresentation of time that takes the NTSC
 * framerate ratio 1000/1001 into account (also known as NTSC-time), but unlike NTSC-time also takes
 * drop frames into account.See README.md for a more detailed description.
 */
class Dolby::TcUtils::DAMFSeconds
{
    double mValue;

public:
    explicit DAMFSeconds(double value)
    : mValue(value)
    {
    }
    double GetValue() const
    {
        return mValue;
    }
};

namespace Dolby
{
    namespace TcUtils
    {
        inline bool operator==(const Seconds& a, const Seconds& b)
        {
            return a.GetValue() == b.GetValue();
        }

        inline bool operator==(const DAMFSeconds& a, const DAMFSeconds& b)
        {
            return a.GetValue() == b.GetValue();
        }

        inline bool operator==(const Samples& a, const Samples& b)
        {
            return a.GetValue() == b.GetValue();
        }
    }
}

#endif
