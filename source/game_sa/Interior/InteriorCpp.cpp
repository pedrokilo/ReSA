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

// 0x599A30
// Integer layout and return value match the EXE, including negative remainders.
int32 Interior_c::Office_FurnishCenter() {
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    const int32 width = int32(box[2]) - 6;
    const int32 height = int32(box[3]) - 6;
    const int32 columns = width / 6;
    const int32 rows = height / 6;
    const int32 insetX = (width % 6) / 2;
    const int32 insetY = (height % 6) / 2;
    if (width > 0 && height > 0 && columns > 0) {
        for (int32 column = 0; column < columns; ++column) {
            for (int32 row = 0; row < rows; ++row) {
                Office_PlaceDeskQuad(-1, insetX + 6 + column * 6,
                    insetY + 6 + row * 6, int32(m_furnitureId));
            }
        }
    }
    // 0x599ACB reloads this value after each populated column. Empty paths
    // retain the same EAX computed at 0x599A8C.
    return insetY;
}

// 0x59A130
void Interior_c::Shop_FurnishCeiling() {
}


// 0x599AF0
void Interior_c::FurnishOffice() {
    const auto dimension = [this](unsigned offset) {
        return int32(reinterpret_cast<const uint8*>(m_box)[offset]);
    };
    SetTilesStatus(0, 0, 2, 2, 2, 0);
    SetTilesStatus(0, dimension(3) - 2, 2, 2, 2, 0);
    SetTilesStatus(dimension(2) - 2, 0, 2, 2, 2, 0);
    SetTilesStatus(dimension(2) - 2, dimension(3) - 2, 2, 2, 2, 0);
    m_furnitureId = int8(g_furnitureMan.GetRandomId(1, 0, uint8(dimension(0x1F))));
    field_792 = int8(g_furnitureMan.GetRandomId(1, 1, uint8(dimension(0x1F))));
    Office_FurnishEdges();
    Office_FurnishCenter();
    Shop_FurnishCeiling();
}

