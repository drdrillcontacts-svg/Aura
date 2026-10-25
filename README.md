# AURA — Planetary One-Shot Instrument

**D.A Production Instruments** · JUCE 8 · C++17 · Windows VST3 / Standalone

AURA is an instrument plugin: insert it on an instrument/MIDI track, drag a one-shot into the drop area, then play it from MIDI. Each note transposes the sample relative to MIDI note 60 (C4) and plays it as a one-shot, polyphonically.

## Current prototype features
- Dark planetary/orbital interface inspired by the approved concept.
- Drag-and-drop or file-picker loading for WAV, AIFF, FLAC and OGG (supported by JUCE format readers available in the build).
- 16-voice polyphonic one-shot sample playback.
- Planet selector and tone controls, parameter state saving, and sample path restoration.
- GitHub Actions workflow to compile a Windows x64 VST3.

## Build
Requirements: CMake 3.22+, Visual Studio 2022 C++ workload, Git, and internet access on the first configure (JUCE is fetched by CMake).

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --target AURA_VST3
```

The VST3 is normally produced under `build/AURA_artefacts/Release/VST3/`.

## Important prototype notes
This is a first coding milestone, not yet a production-ready commercial instrument. The planet selection currently changes the selected visual world; the controls are wired to plugin parameters, but several are intentionally scaffolding for the next DSP pass. The first sampler engine includes basic resampling and one-shot polyphony. The next engineering pass should add high-quality bandlimited resampling, envelopes and choke/voice-stealing policies, per-planet DSP models, actual orbital reverb/width, sample start/end and root-note controls, waveform display, preset management, and automated audio tests.

## Usage
1. Add AURA to a MIDI/instrument track in your DAW.
2. Drag a one-shot audio file onto the plugin, or press **LOAD ONE-SHOT**.
3. Send MIDI notes from your keyboard or piano roll. C4 (MIDI 60) plays the sample at its original pitch; each semitone transposes it.
4. Adjust the sound controls to shape the tone.

AURA is a sampler instrument, not an audio effect for a regular audio-only piano track. To use it with MIDI piano performance, insert it on an instrument track and play the MIDI input through AURA. A later version can add an audio-effect mode for processing a piano's live audio.


## UI concept asset
- `Assets/AURA_UI_Concept.svg` — scalable SVG mockup of the approved AURA visual direction, including the planetary selector, sound controls, one-shot drop zone and keyboard. This is a visual design reference; the JUCE editor is currently drawn natively in C++ and does not yet load this SVG as its runtime skin.


## Exact approved UI reference
- `Assets/AURA_UI_Exact_Reference.png` — exact uploaded reference image.
- `Assets/AURA_UI_Exact_Reference.svg` — SVG wrapper embedding that same image, so it displays the exact reference rather than a reinterpreted mockup.
- `Assets/AURA_UI_Concept.svg` — separate scalable vector concept mockup.
The exact-reference SVG is an image wrapper, not a vector-traced/editable recreation. The JUCE runtime interface is still rendered in C++ and has not yet been made pixel-identical to the reference.
