# 1.3.0

- PRAU-6084: TimecodeSubframes rounds to nearest subframe.
  The previous method was using ceiling in the first half part of the frame and floor in the second half.
- PRAU-6082: tcutils utility uses the given time, not the rounded given time and includes Subframes.

# 1.2.0

- Initial public release in Github
