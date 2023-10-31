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
#include <algorithm>
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <tcutils/Convert.h>
#include <tcutils/FrameBoundary.h>
#include <tcutils/Framerate.h>
#include <tcutils/Timecode.h>
#include <tcutils/TimecodeSubframes.h>

using namespace testing;
using namespace Dolby::TcUtils;

class TimecodeTests : public Test
{
};

// Test conversion to frame count. Values were checked to match Pro Tools
// (logging values passed to an AAX plugin)
TEST_F(TimecodeTests, ToFrames)
{
    // clang-format off
    ASSERT_EQ(Timecode(Framerate::FPS_23976,  "01:00:00:00").ToFrames().GetValue(), 86400);
    ASSERT_EQ(Timecode(Framerate::FPS_24,     "01:00:00:00").ToFrames().GetValue(), 86400);
    ASSERT_EQ(Timecode(Framerate::FPS_25,     "01:00:00:00").ToFrames().GetValue(), 90000);
    ASSERT_EQ(Timecode(Framerate::FPS_2997,   "01:00:00:00").ToFrames().GetValue(), 108000);
    ASSERT_EQ(Timecode(Framerate::FPS_2997DF, "01:00:00;00").ToFrames().GetValue(), 107892);
    ASSERT_EQ(Timecode(Framerate::FPS_30,     "01:00:00:00").ToFrames().GetValue(), 108000);
    ASSERT_EQ(Timecode(Framerate::FPS_30DF,   "01:00:00;00").ToFrames().GetValue(), 107892);
    ASSERT_EQ(Timecode(Framerate::FPS_48,     "01:00:00:00").ToFrames().GetValue(), 172800);
    ASSERT_EQ(Timecode(Framerate::FPS_50,     "01:00:00:00").ToFrames().GetValue(), 180000);
    ASSERT_EQ(Timecode(Framerate::FPS_60,     "01:00:00:00").ToFrames().GetValue(), 216000);
    ASSERT_EQ(Timecode(Framerate::FPS_96,     "01:00:00:00").ToFrames().GetValue(), 345600);
    ASSERT_EQ(Timecode(Framerate::FPS_100,    "01:00:00:00").ToFrames().GetValue(), 360000);
    ASSERT_EQ(Timecode(Framerate::FPS_120,    "01:00:00:000").ToFrames().GetValue(), 432000);

    ASSERT_EQ(Timecode(Framerate::FPS_23976,  "10:00:00:00").ToFrames().GetValue(), 864000);
    ASSERT_EQ(Timecode(Framerate::FPS_24,     "10:00:00:00").ToFrames().GetValue(), 864000);
    ASSERT_EQ(Timecode(Framerate::FPS_25,     "10:00:00:00").ToFrames().GetValue(), 900000);
    ASSERT_EQ(Timecode(Framerate::FPS_2997,   "10:00:00:00").ToFrames().GetValue(), 1080000);
    ASSERT_EQ(Timecode(Framerate::FPS_2997DF, "10:00:00;00").ToFrames().GetValue(), 1078920);
    ASSERT_EQ(Timecode(Framerate::FPS_30,     "10:00:00:00").ToFrames().GetValue(), 1080000);
    ASSERT_EQ(Timecode(Framerate::FPS_30DF,   "10:00:00;00").ToFrames().GetValue(), 1078920);
    ASSERT_EQ(Timecode(Framerate::FPS_48,     "10:00:00:00").ToFrames().GetValue(), 1728000);
    ASSERT_EQ(Timecode(Framerate::FPS_50,     "10:00:00:00").ToFrames().GetValue(), 1800000);
    ASSERT_EQ(Timecode(Framerate::FPS_60,     "10:00:00:00").ToFrames().GetValue(), 2160000);
    ASSERT_EQ(Timecode(Framerate::FPS_96,     "10:00:00:00").ToFrames().GetValue(), 3456000);
    ASSERT_EQ(Timecode(Framerate::FPS_100,    "10:00:00:00").ToFrames().GetValue(), 3600000);
    ASSERT_EQ(Timecode(Framerate::FPS_120,    "10:00:00:000").ToFrames().GetValue(), 4320000);
    // clang-format on
}

// Test conversion to frame count. Values were checked to match Pro Tools
// (logging values passed to an AAX plugin)
TEST_F(TimecodeTests, FromFrames)
{
    // clang-format off
    ASSERT_EQ(Timecode(Framerate::FPS_23976,  Frames(86400)).ToString(),   "01:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_24,     Frames(86400)).ToString(),   "01:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_25,     Frames(90000)).ToString(),   "01:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_2997,   Frames(108000)).ToString(),  "01:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_2997DF, Frames(107892)).ToString(),  "01:00:00;00");
    ASSERT_EQ(Timecode(Framerate::FPS_30,     Frames(108000)).ToString(),  "01:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_30DF,   Frames(107892)).ToString(),  "01:00:00;00");
    ASSERT_EQ(Timecode(Framerate::FPS_48,     Frames(172800)).ToString(),  "01:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_50,     Frames(180000)).ToString(),  "01:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_60,     Frames(216000)).ToString(),  "01:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_96,     Frames(345600)).ToString(),  "01:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_100,    Frames(360000)).ToString(),  "01:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_120,    Frames(432000)).ToString(),  "01:00:00:000");

    ASSERT_EQ(Timecode(Framerate::FPS_23976,  Frames(864000)).ToString(),  "10:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_24,     Frames(864000)).ToString(),  "10:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_25,     Frames(900000)).ToString(),  "10:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_2997,   Frames(1080000)).ToString(), "10:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_2997DF, Frames(1078920)).ToString(), "10:00:00;00");
    ASSERT_EQ(Timecode(Framerate::FPS_30,     Frames(1080000)).ToString(), "10:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_30DF,   Frames(1078920)).ToString(), "10:00:00;00");
    ASSERT_EQ(Timecode(Framerate::FPS_48,     Frames(1728000)).ToString(), "10:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_50,     Frames(1800000)).ToString(), "10:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_60,     Frames(2160000)).ToString(), "10:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_96,     Frames(3456000)).ToString(), "10:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_100,    Frames(3600000)).ToString(), "10:00:00:00");
    ASSERT_EQ(Timecode(Framerate::FPS_120,    Frames(4320000)).ToString(), "10:00:00:000");

    ASSERT_EQ(Timecode(Framerate::FPS_2997DF, Frames(3598)).ToString(),    "00:02:00;02");
    ASSERT_EQ(Timecode(Framerate::FPS_2997DF, Frames(3598)).ToString(),    "00:02:00;02");
    ASSERT_EQ(Timecode(Framerate::FPS_2997DF, Frames(7193)).ToString(),    "00:03:59;29");
    ASSERT_EQ(Timecode(Framerate::FPS_2997DF, Frames(1357529)).ToString(), "12:34:56;07");
    ASSERT_EQ(Timecode(Framerate::FPS_2997DF, Frames(30598)).ToString(),   "00:17:01;00");
    ASSERT_EQ(Timecode(Framerate::FPS_2997DF, Frames(1078920)).ToString(), "10:00:00;00");
    // clang-format on

    ASSERT_EQ(
        49005757,
        Timecode(Framerate::FPS_2997DF, Frames(30598)).ToSamples(Samplerate(48000.)).GetValue());
}

// Test that going from timecode to frame count and back to timecode gives the original
// timecode. Check all timecodes for the first 2 hours, or for 24 hours when doing a full stress
// test.
TEST_F(TimecodeTests, ToFramesSymmetry)
{
    int hEnd = FullStressTest::Active() ? 24 : 2;

    for (auto framerate : Framerate::values())
    {
        if (FullStressTest::Active())
        {
            std::cout << framerate.ToString() << std::endl;
        }
        for (int h = 0; h < hEnd; h++)
        {
            for (int m = 0; m < 60; m++)
            {
                for (int s = 0; s < 60; s++)
                {
                    int sf = 0;
                    if (framerate.IsDrop() && s == 0 && (m % 10) != 0)
                    {
                        sf = 2;
                    }
                    for (int f = sf; f < framerate.GetFrameCount(); f++)
                    {
                        Timecode tc(framerate, h, m, s, f);

                        Frames frames = tc.ToFrames();

                        Timecode tcBack(framerate, frames);

                        ASSERT_EQ(tc.ToString(), tcBack.ToString());
                    }
                }
            }
        }
    }
}

// Test that going from frame count to timecode and back to frame count back gives the original
// frame count. Check all timecodes for the first 2 hours, or for 24 hours when doing a full
// stress test.
TEST_F(TimecodeTests, FramesToTimecodeSymmetry)
{
    int hEnd = FullStressTest::Active() ? 23 : 2;

    for (auto framerate : Framerate::values())
    {
        // last frame to check
        int32_t lastFrame =
            Timecode(framerate, hEnd, 59, 59, framerate.GetFrameCount() - 1).ToFrames().GetValue();
        // make sure the last frame roughly makes sense
        ASSERT_GT(lastFrame, (hEnd + 1) * 60 * 60 * (framerate.GetFrameCount() - 1));
        ASSERT_LT(lastFrame, (hEnd + 1) * 60 * 60 * (framerate.GetFrameCount() + 1));

        if (FullStressTest::Active())
        {
            std::cout << framerate.ToString() << std::endl;
        }

        for (int32_t frames = 0; frames <= lastFrame; frames++)
        {
            Timecode tc        = Timecode(framerate, Frames(frames));
            int32_t framesBack = tc.ToFrames().GetValue();
            ASSERT_EQ(frames, framesBack);
        }
    }
}

// Test conversion to string
TEST_F(TimecodeTests, ToString)
{
    for (auto framerate : Framerate::values())
    {
        Timecode timecode(framerate, 1, 2, 3, 4);
        if (framerate.GetFrameCount() > 100)
        {
            char str[13];
            timecode.ToString(str, 13);
            ASSERT_STREQ(str, framerate.IsDrop() ? "01:02:03;004" : "01:02:03:004");
        }
        else
        {
            char str[12];
            timecode.ToString(str, 12);
            ASSERT_STREQ(str, framerate.IsDrop() ? "01:02:03;04" : "01:02:03:04");
        }
    }
    for (auto framerate : Framerate::values())
    {
        Timecode timecode(framerate, 22, 33, 44, 11);
        if (framerate.GetFrameCount() > 100)
        {
            char str[13];
            timecode.ToString(str, 13);
            ASSERT_STREQ(str, framerate.IsDrop() ? "22:33:44;011" : "22:33:44:011");
        }
        else
        {
            char str[12];
            timecode.ToString(str, 12);
            ASSERT_STREQ(str, framerate.IsDrop() ? "22:33:44;11" : "22:33:44:11");
        }
    }
}

// Test conversion from string for undefined timecode (--:--:--:--)
TEST_F(TimecodeTests, UndefinedString)
{
    for (auto framerate : Framerate::values())
    {
        char str[13];
        Timecode tc(framerate);
        tc.ToString(str, 13);
        ASSERT_EQ(strlen(str), framerate.GetFrameCount() > 100 ? 12 : 11);
        for (size_t i = 0; i < strlen(str); i++)
        {
            ASSERT_EQ(str[i],
                      (framerate.IsDrop() && i == 8) ? ';'
                      : (i == 2 || i == 5 || i == 8) ? ':'
                                                     : '-');
        }
        Timecode tc2(framerate, str);
        ASSERT_EQ(tc, tc2);
    }
}

// Test conversion from string.
TEST_F(TimecodeTests, FromString)
{
    for (auto framerate : Framerate::values())
    {
        Timecode timecode(framerate,
                          framerate.GetFrameCount() > 100 ? "01:02:03:004"
                          : framerate.IsDrop()            ? "01:02:03;04"
                                                          : "01:02:03:04");
        ASSERT_EQ(timecode.GetUnitHours(), 1);
        ASSERT_EQ(timecode.GetUnitMinutes(), 2);
        ASSERT_EQ(timecode.GetUnitSeconds(), 3);
        ASSERT_EQ(timecode.GetUnitFrames(), 4);
    }
    for (auto framerate : Framerate::values())
    {
        Timecode timecode(framerate,
                          framerate.GetFrameCount() > 100 ? "22:33:44:011"
                          : framerate.IsDrop()            ? "22:33:44;11"
                                                          : "22:33:44:11");
        ASSERT_EQ(timecode.GetUnitHours(), 22);
        ASSERT_EQ(timecode.GetUnitMinutes(), 33);
        ASSERT_EQ(timecode.GetUnitSeconds(), 44);
        ASSERT_EQ(timecode.GetUnitFrames(), 11);
    }
}

// Test that passing values out of range to the constructor throws an exception.
TEST_F(TimecodeTests, OutOfRange)
{
    for (auto framerate : Framerate::values())
    {

        const auto f = framerate.GetFrameCount();

        // clang-format off

        // TODO @mdebo Neither throws. Do we want to add a WrapMode::NONE that does
        // throw at >= 24 h?
        ASSERT_NO_THROW  (Timecode(framerate, 24,  0,  0, 0, WrapMode::WRAP_AT_MIDNIGHT));
        ASSERT_NO_THROW  (Timecode(framerate, 24,  0,  0, 0, WrapMode::CONTINUE));

        ASSERT_ANY_THROW (Timecode(framerate,  0, 60,  0, 0));
        ASSERT_ANY_THROW (Timecode(framerate,  0,  0, 60, 0));
        ASSERT_ANY_THROW (Timecode(framerate,  0,  0,  0, f));
        // clang-format on
    }
}

// Test that passing drop frames to the constructor throws an exception.
TEST_F(TimecodeTests, DropFrame)
{
    // clang-format off
    ASSERT_ANY_THROW (Timecode(Framerate::FPS_2997DF, 0,  1, 0, 0));
    ASSERT_ANY_THROW (Timecode(Framerate::FPS_2997DF, 0,  1, 0, 1));
    ASSERT_NO_THROW  (Timecode(Framerate::FPS_2997DF, 0,  1, 0, 2));
    ASSERT_ANY_THROW (Timecode(Framerate::FPS_2997DF, 0,  9, 0, 0));
    ASSERT_ANY_THROW (Timecode(Framerate::FPS_2997DF, 0,  9, 0, 1));
    ASSERT_NO_THROW  (Timecode(Framerate::FPS_2997DF, 0,  9, 0, 2));
    ASSERT_NO_THROW  (Timecode(Framerate::FPS_2997DF, 0, 10, 0, 0));

    ASSERT_ANY_THROW (Timecode(Framerate::FPS_30DF, 0,  1, 0, 0));
    ASSERT_ANY_THROW (Timecode(Framerate::FPS_30DF, 0,  1, 0, 1));
    ASSERT_NO_THROW  (Timecode(Framerate::FPS_30DF, 0,  1, 0, 2));
    ASSERT_ANY_THROW (Timecode(Framerate::FPS_30DF, 0,  9, 0, 0));
    ASSERT_ANY_THROW (Timecode(Framerate::FPS_30DF, 0,  9, 0, 1));
    ASSERT_NO_THROW  (Timecode(Framerate::FPS_30DF, 0,  9, 0, 2));
    ASSERT_NO_THROW  (Timecode(Framerate::FPS_30DF, 0, 10, 0, 0));
    // clang-format on
}

// Test that passing strings containing values out of range or drop frames throws an exception.
TEST_F(TimecodeTests, OutOfRangeFromString)
{
    for (auto framerate : Framerate::values())
    {
        const bool isDrop = framerate.IsDrop();
        ASSERT_ANY_THROW(Timecode(framerate, isDrop ? "00:60:00;00" : "00:60:00:00"));
        ASSERT_ANY_THROW(Timecode(framerate, isDrop ? "00:00:60;00" : "00:00:60:00"));
        std::string s = std::string("00:00:00:") + std::to_string(framerate.GetFrameCount());
        ASSERT_ANY_THROW(Timecode(framerate, s));
    }
    ASSERT_ANY_THROW(Timecode(Framerate::FPS_2997DF, "00:01:00:00")); // drop frame
    ASSERT_ANY_THROW(Timecode(Framerate::FPS_30DF, "00:01:00:00"));   // drop frame
}

// Test difference between WrapMode WRAP_AT_MIDNIGHT and CONTINUE.
TEST_F(TimecodeTests, WrapVsContinue)
{
    const double midnight = 24. * 3600.;
    for (int i = -10; i < 10; i++)
    {
        Timecode tc1 = Timecode(Framerate::FPS_24,
                                DAMFSeconds(midnight + i),
                                RoundingMode::NEAREST,
                                WrapMode::WRAP_AT_MIDNIGHT);
        Timecode tc2 = Timecode(Framerate::FPS_24,
                                DAMFSeconds(midnight + i),
                                RoundingMode::NEAREST,
                                WrapMode::CONTINUE);

        if (i < 0)
        {
            ASSERT_EQ(tc1, tc2);
            ASSERT_EQ(tc1.GetUnitHours(), 23);
            ASSERT_EQ(tc1.GetUnitMinutes(), 59);
            ASSERT_EQ(tc1.GetUnitSeconds(), 60 + i);
            ASSERT_EQ(tc1.GetUnitFrames(), 0);
        }
        else
        {
            ASSERT_EQ(tc1.GetUnitHours(), 0);
            ASSERT_EQ(tc1.GetUnitMinutes(), 0);
            ASSERT_EQ(tc1.GetUnitSeconds(), i);
            ASSERT_EQ(tc1.GetUnitFrames(), 0);

            ASSERT_EQ(tc2.GetUnitHours(), 24);
            ASSERT_EQ(tc2.GetUnitMinutes(), 0);
            ASSERT_EQ(tc2.GetUnitSeconds(), i);
            ASSERT_EQ(tc2.GetUnitFrames(), 0);
        }
    }
}

// Test that summing the difference in samples between the last and one-before-last frame
// (before midnight) to the last frame gives the sample at midnight.
TEST_F(TimecodeTests, FramesNearMidnight)
{
    // midnight in damf-seconds
    const DAMFSeconds midnight{24. * 3600.};
    const Samplerate sr{48000.};

    ASSERT_EQ(Convert::ToSamples(Framerate(Framerate::FPS_24), midnight, sr).GetValue(),
              4147200000);
    ASSERT_EQ(Convert::ToSamples(Framerate(Framerate::FPS_25), midnight, sr).GetValue(),
              4147200000);
    ASSERT_EQ(Convert::ToSamples(Framerate(Framerate::FPS_30), midnight, sr).GetValue(),
              4147200000);

    for (auto framerate : Framerate::values())
    {
        Timecode timecodeA = Timecode(framerate, 23, 59, 59, framerate.GetFrameCount() - 2);
        Timecode timecodeB = Timecode(framerate, 23, 59, 59, framerate.GetFrameCount() - 1);
        int64_t samplesA   = timecodeA.ToSamples(sr).GetValue();
        int64_t samplesB   = timecodeB.ToSamples(sr).GetValue();
        int64_t samples    = samplesB + (samplesB - samplesA);
        int64_t margin     = framerate.IsRatio1001() ? 1 : 0;
        ASSERT_GE(samples,
                  Convert::ToSamples(framerate, DAMFSeconds(midnight), sr).GetValue() - margin);
        ASSERT_LE(samples, Convert::ToSamples(framerate, DAMFSeconds(midnight), sr).GetValue());
    }
}

TEST_F(TimecodeTests, CompareOperators)
{
    Framerate framerate(Framerate::FPS_24);

    // clang-format off
    ASSERT_TRUE (Timecode(framerate, "01:02:03:04") == Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "00:02:03:04") == Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:01:03:04") == Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:02:02:04") == Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:02:03:03") == Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "02:02:03:04") == Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:03:03:04") == Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:02:04:04") == Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:02:03:05") == Timecode(framerate, "01:02:03:04"));

    ASSERT_FALSE(Timecode(framerate, "01:02:03:04") != Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "00:02:03:04") != Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:01:03:04") != Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:02:02:04") != Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:02:03:03") != Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "02:02:03:04") != Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:03:03:04") != Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:02:04:04") != Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:02:03:05") != Timecode(framerate, "01:02:03:04"));

    ASSERT_FALSE(Timecode(framerate, "01:02:03:04") < Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "00:02:03:04") < Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:01:03:04") < Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:02:02:04") < Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:02:03:03") < Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "02:02:03:04") < Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:03:03:04") < Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:02:04:04") < Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:02:03:05") < Timecode(framerate, "01:02:03:04"));

    ASSERT_FALSE(Timecode(framerate, "01:02:03:04") > Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "00:02:03:04") > Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:01:03:04") > Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:02:02:04") > Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:02:03:03") > Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "02:02:03:04") > Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:03:03:04") > Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:02:04:04") > Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:02:03:05") > Timecode(framerate, "01:02:03:04"));

    ASSERT_TRUE (Timecode(framerate, "01:02:03:04") <= Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "00:02:03:04") <= Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:01:03:04") <= Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:02:02:04") <= Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:02:03:03") <= Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "02:02:03:04") <= Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:03:03:04") <= Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:02:04:04") <= Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:02:03:05") <= Timecode(framerate, "01:02:03:04"));

    ASSERT_TRUE (Timecode(framerate, "01:02:03:04") >= Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "00:02:03:04") >= Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:01:03:04") >= Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:02:02:04") >= Timecode(framerate, "01:02:03:04"));
    ASSERT_FALSE(Timecode(framerate, "01:02:03:03") >= Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "02:02:03:04") >= Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:03:03:04") >= Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:02:04:04") >= Timecode(framerate, "01:02:03:04"));
    ASSERT_TRUE (Timecode(framerate, "01:02:03:05") >= Timecode(framerate, "01:02:03:04"));
    // clang-format on
}

// Test increment operator
TEST_F(TimecodeTests, IncOperator)
{
    Timecode tc(Framerate::FPS_24, 0, 0, 0, 0);
    ++tc;
    ASSERT_EQ(tc, Timecode(Framerate::FPS_24, "00:00:00:01"));
    for (int i = 0; i < 24 - 1; i++)
    {
        ++tc;
    }
    ASSERT_EQ(tc, Timecode(Framerate::FPS_24, "00:00:01:00"));
    for (int i = 0; i < 6 * 60 * 60 * 24 - 24; i++)
    {
        ++tc;
    }
    ASSERT_EQ(tc, Timecode(Framerate::FPS_24, "06:00:00:00"));
    for (int i = 0; i < 6 * 60 * 60 * 24; i++)
    {
        ++tc;
    }
    ASSERT_EQ(tc, Timecode(Framerate::FPS_24, "12:00:00:00"));
    for (int i = 0; i < 6 * 60 * 60 * 24; i++)
    {
        ++tc;
    }
    ASSERT_EQ(tc, Timecode(Framerate::FPS_24, "18:00:00:00"));
    for (int i = 0; i < 6 * 60 * 60 * 24; i++)
    {
        ++tc;
    }
    ASSERT_EQ(tc, Timecode(Framerate::FPS_24, "00:00:00:00"));
}

// Test decrement operator
TEST_F(TimecodeTests, DecOperator)
{
    Timecode tc(Framerate::FPS_24, 0, 0, 0, 0);
    --tc;
    ASSERT_EQ(tc, Timecode(Framerate::FPS_24, "23:59:59:23"));
    for (int i = 0; i < 24 - 1; i++)
    {
        --tc;
    }
    ASSERT_EQ(tc, Timecode(Framerate::FPS_24, "23:59:59:00"));
    for (int i = 0; i < 24 * 59; i++)
    {
        --tc;
    }
    ASSERT_EQ(tc, Timecode(Framerate::FPS_24, "23:59:00:00"));
    for (int i = 0; i < 24 * 60 * 59; i++)
    {
        --tc;
    }
    ASSERT_EQ(tc, Timecode(Framerate::FPS_24, "23:00:00:00"));
    for (int i = 0; i < 24 * 60 * 60 * 3; i++)
    {
        --tc;
    }
    ASSERT_EQ(tc, Timecode(Framerate::FPS_24, "20:00:00:00"));
}

namespace
{
    int summedFrameCounts()
    {
        int result = 0;
        for (auto framerate : Framerate::values())
        {
            result += framerate.GetFrameCount();
        }
        return result;
    }
}

// Test incrementing and decrement operators with wrapping at midnight
TEST_F(TimecodeTests, IncDecOperators)
{
    size_t cnt = 0;
    for (auto framerate : Framerate::values())
    {
        // go from 23:00:00:00 to 01:00:00:00 and back
        Timecode tcBegin(framerate, 23, 0, 0, 0);
        Timecode tcEnd(framerate, 1, 0, 0, 0);
        Timecode tc = tcBegin;
        // two incs, one dec, making sure the dec goes to the previous
        do
        {
            ++tc;
            Timecode tcCopy = tc;
            ++tc;
            --tc;
            ASSERT_EQ(tc, tcCopy);
            cnt++;
        } while (tc != tcEnd);
        // two decs, one inc, making sure the inc goes to the previous
        do
        {
            --tc;
            Timecode tcCopy = tc;
            --tc;
            ++tc;
            ASSERT_EQ(tc, tcCopy);
            cnt++;
        } while (tc != tcBegin);
    }

    // make sure the operation got executed as often as expected:

    // 18 frames dropped every 10 minutes = 18 * 6 every hour,
    // during 2 hours, for 2 df framerates, once going up, once going down
    const int dropped = 18 * 6 * 2 * 2 * 2;

    ASSERT_EQ(cnt, summedFrameCounts() * 2 * 2 * 60 * 60 - dropped);
}

// Test incrementing and decrement operators with continuing at midnight
TEST_F(TimecodeTests, IncDecOperatorsContinue)
{
    size_t cnt = 0;
    for (auto framerate : Framerate::values())
    {
        // go from 23:00:00:00 to 25:00:00:00 and back
        Timecode tcBegin(framerate, 23, 0, 0, 0, WrapMode::CONTINUE);
        Timecode tcEnd(framerate, 25, 0, 0, 0, WrapMode::CONTINUE);
        Timecode tc = tcBegin;
        // two incs, one dec, making sure the dec goes to the previous
        do
        {
            ++tc;
            Timecode tcCopy = tc;
            ++tc;
            --tc;
            ASSERT_EQ(tc, tcCopy);
            cnt++;
        } while (tc != tcEnd);
        // two decs, one inc, making sure the inc goes to the previous
        do
        {
            --tc;
            Timecode tcCopy = tc;
            --tc;
            ++tc;
            ASSERT_EQ(tc, tcCopy);
            cnt++;
        } while (tc != tcBegin);
    }

    // make sure the operation got executed as often as expected:

    // 18 frames dropped every 10 minutes = 18 * 6 every hour,
    // during 2 hours, for 2 df framerates, once going up, once going down
    const int dropped = 18 * 6 * 2 * 2 * 2;

    ASSERT_EQ(cnt, summedFrameCounts() * 2 * 2 * 60 * 60 - dropped);
}

// Test to DAMFSeconds symmetry (nearest)
TEST_F(TimecodeTests, ToDAMFSecondsNearestSymmetry)
{
    for (auto framerate : Framerate::values())
    {
        if (FullStressTest::Active())
        {
            std::cout << framerate.ToString() << std::endl;
        }
        Timecode tcReference(framerate, 0, 0, 0, 0);
        Timecode tcEnd(framerate, 0, 0, 0, 0); // end after wrapping 24 hours
        int i = 0;
        do
        {
            if (FullStressTest::Active() || i < 100000 || (i % 17) == 0)
            {
                DAMFSeconds tcTime = tcReference.ToDAMFSeconds();
                Timecode tc        = Timecode(framerate, tcTime, RoundingMode::NEAREST);
                ASSERT_EQ(tc, tcReference);
            }
            ++tcReference;
            i++;
        } while (tcReference != tcEnd);
    }
}

// Test to DAMFSeconds symmetry (truncate)
TEST_F(TimecodeTests, ToDAMFSecondsTruncateSymmetry)
{
    for (auto framerate : Framerate::values())
    {
        if (FullStressTest::Active())
        {
            std::cout << framerate.ToString() << std::endl;
        }
        Timecode tcReference(framerate, 0, 0, 0, 0);
        Timecode tcEnd(framerate, 0, 0, 0, 0); // end after wrapping 24 hours
        int i = 0;
        do
        {
            if (FullStressTest::Active() || i < 100000 || (i % 17) == 0)
            {
                DAMFSeconds tcTime = tcReference.ToDAMFSeconds();
                Timecode tc        = Timecode(framerate, tcTime, RoundingMode::TRUNCATE);
                ASSERT_EQ(tc, tcReference);
            }
            ++tcReference;
            i++;
        } while (tcReference != tcEnd);
    }
}

// Test to Seconds symmetry (nearest)
TEST_F(TimecodeTests, ToSecondsNearestSymmetry)
{
    for (auto framerate : Framerate::values())
    {
        if (FullStressTest::Active())
        {
            std::cout << framerate.ToString() << std::endl;
        }
        Timecode tcReference(framerate, 0, 0, 0, 0);
        Timecode tcEnd(framerate, 0, 0, 0, 0); // end after wrapping 24 hours
        int i = 0;
        do
        {
            if (FullStressTest::Active() || i < 100000 || (i % 17) == 0)
            {
                Seconds seconds = tcReference.ToSeconds();
                Timecode tc     = Timecode(framerate, seconds, RoundingMode::NEAREST);
                ASSERT_EQ(tc, tcReference);
                Seconds secondsEarly{seconds.GetValue() - 0.49 / framerate.GetFrameCount()};
                tc = Timecode(framerate, secondsEarly, RoundingMode::NEAREST);
                if (secondsEarly.GetValue() >= 0.0)
                {
                    ASSERT_EQ(tc, tcReference);
                }
                else
                {
                    ASSERT_EQ(tc, Timecode());
                }
                Seconds secondsLate{seconds.GetValue() + 0.49 / framerate.GetFrameCount()};
                tc = Timecode(framerate, secondsLate, RoundingMode::NEAREST);
                ASSERT_EQ(tc, tcReference);
            }
            ++tcReference;
            i++;
        } while (tcReference != tcEnd);
    }
}

// Test to Seconds symmetry (truncate)
TEST_F(TimecodeTests, ToSecondsTruncateSymmetry)
{
    for (auto framerate : Framerate::values())
    {
        if (FullStressTest::Active())
        {
            std::cout << framerate.ToString() << std::endl;
        }
        Timecode tcReference(framerate, 0, 0, 0, 0);
        Timecode tcEnd(framerate, 0, 0, 0, 0); // end after wrapping 24 hours
        int i = 0;
        do
        {
            if (FullStressTest::Active() || i < 100000 || (i % 17) == 0)
            {
                Seconds seconds = tcReference.ToSeconds();
                Timecode tc     = Timecode(framerate, seconds, RoundingMode::TRUNCATE);
                ASSERT_EQ(tc, tcReference);
                Seconds secondsLate{seconds.GetValue() + 0.99 / framerate.GetFrameCount()};
                tc = Timecode(framerate, secondsLate, RoundingMode::TRUNCATE);
                ASSERT_EQ(tc, tcReference);
            }
            ++tcReference;
            i++;
        } while (tcReference != tcEnd);
    }
}

// Test to samples symmetry (truncate)
TEST_F(TimecodeTests, ToSamplesSymmetry)
{
    for (auto framerate : Framerate::values())
    {
        if (FullStressTest::Active())
        {
            std::cout << framerate.ToString() << std::endl;
        }
        int64_t samplePos        = 0;
        int frameCount           = framerate.GetFrameCount(); // 24, 25 or 30
        int samplesPerFrameCount = 48000 / frameCount;
        ASSERT_EQ(48000, samplesPerFrameCount * frameCount);
        Timecode tcPrevReference;
        Timecode tcReference(framerate, 0, 0, 0, 0);
        Timecode tcEnd(framerate, 0, 0, 0, 0); // end after wrapping 24 hours
        int i = 0;
        Samplerate sr(48000.0);
        do
        {
            // calculate the sample pos where we expect each frame to start
            int64_t adjustedSamplePos =
                framerate.IsRatio1001() ? ((samplePos * 1001 + 500) / 1000) : samplePos;
            // check every frame for the first 100000 frames, after that check every 17th frame,
            // unless env var TCUTILS_FULL_STRESS_TEST is set to YES.
            if (FullStressTest::Active() || i < 100000 || (i % 17) == 0)
            {
                // check that Timecode::toSamples gives the same position
                ASSERT_EQ(tcReference.ToSamples(sr).GetValue(), adjustedSamplePos);
                // check that fromSamples creates the expected Timecode
                Timecode tcFromSamples =
                    Timecode(framerate, Samples(adjustedSamplePos), sr, RoundingMode::TRUNCATE);
                ASSERT_TRUE(tcReference == tcFromSamples);
                // check that fromSamples for the sample pos one before the frame start creates the
                // previous frame's Timecode
                Timecode tcFromSamplesMinusOne =
                    Timecode(framerate, Samples(adjustedSamplePos - 1), sr, RoundingMode::TRUNCATE);
                ASSERT_TRUE(tcPrevReference == tcFromSamplesMinusOne);
            }
            // go to next frame, both for samplePos and for the reference timecode
            samplePos += samplesPerFrameCount;
            tcPrevReference = tcReference;
            ++tcReference;
            i++;
        } while (tcReference != tcEnd);
    }
}

TEST_F(TimecodeTests, RoundingModeTruncate)
{
    const Samplerate sr{48000.0};
    for (auto framerate : Framerate::values())
    {
        Timecode tc(framerate, 1, 0, 0, 0);
        for (int i = 0; i < 1000; i++)
        {
            Timecode next = tc;
            ++next;
            Samples samples{next.ToSamples(sr).GetValue() - 1};
            ASSERT_EQ(tc, Timecode(framerate, samples, sr, RoundingMode::TRUNCATE));
            tc = next;
        }
    }
}

TEST_F(TimecodeTests, NextSampleCeilingRoundingModeTruncate)
{
    const Samplerate sr{48000.0};
    for (auto framerate : Framerate::values())
    {
        Timecode tc(framerate, 1, 0, 0, 0);
        for (int i = 0; i < 1000; i++)
        {
            Samples samples{tc.ToSamples(sr).GetValue() + 1};
            ++tc;
            ASSERT_EQ(tc,
                      Timecode(framerate,
                               FrameBoundary::Ceiling(framerate, samples, sr),
                               sr,
                               RoundingMode::TRUNCATE));
        }
    }
}

TEST_F(TimecodeTests, FromStringToSamples)
{
    const Samplerate sr{48000.0};
    for (auto framerate : Framerate::values())
    {
        int32_t frameCount      = framerate.GetFrameCount();
        int64_t oneFrmInSamples = 48000 / frameCount;
        int64_t oneSecInSamples = 48000;
        int64_t oneMinInSamples = 48000 * 60;
        if (framerate.IsRatio1001())
        {
            oneFrmInSamples = (oneFrmInSamples * 1001 + 500) / 1000;
            oneSecInSamples = (oneSecInSamples * 1001 + 500) / 1000;
            oneMinInSamples = (oneMinInSamples * 1001 + 500) / 1000;
        }
        ASSERT_ANY_THROW(Timecode(framerate, "XXX"));
        const char* strZero = framerate.GetFrameCount() > 100 ? "00:00:00:000"
                              : framerate.IsDrop()            ? "00:00:00;00"
                                                              : "00:00:00:00";
        const char* str1Frm = framerate.GetFrameCount() > 100 ? "00:00:00:001"
                              : framerate.IsDrop()            ? "00:00:00;01"
                                                              : "00:00:00:01";
        const char* str1Sec = framerate.GetFrameCount() > 100 ? "00:00:01:000"
                              : framerate.IsDrop()            ? "00:00:01;00"
                                                              : "00:00:01:00";
        const char* str1Min = framerate.GetFrameCount() > 100 ? "00:01:00:000"
                              : framerate.IsDrop()            ? "00:01:00;02"
                                                              : "00:01:00:00";
        ASSERT_EQ(Timecode(framerate, strZero).ToSamples(sr).GetValue(), 0);
        ASSERT_EQ(Timecode(framerate, str1Frm).ToSamples(sr).GetValue(), oneFrmInSamples);
        ASSERT_EQ(Timecode(framerate, str1Sec).ToSamples(sr).GetValue(), oneSecInSamples);
        ASSERT_EQ(Timecode(framerate, str1Min).ToSamples(sr).GetValue(), oneMinInSamples);

        ASSERT_EQ(Timecode(framerate, Samples(0), sr, RoundingMode::TRUNCATE).ToString(), strZero);
        ASSERT_EQ(
            Timecode(framerate, Samples(oneFrmInSamples), sr, RoundingMode::TRUNCATE).ToString(),
            str1Frm);
        ASSERT_EQ(
            Timecode(framerate, Samples(oneSecInSamples), sr, RoundingMode::TRUNCATE).ToString(),
            str1Sec);
        ASSERT_EQ(
            Timecode(framerate, Samples(oneMinInSamples), sr, RoundingMode::TRUNCATE).ToString(),
            str1Min);

        ASSERT_EQ(Timecode(framerate, Samples(oneFrmInSamples - 1), sr, RoundingMode::TRUNCATE)
                      .ToString(),
                  strZero);

        ASSERT_EQ(
            Timecode(framerate, Samples(oneFrmInSamples), sr, RoundingMode::TRUNCATE).ToString(),
            str1Frm);
        ASSERT_EQ(
            Timecode(framerate, Samples(oneSecInSamples), sr, RoundingMode::TRUNCATE).ToString(),
            str1Sec);
        ASSERT_EQ(
            Timecode(framerate, Samples(oneMinInSamples), sr, RoundingMode::TRUNCATE).ToString(),
            str1Min);
    }
}
