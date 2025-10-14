README
======

I am currently directing interested parties to this project instead => https://github.com/EnemyV/EnemyNationsRevival.git

This is the source code of the game [Enemy Nations](http://enemynations.com)
with some of the [content](http://timeoutd.net/EnemyNationsSource.rar) now added. Keep in mind, everything here remains the copyright of Windward.  I have packaged the additional media for testing the updated code.  The company can hopefully benefit from anything we do here if or when they decide to make an Enemy Nations Two.

For a historical overview of the game's development, legal battles, and
open-source revival, see [HISTORY.md](HISTORY.md).

There is also a [very old binary package](http://timeoutd.net/enemy_nations.iso.gz) available that I actually got working
in windows 7 and probably also in wine, but it requires some fiddling. 
Message me if you need help.

See license.txt for the license.

Goals and roadmap
=================

Well, ultimately to port it to linux and update the graphics and udpade the
weird userinterface to something more usable.

A living roadmap that expands on these themes now lives in
[ROADMAP.md](ROADMAP.md).  It tracks the remaining gaps for Linux support,
modern rendering, UI upgrades, and supporting infrastructure.

But step by step:

1. Update to compile with Visual C++ 2010 and remove CD check
2. Port to Linux
3. Enhance rendering engine and graphics
4. Enhance userinterface

--Marenz


Related
=======

There is also the [Enemy Nations Revival Project](http://groups.google.com/forum) which seems pretty dead though.


Building on modern Windows
==========================

The repository now ships with a first-class CMake toolchain that generates a
Visual Studio 2022 solution on demand.  The project file list is no longer
hand-maintained: the `src/CMakeLists.txt` script parses `src/enations.dsp`
directly, ensuring that every source and resource listed in the legacy
project is built automatically.

Prerequisites
-------------

* Windows 10 or Windows 11.
* [Visual Studio 2022](https://visualstudio.microsoft.com/vs/) with the
  **Desktop development with C++** workload and the optional MFC component.
* [CMake 3.20 or newer](https://cmake.org/download/).  The Visual Studio
  installer ships with a compatible CMake build.

Configuring and building
------------------------

1. Launch a "x86 Native Tools Command Prompt for VS 2022" (the game is still
   a Win32 application).
2. Generate the Visual Studio solution and project files:

   ```cmd
   cmake -S src -B build -G "Visual Studio 17 2022" -A Win32
   ```

3. Build from the command prompt or open the generated `build/EnemyNations.sln`
   in the Visual Studio IDE:

   ```cmd
   cmake --build build --config Release
   ```

Open-source replacements for proprietary middleware
---------------------------------------------------

The original build linked against proprietary libraries such as
`shmfc4m.lib`, `shlw32m.lib`, `wind40.lib`, `mss32.lib`, and `vdmplay.lib`.
Those dependencies have been replaced by the in-tree `windward` static
library, which now compiles from source and provides stubbed
implementations of the Miles Sound System (`mss_stub.cpp`) and VDMPlay
(`vdmplay_stub.cpp`).  No external binary blobs are required to link the
game when using the new build system.

When the build completes, Visual Studio places the game executable in the
`build/Release` directory (or `build/Debug` when selecting the Debug
configuration).
