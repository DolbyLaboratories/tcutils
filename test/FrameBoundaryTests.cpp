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
#include <tcutils/FrameBoundary.h>
#include <tcutils/Timecode.h>

using namespace testing;
using namespace Dolby::TcUtils;

// test FrameBoundary methods

class FrameBoundaryTests : public Test
{
};

TEST_F(FrameBoundaryTests, Floor)
{
    const Samplerate sr{48000.};
    for (auto framerate : Framerate::values())
    {
        if (FullStressTest::Active())
        {
            std::cout << framerate.ToString() << std::endl;
        }
        Timecode tc(framerate, 0, 0, 0, 0, WrapMode::CONTINUE);
        Timecode tcEnd(framerate, 24, 0, 0, 0, WrapMode::CONTINUE);
        int i = 0;
        do
        {
            // check every frame for the first 100000 frames, after that check every 17th frame,
            // unless env var TCUTILS_FULL_STRESS_TEST is set to YES.
            if (FullStressTest::Active() || i < 100000 || (i % 17) == 0)
            {
                Samples samplePos{tc.ToSamples(sr)};
                ASSERT_TRUE(FrameBoundary::IsOnBoundary(framerate, samplePos, sr));
                ASSERT_EQ(FrameBoundary::Floor(framerate, Samples(samplePos), Samplerate(sr)),
                          samplePos);
            }
            // go to next frame, both for samplePos and for the reference timecode
            ++tc;
            i++;
        } while (tc != tcEnd);
    }
}

TEST_F(FrameBoundaryTests, Ceiling)
{
    const Samplerate sr{48000.};
    for (auto framerate : Framerate::values())
    {
        if (FullStressTest::Active())
        {
            std::cout << framerate.ToString() << std::endl;
        }
        Timecode tc(framerate, 0, 0, 0, 0, WrapMode::CONTINUE);
        Timecode tcEnd(framerate, 24, 0, 0, 0, WrapMode::CONTINUE);
        int i = 0;
        do
        {
            // check every frame for the first 100000 frames, after that check every 17th frame,
            // unless env var TCUTILS_FULL_STRESS_TEST is set to YES.
            if (FullStressTest::Active() || i < 100000 || (i % 17) == 0)
            {
                Samples samplePos     = tc.ToSamples(sr);
                Samples nextSamplePos = Samples(samplePos.GetValue() + 1);
                Timecode tcNext       = tc;
                ++tcNext;
                Samples nextFrameSamplePos = tcNext.ToSamples(sr);
                ASSERT_EQ(FrameBoundary::Ceiling(framerate, Samples(nextSamplePos), sr),
                          nextFrameSamplePos);
                ASSERT_EQ(Timecode(framerate,
                                   nextFrameSamplePos,
                                   sr,
                                   RoundingMode::TRUNCATE,
                                   WrapMode::CONTINUE),
                          tcNext);
            }
            // go to next frame, both for samplePos and for the reference timecode
            ++tc;
            i++;
        } while (tc != tcEnd);
    }
}

TEST_F(FrameBoundaryTests, Round)
{
    const Samplerate sr{48000.};
    for (auto framerate : Framerate::values())
    {
        if (FullStressTest::Active())
        {
            std::cout << framerate.ToString() << std::endl;
        }
        Timecode tc(framerate, 0, 0, 0, 0, WrapMode::CONTINUE);
        Timecode tcEnd(framerate, 24, 0, 0, 0, WrapMode::CONTINUE);
        int i = 0;
        do
        {
            // check every frame for the first 100000 frames, after that check every 17th frame,
            // unless env var TCUTILS_FULL_STRESS_TEST is set to YES.
            if (FullStressTest::Active() || i < 100000 || (i % 17) == 0)
            {
                int64_t samplePos = tc.ToSamples(sr).GetValue();
                Timecode tcNext   = tc;
                ++tcNext;
                int64_t nextSamplePos = tcNext.ToSamples(sr).GetValue();
                int64_t midSamplePos  = (nextSamplePos + samplePos) / 2;
                for (int64_t offset = -1; offset <= 1; offset++)
                {
                    int64_t nearestSamplePos =
                        FrameBoundary::Round(framerate, Samples(midSamplePos + offset), sr)
                            .GetValue();
                    if (offset == -1)
                    {
                        ASSERT_EQ(nearestSamplePos, samplePos);
                    }
                    else if (offset == 1)
                    {
                        ASSERT_EQ(nearestSamplePos, nextSamplePos);
                    }
                    else
                    {
                        ASSERT_TRUE(nearestSamplePos == samplePos ||
                                    nearestSamplePos == nextSamplePos);
                    }
                    Timecode tcFSN               = Timecode(framerate,
                                              Samples(midSamplePos + offset),
                                              sr,
                                              RoundingMode::NEAREST,
                                              WrapMode::CONTINUE);
                    int64_t nearestSamplePosBack = tcFSN.ToSamples(sr).GetValue();
                    if (nearestSamplePosBack != nearestSamplePos)
                    {
                        tcFSN                = Timecode(framerate,
                                         Samples(midSamplePos + offset),
                                         sr,
                                         RoundingMode::NEAREST,
                                         WrapMode::CONTINUE);
                        nearestSamplePosBack = tcFSN.ToSamples(sr).GetValue();
                    }
                    ASSERT_EQ(nearestSamplePosBack, nearestSamplePos);
                }
            }
            // go to next frame, both for samplePos and for the reference timecode
            ++tc;
            i++;
        } while (tc != tcEnd);
    }
}
