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

#include <tcutils/FrameBoundary.h>

using namespace Dolby::TcUtils;

Samples FrameBoundary::Floor(Framerate framerate, Samples samples, Samplerate samplerate)
{
    const double dsamplerate      = samplerate.GetValue();
    const double dframeCount      = static_cast<double>(framerate.GetFrameCount());
    const double framesPerSample  = dframeCount / dsamplerate;
    const double samplesPerFrames = dsamplerate / dframeCount;

    double dsamples = static_cast<double>(samples.GetValue()) + 0.5;
    dsamples        = framerate.ApplyRatio(dsamples);

    const int32_t frames = static_cast<int32_t>(dsamples * framesPerSample);

    return Samples(
        static_cast<int64_t>(framerate.ApplyInverseRatio(frames * samplesPerFrames) + 0.5));
}

Samples FrameBoundary::Round(Framerate framerate, Samples samples, Samplerate samplerate)
{
    const double dsamplerate      = samplerate.GetValue();
    const double dframeCount      = static_cast<double>(framerate.GetFrameCount());
    const double framesPerSample  = dframeCount / dsamplerate;
    const double samplesPerFrames = dsamplerate / dframeCount;

    double dsamples = static_cast<double>(samples.GetValue()) + 0.5;
    dsamples        = framerate.ApplyRatio(dsamples);

    const int32_t frames = static_cast<int32_t>(dsamples * framesPerSample);

    const int64_t samplesFloor = static_cast<int64_t>(
        framerate.ApplyInverseRatio(static_cast<double>(frames) * samplesPerFrames) + 0.5);
    const int64_t samplesCeiling = static_cast<int64_t>(
        framerate.ApplyInverseRatio(static_cast<double>(frames + 1) * samplesPerFrames) + 0.5);

    return Samples(samples.GetValue() - samplesFloor < samplesCeiling - samples.GetValue()
                       ? samplesFloor
                       : samplesCeiling);
}

Samples FrameBoundary::Ceiling(Framerate framerate, Samples samples, Samplerate samplerate)
{
    const double dsamplerate      = samplerate.GetValue();
    const double dframeCount      = static_cast<double>(framerate.GetFrameCount());
    const double framesPerSample  = dframeCount / dsamplerate;
    const double samplesPerFrames = dsamplerate / dframeCount;

    double dsamples = static_cast<double>(samples.GetValue()) + 0.5;
    dsamples        = framerate.ApplyRatio(dsamples);

    const int32_t frames = static_cast<int32_t>(dsamples * framesPerSample);

    const int64_t samplesFloor = static_cast<int64_t>(
        framerate.ApplyInverseRatio(static_cast<double>(frames) * samplesPerFrames) + 0.5);
    const int64_t samplesCeiling = static_cast<int64_t>(
        framerate.ApplyInverseRatio(static_cast<double>(frames + 1) * samplesPerFrames) + 0.5);

    return Samples(samples.GetValue() == samplesFloor ? samplesFloor : samplesCeiling);
}

bool FrameBoundary::IsOnBoundary(Framerate framerate, Samples samples, Samplerate samplerate)
{
    return samples.GetValue() == Floor(framerate, samples, samplerate).GetValue();
}
