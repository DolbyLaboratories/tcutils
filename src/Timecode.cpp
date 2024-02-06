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

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <tcutils/Timecode.h>

#include "DropFrameHelpers.h"

using namespace Dolby::TcUtils;

struct Timecode::TimeParameters
{
    int mHours;
    int mMinutes;
    int mSeconds;
    int mFrames;

    TimeParameters(Framerate framerate, int32_t framesCountingDroppedFrames);
};

Timecode::Timecode()
: mFramerate(Framerate::UNDEFINED)
, mWrapMode(WrapMode::DEFAULT)
, mHours(-1)
, mMinutes(-1)
, mSeconds(-1)
, mFrames(-1)
{
}

Timecode::Timecode(Framerate framerate, WrapMode wrapMode)
: mFramerate(framerate)
, mWrapMode(wrapMode)
, mHours(-1)
, mMinutes(-1)
, mSeconds(-1)
, mFrames(-1)
{
}

namespace
{
    // Local helper functions

    int SkipDropFrames(Framerate framerate, int minutes, int seconds, int frames)
    {
        return (framerate.IsDrop() && seconds == 0 && (frames == 0 || frames == 1) &&
                (minutes % 10) != 0)
                   ? 2
                   : frames;
    }

    Samples CalculateSamples(Framerate framerate, Frames frames, Samplerate samplerate)
    {
        const double samplesPerFrame =
            samplerate.GetValue() / static_cast<double>(framerate.GetFrameCount());
        const double dsamples = static_cast<double>(frames.GetValue()) * samplesPerFrame;
        return Samples(static_cast<int64_t>(std::llround(framerate.ApplyInverseRatio(dsamples))));
    }

    Frames CalculateFrames(Framerate framerate,
                           Samples samples,
                           Samplerate samplerate,
                           RoundingMode roundingMode)
    {
        if (framerate == Framerate::UNDEFINED || samples.GetValue() < 0)
        {
            return Frames(-1);
        }

        const double dsamples = framerate.ApplyRatio(static_cast<double>(samples.GetValue()) + 0.5);
        const double framesPerSample =
            static_cast<double>(framerate.GetFrameCount()) / samplerate.GetValue();
        const auto frames = static_cast<int32_t>(dsamples * framesPerSample);

        switch (roundingMode)
        {
            case RoundingMode::TRUNCATE:
            {
                return Frames(frames);
            }
            case RoundingMode::NEAREST:
            {
                int64_t samplesFloor =
                    CalculateSamples(framerate, Frames(frames), samplerate).GetValue();
                int64_t samplesCeiling =
                    CalculateSamples(framerate, Frames(frames + 1), samplerate).GetValue();
                return (samples.GetValue() - samplesFloor < samplesCeiling - samples.GetValue())
                           ? Frames(frames)
                           : Frames(frames + 1);
            }
            default:
                assert(false);
                break;
        }
        return Frames(-1);
    }

    Frames CalculateFrames(Framerate framerate, Seconds seconds, RoundingMode roundingMode)
    {
        if (framerate == Framerate::UNDEFINED || seconds.GetValue() < 0.0)
        {
            return Frames(-1);
        }

        const double secondsRatioAdjusted = framerate.ApplyRatio(seconds.GetValue());

        switch (roundingMode)
        {
            case RoundingMode::NEAREST:
            {
                return Frames(static_cast<int32_t>(
                    std::lround(secondsRatioAdjusted * framerate.GetFrameCount())));
            }
            case RoundingMode::TRUNCATE:
            {
                const auto dframeCount = static_cast<double>(framerate.GetFrameCount());
                const auto frames      = static_cast<int32_t>(secondsRatioAdjusted * dframeCount);

                // Due to rounding errors, it is possible that going from Timecode to seconds and
                // back will leave us at the previous frame. By reconverting the next frame back to
                // seconds, we can test for this situation and return the expected timecode,
                // effectively making this function symmetrical.
                const double invSeconds =
                    framerate.ApplyInverseRatio(static_cast<double>(frames + 1) / dframeCount);
                if (invSeconds == seconds.GetValue())
                {
                    return Frames(frames + 1);
                }

                return Frames(frames);
            }
            default:
                assert(false);
                break;
        }
        return Frames(-1);
    }

    int32_t CalculateFramesCountingDroppedFrames(Framerate framerate,
                                                 DAMFSeconds seconds,
                                                 RoundingMode roundingMode)
    {
        if (framerate == Framerate::UNDEFINED || seconds.GetValue() < 0.0)
        {
            return -1;
        }

        const double framesCountingDroppedFrames =
            seconds.GetValue() * static_cast<double>(framerate.GetFrameCount());

        switch (roundingMode)
        {
            case RoundingMode::NEAREST:
            {
                return static_cast<int32_t>(std::lround(framesCountingDroppedFrames));
            }
            case RoundingMode::TRUNCATE:
            {
                const auto frames = static_cast<int32_t>(framesCountingDroppedFrames);

                // Due to rounding errors, it is possible that going from Timecode to seconds and
                // back will leave us at the previous frame. By reconverting the next frame back to
                // seconds, we can test for this situation and return the expected timecode,
                // effectively making this function symmetrical.
                const double invSeconds = static_cast<double>(frames + 1) /
                                          static_cast<double>(framerate.GetFrameCount());
                if (invSeconds == seconds.GetValue())
                {
                    return frames + 1;
                }
                return frames;
            }
            default:
                assert(false);
                break;
        }
        return -1;
    }

}

Timecode::TimeParameters::TimeParameters(Framerate framerate, int32_t framesCountingDroppedFrames)
{
    if (framesCountingDroppedFrames == -1)
    {
        mHours   = -1;
        mMinutes = -1;
        mSeconds = -1;
        mFrames  = -1;
        return;
    }

    const int framesPerTCSecond = framerate.GetFrameCount();
    const int framesPerMinute   = framesPerTCSecond * 60;
    const int framesPerHour     = framesPerMinute * 60;

    mHours = framesCountingDroppedFrames / framesPerHour;
    framesCountingDroppedFrames -= mHours * framesPerHour;
    mMinutes = framesCountingDroppedFrames / framesPerMinute;
    framesCountingDroppedFrames -= mMinutes * framesPerMinute;
    mSeconds = framesCountingDroppedFrames / framesPerTCSecond;
    framesCountingDroppedFrames -= mSeconds * framesPerTCSecond;
    mFrames = SkipDropFrames(framerate, mMinutes, mSeconds, framesCountingDroppedFrames);
}

Timecode::Timecode(Framerate framerate, const Timecode::TimeParameters& params, WrapMode wrapMode)
: Timecode(framerate, params.mHours, params.mMinutes, params.mSeconds, params.mFrames, wrapMode)
{
}

Timecode::Timecode(Framerate framerate,
                   int hours,
                   int minutes,
                   int seconds,
                   int frames,
                   WrapMode wrapMode)
: mFramerate(framerate)
, mWrapMode(wrapMode)
, mHours(wrapMode == WrapMode::WRAP_AT_MIDNIGHT && hours >= 0 ? hours % 24 : hours)
, mMinutes(minutes)
, mSeconds(seconds)
, mFrames(frames)
{
    if ((mHours == -1 || mMinutes == -1 || mSeconds == -1 || mFrames == -1) &&
        (mHours != -1 || mMinutes != -1 || mSeconds != -1 || mFrames != -1))
    {
        // either all or none of the fields should be set to -1
        // throw NegativeValues();
    }
    if (mHours >= 100 || mMinutes >= 60 || mSeconds >= 60 || mFrames >= mFramerate.GetFrameCount())
    {
        throw ValueOutOfRange();
    }
    if (mFramerate.IsDrop() && mSeconds == 0 && (mFrames == 0 || mFrames == 1) &&
        (mMinutes % 10) != 0)
    {
        throw InvalidDropFrame();
    }
}

Timecode::Timecode(Framerate framerate, Frames frames, WrapMode wrapMode)
: Timecode(
      frames.GetValue() == -1 ? Framerate() : framerate,
      Timecode::TimeParameters(framerate,
                               framerate.IsDrop() || frames.GetValue() == -1
                                   ? frames.GetValue() + dropFramesToAddForFrames(frames.GetValue())
                                   : frames.GetValue()),
      wrapMode)
{
}
Timecode::Timecode(Framerate framerate,
                   Seconds seconds,
                   RoundingMode roundingMode,
                   WrapMode wrapMode)
: Timecode(framerate, CalculateFrames(framerate, seconds, roundingMode), wrapMode)
{
}

Timecode::Timecode(Framerate framerate,
                   DAMFSeconds seconds,
                   RoundingMode roundingMode,
                   WrapMode wrapMode)
: Timecode(framerate,
           TimeParameters(framerate,
                          CalculateFramesCountingDroppedFrames(framerate, seconds, roundingMode)),
           wrapMode)
{
}

Timecode::Timecode(Framerate framerate,
                   Samples samples,
                   Samplerate samplerate,
                   RoundingMode roundingMode,
                   WrapMode wrapMode)
: Timecode(framerate, CalculateFrames(framerate, samples, samplerate, roundingMode), wrapMode)
{
}

namespace
{
    int Parse2Digits(const char* str, char term)
    {
        if (str[0] == '-' && str[1] == '-' && str[2] == term)
        {
            return -1;
        }
        if (str[0] >= '0' && str[0] <= '9' && str[1] >= '0' && str[1] <= '9' && str[2] == term)
        {
            return (str[0] - '0') * 10 + (str[1] - '0');
        }

        throw Timecode::StringParseError();
    }
    int Parse3Digits(const char* str, char term)
    {
        if (str[0] == '-' && str[1] == '-' && str[2] == '-' && str[3] == term)
        {
            return -1;
        }
        if (str[0] >= '0' && str[0] <= '9' && str[1] >= '0' && str[1] <= '9' && str[2] >= '0' &&
            str[2] <= '9' && str[3] == term)
        {
            return (str[0] - '0') * 100 + (str[1] - '0') * 10 + (str[2] - '0');
        }

        throw Timecode::StringParseError();
    }
}

Timecode::Timecode(Framerate framerate, const char* str, WrapMode wrapMode)
: Timecode(framerate,
           Parse2Digits(str + 0, ':'),
           Parse2Digits(str + 3, ':'),
           Parse2Digits(str + 6, framerate.IsDrop() ? ';' : ':'),
           framerate.GetFrameCount() > 100 ? Parse3Digits(str + 9, '\0')
                                           : Parse2Digits(str + 9, '\0'),
           wrapMode)
{
}

Timecode::Timecode(Framerate framerate, const std::string& str, WrapMode wrapMode)
: Timecode(framerate, str.c_str(), wrapMode)
{
}

Seconds Timecode::ToSeconds() const
{
    const double seconds = static_cast<double>(ToFrames().GetValue()) /
                           static_cast<double>(mFramerate.GetFrameCount());
    return Seconds(mFramerate.ApplyInverseRatio(seconds));
}

Samples Timecode::ToSamples(Samplerate samplerate) const
{
    return CalculateSamples(mFramerate, ToFrames(), samplerate);
}

bool Timecode::IsValid() const
{
    return mFramerate != Framerate::UNDEFINED;
}

bool Timecode::IsSet() const
{
    return mHours != -1 && mMinutes != -1 && mSeconds != -1 && mFrames != -1;
}

void Timecode::ToString(char* str, size_t size) const
{
    const bool frames3Digits = mFramerate.GetFrameCount() > 100;
    if (frames3Digits && size < 13)
    {
        throw std::runtime_error(
            "time code to string conversion for frame rate > 100 fps requires 12 chars");
    }
    if (size < 12)
    {
        throw std::runtime_error("time code to string conversion for requires 12 chars");
    }

    const bool useHypen = !IsSet();

    str[0] = useHypen ? '-' : '0' + (mHours / 10);
    str[1] = useHypen ? '-' : '0' + (mHours % 10);

    str[2] = ':';

    str[3] = useHypen ? '-' : '0' + (mMinutes / 10);
    str[4] = useHypen ? '-' : '0' + (mMinutes % 10);

    str[5] = ':';

    str[6] = useHypen ? '-' : '0' + (mSeconds / 10);
    str[7] = useHypen ? '-' : '0' + (mSeconds % 10);

    str[8] = mFramerate.IsDrop() ? ';' : ':';

    if (frames3Digits)
    {
        str[9]  = useHypen ? '-' : '0' + (mFrames / 100);
        str[10] = useHypen ? '-' : '0' + ((mFrames / 10) % 10);
        str[11] = useHypen ? '-' : '0' + (mFrames % 10);
        str[12] = 0;
    }
    else
    {
        str[9]  = useHypen ? '-' : '0' + (mFrames / 10);
        str[10] = useHypen ? '-' : '0' + (mFrames % 10);
        str[11] = 0;
    }
}

std::string Timecode::ToString() const
{
    char tmp[13];
    ToString(tmp, 13);
    return {tmp};
}

DAMFSeconds Timecode::ToDAMFSeconds() const
{
    if (mFramerate == Framerate::UNDEFINED || mHours == -1 || mMinutes == -1 || mSeconds == -1 ||
        mFrames == -1)
    {
        return DAMFSeconds(-1.);
    }

    const int32_t framesPerSecond = mFramerate.GetFrameCount();
    const int32_t framesPerMinute = framesPerSecond * 60;
    const int32_t framesPerHour   = framesPerMinute * 60;

    const int32_t frames =
        mHours * framesPerHour + mMinutes * framesPerMinute + mSeconds * framesPerSecond + mFrames;

    return DAMFSeconds(static_cast<double>(frames) / static_cast<double>(framesPerSecond));
}

Frames Timecode::ToFrames() const
{
    const int32_t frames =
        (mHours * 3600 + mMinutes * 60 + mSeconds) * mFramerate.GetFrameCount() + mFrames;
    if (mFramerate.IsDrop())
    {
        const int32_t mins = mHours * 60 + mMinutes;
        return Frames(frames - dropFramesToRemoveForMinutes(mins));
    }
    return Frames(frames);
}

const Framerate& Timecode::GetFramerate() const
{
    return mFramerate;
}

void Timecode::RequireMatchingFramerate(const Timecode& other) const
{
    if (mFramerate == other.mFramerate)
        return;

    throw std::logic_error("Operation requires timecodes with matching framerates");
}

bool Timecode::operator==(const Timecode& other) const
{
    return mFramerate == other.mFramerate && mHours == other.mHours && mMinutes == other.mMinutes &&
           mSeconds == other.mSeconds && mFrames == other.mFrames;
}

bool Timecode::operator!=(const Timecode& other) const
{
    return mFramerate != other.mFramerate || mHours != other.mHours || mMinutes != other.mMinutes ||
           mSeconds != other.mSeconds || mFrames != other.mFrames;
}

bool Timecode::operator<(const Timecode& other) const
{
    RequireMatchingFramerate(other);

    return mHours < other.mHours ||
           (mHours == other.mHours && (mMinutes < other.mMinutes ||
                                       (mMinutes == other.mMinutes &&
                                        (mSeconds < other.mSeconds || (mSeconds == other.mSeconds &&
                                                                       mFrames < other.mFrames)))));
}

bool Timecode::operator>(const Timecode& other) const
{
    RequireMatchingFramerate(other);

    return mHours > other.mHours ||
           (mHours == other.mHours && (mMinutes > other.mMinutes ||
                                       (mMinutes == other.mMinutes &&
                                        (mSeconds > other.mSeconds || (mSeconds == other.mSeconds &&
                                                                       mFrames > other.mFrames)))));
}

bool Timecode::operator<=(const Timecode& other) const
{
    RequireMatchingFramerate(other);

    return !(*this > other);
}

bool Timecode::operator>=(const Timecode& other) const
{
    RequireMatchingFramerate(other);

    return !(*this < other);
}

Timecode& Timecode::operator++()
{
    mFrames++;

    if (mFrames == mFramerate.GetFrameCount())
    {
        mFrames = 0;
        mSeconds++;
        if (mSeconds == 60)
        {
            mSeconds = 0;
            mMinutes++;
            if (mMinutes == 60)
            {
                mMinutes = 0;
                if (mWrapMode == WrapMode::WRAP_AT_MIDNIGHT)
                {
                    mHours = (mHours + 1) % 24;
                }
                else if (mHours < 99)
                {
                    mHours++;
                }
                else
                {
                    throw std::runtime_error("Hour value out of range");
                }
            }
        }
    }
    if (mFramerate.IsDrop() && (mFrames == 0 || mFrames == 1) && mSeconds == 0 &&
        (mMinutes % 10) != 0)
    {
        mFrames = 2;
    }
    return *this;
}

Timecode& Timecode::operator--()
{
    if (mWrapMode != WrapMode::WRAP_AT_MIDNIGHT && mHours == 0 && mSeconds == 0 && mMinutes == 0 &&
        mFrames == 0)
    {
        throw std::runtime_error("Can't go below frame zero in continuous mode");
    }
    mFrames--;
    if (mFramerate.IsDrop() && (mFrames == 0 || mFrames == 1) && mSeconds == 0 &&
        (mMinutes % 10) != 0)
    {
        mFrames = -1;
    }
    if (mFrames == -1)
    {
        mFrames = mFramerate.GetFrameCount() - 1;
        mSeconds--;
        if (mSeconds == -1)
        {
            mSeconds = 59;
            mMinutes--;
            if (mMinutes == -1)
            {
                mMinutes = 59;
                mHours--;
                if (mHours == -1)
                {
                    mHours = 23;
                }
            }
        }
    }
    return *this;
}

namespace Dolby
{
    namespace TcUtils
    {
        std::ostream& operator<<(std::ostream& os, const Timecode& tc)
        {
            os << tc.ToString();
            return os;
        }
    }
}
