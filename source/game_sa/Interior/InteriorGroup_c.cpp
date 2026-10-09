/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: InteriorGroup_c
 * Reconstrucción: COMPLETA — 24/24 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 22.
 * Validación registrada: 24/24 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   AddInterior | 0x00594840 | S
 *   ArePathsLoaded | 0x00595380 | S
 *   CalcIsVisible | 0x00595200 | S
 *   ContainsInteriorType | 0x00595250 | S
 *   Exit | 0x00595290 | S
 *   FindClosestInteriorInfo | 0x00594A50 | S
 *   FindInteriorInfo | 0x00594970 | S
 *   GetEntity | 0x00594BD0 | S
 *   GetNumInteriorInfos | 0x00594920 | S
 *   GetPed | 0x00594B90 | S
 *   GetRandomInterior | 0x005948C0 | S
 *   Init | 0x005947E0 | S
 *   RemovePed | 0x00594E30 | S
 *   Setup | 0x00595320 | S
 *   SetupHousePeds | 0x005965E0 | S
 *   SetupOfficePeds | 0x00594BF0 | S
 *   SetupPaths | 0x00595590 | S
 *   SetupPeds | 0x00596890 | S
 *   SetupShopPeds | 0x00594C10 | S
 *   Update | 0x005968E0 | S
 *   UpdateOfficePeds | 0x00594E90 | S
 *   UpdatePeds | 0x00596830 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "InteriorGroup_c.h"
#include "Interior_c.h"
#include "PathFind.h"
#include "InteriorManager_c.h"
#include "Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.h"
#include "Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.h"
#include "Tasks/TaskTypes/Interior/TaskInteriorBeInShop.h"
#include "Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h"

void InteriorGroup_c::InjectHooks() {
    RH_ScopedClass(InteriorGroup_c);
    RH_ScopedCategory("Interior");

    //RH_ScopedInstall(Constructor, 0x597FE0, { .reversed = false });
    //RH_ScopedInstall(Destructor, 0x597FF0, { .reversed = false });

    RH_ScopedInstall(Init, 0x5947E0);
    RH_ScopedInstall(Update, 0x5968E0);
    RH_ScopedInstall(SetupPeds, 0x596890);
    RH_ScopedInstall(UpdatePeds, 0x596830);
    RH_ScopedInstall(SetupHousePeds, 0x5965E0);
    RH_ScopedInstall(SetupPaths, 0x595590);
    RH_ScopedInstall(ArePathsLoaded, 0x595380);
    RH_ScopedInstall(Setup, 0x595320);
    RH_ScopedInstall(Exit, 0x595290);
    RH_ScopedInstall(ContainsInteriorType, 0x595250);
    RH_ScopedInstall(CalcIsVisible, 0x595200);
    RH_ScopedInstall(DereferenceAnims, 0x595160);
    RH_ScopedInstall(ReferenceAnims, 0x5950D0);
    RH_ScopedInstall(UpdateOfficePeds, 0x594E90);
    RH_ScopedInstall(RemovePed, 0x594E30);
    RH_ScopedInstall(SetupShopPeds, 0x594C10);
    RH_ScopedInstall(SetupOfficePeds, 0x594BF0);
    RH_ScopedInstall(GetEntity, 0x594BD0);
    RH_ScopedInstall(GetPed, 0x594B90);
    RH_ScopedInstall(FindClosestInteriorInfo, 0x594A50);
    RH_ScopedInstall(FindInteriorInfo, 0x594970);
    RH_ScopedInstall(GetNumInteriorInfos, 0x594920);
    RH_ScopedInstall(GetRandomInterior, 0x5948C0);
    RH_ScopedInstall(AddInterior, 0x594840);
}

// 0x5947E0
void InteriorGroup_c::Init(CEntity* entity, int32 id) {
    std::fill(std::begin(m_interiors), std::end(m_interiors), nullptr);
    std::fill(std::begin(m_peds), std::end(m_peds), nullptr);
    std::fill(std::begin(m_pedsToRemove), std::end(m_pedsToRemove), nullptr);
    m_numInteriors = 0;
    m_pathSetupComplete = m_updatePeds = m_animBlockReferenced = 0;
    m_isVisible = m_lastIsVisible = false;
    m_pEntity = entity;
    m_groupId = uint8(id);
}

// 0x5968E0
void InteriorGroup_c::Update() {
    CalcIsVisible();
    if (!m_pathSetupComplete) SetupPaths();
    if (m_pathSetupComplete && !m_updatePeds) SetupPeds();
    if (m_updatePeds) UpdatePeds();
    ReferenceAnims();
}

// 0x594840
int32 InteriorGroup_c::AddInterior(Interior_c* interior) {
    for (int32 i = 0; i < 8; ++i) {
        if (!m_interiors[i]) {
            m_interiors[i] = interior;
            ++m_numInteriors;
            return i;
        }
    }
    return 8;
}

// 0x596890
void InteriorGroup_c::SetupPeds() {
    if (m_EnEx && StaticRef<bool>(0xBB3DC2)) {
        switch (m_groupType) {
        case 0: SetupHousePeds(); break;
        case 1: SetupShopPeds(); break;
        case 2: SetupOfficePeds(); break;
        }
        m_updatePeds = 1;
    }
}

// 0x596830
void InteriorGroup_c::UpdatePeds() {
    if (m_EnEx && StaticRef<bool>(0xBB3DC2)) {
        for (auto& ped : m_pedsToRemove) {
            if (ped) {
                if (ped->IsPointerValid()) RemovePed(ped);
                ped = nullptr;
            }
        }
        if (m_groupType == 2) UpdateOfficePeds();
    }
}

// 0x5965E0
void InteriorGroup_c::SetupHousePeds() {
    CStreaming::StreamPedsForInterior(0);
    CStreaming::LoadAllRequestedModels(false);
    m_numPeds = 0;
    const CVector position=m_pEntity->GetMatrix().TransformPoint(CVector{0,0,0});
    int32 count = int32(float(CGeneral::GetRandomNumber())*(1.0f/32768.0f)*100.0f)<=50?2:1;
    bool gang=false;
    ePedType gangType{};
    if(CPopCycle::m_pCurrZoneInfo){
        gangType=CPopCycle::PickGangToCreateMembersOf();
        if(int32(gangType)!=0&&int32(CPopulation::ChooseGangOccupation(eGangID(int32(gangType)-7)))>-1){
            gang=true;
            if(int32(float(CGeneral::GetRandomNumber())*(1.0f/32768.0f)*100.0f)<=15)count=CGeneral::GetRandomNumber()>=0x3FFF?4:3;
            else gang=false;
        }
    }
    for(int32 i=0;i<count;++i){
        const int32 model=gang?int32(CPopulation::ChooseGangOccupation(eGangID(int32(gangType)-7))):CStreaming::FindMIPedSlotForInterior(count==1?(CGeneral::GetRandomNumber()>=0x3FFF?1:0):i);
        const int32 slot=m_numPeds;
        auto* ped=CPopulation::AddPed(CModelInfo::GetPedModelInfo(model)->m_nPedType,eModelID(model),position,false);
        m_peds[slot]=ped;
        if(ped){
            ++m_numPeds;
            plugin::CallMethod<0x46CED0, CScriptsForBrains*, const char*, CEntity*, int8>(&CTheScripts::ScriptsForBrains,"house",ped,3);
            ped->SetCharCreatedBy(ePedCreatedBy(2));
            ped->GetIntelligence()->SetPedDecisionMakerType(7);
            auto* memory=CTask::operator new(sizeof(CTaskInteriorBeInHouse));
            auto* task=memory?::new(memory) CTaskInteriorBeInHouse(this):nullptr;
            ped->GetTaskManager().SetTask(task,TASK_PRIMARY_DEFAULT,false);
        }
        if(!StaticRef<uint8>(0xBB3D9C+i))RemovePed(m_peds[slot]);
    }
}

// 0x595590
void InteriorGroup_c::SetupPaths() {
    if (!ArePathsLoaded()) return;
    ThePaths.StartNewInterior(int8(m_id));
    int32 offsets[8]{}, total = 0, route = 0;
    const auto rawVector = [](const uint8* data, size_t offset) { return *reinterpret_cast<const CVector*>(data + offset); };
    const auto add = [](const CVector& position, bool dontWander) { return ThePaths.AddNodeToNewInterior(position.x, position.y, position.z, dontWander, -1, -1, -1, -1, -1, -1); };
    for (int32 i = 0; i < 8; ++i) {
        if (auto* interior = m_interiors[i]) {
            const auto* data = reinterpret_cast<const uint8*>(interior);
            offsets[i] = total;
            const int32 count = *reinterpret_cast<const int8*>(data + 0x40C);
            if (count > 2) {
                add(rawVector(data, 0x414), false); add(rawVector(data, 0x424), false);
                ThePaths.AddInteriorLink(route + 1, total); route += 2;
                for (int32 j = 2; j < count; ++j) {
                    add(rawVector(data, 0x414 + j * 16), false);
                    ThePaths.AddInteriorLink(route, total + j - 2);
                    if (j & 1) ThePaths.AddInteriorLink(route, total + j - 1);
                    ++route;
                }
            }
            total += count;
        }
    }
    CVector unmatchedPositions[16]; int32 unmatchedNodes[16]; bool matched[16]{}; int32 unmatchedCount = 0;
    constexpr size_t enableOffsets[]{5, 6, 10, 8};
    for (int32 i = 0; i < 8; ++i) {
        auto* interior = m_interiors[i]; if (!interior) continue;
        auto* data = reinterpret_cast<uint8*>(interior);
        const auto* effect = *reinterpret_cast<const uint8* const*>(data + 0x14);
        bool first = true; bool enabled[4]{}; int32 insideNodes[4]{};
        for (int32 portal = 0; portal < 4; ++portal) {
            if (*reinterpret_cast<const int8*>(effect + enableOffsets[portal]) < 0) continue;
            enabled[portal] = true; insideNodes[portal] = total;
            const size_t base = 0x510 + portal * 0x20;
            const auto inside = rawVector(data, base + 4), outside = rawVector(data, base + 0x14);
            const auto insideAddress = add(inside, false);
            for (size_t c = 2; c < 4; ++c) {
                const int32 connection = *reinterpret_cast<const int8*>(data + base + c);
                if (portal == 0 ? connection >= 0 : connection != -1) ThePaths.AddInteriorLink(total, offsets[i] + connection);
            }
            if (first) {
                *reinterpret_cast<CVector*>(data + 0x400) = inside;
                *reinterpret_cast<CNodeAddress*>(data + 0x3F0) = insideAddress;
                first = false;
            }
            add(outside, portal == 0 && m_groupType != 0);
            ThePaths.AddInteriorLink(total + 1, total);
            const auto exterior = ThePaths.FindNearestExteriorNodeToInteriorNode(total + 1);
            if (exterior.m_wAreaId == uint16(-1)) {
                unmatchedPositions[unmatchedCount] = outside;
                unmatchedNodes[unmatchedCount++] = total + 1;
            } else {
                *reinterpret_cast<CNodeAddress*>(data + 0x3EC) = exterior;
                const auto* node = reinterpret_cast<const uint8*>(StaticRef<uint8*>(0x96F854 + exterior.m_wAreaId * 4)) + exterior.m_wNodeId * 0x1C;
                const CVector position{float(*reinterpret_cast<const int16*>(node + 8)) * .125f, float(*reinterpret_cast<const int16*>(node + 10)) * .125f, float(*reinterpret_cast<const int16*>(node + 12)) * .125f};
                *reinterpret_cast<CVector*>(data + 0x3F4) = position;
                if (!m_groupType) {
                    const auto delta = position - outside;
                    const float square = portal == 0 ? (delta.x * delta.x + delta.y * delta.y) + delta.z * delta.z : (delta.y * delta.y + delta.z * delta.z) + delta.x * delta.x;
                    if (square <= 9.0f) ThePaths.AddInteriorLinkToExternalNode(total + 1, exterior);
                }
            }
            total += 2;
        }
        if (*reinterpret_cast<const int8*>(data + 0x40C) == 0) {
            if (enabled[0]) for (int32 j = 1; j < 4; ++j) if (enabled[j]) ThePaths.AddInteriorLink(insideNodes[0], insideNodes[j]);
            if (enabled[2]) { if (enabled[1]) ThePaths.AddInteriorLink(insideNodes[2], insideNodes[1]); if (enabled[3]) ThePaths.AddInteriorLink(insideNodes[2], insideNodes[3]); }
        }
    }
    for (int32 i = 0; i < unmatchedCount; ++i) {
        if (matched[i]) continue;
        float best = 1e9f; int32 closest = -1;
        for (int32 j = i + 1; j < unmatchedCount; ++j) if (!matched[j]) {
            const auto delta = unmatchedPositions[i] - unmatchedPositions[j];
            const float square = (delta.x * delta.x + delta.y * delta.y) + delta.z * delta.z;
            if (square < best) { best = square; closest = j; }
        }
        if (best >= 3.0f) plugin::CallMethod<0x44DF60>(&ThePaths, unmatchedNodes[i]);
        else { matched[i] = matched[closest] = true; ThePaths.AddInteriorLink(unmatchedNodes[i], unmatchedNodes[closest]); }
    }
    plugin::CallMethod<0x452270>(&ThePaths, static_cast<CNodeAddress*>(nullptr));
    m_pathSetupComplete = 1;
}

// 0x595380
int8 InteriorGroup_c::ArePathsLoaded() {
    const auto& box = m_pEntity->GetColModel()->m_boundBox;
    const auto& a = box.m_vecMin;
    const auto& b = box.m_vecMax;
    CVector corners[]{a, {a.x,b.y,a.z}, {b.x,b.y,a.z}, {b.x,a.y,a.z}, {a.x,a.y,b.z}, {a.x,b.y,b.z}, b, {b.x,a.y,b.z}};
    CVector minimum{999999.0f,999999.0f,999999.0f};
    CVector maximum{-999999.0f,-999999.0f,-999999.0f};
    for (auto& corner : corners) {
        corner = m_pEntity->GetMatrix().TransformPoint(corner);
        if (corner.x < minimum.x) minimum.x = corner.x;
        if (corner.y < minimum.y) minimum.y = corner.y;
        if (corner.z < minimum.z) minimum.z = corner.z;
        if (corner.x > maximum.x) maximum.x = corner.x;
        if (corner.y > maximum.y) maximum.y = corner.y;
        if (corner.z > maximum.z) maximum.z = corner.z;
    }
    return ThePaths.AreNodesLoadedForArea(minimum.x,maximum.x,minimum.y,maximum.y);
}

// 0x595320
void InteriorGroup_c::Setup() {
    if (ContainsInteriorType(2)) m_groupType = 0;
    else if (ContainsInteriorType(0) || ContainsInteriorType(6)) m_groupType = 1;
    else m_groupType = ContainsInteriorType(1) ? 2 : uint8(-1);
    ReferenceAnims();
}

// 0x595290
void InteriorGroup_c::Exit() {
    for (auto* interior : m_interiors) {
        if (!interior) continue;
        interior->Exit();
        g_interiorMan.ReturnInteriorToPool(interior);
    }
    for (size_t i = 0; i < 16; ++i) {
        auto*& ped = m_peds[i];
        if (!ped) continue;
        if (ped->IsPointerValid()) {
            if (!ped->IsAlive()) StaticRef<uint8>(0xBB3D9C + i) = 0;
            CPopulation::RemovePed(ped);
        } else {
            StaticRef<uint8>(0xBB3D9C + i) = 0;
        }
        ped = nullptr;
    }
    m_numPeds = 0;
    ThePaths.RemoveInterior(uint32(int32(int8(m_id))));
    DereferenceAnims();
}

// 0x595250
int8 InteriorGroup_c::ContainsInteriorType(int32 a2) {
    for (auto* interior : m_interiors) {
        if (interior && *reinterpret_cast<const uint8*>(interior->m_box) == a2) {
            return 1;
        }
    }
    return 0;
}

// 0x595200
void InteriorGroup_c::CalcIsVisible() {
    m_lastIsVisible = m_isVisible;
    m_isVisible = false;
    for (int32 i = 0; i < int8(m_numInteriors); ++i) {
        if (m_interiors[i]->IsVisible()) {
            m_isVisible = true;
            break;
        }
    }
}

// 0x595160
void InteriorGroup_c::DereferenceAnims() {
    if (!m_animBlockReferenced) {
        return;
    }
    CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(GetAnimBlockName()));
    m_animBlockReferenced = false;
}

// 0x5950D0
void InteriorGroup_c::ReferenceAnims() {
    if (m_animBlockReferenced) {
        return;
    }
    const auto animBlkIdx = CAnimManager::GetAnimationBlockIndex(GetAnimBlockName());
    if (*reinterpret_cast<const uint8*>(&CAnimManager::GetAnimBlocks().data()[animBlkIdx].IsLoaded) == 1) {
        CAnimManager::AddAnimBlockRef(animBlkIdx);
        m_animBlockReferenced = true;
    } else {
        CStreaming::RequestModel(IFPToModelId(animBlkIdx), STREAMING_KEEP_IN_MEMORY);
    }
}

// 0x594E90
void InteriorGroup_c::UpdateOfficePeds() {
    if (!m_isVisible) {
        if (m_lastIsVisible) {
            for (auto*& ped : m_peds) {
                if (!ped) continue;
                if (ped->IsPointerValid()) CPopulation::RemovePed(ped);
                ped = nullptr;
            }
            m_numPeds = 0;
        }
        return;
    }
    if (m_lastIsVisible) return;
    const CVector position = m_pEntity->GetMatrix().TransformPoint(CVector{0.0f,0.0f,0.0f});
    const int32 available = GetNumInteriorInfos(6);
    int32 count = 0;
    if (CClock::GetIsTimeInRange(9,18)) {
        count = CGeneral::GetRandomIntInRange(available/2,available);
    } else if (CClock::GetIsTimeInRange(18,22) || CClock::GetIsTimeInRange(6,9)) {
        count = CGeneral::GetRandomIntInRange(0,available/2);
    }
    if (count > 15) count = 16;
    while (count-- > 0) {
        const int32 selector = int32(float(CGeneral::GetRandomNumber()) * (1.0f/32768.0f) * 8.0f);
        const auto model = CStreaming::FindMIPedSlotForInterior(selector);
        const int32 slot = m_numPeds;
        auto* ped = CPopulation::AddPed(CModelInfo::GetPedModelInfo(model)->m_nPedType,eModelID(model),position,false);
        m_peds[slot] = ped;
        if (!ped) continue;
        ++m_numPeds;
        ped->SetCharCreatedBy(ePedCreatedBy(2));
        ped->GetIntelligence()->SetPedDecisionMakerType(7);
        auto* memory = CTask::operator new(sizeof(CTaskInteriorBeInOffice));
        auto* task = memory ? ::new(memory) CTaskInteriorBeInOffice(this) : nullptr;
        ped->GetTaskManager().SetTask(task,TASK_PRIMARY_DEFAULT,false);
    }
}

// 0x594E30
void InteriorGroup_c::RemovePed(CPed* a2) {
    for (auto& ped : m_peds) {
        if (ped && ped == a2) {
            CPopulation::RemovePed(ped);
            ped = nullptr;
            --m_numPeds;
            break;
        }
    }
}

// 0x594C10
void InteriorGroup_c::SetupShopPeds() {
    CStreaming::StreamPedsForInterior(1);
    CStreaming::LoadAllRequestedModels(false);
    m_numPeds=0;
    const int32 count=(2-int32(float(CGeneral::GetRandomNumber())*(1.0f/32768.0f)*-3.0f))*int8(m_numInteriors)+1;
    for(int32 i=0;i<count;++i){
        const int32 selector=i?1-int32(float(CGeneral::GetRandomNumber())*(1.0f/32768.0f)*-7.0f):0;
        const auto model=CStreaming::FindMIPedSlotForInterior(selector);
        const auto type=CModelInfo::GetPedModelInfo(model)->m_nPedType;
        CVector position{0,0,0};
        auto* interior=reinterpret_cast<Interior_c*>(GetRandomInterior());
        int32 tileX,tileY;
        interior->GetRandomTile(3,&tileX,&tileY);
        interior->GetTileCentre(float(tileX),float(tileY),&position);
        const int32 slot=m_numPeds;
        position.z+=1.0f;
        auto* ped=CPopulation::AddPed(type,eModelID(model),position,false);
        m_peds[slot]=ped;
        if(!ped)continue;
        ++m_numPeds;
        ped->SetCharCreatedBy(ePedCreatedBy(2));
        ped->GetIntelligence()->SetPedDecisionMakerType(7);
        CTask* task=nullptr;
        if(i==0){auto* memory=CTask::operator new(sizeof(CTaskInteriorShopKeeper));if(memory)task=::new(memory) CTaskInteriorShopKeeper(this,false);}
        else {auto* memory=CTask::operator new(sizeof(CTaskInteriorBeInShop));if(memory)task=::new(memory) CTaskInteriorBeInShop(this);}
        ped->GetTaskManager().SetTask(task,TASK_PRIMARY_DEFAULT,false);
    }
}

// 0x594BF0
void InteriorGroup_c::SetupOfficePeds() {
    CStreaming::StreamPedsForInterior(2);
    CStreaming::LoadAllRequestedModels(false);
    m_numPeds = 0;
}

// 0x594BD0
CEntity* InteriorGroup_c::GetEntity() {
    return m_pEntity;
}

// 0x594B90
CPed* InteriorGroup_c::GetPed(int32 idx) {
    return m_peds[idx];
}

// 0x594A50
bool InteriorGroup_c::FindClosestInteriorInfo(int32 type, CVector point, float radius, InteriorInfo_t** outInfo, Interior_c** outInterior, float* outDistance) {
    const auto radiusSquared = radius * radius;
    float bestDistance = 999999.0f;
    InteriorInfo_t* bestInfo = nullptr;
    Interior_c* bestInterior = nullptr;
    for (auto* interior : m_interiors) {
        if (!interior || !interior->IsPtInside(point, {})) continue;
        for (int32 i = 0; i < interior->m_interiorInfosCount; ++i) {
            auto* info = &interior->m_interiorInfos[i];
            if ((type != -1 && *reinterpret_cast<const int8*>(&info->Type) != type) || info->IsInUse) continue;
            const auto x = point.x - info->Pos.x;
            const auto y = point.y - info->Pos.y;
            const auto z = point.z - info->Pos.z;
            const auto distance = (z * z + y * y) + x * x;
            if (distance < radiusSquared && distance < bestDistance) {
                bestDistance = distance;
                bestInfo = info;
                bestInterior = interior;
            }
        }
    }
    if (!bestInfo) return false;
    *outInfo = bestInfo;
    *outInterior = bestInterior;
    *outDistance = bestDistance;
    return true;
}

// 0x594970
bool InteriorGroup_c::FindInteriorInfo(eInteriorInfoType infoType, InteriorInfo_t** a3, Interior_c** a4) {
    InteriorInfo_t* infos[64];
    Interior_c* interiors[64];
    int32 count = 0;
    for (auto* interior : m_interiors) {
        if (!interior) continue;
        for (int32 i = 0; i < interior->m_interiorInfosCount; ++i) {
            auto* info = &interior->m_interiorInfos[i];
            if (*reinterpret_cast<const int8*>(&info->Type) == int32(infoType) && !info->IsInUse) {
                infos[count] = info;
                interiors[count++] = interior;
            }
        }
    }
    if (count <= 0) {
        *a3 = nullptr;
        *a4 = nullptr;
        return false;
    }
    const float value = float(CGeneral::GetRandomNumber() & 0xFFFF) * (1.0f / 32768.0f);
    const auto index = int32(value * count);
    *a3 = infos[index];
    *a4 = interiors[index];
    return true;
}

// 0x594920
int32 InteriorGroup_c::GetNumInteriorInfos(int32 a2) {
    int32 count = 0;
    for (auto* interior : m_interiors) {
        if (!interior) continue;
        for (int32 i = 0; i < interior->m_interiorInfosCount; ++i) {
            if (*reinterpret_cast<const int8*>(&interior->m_interiorInfos[i].Type) == a2) ++count;
        }
    }
    return count;
}

// 0x5948C0
int32 InteriorGroup_c::GetRandomInterior() {
    const float value = float(CGeneral::GetRandomNumber() & 0xFFFF) * (1.0f / 32768.0f);
    const int32 selected = int32(value * int8(m_numInteriors));
    int32 count = 0;
    for (auto* interior : m_interiors) {
        if (interior && count++ == selected) return int32(uintptr_t(interior));
    }
    return 0;
}

//! @notsa
const char* InteriorGroup_c::GetAnimBlockName() {
    switch ((eInteriorGroupType)m_groupType) {
    case eInteriorGroupType::HOUSE:  return "int_house";
    case eInteriorGroupType::SHOP:   return "int_shop";
    case eInteriorGroupType::OFFICE: return "int_office";
    default:                         NOTSA_UNREACHABLE();
    }
}
