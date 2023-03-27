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

#include <tcutils/Convert.h>
#include <tcutils/Timecode.h>
#include <tcutils/Types.h>

#include <gtest/gtest.h>

#include "FullStressTest.h"

using namespace testing;
using namespace Dolby::TcUtils;

// test Convert methods

class ConvertTests : public Test
{
};

TEST_F(ConvertTests, DAMFSecondsToSeconds)
{
    // clang-format off
    ASSERT_EQ(Convert::ToSeconds(Framerate::FPS_24,     DAMFSeconds(1000.0)), Seconds(1000.0));
    ASSERT_EQ(Convert::ToSeconds(Framerate::FPS_23976,  DAMFSeconds(1000.0)), Seconds(1001.0));
    ASSERT_EQ(Convert::ToSeconds(Framerate::FPS_2997,   DAMFSeconds(1000.0)), Seconds(1001.0));
    ASSERT_EQ(Convert::ToSeconds(Framerate::FPS_2997DF, DAMFSeconds(1000.0)), Seconds(999.999));
    ASSERT_EQ(Convert::ToSeconds(Framerate::FPS_2997DF, DAMFSeconds(1001.0)), Seconds(1001.0));
    ASSERT_EQ(Convert::ToSeconds(Framerate::FPS_30DF,   DAMFSeconds(1000.0)), Seconds(999.0));
    ASSERT_EQ(Convert::ToSeconds(Framerate::FPS_30DF,   DAMFSeconds(1001.0)), Seconds(1000.0));
    // clang-format on
}

TEST_F(ConvertTests, SecondsToDAMFSeconds)
{
    // clang-format off
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_24,     Seconds(1000.0)), DAMFSeconds(1000.0));
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_24,     Seconds(1001.0)), DAMFSeconds(1001.0));
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_23976,  Seconds(1001.0)), DAMFSeconds(1000.0));
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_2997,   Seconds(1001.0)), DAMFSeconds(1000.0));
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_2997DF, Seconds(1001.0)), DAMFSeconds(1001.0));
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_30DF,   Seconds(1000.0)), DAMFSeconds(1001.0));
    // clang-format on
}

TEST_F(ConvertTests, DAMFSecondsToSamples)
{
    const Samplerate sr{48000.0};
    const DAMFSeconds oneSecond{1.0};
    const DAMFSeconds oneHour{3600.0};

    // clang-format off
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_23976,  oneSecond, sr), Samples(48048));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_24,     oneSecond, sr), Samples(48000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_25,     oneSecond, sr), Samples(48000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_2997,   oneSecond, sr), Samples(48048));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_2997DF, oneSecond, sr), Samples(48048));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_30,     oneSecond, sr), Samples(48000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_30DF,   oneSecond, sr), Samples(48000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_30,     oneSecond, sr), Samples(48000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_48,     oneSecond, sr), Samples(48000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_50,     oneSecond, sr), Samples(48000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_60,     oneSecond, sr), Samples(48000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_96,     oneSecond, sr), Samples(48000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_100,    oneSecond, sr), Samples(48000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_120,    oneSecond, sr), Samples(48000));

    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_23976,  oneHour, sr), Samples(172972800));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_24,     oneHour, sr), Samples(172800000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_25,     oneHour, sr), Samples(172800000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_2997,   oneHour, sr), Samples(172972800));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_2997DF, oneHour, sr), Samples(172799827));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_30,     oneHour, sr), Samples(172800000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_30DF,   oneHour, sr), Samples(172627200));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_30,     oneHour, sr), Samples(172800000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_48,     oneHour, sr), Samples(172800000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_50,     oneHour, sr), Samples(172800000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_60,     oneHour, sr), Samples(172800000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_96,     oneHour, sr), Samples(172800000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_100,    oneHour, sr), Samples(172800000));
    ASSERT_EQ(Convert::ToSamples(Framerate::FPS_120,    oneHour, sr), Samples(172800000));
    // clang-format on
}

TEST_F(ConvertTests, SamplesToDAMFSeconds)
{
    const Samplerate sr{48000.0};
    const DAMFSeconds oneSecond{1.0};
    const DAMFSeconds oneHour{3600.0};

    // clang-format off
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_23976,  Samples(48048), sr), oneSecond);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_24,     Samples(48000), sr), oneSecond);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_25,     Samples(48000), sr), oneSecond);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_2997,   Samples(48048), sr), oneSecond);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_2997DF, Samples(48048), sr), oneSecond);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_30,     Samples(48000), sr), oneSecond);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_30DF,   Samples(48000), sr), oneSecond);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_48,     Samples(48000), sr), oneSecond);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_50,     Samples(48000), sr), oneSecond);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_60,     Samples(48000), sr), oneSecond);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_96,     Samples(48000), sr), oneSecond);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_100,    Samples(48000), sr), oneSecond);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_120,    Samples(48000), sr), oneSecond);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_23976,  Samples(172972800), sr), oneHour);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_24,     Samples(172800000), sr), oneHour);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_25,     Samples(172800000), sr), oneHour);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_2997,   Samples(172972800), sr), oneHour);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_2997DF, Samples(172799827), sr), oneHour);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_30,     Samples(172800000), sr), oneHour);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_30DF,   Samples(172627200), sr), oneHour);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_48,     Samples(172800000), sr), oneHour);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_50,     Samples(172800000), sr), oneHour);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_60,     Samples(172800000), sr), oneHour);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_96,     Samples(172800000), sr), oneHour);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_100,    Samples(172800000), sr), oneHour);
    ASSERT_EQ(Convert::ToDAMFSeconds(Framerate::FPS_120,    Samples(172800000), sr), oneHour);
    // clang-format on
}

TEST_F(ConvertTests, SamplesToDAMFSecondsToSamples)
{
    // for each framerate, check that the conversion from samplePos to DAMFSeconds and back gives
    // the original value

    const bool fullStressTestActive = FullStressTest::Active();

    for (const auto framerate : Framerate::values())
    {
        if (fullStressTestActive)
        {
            std::cout << framerate.ToString() << std::endl;
        }

        int64_t samplePosEnd = 48000LL * 24LL * 60LL * 60LL;
        samplePosEnd =
            framerate.IsRatio1001() ? ((samplePosEnd * 1001 + 500) / 1000) : samplePosEnd;

        int64_t inc = 1;

        int64_t showProgressAtSamplesPos = 0;

        for (int64_t samplePos = 0; samplePos < samplePosEnd; samplePos += inc)
        {
            if (fullStressTestActive && samplePos >= showProgressAtSamplesPos)
            {
                std::cout << ".";
                std::cout.flush();
                showProgressAtSamplesPos += samplePosEnd / 100;
            }
            double t = Convert::ToDAMFSeconds(framerate, Samples(samplePos), Samplerate(48000.0))
                           .GetValue();
            int64_t samplePosBackFromDAMFSeconds =
                Convert::ToSamples(framerate, DAMFSeconds(t), Samplerate(48000.0)).GetValue();
            ASSERT_EQ(samplePos, samplePosBackFromDAMFSeconds);
            if (!fullStressTestActive)
            {
                // at increasing intervals, unless when fullStressTestActive
                inc = (inc * 3) & 4095;
            }
        }
        if (fullStressTestActive)
        {
            std::cout << std::endl;
        }
    }
}

TEST_F(ConvertTests, Hour24)
{
    for (auto framerate : Framerate::values())
    {
        const Samplerate sr(48000.0);
        {
            double hour24               = 24. * 60. * 60.;
            auto samplesFromDAMFSeconds = Convert::ToSamples(framerate, DAMFSeconds(hour24), sr);
            auto samplesFromSeconds     = Convert::ToSamples(Seconds(hour24), sr);

            ASSERT_NE(samplesFromDAMFSeconds.GetValue(), 0);

            if (!framerate.IsRatio1001() && !framerate.IsDrop())
            {
                // for integer non-drop framerate, seconds and damf seconds are the same
                ASSERT_EQ(samplesFromDAMFSeconds.GetValue(), samplesFromSeconds.GetValue());
            }
            else if (!framerate.IsDrop())
            {
                // for non-drop ntsc framerate (23.976, 29.97), at 24 hours real time
                // the timecode hasn't reached 24:00:00;00 yet (ntsc time runs a
                // factor 1001/1000 slower) ...
                ASSERT_GT(samplesFromDAMFSeconds.GetValue(), samplesFromSeconds.GetValue());
            }
            else
            {
                // ... but at drop frame, frames are dropped to compensate for that
                // and the timecode actually overshoots 24:00:00:00
                ASSERT_LT(samplesFromDAMFSeconds.GetValue(), samplesFromSeconds.GetValue());
            }

            auto samplesFromTimeCode =
                Timecode(framerate, 24, 0, 0, 0, WrapMode::CONTINUE).ToSamples(sr);
            ASSERT_EQ(samplesFromDAMFSeconds.GetValue(), samplesFromTimeCode.GetValue());
        }
    }
}
