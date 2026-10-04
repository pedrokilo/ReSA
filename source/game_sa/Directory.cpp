/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDirectory
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Directory.h"

void CDirectory::InjectHooks() {
    RH_ScopedClass(CDirectory);
    RH_ScopedCategoryGlobal();

    RH_ScopedOverloadedInstall(Constructor, "Empty", 0x532290, CDirectory*(CDirectory::*)());
    RH_ScopedOverloadedInstall(Constructor, "Capacity", 0x5322A0, CDirectory*(CDirectory::*)(size_t));
    RH_ScopedInstall(Destructor, 0x5322D0);
    RH_ScopedInstall(Init, 0x5322F0);
    RH_ScopedOverloadedInstall(AddItem, "", 0x532310, void(CDirectory::*)(const DirectoryInfo&));
    RH_ScopedInstall(ReadDirFile, 0x532350);
    RH_ScopedInstall(WriteDirFile, 0x532410);
    RH_ScopedOverloadedInstall(FindItem, "", 0x532450, DirectoryInfo*(CDirectory::*)(const char*) const);
    RH_ScopedOverloadedInstall(FindItem, "ByName", 0x5324A0, bool(CDirectory::*)(const char*, CdStreamPos&, uint32&) const);
    RH_ScopedOverloadedInstall(FindItem, "ByHash", 0x5324D0, bool(CDirectory::*)(uint32, CdStreamPos&, uint32&) const);
}

CDirectory* CDirectory::Constructor() { this->CDirectory::CDirectory(); return this; }
CDirectory* CDirectory::Constructor(size_t capacity) { this->CDirectory::CDirectory(capacity); return this; }
CDirectory* CDirectory::Destructor() { this->CDirectory::~CDirectory(); return this; }

// 0x5322A0
// Como el original: operator new escalar de capacidad · 32 bytes (multiplicación de 32 bits), sin construir entradas.
CDirectory::CDirectory(size_t capacity) {
    m_nCapacity    = capacity;
    m_nNumEntries  = 0;
    m_pEntries     = static_cast<DirectoryInfo*>(operator new(capacity * sizeof(DirectoryInfo)));
    m_bOwnsEntries = true;
}

// 0x5322D0
// Como el original: operator delete escalar.
CDirectory::~CDirectory() {
    if (m_pEntries && m_bOwnsEntries) {
        operator delete(m_pEntries);
    }
}

// 0x5322F0
void CDirectory::Init(int32 capacity, DirectoryInfo* entries) {
    m_nCapacity = capacity;
    m_pEntries = entries;
    m_nNumEntries = 0;
    m_bOwnsEntries = false;
}

// 0x532310
void CDirectory::AddItem(const DirectoryInfo& dirInfo) {
    if ((int32)m_nNumEntries < (int32)m_nCapacity) { // Como el original: comparación con signo, y se añade aunque el nombre ya exista
        std::memcpy(&m_pEntries[m_nNumEntries], &dirInfo, sizeof(DirectoryInfo)); // Como el original: rep movs de 32 bytes
        m_nNumEntries++;
    } else {
        NOTSA_LOG_DEBUG("Too many objects without modelinfo structures");
    }
}

// NOTSA
void CDirectory::AddItem(const DirectoryInfo& dirInfo, CdStreamID streamID) {
    DirectoryInfo di = dirInfo;
    di.Pos.FileID = streamID;
    AddItem(di);
}

// 0x532350
void CDirectory::ReadDirFile(const char* filename) {
    auto file = CFileMgr::OpenFile(filename, "rb");
    {
        /* Unused stuff */
        byte unused[4];
        CFileMgr::Read(file, &unused, sizeof(unused));
    }
    uint32 numEntries; // Como el original: ni el contador ni la entrada se inicializan antes de leerlos
    CFileMgr::Read(file, &numEntries, sizeof(numEntries));

    for (size_t i = 0; i < numEntries; i++) {
        DirectoryInfo info;
        CFileMgr::Read(file, &info, sizeof(info));
        AddItem(info);
        /* Possible optimization: Read directly into m_pEntries */
    }
    CFileMgr::CloseFile(file);
}

// 0x532410
bool CDirectory::WriteDirFile(const char* fileName) {
    auto file = CFileMgr::OpenFileForWriting(fileName);
    const auto nNumBytesToWrite = sizeof(*m_pEntries) * m_nNumEntries;
    const auto nBytesWritten = CFileMgr::Write(file, m_pEntries, nNumBytesToWrite);
    CFileMgr::CloseFile(file);
    return nNumBytesToWrite == nBytesWritten;
}

// 0x532450
// Como el original: el número de entradas se compara con signo.
CDirectory::DirectoryInfo* CDirectory::FindItem(const char* itemName) const {
    for (int32 i = 0; i < (int32)m_nNumEntries; i++) {
        if (_stricmp(m_pEntries[i].Name, itemName) == 0) {
            return &m_pEntries[i];
        }
    }
    return nullptr;
}

// 0x5324A0
bool CDirectory::FindItem(const char* name, CdStreamPos& pos, uint32& outSize) const {
    if (DirectoryInfo* info = FindItem(name)) {
        outSize = info->Size;
        pos     = info->Pos;
        return true;
    }
    return false;
}

// 0x5324D0
// Como el original: el número de entradas se compara con signo.
bool CDirectory::FindItem(uint32 hashKey, CdStreamPos& pos, uint32& outSize) const {
    for (int32 i = 0; i < (int32)m_nNumEntries; i++) {
        const DirectoryInfo& info = m_pEntries[i];
        if (CKeyGen::GetUppercaseKey(info.Name) == hashKey) {
            outSize = info.Size;
            pos     = info.Pos;
            return true;
        }
    }
    return false;
}
