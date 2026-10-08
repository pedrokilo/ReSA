/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedGroupPlacer
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "PedPlacement.h"
#include "Tasks/TaskTypes/TaskComplexBeInGroup.h"
#include "Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.h"
#include "Tasks/TaskTypes/TaskComplexWanderGang.h"
#include "Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h"

void CPedGroupPlacer::InjectHooks() {
    RH_ScopedClass(CPedGroupPlacer);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(PlaceFormationGroup, 0x5FC9B0);
    RH_ScopedInstall(PlaceChatGroup, 0x5FCE80);
    RH_ScopedInstall(PlaceRandomGroup, 0x5FD330);
    RH_ScopedInstall(PlaceGroup, 0x5FD810);
}

namespace {
// Como el original: comprobar visibilidad y cercania antes de crear cualquier peaton.
bool GroupOriginCanBeUsed(const CVector& origin, float radius) {
    if (TheCamera.IsSphereVisible(origin, radius)) {
        const auto& player = FindPlayerPed()->GetPosition();
        const float y = origin.y - player.y, x = origin.x - player.x;
        const float distance = std::sqrt(x * x + y * y);
        if (CPopulation::PedCreationDistMultiplier() * 42.5f > distance) return false;
    }
    return CPedPlacement::IsPositionClearForPed(origin, radius, -1, nullptr, true, true, true);
}
void InstallGroupPed(CPedGroup& group, CPed* ped, int32 groupId, bool leader, bool setLeader) {
    if (setLeader) group.GetMembership().SetLeader(ped);
    else group.GetMembership().AddFollower(ped);
    group.GetMembership().Process();
    group.GetIntelligence().Process();
    ped->GetTaskManager().SetTask(new CTaskComplexBeInGroup{groupId, leader}, TASK_PRIMARY_PRIMARY, false);
}
bool PlaceCircularGroup(ePedType type, uint32 numPeds, const CVector& origin, bool random) {
    if (static_cast<int32>(numPeds) < 2) return false;
    const int32 groupId = CPedGroups::AddGroup();
    if (groupId < 0) return false;
    const float twoPi = std::bit_cast<float>(0x40C90FDBu), half = 0.5f, one = 1.0f;
    const int32 signedNumPeds = static_cast<int32>(numPeds);
    float angleStep, radius;
    __asm {
        fild signedNumPeds
        fdivr twoPi
        fst angleStep
        fcos
        fsubr one
        fdivr half
        fsqrt
        fstp radius
    }
    if (!GroupOriginCanBeUsed(origin, radius)) return false;
    std::array<CPed*, 8> created;
    int32 count = 0;
    CVector first;
    for (int32 i = 0; i < static_cast<int32>(numPeds); ++i) {
        const float randomAngle = ((float(CGeneral::GetRandomNumber()) * std::bit_cast<float>(0x38000100u)) * 0.4f - 0.2f) * angleStep;
        const float randomRadius = ((float(CGeneral::GetRandomNumber()) * std::bit_cast<float>(0x38000100u)) * 0.4f - 0.2f) * radius + radius;
        const float angle = static_cast<float>(i) * angleStep + randomAngle;
        float offsetX, offsetY;
        __asm {
            fld angle
            fld st(0)
            fcos
            fmul randomRadius
            fstp offsetX
            fsin
            fmul randomRadius
            fstp offsetY
        }
        bool found;
        const float ground = CWorld::FindGroundZFor3DCoord({offsetX + origin.x, offsetY + origin.y, origin.z + 1.0f}, &found, nullptr) + 1.0f;
        if (!found) continue;
        const CVector position{offsetX + origin.x, offsetY + origin.y, ground < origin.z ? origin.z : ground};
        if (i == 0) first = position;
        const auto model = CGangs::ChooseGangPedModel(static_cast<eGangID>(static_cast<int32>(type) - 7));
        const auto* info = CModelInfo::GetModelInfo(model);
        if (!*reinterpret_cast<void* const*>(reinterpret_cast<const uint8*>(info) + 0x1C)) continue;
        std::array<CEntity*, 9> hits{};
        CPedPlacement::IsPositionClearForPed(position, info->GetColModel()->m_boundSphere.m_fRadius, 9, hits.data(), true, true, true);
        bool foreign = false;
        for (auto* hit : hits) {
            if (!hit) continue;
            bool own = false;
            for (int32 n = 0; n < count; ++n) own |= created[n] == hit;
            if (!own) { foreign = true; break; }
        }
        const bool lineClear = i == 0 || CWorld::GetIsLineOfSightClear(position, first, true, false, false, false, false, false, false);
        const bool heightOK = i == 0 || std::abs(position.z - first.z) < 1.0f;
        if (foreign || !lineClear || !heightOK) continue;
        auto* ped = CPopulation::AddPed(type, model, position, false);
        if (!ped) { CPopulation::RemovePed(nullptr); continue; }
        created[count++] = ped;
        ped->m_fCurrentRotation = CGeneral::GetRadianAngleBetweenPoints(origin.x, origin.y, position.x, position.y);
        ped->m_fAimingRotation = ped->m_fCurrentRotation;
        CVisibilityPlugins::SetClumpAlpha(ped->GetRpClump(), 0);
    }
    if (count <= 0) return false;
    auto& group = CPedGroups::GetGroup(groupId);
    group.GetIntelligence().SetDefaultTaskAllocator(CPedGroupDefaultTaskAllocators::Get(random ? ePedGroupDefaultTaskAllocatorType::RANDOM : ePedGroupDefaultTaskAllocatorType::STAND_STILL));
    for (int32 i = 0; i < count; ++i) {
        auto* ped = created[i];
        if (random) {
            if (i == 0) group.GetMembership().SetLeader(ped);
            else group.GetMembership().AddFollower(ped);
            group.GetMembership().Process();
            group.GetIntelligence().Process();
            auto* memory = CTask::operator new(sizeof(CTaskComplexWanderGang));
            CTaskComplexWanderGang* wander = nullptr;
            if (memory) {
                const auto direction = static_cast<uint8>(static_cast<int32>((float(CGeneral::GetRandomNumber()) * std::bit_cast<float>(0x38000000u)) * 8.0f));
                wander = ::new(memory) CTaskComplexWanderGang{static_cast<eMoveState>(4), direction, 5000, true, 0.5f};
            }
            ped->GetTaskManager().SetTask(wander, TASK_PRIMARY_DEFAULT, false);
            ped->GetTaskManager().SetTask(new CTaskComplexBeInGroup{groupId, false}, TASK_PRIMARY_PRIMARY, false);
        } else {
            InstallGroupPed(group, ped, groupId, i == 0, i == 0);
        }
    }
    return true;
}
}

// 0x5FC9B0
bool CPedGroupPlacer::PlaceFormationGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType unused) {
    const int32 groupId = CPedGroups::AddGroup();
    if (groupId < 0 || !GroupOriginCanBeUsed(origin, 3.0f)) return false;
    bool found;
    const float ground = CWorld::FindGroundZFor3DCoord({origin.x, origin.y, origin.z + 1.0f}, &found, nullptr) + 1.0f;
    if (!found) return false;
    const float height = origin.z <= ground ? ground : origin.z;
    const auto leaderModel = CGangs::ChooseGangPedModel(static_cast<eGangID>(static_cast<int32>(type) - 7));
    if (!*reinterpret_cast<void* const*>(reinterpret_cast<const uint8*>(CModelInfo::GetModelInfo(leaderModel)) + 0x1C)) return false;
    auto* leader = CPopulation::AddPed(type, leaderModel, {origin.x, origin.y, height}, false);
    if (!leader) return false;
    std::array<CPed*, 8> created;
    created[0] = leader;
    int32 count = 1;
    for (int32 i = 1; i < static_cast<int32>(numOfPeds); ++i) {
        const auto model = CGangs::ChooseGangPedModel(static_cast<eGangID>(static_cast<int32>(type) - 7));
        if (!*reinterpret_cast<void* const*>(reinterpret_cast<const uint8*>(CModelInfo::GetModelInfo(model)) + 0x1C)) continue;
        auto* ped = CPopulation::AddPed(type, model, {origin.x, origin.y, height}, false);
        if (!ped) continue;
        const auto offset = CTaskComplexFollowLeaderInFormation::ms_offsets.Offsets.data()[i];
        ped->SetPosn(leader->GetPosition() + CVector{offset.x, offset.y, 0.0f});
        const auto initial = ped->GetPosition();
        const float pedGround = CWorld::FindGroundZFor3DCoord({initial.x, initial.y, initial.z + 1.0f}, &found, nullptr) + 1.0f;
        const float pedHeight = initial.z <= pedGround ? pedGround : initial.z;
        if (!found || std::abs(pedHeight - leader->GetPosition().z) > 1.0f || !CWorld::GetIsLineOfSightClear({initial.x, initial.y, pedHeight}, leader->GetPosition(), true, false, false, false, false, false, false)) {
            CPopulation::RemovePed(ped);
            continue;
        }
        ped->SetPosn(initial.x, initial.y, pedHeight);
        created[count++] = ped;
        CVisibilityPlugins::SetClumpAlpha(ped->GetRpClump(), 0);
    }
    auto& group = CPedGroups::GetGroup(groupId);
    group.GetIntelligence().SetDefaultTaskAllocator(CPedGroupDefaultTaskAllocators::Get(ePedGroupDefaultTaskAllocatorType::FOLLOW_ANY_MEANS));
    for (int32 i = 0; i < count; ++i) InstallGroupPed(group, created[i], groupId, i == 0, i == 0);
    return true;
}

// 0x5FCE80
bool CPedGroupPlacer::PlaceChatGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType unused) {
    return PlaceCircularGroup(type, numOfPeds, origin, false);
}

// 0x5FD330
bool CPedGroupPlacer::PlaceRandomGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType unused) {
    return PlaceCircularGroup(type, numOfPeds, origin, true);
}

// 0x5FD810
bool CPedGroupPlacer::PlaceGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType allocType) {
    using enum ePedGroupDefaultTaskAllocatorType;
    switch (allocType) {
    case FOLLOW_ANY_MEANS:
    case FOLLOW_LIMITED:   return PlaceFormationGroup(type, numOfPeds, origin, allocType);
    case STAND_STILL:
    case CHAT:             return PlaceChatGroup(type, numOfPeds, origin, allocType);
    case RANDOM:           return PlaceRandomGroup(type, numOfPeds, origin, allocType);
    default:               NOTSA_UNREACHABLE();
    }
}
