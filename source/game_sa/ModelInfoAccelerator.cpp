#include "StdInc.h"

#include "ModelInfoAccelerator.h"
#include "FileMgr.h"

void CModelInfoAccelerator::InjectHooks() {
    RH_ScopedClass(CModelInfoAccelerator);
    RH_ScopedCategory("Models");

    RH_ScopedInstall(FreeModelInfoIds, 0x4C6B10);    // lote 194A (sin validar)
    RH_ScopedInstall(GetModelInfoIdFile, 0x4C6B70);  // lote 194A (sin validar)
    RH_ScopedInstall(EndOfLoadPhase, 0x4C6BD0);      // lote 194A (sin validar)
}

// 0x4C6A80
CModelInfoAccelerator::CModelInfoAccelerator() {
    Init();
}

// 0x4C6A80
void CModelInfoAccelerator::Init() {
    m_nNumIDs = 0;
    m_bFileRead = false;
    m_pIDs = nullptr;
    field_1A = false;
    m_szFilePath[0] = '\0';
}

// 0x4C6AA0
void CModelInfoAccelerator::AddModelInfoId(uint16 modelId) {
    m_pIDs[m_nNumIDs++] = modelId;
}

// 0x4C6AC0
uint16 CModelInfoAccelerator::GetNextModelInfoId() {
    return m_pIDs[m_nNumIDs++];
}

// 0x4C6AE0 (corregida en el lote 194A, sin validar; sin hook): operator new del EXE (0x82119A) de BUFFER_SIZE BYTES (0xA08C; antes `new uint16[BUFFER_SIZE]` reservaba el doble y por operator new[]) y se pone a cero
// el bloque entero (rep stosd de 0x2823 dwords). Los identificadores son uint16, pero el EXE reserva y guarda solo 0xA08C bytes.
void CModelInfoAccelerator::AllocModelInfoIds() {
    m_pIDs = static_cast<uint16*>(::operator new(BUFFER_SIZE));
    memset(m_pIDs, 0, BUFFER_SIZE);
}

// 0x4C6B10 (lote 194A, sin validar; thiscall, sin argumentos, `ret`, 22 bytes; el listado decia 11): operator delete del EXE (0x8214BD, no el de vectores) sobre el bloque y puntero a nulo.
void CModelInfoAccelerator::FreeModelInfoIds() {
    ::operator delete(m_pIDs);
    m_pIDs = nullptr;
}

// 0x4C6B30
void CModelInfoAccelerator::GetEntry(CBaseModelInfo** mi, int32* arg1, char* arg2) {
    // NOP
}

// 0x4C6C20
bool CModelInfoAccelerator::Begin(const char* filePath) {
    strcpy_s(m_szFilePath, filePath);
    if (field_1A)
        return field_1A;

    return GetModelInfoIdFile();
}

// 0x4C6B40
void CModelInfoAccelerator::End(const char* filePath) {
    // NOP
}

// 0x4C6B70 (lote 194A, sin validar; thiscall, sin argumentos, `ret`, 92 bytes; AllocModelInfoIds (0x4C6AE0) va expandida en linea): abre el archivo de ids en modo "rb" (0x859C6C; CFileMgr::OpenFile 0x538900) sobre
// la ruta (+0x6) y guarda en +0x1B el resultado de `handle > 0` COMPARADO CON SIGNO (setg). Reserva y pone a cero el bloque (operator new 0x82119A de 0xA08C bytes en +0x0). Si el archivo se abrio (el byte de +0x1B),
// lo lee entero (CFileMgr::Read 0x538950, 0xA08C bytes) y lo cierra (0x5389D0). Devuelve el byte de +0x1B.
bool CModelInfoAccelerator::GetModelInfoIdFile() {
    const FILESTREAM file = CFileMgr::OpenFile(m_szFilePath, "rb");
    m_bFileRead = int32(intptr_t(file)) > 0;

    AllocModelInfoIds();
    if (m_bFileRead) {
        CFileMgr::Read(file, m_pIDs, BUFFER_SIZE);
        CFileMgr::CloseFile(file);
    }

    return m_bFileRead;
}

// 0x4C6BD0 (lote 194A, sin validar; thiscall, sin argumentos, `ret`, 72 bytes; el listado decia 61): si el archivo NO se leyo (+0x1B = 0) abre la ruta para escribir (CFileMgr::OpenFileForWriting 0x538910) y, si el
// handle es > 0 (comparacion con signo, jle), escribe el bloque entero (CFileMgr::Write 0x538970, 0xA08C bytes) y cierra (0x5389D0); con handle <= 0 NO hace nada (no hay CloseFile ni mensaje: la rama FIX_BUGS
// de antes se quita). En todos los casos libera el bloque con operator delete (0x8214BD) y deja +0x0 a nulo.
void CModelInfoAccelerator::EndOfLoadPhase() {
    if (!m_bFileRead) {
        const FILESTREAM file = CFileMgr::OpenFileForWriting(m_szFilePath);
        if (int32(intptr_t(file)) > 0) {
            CFileMgr::Write(file, m_pIDs, BUFFER_SIZE);
            CFileMgr::CloseFile(file);
        }
    }
    ::operator delete(m_pIDs);
    m_pIDs = nullptr;
}
