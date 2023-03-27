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
#include <tcutils/Timecode.h>
#include <tcutils/Version.h>

using namespace testing;
using namespace Dolby::TcUtils;

class VersionTest : public Test
{
};

TEST_F(VersionTest, RuntimeVsCompileNumbers)
{
    ASSERT_EQ(Version::GetMajor(), DOLBY_TCUTILS_MAJOR);
    ASSERT_EQ(Version::GetMinor(), DOLBY_TCUTILS_MINOR);
    ASSERT_EQ(Version::GetPatch(), DOLBY_TCUTILS_PATCH);
}

TEST_F(VersionTest, RuntimeVsCompileString)
{
    std::string s;
    s = std::to_string(DOLBY_TCUTILS_MAJOR) + "." + std::to_string(DOLBY_TCUTILS_MINOR) + "." +
        std::to_string(DOLBY_TCUTILS_PATCH);
    ASSERT_STREQ(Version::GetString(), s.c_str());
}
