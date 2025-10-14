# Enemy Nations Roadmap

The roadmap documents the next milestones required to get Enemy Nations into a
healthy, cross-platform state.  It complements the historical notes in
[HISTORY.md](HISTORY.md) and the build documentation in [README.md](README.md).

## 1. Cross-platform foundation

### 1.1. Linux build enablement
- Replace Windows-only MFC dependencies with portable UI abstractions (e.g.
  wxWidgets or Dear ImGui) or add a thin SDL-based shim for an initial port.
- Introduce platform-neutral entry points for window creation, message loops,
  timers, and file dialogs.
- Stub or wrap Win32 synchronization primitives (`CRITICAL_SECTION`, events) in
  terms of `std::mutex`, `std::condition_variable`, and `std::thread`.
- Isolate direct Win32 API calls behind platform layers to simplify POSIX
  implementations.

### 1.2. Toolchain and packaging
- Add CMake presets for GCC and Clang builds on Linux.
- Provide scripts or container definitions to fetch the required assets and run
  smoke tests headlessly.
- Package data files separately from the executable to simplify distribution via
  Flatpak, AppImage, or traditional package managers.

## 2. Rendering and audio modernization
- Investigate migrating the software voxel renderer to OpenGL (desktop and
  GLES) while keeping a compatibility mode for legacy hardware.
- Replace the Miles Sound System stubs with SDL_mixer or OpenAL-based playback,
  making sure MIDI and positional audio have drop-in replacements.
- Audit texture and sprite loading paths to ensure they run on little- and
  big-endian architectures.

## 3. UI/UX overhaul
- Redesign the main shell using responsive layouts that adapt to modern desktop
  resolutions.
- Improve accessibility with scalable fonts, keyboard navigation, and remappable
  hotkeys.
- Document the UI architecture to guide community contributions and mod support.

## 4. Gameplay and systems polish
- Expand automated tests in `src/tests` to cover AI regression cases and economy
  simulations.
- Revisit balance data for campaign missions to account for community feedback.
- Add save-game versioning so future patches can remain backward compatible.

## 5. Project health and infrastructure
- Stand up continuous integration that builds on Windows and Linux, runs unit
  tests, and packages nightly artifacts.
- Publish contribution guidelines and code-style references to lower the barrier
  for new contributors.
- Track bugs and feature requests in GitHub Discussions or Issues with labels
  aligned to the roadmap categories above.

Contributions that chip away at any of these items—especially the Linux porting
work—are welcome.  Please coordinate major architectural efforts through issues
or discussions before starting to avoid duplicated work.
