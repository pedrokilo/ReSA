#include "StdInc.h"

#include "Data.h"
#include "GxtChar.h"

CData::CData() {
    m_data = nullptr;
    m_size = 0;
}

CData::~CData() {
    Unload();
}

// 0x69F640
void CData::Unload() {
    // Como el original: solo se libera (y se anula) el buffer si existe.
    if (m_data) {
        delete[] m_data;
        m_data = nullptr;
    }
    m_size = 0;
}

// unknown always 0
// 0x69F5D0
bool CData::Load(uint32 length, FILESTREAM file, uint32* offset, uint8 unknown) {
    m_size = length;
    m_data = new char[length];

    // Como el original: se lee byte a byte sobre la ranura del parámetro length (solo cambia su byte bajo);
    // con unknown != 0 no se lee y el "resultado" de la lectura es el propio unknown. Si una lectura
    // falla se devuelve false dejando el buffer y el desplazamiento a medias.
    uint32 temp = length;
    for (uint32 i = 0; i < length; ++i) {
        const size_t read = unknown ? size_t(unknown) : CFileMgr::Read(file, &temp, 1);
        if (read != 1) {
            return false;
        }
        m_data[i] = (char)temp;
        ++*offset;
    }
    return true;
}
