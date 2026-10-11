/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexSunbathe
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
#include "TaskComplexSunbathe.h"
#include "TaskSimplePause.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleRunTimedAnim.h"
#include "InterestingEvents.h"

// 0x632140: whole EAX result, readonly hour/weather, no original GAME delegation.
int32 CanSunbathe() {
    const uint8 hour = *reinterpret_cast<volatile const uint8*>(0xB70153);
    if (hour < 10 || hour >= 18) {
        return 0;
    }
    const int16 weather = *reinterpret_cast<volatile const int16*>(0xC8131C);
    if (weather == 1 || weather == 3 || weather == 14 || weather == 5 ||
        weather == 10 || weather == 18 || weather == 0 || weather == 2 ||
        weather == 13 || weather == 6 || weather == 11 || weather == 17) {
        return 1;
    }
    return 0;
}


namespace {
bool SunbatheWeatherAllows() {
    const auto hour = CClock::GetGameClockHours();
    if (hour < 10 || hour >= 18) return false;
    // This task reads the current executable weather, at C8131C.
    switch (*reinterpret_cast<const int16*>(0xC8131C)) {
    case 1: case 3: case 14: case 5: case 10: case 18:
    case 0: case 2: case 13: case 6: case 11: case 17: return true;
    default: return false;
    }
}
bool SunbatheShouldLoad() {
    const auto* player = CWorld::Players[CWorld::PlayerInFocus].m_pPed;
    if (player->bInVehicle && player->m_pVehicle) {
        const auto& v = player->m_pVehicle->m_vecMoveSpeed;
        if ((v.x*v.x + v.y*v.y) + v.z*v.z > std::bit_cast<float>(uint32{0x3D23D70B})) return false;
    }
    return true;
}
}

void CTaskComplexSunbathe::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSunbathe, 0x86e0ac, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x631F80);
    RH_ScopedInstall(Destructor, 0x632050);

    {
        RH_ScopedCategory("Tasks/TaskTypes");
        RH_ScopedGlobalInstall(CanSunbathe, 0x632140);
    }
    //RH_ScopedGlobalInstall(ShouldLoadSunbatheAnims, 0x632190, { .reversed = false });

    //RH_ScopedInstall(CreateSubTask, 0x638290, { .reversed = false });

    RH_ScopedVMTInstall(Clone, 0x6366A0);
    RH_ScopedVMTInstall(GetTaskType, 0x632040);
    RH_ScopedVMTInstall(MakeAbortable, 0x6320F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6399F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x639CB0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6381A0);

}

// 0x631f80
CTaskComplexSunbathe::CTaskComplexSunbathe(CObject* towel, bool bStartStanding) :
    m_pTowel{towel},
    m_bStartStanding{bStartStanding}
{
    if (m_pTowel) {
        CEntity::SafeRegisterRef(m_pTowel);
        m_pTowel->m_nObjectType = OBJECT_TYPE_DECORATION;
    }

    const auto LoadAnim = [](int32& idx, CAnimBlock*& blk, const char* name) {
        blk = CAnimManager::GetAnimationBlock(name);
        idx = CAnimManager::GetAnimationBlockIndex(name);
    };
    LoadAnim(m_BeachAnimBlockIndex, m_pBeachAnimBlock, "beach");
    LoadAnim(m_SunbatheAnimBlockIndex, m_pSunbatheAnimBlock, "sunbathe");

    // Refs added later in another function
}

CTaskComplexSunbathe::CTaskComplexSunbathe(const CTaskComplexSunbathe& o) :
    CTaskComplexSunbathe{ o.m_pTowel, o.m_bStartStanding }
{
}

// 0x632050
CTaskComplexSunbathe::~CTaskComplexSunbathe() {
    if (m_pTowel) {
        CEntity::SafeCleanUpRef(m_pTowel);
        m_pTowel->m_nObjectType = OBJECT_TEMPORARY;
    }

    const auto UnrefAnimBlock = [](bool& refed, int32 blkIdx) {
        if (refed) {
            CAnimManager::RemoveAnimBlockRef(blkIdx);
            refed = false;
        }
    };
    UnrefAnimBlock(m_bBeachAnimsReferenced, m_BeachAnimBlockIndex);
    UnrefAnimBlock(m_bSunbatheAnimsReferenced, m_SunbatheAnimBlockIndex);
}

// 0x6320F0
bool CTaskComplexSunbathe::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (event && (event->GetEventType() == static_cast<eEventType>(2) || event->GetEventType() == static_cast<eEventType>(3))) return false;
    if (!m_pSubTask->MakeAbortable(ped, priority, event)) return false;
    m_bAborted = true;
    return true;
}

// 0x6399F0
CTask* CTaskComplexSunbathe::CreateNextSubTask(CPed* ped) {
    if (ped->m_nPedType == static_cast<ePedType>(22) || ped->m_nPedType == static_cast<ePedType>(5)) {
        g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_2, ped);
    }
    auto create = [&](int type) { return CreateSubTask(static_cast<eTaskType>(type), ped); };
    switch (m_pSubTask->GetTaskType()) {
    case static_cast<eTaskType>(0xCA):
        if (!m_bBathing) {
            if (m_pSunbatheAnimBlock->IsLoaded && m_pBeachAnimBlock->IsLoaded) return create(0x1AC);
        } else if (SunbatheShouldLoad() && m_pSunbatheAnimBlock->IsLoaded) return create(0x1AE);
        return create(0x516);
    case static_cast<eTaskType>(0x1A2):
        if (!m_pSunbatheAnimBlock->IsLoaded) return create(0xCA);
        if (SunbatheWeatherAllows() && !m_BathingTimer.IsOutOfTime() && (m_SunbatherType == 0 || m_SunbatherType == 2)) return create(0x1AD);
        return create(0x1AE);
    case static_cast<eTaskType>(0x1AC):
        if (!m_pBeachAnimBlock->IsLoaded) return create(0x516);
        m_BathingTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_BathingTimer.m_nInterval = CGeneral::GetRandomIntInRange(20000, 100000);
        m_BathingTimer.m_bStarted = true;
        { auto* result = create(0x1A2); m_bBathing = true; return result; }
    case static_cast<eTaskType>(0x1AD): return create(m_pBeachAnimBlock->IsLoaded ? 0x1A2 : 0x516);
    case static_cast<eTaskType>(0x1AE):
        { auto* result = create(0x516); m_bBathing = false; return result; }
    default: return nullptr;
    }
}

// 0x639CB0
CTask* CTaskComplexSunbathe::CreateFirstSubTask(CPed* ped) {
    CGeneral::GetRandomNumber();
    if (ped->m_nPedType == static_cast<ePedType>(5)) {
        const auto random = CGeneral::GetRandomNumber() & 0xFFFF;
        m_SunbatherType = static_cast<eSunbatherType>(2 - int(double(random) * (1.0 / 32768.0) * -3.0));
    } else {
        m_SunbatherType = static_cast<eSunbatherType>(CPopulation::IsSunbather(static_cast<eModelID>(ped->m_nModelIndex)) ? int(CGeneral::GetRandomNumber() < 0x3FFF) : 1);
    }
    if (!m_pBeachAnimBlock->IsLoaded) CStreaming::RequestModel(IFPToModelId(m_BeachAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
    if (!m_pSunbatheAnimBlock->IsLoaded) CStreaming::RequestModel(IFPToModelId(m_SunbatheAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
    m_bBathing = false;
    const bool load = SunbatheShouldLoad();
    if (m_bStartStanding && load) {
        if (!m_pTowel && m_pSunbatheAnimBlock->IsLoaded) return CreateSubTask(static_cast<eTaskType>(0x1AC), ped);
        return CreateSubTask(static_cast<eTaskType>(0xCA), ped);
    }
    if (!load && !m_pBeachAnimBlock->IsLoaded) return nullptr;
    if (m_pBeachAnimBlock->IsLoaded) {
        const auto random = CGeneral::GetRandomNumber() & 0xFFFF;
        m_BathingTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_BathingTimer.m_nInterval = 20000 - int(double(random) * (1.0 / 32768.0) * -80000.0);
        m_BathingTimer.m_bStarted = true;
        m_bBathing = true;
        return CreateSubTask(static_cast<eTaskType>(0x1A2), ped);
    }
    return load ? CreateSubTask(static_cast<eTaskType>(0xCA), ped) : nullptr;
}

// 0x6381A0
CTask* CTaskComplexSunbathe::ControlSubTask(CPed* ped) {
    if (m_bAborted) return nullptr;
    if (!m_bBeachAnimsReferenced) {
        if (m_pBeachAnimBlock->IsLoaded) { CAnimManager::AddAnimBlockRef(m_BeachAnimBlockIndex); m_bBeachAnimsReferenced = true; }
        else CStreaming::RequestModel(IFPToModelId(m_BeachAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
    }
    if (!m_bSunbatheAnimsReferenced && SunbatheShouldLoad()) {
        if (m_pSunbatheAnimBlock->IsLoaded) { CAnimManager::AddAnimBlockRef(m_SunbatheAnimBlockIndex); m_bSunbatheAnimsReferenced = true; }
        else CStreaming::RequestModel(IFPToModelId(m_SunbatheAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
    }
    if (m_bBathing) {
        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped) + 0x528) |= 8;
        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped) + 0x474) |= 0x100000;
    }
    if (!SunbatheWeatherAllows()) {
        m_BathingTimer.Pause();
        if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x1A2)) m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
    }
    return m_pSubTask;
}

CTask* CTaskComplexSunbathe::CreateSubTask(eTaskType type, CPed* ped) {
    if (m_pSubTask && m_pSubTask->GetTaskType() == type) return m_pSubTask;
    const auto anim = [&](int id, bool hold, const char* name) -> CTask* {
        return new CTaskSimpleRunAnim(static_cast<AssocGroupId>(0x30), static_cast<AnimationId>(id), 4.f, int(type), name, hold);
    };
    switch (type) {
    case static_cast<eTaskType>(0xCA): {
        const bool wait = (!m_pSunbatheAnimBlock->IsLoaded && SunbatheShouldLoad()) || !m_pBeachAnimBlock->IsLoaded;
        auto* storage = CTask::operator new(sizeof(CTaskSimplePause));
        return storage ? ::new (storage) CTaskSimplePause(wait ? 10000 : CGeneral::GetRandomIntInRange(1000, 5000)) : nullptr;
    }
    case static_cast<eTaskType>(0x1A2): {
        const auto remaining = [&]() { return m_BathingTimer.m_bStarted ? int32(m_BathingTimer.m_nStartTime + m_BathingTimer.m_nInterval - CTimer::GetTimeInMS()) : 0; };
        int duration;
        if (!m_pSunbatheAnimBlock->IsLoaded || (m_SunbatherType != 0 && m_SunbatherType != 2)) duration = remaining() + 1000;
        else duration = CGeneral::GetRandomIntInRange(3000, remaining() + 1000 < 12001 ? remaining() + 1000 : 12000);
        return new CTaskSimpleRunTimedAnim(static_cast<AssocGroupId>(0x2F), static_cast<AnimationId>(int(m_SunbatherType) + 0xEF), 4.f, -4.f, duration, type, "sunbathe", true);
    }
    case static_cast<eTaskType>(0x1AC): return anim(int(m_SunbatherType) + 0xF4, true, "start sunbathing");
    case static_cast<eTaskType>(0x1AD): {
        const int id = m_SunbatherType == 0 ? CGeneral::GetRandomIntInRange(0, 3) + 0xFE : m_SunbatherType == 2 ? CGeneral::GetRandomIntInRange(0, 3) + 0x101 : 0x1AD;
        return anim(id, true, "idle sunbathing");
    }
    case static_cast<eTaskType>(0x1AE): return anim(int(m_SunbatherType) + 0xF9, false, "stop sunbathing");
    default: return nullptr;
    }
}
