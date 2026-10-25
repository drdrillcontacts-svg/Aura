#pragma once
#include <cmath>
#include <array>

namespace aura
{

// Nine artistic planet sound-design profiles.
// These are creative interpretations, not scientific measurements.
//
// "user" targets are the values the UI parameters animate to when a planet
// is selected (Gravity, Atmosphere, Temperature, Density, Brightness,
// Harmonics, Reverb, Width, Orbital Size, Orbital Decay seconds).
//
// "char" is the per-planet character layer that is cross-faded inside the
// audio engine when the planet changes:
//   peakHz / peakDb / peakQ  - tonal body (peaking EQ)
//   chorusMs / chorusHz      - stereo modulation (depth in ms, LFO rate)
//   airDb                    - high shelf (airy / dark)
//   driveAdd                 - extra harmonic saturation
//   levelDb                  - overall planet level
struct PlanetProfile
{
    const char* name;
    const char* descriptor;

    // user-facing targets (0..1 unless noted)
    float gravity, atmosphere, temperature, density;
    float brightness, harmonics, reverb, width;
    float orbSize, orbDecaySeconds;

    // engine character
    float peakHz, peakDb, peakQ;
    float chorusMs, chorusHz;
    float airDb, driveAdd, levelDb;
};

inline const std::array<PlanetProfile, 9>& getPlanetProfiles()
{
    static const std::array<PlanetProfile, 9> profiles = {
        // Mercury - dry, metallic, precise
        PlanetProfile{
            "Mercury", "Dry  •  Metallic  •  Precise",
            0.80f, 0.20f, 0.55f, 0.35f, 0.78f, 0.45f, 0.10f, 0.55f, 0.30f, 0.5f,
            3200.0f, 3.0f, 1.2f, 4.0f, 1.8f, 4.0f, 0.6f, -1.0f
        },
        // Venus - warm, veiled, hot
        PlanetProfile{
            "Venus", "Warm  •  Veiled  •  Hot",
            0.65f, 0.75f, 0.70f, 0.55f, 0.45f, 0.55f, 0.35f, 0.70f, 0.55f, 2.2f,
            900.0f, 2.5f, 1.0f, 8.0f, 0.4f, -1.0f, 1.0f, 0.0f
        },
        // Earth - balanced, natural, organic (default)
        PlanetProfile{
            "Earth", "Warm  •  Natural  •  Organic",
            0.68f, 0.50f, 0.45f, 0.60f, 0.62f, 0.48f, 0.35f, 0.70f, 0.68f, 1.8f,
            1800.0f, 1.0f, 0.8f, 0.0f, 0.0f, 0.0f, 0.3f, 0.0f
        },
        // Mars - dusty, red, dry
        PlanetProfile{
            "Mars", "Dusty  •  Red  •  Dry",
            0.72f, 0.30f, 0.50f, 0.50f, 0.58f, 0.60f, 0.15f, 0.60f, 0.40f, 1.2f,
            1200.0f, 4.0f, 2.2f, 3.0f, 0.9f, 2.0f, 1.2f, -0.5f
        },
        // Jupiter - massive, deep, wide
        PlanetProfile{
            "Jupiter", "Massive  •  Deep  •  Wide",
            0.90f, 0.55f, 0.55f, 0.80f, 0.45f, 0.70f, 0.50f, 0.90f, 0.90f, 3.5f,
            110.0f, 6.0f, 0.9f, 10.0f, 0.2f, -2.0f, 1.4f, 0.0f
        },
        // Saturn - round, warm, ringed
        PlanetProfile{
            "Saturn", "Round  •  Warm  •  Ringed",
            0.78f, 0.60f, 0.60f, 0.65f, 0.52f, 0.58f, 0.40f, 0.80f, 0.75f, 2.6f,
            220.0f, 4.0f, 1.1f, 12.0f, 0.3f, 0.0f, 0.8f, -0.5f
        },
        // Uranus - cool, icy, airy
        PlanetProfile{
            "Uranus", "Cool  •  Icy  •  Airy",
            0.55f, 0.65f, 0.25f, 0.45f, 0.80f, 0.35f, 0.45f, 0.85f, 0.85f, 3.0f,
            4500.0f, 4.0f, 1.4f, 14.0f, 0.12f, 6.0f, 0.4f, -0.5f
        },
        // Neptune - deep, cold, stormy
        PlanetProfile{
            "Neptune", "Deep  •  Cold  •  Stormy",
            0.82f, 0.70f, 0.18f, 0.70f, 0.38f, 0.62f, 0.55f, 0.88f, 0.95f, 4.0f,
            800.0f, 3.0f, 1.2f, 16.0f, 0.15f, -4.0f, 1.1f, -0.5f
        },
        // Pluto - distant, small, ghostly
        PlanetProfile{
            "Pluto", "Distant  •  Small  •  Ghostly",
            0.50f, 0.85f, 0.20f, 0.30f, 0.50f, 0.40f, 0.70f, 0.92f, 1.0f, 5.5f,
            600.0f, 2.0f, 1.5f, 18.0f, 0.1f, 2.0f, 0.5f, -3.0f
        },
    };
    return profiles;
}

// Visual order of the planets in the orbit display. The approved reference
// art arranges them [Mercury, Venus, Jupiter, Mars, EARTH (centre), Saturn,
// Uranus, Neptune, Pluto] - the centre slot is Earth, not the 3rd planet.
inline std::array<int, 9> getOrbitVisualOrder()
{
    // visual slot -> profile index
    return { 0, 1, 4, 3, 2, 5, 6, 7, 8 };
}

} // namespace aura
