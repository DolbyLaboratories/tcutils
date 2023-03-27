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

#ifndef DOLBY_TCUTILS_TIMECODE_H
#define DOLBY_TCUTILS_TIMECODE_H

#include <iosfwd>
#include <stdexcept>
#include <string>
#include <tcutils/Framerate.h>
#include <tcutils/Types.h>

namespace Dolby
{
    namespace TcUtils
    {
        class Timecode;
        std::ostream& operator<<(std::ostream& os, const Timecode& timecode);

        /**
         * @brief Enum used to indicate whether to wrap timecode at 24 hours for Timecode
         * constructors.
         */
        enum class WrapMode
        {
            WRAP_AT_MIDNIGHT = 0, /// Wrap at midnight (24:00:00:00 becomes 00:00:00:00). (default)
            CONTINUE = 1, /// Don't wrap at midnight but allow to continue until 99 hours (limit of
                          /// 2 digits).
            DEFAULT = WRAP_AT_MIDNIGHT
        };

        /**
         * Enum used to indicate frame boundary rounding behaviour for Timecode constructors
         */
        enum class RoundingMode
        {
            TRUNCATE = 0, /// Create Timecode from given location rounding to lower frame boundary
            NEAREST  = 1, /// Create Timecode from given location rounding to nearest frame boundary
                          /// (default)
            DEFAULT = NEAREST
        };
    }
}

/**
 * @brief Class to represent and manipulate timecode in hours, minutes, seconds and frames.
 */
class Dolby::TcUtils::Timecode
{
    /// Internal helper struct for construction only
    struct TimeParameters;

    Timecode(Framerate framerate,
             const TimeParameters& params,
             WrapMode wrapMode = WrapMode::DEFAULT);

    Framerate mFramerate;
    WrapMode mWrapMode{WrapMode::DEFAULT};

    int mHours;
    int mMinutes;
    int mSeconds;
    int mFrames;

public:
    struct ValueOutOfRange : std::exception
    {
    };

    struct NegativeValues : std::exception
    {
    };

    struct InvalidDropFrame : std::exception
    {
    };

    struct InvalidFormat : std::exception
    {
    };

    struct StringParseError : std::exception
    {
    };

    /**
     * @brief Default constructor. Leaves all values initialized at -1 and framerate UNDEFINED.
     */
    Timecode();

    /**
     * @brief Constructor for Timecode with time not set ("--:--:--:--" or "--:--:--;--" for
     * drop-frame). Note that such timecode is considered valid, but not set.
     */
    explicit Timecode(Framerate framerate, WrapMode wrapMode = WrapMode::DEFAULT);

    /**
     * @brief Construct from std::string.
     * Throws when validation fails. To validate before construction, use the validate functions.
     * @param str String in hh:mm:ss:ff format (hh:mm:ss;ff for drop-frame frame rates).
     * @param wrapMode Optional value to indicate whether the timecode should wrap at 24 hours.
     */
    explicit Timecode(Framerate framerate,
                      const std::string& str,
                      WrapMode wrapMode = WrapMode::DEFAULT);

    /**
     * @brief Construct from char pointer.
     * Throws when validation fails. To validate before construction, use the validate functions.
     * @param str String in hh:mm:ss:ff format (hh:mm:ss;ff for drop-frame frame rates).
     * @param wrapMode Optional value to indicate whether the timecode should wrap at 24 hours.
     */
    explicit Timecode(Framerate framerate, const char* str, WrapMode wrapMode = WrapMode::DEFAULT);

    /**
     * @brief Construct from individual time units.
     * Throws when validation fails. To validate before construction, use the validate functions.
     * @param wrapMode Optional value to indicate whether the timecode should wrap at 24 hours.
     */
    explicit Timecode(Framerate framerate,
                      int hours,
                      int minutes,
                      int seconds,
                      int frames,
                      WrapMode wrapMode = WrapMode::DEFAULT);

    /**
     * @brief Construct from frames, adding drop-frames if required.
     * @details Create a Timecode for the given frame position, without having counted the drop
     * frames (if applicable). This function is symmetrical with toFrames().
     */
    explicit Timecode(Framerate framerate, Frames frames, WrapMode wrapMode = WrapMode::DEFAULT);

    /**
     * @brief Construct from a sample position.
     * @details Create a Timecode for the give sample position, either rounding down to the first
     * frame boundary not greater than the sample position, rounding to the nearest frame boundary
     * (half way rounds up) or rounding to the first frame boundary not less than the sample
     * position (the frame when the sample falls exactly on the frame boundary, otherwise the next
     * frame)
     */
    explicit Timecode(Framerate framerate,
                      Samples samples,
                      Samplerate samplerate,
                      RoundingMode roundingMode = RoundingMode::DEFAULT,
                      WrapMode wrapMode         = WrapMode::DEFAULT);

    /**
     * @brief Constructs from real-time in seconds
     * @details Note: when coming from samples, construct from the sample position instead, as this
     * takes sample precision into account. Using seconds = samples/samplerate is not the same as
     * using samples directly.
     */
    explicit Timecode(Framerate framerate,
                      Seconds seconds,
                      RoundingMode roundingMode = RoundingMode::DEFAULT,
                      WrapMode wrapMode         = WrapMode::DEFAULT);

    /**
     * @brief Constructs from DAMF-time expressed in seconds, as used in Dolby DAMF files.
     * @details DAMF-time is a way to represent the timecode value hh:mm:ss:ff value in seconds
     * as (3600 * hh + 60 * mm + ss + ff / framecount) seconds. Note that real-time equals DAMF-time
     * for integer framerates. For non-integer framerates (23.976, 29.97) real-time equals
     * (DAMF-time*1001/1000). This is also commonly known as NTSC-time. However, for drop-frame
     * framerate 29.97DF DAMF-time behaves differently compared to NTSC-time. See README.md for a
     * detailed description of DAMF-time and its relation to real-time and NTSC-time.
     */
    explicit Timecode(Framerate framerate,
                      DAMFSeconds seconds,
                      RoundingMode roundingMode = RoundingMode::DEFAULT,
                      WrapMode wrapMode         = WrapMode::DEFAULT);

    /**
     * @brief Return true if the time code is valid (not initialized with the default constructor).
     * Note that --:--:--:-- / --:--:--;-- is considered valid, but not set.
     */
    bool IsValid() const;

    /**
     * @brief Return true if the time code is valid and set (has a defined framerate and hours,
     * minutes, seconds and frames are all set.)
     */
    bool IsSet() const;

    int GetUnitHours() const
    {
        return mHours;
    }

    int GetUnitMinutes() const
    {
        return mMinutes;
    }

    int GetUnitSeconds() const
    {
        return mSeconds;
    }

    int GetUnitFrames() const
    {
        return mFrames;
    }

    /**
     * @brief Convert to string.
     * @details Write the timecode to a char array in format hh:mm:ss:ff (hh:mm:ss;ff for drop-frame
     * frame rate). Size is the allocated size of str, and includes the trailing null char (minimum
     * 12 for normal frame rates, 13 for frame rates >= 100).
     */
    void ToString(char* str, size_t size) const;

    /**
     * @brief Convert to string.
     * @details Write the timecode to an std::string in format hh:mm:ss:ff (hh:mm:ss;ff for
     * drop-frame frame rate).
     */
    std::string ToString() const;

    /** @brief Convert the timecode to seconds in real-time. */
    Seconds ToSeconds() const;
    /** @brief Convert the timecode to sample position. */
    Samples ToSamples(Samplerate samplerate) const;
    /** @brief Convert the timecode to seconds in damf format, symmetrical with constructor. */
    DAMFSeconds ToDAMFSeconds() const;

    /**
     * @brief Convert the timecode to frames, not counting drop-frames; symmetrical with constructor
     */
    Frames ToFrames() const;

    /** @brief Get the Framerate value used for the timecode */
    const Framerate& GetFramerate() const;

    /** @brief Compare timecode with another timecode (equals). */
    bool operator==(const Timecode& other) const;
    /** @brief Compare timecode with another timecode (not equals). */
    bool operator!=(const Timecode& other) const;
    /** @brief Compare timecode with another timecode (less than). */
    bool operator<(const Timecode& other) const;
    /** @brief Compare timecode with another timecode (greater than). */
    bool operator>(const Timecode& other) const;
    /** @brief Compare timecode with another timecode (less or equal). */
    bool operator<=(const Timecode& other) const;
    /** @brief Compare timecode with another timecode (greater or equal). */
    bool operator>=(const Timecode& other) const;

    /** @brief Increase timecode to the next frame. */
    Timecode& operator++();
    /** @brief Decrease timecode to the previous frame. */
    Timecode& operator--();

private:
    /** @brief Helper function to ensure the comparison operators are used on timecodes with these
     * same frame rate. */
    void RequireMatchingFramerate(const Timecode& other) const;
};

#endif
