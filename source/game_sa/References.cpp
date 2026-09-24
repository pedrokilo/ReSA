/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CReferences
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "References.h"

void CReferences::InjectHooks() {
    RH_ScopedClass(CReferences);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x5719B0);
    RH_ScopedInstall(ListSize, 0x5719E0);
    RH_ScopedInstall(RemoveReferencesToPlayer, 0x571AD0);
    RH_ScopedInstall(PruneAllReferencesInWorld, 0x571CC0);
}

// 0x5719B0
void CReferences::Init() {
    ZoneScoped;

    pEmptyList = aRefs;

    // todo: do better
    for (int32 i = 0; i < MAX_NUM_REFERENCES - 1; ++i) {
        (&aRefs[i])->m_pNext = &aRefs[i + 1];
        (&aRefs[i])->m_ppEntity = nullptr;
    }

    (&aRefs[MAX_NUM_REFERENCES - 1])->m_pNext = nullptr;
}

uint32 CReferences::ListSize(CReference* ref) {
    uint32 count = 0;
    while (ref) {
        ref = ref->m_pNext;
        ++count;
    }

    return count;
}

void CReferences::RemoveReferencesToPlayer() {
    auto vehicle = FindPlayerVehicle();
    if (vehicle)
        vehicle->ResolveReferences();

    auto player = FindPlayerPed();
    if (player) {
        player->ResolveReferences();
        auto& group = CPedGroups::GetGroup(player->GetPlayerData()->m_nPlayerGroup);
        group.GetMembership().SetLeader(player);
        group.Process();
    }
}

void CReferences::PruneAllReferencesInWorld() {
    rng::for_each(GetPedPool()->GetAllValid(), &CEntity::PruneReferences);
    rng::for_each(GetVehiclePool()->GetAllValid(), &CEntity::PruneReferences);
    rng::for_each(GetObjectPool()->GetAllValid(), &CEntity::PruneReferences);
}
