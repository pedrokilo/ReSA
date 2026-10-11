#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <bit>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <optional>
#include <vector>
#include "app/platform/win/winincl.h"
struct tEffectInterior;
#include "Interior_c.h"
#include "FurnitureManager_c.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// 0x59A140
void Interior_c::Shop_AddShelfInfo(int32 x, int32 y, int32 direction) {
    auto& counter = *reinterpret_cast<uint32*>(0x8D0948);
    if (std::bit_cast<int32>(counter) > 1) {
        // This is the external CRT rand collaborator, not a game method body.
        const uint32 draw = uint32(reinterpret_cast<int(__cdecl*)()>(0x821B1E)()) & 0xFFFF;
        const float chance = float(draw) * (1.0f / 32768.0f) * 100.0f;
        if (int32(chance) > 60) {
            AddInteriorInfo(8, float(x), float(y), direction, nullptr);
            counter = 1;
            return;
        }
    }
    // EXE INC wraps all bit patterns, including INT_MAX. Unsigned avoids UB.
    ++counter;
}
