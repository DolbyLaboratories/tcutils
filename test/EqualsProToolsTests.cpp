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
#include <tcutils/Framerate.h>
#include <tcutils/Timecode.h>

using namespace testing;
using namespace Dolby::TcUtils;

class EqualsProToolsTests : public Test
{
};

// Test against some manually confirmed values from Pro Tools at 29.97 FPS.
TEST_F(EqualsProToolsTests, SamplesAt2997)
{
    const int offset[31] = {0,     1602,  3203,  4805,  6406,  8008,  9610,  11211, 12813, 14414,
                            16016, 17618, 19219, 20821, 22422, 24024, 25626, 27227, 28829, 30430,
                            32032, 33634, 35235, 36837, 38438, 40040, 41642, 43243, 44845, 46446};

    const char* tcStr[5] = {
        "00:00:00:00", "00:16:40:00", "00:33:20:00", "02:46:40:00", "22:13:20:00"};
    int64_t samplepos[5] = {0, 48048000, 96096000, 480480000, 3843840000};

    Framerate framerate(Framerate::FPS_2997);

    Samplerate sr(48000.0);

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 30; j++)
        {
            // create from string
            Timecode tc1(framerate, tcStr[i]);
            // replace frames
            tc1 = Timecode(
                framerate, tc1.GetUnitHours(), tc1.GetUnitMinutes(), tc1.GetUnitSeconds(), j);

            ASSERT_EQ(tc1.ToSamples(sr).GetValue(), samplepos[i] + offset[j]);

            Timecode tc2 =
                Timecode(framerate, Samples(samplepos[i] + offset[j]), sr, RoundingMode::TRUNCATE);
            ASSERT_EQ(tc1.ToString(), tc2.ToString());

            Timecode tc3 = Timecode(
                framerate, Samples(samplepos[i] + offset[j] - 1), sr, RoundingMode::TRUNCATE);
            ASSERT_NE(tc2.ToString(), tc3.ToString());
        }
    }
}

// Test against some manually confirmed values from Pro Tools at 29.97DF.
// These are drop frame (00:0x:00:02) positions except for the first (00:00:00:00) and the last
// (00:10:00:00)
TEST_F(EqualsProToolsTests, SamplesAt2997DF)
{
    int64_t samplepos[11] = {0,
                             2882880,
                             5762557,
                             8642234,
                             11521910,
                             14401587,
                             17281264,
                             20160941,
                             23040618,
                             25920294,
                             28799971};

    Framerate framerate(Framerate::FPS_2997DF);

    Samplerate sr(48000.0);

    for (int i = 0; i < 11; i++)
    {
        Timecode tc1(framerate, 0, i, 0, ((i % 10) ? 2 : 0));

        ASSERT_EQ(tc1.ToSamples(sr).GetValue(), samplepos[i]);

        Timecode tc2 = Timecode(framerate, Samples(samplepos[i]), sr, RoundingMode::TRUNCATE);
        ASSERT_EQ(tc1.ToString(), tc2.ToString());

        Timecode tc3 = Timecode(framerate, Samples(samplepos[i] - 1), sr, RoundingMode::TRUNCATE);
        ASSERT_NE(tc2.ToString(), tc3.ToString());
    }

    ASSERT_EQ(Timecode(framerate, 1, 0, 0, 0).ToSamples(sr), Samples(172799827));
    ASSERT_EQ(Timecode(framerate, 10, 0, 0, 0).ToSamples(sr), Samples(1727998272));
}

// Test against some manually confirmed values from Pro Tools.
// These are drop frame (00:0x:00:02) positions except for the first (00:00:00:00) and the last
// (00:10:00:00)
TEST_F(EqualsProToolsTests, SamplesAt30DF)
{
    int64_t samplepos[11] = {0,
                             2880000,
                             5756800,
                             8633600,
                             11510400,
                             14387200,
                             17264000,
                             20140800,
                             23017600,
                             25894400,
                             28771200};

    Framerate framerate(Framerate::FPS_30DF);
    Samplerate sr(48000.0);

    for (int i = 0; i < 11; i++)
    {
        Timecode tc1(framerate, 0, i, 0, ((i % 10) ? 2 : 0));

        ASSERT_EQ(tc1.ToSamples(sr), Samples(samplepos[i]));

        Timecode tc2 = Timecode(framerate, Samples(samplepos[i]), sr, RoundingMode::TRUNCATE);
        ASSERT_EQ(tc1.ToString(), tc2.ToString());

        Timecode tc3 = Timecode(framerate, Samples(samplepos[i] - 1), sr, RoundingMode::TRUNCATE);
        ASSERT_NE(tc2.ToString(), tc3.ToString());
    }

    ASSERT_EQ(Timecode(framerate, 1, 0, 0, 0).ToSamples(sr), Samples(172627200));
    ASSERT_EQ(Timecode(framerate, 10, 0, 0, 0).ToSamples(sr), Samples(1726272000));
}
