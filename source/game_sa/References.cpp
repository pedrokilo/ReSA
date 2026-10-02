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
 * Validación registrada: 4/4 entradas.
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

    for (int32 i = 0; i < MAX_NUM_REFERENCES - 1; ++i) {
        (&aRefs[i])->m_pNext = &aRefs[i + 1];
        (&aRefs[i])->m_ppEntity = nullptr;
    }

    // Como el original: el ultimo nodo tambien queda con m_ppEntity a null (el bucle del EXE da 3000 vueltas)
    (&aRefs[MAX_NUM_REFERENCES - 1])->m_ppEntity = nullptr;
    (&aRefs[MAX_NUM_REFERENCES - 1])->m_pNext = nullptr;
}

// 0x5719E0
uint32 CReferences::ListSize(CReference* ref) {
    uint32 count = 0;
    while (ref) {
        ref = ref->m_pNext;
        ++count;
    }

    return count;
}

// 0x571AD0
void CReferences::RemoveReferencesToPlayer() {
    // Como el original: FindPlayerVehicle/FindPlayerPed se vuelven a llamar en cada uso
    if (FindPlayerVehicle())
        FindPlayerVehicle()->ResolveReferences();

    if (FindPlayerPed()) {
        FindPlayerPed()->ResolveReferences();
        auto& group = CPedGroups::GetGroup(FindPlayerPed()->GetPlayerData()->m_nPlayerGroup);
        group.GetMembership().SetLeader(FindPlayerPed());
        CPedGroups::GetGroup(FindPlayerPed()->GetPlayerData()->m_nPlayerGroup).Process();
    }
}

// 0x571CC0
void CReferences::PruneAllReferencesInWorld() {
    // Como el original: cada pool se recorre desde el ultimo hueco hacia el primero
    const auto prune = [](auto* pool) {
        for (auto i = (int32)pool->GetSize(); i > 0; --i) {
            if (auto* entity = pool->GetAt((size_t)(i - 1))) {
                entity->PruneReferences();
            }
        }
    };
    prune(GetPedPool());
    prune(GetVehiclePool());
    prune(GetObjectPool());
}
