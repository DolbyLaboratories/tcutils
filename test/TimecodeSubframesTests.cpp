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
                    if (result.GetSubframesNum() == 0)
                    {
                        Timecode tc = prev.GetTimecode();
                        ++tc;
                        ASSERT_EQ(result.GetTimecode(), tc);
                        ASSERT_GE(i - frameStart, frameLengthSamples);
                        ASSERT_LE(i - frameStart, frameLengthSamples + 1);
                        ASSERT_EQ(denom - 1, prev.GetSubframesNum());
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
            TimecodeSubframes results[2400];
            int64_t frameStart = 0;
            for (int64_t i = 0; i < 48000 * 3; i++)
            {
                TimecodeSubframes result(framerate, Samples(i), sr, denom);
                int64_t j = i - frameStart;
                ASSERT_LT(j, 2400);
                if (result.GetSubframesNum() == 0)
                {
                    if (i != 0)
                    {
                        for (int k = 1; k < j; k++)
                        {
                            ASSERT_EQ(denom,
                                      results[k].GetSubframesNum() +
                                          results[j - k].GetSubframesNum());
                        }
                    }
                    frameStart = i;
                }
                results[j] = result;
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
