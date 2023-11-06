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

#include <gtest/gtest.h>
#include <tcutils/TimecodeSubframes.h>

using namespace testing;
using namespace Dolby::TcUtils;

class TimecodeSubframesTests : public Test
{
};

namespace
{
    int64_t CalcTruncatedFrameLengthInSamples(Samplerate sr, Framerate videoFramerate)
    {
        const int64_t iSamplerate = static_cast<int64_t>(sr.GetValue() + 0.5);
        const int64_t ratio       = videoFramerate.IsRatio1001() ? 1001 : 1000;
        const int64_t iFrameCount = videoFramerate.GetFrameCount();
        const int64_t frameLength = (iSamplerate * ratio) / (iFrameCount * 1000);
        return frameLength;
    }
}

TEST_F(TimecodeSubframesTests, SubframeBasicCases)
{
    auto toFrames = [](TimecodeSubframes t) { return t.GetTimecode().ToFrames().GetValue(); };

    const Framerate fr(24);
    const Samplerate sr(48000);
    const int samplesPerFrame = 48000 / 24;
    const int denom           = 100;

    TimecodeSubframes sample1(fr, Samples(1), sr, denom);
    ASSERT_EQ(0, sample1.GetSubframesNum());
    ASSERT_EQ(0, toFrames(sample1));

    TimecodeSubframes frameMinus1Sample(fr, Samples(samplesPerFrame - 1), sr, denom);
    ASSERT_EQ(0, frameMinus1Sample.GetSubframesNum());
    ASSERT_EQ(1, toFrames(frameMinus1Sample));

    // test that we change to numerator 1 at samplesPerFrame / denom / 2
    // that is: mid-subframe rounds up
    TimecodeSubframes midSubframeMinus1Sample(
        fr, Samples(samplesPerFrame / denom / 2 - 1), sr, denom);
    TimecodeSubframes midSubframe(fr, Samples(samplesPerFrame / denom / 2), sr, denom);
    ASSERT_EQ(0, midSubframeMinus1Sample.GetSubframesNum());
    ASSERT_EQ(0, toFrames(midSubframeMinus1Sample));
    ASSERT_EQ(1, midSubframe.GetSubframesNum());
    ASSERT_EQ(0, toFrames(midSubframe));
}

TEST_F(TimecodeSubframesTests, SubframeContinuity)
{
    std::vector<int> denoms{10, 80, 100};

    for (auto denom : denoms)
    {
        for (auto framerate : Framerate::values())
        {
            const Samplerate sr(48000.0);
            const int64_t frameLengthSamples = CalcTruncatedFrameLengthInSamples(sr, framerate);

            TimecodeSubframes prev;
            int32_t frameStart = 0;
            for (int64_t i = 0; i < 48000 * 3; i++)
            {
                TimecodeSubframes result(framerate, Samples(i), sr, denom);
                if (i != 0)
                {
                    if (result.GetSubframesNum() < prev.GetSubframesNum())
                    {
                        Timecode tc = prev.GetTimecode();
                        ++tc;
                        ASSERT_EQ(result.GetTimecode(), tc);
                        ASSERT_EQ(denom - 1, prev.GetSubframesNum());
                    }
                    else if (result.GetTimecode() != prev.GetTimecode())
                    {
                        ASSERT_GE(i - frameStart, frameLengthSamples);
                        ASSERT_LE(i - frameStart, frameLengthSamples + 1);
                        frameStart = i;
                    }
                    else
                    {
                        ASSERT_EQ(result.GetTimecode(), prev.GetTimecode());
                        ASSERT_GE(result.GetSubframesNum(), prev.GetSubframesNum());
                        ASSERT_LE(result.GetSubframesNum(), prev.GetSubframesNum() + 1);
                    }
                }
                prev = result;
            }
        }
    }
}

TEST_F(TimecodeSubframesTests, SubframeComplementary)
{
    std::vector<int> denoms{10, 80, 100};
    for (auto denom : denoms)
    {
        for (auto framerate : Framerate::values())
        {
            const Samplerate sr(48000.0);
            const Timecode totalTime(framerate, 0, 0, 2, 0);
            const int64_t totalSamples       = totalTime.ToSamples(sr).GetValue();
            const int64_t expectedNumerators = totalTime.ToFrames().GetValue() * denom;

            // we expect an exact match if the amount of samples per subframe is integer and even
            const bool subframeLengthIsInteger =
                !framerate.IsRatio1001() &&
                (48000 / framerate.GetFrameCount() / denom) * framerate.GetFrameCount() * denom ==
                    48000;
            const bool subframeLengthIsEven =
                subframeLengthIsInteger && ((48000 / framerate.GetFrameCount() / denom) % 2) == 0;
            const bool exactMatch = subframeLengthIsInteger && subframeLengthIsEven;

            for (int i = 0; i < totalSamples; i++)
            {
                TimecodeSubframes subframe(framerate, Samples(i), sr, denom);
                TimecodeSubframes compSubframe(framerate, Samples(totalSamples - i - 1), sr, denom);

                auto toNumeratorCount = [=](const TimecodeSubframes& subframe) {
                    return subframe.GetSubframesNum() +
                           subframe.GetTimecode().ToFrames().GetValue() * denom;
                };
                auto numeratorsSum = toNumeratorCount(subframe) + toNumeratorCount(compSubframe);

                if (exactMatch)
                {
                    ASSERT_EQ(expectedNumerators, numeratorsSum);
                }
                else
                {
                    ASSERT_GE(numeratorsSum, expectedNumerators - 1);
                    ASSERT_LE(numeratorsSum, expectedNumerators);
                }
            }
        }
    }
}

TEST_F(TimecodeSubframesTests, ToString)
{
    std::vector<int> denoms{10, 80, 100};

    for (auto denom : denoms)
    {
        for (auto framerate : Framerate::values())
        {
            const Samplerate sr(48000.0);
            for (int64_t i = 0; i < 48000 * 10; i += 17)
            {
                TimecodeSubframes result(framerate, Samples(i), sr, denom);
                std::string s = result.ToString();
                char tmp[3]   = {0, 0, 0};
                if (denom <= 10)
                {
                    tmp[0] = char('0' + result.GetSubframesNum());
                }
                else
                {
                    tmp[0] = char('0' + (result.GetSubframesNum() / 10));
                    tmp[1] = char('0' + (result.GetSubframesNum() % 10));
                }
                size_t n = result.GetTimecode().ToString().length();
                ASSERT_EQ(s.substr(0, n), result.GetTimecode().ToString());
                ASSERT_EQ(s.substr(n + 1), std::string(tmp));
            }
        }
    }
}
