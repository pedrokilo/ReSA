/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedSaveStructure
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)
#include "PedSaveStructure.h"
#include "EntryExitManager.h"
#include "EntryExit.h"

// 0x5D44B0
void CPedSaveStructure::Extract(CPed* ped) {
    // Como el original: posicion (matriz o colocacion), vida, armadura, slot activo, creador y estilo de lucha
    ped->GetPosition() = m_pos;
    ped->m_fHealth     = m_health;
    ped->m_fArmour     = m_armor;
    ped->m_nActiveWeaponSlot = static_cast<uint8>(m_activeWeaponSlot);
    ped->SetCharCreatedBy(m_createdBy);
    ped->m_nFightingStyle      = static_cast<eFightingStyle>(m_nFightingStyle);
    ped->m_nAllowedAttackMoves = static_cast<char>(m_nAllowedAttackMoves);

    // Armas: carga los modelos (flags 8) y las entrega; despues restaura la municion del cargador
    for (size_t i = 0; i < std::size(m_weapons); i++) {
        const CWeapon& saved = m_weapons[i];
        if (static_cast<int32>(saved.m_Type) == 0) {
            continue;
        }
        const auto modelId1 = CWeaponInfo::GetWeaponInfo(saved.m_Type, eWeaponSkill::STD)->m_nModelId1;
        if (modelId1 != -1) {
            CStreaming::RequestModel(modelId1, 8);
            CStreaming::LoadAllRequestedModels(false);
        }
        const auto modelId2 = CWeaponInfo::GetWeaponInfo(saved.m_Type, eWeaponSkill::STD)->m_nModelId2;
        if (modelId2 != -1) {
            CStreaming::RequestModel(modelId2, 8);
            CStreaming::LoadAllRequestedModels(false);
        }
        ped->GiveWeapon(saved.m_Type, saved.m_TotalAmmo, false);
        ped->m_aWeapons[i].m_AmmoInClip = saved.m_AmmoInClip;
    }
    ped->SetCurrentWeapon(static_cast<int32>(m_activeWeaponSlot));
    ped->SetAreaCode(static_cast<eAreaCodes>(m_areaCode));

    // Como el original: la entrada/salida se recupera del pool por indice (-1 = ninguna; hueco libre = ninguna)
    // El EXE (0x5D45D7) no comprueba el rango del indice: lee el byte de estado [indice] del pool 0x96A7D8 (bit 7 = libre)
    // y devuelve almacenamiento + indice * 0x3C. CPool::GetAt lo comprobaria con assert, asi que se lee en crudo
    // (primer miembro = almacenamiento, segundo = estados).
    CEntryExit* enex = nullptr;
    if (const int32 idx = m_nExitIndex; idx != -1) {
        const auto* const pool   = reinterpret_cast<const uintptr_t*>(CEntryExitManager::GetPool());
        const auto        states = reinterpret_cast<const int8*>(pool[1]);
        if (states[idx] >= 0) {
            enex = reinterpret_cast<CEntryExit*>(pool[0] + static_cast<uint32>(idx) * sizeof(CEntryExit));
        }
    }
    ped->m_pEnex = enex;
}

// 0x5D43D0
uint64 CPedSaveStructure::Construct(CPed* ped) {
    // Como el original: posicion de la matriz o de la colocacion, vida y armadura
    m_pos    = ped->GetPosition();
    m_health = ped->m_fHealth;
    m_armor  = ped->m_fArmour;
    m_createdBy           = static_cast<ePedCreatedBy>(ped->GetCreatedBy());
    m_activeWeaponSlot    = static_cast<int8>(ped->m_nActiveWeaponSlot);
    m_areaCode            = static_cast<int8>(ped->GetAreaCode());
    m_nFightingStyle      = static_cast<uint8>(ped->m_nFightingStyle);
    m_nAllowedAttackMoves = static_cast<uint8>(ped->m_nAllowedAttackMoves);

    // Residuo de EAX y EDX del EXE: EDX = bits de la vida con el byte bajo reemplazado por los movimientos de ataque permitidos;
    // EAX = la entrada/salida del ped (0 si no hay)
    uint32 eax = reinterpret_cast<uintptr_t>(ped->m_pEnex);
    uint32 edx = (std::bit_cast<uint32>(ped->m_fHealth) & 0xFFFFFF00u) | static_cast<uint8>(ped->m_nAllowedAttackMoves);

    // Como el original: indice de la entrada/salida (division con signo entre el tamano del objeto del pool) solo si ella
    // o su enlace tiene area distinta de cero; si no, -1
    m_nExitIndex = -1;
    if (auto* const enex = ped->m_pEnex) {
        const auto* const target = enex->m_pLink ? enex->m_pLink : enex;
        edx = reinterpret_cast<uintptr_t>(target);
        if (target->m_nArea != 0) {
            // imul por 0x88888889, suma, sar 5 y correccion del signo (el EXE deja el signo en EAX y el indice en EDX)
            const auto storage = *reinterpret_cast<const uintptr_t*>(CEntryExitManager::GetPool()); // primer miembro del pool: el almacenamiento
            const int32 diff   = static_cast<int32>(reinterpret_cast<uintptr_t>(enex) - storage);
            const int32 high   = static_cast<int32>((static_cast<int64>(static_cast<int32>(0x88888889)) * diff) >> 32);
            const int32 shifted = (high + diff) >> 5;
            eax = static_cast<uint32>(shifted) >> 31;
            edx = static_cast<uint32>(shifted) + eax;
            m_nExitIndex = static_cast<int32>(edx);
        }
    }

    // Como el original: las trece armas se copian enteras (0x5B dwords)
    std::memcpy(m_weapons, ped->m_aWeapons.data(), sizeof(m_weapons));
    return (static_cast<uint64>(edx) << 32) | eax;
}

void CPedSaveStructure::InjectHooks() {
    RH_ScopedClass(CPedSaveStructure);
    RH_ScopedCategory("Entity/Ped");
    RH_ScopedInstall(Extract, 0x5D44B0);
    RH_ScopedInstall(Construct, 0x5D43D0);
}
