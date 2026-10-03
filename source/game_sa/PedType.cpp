/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedType
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 13/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedType.h"
CAcquaintance* CPedType::ms_apPedTypesOld = {}; // NOTSA (ver s_PedTypesOldStorage)

void CPedType::InjectHooks() {
    RH_ScopedClass(CPedType);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x608E40);
    RH_ScopedInstall(Shutdown, 0x608B00);
    RH_ScopedInstall(Save, 0x5D3CD0);
    RH_ScopedInstall(Load, 0x5D3D10);
    RH_ScopedInstall(LoadPedData, 0x608B30);
    RH_ScopedInstall(FindPedType, 0x608790);
    RH_ScopedInstall(GetPedFlag, 0x608830);
    RH_ScopedOverloadedInstall(GetPedTypeAcquaintances, "CAcquaintance", 0x6089B0, CAcquaintance&(*)(ePedType));
    RH_ScopedOverloadedInstall(GetPedTypeAcquaintances, "uint32", 0x6089D0, uint32(*)(AcquaintanceId, ePedType));
    RH_ScopedInstall(SetPedTypeAsAcquaintance, 0x608E20);
    RH_ScopedInstall(ClearPedTypeAcquaintances, 0x608A20);
    RH_ScopedInstall(ClearPedTypeAsAcquaintance, 0x6089F0);
    RH_ScopedInstall(PoliceDontCareAboutCrimesAgainstPedType, 0x608A40);
}

// NOTSA: copia de seguridad que usa MayhemCheat (Cheat.cpp). Vive en almacenamiento propio para no añadir
// reservas que el EXE no hace en Initialise.
static std::array<CAcquaintance, PED_TYPE_COUNT> s_PedTypesOldStorage{};

// 0x608E40
void CPedType::Initialise() {
    // Como el original: new[] de 32 entradas con su cookie (0x284 bytes, la cuenta delante), constructor de cada
    // entrada y después otra pasada que las vuelve a poner a cero.
    auto* const block = static_cast<uint32*>(operator new[](sizeof(uint32) + sizeof(CAcquaintance) * PED_TYPE_COUNT));
    CAcquaintance* types = nullptr;
    if (block) {
        block[0] = PED_TYPE_COUNT;
        types = reinterpret_cast<CAcquaintance*>(block + 1);
        for (uint32 i = 0; i < PED_TYPE_COUNT; i++) {
            new (&types[i]) CAcquaintance();
        }
    }
    ms_apPedTypes = types;
    for (uint32 i = 0; i < PED_TYPE_COUNT; i++) {
        auto& acquaintance = ms_apPedTypes[i];
        acquaintance.m_nRespect = 0;
        acquaintance.m_nLike    = 0;
        acquaintance.m_nIgnore  = 0;
        acquaintance.m_nDislike = 0;
        acquaintance.m_nHate    = 0;
    }

    // NOTSA
    s_PedTypesOldStorage.fill(CAcquaintance{});
    ms_apPedTypesOld = s_PedTypesOldStorage.data();

    LoadPedData();
}

// 0x608B00
void CPedType::Shutdown() {
    if (ms_apPedTypes == s_PedTypesOldStorage.data()) { // NOTSA: MayhemCheat activo, recupera el bloque reservado
        std::swap(ms_apPedTypes, ms_apPedTypesOld);
    }
    // Como el original: delete[] del bloque con cookie (el puntero no se pone a nulo)
    if (ms_apPedTypes) {
        operator delete[](reinterpret_cast<uint32*>(ms_apPedTypes) - 1);
    }
}

// 0x608B30
void CPedType::LoadPedData() {
    // Como el original: hasta la primera línea de tipo se escribe en la entrada 32 (fuera de la tabla)
    auto pedType = PED_TYPE_COUNT;
    auto file = CFileMgr::OpenFile("DATA\\PED.DAT", "r");
    for (char* line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '#' || !*line)
            continue;

        char word[32];
        sscanf(line, "%s", word);

        // Como el original: strtok sobre la línea; el primer token es la palabra clave
        const auto ReadFlags = [line]() -> uint32 {
            uint32 flags = 0;
            strtok(line, " ,\t");
            for (const char* pedTypeName = strtok(nullptr, " ,\t"); pedTypeName; pedTypeName = strtok(nullptr, " ,\t")) {
                flags |= GetPedFlag(FindPedType(pedTypeName));
            }
            return flags;
        };

        // Como el original: comparación exacta (no por prefijo)
        if (!strcmp(word, "Hate")) {
            const auto flags = ReadFlags();
            GetPedTypeAcquaintances(pedType).m_nHate = flags;
        } else if (!strcmp(word, "Dislike")) {
            const auto flags = ReadFlags();
            GetPedTypeAcquaintances(pedType).m_nDislike = flags;
        } else if (!strcmp(word, "Like")) {
            const auto flags = ReadFlags();
            GetPedTypeAcquaintances(pedType).m_nLike = flags;
        } else if (!strcmp(word, "Respect")) {
            const auto flags = ReadFlags();
            GetPedTypeAcquaintances(pedType).m_nRespect = flags;
        } else {
            pedType = FindPedType(word);
        }
    }
    CFileMgr::CloseFile(file);
}

// 0x5D3D10
bool CPedType::Load() {
    for (uint32 i = 0; i < PED_TYPE_COUNT; ++i ) {
        for (auto id = 0; id < ACQUAINTANCE_NUM; ++id) {
            auto value = CGenericGameStorage::LoadDataFromWorkBuffer<uint32>();
            GetPedTypeAcquaintances(static_cast<ePedType>(i)).SetAcquaintances(id, value);
        }
    }
    return true; // Como el original: devuelve true en al
}

// 0x5D3CD0
bool CPedType::Save() {
    for (uint32 i = 0; i < PED_TYPE_COUNT; ++i ) {
        for (auto id = 0; id < ACQUAINTANCE_NUM; ++id) {
            uint32 value = GetPedTypeAcquaintances(static_cast<ePedType>(i)).GetAcquaintances(id);
            CGenericGameStorage::SaveDataToWorkBuffer(value);
        }
    }
    return true; // Como el original: devuelve true en al
}

// 0x608790
ePedType CPedType::FindPedType(const char* pedTypeName) {
    // Como el original: tabla de nombres 0x8D23B8 recorrida con strcmp
    static constexpr const char* s_PedTypeNames[PED_TYPE_COUNT] = {
        "PLAYER1", "PLAYER2", "PLAYER_NETWORK", "PLAYER_UNUSED", "CIVMALE", "CIVFEMALE", "COP", "GANG1",
        "GANG2", "GANG3", "GANG4", "GANG5", "GANG6", "GANG7", "GANG8", "GANG9",
        "GANG10", "DEALER", "MEDIC", "FIREMAN", "CRIMINAL", "BUM", "PROSTITUTE", "SPECIAL",
        "MISSION1", "MISSION2", "MISSION3", "MISSION4", "MISSION5", "MISSION6", "MISSION7", "MISSION8",
    };
    for (uint32 i = 0; i < PED_TYPE_COUNT; i++) {
        if (!strcmp(pedTypeName, s_PedTypeNames[i])) {
            return static_cast<ePedType>(i);
        }
    }
    // Como el original: dos comparaciones más (redundantes) y PED_TYPE_COUNT si el nombre no existe
    if (!strcmp(pedTypeName, "PLAYER_NETWORK")) {
        return PED_TYPE_PLAYER_NETWORK;
    }
    return !strcmp(pedTypeName, "PLAYER_UNUSED") ? PED_TYPE_PLAYER_UNUSED : PED_TYPE_COUNT;
}

// 0x608830
uint32 CPedType::GetPedFlag(ePedType pedType) {
    // Como el original: comparación con signo (jge 32) y shl, que solo usa los 5 bits bajos
    const auto type = static_cast<int32>(pedType);
    if (type < 32) {
        return 1u << (type & 31);
    }
    return 0;
}

// 0x6089B0
CAcquaintance& CPedType::GetPedTypeAcquaintances(ePedType pedType) {
    return ms_apPedTypes[static_cast<int32>(pedType)];
}

// 0x6089D0
uint32 CPedType::GetPedTypeAcquaintances(AcquaintanceId id, ePedType pedType) {
    return GetPedTypeAcquaintances(pedType).GetAcquaintances(id);
}

// 0x608E20
void CPedType::SetPedTypeAsAcquaintance(AcquaintanceId id, ePedType pedType, int32 pedTypeBitNum) {
    GetPedTypeAcquaintances(pedType).SetAsAcquaintance(id, pedTypeBitNum);
}

// 0x608A20
void CPedType::ClearPedTypeAcquaintances(AcquaintanceId id, ePedType pedType) {
    GetPedTypeAcquaintances(pedType).ClearAcquaintances(id);
}

// 0x6089F0
void CPedType::ClearPedTypeAsAcquaintance(AcquaintanceId id, ePedType pedType, int32 pedTypeBitNum) {
    GetPedTypeAcquaintances(pedType).ClearAsAcquaintance(id, pedTypeBitNum);
}

// 0x608A40
bool CPedType::PoliceDontCareAboutCrimesAgainstPedType(ePedType pedType) {
    return pedType >= PED_TYPE_GANG1 && pedType <= PED_TYPE_GANG10 || pedType == PED_TYPE_CRIMINAL || pedType == PED_TYPE_DEALER;
}
