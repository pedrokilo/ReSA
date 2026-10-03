/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CData
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CKeyArray
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CMissionTextOffsets
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CText
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "GxtChar.h"

#include "Text.h"
#include "Data.h"
#include "KeyArray.h"
#include "MissionTextOffsets.h"

#include "eLanguage.h"

constexpr auto CHUNK_TKEY = "TKEY";
constexpr auto CHUNK_TDAT = "TDAT";
constexpr auto CHUNK_TABL = "TABL";
constexpr auto GAME_ENCODING = (sizeof(GxtChar) * 8u); // Bits per char (8: ASCII, 16: UTF-16)

static constexpr uint8 UpperCaseTable[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x41, 0x41, 0x41, 0x41, 0x84, 0x85, 0x45, 0x45, 0x45,
    0x45, 0x49, 0x49, 0x49, 0x49, 0x4F, 0x4F, 0x4F, 0x4F, 0x55, 0x55, 0x55, 0x55, 0xAD, 0xAD, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

static constexpr uint8 FrenchUpperCaseTable[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88,
    0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0xAD, 0xAD, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

// 0x69F7E0 (GxtCharToAscii)
// Como el original: convierte al buffer estático del EXE (0xC1BDD0) con la tabla de acentos de un byte;
// los códigos >= 0xD0 (salvo 0xCF) pasan a '#'. Como máximo 255 caracteres.
static const char* GxtCharToAsciiOriginal(const GxtChar* src, uint8 start) {
    auto& buffer = StaticRef<char[256]>(0xC1BDD0);
    auto* str = reinterpret_cast<const uint8*>(src);
    if (start) {
        str += start;
    }
    int32 i = 0;
    if (str) {
        for (; i < 255 && *str; ++i, ++str) {
            auto c = *str;
            if (c < 0x80)       {}
            else if (c <= 0x83) { c += 0x40; }
            else if (c <= 0x8D) { c += 0x42; }
            else if (c <= 0x91) { c += 0x44; }
            else if (c <= 0x95) { c += 0x47; }
            else if (c <= 0x9A) { c += 0x49; }
            else if (c <= 0xA4) { c += 0x4B; }
            else if (c <= 0xA8) { c += 0x4D; }
            else if (c <= 0xCC) { c += 0x50; }
            else if (c == 0xCD) { c = 0xD1; }
            else if (c == 0xCE) { c = 0xF1; }
            else                { c = c == 0xCF ? 0xBF : 0x23; }
            buffer[i] = static_cast<char>(c);
        }
    }
    buffer[i] = '\0';
    return buffer;
}

void CText::InjectHooks() {
    RH_ScopedClass(CText);
    RH_ScopedCategory("Text");

    RH_ScopedInstall(Constructor, 0x6A00F0);
    RH_ScopedInstall(Destructor, 0x6A0140);
    RH_ScopedInstall(Get, 0x6A0050);
    RH_ScopedInstall(GetNameOfLoadedMissionText, 0x69FBD0);
    RH_ScopedInstall(ReadChunkHeader, 0x69F940);
    RH_ScopedInstall(LoadMissionPackText, 0x69F9A0);
    RH_ScopedInstall(LoadMissionText, 0x69FBF0);
    RH_ScopedInstall(Load, 0x6A01A0);
    RH_ScopedInstall(Unload, 0x69FF20);

    {
        RH_ScopedClass(CMissionTextOffsets);
        RH_ScopedInstall(Load, 0x69F670);
    }

    {
        RH_ScopedClass(CData);
        RH_ScopedInstall(Unload, 0x69F640);
        RH_ScopedInstall(Load, 0x69F5D0);
    }

    {
        RH_ScopedClass(CKeyArray);
        RH_ScopedCategory("Text");
        RH_ScopedInstall(Unload, 0x69F510);
        RH_ScopedInstall(BinarySearch, 0x69F570);
        RH_ScopedInstall(Search, 0x6A0000);
        RH_ScopedInstall(Load, 0x69F490);
    }
}

// 0x6A00F0
// Como el original: los constructores de los miembros ponen a cero los cuatro arrays y el contador de
// misiones; no se llama a Unload, no se toca m_bCdErrorLoaded ni m_szCdErrorText, se limpian los 8 bytes
// del nombre de misión y los 25 primeros bytes de GxtErrorString.
CText::CText() {
    m_bIsMissionPackLoaded = false;
    m_bIsMissionTextOffsetsLoaded = false;
    m_nLangCode = eTextLangCode::ENGLISH;

    std::memset(m_szMissionName, 0, sizeof(m_szMissionName));
    std::memset(GxtErrorString, 0, 25);
}

// 0x6A0140
CText::~CText() {
    m_MissionText.Unload();
    m_MissionKeyArray.Unload();
    m_MainText.Unload();
    m_MainKeyArray.Unload();
}

// Unloads GXT file
// 0x69FF20
void CText::Unload(bool unloadMissionData) {
    CMessages::ClearAllMessagesDisplayedByGame(false);

    m_MainKeyArray.Unload();
    m_MainText.Unload();
    m_bCdErrorLoaded = false;
    if (!unloadMissionData) {
        m_MissionKeyArray.Unload();
        m_MissionText.Unload();

        m_bIsMissionPackLoaded = false;
        std::memset(m_szMissionName, 0, sizeof(m_szMissionName)); // Como el original: los 8 bytes
    }
}

// NOTSA
const char* GetGxtName() {
    switch (FrontEndMenuManager.m_nPrefsLanguage) {
    case eLanguage::AMERICAN:
        return "AMERICAN.GXT";
    case eLanguage::FRENCH:
        return "FRENCH.GXT";
    case eLanguage::GERMAN:
        return "GERMAN.GXT";
    case eLanguage::ITALIAN:
        return "ITALIAN.GXT";
    case eLanguage::SPANISH:
        return "SPANISH.GXT";
#ifdef MORE_LANGUAGES
    case eLanguage::RUSSIAN:
        return "RUSSIAN.GXT";
    case eLanguage::JAPANESE:
        return "JAPANESE.GXT";
#endif
    default:
        NOTSA_UNREACHABLE("GXT file for language={:02d} is not defined.", (uint8)FrontEndMenuManager.m_nPrefsLanguage);
    }
}

// NOTSA
void CheckFileEncoding(const char* file, uint16 version, uint16 encoding) {
    const auto GetEncodingName = [](uint16 e) -> std::string {
        switch (e) {
        case 8:
            return "8-bit ASCII";
        case 16:
            return "16-bit UTF-16";
        default:
            return std::format("{}-bit unknown", e);
        }
    };
    NOTSA_LOG_DEBUG("Loading '{}' version={:02d} ({})", file, version, GetEncodingName(encoding));
    if (encoding != GAME_ENCODING) {
        NOTSA_UNREACHABLE("File {} was compiled with {} encoding but {} is required.", file, GetEncodingName(encoding), GetEncodingName(GAME_ENCODING));
    }
}

// Loads GXT file
// 0x6A01A0
void CText::Load(bool keepMissionPack) {
    m_bIsMissionTextOffsetsLoaded = false;
    Unload(keepMissionPack);

    CFileMgr::SetDir("TEXT");
    auto fileName = GetGxtName();
    auto file = CFileMgr::OpenFile(fileName, "rb");
    assert(file);

    uint16 version = 0, encoding = 0;
    CFileMgr::Read(file, &version, sizeof(version));
    CFileMgr::Read(file, &encoding, sizeof(encoding));
    CheckFileEncoding(fileName, version, encoding);

    uint32 offset = sizeof(uint16) * 2; // skip version and encoding
    bool bTKEY = false;
    bool bTDAT = false;
    ChunkHeader header{};
    while (!bTKEY || !bTDAT) {
        ReadChunkHeader(&header, file, &offset, false);
        if (header.size == 0)
            continue;

        if (!strncmp(header.magic, CHUNK_TABL, sizeof(header.magic))) {
            m_MissionTextOffsets.Load(header.size, file, &offset, 0x58000); // todo: magic. Android have different value 0x64000
            m_bIsMissionTextOffsetsLoaded = true;
            continue;
        }

        if (!strncmp(header.magic, CHUNK_TKEY, sizeof(header.magic))) {
            m_MainKeyArray.Load(header.size, file, &offset, false);
            bTKEY = true;
            continue;
        }

        if (!strncmp(header.magic, CHUNK_TDAT, sizeof(header.magic))) {
            m_MainText.Load(header.size, file, &offset, false);
            bTDAT = true;
            continue;
        }

        // Como el original: salta el bloque leyendo byte a byte (contador de 16 bits, sin comprobar errores).
        for (uint16 i = 0; i < static_cast<uint32>(header.size); ++i) {
            uint8 tmp;
            CFileMgr::Read(file, &tmp, sizeof(tmp));
            ++offset;
        }
    }

    m_MainKeyArray.Update(m_MainText.m_data);
    CFileMgr::CloseFile(file);

    // Como el original: consulta TheText (no `this`) y convierte con GxtCharToAscii (0x69F7E0).
    strcpy(m_szCdErrorText, GxtCharToAsciiOriginal(TheText.Get("CDERROR"), 0));
    m_bCdErrorLoaded = true;

    CFileMgr::SetDir("");

    if (keepMissionPack)
        return;

    while (!m_bIsMissionPackLoaded && CGame::bMissionPackGame) {
        if (FrontEndMenuManager.CheckMissionPackValidMenu()) {
            CTimer::Suspend();
            LoadMissionPackText();
            CFileMgr::SetDir("");
            CTimer::Resume();
        }
    }
}

// 0x69FBF0
void CText::LoadMissionText(const char* mission) {
    if (CGame::bMissionPackGame != 0)
        return;

    CMessages::ClearAllMessagesDisplayedByGame(true);

    m_MissionKeyArray.Unload();
    m_MissionText.Unload();

    m_bIsMissionPackLoaded = false;
    rng::fill(m_szMissionName, '\0');

    // Como el original: el contador de tablas se lee como uint16 y se compara la longitud (16 bits)
    // antes del strncmp; si la misión no está, simplemente vuelve.
    const auto numOffsets = static_cast<uint16>(m_MissionTextOffsets.GetSize());
    const auto missionLen = static_cast<uint16>(strlen(mission));
    uint16 offsetId = 0;
    bool missionIdxFound = false;
    for (; offsetId < numOffsets; offsetId++) {
        const auto& entry = reinterpret_cast<const tMissionTextOffset*>(&m_MissionTextOffsets)[offsetId];
        if (static_cast<uint16>(strlen(entry.szMissionName)) == missionLen && !strncmp(entry.szMissionName, mission, missionLen)) {
            missionIdxFound = true;
            break;
        }
    }
    if (!missionIdxFound) {
        return;
    }

    CFileMgr::SetDir("TEXT");
    char fileName[32];
    strcpy(fileName, GetGxtName());
    CTimer::Suspend();

    const auto textOffset = reinterpret_cast<const tMissionTextOffset*>(&m_MissionTextOffsets)[static_cast<int16>(offsetId)].offset;
    auto file = CFileMgr::OpenFile(fileName, "rb");
    CFileMgr::Seek(file, textOffset, 0);

    // Como el original: el nombre de la tabla se lee byte a byte (sin comprobar) y el contador empieza en 8.
    char tablName[8];
    for (auto& c : tablName) {
        CFileMgr::Read(file, &c, 1);
    }
    // (el EXE compara aquí tablName con la misión con strncmp y descarta el resultado)

    uint32 offset = sizeof(tablName);
    auto missionKeyArrayLoaded = false;
    auto missionTextLoaded = false;
    ChunkHeader header{};

    while (!missionKeyArrayLoaded || !missionTextLoaded) {
        ReadChunkHeader(&header, file, &offset, false);
        if (header.size == 0)
            continue;

        if (!strncmp(header.magic, CHUNK_TKEY, sizeof(header.magic))) {
            m_MissionKeyArray.Load(header.size, file, &offset, 0);
            missionKeyArrayLoaded = true;
            continue;
        }

        if (!strncmp(header.magic, CHUNK_TDAT, sizeof(header.magic))) {
            m_MissionText.Load(header.size, file, &offset, 0);
            missionTextLoaded = true;
            continue;
        }

        // Como el original: salta el bloque leyendo byte a byte (contador de 16 bits, sin comprobar errores).
        for (uint16 i = 0; i < static_cast<uint32>(header.size); ++i) {
            uint8 tmp;
            CFileMgr::Read(file, &tmp, sizeof(tmp));
            ++offset;
        }
    }

    m_MissionKeyArray.Update(m_MissionText.m_data);

    CFileMgr::CloseFile(file);
    CTimer::Resume();
    CFileMgr::SetDir("");

    strcpy(m_szMissionName, mission); // Como el original: strcpy sin límite
    m_bIsMissionPackLoaded = true;
}

// Loads mission table from GXT file
// 0x69F9A0
void CText::LoadMissionPackText() {
    CMessages::ClearAllMessagesDisplayedByGame(true);
    CFileMgr::SetDir("");

    m_MissionKeyArray.Unload();
    m_MissionText.Unload();

    m_bIsMissionPackLoaded = false;
    rng::fill(m_szMissionName, '\0');

    CFileMgr::SetDirMyDocuments();
    char fileName[64];
    sprintf(fileName, "MPACK//MPACK%d//TEXT.GXT", CGame::bMissionPackGame);

    // Como el original: el handle se compara con signo y, en los fallos posteriores, no se cierra el fichero.
    auto file = CFileMgr::OpenFile(fileName, "rb");
    if (reinterpret_cast<int32>(file) <= 0) {
        m_bIsMissionPackLoaded = false;
        return;
    }

    uint16 version = 0, encoding = 0;
    if (CFileMgr::Read(file, &version, sizeof(version)) != sizeof(version)) {
        m_bIsMissionPackLoaded = false;
        return;
    }
    if (CFileMgr::Read(file, &encoding, sizeof(encoding)) != sizeof(encoding)) {
        m_bIsMissionPackLoaded = false;
        return;
    }
    CheckFileEncoding(fileName, version, encoding); // NOTSA

    uint32 offset = sizeof(uint16) * 2;
    ChunkHeader header{};

    bool bKey = false, bText = false;
    while (!bKey || !bText) { // Como el original: hasta tener las dos tablas
        if (!ReadChunkHeader(&header, file, &offset, false)) {
            m_bIsMissionPackLoaded = false;
            return;
        }
        if (header.size == 0)
            continue;

        if (!strncmp(header.magic, CHUNK_TKEY, sizeof(header.magic))) {
            m_MissionKeyArray.Load(header.size, file, &offset, false);
            bKey = true;
            continue;
        }

        if (!strncmp(header.magic, CHUNK_TDAT, sizeof(header.magic))) {
            m_MissionText.Load(header.size, file, &offset, false);
            bText = true;
            continue;
        }

        // Skip any other data (como el original: byte a byte, contador de 16 bits, avanzando offset)
        for (uint16 i = 0; i < static_cast<uint32>(header.size); ++i) {
            uint8 tmp;
            if (CFileMgr::Read(file, &tmp, sizeof(tmp)) != 1) {
                m_bIsMissionPackLoaded = false;
                return;
            }
            ++offset;
        }
    }
    m_MissionKeyArray.Update(m_MissionText.m_data); // Como el original: las claves de misión
    std::memcpy(m_szMissionName, "MPNAME", sizeof("MPNAME"));
    m_bIsMissionPackLoaded = true;
    // Como el original: no se cierra el fichero.
}

// 0x6A0050
const GxtChar* CText::Get(const char* key) {
    if (key[0] && key[0] != ' ') {
        bool found = false;
        auto str = m_MainKeyArray.Search(key, found);
        if (found) {
            return str;
        }

        // check mission keys block if no entry found yet
        if ((CGame::bMissionPackGame || m_bIsMissionTextOffsetsLoaded) && m_bIsMissionPackLoaded) {
            str = m_MissionKeyArray.Search(key, found);
            if (found) {
                return str;
            }
        }
    }

    char buf[32];
    sprintf_s(buf, "");
    AsciiToGxtChar(buf, GxtErrorString);
    return GxtErrorString;
}

// Writes loaded mission text into outStr
// 0x69FBD0
void CText::GetNameOfLoadedMissionText(char* outStr) {
    strcpy(outStr, m_szMissionName); // Como el original: strcpy sin límite
}

// 0x69F940
bool CText::ReadChunkHeader(ChunkHeader* header, FILESTREAM file, uint32* offset, uint8 unknown) {
    // Como el original: lee byte a byte sobre la ranura del parámetro offset (byte temporal inicial = byte bajo
    // del puntero), avanza *offset por byte y falla en una lectura corta. Con unknown != 0 no se lee y el
    // "resultado" es el propio unknown.
    uint32 readResult = unknown;
    auto byte = static_cast<uint8>(reinterpret_cast<uintptr_t>(offset));
    for (uint16 i = 0; i < sizeof(ChunkHeader); ++i) {
        if (!unknown) {
            readResult = CFileMgr::Read(file, &byte, 1);
        }
        if (readResult != 1) {
            return false;
        }
        reinterpret_cast<uint8*>(header)[i] = byte;
        ++*offset;
    }
    return true;
}

// 0x69F750
char CText::GetUpperCase(const char c) const {
    switch (m_nLangCode) {
    case eTextLangCode::ENGLISH:
        if (c >= 'a' && c <= 'z')
            return std::toupper(c);
        break;

    case eTextLangCode::FRENCH:
        if (c >= 'a' && c <= 'z')
            return std::toupper(c);

        if (c >= 128 && c <= 255)
            return FrenchUpperCaseTable[c - 128];
        break;

    case eTextLangCode::GERMAN:
    case eTextLangCode::ITALIAN:
    case eTextLangCode::SPANISH:
        if (c >= 'a' && c <= 'z')
            return std::toupper(c);

        if (c >= 128 && c <= 255)
            return UpperCaseTable[c - 128];
        break;

    default:
        break;
    }
    return c;
}

CText* CText::Constructor() {
    this->CText::CText();
    return this;
}

CText* CText::Destructor() {
    this->CText::~CText();
    return this;
}
