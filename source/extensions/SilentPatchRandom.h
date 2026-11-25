#pragma once

#include <cstdint>
#include <ctime>

/*
 * SilentPatch (MIT, Copyright (c) 2024 Adrian Zdanowicz (Silent); ver docs/THIRD_PARTY_NOTICES.md): rand() de 31 bits como el de la PS2
 * (adaptado de SilentPatch/Random.h, ConsoleRandomness::rand31). Lo usan los arreglos que dependian de un rand() de 31 bits que en PC solo da 15.
 */
namespace SilentPatchRandom {
inline uint64_t seed = static_cast<uint64_t>(std::time(nullptr));
inline int Rand31() {
    seed = 0x5851F42D4C957F2D * seed + 1;
    return static_cast<int>((seed >> 32) & 0x7FFFFFFF);
}
} // namespace SilentPatchRandom
