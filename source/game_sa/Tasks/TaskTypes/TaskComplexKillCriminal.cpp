/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexKillCriminal
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

#include <extensions/utility.hpp>

#include "TaskComplexKillCriminal.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskSimpleGangDriveBy.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexCarDriveMission.h"
#include "EventAcquaintancePedHate.h"
#include "InterestingEvents.h"


void CTaskComplexKillCriminal::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillCriminal, 0x870a00, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x68BE70);
    RH_ScopedInstall(Destructor, 0x68BF30);

    RH_ScopedInstall(CreateSubTask, 0x68C050);
    RH_ScopedInstall(FindNextCriminalToKill, 0x68C3C0);
    RH_ScopedInstall(ChangeTarget, 0x68C6E0);

    RH_ScopedVMTInstall(Clone, 0x68CE50);
    RH_ScopedVMTInstall(GetTaskType, 0x68BF20);
    RH_ScopedVMTInstall(MakeAbortable, 0x68DAD0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x68E4F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x68DC60);
    RH_ScopedVMTInstall(ControlSubTask, 0x68E950);
}

bool NoPedOrNoHp(CPed* ped) {
    return !ped || ped->m_fHealth <= 0.f;
}

// 0x68BE70
CTaskComplexKillCriminal::CTaskComplexKillCriminal(CPed* criminal, bool randomize) :
    m_Randomize{randomize}
{
    // Como el original (0x68BE70): el EXE guarda el puntero y solo registra la referencia si pasa todos los filtros
    // (IsPlayer 0x5DF8F0, tipos 6/0x12/0x13, tipo >= 0x18 (cualquier MISSIONn) y creado por mision [+0x484] == 2)
    if (   criminal
        && !criminal->IsPlayer()
        && criminal->m_nPedType != PED_TYPE_COP
        && criminal->m_nPedType != PED_TYPE_MEDIC
        && criminal->m_nPedType != PED_TYPE_FIREMAN
        && (int32)criminal->m_nPedType < (int32)PED_TYPE_MISSION1
        && !criminal->IsCreatedByMission()
    ) {
        m_Criminal = criminal;
    }
}

// NOTSA (For 0x68CE50)
CTaskComplexKillCriminal::CTaskComplexKillCriminal(const CTaskComplexKillCriminal& o) :
    CTaskComplexKillCriminal{o.m_Criminal, false}
{
}

// 0x68BF30
CTaskComplexKillCriminal::~CTaskComplexKillCriminal() {
    if (m_Cop) {
        m_Cop->m_nTimeTillWeNeedThisPed = CTimer::GetTimeInMS();
        m_Cop->bCullExtraFarAway = false;
        m_Cop->m_fRemovalDistMultiplier = 1.f;
        if (const auto veh = m_Cop->m_pVehicle) {
            veh->m_nExtendedRemovalRange = false;
            veh->vehicleFlags.bNeverUseSmallerRemovalRange = false;
            if (veh->IsDriver(m_Cop)) {
                const auto ap = &veh->m_autoPilot;
                ap->SetCarMission(MISSION_CRUISE);
                ap->SetDrivingStyle(DRIVING_STYLE_AVOID_CARS);
                ap->SetCruiseSpeed(10);
                if (veh->GetStatus() != STATUS_SIMPLE) {
                    CCarCtrl::JoinCarWithRoadSystem(veh);
                }
                veh->vehicleFlags.bSirenOrAlarm = false;
            }
            // Como el original: 0x68BF30 solo limpia la sirena (and byte [veh+42Dh],7Fh) dentro de la rama del conductor
        }
    }
}

// 0x68C050
CTask* CTaskComplexKillCriminal::CreateSubTask(eTaskType tt, CPed* ped, bool force) {
    if (!force && m_pSubTask && m_pSubTask->GetTaskType() == tt) {
        return m_pSubTask;        
    }

    switch (tt) {
    case TASK_COMPLEX_KILL_PED_ON_FOOT: {
        // Como el original: 0x68C361 construye primero y 0x68C36E (SetCurrentWeapon(22)) llega despues
        const auto task = new CTaskComplexKillPedOnFoot{ m_Criminal };
        ped->SetCurrentWeapon(WEAPON_PISTOL);
        return task;
    }
    case TASK_FINISHED: {
        if (m_Criminal) {
            m_Criminal->SetPedDefaultDecisionMaker();
        }
        return nullptr;
    }
    case TASK_SIMPLE_GANG_DRIVEBY: {
        // Como el original: 0x68C308 comparte el final 0x68C36A con KILL_PED_ON_FOOT: SetCurrentWeapon(22) tras construir
        const auto task = new CTaskSimpleGangDriveBy{
            m_Criminal,
            nullptr,
            70.f,
            70,
            eDrivebyStyle::AI_ALL_DIRN,
            false
        };
        ped->SetCurrentWeapon(WEAPON_PISTOL);
        return task;
    }
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
        return new CTaskComplexEnterCarAsPassenger{ ped->m_pVehicle };
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
        return new CTaskComplexEnterCarAsDriver{ ped->m_pVehicle };
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, 0, 0, true, false };
    case TASK_SIMPLE_CAR_DRIVE:
        return new CTaskSimpleCarDrive{ ped->m_pVehicle };
    case TASK_COMPLEX_CAR_DRIVE_MISSION: {
        const auto oveh = ped->m_pVehicle; // (o)ur (veh)icle
        if (!oveh) {
            return nullptr;
        }

        const auto CreateDriveMission = [&, this](eCarMission mission, float cruiseSpeed, CEntity* traget) {
            return new CTaskComplexCarDriveMission{
                ped->m_pVehicle,
                traget,
                mission,
                DRIVING_STYLE_AVOID_CARS,
                cruiseSpeed
            };
        };

        if (const auto cveh = m_Criminal->GetVehicleIfInOne()) {
            return CreateDriveMission(
                oveh->IsBike()
                    ? MISSION_FOLLOWCAR_CLOSE
                    : MISSION_BLOCKCAR_CLOSE,
                (float)(m_Criminal->m_pVehicle->m_autoPilot.m_nCruiseSpeed) + 10.f,
                cveh
            );
        } else {
            return CreateDriveMission(MISSION_KILLPED_CLOSE, 20.f, m_Criminal);
        }
    }
    default:
        return nullptr; // Como el original: 0x68C09F/0x68C0BA/0x68C2EE devuelven 0 para tipos sin caso
    }
}

// 0x68C3C0
CPed* CTaskComplexKillCriminal::FindNextCriminalToKill(CPed* ped, bool any) {
    // Como el original (0x68C3C0): no es una busqueda del mas cercano. [esp+0Ch] = distancia^2 de referencia
    // (la del criminal actual si !any y esta vivo, si no 0.0f) y NO se actualiza; gana el ULTIMO candidato
    // valido (distinto del criminal actual) con d < referencia, o cualquiera si la referencia es 0.0f.
    // d^2 se suma en el orden del EXE: (dz*dz + dy*dy) + dx*dx.
    const auto DistSq = [ped](CPed* other) {
        const CVector& a = other->GetPosition();
        const CVector& b = ped->GetPosition();
        const float dx = a.x - b.x;
        const float dy = a.y - b.y;
        const float dz = a.z - b.z;
        return (dz * dz + dy * dy) + dx * dx;
    };
    const auto Alive = [](CPed* p) { return p->m_fHealth > 0.f; }; // fcomp 0.0 + test ah,41h: vivo solo si health > 0

    float refDistSq = 0.f;
    CPed* best      = nullptr;
    if (!any && m_Criminal && Alive(m_Criminal)) {
        refDistSq = DistSq(m_Criminal);
    }
    for (int32 i = 0; i < 5; i++) { // +0x7AC..+0x7BC
        CPed* const c = m_Cop->m_apCriminalsToKill[i];
        if (!c || !Alive(c) || c == m_Criminal.Get()) {
            continue;
        }
        const float d = DistSq(c);
        if (d < refDistSq || refDistSq == 0.f) {
            best = c;
        }
    }
    return best;
}

// 0x68C6E0
bool CTaskComplexKillCriminal::ChangeTarget(CPed* newTarget) { // TODO: Figure out if `newTarget` is actually the new target, or it's just he ped that is the owner of this task
    if (newTarget == m_Criminal) {
        return true;
    }

    if (NoPedOrNoHp(newTarget)) {
        return false;
    }

    if (m_Criminal && m_Criminal->bInVehicle) {
        return false;
    }

    if (notsa::isa_and_nonnull<CTaskComplexKillPedOnFoot>(m_pSubTask) && !m_pSubTask->MakeAbortable(newTarget, ABORT_PRIORITY_URGENT, nullptr)) {
        return false;
    }

    { // 0x68C760: el objetivo debe estar en la lista Y vivo (fcomp 0.0 + test ah,41h + je)
        bool found = false;
        for (int32 i = 0; i < 5; i++) {
            const auto c = m_Cop->m_apCriminalsToKill[i];
            if (c == newTarget && c->m_fHealth > 0.f) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }

    m_Criminal = newTarget;

    // Propagate change to partner
    if (const auto partner = m_Cop->m_pCopPartner) {
        if (partner->bInVehicle) {
            // Como el original: 0x68C7C6 CPedIntelligence::FindTaskByType(0x451) (0x600EE0) y se llama a ChangeTarget sin mas comprobaciones
            if (const auto partnersTask = partner->GetIntelligence()->FindTaskByType(TASK_COMPLEX_KILL_CRIMINAL)) {
                static_cast<CTaskComplexKillCriminal*>(partnersTask)->ChangeTarget(newTarget);
            }
        }
    }

    m_HasFinished = false;

    return true;
}

// 0x68DAD0
bool CTaskComplexKillCriminal::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    // Como el original (0x68DAD0): GetEventType (slot 1) una vez y GetSourceEntity (slot 10) se vuelve a llamar en cada uso
    if (event) {
        const auto evType = event->GetEventType();
        const auto IsTypeIn = [&](std::initializer_list<eEventType> types) { return notsa::contains(types, evType); };
        const auto Tick = [&] { const_cast<CEvent*>(event)->m_nTimeActive++; };
        if (event->GetSourceEntity() == m_Criminal.Get() && m_Criminal) {
            if (IsTypeIn({ EVENT_DAMAGE, EVENT_ACQUAINTANCE_PED_HATE, EVENT_VEHICLE_DAMAGE_WEAPON, EVENT_VEHICLE_DAMAGE_COLLISION, EVENT_GUN_AIMED_AT, EVENT_SHOT_FIRED })) { // 0x68DB00
                Tick();
                return false;
            }
        } else { // 0x68DB35
            if (IsTypeIn({ EVENT_ACQUAINTANCE_PED_HATE, EVENT_VEHICLE_DAMAGE_COLLISION })) {
                Tick();
                return false;
            }
            if (IsTypeIn({ EVENT_DAMAGE, EVENT_VEHICLE_DAMAGE_WEAPON, EVENT_GUN_AIMED_AT, EVENT_SHOT_FIRED })) { // 0x68DB5F
                if (   event->GetSourceEntity()
                    && event->GetSourceEntity()->GetIsTypePed()
                    && !event->GetSourceEntity()->AsPed()->IsPlayer()
                    && m_Cop
                    && m_Cop->AddCriminalToKill(event->GetSourceEntity()->AsPed()) != 0 // Como el original: 0x68DBB3 test eax,eax / je (el -1 de fallo cuenta como exito)
                ) {
                    if (IsTypeIn({ EVENT_DAMAGE, EVENT_VEHICLE_DAMAGE_WEAPON })) { // Change target immediately
                        // Como el original: 0x68DC02 fcomp 625.0 + test ah,5 + jp => estrictamente menor
                        if (!m_Criminal || (m_Criminal->GetPosition() - ped->GetPosition()).SquaredMagnitude() < sq(25.f)) {
                            ChangeTarget(event->GetSourceEntity()->AsPed());
                        }
                    }
                }
                Tick();
                return false;
            }
        }
    }
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x68E4F0
CTask* CTaskComplexKillCriminal::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GANG_DRIVEBY:
        return CreateSubTask(
            m_Cop->m_isTheDriver
                ? TASK_COMPLEX_CAR_DRIVE_MISSION
                : TASK_SIMPLE_CAR_DRIVE,
            ped
        );
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // Try finding the next criminal, if none, set `m_finished` and get into *the* car (if possible)
        CPed* const nextCriminal = NoPedOrNoHp(m_Criminal)
            ? FindNextCriminalToKill(ped, true)
            : m_Criminal.Get();
        if (nextCriminal && ChangeTarget(nextCriminal)) {
            return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped, true);
        }

        // No criminal, or can't target it, so just bail, so try getting back into the car
        m_HasFinished = true;
        if (m_CantGetInCar || !ped->m_pVehicle) {
            return CreateSubTask(TASK_FINISHED, ped);
        }

        if (!m_Cop->m_isTheDriver) {
            if (NoPedOrNoHp(m_Cop->m_pCopPartner)) { // Partner is dead, we get in as the driver
                m_Cop->m_isTheDriver = true;
                m_Cop->SetPartner(nullptr);
            } else {
                return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER, ped);
            }
        }

        return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_DRIVER, ped);
    }
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER: { // 0x68E533
        if (!ped->bInVehicle) {
            m_CantGetInCar = true; // Como el original: 0x68E547 mov byte [esi+1Ah],1
            return CreateSubTask(
                m_HasFinished || NoPedOrNoHp(m_Criminal)
                    ? TASK_FINISHED
                    : TASK_COMPLEX_KILL_PED_ON_FOOT,
                ped
            );
        }
        const auto copPartnerNoneOrInVeh = NoPedOrNoHp(m_Cop->m_pCopPartner) || m_Cop->m_pCopPartner->bInVehicle;
        if (!m_HasFinished && !NoPedOrNoHp(m_Criminal)) {
            // Como el original: 0x68E5D8 si el criminal esta en un vehiculo va directo a 0x68E635 (perseguir en coche);
            // a pie: sin vehiculo propio o d^2 <= 625 => a pie, si no perseguir
            const bool onFoot = m_Criminal->IsInVehicle()
                ? false
                : (!ped->m_pVehicle || m_Criminal->IsEntityInRange(ped->m_pVehicle, 25.f));
            return CreateSubTask(
                onFoot
                    ? TASK_COMPLEX_KILL_PED_ON_FOOT
                    : copPartnerNoneOrInVeh // otherwise if criminal is too far chase them with the car
                        ? TASK_COMPLEX_CAR_DRIVE_MISSION
                        : TASK_SIMPLE_CAR_DRIVE,
                ped
            );
        }
        if (const auto next = FindNextCriminalToKill(ped, true); next && ChangeTarget(next)) { // Try finding another criminal to kill
            return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped, true);
        }
        // No criminal to kill, so get into *the* vehicle and fuck off
        if (ped->IsInVehicle()) {
            ped->m_pVehicle->vehicleFlags.bSirenOrAlarm = false;
        }
        return CreateSubTask(
            copPartnerNoneOrInVeh
                ? TASK_FINISHED
                : TASK_SIMPLE_CAR_DRIVE,
            ped
        );
    }
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: { // 0x68E6FA
        if (ped->bInVehicle) { // (Inverted)
            return CreateSubTask(TASK_SIMPLE_CAR_DRIVE, ped);
        }
        m_CantGetInCar = true;
        if (m_HasFinished || NoPedOrNoHp(m_Criminal)) {
            return CreateSubTask(TASK_FINISHED, ped);
        } 
        return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
    }
    case TASK_COMPLEX_LEAVE_CAR: // 0x68E77F
        return CreateSubTask(
            !ped->m_pVehicle || m_CantGetInCar || (!m_HasFinished && !NoPedOrNoHp(m_Criminal) && !m_Criminal->IsInVehicle() && m_Criminal->IsEntityInRange(ped, 25.f))
                ? TASK_COMPLEX_KILL_PED_ON_FOOT     // Criminal can be killed on foot
                : TASK_COMPLEX_ENTER_CAR_AS_DRIVER, // We have to chase the criminal with a vehicle
            ped
        );
    default:
        return nullptr; // Como el original: 0x68E936 devuelve 0 para subtareas sin caso
    }
}

// Construction shared by the two inlined EXE dispatchers. Weapon selection is
// deliberately performed after construction, whereas CreateSubTask does it first.
CTask* CTaskComplexKillCriminal::CreateChaseSubTask(eTaskType type, CPed* ped) {
    if (m_pSubTask && m_pSubTask->GetTaskType() == type) {
        return m_pSubTask;
    }
    CTask* task{};
    switch (type) {
    case TASK_COMPLEX_KILL_PED_ON_FOOT:
        task = new CTaskComplexKillPedOnFoot{m_Criminal, -1, 0, 0, 0, 1};
        break;
    case TASK_SIMPLE_GANG_DRIVEBY:
        task = new CTaskSimpleGangDriveBy{m_Criminal, nullptr, 70.f, 70, eDrivebyStyle::AI_ALL_DIRN, false};
        break;
    default:
        return CreateSubTask(type, ped, true);
    }
    ped->SetCurrentWeapon(WEAPON_PISTOL);
    return task;
}

namespace {
template<typename T> T& CriminalField(void* object, size_t offset) {
    return *reinterpret_cast<T*>(static_cast<uint8*>(object) + offset);
}
void CriminalHateEvent(CPed* target, CPed* recipient, eTaskType taskType) {
    alignas(CEventAcquaintancePedHate) uint8 storage[sizeof(CEventAcquaintancePedHate)];
    auto* event = reinterpret_cast<CEventAcquaintancePedHate*>(storage);
    event->Constructor2(target);
    event->m_TaskId = taskType;
    recipient->GetEventGroup().Add(event, false);
    event->CEventAcquaintancePed::~CEventAcquaintancePed();
}
}

// 0x68DC60
CTask* CTaskComplexKillCriminal::CreateFirstSubTask(CPed* ped) {
    if (!m_Criminal || m_Criminal->IsPlayer() || (FindPlayerPed()->GetPlayerData()->m_pWanted->m_WantedLevel != static_cast<eWantedLevel>(0))
        || !StaticRef<bool>(0xB72994) || ped->m_nPedType != PED_TYPE_COP) {
        return nullptr;
    }
    if (auto* vehicle = m_Criminal->m_pVehicle) {
        if (vehicle->m_pDriver && vehicle->m_pDriver->IsPlayer()) {
            return nullptr;
        }
        for (size_t i = 0; i < vehicle->m_nMaxPassengers; ++i) {
            if (auto* passenger = vehicle->m_apPassengers[i]; passenger && passenger->IsPlayer()) {
                return nullptr;
            }
        }
    }
    // This entry writes the reference directly before registering it; no cleanup.
    CriminalField<CCopPed*>(this, 0x10) = static_cast<CCopPed*>(ped);
    ped->RegisterReference(reinterpret_cast<CEntity**>(&CriminalField<CCopPed*>(this, 0x10)));
    if (m_Randomize && !(CriminalField<uint32>(m_Criminal.Get(), 0x478) & 0x800)) {
        auto* vehicle = m_Criminal->GetVehicleIfInOne();
        if (!vehicle || !(CriminalField<uint8>(vehicle, 0x42E) & 8)
            || CGeneral::GetRandomIntInRange(0, 3) != 0) {
            return nullptr;
        }
    }
    m_Cop->AddCriminalToKill(m_Criminal);
    if (m_Criminal->bInVehicle) {
        m_Criminal->GetIntelligence()->SetPedDecisionMakerType(6);
    }
    CTask* task{};
    bool selected{};
    if (ped->bInVehicle && ped->m_pVehicle) {
        auto* vehicle = ped->m_pVehicle;
        if (vehicle->m_pDriver == ped) {
            m_Cop->m_isTheDriver = true;
            if (!m_Cop->m_pCopPartner) {
                for (size_t i = 0; i < vehicle->m_nMaxPassengers; ++i) {
                    auto* passenger = vehicle->m_apPassengers[i];
                    if (passenger && passenger->m_nPedType == PED_TYPE_COP) {
                        m_Cop->SetPartner(static_cast<CCopPed*>(passenger));
                        break;
                    }
                }
            }
            if (m_Cop->m_pCopPartner) {
                CriminalHateEvent(m_Criminal, m_Cop->m_pCopPartner, TASK_COMPLEX_KILL_CRIMINAL);
            }
        } else {
            if (auto* driver = vehicle->m_pDriver; driver && driver->m_nPedType == PED_TYPE_COP) {
                m_Cop->SetPartner(static_cast<CCopPed*>(driver));
                static_cast<CCopPed*>(vehicle->m_pDriver)->m_isTheDriver = true;
            }
            if (auto* partner = m_Cop->m_pCopPartner) {
                if (partner->m_isTheDriver) {
                    auto* chase = partner->GetIntelligence()->FindTaskByType(TASK_COMPLEX_KILL_CRIMINAL);
                    if (chase && CriminalField<CPed*>(chase, 0xC) == m_Criminal.Get()) {
                        m_Cop->m_isTheDriver = false;
                        CriminalField<uint32>(m_Cop.Get(), 0x46C) |= 0x20000000;
                    } else {
                        CriminalHateEvent(m_Criminal, partner, TASK_COMPLEX_KILL_CRIMINAL);
                        return nullptr;
                    }
                }
            } else {
                m_Cop->m_isTheDriver = true;
                task = CreateChaseSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
                selected = task != nullptr;
            }
        }
        if (!selected) {
            task = CreateChaseSubTask(m_Criminal->GetVehicleIfInOne()
                ? (m_Cop->m_isTheDriver ? TASK_COMPLEX_CAR_DRIVE_MISSION : TASK_SIMPLE_CAR_DRIVE)
                : TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
        }
    } else {
        if (NoPedOrNoHp(m_Cop->m_pCopPartner)) {
            m_Cop->m_isTheDriver = true;
        }
        task = CreateChaseSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
    }
    if (auto* vehicle = ped->m_pVehicle; vehicle && m_Cop->m_isTheDriver) {
        m_OrigDrivingMode = CriminalField<int8>(vehicle, 0x3B9);
        m_OrigMission = CriminalField<int8>(vehicle, 0x3BA);
        m_OrigCruiseSpeed = CriminalField<uint8>(vehicle, 0x3D0);
        m_IsSetUp = true;
    }
    const auto arm = [](CPed* who) {
        who->GiveWeapon(WEAPON_PISTOL, 1000, true);
        who->SetCurrentWeapon(WEAPON_PISTOL);
    };
    if (auto* vehicle = m_Criminal->m_pVehicle) {
        if (CriminalField<uint8>(vehicle, 0x42E) & 8) {
            if (CriminalField<uint8>(vehicle, 0x484)) {
                arm(m_Criminal);
                for (size_t i = 0; i < vehicle->m_nMaxPassengers; ++i) {
                    if (auto* passenger = vehicle->m_apPassengers[i]) {
                        arm(passenger);
                        m_Cop->AddCriminalToKill(passenger);
                    }
                }
            } else if (CGeneral::GetRandomNumber() & 1) {
                arm(m_Criminal);
            }
        }
    } else if (CGeneral::GetRandomNumber() & 1) {
        arm(m_Criminal);
        CriminalHateEvent(ped, m_Criminal, TASK_COMPLEX_KILL_PED_ON_FOOT);
    }
    ped->m_nTimeTillWeNeedThisPed = CTimer::GetTimeInMS() + 300000;
    ped->bCullExtraFarAway = true;
    ped->m_fRemovalDistMultiplier = 0.3f;
    if (auto* vehicle = ped->m_pVehicle) {
        CriminalField<uint16>(vehicle, 0x4A6) = 0xFF;
        CriminalField<uint8>(vehicle, 0x42C) |= 0x40;
        if (ped->bInVehicle) {
            CriminalField<uint8>(vehicle, 0x42D) |= 0x80;
        }
    }
    g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_25, ped);
    return task;
}

// 0x68E950
CTask* CTaskComplexKillCriminal::ControlSubTask(CPed* ped) {
    if (m_Criminal && (m_Criminal->IsPlayer() || m_Criminal->m_nPedType == PED_TYPE_COP
        || m_Criminal->m_nPedType == PED_TYPE_MEDIC || m_Criminal->m_nPedType == PED_TYPE_FIREMAN
        || m_Criminal->m_nPedType >= PED_TYPE_MISSION1 || m_Criminal->IsCreatedByMission())) {
        return nullptr;
    }
    if ((FindPlayerPed()->GetPlayerData()->m_pWanted->m_WantedLevel != static_cast<eWantedLevel>(0))
        && FindPlayerWanted()->CanCopJoinPursuit(static_cast<CCopPed*>(ped))
        && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        return nullptr;
    }
    if (!StaticRef<bool>(0xB72994)) {
        return nullptr;
    }
    auto* original = m_pSubTask;
    auto type = TASK_NONE;
    auto* partner = m_Cop->m_pCopPartner;
    const auto squaredDistance = [](CEntity* from, CEntity* to) {
        return (from->GetPosition() - to->GetPosition()).SquaredMagnitude();
    };
    const auto distance = [](CEntity* from, CEntity* to) {
        return (from->GetPosition() - to->GetPosition()).Magnitude();
    };
    if (!m_Cop->m_isTheDriver && NoPedOrNoHp(partner)) {
        m_Cop->m_isTheDriver = true;
        m_Cop->SetPartner(nullptr);
        if (ped->GetVehicleIfInOne()) {
            type = TASK_COMPLEX_LEAVE_CAR;
        }
    }
    if (ped->m_fHealth <= 0.f) {
        type = TASK_FINISHED;
    } else if (NoPedOrNoHp(m_Criminal)) {
        if (m_pSubTask->GetTaskType() == TASK_COMPLEX_KILL_PED_ON_FOOT) {
            goto common;
        }
        if (auto* target = FindNextCriminalToKill(ped, true); target && ChangeTarget(target)) {
            goto common;
        }
        if (m_pSubTask->GetTaskType() == TASK_COMPLEX_ENTER_CAR_AS_PASSENGER
            || m_pSubTask->GetTaskType() == TASK_COMPLEX_ENTER_CAR_AS_DRIVER) {
            goto common;
        }
        if (ped->m_pVehicle && !ped->bInVehicle && !m_CantGetInCar) {
            type = m_Cop->m_isTheDriver ? TASK_COMPLEX_ENTER_CAR_AS_DRIVER : TASK_COMPLEX_ENTER_CAR_AS_PASSENGER;
        } else if (!ped->bInVehicle || m_pSubTask->GetTaskType() != TASK_SIMPLE_CAR_DRIVE
            || NoPedOrNoHp(partner) || partner->bInVehicle) {
            type = TASK_FINISHED;
        }
    } else {
        if (!m_Cop->m_isTheDriver) {
            type = TASK_COMPLEX_KILL_PED_ON_FOOT;
            if ((partner->bInVehicle || partner->GetIntelligence()->FindTaskByType(TASK_COMPLEX_ENTER_CAR_AS_DRIVER))
                && ped->m_pVehicle && !m_CantGetInCar) {
                if (!ped->bInVehicle) {
                    type = TASK_COMPLEX_ENTER_CAR_AS_PASSENGER;
                } else if (partner->bInVehicle && distance(m_Criminal, ped->m_pVehicle) < 60.f) {
                    type = TASK_SIMPLE_GANG_DRIVEBY;
                } else {
                    type = TASK_SIMPLE_CAR_DRIVE;
                }
            }
        } else {
            if (auto* vehicle = ped->GetVehicleIfInOne()) {
                CriminalField<uint8>(vehicle, 0x42D) |= 0x80;
            }
            if (!m_Criminal->GetVehicleIfInOne()) {
                if (!ped->GetVehicleIfInOne()) {
                    if (!m_CantGetInCar && ped->m_pVehicle) {
                        if (squaredDistance(m_Criminal, ped) > 625.f
                            || squaredDistance(ped->m_pVehicle, ped) > 250.f) {
                            if (auto* next = FindNextCriminalToKill(ped, false); next && ChangeTarget(next)) {
                                type = TASK_COMPLEX_KILL_PED_ON_FOOT;
                            } else {
                                type = TASK_COMPLEX_ENTER_CAR_AS_DRIVER;
                            }
                        }
                    }
                } else {
                    auto* vehicle = ped->m_pVehicle;
                    const auto d = squaredDistance(m_Criminal, vehicle);
                    const bool partnerReady = NoPedOrNoHp(partner) || partner->bInVehicle;
                    if (d <= 225.f) {
                        if ((CriminalField<uint8>(vehicle, 0x3BA) != 55 && CriminalField<uint8>(vehicle, 0x3BA) != 54)
                            || d < (vehicle->IsBike() ? 5.f : 16.f)) {
                            type = TASK_COMPLEX_KILL_PED_ON_FOOT;
                        }
                    } else if (partnerReady) {
                        type = TASK_COMPLEX_CAR_DRIVE_MISSION;
                    }
                }
            } else {
                auto* criminalVehicle = m_Criminal->m_pVehicle;
                const float health = criminalVehicle->m_fHealth;
                bool leave{};
                if (!ped->GetVehicleIfInOne()) {
                    if (criminalVehicle->m_vecMoveSpeed.Magnitude() >= 0.2f) {
                        if (ped->m_pVehicle && !m_CantGetInCar
                            && m_pSubTask->GetTaskType() != TASK_COMPLEX_ENTER_CAR_AS_DRIVER
                            && m_pSubTask->GetTaskType() != TASK_COMPLEX_ENTER_CAR_AS_PASSENGER) {
                            const auto d = squaredDistance(criminalVehicle, ped->m_pVehicle);
                            if (criminalVehicle->m_vecMoveSpeed.Magnitude() >= 0.2f || d > 400.f) {
                                type = m_Cop->m_isTheDriver ? TASK_COMPLEX_ENTER_CAR_AS_DRIVER : TASK_COMPLEX_ENTER_CAR_AS_PASSENGER;
                            }
                        }
                    } else {
                        leave = squaredDistance(m_Criminal, ped) < 36.f
                            || (partner && !partner->bInVehicle && squaredDistance(m_Criminal, partner) < 36.f);
                    }
                } else {
                    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_CAR_DRIVE_MISSION
                        || m_pSubTask->GetTaskType() == TASK_SIMPLE_GANG_DRIVEBY) {
                        if (criminalVehicle->m_vecMoveSpeed.Magnitude() >= 0.12f) {
                            m_TimeToGetOutOfCar = 1.f;
                        } else {
                            m_TimeToGetOutOfCar -= CTimer::GetTimeStep() * 0.02f;
                            if (m_TimeToGetOutOfCar <= 0.f && ped->GetEventHandler().GetCurrentEventType() != static_cast<eEventType>(36)
                                && squaredDistance(criminalVehicle, ped->m_pVehicle) < 225.f) {
                                type = TASK_COMPLEX_KILL_PED_ON_FOOT;
                                if (auto* next = FindNextCriminalToKill(ped, false)) {
                                    ChangeTarget(next);
                                }
                            }
                        }
                    } else if (m_pSubTask->GetTaskType() == TASK_SIMPLE_CAR_DRIVE
                        && (NoPedOrNoHp(partner) || partner->bInVehicle)) {
                        type = TASK_COMPLEX_CAR_DRIVE_MISSION;
                    }
                }
                if (leave || health < 250.f) {
                    alignas(CEventAcquaintancePedHate) uint8 storage[sizeof(CEventAcquaintancePedHate)];
                    auto* event = reinterpret_cast<CEventAcquaintancePedHate*>(storage);
                    event->Constructor2(ped);
                    event->m_TaskId = static_cast<eTaskType>(m_Criminal->m_pVehicle->m_fHealth >= 250.f ? 1030 : 704);
                    m_Criminal->GetEventGroup().Add(event, false);
                    m_Criminal->SetPedDefaultDecisionMaker();
                    auto* vehicle = m_Criminal->m_pVehicle;
                    if (auto* driver = vehicle->m_pDriver; driver && driver != m_Criminal.Get() && !driver->IsPlayer()) {
                        driver->GetEventGroup().Add(event, false);
                    }
                    for (size_t i = 0; i < vehicle->m_nMaxPassengers; ++i) {
                        if (auto* passenger = vehicle->m_apPassengers[i]; passenger && passenger != m_Criminal.Get() && !passenger->IsPlayer()) {
                            passenger->GetEventGroup().Add(event, false);
                            passenger->SetPedDefaultDecisionMaker();
                        }
                    }
                    event->CEventAcquaintancePed::~CEventAcquaintancePed();
                }
            }
        }
        if (auto* vehicle = ped->m_pVehicle; vehicle && vehicle->IsBike()) {
            if (m_pSubTask->GetTaskType() == TASK_COMPLEX_CAR_DRIVE_MISSION
                && (type == TASK_NONE || type == TASK_COMPLEX_CAR_DRIVE_MISSION)) {
                if (distance(m_Criminal, vehicle) < 60.f) {
                    type = TASK_SIMPLE_GANG_DRIVEBY;
                } else if (type != TASK_SIMPLE_GANG_DRIVEBY && m_pSubTask->GetTaskType() != TASK_SIMPLE_GANG_DRIVEBY) {
                    goto common;
                }
            } else if (type != TASK_SIMPLE_GANG_DRIVEBY && m_pSubTask->GetTaskType() != TASK_SIMPLE_GANG_DRIVEBY) {
                goto common;
            }
            vehicle->SetStatus(STATUS_PHYSICS);
            CriminalField<uint8>(vehicle, 0x3BA) = m_Criminal->GetVehicleIfInOne() ? 53 : 55;
            CriminalField<uint8>(vehicle, 0x3D0) = m_Criminal->GetVehicleIfInOne()
                ? static_cast<uint8>(CriminalField<uint8>(m_Criminal->m_pVehicle, 0x3D0) + 10) : 20;
            CriminalField<float>(vehicle, 0x3C8) = float(CriminalField<uint8>(vehicle, 0x3D0));
            CriminalField<uint8>(vehicle, 0x3B9) = 2;
            CriminalField<CEntity*>(vehicle, 0x41C) = m_Criminal->GetVehicleIfInOne()
                ? static_cast<CEntity*>(m_Criminal->m_pVehicle) : m_Criminal.Get();
        }
    }
common:
    if (m_Criminal && m_Criminal->GetVehicleIfInOne()
        && CriminalField<uint8>(m_Criminal->m_pVehicle, 0x484)
        && distance(m_Criminal->m_pVehicle, ped) < 60.f) {
        auto* vehicle = m_Criminal->m_pVehicle;
        for (size_t i = 0; i < vehicle->m_nMaxPassengers; ++i) {
            if (auto* passenger = vehicle->m_apPassengers[i]; passenger && passenger->bInVehicle
                && !passenger->GetIntelligence()->FindTaskByType(TASK_SIMPLE_GANG_DRIVEBY)) {
                passenger->GetTaskManager().SetTask(new CTaskSimpleGangDriveBy{
                    ped, nullptr, 70.f, 70, eDrivebyStyle::AI_ALL_DIRN, false
                }, static_cast<ePrimaryTasks>(3), false);
            }
        }
    }
    if (type == TASK_NONE || !m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        return original;
    }
    return CreateChaseSubTask(type, ped);
}
