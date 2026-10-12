#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)
#include "PedSaveStructure.h"
#include "EntryExitManager.h"
#include "EntryExit.h"

void CPedSaveStructure::Extract(CPed* ped) {
    plugin::CallMethod<0x5D44B0, CPedSaveStructure*, CPed*>(this, ped);
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
    RH_ScopedInstall(Construct, 0x5D43D0);
}
