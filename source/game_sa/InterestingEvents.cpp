/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CInterestingEvents
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 5.
 * Validación registrada: 5/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Add | 0x00602590 | S
 *   Constructor | 0x006023A0 | S
 *   Destructor | 0x00856880 | S
 *   GetInterestingEvent | 0x006028A0 | S
 *   ScanForNearbyEntities | 0x00605A30 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "InterestingEvents.h"

auto& g_InterestingEvents = StaticRef<CInterestingEvents>(0xC0B058);

void CInterestingEvents::InjectHooks() {
    RH_ScopedClass(CInterestingEvents);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x6023A0);
    RH_ScopedInstall(Destructor, 0x856880);
    RH_ScopedInstall(Add, 0x602590);
    RH_ScopedInstall(ScanForNearbyEntities, 0x605A30);
    RH_ScopedInstall(GetInterestingEvent, 0x6028A0);
    RH_ScopedInstall(InvalidateEvent, 0x602960);
    RH_ScopedInstall(InvalidateNonVisibleEvents, 0x6029C0);
}

// 0x6023A0
CInterestingEvents::CInterestingEvents()
{
    m_nFlags = (m_nFlags & 0xF0) | 0x0E;
    m_b2 = true;
    m_b4 = true;
    m_b8 = true;

    /* Everything missing from here is initialized in the header */

    const auto SetOptions = [=](auto index, auto priority, auto delay, uint32 end = 0) {
        m_nPriorities[index] = priority;
        m_nDelays[index]     = delay;
        m_nEndsOfTime[index] = end;
    };

    SetOptions(INTERESTING_EVENT_0,     5,  2000);
    SetOptions(PEDS_CHATTING,           1,  5000);
    SetOptions(INTERESTING_EVENT_2,     1,  5000);
    SetOptions(INTERESTING_EVENT_3,     1,  5000);
    SetOptions(INTERESTING_EVENT_4,     2,  3000);
    SetOptions(INTERESTING_EVENT_5,     2,  3000);
    SetOptions(INTERESTING_EVENT_6,     2,  3000);
    SetOptions(INTERESTING_EVENT_7,     2,  3000);
    SetOptions(INTERESTING_EVENT_8,     4,  3000);
    SetOptions(INTERESTING_EVENT_9,     4,  3000);
    SetOptions(INTERESTING_EVENT_10,    5,  6000);
    SetOptions(INTERESTING_EVENT_11,    6,  6000);
    SetOptions(INTERESTING_EVENT_12,    6,  8000);
    SetOptions(INTERESTING_EVENT_13,    6,  5000);
    SetOptions(INTERESTING_EVENT_14,    5,  6000);
    SetOptions(INTERESTING_EVENT_15,    9,  6000);
    SetOptions(INTERESTING_EVENT_16,    9,  6000);
    SetOptions(VEHICLE_DAMAGE,          8,  6000);
    SetOptions(INTERESTING_EVENT_18,    7,  6000);
    SetOptions(INTERESTING_EVENT_19,    6,  5000);
    SetOptions(INTERESTING_EVENT_20,    7,  6000);
    SetOptions(INTERESTING_EVENT_21,    8,  8000);
    SetOptions(INTERESTING_EVENT_22,    9,  5000);
    SetOptions(GANG_ATTACKING_PED,      9,  6000);
    SetOptions(GANG_FIGHT,              9,  6000);
    SetOptions(INTERESTING_EVENT_25,    9,  6000);
    SetOptions(ZELDICK_OCCUPATION,      9,  8000);
    SetOptions(EVENT_ATTRACTOR,         10, 4000);
    SetOptions(INTERESTING_EVENT_28,    10, 4000);
}

CInterestingEvents* CInterestingEvents::Constructor() {
    this->CInterestingEvents::CInterestingEvents();
    return this;
}

// 0x856880
CInterestingEvents::~CInterestingEvents() {
    for (auto& event : m_Events) {
        CEntity::ClearReference(event.entity);
    }
}

void CInterestingEvents::Destructor() {
    g_InterestingEvents.~CInterestingEvents();
}

// 0x602590
void CInterestingEvents::Add(CInterestingEvents::EType type, CEntity* entity) {
    if (!m_b1 || !entity)
        return;

    const auto& camPos = CCamera::GetActiveCamera().m_vecSource;
    if (m_CurrentFrameCounter != CTimer::GetFrameCounter()) {
        m_CurrentFrameCounter = CTimer::GetFrameCounter();

        CPlayerPed* player = FindPlayerPed();
        const auto& playerPos = player->GetPosition();
        vec148 = playerPos - camPos;
        vec148.z = 0.f;
        if (vec148.NormaliseAndMag() == 0.f) {
            vec148 = player->GetMatrix().GetForward();
        }
        m_vecCenter = CVector{float(double(float(double(vec148.x) * m_fRadius)) + playerPos.x), float(double(float(double(vec148.y) * m_fRadius)) + playerPos.y), float(double(vec148.z) * m_fRadius + playerPos.z)};
    }

    CVector2D distance = m_vecCenter - entity->GetPosition();
    if (distance.SquaredMagnitude() > m_fRadius * m_fRadius)
        return;

    const auto& pos = entity->GetPosition();
    const float cameraDot = (camPos.y * vec148.y + camPos.z * vec148.z) + camPos.x * vec148.x;
    const float entityDot = (vec148.z * pos.z + vec148.y * pos.y) + vec148.x * pos.x;
    if (m_b2 && entityDot + -cameraDot < 0.0f) {
        return;
    }

    if (!CWorld::GetIsLineOfSightClear(camPos, entity->GetPosition(), true, false, false, false, false, true, false))
        return;

    const uint32 time = CTimer::GetTimeInMS();
    const bool cooldownPassed = time > m_nEndsOfTime[type];
    for (int8 index = 0; index < MAX_INTERESTING_EVENTS; ++index) {
        auto& event = m_Events[index];
        if (event.entity) {
            if (event.type != 0 && !((m_nPriorities[type] >= m_nPriorities[event.type]
                || time > event.time + uint32(m_nDelays[event.type]))
                && cooldownPassed && m_nInterestingEvent != index)) {
                continue;
            }
        } else {
            event.type = 0;
            event.entity = nullptr;
        }
        if (event.entity) {
            event.entity->CleanUpOldReference(&event.entity);
        }
        event.type = type;
        event.entity = entity;
        event.time = time;
        entity->RegisterReference(&event.entity);
        m_nEndsOfTime[type] = m_b8 ? time + (m_nDelays[type] >> 1) : time;
        break;
    }
}

namespace {
int32 InterestingSector(float coordinate, float radius, bool useEnd) {
        constexpr float scale = 0.02f, sectorBias = 60.0f;
        double result;
        __asm {
            fld coordinate
            cmp useEnd, 0
            je subtractRadius
            fadd radius
            jmp scaleSector
        subtractRadius:
            fsub radius
        scaleSector:
            fmul scale
            fadd sectorBias
            fstp result
        }
        return int32(std::floor(result));
}

}

// 0x605A30
void CInterestingEvents::ScanForNearbyEntities() {
    ZoneScoped;

    if (!g_InterestingEvents.m_b1)
        return;

    const auto UPDATE_INTERVAL = 500;
    if (CTimer::GetTimeInMS() - m_nLastScanTimeUpdate < UPDATE_INTERVAL) {
        return;
    }
    m_nLastScanTimeUpdate = CTimer::GetTimeInMS();

    CPlayerPed* player = FindPlayerPed();
    if (m_CurrentFrameCounter != CTimer::GetFrameCounter()) {
        m_CurrentFrameCounter = CTimer::GetFrameCounter();
        const auto& camPos = CCamera::GetActiveCamera().m_vecSource, playerPos = player->GetPosition();
        vec148 = playerPos - camPos;
        vec148.z = 0.f;
        if (vec148.NormaliseAndMag() == 0.f)
            vec148 = player->GetMatrix().GetForward();
        m_vecCenter = CVector{float(double(float(double(vec148.x) * m_fRadius)) + playerPos.x), float(double(float(double(vec148.y) * m_fRadius)) + playerPos.y), float(double(vec148.z) * m_fRadius + playerPos.z)};
    }

    const int32 startSectorX = std::max(InterestingSector(m_vecCenter.x, m_fRadius, false), 0);
    const int32 startSectorY = std::max(InterestingSector(m_vecCenter.y, m_fRadius, false), 0);
    const int32 endSectorX = std::min(InterestingSector(m_vecCenter.x, m_fRadius, true), 119);
    const int32 endSectorY = std::min(InterestingSector(m_vecCenter.y, m_fRadius, true), 119);

    CWorld::AdvanceCurrentScanCode();
    player->SetCurrentScanCode();

    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            auto& rs = CWorld::GetRepeatSector(sectorX, sectorY);

            for (auto* node = rs.Peds.GetNode(); node;) {
                auto* const ped = node->Item;
                node = node->Next;
                if (ped->IsScanCodeCurrent())
                    continue;

                ped->SetCurrentScanCode();

                if (ped->m_nPedState == PEDSTATE_DEAD)
                    continue;

                CEntity* entity;
                if (ped->bInVehicle && ped->m_pVehicle) {
                    entity = ped->m_pVehicle;
                } else {
                    entity = ped;
                }

                switch (ped->m_nPedType) {
                case PED_TYPE_COP:
                    Add(INTERESTING_EVENT_5, entity);
                    break;
                case PED_TYPE_CRIMINAL:
                    Add(INTERESTING_EVENT_6, entity);
                    break;
                case PED_TYPE_PROSTITUTE:
                    Add(INTERESTING_EVENT_4, entity);
                    break;
                default:
                    if (IsPedTypeGang(ped->m_nPedType)) {
                        Add(INTERESTING_EVENT_7, entity);
                    }
                    break;
                }
            }

            for (auto* node = rs.Vehicles.GetNode(); node;) {
                auto* const vehicle = node->Item;
                node = node->Next;
                if (vehicle->IsScanCodeCurrent())
                    continue;

                vehicle->SetCurrentScanCode();
                if (vehicle->physicalFlags.bRenderScorched != 0)
                    continue;

                if (!vehicle->m_pDriver)
                    continue;

                auto style = vehicle->m_autoPilot.m_nCarDrivingStyle;
                if (!style)
                    continue;

                if (style == DRIVING_STYLE_DRIVINGMODE_AVOIDCARS_STOPFORPEDS_OBEYLIGHTS)
                    continue;

                Add(INTERESTING_EVENT_14, vehicle);
            }
        }
    }
}

// 0x6028A0
TInterestingEvent* CInterestingEvents::GetInterestingEvent() {
    const uint32 now = CTimer::GetTimeInMS();
    if (m_b4 && m_nInterestingEvent != -1) {
        auto& event = m_Events[m_nInterestingEvent];
        if (event.entity && now < event.time + uint32(m_nDelays[event.type])) {
            return &event;
        }
    }
    uint8 priority = 0;
    int8 chosen = -1;
    for (int8 index = 0; index < MAX_INTERESTING_EVENTS; ++index) {
        auto& event = m_Events[index];
        if (event.entity && now < event.time + uint32(m_nDelays[event.type])
            && (m_nPriorities[event.type] > priority || uint16(CGeneral::GetRandomNumber()) < 128)) {
            priority = m_nPriorities[event.type];
            chosen = index;
        }
    }
    m_nInterestingEvent = chosen;
    return chosen == -1 ? nullptr : &m_Events[chosen];
}

// 0x602960
void CInterestingEvents::InvalidateEvent(const TInterestingEvent* event) {
    for (auto index = 0; index < MAX_INTERESTING_EVENTS; index++) {
        TInterestingEvent* tevent = &m_Events[index];
        if (tevent != event)
            continue;

        tevent->time = 0;
        CEntity::ClearReference(tevent->entity);
        if (m_nInterestingEvent == index) {
            m_nInterestingEvent = -1;
        }
    }
}

// 0x6029C0
void CInterestingEvents::InvalidateNonVisibleEvents() {
    const auto& camPos = CCamera::GetActiveCamera().m_vecSource;
    for (auto i = 0; i < MAX_INTERESTING_EVENTS; i++) {
        TInterestingEvent& event = m_Events[i];
        if (!event.entity)
            continue;

        CVector pos = event.entity->GetPosition();
        if (CWorld::GetIsLineOfSightClear(camPos, pos, true, false, false, false, false, true, false))
            continue;

        event.time = 0;
        CEntity::SafeCleanUpRef(event.entity);
        if (m_nInterestingEvent == i) {
            m_nInterestingEvent = -1;
        }
    }
}
