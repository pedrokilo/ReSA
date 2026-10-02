#include "StdInc.h"

#include "KeyArray.h"
#include "GxtChar.h"

CKeyArray::CKeyArray() {
    m_data = nullptr;
    m_size = 0;
}

CKeyArray::~CKeyArray() {
    Unload();
}

// 0x69F510
void CKeyArray::Unload() {
    delete[] m_data;
    m_data = nullptr;
    m_size = 0;
}

// unknown always 0
// 0x69F490
bool CKeyArray::Load(uint32 length, FILESTREAM file, uint32* offset, uint8 unknown) {
    m_size = length / sizeof(CKeyEntry);
    m_data = new CKeyEntry[m_size];

    // 0x69F4C0: read byte by byte, advancing `offset` per byte and failing on a short read.
    // A non-zero `unknown` (always 0 in practice) skips the read and stands in for its result.
    auto* const bytes = reinterpret_cast<uint8*>(m_data);
    uint32 readResult = unknown;
    uint8  byte       = static_cast<uint8>(length);
    for (uint32 i = 0; i < length; i++) {
        if (!unknown) {
            readResult = CFileMgr::Read(file, &byte, 1);
        }
        if (readResult != 1) {
            return false;
        }
        bytes[i] = byte;
        ++*offset;
    }
    return true;
}

// 0x69F540
void CKeyArray::Update(char* offset) {
    for (uint32 i = 0; i < m_size; ++i) {
        m_data[i].string = (GxtChar*)((uint8*)offset + (uint32)(m_data[i].string));
    }
}

// 0x6A0000
const GxtChar* CKeyArray::Search(const char* key, bool& found) {
    const auto entry = BinarySearch(CKeyGen::GetUppercaseKey(key), m_data, 0, m_size - 1);
    found = entry != nullptr;

    return entry ? entry->string : nullptr;
}

// 0x69F570
CKeyEntry* CKeyArray::BinarySearch(uint32 hash, CKeyEntry* entries, int16 firstIndex, int16 lastIndex) {
    if (firstIndex > lastIndex)
        return nullptr;

    while (true) {
        uint16 middle = (firstIndex + lastIndex) >> 1;
        auto entryHash = entries[middle].hash;

        if (hash == entryHash)
            return &entries[middle];

        if (hash > entryHash)
            firstIndex = middle + 1;
        else
            lastIndex = middle - 1;

        if (firstIndex > lastIndex)
            return nullptr;
    }
}
