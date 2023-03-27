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

#include "FullStressTest.h"
#include <gtest/gtest.h>
#include <tcutils/TimecodeSamples.h>

using namespace testing;
using namespace Dolby::TcUtils;

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

class TimecodeSamplesTests : public Test
{
};

TEST_F(TimecodeSamplesTests, NextSampleInFrameRemainder)
{
    for (auto framerate : Framerate::values())
    {
        const Samplerate sr(48000.0);

        if (FullStressTest::Active())
        {
            std::cout << framerate.ToString() << std::endl;
        }
        Timecode tc(framerate, 0, 0, 0, 0);
        Timecode tcEnd(framerate, 0, 0, 0, 0); // end after wrapping 24 hours
        int i = 0;
        do
        {
            if (FullStressTest::Active() || i < 10000 || (i % 173) == 0)
            {
                Samples nextSampleInFrame = Samples(tc.ToSamples(sr).GetValue() + 1);
                TimecodeSamples tcSamples(framerate, tc.ToSamples(sr), sr);
                TimecodeSamples tcSamplesNextSampleInFrame(framerate, nextSampleInFrame, sr);
                ASSERT_EQ(tcSamples.GetTimecode(), tcSamplesNextSampleInFrame.GetTimecode());
                ASSERT_EQ(tcSamples.GetRemainder(), Samples(0));
                ASSERT_EQ(tcSamplesNextSampleInFrame.GetRemainder(), Samples(1));
            }
            ++tc;
            i++;
        } while (tc != tcEnd);
    }
}

TEST_F(TimecodeSamplesTests, LastSampleInFrameRemainder)
{
    for (auto framerate : Framerate::values())
    {
        const Samplerate sr(48000.0);
        const int64_t frameLengthSamples = CalcTruncatedFrameLengthInSamples(sr, framerate);

        if (FullStressTest::Active())
        {
            std::cout << framerate.ToString() << std::endl;
        }
        Timecode tc(framerate, 0, 0, 0, 0);
        Timecode tcEnd(framerate, 0, 0, 0, 0); // end after wrapping 24 hours
        int i = 0;
        do
        {
            if (FullStressTest::Active() || i < 10000 || (i % 173) == 0)
            {
                Timecode tcNext = tc;
                ++tcNext;
                Samples nextFrameSamples  = tcNext.ToSamples(sr);
                Samples lastSampleInFrame = Samples(nextFrameSamples.GetValue() - 1);
                TimecodeSamples tcSamples(framerate, tc.ToSamples(sr), sr);
                TimecodeSamples tcSamplesLastSampleInFrame(framerate, lastSampleInFrame, sr);
                ASSERT_EQ(tcSamples.GetTimecode(), tcSamplesLastSampleInFrame.GetTimecode());
                ASSERT_EQ(tcSamples.GetRemainder(), Samples(0));
                ASSERT_GE(tcSamplesLastSampleInFrame.GetRemainder().GetValue(),
                          frameLengthSamples - 1);
                ASSERT_LE(tcSamplesLastSampleInFrame.GetRemainder().GetValue(),
                          frameLengthSamples + 1);
            }
            ++tc;
            i++;
        } while (tc != tcEnd);
    }
}

TEST_F(TimecodeSamplesTests, ToString)
{
    for (auto framerate : Framerate::values())
    {
        const Samplerate sr(48000.0);

        if (FullStressTest::Active())
        {
            std::cout << framerate.ToString() << std::endl;
        }
        Timecode tc(framerate, 0, 0, 0, 0);
        Timecode tcEnd(framerate, 0, 0, 0, 0); // end after wrapping 24 hours
        int i = 0;
        do
        {
            if (FullStressTest::Active() || i < 10000 || (i % 173) == 0)
            {
                Samples somewhereInFrame = Samples(tc.ToSamples(sr).GetValue() + 123);
                TimecodeSamples tcSamples(framerate, tc.ToSamples(sr), sr);
                TimecodeSamples tcSamplesNextSampleInFrame(framerate, somewhereInFrame, sr);
                ASSERT_EQ(tcSamples.ToString(false), tc.ToString());
                ASSERT_EQ(tcSamples.ToString(true), tc.ToString() + "+0 samples");
                ASSERT_EQ(tcSamplesNextSampleInFrame.ToString(false),
                          tc.ToString() + "+123 samples");
                ASSERT_EQ(tcSamplesNextSampleInFrame.ToString(true),
                          tc.ToString() + "+123 samples");
            }
            ++tc;
            i++;
        } while (tc != tcEnd);
    }
}
