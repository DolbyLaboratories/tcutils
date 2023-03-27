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

using namespace testing;
using namespace Dolby::TcUtils;

class FramerateTests : public Test
{
};

// Test converting a Framerate to a string
TEST_F(FramerateTests, ToString)
{
    // clang-format off
    ASSERT_STREQ(Framerate(Framerate::FPS_23976).ToString(),  "23.976");
    ASSERT_STREQ(Framerate(Framerate::FPS_24).ToString(),     "24");
    ASSERT_STREQ(Framerate(Framerate::FPS_25).ToString(),     "25");
    ASSERT_STREQ(Framerate(Framerate::FPS_2997).ToString(),   "29.97");
    ASSERT_STREQ(Framerate(Framerate::FPS_2997DF).ToString(), "29.97df");
    ASSERT_STREQ(Framerate(Framerate::FPS_30).ToString(),     "30");
    ASSERT_STREQ(Framerate(Framerate::FPS_30DF).ToString(),   "30df");
    ASSERT_STREQ(Framerate(Framerate::FPS_48).ToString(),     "48");
    ASSERT_STREQ(Framerate(Framerate::FPS_50).ToString(),     "50");
    ASSERT_STREQ(Framerate(Framerate::FPS_60).ToString(),     "60");
    ASSERT_STREQ(Framerate(Framerate::FPS_96).ToString(),     "96");
    ASSERT_STREQ(Framerate(Framerate::FPS_100).ToString(),    "100");
    ASSERT_STREQ(Framerate(Framerate::FPS_120).ToString(),    "120");
    ASSERT_STREQ(Framerate(Framerate::UNDEFINED).ToString(),  "");
    // clang-format on
}

// Test constructing a Framerate from a string
TEST_F(FramerateTests, FromString)
{
    // clang-format off
    ASSERT_EQ(Framerate("23.976"),   Framerate(Framerate::FPS_23976));
    ASSERT_EQ(Framerate("24"),       Framerate(Framerate::FPS_24));
    ASSERT_EQ(Framerate("25"),       Framerate(Framerate::FPS_25));
    ASSERT_EQ(Framerate("29.97"),    Framerate(Framerate::FPS_2997));
    ASSERT_EQ(Framerate("29.97df"),  Framerate(Framerate::FPS_2997DF));
    ASSERT_EQ(Framerate("29.97DF"),  Framerate(Framerate::FPS_2997DF));
    ASSERT_EQ(Framerate("29.97 df"), Framerate(Framerate::FPS_2997DF));
    ASSERT_EQ(Framerate("29.97 DF"), Framerate(Framerate::FPS_2997DF));
    ASSERT_EQ(Framerate("30"),       Framerate(Framerate::FPS_30));
    ASSERT_EQ(Framerate("30df"),     Framerate(Framerate::FPS_30DF));
    ASSERT_EQ(Framerate("30DF"),     Framerate(Framerate::FPS_30DF));
    ASSERT_EQ(Framerate("30 df"),    Framerate(Framerate::FPS_30DF));
    ASSERT_EQ(Framerate("30 DF"),    Framerate(Framerate::FPS_30DF));
    ASSERT_EQ(Framerate("48"),       Framerate(Framerate::FPS_48));
    ASSERT_EQ(Framerate("50"),       Framerate(Framerate::FPS_50));
    ASSERT_EQ(Framerate("60"),       Framerate(Framerate::FPS_60));
    ASSERT_EQ(Framerate("96"),       Framerate(Framerate::FPS_96));
    ASSERT_EQ(Framerate("100"),      Framerate(Framerate::FPS_100));
    ASSERT_EQ(Framerate("120"),      Framerate(Framerate::FPS_120));
    // clang-format on

    ASSERT_ANY_THROW(Framerate("XXX"));
    ASSERT_ANY_THROW(Framerate(""));
}

// Test if GetEnum method returns expected value
TEST_F(FramerateTests, GetEnum)
{
    // clang-format off
    ASSERT_EQ(Framerate(Framerate::FPS_23976)  .GetEnum(), Framerate::FPS_23976);
    ASSERT_EQ(Framerate(Framerate::FPS_24)     .GetEnum(), Framerate::FPS_24);
    ASSERT_EQ(Framerate(Framerate::FPS_25)     .GetEnum(), Framerate::FPS_25);
    ASSERT_EQ(Framerate(Framerate::FPS_2997)   .GetEnum(), Framerate::FPS_2997);
    ASSERT_EQ(Framerate(Framerate::FPS_2997DF) .GetEnum(), Framerate::FPS_2997DF);
    ASSERT_EQ(Framerate(Framerate::FPS_30)     .GetEnum(), Framerate::FPS_30);
    ASSERT_EQ(Framerate(Framerate::FPS_30DF)   .GetEnum(), Framerate::FPS_30DF);
    ASSERT_EQ(Framerate(Framerate::FPS_48)     .GetEnum(), Framerate::FPS_48);
    ASSERT_EQ(Framerate(Framerate::FPS_50)     .GetEnum(), Framerate::FPS_50);
    ASSERT_EQ(Framerate(Framerate::FPS_60)     .GetEnum(), Framerate::FPS_60);
    ASSERT_EQ(Framerate(Framerate::FPS_96)     .GetEnum(), Framerate::FPS_96);
    ASSERT_EQ(Framerate(Framerate::FPS_100)    .GetEnum(), Framerate::FPS_100);
    ASSERT_EQ(Framerate(Framerate::FPS_120)    .GetEnum(), Framerate::FPS_120);
    // clang-format on
    ASSERT_EQ(Framerate().GetEnum(), Framerate::UNDEFINED);
}

// Test constructing for fps double and drop bool
TEST_F(FramerateTests, FromFpsDoubleAndDropBool)
{
    const double exact23976 = 24 * 1000.0 / 1001.0;
    const double exact2997  = 30 * 1000.0 / 1001.0;

    // clang-format off
    ASSERT_EQ(Framerate(23.976,     false), Framerate(Framerate::FPS_23976));
    ASSERT_EQ(Framerate(exact23976, false), Framerate(Framerate::FPS_23976));
    ASSERT_EQ(Framerate(24,         false), Framerate(Framerate::FPS_24));
    ASSERT_EQ(Framerate(25,         false), Framerate(Framerate::FPS_25));
    ASSERT_EQ(Framerate(29.97,      false), Framerate(Framerate::FPS_2997));
    ASSERT_EQ(Framerate(exact2997,  false), Framerate(Framerate::FPS_2997));
    ASSERT_EQ(Framerate(29.97,      true),  Framerate(Framerate::FPS_2997DF));
    ASSERT_EQ(Framerate(exact2997,  true),  Framerate(Framerate::FPS_2997DF));
    ASSERT_EQ(Framerate(30,         false), Framerate(Framerate::FPS_30));
    ASSERT_EQ(Framerate(30,         true),  Framerate(Framerate::FPS_30DF));
    ASSERT_EQ(Framerate(48,         false), Framerate(Framerate::FPS_48));
    ASSERT_EQ(Framerate(50,         false), Framerate(Framerate::FPS_50));
    ASSERT_EQ(Framerate(60,         false), Framerate(Framerate::FPS_60));
    ASSERT_EQ(Framerate(96,         false), Framerate(Framerate::FPS_96));
    ASSERT_EQ(Framerate(100,        false), Framerate(Framerate::FPS_100));
    ASSERT_EQ(Framerate(120,        false), Framerate(Framerate::FPS_120));
    // clang-format on
}

// Test if GetFrameCount method returns expected value
TEST_F(FramerateTests, GetFrameCount)
{
    // clang-format off
    ASSERT_EQ(Framerate(Framerate::FPS_23976) .GetFrameCount(), 24);
    ASSERT_EQ(Framerate(Framerate::FPS_24)    .GetFrameCount(), 24);
    ASSERT_EQ(Framerate(Framerate::FPS_25)    .GetFrameCount(), 25);
    ASSERT_EQ(Framerate(Framerate::FPS_2997)  .GetFrameCount(), 30);
    ASSERT_EQ(Framerate(Framerate::FPS_2997DF).GetFrameCount(), 30);
    ASSERT_EQ(Framerate(Framerate::FPS_30)    .GetFrameCount(), 30);
    ASSERT_EQ(Framerate(Framerate::FPS_30DF)  .GetFrameCount(), 30);
    ASSERT_EQ(Framerate(Framerate::FPS_48)    .GetFrameCount(), 48);
    ASSERT_EQ(Framerate(Framerate::FPS_50)    .GetFrameCount(), 50);
    ASSERT_EQ(Framerate(Framerate::FPS_60)    .GetFrameCount(), 60);
    ASSERT_EQ(Framerate(Framerate::FPS_96)    .GetFrameCount(), 96);
    ASSERT_EQ(Framerate(Framerate::FPS_100)   .GetFrameCount(), 100);
    ASSERT_EQ(Framerate(Framerate::FPS_120)   .GetFrameCount(), 120);
    ASSERT_EQ(Framerate(Framerate::UNDEFINED) .GetFrameCount(), 0);
    // clang-format on
}

// Test if IsRatio1001 method returns expected value
TEST_F(FramerateTests, IsRatio1001)
{
    // clang-format off
    ASSERT_TRUE (Framerate(Framerate::FPS_23976) .IsRatio1001());
    ASSERT_FALSE(Framerate(Framerate::FPS_24)    .IsRatio1001());
    ASSERT_FALSE(Framerate(Framerate::FPS_25)    .IsRatio1001());
    ASSERT_TRUE (Framerate(Framerate::FPS_2997)  .IsRatio1001());
    ASSERT_TRUE (Framerate(Framerate::FPS_2997DF).IsRatio1001());
    ASSERT_FALSE(Framerate(Framerate::FPS_30)    .IsRatio1001());
    ASSERT_FALSE(Framerate(Framerate::FPS_30DF)  .IsRatio1001());
    ASSERT_FALSE(Framerate(Framerate::FPS_48)    .IsRatio1001());
    ASSERT_FALSE(Framerate(Framerate::FPS_50)    .IsRatio1001());
    ASSERT_FALSE(Framerate(Framerate::FPS_60)    .IsRatio1001());
    ASSERT_FALSE(Framerate(Framerate::FPS_96)    .IsRatio1001());
    ASSERT_FALSE(Framerate(Framerate::FPS_100)   .IsRatio1001());
    ASSERT_FALSE(Framerate(Framerate::FPS_120)   .IsRatio1001());
    ASSERT_FALSE(Framerate(Framerate::UNDEFINED) .IsRatio1001());
    // clang-format on
}

// Test if IsDrop method returns expected value
TEST_F(FramerateTests, IsDrop)
{
    // clang-format off
    ASSERT_FALSE(Framerate(Framerate::FPS_23976)  .IsDrop());
    ASSERT_FALSE(Framerate(Framerate::FPS_24)     .IsDrop());
    ASSERT_FALSE(Framerate(Framerate::FPS_25)     .IsDrop());
    ASSERT_FALSE(Framerate(Framerate::FPS_2997)   .IsDrop());
    ASSERT_TRUE (Framerate(Framerate::FPS_2997DF) .IsDrop());
    ASSERT_FALSE(Framerate(Framerate::FPS_30)     .IsDrop());
    ASSERT_TRUE (Framerate(Framerate::FPS_30DF)   .IsDrop());
    ASSERT_FALSE(Framerate(Framerate::FPS_48)     .IsDrop());
    ASSERT_FALSE(Framerate(Framerate::FPS_50)     .IsDrop());
    ASSERT_FALSE(Framerate(Framerate::FPS_60)     .IsDrop());
    ASSERT_FALSE(Framerate(Framerate::FPS_96)     .IsDrop());
    ASSERT_FALSE(Framerate(Framerate::FPS_100)    .IsDrop());
    ASSERT_FALSE(Framerate(Framerate::FPS_120)    .IsDrop());
    ASSERT_FALSE(Framerate(Framerate::UNDEFINED)  .IsDrop());
    // clang-format on
}

// Test if IsDefined method returns expected value
TEST_F(FramerateTests, IsDefined)
{
    ASSERT_TRUE(Framerate(Framerate::FPS_24).IsDefined());
    ASSERT_FALSE(Framerate().IsDefined());
}

// Test iterator
TEST_F(FramerateTests, IterateFramerates)
{
    Framerate framerates[] = {Framerate::FPS_23976,
                              Framerate::FPS_24,
                              Framerate::FPS_25,
                              Framerate::FPS_2997,
                              Framerate::FPS_2997DF,
                              Framerate::FPS_30,
                              Framerate::FPS_30DF,
                              Framerate::FPS_48,
                              Framerate::FPS_50,
                              Framerate::FPS_60,
                              Framerate::FPS_96,
                              Framerate::FPS_100,
                              Framerate::FPS_120,
                              Framerate::UNDEFINED};

    size_t i = 0;

    for (Framerate::Iterator it = Framerate::values().begin(); it != Framerate::values().end();
         ++it)
    {
        ASSERT_EQ(*it, framerates[i++]);
    }

    ASSERT_EQ(framerates[i], Framerate::UNDEFINED);
#if __cplusplus == 201103L
    i = 0;
    for (auto framerate : Framerate::values())
    {
        ASSERT_EQ(framerate, framerates[i++]);
    }
    ASSERT_EQ(framerates[i], Framerate::UNDEFINED);
#endif
}
