# AURA — Planetary One-Shot Instrument

**D.A Production Instruments** · JUCE 8.0.6 · C++17 · Windows VST3 / Standalone (+ Linux development build)

AURA is an instrument plugin: put it on an instrument/MIDI track, drag a one-shot into the drop area, then play it from MIDI. Each note transposes the sample relative to the root note and plays it as a one-shot, polyphonically. Nine planet profiles apply real DSP chains (filtering, saturation, pitch coloration, transients, resonance, modulation, width, reverb) rather than cosmetic changes.

## Features
- **UI** drawn natively in JUCE (C++), reproducing the approved reference `Assets/AURA_UI_Exact_Reference.png`: proportions, panels, typography, planet orbit display, and keyboard.
- **One-shot loading**: drag-and-drop zone plus file-browser fallback; accepts WAV/AIFF/FLAC/OGG; shows sample name, waveform, duration and status; invalid/missing files are rejected or reported gracefully; all decoding happens off the audio thread.
- **Playback**: chromatic MIDI playback with root-note control, note on/off, per-voice ADSR, velocity, 16-voice polyphony with last-note-priority voice stealing, sample-accurate pitch shifting.
- **Planet profiles**: Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, Neptune, Pluto — each maps to a distinct, audible DSP preset built from the same control set.
- **Controls wired to real DSP**: Gravity, Atmosphere, Temperature, Density, Brightness, Harmonics, Reverb, Width, Orbital Reverb Size/Decay. All are APVTS parameters (automatable), smoothed, and click-free.
- **Live visualizations** driven from the actual audio thread: spectrum, partials, envelope meter, orbital reverb animation, per-key note activity, IN/OUT meters — bounded allocations, lock-free UI/audio hand-off.
- **State**: full state save/restore including sample path restoration with graceful handling of missing files.

## Build

### Linux (development + headless test build) — verified
JUCE 8.0.6 is fetched by CMake on first configure (needs internet). The plugin target plus a headless `AURA_SelfTest` console app build without X11.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/AURA_artefacts/Release/AURA_SelfTest   # 49/49 checks pass
```

Artifacts land in `build/AURA_artefacts/Release/` (`AURA.vst3`, `AURA Standalone`).

### Windows x64 VST3 — cross-build from Linux, link-verified
A full Windows cross-build toolchain ships in `tools/` (zig 0.17 `cc`/`c++` shims as `x86_64-w64-mingw32-*`, a CMake toolchain file, a small Win32 header gap-fill set, and a deterministic `moduleinfo.json` stub). No Visual Studio or Windows machine is required:

```bash
cmake -S . -B build-win -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_TOOLCHAIN_FILE=tools/cmake-windows-x86_64.cmake
cmake --build build-win -j
```

Artifacts: `build-win/AURA_artefacts/Release/VST3/AURA.vst3/` (PE32+ x86-64 DLL `Contents/x86_64-win/AURA.vst3` + `Contents/Resources/moduleinfo.json`) and `Standalone/AURA.exe`.

Honest status of the Windows build: every translation unit (JUCE modules + AURA) compiles for x86_64-windows, and both final binaries **link cleanly** — the VST3 binary contains the `GetPluginFactory` VST3 entry point and imports the expected system DLLs (kernel32/user32/gdi32/ole32/dxgi/d3d11/d2d1/winmm/ws2_32/...). It has **not** been executed on a Windows DAW or VST host (no Windows machine in this environment); treat it as build-and-link verified, pending host-side smoke testing. Cross-build notes: LTO is disabled in the `build-win` tree (zig's LTO bitcode members are not resolved by lld from MS-format archives), the VST3 manifest helper is replaced by the deterministic `tools/moduleinfo-stub.json`, and `tools/win-gaps/` supplies a few msvcrt/ole32 symbols that zig's CRT references but does not provide.

### GitHub Actions
`.github/workflows/build-windows-vst3.yml` builds the Windows x64 VST3 on a Windows runner (the production path); the local cross-build above is a convenience for development without a Windows machine.

## Testing (actual results)
- **`AURA_SelfTest` (Linux, headless): 49/49 checks pass**, covering: sample loading (valid/invalid/missing), WAV/AIFF/FLAC/OGG decode, pitch transposition math at several octaves, ADSR envelope values, velocity scaling, polyphonic allocation and voice stealing, all nine planet profiles producing audibly distinct output (per-planet checksums), parameter automation round-trip, full state save/restore including sample path, and crash-safety probes (allocation-free and lock-free checks in the processBlock path).
- MIDI note on/off, note activity, and IN/OUT metering are exercised in the self-test and by direct editor instrumentation.
- **Not tested**: real DAW/VST-host integration on Windows (no Windows host available here); long-duration soak runs beyond the self-test.

## Changed files (this implementation)
- `Source/PluginProcessor.{h,cpp}` — audio engine: one-shot sampler, 16-voice polyphony + stealing, root note/ADSR/velocity, planet DSP graph, APVTS state, sample path restore, RT-safe processBlock.
- `Source/PlanetDSP.h` — the per-planet DSP blocks (filter/saturate/pitch-color/transient/resonance/modulation/width/reverb chains).
- `Source/PlanetProfiles.h` — the nine planet parameter presets.
- `Source/PluginEditor.{h,cpp}` — the JUCE UI reproducing the approved reference (panels, orbit display, keyboard, meters).
- `Source/UIWidgets.h` — custom widgets (orbit planet selector, spectrum/partials/envelope visualizations, meters).
- `Source/EditorFactory.cpp` — editor factory.
- `Source/AuraTests.cpp` — headless self-test (49 checks).
- `CMakeLists.txt` — plugin definition, SelfTest target, JUCE fetch, and the Windows cross-build wiring.
- `tools/` — Windows cross-build toolchain: `cmake-windows-x86_64.cmake`, `mingw-shim/` compiler/windres/ranlib shims, `win-gaps/` (Dbghelp.h, CRT/ole32 gap fillers, uuidof gap filler), `aura_stub_rc.cmake`, `moduleinfo-stub.json`, `throwhook`/probe utilities.
- `Assets/` — reference images preserved untouched.

## Important notes
- The planet selection activates real DSP profiles; no control is decorative.
- The Windows cross-build is link-verified, not host-verified (see above). The GitHub Actions Windows build is the path to a host-tested release binary.
- AURA is a sampler instrument: insert it on an instrument/MIDI track.

## UI reference assets
- `Assets/AURA_UI_Exact_Reference.png` — the exact approved reference image (definitive visual spec).
- `Assets/AURA_UI_Exact_Reference.svg` — SVG wrapper embedding that same image.
- `Assets/AURA_UI_Concept.svg` — separate scalable vector concept mockup.
