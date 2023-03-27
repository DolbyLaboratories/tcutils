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

using namespace testing;

// In order to run a full stress test, set the environment variable
//   TCUTILS_FULL_STRESS_TEST=YES
// before running the test. This will run some of the tests that otherwise only test for a limit
// amount of values on all possible values (e.g. framerates, sample-positions from 0 to 24
// hours), which would take too long for standard unit testing, but is useful to proof the
// implementation is correct for all cases.

bool FullStressTest::Helper()
{
    const char* ev = getenv("TCUTILS_FULL_STRESS_TEST");
    return (ev && strcmp(ev, "YES") == 0);
}

bool FullStressTest::Active()
{
    static bool ret = Helper();
    return ret;
}

int main(int argc, char** argv)
{
    if (FullStressTest::Active())
    {
        std::cout << "======== RUNNING FULL STRESS TEST, THIS WILL BE SLOW ========" << std::endl;
    }
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
