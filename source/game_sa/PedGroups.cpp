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
    RH_ScopedInstall(AddGroup, 0x5FB800);
    RH_ScopedInstall(Init, 0x5FB8C0);
    RH_ScopedInstall(CleanUpForShutDown, 0x5FB930);
    RH_ScopedInstall(RemoveGroup, 0x5FB870);
    RH_ScopedInstall(GetPedsGroup, 0x5F7E80);
    RH_ScopedInstall(IsGroupLeader, 0x5F7E40);
    RH_ScopedInstall(GetGroupId, 0x5F7EE0);
    RH_ScopedInstall(IsInPlayersGroup, 0x5F7F10);
    RH_ScopedInstall(AreInSameGroup, 0x5F7F40);
}

#ifdef ANDROID
void CPedGroups::Save() {
    
}

void CPedGroups::Load() {
    
}
#endif

namespace {
__declspec(naked) void Task10RemoveMember() {
    __asm {
        push 005F80D0h
        ret
    }
}
__declspec(naked) void Task10FlushIntelligence() {
    __asm {
        push 005F7350h
        ret
    }
}
}

// return the index of the added group , return -1 if failed.
// 0x5FB800
__declspec(naked) int32 CPedGroups::AddGroup() {
    __asm {
        push        ebx
        or          eax,0FFFFFFFFh
        xor         ebx,ebx
task10_5FB806:
        mov         cl,byte ptr [ebx+00C098E0h]
        test        cl,cl
        je          task10_5FB818
        inc         ebx
        cmp         ebx,8
        jl          task10_5FB806
        pop         ebx
        ret
task10_5FB818:
        push        ebp
        push        esi
        mov         esi,ebx
        imul        esi,esi,2D4h
        add         esi,0C09920h
        push        edi
        mov         byte ptr [ebx+00C098E0h],1
        xor         edi,edi
        lea         ebp,[esi+0Ch]
task10_5FB835:
        cmp         dword ptr [ebp],0
        je          task10_5FB844
        push        edi
        lea         ecx,[esi+8]
        call Task10RemoveMember
task10_5FB844:
        inc         edi
        add         ebp,4
        cmp         edi,8
        jl          task10_5FB835
        lea         ecx,[esi+30h]
        call Task10FlushIntelligence
        pop         edi
        mov         byte ptr [esi+000002D0h],0
        pop         esi
        pop         ebp
        mov         eax,ebx
        pop         ebx
        ret
    }
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
__declspec(naked) void CPedGroups::Init() {
    __asm {
        push        ebx
        push        ebp
        push        esi
        push        edi
        xor         ebx,ebx
        mov         edi,0C09928h
        jmp         task10_5FB8D0
        lea         ecx,[ecx]
task10_5FB8D0:
        mov         al,byte ptr [ebx+00C098E0h]
        test        al,al
        je          task10_5FB90C
        mov         byte ptr [ebx+00C098E0h],0
        xor         esi,esi
        lea         ebp,[edi+4]
task10_5FB8E6:
        cmp         dword ptr [ebp],0
        je          task10_5FB8F4
        push        esi
        mov         ecx,edi
        call Task10RemoveMember
task10_5FB8F4:
        inc         esi
        add         ebp,4
        cmp         esi,8
        jl          task10_5FB8E6
        lea         ecx,[edi+28h]
        call Task10FlushIntelligence
        mov         byte ptr [edi+000002C8h],0
task10_5FB90C:
        mov         word ptr [ebx*2+00C098D0h],1
        add         edi,2D4h
        inc         ebx
        cmp         edi,0C0AFC8h
        jl          task10_5FB8D0
        pop         edi
        pop         esi
        pop         ebp
        pop         ebx
        ret
    }
}

// 0x5F7E30
void CPedGroups::RegisterKillByPlayer() {
    if (ms_bIsPlayerOnAMission)
        ms_iNoOfPlayerKills++;
}

// 0x5FB930
__declspec(naked) void CPedGroups::CleanUpForShutDown() {
    __asm {
        push        ecx
        push        ebp
        push        esi
        push        edi
        mov         edi,0C09928h
        mov         dword ptr [esp+0Ch],8
task10_5FB941:
        xor         esi,esi
        lea         ebp,[edi+4]
task10_5FB946:
        cmp         dword ptr [ebp],0
        je          task10_5FB954
        push        esi
        mov         ecx,edi
        call Task10RemoveMember
task10_5FB954:
        inc         esi
        add         ebp,4
        cmp         esi,8
        jl          task10_5FB946
        lea         ecx,[edi+28h]
        call Task10FlushIntelligence
        mov         eax,dword ptr [esp+0Ch]
        mov         byte ptr [edi+000002C8h],0
        add         edi,2D4h
        dec         eax
        mov         dword ptr [esp+0Ch],eax
        jne         task10_5FB941
        pop         edi
        pop         esi
        pop         ebp
        pop         ecx
        ret
    }
}

// 0x5F7E40
bool CPedGroups::IsGroupLeader(CPed* ped) {
    // 0x5F7E50..0x5F7E73: ignora grupos inactivos y rechaza ped nulo.
    for (size_t i = 0; i < ms_groups.size(); ++i) {
        if (ms_activeGroups[i] && ped && ms_groups[i].GetMembership().GetLeader() == ped) {
            return true;
        }
    }
    return false;
}

// 0x5F7E80
CPedGroup* CPedGroups::GetPedsGroup(const CPed* ped) {
    // 0x5F7E90..0x5F7ED6: primer grupo activo que contiene el puntero,
    // seguidores antes del líder; un puntero nulo nunca pertenece a un grupo.
    for (size_t i = 0; i < ms_groups.size(); ++i) {
        if (!ms_activeGroups[i] || !ped) {
            continue;
        }
        auto& group = ms_groups[i];
        for (const auto* member : group.GetMembership().GetMembers()) {
            if (member == ped) {
                return &group;
            }
        }
    }
    return nullptr;
}

// 0x5F7EE0
int32 CPedGroups::GetGroupId(const CPedGroup* pedGroup) {
    // 0x5F7EF0..0x5F7F09: compara identidades, sin consultar actividad.
    for (size_t i = 0; i < ms_groups.size(); ++i) {
        if (&ms_groups[i] == pedGroup) {
            return static_cast<int32>(i);
        }
    }
    return -1;
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
    // 0x5F7F14 lee player data antes de cualquier prueba: ped debe ser válido.
    // 0x5F7F1E..0x5F7F39 busca sólo el grupo cero, aunque esté inactivo.
    if (ped->GetPlayerData()) {
        return false;
    }
    for (const auto* member : ms_groups[0].GetMembership().GetMembers()) {
        if (member == ped) {
            return true;
        }
    }
    return false;
}

CPedGroup& CPedGroups::GetGroup(int32 groupId) {
    // Como el original: el EXE indexa ms_groups sin comprobar que el grupo esté activo (el assert NOTSA
    // saltaba en juego con grupos inactivos que el EXE acepta). Sin comprobación de rango de std::array.
    return ms_groups.data()[groupId];
}

// 0x5F7F40
bool CPedGroups::AreInSameGroup(const CPed* ped1, const CPed* ped2) {
    // 0x5F7F4A/52 rechazan nulos. No basta comparar GetPedsGroup:
    // el EXE sigue buscando si ped1 aparece en más de un grupo activo.
    if (!ped1 || !ped2) {
        return false;
    }
    for (size_t i = 0; i < ms_groups.size(); ++i) {
        if (!ms_activeGroups[i]) {
            continue;
        }
        auto& membership = ms_groups[i].GetMembership();
        for (const auto* member : membership.GetMembers()) {
            if (member == ped1) {
                if (membership.IsMember(ped2)) { // 0x5F7F85 llama 0x5F6A10
                    return true;
                }
                break;
            }
        }
    }
    return false;
}
