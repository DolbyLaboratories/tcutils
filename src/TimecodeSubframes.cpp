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
#include <iostream>
#include <locale>
#include <tcutils/FrameBoundary.h>
#include <tcutils/TimecodeSubframes.h>

using namespace Dolby::TcUtils;

TimecodeSubframes::TimecodeSubframes()
: mSubframesNum(0)
, mSubframesDenom(0)
{
}

TimecodeSubframes::TimecodeSubframes(Timecode timecode, int subframesNum, int subframesDenom)
: mTimecode(timecode)
, mSubframesNum(subframesNum)
, mSubframesDenom(subframesDenom)
{
    assert(mSubframesNum >= 0);
    assert(mSubframesDenom > 1);
    assert(mSubframesNum < subframesDenom);
}

namespace
{
    int CalculateSubframesNum(Framerate framerate,
                              Samples samples,
                              Samplerate samplerate,
                              int subframesDenom)

    {
        const int64_t samplesFloor =
            FrameBoundary::Floor(framerate, samples, samplerate).GetValue();
        const int64_t sampleOffset = samples.GetValue() - samplesFloor;
        const Samples nextSample(samples.GetValue() + 1);
        const int64_t samplesCeiling =
            FrameBoundary::Ceiling(framerate, nextSample, samplerate).GetValue();
        const int64_t samplesInCurrentFrame =
            samplesCeiling - samplesFloor; // on some framerates, the number of samples per frame
                                           // can vary slightly from frame to frame

        // The return expression is equivalent to:
        // return std::round(float(subframesDenom * sampleOffset) / float(samplesInCurrentFrame));
        // but expressed in integer arithmetic
        return (subframesDenom * sampleOffset * 2 + samplesInCurrentFrame) /
               (samplesInCurrentFrame * 2);
    }
}

TimecodeSubframes::TimecodeSubframes(Framerate framerate,
                                     Samples samples,
                                     Samplerate samplerate,
                                     int subframesDenom)
: mTimecode(framerate, samples, samplerate, RoundingMode::TRUNCATE)
, mSubframesNum(CalculateSubframesNum(framerate, samples, samplerate, subframesDenom))
, mSubframesDenom(subframesDenom)
{
    if (mSubframesNum == mSubframesDenom)
    {
        mSubframesNum = 0;
        ++mTimecode;
    }
}

namespace
{
    char decimal_point()
    {
        static char c = std::use_facet<std::numpunct<char>>(std::cout.getloc()).decimal_point();
        return c;
    }
}

std::string TimecodeSubframes::ToString(bool showZeroSubframes) const
{
    std::string s;
    if (mSubframesDenom < 2 || (mSubframesNum == 0 && !showZeroSubframes))
    {
        return mTimecode.ToString();
    }
    // Fill the numerator with the needed 0's
    int i = mSubframesDenom - 1;
    int j = 1;
    while (i != 0)
    {
        i /= 10;
        j *= 10;
    }
    // j is 10^n where 10^n is the smaller number >= mSubframeDenom
    i = mSubframesNum;
    while (j != 1)
    {
        j /= 10;
        int k = i / j;
        s += char('0' + k);
        i -= j * k;
    }
    return mTimecode.ToString() + decimal_point() + s;
}
