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

#ifndef DOLBY_TCUTILS_FRAMERATE_H
#define DOLBY_TCUTILS_FRAMERATE_H

#include <cstdint>
#include <iosfwd>
#include <stdexcept>
#include <string>

namespace Dolby
{
    namespace TcUtils
    {
        class Framerate;
    }
}

/**
 * @brief Class to encapsulates an enum representing common framerates and related functionality.
 * @details Provides functions to access inherent properties of the various framerates and to apply
 * ratio conversion.
 */
class Dolby::TcUtils::Framerate
{
public:
    enum Enum : int
    {
        UNDEFINED = -1,
        FPS_23976,
        FPS_24,
        FPS_25,
        FPS_2997,
        FPS_2997DF,
        FPS_30,
        FPS_30DF,
        FPS_48,
        FPS_50,
        FPS_60,
        FPS_96,
        FPS_100,
        FPS_120,
        COUNT
    };

    /** @brief Exception thrown when trying to construct a Framerate from an invalid string */
    struct StringParseError : std::exception
    {
    };

private:
    /** @brief The encapsulated enum value */
    Enum mValue{UNDEFINED};

public:
    /**
     * @brief Default constructor. Initializes as UNDEFINED.
     */
    Framerate() = default;
    /**
     * @brief Construct from enum Enum.
     */
    constexpr Framerate(Enum value)
    : mValue(value)
    {
    }

    /**
     * @brief Construct from std::string.
     * @param str String representing the framerate value, with optional df or DF suffix for
     * drop-frame.
     */
    explicit Framerate(const std::string& str);
    /**
     * @brief Construct from char pointer
     * @param str String representing the framerate value, with optional df or DF suffix for
     * drop-frame.
     */
    explicit Framerate(const char* str);
    /**
     * @brief Construct from value in FPS.
     * @param fps The framerate value in FPS. Some margin is allowed for rounding errors (e.g.
     * 30/1.001 will construct a FPS_2997 Framerate).
     */
    explicit Framerate(double fps);
    /**
     * @brief Construct from value in FPS and drop-frame indicator.
     * @param fps The framerate value in FPS.
     * @param drop Whether to create a drop-frame Framerate (only for 29.97 and 30).
     */
    explicit Framerate(double fps, bool drop);

    /** @brief Compare framerate with another framerate (equals). */
    bool operator==(const Framerate& other) const
    {
        return GetEnum() == other.GetEnum();
    }

    /** @brief Compare framerate with another framerate (not equals). */
    bool operator!=(const Framerate& other) const
    {
        return GetEnum() != other.GetEnum();
    }

    /** @brief Get the encapsulated enum value. */
    Enum GetEnum() const
    {
        return mValue;
    }

    /** @brief Get the integer frame count. This is the frame count without applying the 1.001
     * ratio, e.g. 30 for FPS_2997.*/
    int GetFrameCount() const;

    /** @brief Convert to string. */
    const char* ToString() const;

    /** @brief Whether this framerate is valid or undefined */
    bool IsDefined() const;

    /** @brief Whether this framerate is a drop-frame framerate. */
    bool IsDrop() const;
    /** @brief Whether this framerate is an NTSC 1000/1001 framerate (23.976 and 29.97) */
    bool IsRatio1001() const;
    /** @brief Whether this framerate is a high framerate (double and quadruple of 24,25,30 fps) */
    bool IsHighFramerate() const;

    /** @brief Multiply value with 1000/1001 for NTSC 1000/1001 framerates, else leave untouched.
     */
    double ApplyRatio(double value) const;
    /** @brief Multiply value with 1001/1000 for NTSC 1000/1001 framerates, else leave untouched.
     */
    double ApplyInverseRatio(double value) const;

    /**
     * @brief Iterator class to iterate over all framerate values.
     */
    class Iterator
    {
        int mValue;

    public:
        Iterator() = delete;
        Iterator(Enum value);
        Framerate operator*() const;
        void operator++();
        bool operator!=(const Iterator& other) const;
    };

    /**
     * @brief Range class to iterate over all framerate values.
     */
    class Range
    {
        Iterator mBegin;
        Iterator mEnd;

    public:
        Range();
        Iterator begin() const
        {
            return mBegin;
        }
        Iterator end() const
        {
            return mEnd;
        }
    };

    /**
     * @brief Returns a Range to iterate over all framerate values.
     */
    static Range values();
};

namespace Dolby
{
    namespace TcUtils
    {
        std::ostream& operator<<(std::ostream& os, const Framerate& framerate);
    }
}

#endif // DOLBY_TCUTILS_FRAMERATE_H
