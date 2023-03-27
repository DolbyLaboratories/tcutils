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
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <tcutils/Framerate.h>

#include "DropFrameHelpers.h"

using namespace Dolby::TcUtils;

namespace
{
    Framerate::Enum ToEnum(const char* str)
    {
        if (std::strcmp("23.976", str) == 0)
            return Framerate::FPS_23976;
        if (std::strcmp("24", str) == 0)
            return Framerate::FPS_24;
        if (std::strcmp("25", str) == 0)
            return Framerate::FPS_25;
        if (std::strcmp("29.97", str) == 0)
            return Framerate::FPS_2997;
        if (std::strcmp("29.97df", str) == 0)
            return Framerate::FPS_2997DF;
        if (std::strcmp("29.97DF", str) == 0)
            return Framerate::FPS_2997DF;
        if (std::strcmp("29.97 df", str) == 0)
            return Framerate::FPS_2997DF;
        if (std::strcmp("29.97 DF", str) == 0)
            return Framerate::FPS_2997DF;
        if (std::strcmp("30", str) == 0)
            return Framerate::FPS_30;
        if (std::strcmp("30df", str) == 0)
            return Framerate::FPS_30DF;
        if (std::strcmp("30DF", str) == 0)
            return Framerate::FPS_30DF;
        if (std::strcmp("30 df", str) == 0)
            return Framerate::FPS_30DF;
        if (std::strcmp("30 DF", str) == 0)
            return Framerate::FPS_30DF;
        if (std::strcmp("48", str) == 0)
            return Framerate::FPS_48;
        if (std::strcmp("50", str) == 0)
            return Framerate::FPS_50;
        if (std::strcmp("60", str) == 0)
            return Framerate::FPS_60;
        if (std::strcmp("96", str) == 0)
            return Framerate::FPS_96;
        if (std::strcmp("100", str) == 0)
            return Framerate::FPS_100;
        if (std::strcmp("120", str) == 0)
            return Framerate::FPS_120;

        throw Framerate::StringParseError();
    }

    bool isNear(double value, double ref)
    {
        const double e = 0.0001;
        return std::abs(value - ref) < e;
    }

    Framerate::Enum ToEnum(double value, bool isDrop)
    {
        if (isDrop)
        {
            if (isNear(value, 29.97))
                return Framerate::FPS_2997DF;
            if (isNear(value, 30.0))
                return Framerate::FPS_30DF;
            throw std::runtime_error("unsupported drop framerate");
        }
        if (isNear(value, 23.976))
            return Framerate::FPS_23976;
        if (isNear(value, 24))
            return Framerate::FPS_24;
        if (isNear(value, 25))
            return Framerate::FPS_25;
        if (isNear(value, 29.97))
            return Framerate::FPS_2997;
        if (isNear(value, 30))
            return Framerate::FPS_30;
        if (isNear(value, 48))
            return Framerate::FPS_48;
        if (isNear(value, 50))
            return Framerate::FPS_50;
        if (isNear(value, 60))
            return Framerate::FPS_60;
        if (isNear(value, 96))
            return Framerate::FPS_96;
        if (isNear(value, 100))
            return Framerate::FPS_100;
        if (isNear(value, 120))
            return Framerate::FPS_120;
        throw std::runtime_error("unsupported framerate");
    }

}

Framerate::Framerate(const char* str)
: mValue(ToEnum(str))
{
}

Framerate::Framerate(const std::string& str)
: Framerate(str.c_str())
{
}

Framerate::Framerate(double value)
: mValue(ToEnum(value, false))
{
}

Framerate::Framerate(double value, bool isDrop)
: mValue(ToEnum(value, isDrop))
{
}

int Framerate::GetFrameCount() const
{
    switch (mValue)
    {
        case UNDEFINED:
            return 0;
        case FPS_23976:
            return 24;
        case FPS_24:
            return 24;
        case FPS_25:
            return 25;
        case FPS_2997:
            return 30;
        case FPS_2997DF:
            return 30;
        case FPS_30:
            return 30;
        case FPS_30DF:
            return 30;
        case FPS_48:
            return 48;
        case FPS_50:
            return 50;
        case FPS_60:
            return 60;
        case FPS_96:
            return 96;
        case FPS_100:
            return 100;
        case FPS_120:
            return 120;
        case COUNT:
            return 0;
    }
    return 0;
}

const char* Framerate::ToString() const
{
    switch (mValue)
    {
        case UNDEFINED:
            return "";
        case FPS_23976:
            return "23.976";
        case FPS_24:
            return "24";
        case FPS_25:
            return "25";
        case FPS_2997:
            return "29.97";
        case FPS_2997DF:
            return "29.97df";
        case FPS_30:
            return "30";
        case FPS_30DF:
            return "30df";
        case FPS_48:
            return "48";
        case FPS_50:
            return "50";
        case FPS_60:
            return "60";
        case FPS_96:
            return "96";
        case FPS_100:
            return "100";
        case FPS_120:
            return "120";
        case COUNT:
            return "";
    }
    return "";
}

bool Framerate::IsDefined() const
{
    return mValue != UNDEFINED;
}

bool Framerate::IsDrop() const
{
    return mValue == FPS_2997DF || mValue == FPS_30DF;
}

bool Framerate::IsHighFramerate() const
{
    return mValue == FPS_48 || mValue == FPS_50 || mValue == FPS_60 || mValue == FPS_96 ||
           mValue == FPS_100 || mValue == FPS_120;
}

bool Framerate::IsRatio1001() const
{
    switch (mValue)
    {
        case UNDEFINED:
            return false;
        case FPS_23976:
            return true;
        case FPS_24:
            return false;
        case FPS_25:
            return false;
        case FPS_2997:
            return true;
        case FPS_2997DF:
            return true;
        case FPS_30:
            return false;
        case FPS_30DF:
            return false;
        case FPS_48:
            return false;
        case FPS_50:
            return false;
        case FPS_60:
            return false;
        case FPS_96:
            return false;
        case FPS_100:
            return false;
        case FPS_120:
            return false;
        case COUNT:
            return false;
    }
    return false;
}

double Framerate::ApplyRatio(double value) const
{
    return IsRatio1001() ? value * 1000.0 / 1001.0 : value;
}

double Framerate::ApplyInverseRatio(double value) const
{
    return IsRatio1001() ? value * 1001.0 / 1000.0 : value;
}

using Iterator = Framerate::Iterator;
using Range    = Framerate::Range;

/* static */ Range Framerate::values()
{
    return Range();
}

Iterator::Iterator(Framerate::Enum value)
: mValue(static_cast<int>(value))
{
}

Framerate Iterator::operator*() const
{
    return Framerate(static_cast<Enum>(mValue));
}

void Iterator::operator++()
{
    ++mValue;
}

bool Iterator::operator!=(const Iterator& other) const
{
    return mValue != other.mValue;
}

Range::Range()
: mBegin(UNDEFINED)
, mEnd(COUNT)
{
    ++mBegin;
}

namespace Dolby
{
    namespace TcUtils
    {
        std::ostream& operator<<(std::ostream& os, const Framerate& framerate)
        {
            os << framerate.ToString();
            return os;
        }
    }
}
