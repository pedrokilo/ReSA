/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: C_PcSave
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <cstdio>

#include "GenericGameStorage.h"
#include "Messages.h"
#include "C_PcSave.h"
#include "SimpleVariablesSaveStructure.h"
#include "GxtChar.h"

void C_PcSave::InjectHooks() {
    RH_ScopedClass(C_PcSave);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(SetSaveDirectory, 0x619040);
    RH_ScopedInstall(GenerateGameFilename, 0x6190A0);
    RH_ScopedInstall(PopulateSlotInfo, 0x619140);
    RH_ScopedInstall(SaveSlot, 0x619060);
    RH_ScopedInstall(DeleteSlot, 0x6190D0);
}

// 0x619040
void C_PcSave::SetSaveDirectory(const char* path) {
    sprintf_s(DefaultPCSaveFileName, "%s\\%s", path, "GTASAsf");
}

// 0x6190A0
void C_PcSave::GenerateGameFilename(int32 slot, char* out) {
    // The original has no output-size parameter; the caller owns the buffer capacity.
    std::sprintf(out, "%s%i%s", DefaultPCSaveFileName, slot + 1, ".b");
}

// 0x69F8C0
// Convierte texto GXT a ASCII apto para nombres (>= 0x7F, '/', '*' y '?' pasan a espacio) en un búfer
// estático del juego; como mucho 256 caracteres.
static char* GxtCharToSafeAscii(const GxtChar* src) {
    auto& out = StaticRef<char[257]>(0xC1BED0);
    size_t n = 0;
    if (src) {
        for (; n <= 0xFF && src[n]; ++n) {
            const auto c = src[n];
            out[n] = (c >= 0x7F || c == '/' || c == '*' || c == '?') ? ' ' : (char)c;
        }
    }
    out[n] = 0;
    return out;
}

// 0x619140
void C_PcSave::PopulateSlotInfo() {
    s_PcSaveHelper.error = eErrorCode::NONE;

    for (auto i = 0u; i < std::size(CGenericGameStorage::ms_Slots); ++i) {
        CGenericGameStorage::ms_Slots[i] = eSlotState::SLOT_FREE;
        CGenericGameStorage::ms_SlotFileName[i][0] = 0;
        CGenericGameStorage::ms_SlotSaveDate[i][0] = 0;
    }

    // El original conserva el texto del mes entre iteraciones: con un mes fuera de rango se reutiliza
    // el anterior (nulo al principio).
    const char* month = nullptr;
    for (auto i = 0u; i < std::size(CGenericGameStorage::ms_Slots); ++i) {
        char path[MAX_PATH];
        CSimpleVariablesSaveStructure vars;

        std::sprintf(path, "%s%i%s", DefaultPCSaveFileName, i + 1, ".b");
        if (auto file = CFileMgr::OpenFile(path, "rb")) {
            CFileMgr::Seek(file, strlen(CGenericGameStorage::ms_BlockTagName), SEEK_SET);
            CFileMgr::Read(file, &vars, sizeof(CSimpleVariablesSaveStructure));
            if (strncmp((const char*)vars.m_szSaveName, TopLineEmptyFile, 26)) {
                memcpy(CGenericGameStorage::ms_SlotFileName[i], vars.m_szSaveName, 48);
                CGenericGameStorage::ms_Slots[i] = eSlotState::SLOT_FILLED;
                CGenericGameStorage::ms_SlotFileName[i][24] = 0; // Truncate the name at 24th character
            }
            CFileMgr::CloseFile(file);
        }

        if (CGenericGameStorage::ms_Slots[i] != eSlotState::SLOT_FILLED) {
            continue;
        }

        if (!CGenericGameStorage::CheckDataNotCorrupt(i, path)) {
            // El original deja el slot como ocupado y pone el aviso (con el número de slot base 1) como nombre.
            CMessages::InsertNumberInString(TheText.Get("FEC_SLC"), i + 1, -1, -1, -1, -1, -1, CGenericGameStorage::ms_SlotFileName[i]);
            continue;
        }

        const auto& time = vars.m_systemTime;
        if ((uint32)(time.wMonth - 1) <= 11) {
            char monthGXTKey[16];
            std::sprintf(monthGXTKey, "MONTH%d", (int32)time.wMonth);
            month = GxtCharToSafeAscii(TheText.Get(monthGXTKey));
        }

        char date[256];
        std::sprintf(date, "%02d %s %04d %02d:%02d:%02d", time.wDay, month, time.wYear, time.wHour, time.wMinute, time.wSecond);
        AsciiToGxtChar(date, CGenericGameStorage::ms_SlotSaveDate[i]);
    }
}

// 0x619060
uint32 C_PcSave::SaveSlot(int32 slot) {
    CGenericGameStorage::MakeValidSaveName((int8)slot);
    s_PcSaveHelper.error = eErrorCode::NONE;
    InitUserDirectories();
    CGenericGameStorage::DoGameSpecificStuffBeforeSave();
    return CGenericGameStorage::GenericSave() ? 0 : 2;
}

// 0x6190D0
// Devuelve true si, tras borrarlo, el archivo ya no se puede abrir.
bool C_PcSave::DeleteSlot(int32 slot) {
    char path[MAX_PATH];
    s_PcSaveHelper.error = eErrorCode::NONE;
    std::sprintf(path, "%s%i.b", DefaultPCSaveFileName, (int8)slot + 1);
    DeleteFileA(path);
    if (auto file = CFileMgr::OpenFile(path, "rb")) {
        CFileMgr::CloseFile(file);
        return false;
    }
    return true;
}
