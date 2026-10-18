/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedGroups
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedGroups.h"
#include "X87Math.h"

// Registros con que el EXE sale de AddGroup (0x5FB800), Init (0x5FB8C0) y CleanUpForShutDown (0x5FB930), deducidos del desensamblado del EXE
// (no del residuo de los colaboradores de la DLL, que en Debug es otro):
//  - Flush (0x5F7350) sale con EAX = -1 y ECX = 0 (ultima lectura: el asignador de respuesta, nulo). Su EDX es el de entrada salvo que
//    borre alguna tarea: el destructor virtual termina en CTask::operator delete (0x61A5B0), que deja EDX = puntero global del pool de
//    tareas (0xB744A8). Los cuatro punteros de eventos/asignadores se suponen nulos (si no, ECX/EDX son el residuo de su destructor).
//  - RemoveMember (0x5F80D0) deja EDX = lo que deja CEntity::CleanUpOldReference (0x571A00): sin lista de referencias, el de entrada;
//    con lista, la direccion del hueco, o el siguiente nodo del que apunta al hueco. En un no jugador con 0x474 & 0x2000 el aviso
//    de radar (0x587C60) lo deja en 1. Su ECX/EAX los pisa luego Flush.
//  - AddGroup sin hueco libre: EAX = -1, ECX = ECX de entrada con CL = ms_activeGroups[7], EDX de entrada.
//  - Init: EAX = (EAX previo con AL = ms_activeGroups[g]) en cada vuelta y -1 tras cada Flush; ECX/EDX = los de entrada hasta el primer Flush.
//  - CleanUpForShutDown: EAX = 0 y ECX = 0 (el `pop ecx` final recupera el contador a cero); EDX = el del ultimo Flush.
// Las envolturas de hook (AddGroupExe, InitExe, CleanUpForShutDownExe) reciben ECX/EDX de entrada como __fastcall (mismo `ret`) y terminan
// con ResidueRegsCpp; los metodos de la clase llaman al mismo nucleo sin modelo de registros (regs == nullptr).
namespace {
struct ExeRegs {
    uint32 eax, ecx, edx;
};

// EDX con que sale 0x5F80D0 al quitar el miembro `idx` (aun presente). Se calcula ANTES de llamar al RemoveMember de la clase.
uint32 ExeEdxAfterRemoveMember(const CPedGroup& group, int32 idx, uint32 edx) {
    const auto  slot = reinterpret_cast<uintptr_t>(&group) + 0xC + 4u * static_cast<uint32>(idx); // &m_members[idx]
    const auto* ped  = *reinterpret_cast<const uint8* const*>(slot);
    // 0x571A00: CEntity::m_pReferences (+0x24); cada nodo es { next, ppEntity }
    const auto list = *reinterpret_cast<const uintptr_t*>(ped + 0x24);
    if (list) {
        edx = static_cast<uint32>(slot);
        for (auto node = list; node; node = *reinterpret_cast<const uintptr_t*>(node)) {
            if (*reinterpret_cast<const uintptr_t*>(node + 4) == slot) {
                edx = *reinterpret_cast<const uint32*>(node); // 0x571A20: mov edx,[eax]
                break;
            }
        }
    }
    // 0x5DF8F0: es jugador si el tipo (+0x598) es 0 o 1; solo los demas llegan al aviso de radar
    const auto type = *reinterpret_cast<const uint32*>(ped + 0x598);
    if (type != 0 && type != 1 && (*reinterpret_cast<const uint32*>(ped + 0x474) & 0x2000u)) {
        edx = 1; // 0x587C60 empieza con mov edx,1 y no vuelve a escribirlo
    }
    return edx;
}

// EDX con que sale 0x5F7350: el de entrada, o el puntero del pool de tareas si hay alguna tarea que borrar (cuatro tablas de ocho pares).
uint32 ExeEdxAfterFlush(const CPedGroup& group, uint32 edx) {
    const auto* const bytes = reinterpret_cast<const uint8*>(&group);
    for (const uint32 table : { 0xCu, 0xACu, 0x14Cu, 0x1ECu }) {
        for (uint32 i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
            if (*reinterpret_cast<const uint32*>(bytes + 0x30 + table + 20 * i + 4)) { // CPedTaskPair::Task
                return *reinterpret_cast<const uint32*>(0xB744A8);                    // pool de tareas; 0x61A5B0: mov edx,ecx
            }
        }
    }
    return edx;
}

// 0x5FB835..0x5FB855 (y las copias de Init y CleanUpForShutDown): quita los miembros que existan (mirando el hueco
// en cada vuelta), vacia la inteligencia (0x5F7350) y baja la bandera de grupo de mision.
// Los colaboradores 0x5F80D0 y 0x5F7350 son los de la clase, enlazados directamente.
void EmptyGroupLikeExe(CPedGroup& group, ExeRegs* regs) {
    auto& membership = group.GetMembership();
    for (int32 i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
        if (membership.GetMember(i)) {
            if (regs) {
                regs->edx = ExeEdxAfterRemoveMember(group, i, regs->edx); // antes de quitarlo: lee la lista de referencias
            }
            membership.RemoveMember(i);
        }
    }
    if (regs) {
        regs->edx = ExeEdxAfterFlush(group, regs->edx); // antes de vaciar: lee las tablas de tareas
        regs->ecx = 0;
        regs->eax = 0xFFFFFFFFu;
    }
    group.GetIntelligence().Flush();
    group.m_bIsMissionGroup = false;
}

// 0x5FB800
int32 AddGroupCore(ExeRegs* regs) {
    // .data()[i]: sin los assert de std::array, como el EXE
    for (int32 g = 0; g < 8; g++) {
        if (CPedGroups::ms_activeGroups.data()[g]) {
            continue;
        }
        CPedGroups::ms_activeGroups.data()[g] = 1; // Como el original: se marca antes de vaciar el grupo
        EmptyGroupLikeExe(CPedGroups::ms_groups.data()[g], regs);
        return g;
    }
    if (regs) {
        // 0x5FB806: mov cl,[ebx+0xC098E0] en cada vuelta; la ultima lee el hueco 7
        regs->ecx = (regs->ecx & 0xFFFFFF00u) | static_cast<uint8>(CPedGroups::ms_activeGroups.data()[7]);
    }
    return -1;
}

// 0x5FB8C0
void InitCore(ExeRegs* regs) {
    for (int32 g = 0; g < 8; g++) {
        if (regs) {
            regs->eax = (regs->eax & 0xFFFFFF00u) | static_cast<uint8>(CPedGroups::ms_activeGroups.data()[g]); // 0x5FB8D0 mov al,[..]
        }
        if (CPedGroups::ms_activeGroups.data()[g]) {
            CPedGroups::ms_activeGroups.data()[g] = 0; // Como el original: se baja antes de vaciar el grupo
            EmptyGroupLikeExe(CPedGroups::ms_groups.data()[g], regs);
        }
        CPedGroups::ScriptReferenceIndex.data()[g] = 1; // 0x5FB90C: tambien en los grupos inactivos
    }
}

// 0x5FB930
void CleanUpForShutDownCore(ExeRegs* regs) {
    // Como el original: vacia los ocho grupos sin tocar las banderas de actividad ni los indices SCM
    for (int32 g = 0; g < 8; g++) {
        EmptyGroupLikeExe(CPedGroups::ms_groups.data()[g], regs);
    }
}

// Envolturas de hook con los registros del EXE. /RTC1 añade __RTC_CheckEsp tras las llamadas (cambiaria ECX/EDX) y la cookie de /GS
// tambien: de ahi runtime_checks off y safebuffers (lo vigila contrib/audit_register_residue.py, objeto PedGroups).
#pragma runtime_checks("", off)
__declspec(safebuffers) uint32 __fastcall AddGroupExe(uint32 entryEcx, uint32 entryEdx) {
    ExeRegs     regs{ 0, entryEcx, entryEdx };
    const int32 result = AddGroupCore(&regs);
    return ResidueRegsCpp(regs.ecx, regs.edx, static_cast<uint32>(result));
}

__declspec(safebuffers) uint32 __fastcall InitExe(uint32 entryEcx, uint32 entryEdx) {
    // Lo primero: ninguna instruccion anterior escribe EAX (lo vigila el auditor)
    const uint32 entryEax = EntryEaxCpp();
    ExeRegs      regs{ entryEax, entryEcx, entryEdx };
    InitCore(&regs);
    return ResidueRegsCpp(regs.ecx, regs.edx, regs.eax);
}

__declspec(safebuffers) uint32 __fastcall CleanUpForShutDownExe(uint32 entryEcx, uint32 entryEdx) {
    ExeRegs regs{ 0, entryEcx, entryEdx };
    CleanUpForShutDownCore(&regs);
    return ResidueRegsCpp(0, regs.edx, 0); // `pop ecx` final = contador a cero; `dec eax` final = 0
}
#pragma runtime_checks("", restore)
}

void CPedGroups::InjectHooks() {
    RH_ScopedClass(CPedGroups);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Process, 0x5FC800);
    RH_ScopedNamedGlobalInstall(AddGroupExe, "AddGroup", 0x5FB800); // Como el original: EAX/ECX/EDX de salida
    RH_ScopedNamedGlobalInstall(InitExe, "Init", 0x5FB8C0);
    RH_ScopedNamedGlobalInstall(CleanUpForShutDownExe, "CleanUpForShutDown", 0x5FB930);
    RH_ScopedInstall(RemoveGroup, 0x5FB870);
    RH_ScopedInstall(RemoveAllFollowersFromGroup, 0x5FB8A0);
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

// return the index of the added group , return -1 if failed.
// 0x5FB800
int32 CPedGroups::AddGroup() {
    return AddGroupCore(nullptr);
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
    // Como el original: sin comprobar ni afirmar el indice (acceso por direccion; std::array::operator[] lo vigila en Debug) y con la bandera leida como byte.
    // El EXE termina con `jmp 0x5FB7D0` (CPedGroup::RemoveAllFollowers) con ECX = 0xC09920 + groupId * 0x2D4; su unico llamador (0x492744) no lee los registros.
    if (!ms_activeGroups.data()[groupId]) {
        return;
    }
    (ms_groups.data() + groupId)->RemoveAllFollowers();
}

// 0x5FB8C0
void CPedGroups::Init() {
    InitCore(nullptr);
}

// 0x5F7E30
void CPedGroups::RegisterKillByPlayer() {
    if (ms_bIsPlayerOnAMission)
        ms_iNoOfPlayerKills++;
}

// 0x5FB930
void CPedGroups::CleanUpForShutDown() {
    CleanUpForShutDownCore(nullptr);
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
