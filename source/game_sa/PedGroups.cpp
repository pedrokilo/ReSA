/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedGroups
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedGroups.h"

void CPedGroups::InjectHooks() {
    RH_ScopedClass(CPedGroups);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Process, 0x5FC800);
    RH_ScopedInstall(RemoveGroup, 0x5FB870);
}

#ifdef ANDROID
void CPedGroups::Save() {
    
}

void CPedGroups::Load() {
    
}
#endif

// return the index of the added group , return -1 if failed.
// 0x5FB800
int32 CPedGroups::AddGroup() {
    return plugin::CallAndReturn<int32, 0x5FB800>();
}

// 0x5FB870
void CPedGroups::RemoveGroup(int32 groupId) {
    // Como el original: sin comprobar (ni afirmar) que el grupo esté activo antes de mirar su bandera.
    if (!std::exchange(ms_activeGroups[groupId], false)) {
        return;
    }
    ms_groups[groupId].Flush();
}

// 0x5FB8A0
void CPedGroups::RemoveAllFollowersFromGroup(int32 groupId) {
    plugin::Call<0x5FB8A0, int32>(groupId);
}

// 0x5FB8C0
void CPedGroups::Init() {
    plugin::Call<0x5FB8C0>();
}

// 0x5F7E30
void CPedGroups::RegisterKillByPlayer() {
    if (ms_bIsPlayerOnAMission)
        ms_iNoOfPlayerKills++;
}

// 0x5FB930
void CPedGroups::CleanUpForShutDown() {
    plugin::Call<0x5FB930>();
}

// 0x5F7E40
bool CPedGroups::IsGroupLeader(CPed* ped) {
    return plugin::CallAndReturn<bool, 0x5F7E40, CPed*>(ped);
}

// 0x5F7E80
CPedGroup* CPedGroups::GetPedsGroup(const CPed* ped) {
    return plugin::CallAndReturn<CPedGroup*, 0x5F7E80>(ped);
}

// 0x5F7EE0
int32 CPedGroups::GetGroupId(const CPedGroup* pedGroup) {
    return plugin::CallAndReturn<int32, 0x5F7EE0, const CPedGroup*>(pedGroup);
}

// 0x5FC800
void CPedGroups::Process() {
    for (auto i = 0; i < (int32)ms_groups.size(); i++) {
        if (!ms_activeGroups[i]) {
            continue;
        }
        auto& g = ms_groups[i];
        g.Process();
        if (!g.GetMembership().CountMembers()) {
            RemoveGroup(i);
        }
    }

    // Como el original: el tipo de decisión solo se cambia en los grupos activos que no son de misión.
    const auto SetGroupsDecisionMakerType = [](eDecisionMakerType dm) {
        for (auto i = 0; i < (int32)ms_groups.size(); i++) {
            if (ms_activeGroups[i] && !ms_groups[i].m_bIsMissionGroup) {
                ms_groups[i].GetIntelligence().SetGroupDecisionMakerType(dm);
            }
        }
    };
    if (CTheScripts::IsPlayerOnAMission() && !ms_bIsPlayerOnAMission) { // 0x5FC897 - Player started a mission
        ms_iNoOfPlayerKills = 0;
        SetGroupsDecisionMakerType(eDecisionMakerType::GROUP_RANDOM_PASSIVE);
    } else if (!CTheScripts::IsPlayerOnAMission() && ms_bIsPlayerOnAMission) { // 0x5FC8B2 - Player ended a mission
        ms_iNoOfPlayerKills = 0; // Como el original
        SetGroupsDecisionMakerType(eDecisionMakerType::UNKNOWN);
    } else if (CTheScripts::IsPlayerOnAMission() && CPedGroups::ms_iNoOfPlayerKills == 8) { // 0x5FC8BF
        SetGroupsDecisionMakerType(eDecisionMakerType::UNKNOWN);
    } else if (CTheScripts::IsPlayerOnAMission()) {
        SetGroupsDecisionMakerType(eDecisionMakerType::GROUP_RANDOM_PASSIVE);
    }

    // 0x5FC98D - Como el original: el contador se pone a cero mientras NO se está en misión (según el valor nuevo).
    ms_bIsPlayerOnAMission = CTheScripts::IsPlayerOnAMission();
    if (!ms_bIsPlayerOnAMission) {
        ms_iNoOfPlayerKills = 0;
    }
}

// 0x5F7F10
bool CPedGroups::IsInPlayersGroup(CPed* ped) {
    return plugin::CallAndReturn<bool, 0x5F7F10, CPed*>(ped);
}

CPedGroup& CPedGroups::GetGroup(int32 groupId) {
    assert(ms_activeGroups[groupId]);
    return ms_groups[groupId];
}

// 0x5F7F40
bool CPedGroups::AreInSameGroup(const CPed* ped1, const CPed* ped2) {
    return plugin::CallAndReturn<bool, 0x5F7F40, const CPed*, const CPed*>(ped1, ped2);
}
