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
#include <iomanip>
#include <iostream>
#include <limits> // digits10
#include <regex>
#include <sstream>
#include <string>
#include <tcutils/Convert.h>
#include <tcutils/Framerate.h>
#include <tcutils/Timecode.h>
#include <tcutils/TimecodeSubframes.h>
#include <tcutils/Version.h>

using namespace Dolby::TcUtils;

namespace
{

    template <typename T>
    std::string ToString(T value)
    {
        std::ostringstream s;
        s << value;
        return s.str();
    }

    template <>
    std::string ToString(RoundingMode roundingMode)
    {
        switch (roundingMode)
        {
            case RoundingMode::TRUNCATE:
                return "truncate";
            case RoundingMode::NEAREST:
                return "nearest";
            default:
                break;
        }
        assert(false);
        return "";
    }

    std::string ToInfoString(RoundingMode roundingMode)
    {
        switch (roundingMode)
        {
            case RoundingMode::TRUNCATE:
                return "truncating to frame below";
            case RoundingMode::NEAREST:
                return "rounding to nearest frame";
            default:
                break;
        }
        assert(false);
        return "";
    }

    struct TimeFormatInfo
    {
        enum Enum
        {
            UNDEFINED = -1,
            TIMECODE,
            T_DAMF,
            T_REAL,
            FRAMES,
            SAMPLE,
        };

        Enum formatEnum;
        std::regex re;
        const char* formatString;
        const char* what;
    };

    const TimeFormatInfo timeArgFormatInfo[] = {
        {TimeFormatInfo::TIMECODE,
         std::regex("^([0-9][0-9]:[0-9][0-9]:[0-9][0-9][:;][0-9][0-9])$"),
         "hh:mm:ss:ff",
         "timecode"},
        {TimeFormatInfo::T_REAL,
         std::regex("^t_real=([0-9]+([.][0-9]*)?|[.][0-9]+)$"),
         "t_real=floating-point",
         "real-time in seconds"},
        {TimeFormatInfo::T_DAMF,
         std::regex("^t_damf=([0-9]+([.][0-9]*)?|[.][0-9]+)$"),
         "t_damf=floating-point",
         "DAMF-time in seconds"},
        {TimeFormatInfo::FRAMES, std::regex("^frames=([0-9]+)$"), "frames=integer", "frame count"},
        {TimeFormatInfo::SAMPLE,
         std::regex("^sample=([0-9]+)$"),
         "sample=integer",
         "sample position"},
        {TimeFormatInfo::UNDEFINED, std::regex(""), nullptr, nullptr}};

} // namespace

int main(int argc, char** argv)
{
    // Variables to store parsed arguments, with defaults when no argument is
    // given
    Framerate framerate("24");
    Samplerate samplerate(48000.0);
    RoundingMode roundingMode(RoundingMode::NEAREST);

    std::cout << std::endl;
    std::cout << "Dolby TcUtils tcutility - version " << Version::GetString() << std::endl;
    std::cout << "This utility converts a given time into all formats handled by TcUtils."
              << std::endl
              << std::endl;

    if (argc < 2 ||
        (argc == 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")))
    {
        // show usage when no arguments are given or -h / --help
        size_t maxFormatStringLength = 0;
        for (size_t i = 0; timeArgFormatInfo[i].what != nullptr; i++)
        {
            std::string formatString(timeArgFormatInfo[i].formatString);
            maxFormatStringLength = std::max(maxFormatStringLength, formatString.length());
        }

        std::cout << "Usage: [time] <framerate> <samplerate> <rounding-mode>" << std::endl
                  << std::endl;
        std::cout << "  time is required and can be given in the following formats:" << std::endl
                  << std::endl;

        for (size_t i = 0; timeArgFormatInfo[i].what != nullptr; i++)
        {
            std::string formatString(timeArgFormatInfo[i].formatString);
            std::cout << "    " << formatString
                      << std::string(maxFormatStringLength + 3 - formatString.length(), ' ')
                      << timeArgFormatInfo[i].what << std::endl;
        }

        std::cout << std::endl;
        std::cout << "  framerate, samplerate and rounding-mode are optional" << std::endl
                  << std::endl;
        std::cout << "  framerate can be one of the following:" << std::endl << std::endl;
        for (auto value : Framerate::values())
        {
            std::cout << "    " << value.ToString() << (framerate == value ? " (default)" : "")
                      << std::endl;
        }
        std::cout << std::endl << "  samplerate default is " << samplerate.GetValue() << std::endl;
        std::cout << std::endl
                  << "  rounding-mode can be " << ToString(RoundingMode(RoundingMode::NEAREST))
                  << " and " << ToString(RoundingMode(RoundingMode::TRUNCATE)) << ". Default is "
                  << ToString(roundingMode) << std::endl;
        std::cout << std::endl;

        // exiting
        return 0;
    }

    // first argument is the time, to be parsed below
    std::string timeArg(argv[1]);

    // parse remaining arguments
    bool haveFramerate    = false;
    bool haveSamplerate   = false;
    bool haveRoundingMode = false;
    for (int i = 2; i < argc; i++)
    {
        std::string arg(argv[i]);
        try
        {
            framerate = Framerate(arg);
            if (haveFramerate)
            {
                std::cerr << "Multiple arguments parsed as framerate" << std::endl;
                return -1;
            }
            haveFramerate = true;
        }
        catch (...)
        {
            if (arg == ToString(RoundingMode::NEAREST) || arg == ToString(RoundingMode::TRUNCATE))
            {
                if (haveRoundingMode)
                {
                    std::cerr << "Multiple arguments parsed as rounding-mode" << std::endl;
                    return -1;
                }
                roundingMode     = arg == ToString(RoundingMode::NEAREST) ? RoundingMode::NEAREST
                                                                          : RoundingMode::TRUNCATE;
                haveRoundingMode = true;
            }
            else
            {
                const std::regex sampleRateRe("^([0-9]+([.][0-9]*)?|[.][0-9]+)$");
                if (std::regex_match(arg, sampleRateRe))
                {
                    if (haveSamplerate)
                    {
                        std::cerr << "Multiple arguments parsed as samplerate" << std::endl;
                        return -1;
                    }
                    // assume that an integer or numeric value that isn't framerate is the
                    // samplerate
                    samplerate     = Samplerate(std::stod(arg));
                    haveSamplerate = true;
                }
                else
                {
                    // neither parsable as framerate nor as samplerate nor
                    // nearest|truncate
                    std::cerr << "Invalid argument " << arg << std::endl;
                    return -1;
                }
            }
        }
    }

    // parse the given time value
    TimeFormatInfo timeFormatInfo{TimeFormatInfo::UNDEFINED, std::regex(""), nullptr, nullptr};
    std::string timeArgValue;
    for (size_t i = 0; timeArgFormatInfo[i].what != nullptr; i++)
    {
        std::smatch match;
        if (std::regex_match(timeArg, match, timeArgFormatInfo[i].re))
        {
            timeFormatInfo = timeArgFormatInfo[i];
            timeArgValue   = match[1].str();
        }
    }

    Timecode timecode;
    TimecodeSubframes timecodeSubframes;
    Samples samples{0};
    Seconds realSeconds{0.0};
    DAMFSeconds damfSeconds{0.0};
    Frames frames{0};
    std::string timeArgValueUsed;

    switch (timeFormatInfo.formatEnum)
    {
        case TimeFormatInfo::TIMECODE:
            if (framerate.IsDrop())
            {
                timeArgValue[8] = ';';
            }
            timecode          = Timecode(framerate, timeArgValue);
            timeArgValueUsed  = timecode.ToString();
            realSeconds       = timecode.ToSeconds();
            damfSeconds       = timecode.ToDAMFSeconds();
            samples           = timecode.ToSamples(samplerate);
            timecodeSubframes = TimecodeSubframes(framerate, samples, samplerate, 100);
            frames            = timecode.ToFrames();
            break;
        case TimeFormatInfo::T_DAMF:
        {
            damfSeconds       = DAMFSeconds(std::stod(timeArgValue));
            timeArgValueUsed  = ToString(damfSeconds.GetValue());
            realSeconds       = Convert::ToSeconds(framerate, damfSeconds);
            samples           = Convert::ToSamples(framerate, damfSeconds, samplerate);
            timecode          = Timecode(framerate, damfSeconds, roundingMode);
            timecodeSubframes = TimecodeSubframes(framerate, samples, samplerate, 100);
            frames            = timecode.ToFrames();
            break;
        }
        case TimeFormatInfo::T_REAL:
        {
            realSeconds       = Seconds(std::stod(timeArgValue));
            timeArgValueUsed  = ToString(realSeconds.GetValue());
            damfSeconds       = Convert::ToDAMFSeconds(framerate, realSeconds);
            samples           = Convert::ToSamples(realSeconds, samplerate);
            timecode          = Timecode(framerate, realSeconds, roundingMode);
            timecodeSubframes = TimecodeSubframes(framerate, samples, samplerate, 100);
            frames            = timecode.ToFrames();
            break;
        }
        case TimeFormatInfo::SAMPLE:
        {
            samples           = Samples(std::stoi(timeArgValue));
            timeArgValueUsed  = std::to_string(samples.GetValue());
            realSeconds       = Seconds(samples.GetValue() / samplerate.GetValue());
            damfSeconds       = Convert::ToDAMFSeconds(framerate, samples, samplerate);
            timecode          = Timecode(framerate, samples, samplerate, roundingMode);
            timecodeSubframes = TimecodeSubframes(framerate, samples, samplerate, 100);
            frames            = timecode.ToFrames();
            break;
        }
        case TimeFormatInfo::FRAMES:
        {
            frames            = Frames(std::stoi(timeArgValue));
            timeArgValueUsed  = ToString(frames.GetValue());
            timecode          = Timecode(framerate, frames);
            samples           = timecode.ToSamples(samplerate);
            realSeconds       = timecode.ToSeconds();
            damfSeconds       = timecode.ToDAMFSeconds();
            timecodeSubframes = TimecodeSubframes(framerate, samples, samplerate, 100);
            break;
        }
        case TimeFormatInfo::UNDEFINED:
            std::cerr << "Failed to parse the time argument" << std::endl;
            return -1;
    }

    constexpr int maxPrecision{std::numeric_limits<double>::digits10 + 1};

    std::cout << timeFormatInfo.what << " " << timeArgValueUsed;
    std::cout << " at framerate " << framerate.ToString();
    std::cout << " and samplerate " << samplerate.GetValue() << std::endl;
    std::cout << "corresponds with" << std::endl << std::endl;
    std::cout << " timecode: " << timecode.ToString() << " (" << ToInfoString(roundingMode) << ")"
              << std::endl;
    std::cout << "           " << timecodeSubframes.ToString(true) << " "
              << "(subframes)" << std::endl;
    std::cout << std::setprecision(maxPrecision) << std::fixed;
    std::cout << "   t_real: " << realSeconds.GetValue() << std::endl;
    std::cout << "   t_damf: " << damfSeconds.GetValue() << std::endl;
    std::cout << "   sample: " << samples.GetValue() << std::endl;
    std::cout << "   frames: " << frames.GetValue() << std::endl << std::endl;

    return 0;
}
