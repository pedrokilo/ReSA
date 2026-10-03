#include "StdInc.h"

#include "MissionTextOffsets.h"

CMissionTextOffsets::CMissionTextOffsets() {
    *reinterpret_cast<uint16*>(&m_size) = 0; // Como el original (inlined en CText::CText 0x6A00F0): solo la palabra baja
}

// 0x69F670
bool CMissionTextOffsets::Load(uint32 length, FILESTREAM file, uint32* offset, int32 unknown) {
    // Como el original (0x69F670): lee byte a byte sin comprobar errores (8 bytes de nombre y 4 de
    // desplazamiento por entrada), con contador de 16 bits, y al final escribe solo la palabra baja de m_size.
    const auto count = length / sizeof(tMissionTextOffset);
    for (uint16 i = 0; i < count; i++) {
        auto& entry = m_offsets[i];
        for (auto k = 0; k < 8; k++) {
            uint8 c;
            CFileMgr::Read(file, &c, 1);
            entry.szMissionName[k] = (char)c;
            ++*offset;
        }
        uint8 raw[4];
        for (auto k = 0; k < 4; k++) {
            CFileMgr::Read(file, &raw[k], 1);
            ++*offset;
        }
        entry.offset = *reinterpret_cast<uint32*>(raw);
    }
    *reinterpret_cast<uint16*>(&m_size) = (uint16)count;
    // Como el original: AL queda con el byte bajo de `this` tras el bucle (0 si no había entradas)
    return std::bit_cast<bool>(count ? (uint8)(uintptr_t)this : (uint8)0);
}
