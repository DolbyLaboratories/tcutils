# tcutils 1.3

A utility library for timecode management

## Introduction

In physics the unit of measurement of time is seconds, in the digital audio domain we use samples. Things become more
complex when we are dealing with video, where we use video frames, expressed in timecode, running at various rates. This
library deals with these different formats and the conversion between them. Special importance was given to making sure
that these conversions are symmetrical (e.g. when going from timecode to sample position and back) and we have been
using Pro Tools as a reference and checked conversions against its transport. The functions in this library are covered
by unit tests and can also be applied with the included command line utility.

## Concepts

The following concepts are used in this library:

- Framerate

  Framerate, expressed in frames per seconds (FPS), refers to the amount of pictures (frames) that are displayed
  consecutively during one second, to obtain a moving image. The common cinematic framerate is 24 FPS, PAL video uses 25
  FPS and NTSC video originally used 30 FPS for black and white. For technical reasons (to add sound in the case of film
  and color in the case of video), these rates were lowered slightly to include additional information while maintaining
  the bandwidth, resulting in 23.976 FPS and 29.97 FPS. It should be noted, that these numbers are an approximation; the
  exact framerates are 24 * 1000 / 1001 and 30 * 1000 / 1001.

- Timecode

  We could refer to a specific frame position by its number, but it is more common - and often more practical - to use
  timecode: hours, minutes, seconds and frames, written in the format hh:mm:ss:ff (or hh:mm:ss;ff for drop-frame
  framerates). As we know from the framerate how many frames go in one second, we can deduce the frame position from the
  timecode and vice versa.

- Real-time

With real-time we refer to actual time in seconds, also known as the wallclock time.

- NTSC-time

  As explained above, with the NTSC framerates each frame takes 1001/1000 times longer than with the corresponding
  integer framerates. We can therefore consider that at these framerates time runs 1001/1000 times slower than
  real-time. We can express time by applying this ratio, calling it NTSC-time. For the integer frame-rates (24, 25 and
  30), NTSC-time equals the real-time (or we could say that NTSC-time does not apply). For non-integer frame-rates (
  23.976, 29.97), the NTSC-time gradually drifts away from real-time. Another way of looking at it, is considering that
  the timecode value at non-integer framerates, corresponds with the NTSC-time. E.g, a timecode of 01:00:00:00 at 29.97
  FPS is 3600.0 seconds in NTSC-time, but 3603.6 seconds in real-time. Note however that this does not hold for DF (
  drop-frame) framerates.

- Drop-frames

  To account for the difference between timecode at the non-integer frame-rate 29.97 and the wallclock time the concept
  of drop-frames was introduced: By not counting frames at a specific interval - similar to how leap years are used to
  keep the calendar closer to the actual astronomical year - the timecode periodically moves closer to the wallclock,
  and occasionally is an exact match: By skipping (or *dropping*) 2 frames every minute, except every tenth minute, the
  timecode of 01:00:00:00 at 29.97df, for example, ends up (nearly) at 3600 seconds instead of at 3603.6 seconds. To be
  clear, no actual frames are dropped, it's only their timecode numbering that is skipped.

  Another way of looking at this: The duration in real-time of 1000 frames at 29.97 FPS corresponds with 1001 frames at
  30 FPS. So in order to make the timecodes at these two rates coincide after this period, we would have to drop one
  timecode value at 29.97 FPS. This would however not be very practical; the positions of these drop-frames would fall
  at very unpredictable timecodes (e.g. timecode 00:00:33:10 would have to be dropped). Now, consider this: 10 minutes (
  timecode minutes that is) consist of 10 * 60 * 30 = 18000 frames. During these 10 minutes we need to drop 18 frames:
  dropping 2 frames at the start of every minute except for the first of those 10 gives us these 18 frames and is much
  easier to comprehend.

- DAMF-time

  As explained above, the concept of NTSC-time is convenient as there is a close relation with the timecode value.
  However, for drop frame framerates this doesn't hold true, as these are intended to bring the timecode value closer to
  real-time. DAMF-time, which is used in the Dolby DAMF file format, is the same as NTSC-time except for drop-frame
  framerates: DAMF-time skips the drop frames, just like we do for drop-frame timecode.

  To illustrate this with an example: At 29.97 FPS, timecode 01:00:00:00 corresponds with NTSC-time (and DAMF-time)
  3600.0 and with real-time 3603.60036003... At 29.97DF FPS, at timecode 01:00:00:00 a total of 108 have been dropped,
  bringing it closer to real-time 3600.0 (3599.9964 to be exact) but leaving us with NTSC-time 3596.4. For DAMF-time we
  take the dropped frames into account, so the DAMF-time corresponding with 01:00:00:00 is in fact 3600.0 for both 29.97
  and 29.97DF.

## Classes

The classes provided by this library are:

- Timecode

  This class represents (video) frames in the common hh:mm:ss:ff format (or hh:mm:ss;ff for drop-frame frame-rates).

  (Note that 3 digits frames are also supported for 120 FPS)

- TimecodeSubframes

  This class encapsulates Timecode and 'subframes', a representation of remaining fraction of a frame with a given
  denominator (typically 80 or 100). The given time is rounded to the nearest subframe. Note that when the last subframe
  is rounded up, the Timecode will be the next frame with 0 subframes.

- Framerate

  This class encapsulates an enum with the most common frame-rates and functions to access its inherent properties.

## Compilation

TcUtils uses the CMake build system. (All commands below as executed from the TcUtils root directory.)

    ```
    $ mkdir build
    $ cd build
    $ cmake ..
    $ cmake --build .
    ```

For unit tests, TcUtils uses GTest. If you include TcUtils in a project that already includes GTest, it will be used. It
is also possible to point CMake to a GTest installation, using the CMAKE_PREFIX_PATH variable:

    ```
    $ mkdir build
    $ cd build
    $ cmake .. -DCMAKE_PREFIX_PATH=<folder-with-gtest-installation>
    $ cmake --build .
    ```

This can also be done using the provided external/CMakeLists.txt which uses the CMake ExternalProject functionality to
download and build GTest:

    ```
    $ mkdir build_external
    $ cd build_external
    $ cmake ../external/
    $ cmake --build .
    ```

Now you can build tcutilities as before, using:

    ```
    $ cmake .. -DCMAKE_PREFIX_PATH=../build_external/install
    ```

When compiling for the Apple Silicon architecture, you also need to specify the architecture adding this option to the
configuration cmake commands: `-DCMAKE_OSX_ARCHITECTURES="arm64"`. For example:

    ```
    $ cmake ../external/ -DCMAKE_OSX_ARCHITECTURES="arm64"
    ```

Note that the above can also be done by invoking the provided ./build.py python script.

## tcutility

We include a command line utility tcutility. For a given time value, tcutility will show the corresponding values in
other formats. To get familiar with the concepts above, we recommend trying out some conversions.

```
Usage: [time] <framerate> <samplerate> <rounding-mode>

  time is required and can be given in the following formats:

    hh:mm:ss:ff             timecode
    t_real=floating-point   real-time in seconds
    t_damf=floating-point   DAMF-time in seconds
    frames=integer          frame count
    sample=integer          sample position

  framerate, samplerate and rounding-mode are optional

  framerate can be one of the following:

    23.976
    24 (default)
    25
    29.97
    29.97df
    30
    30df
    48
    50
    60
    96
    100
    120

  samplerate default is 48000

  rounding-mode can be nearest and truncate. Default is nearest
```
