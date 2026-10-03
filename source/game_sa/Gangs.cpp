/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CGangs
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "Gangs.h"

void CGangs::InjectHooks() {
    RH_ScopedClass(CGangs);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5DE680);
    RH_ScopedInstall(Load, 0x5D3A90);
    RH_ScopedInstall(Save, 0x5D3A60);
    RH_ScopedInstall(SetGangWeapons, 0x5DE550);
    RH_ScopedInstall(ChooseGangPedModel, 0x5DE5A0);
    RH_ScopedInstall(GetWillAttackPlayerWithCops, 0x5DE500);
    RH_ScopedInstall(SetWillAttackPlayerWithCops, 0x5DE4E0);
    RH_ScopedInstall(SetGangPedModelOverride, 0x5DE580);
}

// 0x5DE680
void CGangs::Initialise() {
    SetGangWeapons(GANG_BALLAS,     WEAPON_PISTOL,       WEAPON_MICRO_UZI, WEAPON_UNARMED);
    SetGangWeapons(GANG_GROVE,      WEAPON_PISTOL,       WEAPON_UNARMED,   WEAPON_UNARMED);
    SetGangWeapons(GANG_VAGOS,      WEAPON_PISTOL,       WEAPON_UNARMED,   WEAPON_UNARMED);
    SetGangWeapons(GANG_DANANGBOYS, WEAPON_PISTOL,       WEAPON_MICRO_UZI, WEAPON_UNARMED);
    SetGangWeapons(GANG_MAFIA,      WEAPON_DESERT_EAGLE, WEAPON_UNARMED,   WEAPON_UNARMED);
    SetGangWeapons(GANG_TRIAD,      WEAPON_PISTOL,       WEAPON_AK47,      WEAPON_UNARMED);
    SetGangWeapons(GANG_AZTECAS,    WEAPON_PISTOL,       WEAPON_MICRO_UZI, WEAPON_UNARMED);
}

// 0x5D3A90
bool CGangs::Load() {
    for (auto& gang : Gang) {
        CGenericGameStorage::LoadDataFromWorkBuffer(gang);
    }
    return true;
}

// 0x5D3A60
bool CGangs::Save() {
    for (auto& gang : Gang) {
        CGenericGameStorage::SaveDataToWorkBuffer(gang);
    }
    return true;
}

// 0x5DE550
void CGangs::SetGangWeapons(int16 gangId, eWeaponType weapId1, eWeaponType weapId2, eWeaponType weapId3) {
    // Como el original: solo se escriben las armas no negativas (test/jl).
    auto& weapons = Gang[gangId].m_nGangWeapons;
    if ((int32)weapId1 >= 0) {
        weapons[0] = weapId1;
    }
    if ((int32)weapId2 >= 0) {
        weapons[1] = weapId2;
    }
    if ((int32)weapId3 >= 0) {
        weapons[2] = weapId3;
    }
}

// 0x5DE5A0
eModelID CGangs::ChooseGangPedModel(eGangID gangIdArg) {
    const auto gangId = (eGangID)(int16)gangIdArg; // Como el original: movsx de la palabra baja
    const auto pedGroup = CPopulation::GetGangGroupId(gangId, CPopulation::CurrentWorldZone);

    // A gang with a forced ped model simply uses the first model of its group
    if (Gang[gangId].m_nPedModelOverride != -1) {
        return (eModelID)(uint16)CPopulation::m_PedGroups[pedGroup][0];
    }

    // NOTE: The original sizes the random sequence using world zone 0, but looks the model up using the current world zone
    // Como el original: el contador se lee con signo (movsx).
    const int32 numPeds = (int16)CPopulation::m_nNumPedsInGroup[CPopulation::GetGangGroupId(gangId, 0)];
    CCarCtrl::InitSequence(numPeds);
    if (numPeds <= 0) {
        return MODEL_INVALID;
    }

    for (auto i = 0; i < numPeds; i++) {
        const auto model = (uint16)CPopulation::m_PedGroups[pedGroup][CCarCtrl::FindSequenceElement(i)];
        if (CStreaming::IsModelLoaded(model)) {
            return (eModelID)model;
        }
    }
    return MODEL_INVALID;
}

// unused (0x5DE500)
bool CGangs::GetWillAttackPlayerWithCops(ePedType pedType) {
    // Como el original: fuera de PED_TYPE_GANG1..GANG10 devuelve falso.
    if (pedType < PED_TYPE_GANG1 || pedType > PED_TYPE_GANG10) {
        return false;
    }
    return GangAttackWithCops[pedType - PED_TYPE_GANG1];
}

// unused (0x5DE4E0)
void CGangs::SetWillAttackPlayerWithCops(ePedType pedType, bool attackPlayerWithCops) {
    // Como el original: fuera de PED_TYPE_GANG1..GANG10 no hace nada.
    if (pedType < PED_TYPE_GANG1 || pedType > PED_TYPE_GANG10) {
        return;
    }
    GangAttackWithCops[pedType - PED_TYPE_GANG1] = attackPlayerWithCops;
}

// unused (0x5DE580)
void CGangs::SetGangPedModelOverride(int16 gangId, int8 pedModelOverride) {
    Gang[gangId].m_nPedModelOverride = pedModelOverride;
}
