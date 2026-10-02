/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEBankLoader
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "AEBankLoader.h"

/**
 * Registra la carga de tablas BANKLKUP/BANKSLOT/PAKFILES usadas por SFX.
 */
void CAEBankLoader::InjectHooks() {
    RH_ScopedClass(CAEBankLoader);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Deconstructor, 0x4DFB20);
    RH_ScopedInstall(GetBankLookup, 0x4E01B0);
    RH_ScopedInstall(LoadBankLookupFile, 0x4DFBD0);
    RH_ScopedInstall(LoadBankSlotFile, 0x4E0590);
    RH_ScopedInstall(LoadSFXPakLookupFile, 0x4DFC70, {.reversed = true});
    RH_ScopedInstall(CalculateBankSlotsInfosOffsets, 0x4DFBA0);
}

// 0x4DFB20
// Libera buffers/tablas sólo después de una inicialización válida.
CAEBankLoader::~CAEBankLoader() {
    if (m_IsInitialised) {
        // Como el original: borra primero los slots y después libera el buffer.
        delete m_BankSlots;
        CMemoryMgr::Free(m_Buffer);
        delete m_BankLkups;
        delete m_StreamHandles;
    }
}

// 0x4DFBD0
// Carga BANKLKUP.DAT como array contiguo de AEBankLookup.
bool CAEBankLoader::LoadBankLookupFile() {
    const auto file = CFileMgr::OpenFile("AUDIO\\CONFIG\\BANKLKUP.DAT", "rb");
    if (!file) {
        return false;
    }

    if (const auto size = CFileMgr::GetTotalSize(file)) {
        m_BankLkupCnt = (uint16)(size / sizeof(AEBankLookup));
        // Como el original: operator new escalar (0x82119A) con el contador extendido con signo.
        m_BankLkups   = static_cast<AEBankLookup*>(::operator new((int16)(m_BankLkupCnt) * sizeof(AEBankLookup)));

        if (CFileMgr::Read(file, m_BankLkups, size) != size) {
            delete std::exchange(m_BankLkups, nullptr);
        }

        CFileMgr::CloseFile(file);
        return m_BankLkups != nullptr;
    } else {
        CFileMgr::CloseFile(file);
    }

    return false;
}

// 0x4E0590
/**
 * Carga BANKSLOT.DAT, calcula offsets acumulados y reserva el buffer único donde
 * se materializarán los bancos solicitados.
 */
bool CAEBankLoader::LoadBankSlotFile() {
    const auto file = CFileMgr::OpenFile("AUDIO\\CONFIG\\BANKSLOT.DAT", "rb");
    if (!file) {
        return false;
    }

    // Como el original: tamaño sin signo y m_Buffer intacto en los caminos de error.
    const auto size = (uint32)(CFileMgr::GetTotalSize(file));
    if (size <= 2) {
        CFileMgr::CloseFile(file);
        return false;
    }
    CFileMgr::Read(file, &m_BankSlotCnt, 2u);
    // Como el original: memoria sin inicializar de operator new escalar (0x82119A).
    m_BankSlots = static_cast<CAEBankSlot*>(::operator new((int16)(m_BankSlotCnt) * sizeof(CAEBankSlot)));
    if (!m_BankSlots) {
        CFileMgr::CloseFile(file);
        return false;
    }
    if (CFileMgr::Read(file, m_BankSlots, size - 2) != size - 2) {
        CFileMgr::CloseFile(file); // Como el original: cierra antes de borrar los slots
        delete std::exchange(m_BankSlots, nullptr);
        return false;
    }
    CFileMgr::CloseFile(file); // Como el original: cierra antes de calcular offsets y reservar

    CalculateBankSlotsInfosOffsets();
    const auto& lastSlot = m_BankSlots[(int16)(m_BankSlotCnt) - 1];
    m_BufferSize         = lastSlot.OffsetBytes + lastSlot.NumBytes;
    m_Buffer             = (uint8*)CMemoryMgr::Malloc(m_BufferSize);
    return true; // Como el original: no comprueba el resultado de Malloc
}

// 0x4DFC70
// Lee PAKFILES.DAT y abre cada paquete SFX mediante CdStream, conservando sus handles.
bool CAEBankLoader::LoadSFXPakLookupFile() {
    const auto file = CFileMgr::OpenFile("AUDIO\\CONFIG\\PAKFILES.DAT", "rb");
    if (!file) {
        return false; // Como el original: sin fichero devuelve false
    }

    const auto totalSize = (size_t)(CFileMgr::GetTotalSize(file));
    if (!totalSize) {
        CFileMgr::CloseFile(file);
        return false;
    }
    m_PakLkupCount = (uint16)(totalSize / sizeof(AEPakLookup));
    m_PakLkups     = static_cast<AEPakLookup*>(::operator new((int16)(m_PakLkupCount) * sizeof(AEPakLookup)));

    const auto readSize = CFileMgr::Read(file, m_PakLkups, totalSize);
    CFileMgr::CloseFile(file); // Como el original: cierra antes de comprobar la lectura
    if (readSize != totalSize) {
        delete std::exchange(m_PakLkups, nullptr);
        return false;
    }

    m_StreamHandles = static_cast<int32*>(::operator new((int16)(m_PakLkupCount) * sizeof(int32)));
    for (auto i = 0; i < (int16)(m_PakLkupCount); i++) {
        // Como el original: strcpy("AUDIO\\SFX\\") + strcat(BaseFilename) en un buffer local.
        char path[128];
        strcpy_s(path, "AUDIO\\SFX\\");
        strcat_s(path, m_PakLkups[i].BaseFilename);
        m_StreamHandles[i] = CdStreamOpen(path);
    }

    delete std::exchange(m_PakLkups, nullptr);
    return true;
}

// 0x4DFBA0
// Convierte tamaños por slot en offsets acumulados dentro de m_Buffer.
void CAEBankLoader::CalculateBankSlotsInfosOffsets() {
    auto offset = 0u;
    for (auto i = 0; i < (int16)(m_BankSlotCnt); i++) { // Como el original: contador con signo (movsx)
        m_BankSlots[i].OffsetBytes = offset;
        offset += m_BankSlots[i].NumBytes;
    }
}

// notsa
CAEBankSlot& CAEBankLoader::GetBankSlot(eSoundBankSlot slot) const {
    return GetBankSlots()[slot];
}

// notsa
AEPakLookup& CAEBankLoader::GetPakLookup(AEPakID pak) const {
    return GetPakLookups()[pak];
}

// 0x4E01B0
AEBankLookup& CAEBankLoader::GetBankLookup(uint16 bankId) const {
    // Como el original: sin inicializar o con índice >= contador (con signo) devuelve nullptr.
    if (!m_IsInitialised || (int32)(bankId) >= (int32)((int16)(m_BankLkupCnt))) {
        AEBankLookup* const none = nullptr;
        return *none;
    }
    return m_BankLkups[bankId];
}
