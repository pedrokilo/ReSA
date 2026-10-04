/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedGroupMembership
 * Reconstrucción: COMPLETA — 18/18 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 18/18 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <functional>
#include "PedGroupMembership.h"

CPedGroupMembership::CPedGroupMembership(CPedGroup& group) :
    m_group{&group}
{
}

CPedGroupMembership::~CPedGroupMembership() {
    Flush();
}

// 0x5F7FE0
void CPedGroupMembership::From(const CPedGroupMembership& obj) {
    // Como el original: AddMember con los 8 huecos (tambien el lider), sin limpiar los actuales
    for (auto i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
        AddMember(obj.m_members.data()[i], i);
    }
    m_group           = obj.m_group;
    m_separationRange = obj.m_separationRange;
}

// 0x5F8020
void CPedGroupMembership::AddFollower(CPed* ped) {
    ped->bHasGroupDriveTask = false;

    // Peds in the player's group can't drown
    // Como el original: se mira GetPlayerData() del lider, no su tipo
    if (const auto leader = GetLeader(); leader && leader->GetPlayerData()) {
        ped->bDrownsInWater = false;
    }

    // Como el original: busqueda en los 7 seguidores y luego el lider, sin comprobar nulo
    for (auto i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
        if (m_members.data()[i] == ped) {
            return;
        }
    }
    if (GetLeader() == ped) {
        return;
    }

    // Como el original: solo los huecos de seguidores (0..6); nunca el del lider
    auto memId = 0;
    for (; memId < TOTAL_PED_GROUP_FOLLOWERS; memId++) {
        if (!m_members.data()[memId]) {
            break;
        }
    }
    if (memId == TOTAL_PED_GROUP_FOLLOWERS) {
        return;
    }

    AddMember(ped, memId);
    GivePedRandomObjectToHold(ped);
}

// 0x5F6AE0
void CPedGroupMembership::AddMember(CPed* member, int32 memIdx) {
    assert(member);
    // Como el original: no se comprueba el hueco (From lo sobrescribe sin limpiar)

    auto& slot = m_members.data()[memIdx];
    slot = member;
    CEntity::RegisterReference(slot);
    /* dead code before checking if the member is in the player's group */
    if (!slot->IsPlayer()) {
        slot->GetIntelligence()->SetPedDecisionMakerType(0);
    }
}

// 0x5FB240
void CPedGroupMembership::AppointNewLeader() {
    if (HasLeader()) {
        return;
    }

    // Como el original: primer seguidor no nulo (0..6)
    auto memId = 0;
    for (; memId < TOTAL_PED_GROUP_FOLLOWERS; memId++) {
        if (m_members.data()[memId]) {
            break;
        }
    }
    if (memId == TOTAL_PED_GROUP_FOLLOWERS) {
        return;
    }

    const auto leader = m_members.data()[memId];
    RemoveMember(memId); // Must call as it does some cleanup
    AddMember(leader, LEADER_MEM_ID);
}

// 0x5F6A50
size_t CPedGroupMembership::CountMembers() {
    return rng::count_if(m_members, notsa::NotIsNull{});
}

// 0x5F6AA0
int32 CPedGroupMembership::CountMembersExcludingLeader() {
    // Como el original: cuenta los huecos 0..6
    int32 cnt = 0;
    for (auto i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
        if (m_members.data()[i]) {
            cnt++;
        }
    }
    return cnt;
}

// 0x5FB160
void CPedGroupMembership::Flush() {
    for (auto&& [i, mem] : rngv::enumerate(m_members)) {
        if (mem) {
            RemoveMember(i);
        }
    }
}

// 0x5F69A0
CPed* CPedGroupMembership::GetLeader() const {
    return m_members[LEADER_MEM_ID];
}

// 0x5F69B0
CPed* CPedGroupMembership::GetMember(int32 memberId) {
    return m_members.data()[memberId]; // Como el original: sin comprobar el indice
}

int32 CPedGroupMembership::GetMemberId(const CPed* ped) const {
    for (auto&& [id, mem] : rngv::enumerate(m_members)) {
        if (mem == ped) {
            return id;
        }
    }
    return -1;
}

// 0x5F69E0
bool CPedGroupMembership::IsFollower(const CPed* ped) const {
    // Como el original: solo se busca en los huecos 0..6 (no se mira el lider)
    if (!ped) {
        return false;
    }
    for (auto i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
        if (m_members.data()[i] == ped) {
            return true;
        }
    }
    return false;
}

// 0x5F69C0
bool CPedGroupMembership::IsLeader(const CPed* ped) const {
    return ped && GetLeader() == ped;
}

// 0x5F6A10
bool CPedGroupMembership::IsMember(const CPed* ped) const {
    return ped && notsa::contains(m_members, ped);
}

// 0x5FBA60
void CPedGroupMembership::Process() {
    // Remove dead members (except player ped)
    for (auto i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
        const auto mem = m_members.data()[i];
        if (!mem || mem->IsAlive()) {
            continue;
        }
        if (i == LEADER_MEM_ID && GetLeader()->IsPlayer()) { // Como el original: por indice (el jugador lider se queda)
            continue;
        }
        RemoveMember(i);
    }

    // If no leader, try appointing a new one
    if (!HasLeader()) {
        AppointNewLeader();
        if (!HasLeader()) { // Couldn't appoint a new leader
            return;
        }
    }

    // Now that we have a leader, check for separation distance and remove members further than it
    // Como el original: solo los huecos 0..6; se quita si r^2 < d^2 (NaN se queda) y ademas esta en la misma area que el lider
    for (auto i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
        const auto mem = m_members.data()[i];
        if (!mem || mem->bNeverLeavesGroup) {
            continue;
        }
        const auto  leader    = GetLeader();
        const auto& leaderPos = leader->GetPosition();
        const auto& memPos    = mem->GetPosition();
        const float dx        = memPos.x - leaderPos.x;
        const float dy        = memPos.y - leaderPos.y;
        const float dz        = memPos.z - leaderPos.z;
        const float distSq    = dz * dz + dy * dy + dx * dx; // Como el original: orden z, y, x
        const float sepSq     = m_separationRange * m_separationRange;
        if (!(sepSq < distSq)) {
            continue;
        }
        if (mem->GetAreaCode() != leader->GetAreaCode()) {
            continue;
        }
        RemoveMember(i); // Ped is too far
    }
}

// 0x5FB190
void CPedGroupMembership::RemoveAllFollowers(bool bCreatedByMissionOnly) {
    for (auto i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
        const auto mem = m_members.data()[i];
        if (!mem || i == LEADER_MEM_ID) { // Como el original: el lider se salta por indice
            continue;
        }
        if (bCreatedByMissionOnly && mem->IsCreatedBy(PED_MISSION)) {
            continue;
        }
        RemoveMember(i);
    }
}

// 0x5FB1D0
void CPedGroupMembership::RemoveNFollowers(int32 count) {
    // Como el original: recorre los huecos 0..6 sin cortar el bucle, salta los nulos y compara el contador con signo
    for (auto i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
        const auto mem = m_members.data()[i];
        if (!mem || count <= 0) {
            continue;
        }
        if (mem->IsCreatedBy(PED_MISSION)) {
            continue;
        }
        RemoveMember(i);
        count--;
    }
}

// 0x5F80D0
void CPedGroupMembership::RemoveMember(int32 memIdx) {
    auto& slot = m_members.data()[memIdx];
    const auto mem = slot;
    assert(mem);

    // Como el original: se limpia la referencia pero el hueco se pone a nulo al final
    // (si se quita el lider, GetLeader() todavia lo devuelve en la comprobacion de m_pPlayerData)
    if (slot) {
        CEntity::CleanUpOldReference(slot);
    }

    if (!mem->IsPlayer()) {
        if (mem->bClearRadarBlipOnDeath) {
            // Como el original: el blip se busca por la referencia SCM del pool (indice << 8 | estado)
            CRadar::ClearBlipForEntity(BLIP_CHAR, GetPedPool()->GetRef(mem));
            mem->bClearRadarBlipOnDeath = false;
        }

        mem->GetIntelligence()->RestorePedDecisionMakerType();

        if (const auto leader = GetLeader(); leader && leader->GetPlayerData()) {
            mem->bDrownsInWater = true;
        }
    }

    slot = nullptr;
}

// 0x5FB210
void CPedGroupMembership::RemoveMember(CPed* ped) {
    if (const auto id = GetMemberId(ped); id != -1) {
        RemoveMember(id);
    }
}

// 0x5FB9C0
void CPedGroupMembership::SetLeader(CPed* ped) {
    assert(ped);

    // Remove member (Just in case he's in the group already)
    // Como el original: solo si es seguidor (huecos 0..6); el lider actual se quita por indice
    if (IsFollower(ped)) {
        RemoveMember(ped);
    }

    // Remov the current leader (If any)
    if (GetLeader()) {
        RemoveMember(LEADER_MEM_ID);
    }

    // Now add member as the leader
    AddMember(ped, LEADER_MEM_ID);
    GivePedRandomObjectToHold(ped);
}

// notsa
auto CPedGroupMembership::GetMemberClosestTo(CPed* ped, bool includeLeader) -> FindClosestMemberResult {
    return GetMemberClosestToIf(ped, [](CPed&) { return true; }, includeLeader);
}

// NOTSA
auto CPedGroupMembership::FindClosestFollowerToLeader() -> FindClosestMemberResult {
    if (const auto leader = GetLeader()) {
        return GetMemberClosestTo(leader);
    }
    return { nullptr, 0.f }; // We return 0.f here, `GetMemberClosestTo` returns FLT_MAX, but it should be ignored anyways, because the CPed* is nullptr
}

// NOTSA
CPed* CPedGroupMembership::GetFirstAvailableMember() {
    for (const auto m : m_members) {
        if (m) {
            return m;
        }
    }
    return nullptr;
}

// 0x5F6950
eModelID CPedGroupMembership::GetObjectForPedToHold() {
    using namespace ModelIndices;
    // Como el original: (rand() & 0xFFFF) * (1/32768) * 100 truncado; < 33 cigarro, < 66 bebida, si no -1
    // (los indices de modelo se leen como uint16, sin extension de signo)
    const auto rnd = (int32)((float)(CGeneral::GetRandomNumber() & 0xFFFF) * (1.0f / 32768.0f) * 100.0f);
    if (rnd < 33) {
        return MI_GANG_SMOKE;
    }
    if (rnd < 66) {
        return MI_GANG_DRINK;
    }
    return MODEL_INVALID;
}

// NOTSA
int32 CPedGroupMembership::FindNewLeaderToAppoint() const {
    for (auto&& [i, mem] : rngv::enumerate(m_members)) {
        if (mem) {
            return i;
        }
    }
    return -1;
}

// NOTSA
int32 CPedGroupMembership::FindIdForNewMember() const {
    for (auto&& [i, mem] : rngv::enumerate(m_members)) {
        if (!mem) {
            return i;
        }
    }
    return -1;
}

// Based on code from 0x5F80BE
void CPedGroupMembership::GivePedRandomObjectToHold(CPed* mem, bool onlyIfUnarmed) const {
    if (m_group->m_bIsMissionGroup) {
        return;
    }
    if (onlyIfUnarmed && !mem->IsCurrentlyUnarmed()) {
        return;
    }
    if (const auto modelId = GetObjectForPedToHold(); modelId != MODEL_INVALID) {
        mem->GiveObjectToPedToHold(modelId, true);
    }
}

// NOTSA
bool CPedGroupMembership::CanAddFollower() {
    return std::size(m_members) - 1 >= CountMembers(); // - 1 to compensate for leader
}

// NOTSA
CPed* CPedGroupMembership::GetRandom() {
    return CGeneral::RandomChoice(m_members | rng::views::take(CountMembers()));
}

void CPedGroupMembership::InjectHooks() {
    RH_ScopedClass(CPedGroupMembership);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetObjectForPedToHold, 0x5F6950);
    RH_ScopedInstall(From, 0x5F7FE0);
    RH_ScopedInstall(CountMembersExcludingLeader, 0x5F6AA0);
    RH_ScopedInstall(CountMembers, 0x5F6A50);
    RH_ScopedInstall(IsMember, 0x5F6A10);
    RH_ScopedInstall(IsFollower, 0x5F69E0);
    RH_ScopedInstall(IsLeader, 0x5F69C0);
    RH_ScopedInstall(GetMember, 0x5F69B0);
    RH_ScopedInstall(AddMember, 0x5F6AE0);
    RH_ScopedInstall(AddFollower, 0x5F8020);
    RH_ScopedInstall(GetLeader, 0x5F69A0);
    RH_ScopedInstall(SetLeader, 0x5FB9C0);
    RH_ScopedInstall(AppointNewLeader, 0x5FB240);
    RH_ScopedInstall(RemoveNFollowers, 0x5FB1D0);
    RH_ScopedInstall(RemoveAllFollowers, 0x5FB190);
    RH_ScopedOverloadedInstall(RemoveMember, "ByPed", 0x5FB210, void(CPedGroupMembership::*)(CPed*));
    RH_ScopedOverloadedInstall(RemoveMember, "ByMemIdx", 0x5F80D0, void(CPedGroupMembership::*)(int32));
    RH_ScopedInstall(Process, 0x5FBA60);
}
