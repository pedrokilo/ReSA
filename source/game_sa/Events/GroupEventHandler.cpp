/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CGroupEventHandler
 * Reconstrucción: COMPLETA — 32/32 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 32/32 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "GroupEventHandler.h"
#include "EventHandler.h"
#include "Ragdoll/IKChainManager.h"

#include "Tasks/TaskTypes/TaskComplexStareAtPed.h"
#include "Tasks/TaskTypes/TaskSimpleNone.h"
#include "Tasks/TaskTypes/TaskComplexLeaveAnyCar.h"
#include "Tasks/TaskTypes/TaskComplexGangJoinRespond.h"
#include "Tasks/TaskTypes/TaskSimpleGoToPoint.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskSimpleCarDrive.h"
#include "Tasks/TaskComplexUseSequence.h"
#include "Tasks/TaskSequences.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.h"
#include "Tasks/TaskTypes/TaskComplexSmartFleeEntity.h"
#include "Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h"
#include "Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h"
#include "Tasks/TaskTypes/TaskGoToVehicleAndLean.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFoot.h"
#include "Tasks/TaskTypes/TaskGangHassleVehicle.h"
#include "Tasks/TaskTypes/TaskGangHasslePed.h"
#include "Tasks/TaskTypes/TaskComplexSignalAtPed.h"
#include "Tasks/TaskTypes/TaskComplexPartnerGreet.h"
#include "Tasks/TaskTypes/TaskComplexPartnerDeal.h"
#include "Tasks/TaskTypes/TaskComplexWalkAlongsidePed.h"

#include "Tasks/Allocators/TaskAllocator.h"
#include "Tasks/Allocators/TaskAllocatorPlayerCommandAttack.h"
#include "Tasks/Allocators/TaskAllocatorKillThreatsBasic.h"
#include "Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.h"
#include "Tasks/Allocators/TaskAllocatorKillThreatsDriveby.h"

#include "Events/EventVehicleDamage.h"
#include "Events/EventGunShot.h"
#include "Events/EventSexyPed.h"
#include "Events/EventAcquaintancePed.h"
#include "Events/EventPlayerCommandToGroup.h"
#include "Events/EventPlayerCommandToGroupAttack.h"
#include "Events/EventPlayerCommandToGroupGather.h"
#include "Events/EventGunAimedAt.h"
#include "Events/EventDraggedOutCar.h"
#include "Events/EventDanger.h"
#include "Events/EventDamage.h"
#include "Events/EventLeanOnVehicle.h"
#include "Events/EventSeenCop.h"
#include "Events/EventNewGangMember.h"
#include "Events/EventLeaderEntryExit.h"
#include "Events/EventLeaderExitedCarAsDriver.h"

void CGroupEventHandler::InjectHooks() {
    RH_ScopedClass(CGroupEventHandler);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(IsKillTaskAppropriate, 0x5F7A60);
    RH_ScopedInstall(ComputeWalkAlongsideResponse, 0x5FA910);
    RH_ScopedInstall(ComputeStareResponse, 0x5F9BD0);
    RH_ScopedInstall(ComputeResponseVehicleDamage, 0x5FC070);
    RH_ScopedInstall(ComputeResponseShotFired, 0x5FBDF0);
    RH_ScopedInstall(ComputeResponseSexyPed, 0x5FB390);
    RH_ScopedInstall(ComputeResponseSeenCop, 0x5FBCB0);
    RH_ScopedInstall(ComputeResponsePlayerCommand, 0x5FB470);
    RH_ScopedInstall(ComputeResponsePedThreat, 0x5FBB90);
    RH_ScopedInstall(ComputeResponsePedFriend, 0x5FB2D0);
    RH_ScopedInstall(ComputeResponseNewGangMember, 0x5F9840);
    RH_ScopedInstall(ComputeResponseLeaderExitedCar, 0x5F90A0);
    RH_ScopedInstall(ComputeResponsLeaderQuitEnteringCar, 0x5F9530);
    RH_ScopedInstall(ComputeResponseLeaderEnteredCar, 0x5F8900);
    RH_ScopedInstall(ComputeResponseLeaderEnterExit, 0x5F9710);
    RH_ScopedInstall(ComputeResponseGunAimedAt, 0x5FBD10);
    RH_ScopedInstall(ComputeResponseGather, 0x5F99F0);
    RH_ScopedInstall(ComputeResponseDraggedOutCar, 0x5FBE70);
    RH_ScopedInstall(ComputeResponseDanger, 0x5FB540);
    RH_ScopedInstall(ComputeResponseDamage, 0x5FBF50);
    RH_ScopedInstall(ComputeMemberResponses, 0x5FAA50);
    RH_ScopedInstall(ComputeLeanOnVehicleResponse, 0x5F9B20);
    RH_ScopedInstall(ComputeKillThreatsBasicResponse, 0x5FB590);
    RH_ScopedInstall(ComputeKillPlayerBasicResponse, 0x5FB670);
    RH_ScopedInstall(ComputeHassleThreatResponse, 0x5F9D50);
    RH_ScopedInstall(ComputeHassleSexyPedResponse, 0x5FA020);
    RH_ScopedInstall(ComputeHandSignalResponse, 0x5FA820);
    RH_ScopedInstall(ComputeGreetResponse, 0x5FA550);
    RH_ScopedInstall(ComputeFleePedResponse, 0x5FA130);
    RH_ScopedInstall(ComputeEventResponseTasks, 0x5FC200);
    RH_ScopedInstall(ComputeDrivebyResponse, 0x5F7A00);
    RH_ScopedInstall(ComputeDoDealResponse, 0x5FA290);
}

bool IsPedInPlayersGroup(CPedGroup* pg, CPed* p) {
    const auto playerGrp = &FindPlayerPed(-1)->GetPlayerGroup();
    if (pg == playerGrp) {
        return true;
    }
    if (playerGrp->GetMembership().IsMember(p)) {
        return true;
    }
    return false;
}

// Como el original (0x5FBD3F..0x5FBD87 y 0x5FBF80..0x5FBFCB): el ajuste solo ocurre si el grupo NO es de mision
// (`test al,al; jne` salta al switch) y la amenaza (el peaton origen del evento, no `originator`) es el jugador y no es amigo del lider.
void MaybeAdjustTaskOfGroupThreatEvent(const CEventEditableResponse& e, CPedGroup* pg, CPed* srcPed) {
    if (!pg->m_bIsMissionGroup && srcPed->IsPlayer()) {
        if (const auto l = pg->GetMembership().GetLeader()) {
            if (!l->GetActiveWeapon().IsTypeMelee() && !l->GetIntelligence()->IsFriendlyWith(*srcPed)) {
                const_cast<CEventEditableResponse*>(&e)->m_TaskId = TASK_GROUP_KILL_THREATS_BASIC; // nice R*
            }
        }
    }
}

// 0x5F7A60
bool CGroupEventHandler::IsKillTaskAppropriate(CPedGroup* g, CPed* threat) {
    if (g->m_bIsMissionGroup || threat->GetActiveWeapon().IsTypeMelee()) {
        return true;
    }
    for (auto* const m : g->GetMembership().GetMembers()) {
        if (!m->GetActiveWeapon().IsTypeMelee()) {
            return true;
        }
    }
    return false;
}

// 0x5FA910
CTaskAllocator* CGroupEventHandler::ComputeWalkAlongsideResponse(CPedGroup* pg, CPed* ped1, CPed* ped2) {
    // Como el original (0x5FA910): sin llamadores conocidos, pero el EXE contiene el cuerpo. `ped2` no se lee.
    if (!ped1) {
        return nullptr;
    }
    // Seguidor (7 ranuras, no el lider) mas cercano a ped1: minimo inicial 999999.0f (0x497423F0), comparacion estricta `<`
    // (fcom + test ah,5 + jp), suma x*x + y*y + z*z.
    auto* const members = reinterpret_cast<CPed* const*>(reinterpret_cast<uint8*>(pg) + 0xC);
    CPed* closest{};
    float closestDistSq = std::bit_cast<float>(0x497423F0u);
    for (int32 i = 0; i < 7; ++i) {
        auto* const m = members[i];
        if (!m) {
            continue;
        }
        const auto d = ped1->GetPosition() - m->GetPosition();
        const float distSq = d.x * d.x + d.y * d.y + d.z * d.z;
        if (distSq < closestDistSq) {
            closestDistSq = distSq;
            closest       = m;
        }
    }
    if (!closest) {
        return nullptr;
    }
    pg->GetIntelligence().SetEventResponseTask(closest, CTaskComplexWalkAlongsidePed{ped1, 10.f});
    pg->m_pPed = ped1; // Como el original (0x5FAA1C): escribe el peaton en el primer campo del grupo
    return nullptr;
}

// 0x5F9BD0
CTaskAllocator* CGroupEventHandler::ComputeStareResponse(CPedGroup* pg, CPed* stareAt, CPed* originatorPed, int32 timeout, int32 timeoutBias) {
    if (!stareAt) {
        return nullptr;
    }
    if (!pg->GetMembership().GetLeader()) {
        return nullptr;
    }
    // Como el original (0x5FBC0F..0x5FBC57): distancia 3D al cuadrado y salida solo si es MAYOR que 64.0f (0x859A44); con NaN sigue
    if ((stareAt->GetPosition() - pg->GetMembership().GetLeader()->GetPosition()).SquaredMagnitude() > sq(8.f)) {
        return nullptr;
    }
    for (auto* const m : pg->GetMembership().GetMembers()) {
        // Como el original: rand*(1/32768)*(2*bias) como en 0x407180 (GetRandomIntInRange), no la plantilla entera con 1/32767
        pg->GetIntelligence().SetEventResponseTask(
            m,
            CTaskComplexStareAtPed{
                pg,
                stareAt,
                timeout + CGeneral::GetRandomIntInRange(-timeoutBias, timeoutBias)
            }
        );
    }
    return nullptr;
}

// 0x5FC070
CTaskAllocator* CGroupEventHandler::ComputeResponseVehicleDamage(const CEventVehicleDamage& e, CPedGroup* pg, CPed* originator) {
    if (!e.m_attacker || !e.m_attacker->GetIsTypePed()) {
        return nullptr;
    }
    const auto threat = e.m_attacker->AsPed();
    switch (e.m_TaskId) {
    case TASK_GROUP_KILL_THREATS_BASIC:  return ComputeKillThreatsBasicResponse(pg, threat, originator, true);
    case TASK_GROUP_KILL_PLAYER_BASIC:   return ComputeKillPlayerBasicResponse(pg, threat, originator, true); 
    case TASK_GROUP_FLEE_THREAT:         return ComputeFleePedResponse(pg, threat, originator, true);         
    case TASK_GROUP_USE_MEMBER_DECISION: return ComputeMemberResponses(e, pg, originator);                      
    case TASK_GROUP_DRIVEBY:             return ComputeDrivebyResponse(pg, threat, originator);
    }
    return nullptr;
}

// 0x5FBDF0
CTaskAllocator* CGroupEventHandler::ComputeResponseShotFired(const CEventGunShot& e, CPedGroup* pg, CPed* originator) {
    if (!e.m_firedBy || !e.m_firedBy->GetIsTypePed()) {
        return nullptr;
    }
    const auto threat = e.m_firedBy->AsPed();
    switch (e.m_TaskId) {
    case TASK_GROUP_KILL_THREATS_BASIC:  return ComputeKillThreatsBasicResponse(pg, threat, originator, false);
    case TASK_GROUP_FLEE_THREAT:         return ComputeFleePedResponse(pg, threat, originator, false);
    case TASK_GROUP_USE_MEMBER_DECISION: return ComputeMemberResponses(e, pg, originator);
    }
    return nullptr;
}

// 0x5FB390
CTaskAllocator* CGroupEventHandler::ComputeResponseSexyPed(const CEventSexyPed& e, CPedGroup* pg, CPed* originator) {
    if (!e.m_SexyPed) {
        return nullptr;
    }
    if (pg->GetMembership().IsMember(e.m_SexyPed)) {
        return nullptr;
    }
    if (IsPedInPlayersGroup(pg, e.m_SexyPed)) {
        return nullptr;
    }
    switch (e.m_TaskId) {
    case TASK_GROUP_STARE_AT_PED:    return ComputeStareResponse(pg, e.m_SexyPed, originator, CGeneral::GetRandomIntInRange(3000, 5000), 1000); // 0x407180
    case TASK_GROUP_HASSLE_SEXY_PED: return ComputeHassleSexyPedResponse(pg, e.m_SexyPed, originator);
    }
    return nullptr;
}

// 0x5FBCB0
CTaskAllocator* CGroupEventHandler::ComputeResponseSeenCop(const CEventSeenCop& e, CPedGroup* pg, CPed* originator) {
    switch (e.m_TaskId) {
    case TASK_GROUP_KILL_THREATS_BASIC: return ComputeKillThreatsBasicResponse(pg, e.m_AcquaintancePed, originator, false);
    case TASK_GROUP_FLEE_THREAT:        return ComputeFleePedResponse(pg, e.m_AcquaintancePed, originator, false);
    case TASK_GROUP_HAND_SIGNAL:        return ComputeHandSignalResponse(pg, e.m_AcquaintancePed, originator);
    }
    return nullptr;
}

// 0x5FB470
CTaskAllocator* CGroupEventHandler::ComputeResponsePlayerCommand(const CEventPlayerCommandToGroup& e, CPedGroup* pg, CPed* originator) {
    switch (e.m_command) {
    case ePlayerGroupCommand::PLAYER_GROUP_COMMAND_GATHER:
        return ComputeResponseGather(static_cast<const CEventPlayerCommandToGroupGather&>(e), pg, originator);
    case ePlayerGroupCommand::PLAYER_GROUP_COMMAND_ATTACK:
        if (e.m_target) {
            return new CTaskAllocatorPlayerCommandAttack{
                e.m_target,
                e.m_target->GetGroupId(),
                e.m_target->m_nPedType
            };
        }
        break;
    }
    return nullptr;
}

// 0x5FBB90
CTaskAllocator* CGroupEventHandler::ComputeResponsePedThreat(const CEventAcquaintancePed& e, CPedGroup* pg, CPed* originator) {
    if (!e.m_AcquaintancePed) {
        return nullptr;
    }
    if (pg->GetMembership().IsMember(e.m_AcquaintancePed)) {
        return nullptr;
    }
    switch (e.m_TaskId) {
    case TASK_GROUP_KILL_THREATS_BASIC:    return ComputeKillThreatsBasicResponse(pg, e.m_AcquaintancePed, originator, false);
    case TASK_GROUP_STARE_AT_PED:          return ComputeStareResponse(pg, e.m_AcquaintancePed, originator, 99'999'999, false); // 1.15740739583 days
    case TASK_GROUP_FLEE_THREAT:           return ComputeFleePedResponse(pg, e.m_AcquaintancePed, originator, false);
    case TASK_GROUP_HASSLE_THREAT:         return ComputeHassleThreatResponse(pg, e.m_AcquaintancePed, originator, true);
    case TASK_GROUP_USE_MEMBER_DECISION:   return ComputeMemberResponses(e, pg, originator);
    case TASK_GROUP_DRIVEBY:               return ComputeDrivebyResponse(pg, e.m_AcquaintancePed, originator);
    case TASK_GROUP_HASSLE_THREAT_PASSIVE: return ComputeHassleThreatResponse(pg, e.m_AcquaintancePed, originator, false);
    }
    return nullptr;
}

// 0x5FB2D0
CTaskAllocator* CGroupEventHandler::ComputeResponsePedFriend(const CEventAcquaintancePed& e, CPedGroup* pg, CPed* originator) {
    if (!e.m_AcquaintancePed) {
        return nullptr;
    }
    // Como el original (0x5FB2E9..0x5FB348): grupo del jugador, miembro del grupo del jugador, `pg->m_pPed` y miembro de `pg`
    if (pg == &FindPlayerPed(-1)->GetPlayerGroup()) {
        return nullptr;
    }
    if (FindPlayerPed(-1)->GetPlayerGroup().GetMembership().IsMember(e.m_AcquaintancePed)) {
        return nullptr;
    }
    if (e.m_AcquaintancePed == pg->m_pPed) {
        return nullptr;
    }
    if (pg->GetMembership().IsMember(e.m_AcquaintancePed)) {
        return nullptr;
    }
    switch (e.m_TaskId) {
    case TASK_GROUP_PARTNER_DEAL:  return ComputeDoDealResponse(pg, e.m_AcquaintancePed, originator);
    case TASK_GROUP_PARTNER_GREET: return ComputeGreetResponse(pg, e.m_AcquaintancePed, originator);
    }
    return nullptr;
}

// 0x5F9840
CTaskAllocator* CGroupEventHandler::ComputeResponseNewGangMember(const CEventNewGangMember& e, CPedGroup* pg, CPed* originator) {
    // Como el original (0x5F9880..0x5F9890): continua SOLO si el peaton ES miembro (7 ranuras o lider); si no, devuelve null
    if (!e.m_member || !pg->GetMembership().IsMember(e.m_member)) {
        return nullptr;
    }
    CTaskComplexSequence tseq{
        new CTaskComplexLeaveAnyCar{0, false, true},
        new CTaskComplexGangJoinRespond{true}
    };
    if (pg->GetMembership().GetLeader() == FindPlayerPed(0)) { // Como el original: FindPlayerPed(0) (push 0), no -1
        tseq.AddTask(new CTaskSimpleGoToPoint{
            eMoveState::PEDMOVE_RUN,
            FindPlayerPed(0)->GetPosition(),
            3.f,
            true
        });
    }
    pg->GetIntelligence().SetEventResponseTask(e.m_member, tseq);
    return nullptr;
}

// 0x5F90A0
CTaskAllocator* CGroupEventHandler::ComputeResponseLeaderExitedCar(const CEventEditableResponse&, CPedGroup* pg, CPed* originator) {
    // Como el original (0x5F90CB..0x5F9506): 7 ranuras de seguidores con el retardo `ranura * 500` (ebx += 0x1F4), sin comprimir huecos
    auto* const members = reinterpret_cast<CPed* const*>(reinterpret_cast<uint8*>(pg) + 0xC);
    for (int32 i = 0; i < 7; ++i) {
        auto* const m = members[i];
        if (!m) {
            continue;
        }
        const auto SetResponseTask = [&](const auto& task) {
            pg->GetIntelligence().SetEventResponseTask(m, task);
        };
        CVehicle* mveh = m->m_pVehicle;
        // Como el original (0x5F90EE..0x5F9144): si ya esta en el vehiculo del lider se USA ese vehiculo (salta a 0x5F9144),
        // no se omite; si no, se busca el vehiculo en la tarea de subir como pasajero (espera, 0x2CE; luego 0x2BC)
        if (!(mveh && m->bInVehicle && mveh == pg->GetMembership().GetLeader()->m_pVehicle)) {
            if (const auto t = m->GetTaskManager().Find<CTaskComplexEnterCarAsPassengerWait>(false); !t || !(mveh = t->GetCar())) {
                if (const auto t2 = m->GetTaskManager().Find<CTaskComplexEnterCarAsPassenger>(false); !t2 || !(mveh = t2->GetTargetCar())) {
                    continue;
                }
            }
        }
        const auto isVehOnFire = mveh->m_pFireParticle && mveh->m_pFireParticle->GetPlayStatus() == eFxSystemPlayStatus::FX_PLAYING;
        if (notsa::contains<int>({ 15, 16 }, mveh->m_pHandlingData->m_nAnimGroup)) { // TODO: Enums
            if (isVehOnFire) { // 0x5F9375
                SetResponseTask(CTaskComplexSequence{
                    new CTaskComplexLeaveCarAsPassengerWait{mveh},
                    new CTaskComplexSmartFleeEntity{
                        mveh,
                        false,
                        15.f,
                        1'000'000,
                        1000,
                        fEntityPosChangeThreshold
                    }
                });
            } else { // 0x5F947E
                SetResponseTask(CTaskComplexLeaveCarAsPassengerWait{
                    mveh
                });
            }
        } else if (isVehOnFire) { // 0x5F9189
            const int32 delay = CGeneral::GetRandomIntInRange(-250, 250) + (i + 1) * 500; // 0x407180 antes de crear la secuencia
            SetResponseTask(CTaskComplexSequence{
                new CTaskComplexLeaveCar{
                    mveh,
                    eTargetDoor::TARGET_DOOR_FRONT_LEFT,
                    delay,
                    false,
                    false
                },
                new CTaskComplexSmartFleeEntity{
                    mveh,
                    false,
                    15.f,
                    1'000'000,
                    1'000,
                    fEntityPosChangeThreshold
                }
            });
        } else { // 0x5F92AA: rand inline (rand&0xFFFF)*(1/32768)*-500.0f truncado; retardo = ranura*500 - r + 250
            const int32 r = static_cast<int32>((float(CGeneral::GetRandomNumber()) * std::bit_cast<float>(0x38000000u)) * -500.f);
            SetResponseTask(CTaskComplexLeaveCar{
                mveh,
                eTargetDoor::TARGET_DOOR_FRONT_LEFT,
                i * 500 - r + 250,
                false,
                false
            });
        }
    }
    return nullptr;
}

// 0x5F8900
CTaskAllocator* CGroupEventHandler::ComputeResponseLeaderEnteredCar(const CEvent& e, CPedGroup* pg, CPed* originator) {
    auto* const leader = pg->GetMembership().GetLeader();
    auto* const vehicle = *reinterpret_cast<CVehicle* const*>(reinterpret_cast<const uint8*>(&e) + 0x14);
    if (!leader || !vehicle) return nullptr;
    const auto eventTask = *reinterpret_cast<const int16*>(reinterpret_cast<const uint8*>(&e) + 0xE);
    const auto sequenceId = *reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(pg) + 0x2CC);
    auto& intelligence = pg->GetIntelligence();
    auto* members = reinterpret_cast<CPed**>(reinterpret_cast<uint8*>(pg) + 0xC);
    std::array<CPed*, 7> ordered{};
    size_t count = 0;
    for (int32 phase = 0; phase < 2; ++phase) {
        for (int32 i = 0; i < 7; ++i) {
            auto* member = members[i];
            if (member && (static_cast<int32>(member->GetCreatedBy()) == 2) == (phase == 0)) ordered[count++] = member;
        }
    }
    const auto setInline = [&](CPed* ped, const CTask& task, auto& pairs, bool incomingFirst = false) {
        for (auto& pair : pairs) {
            if (pair.Ped != ped) continue;
            if (pair.Task) {
                const auto first = incomingFirst ? task.GetTaskType() : pair.Task->GetTaskType();
                const auto second = incomingFirst ? pair.Task->GetTaskType() : task.GetTaskType();
                if (first == second) return;
                auto* old = pair.Task;
                pair.Task = task.Clone();
                pair.Slot = TASK_SECONDARY_INVALID;
                delete old;
            } else {
                pair.Task = task.Clone();
                pair.Slot = TASK_SECONDARY_INVALID;
            }
            return;
        }
    };
    const auto addDrive = [&](CPed* ped, bool inlineSet) {
        if (ped->bHasGroupDriveTask) return;
        ped->bHasGroupDriveTask = true;
        CTaskComplexSequence sequence;
        sequence.AddTask(new CTaskSimpleCarDrive{vehicle, nullptr, true});
        for (auto& pair : intelligence.GetDefaultPedTaskPairs()) {
            if (pair.Ped != ped) continue;
            if (pair.Task) sequence.AddTask(pair.Task->Clone());
            break;
        }
        if (inlineSet) setInline(ped, sequence, intelligence.GetDefaultPedTaskPairs(), true);
        else intelligence.SetTask(ped, sequence, intelligence.GetDefaultPedTaskPairs());
    };
    auto* previous = leader;
    int32 assigned = 0;
    for (auto* ped : ordered) {
        if (!ped) continue;
        if (assigned >= vehicle->m_nMaxPassengers) {
            CTaskComplexLeaveAnyCar task{0, true, false};
            setInline(ped, task, intelligence.GetPedTaskPairs());
            continue;
        }
        bool frontSeat = false;
        int32 followers = 0;
        for (int32 i = 0; i < 7; ++i) followers += members[i] != nullptr;
        if (followers == 1 && vehicle->m_nVehicleType != VEHICLE_TYPE_BIKE && vehicle->m_nVehicleSubType != VEHICLE_TYPE_QUAD) {
            frontSeat = true;
            auto* driver = vehicle->m_pDriver;
            if (driver && driver != ped) {
                for (int32 i = 0; i < 7; ++i) {
                    if (members[i] == driver) { frontSeat = false; break; }
                }
                if (leader == driver || driver->bDontDragMeOutCar) frontSeat = false;
            }
        }
        if (eventTask == TASK_GROUP_ENTER_CAR_AND_PERFORM_SEQUENCE && sequenceId >= 0 && sequenceId < 64 && CTaskSequences::ms_taskSequence[sequenceId].IsActive()) {
            CTaskComplexSequence sequence;
            sequence.AddTask(new CTaskComplexEnterCarAsPassengerWait{vehicle, previous, frontSeat, static_cast<eMoveState>(6)});
            sequence.AddTask(new CTaskComplexUseSequence{sequenceId});
            intelligence.SetTask(ped, sequence, intelligence.GetPedTaskPairs());
        } else {
            CTaskComplexEnterCarAsPassengerWait task{vehicle, previous, frontSeat, static_cast<eMoveState>(6)};
            setInline(ped, task, intelligence.GetPedTaskPairs());
            addDrive(ped, true);
        }
        ++assigned;
        previous = ped;
    }
    if (!leader->IsPlayer()) addDrive(leader, false);
    return nullptr;
}

// 0x5F9710
CTaskAllocator* CGroupEventHandler::ComputeResponseLeaderEnterExit(const CEventLeaderEntryExit& e, CPedGroup* pg, CPed* originator) {
    const auto pgms   = &pg->GetMembership();
    const auto leader = pgms->GetLeader();
    if (!leader) {
        return nullptr;
    }
    for (auto* const m : pgms->GetFollowers()) {
        if (m->bInVehicle) {
            continue;
        }
        CTaskSimpleWaitUntilLeaderAreaCodesMatch task{leader};
        if (task.ProcessPed(m)) {
            m->SetUsesCollision(true);
        } else {
            pg->GetIntelligence().SetEventResponseTask(m, task);
        }
    }
    return nullptr;
}

// 0x5FBD10
CTaskAllocator* CGroupEventHandler::ComputeResponseGunAimedAt(const CEventGunAimedAt& e, CPedGroup* pg, CPed* originator) {
    const auto src = e.GetSourceEntity();
    if (!src || !src->GetIsTypePed()) {
        return nullptr;
    }
    const auto srcPed = src->AsPed();
    MaybeAdjustTaskOfGroupThreatEvent(e, pg, srcPed);
    switch (e.m_TaskId) { // Como el original (0x5FBD8D): aqui se vuelve a leer el id tras el ajuste
    case TASK_GROUP_KILL_THREATS_BASIC:  return ComputeKillThreatsBasicResponse(pg, srcPed, originator, false);
    case TASK_GROUP_FLEE_THREAT:         return ComputeFleePedResponse(pg, srcPed, originator, false);
    case TASK_GROUP_USE_MEMBER_DECISION: return ComputeMemberResponses(e, pg, originator);
    }
    return nullptr;
}

// 0x5F99F0
CTaskAllocator* CGroupEventHandler::ComputeResponseGather(const CEventPlayerCommandToGroupGather& e, CPedGroup* pg, CPed* originator) {
    for (auto* const m : pg->GetMembership().GetFollowers()) {
        pg->GetIntelligence().SetEventResponseTask(
            m,
            CTaskComplexSeekEntity<CEntitySeekPosCalculatorStandard>{
                originator,
                50'000,
                1'000,
                7.5f,
                2.f,
                2.f,
                true,
                true,
                CEntitySeekPosCalculatorStandard{}
            }
        );
    }
    return nullptr;
}

// 0x5FBE70
CTaskAllocator* CGroupEventHandler::ComputeResponseDraggedOutCar(const CEventDraggedOutCar& e, CPedGroup* pg, CPed* originator) {
    if (!e.m_CarJacker || !e.m_CarJacker->GetIsTypePed()) { // Como el original (0x5FBE80..0x5FBE89): devuelve null si no es un peaton
        return nullptr;
    }
    switch (e.m_TaskId) {
    case TASK_GROUP_KILL_THREATS_BASIC:
        return e.m_CarJacker->IsPlayer() && originator && originator->GetIntelligence()->Respects(e.m_CarJacker) && !pg->m_bIsMissionGroup
            ? ComputeFleePedResponse(pg, e.m_CarJacker, originator, false)
            : ComputeKillThreatsBasicResponse(pg, e.m_CarJacker, originator, false);
    case TASK_GROUP_FLEE_THREAT:         return ComputeFleePedResponse(pg, e.m_CarJacker, originator, false);
    case TASK_GROUP_USE_MEMBER_DECISION: return ComputeMemberResponses(e, pg, originator);
    case TASK_GROUP_EXIT_CAR:            return ComputeResponseLeaderExitedCar(static_cast<const CEventEditableResponse&>(e), pg, originator);
    }
    return nullptr;
}

// 0x5FB540
CTaskAllocator* CGroupEventHandler::ComputeResponseDanger(const CEventDanger& e, CPedGroup* pg, CPed* originator) {
    const auto esrc = e.GetSourceEntity();
    if (!esrc || !esrc->GetIsTypePed()) {
        return nullptr;
    }
    switch (e.m_TaskId) {
    case TASK_GROUP_FLEE_THREAT: return ComputeFleePedResponse(pg, esrc->AsPed(), originator, false);
    }
    return nullptr;
}

// 0x5FBF50
CTaskAllocator* CGroupEventHandler::ComputeResponseDamage(const CEventDamage& e, CPedGroup* pg, CPed* originator) {
    const auto src = e.m_pSourceEntity; // Como el original (0x5FBF5B): lee el campo, no llama a GetSourceEntity (que devuelve el conductor)
    if (!src || !src->GetIsTypePed()) {
        return nullptr;
    }
    const auto srcPed = src->AsPed();
    const eTaskType taskId = e.m_TaskId; // Como el original (0x5FBF55/0x5FBF62): guarda el id ANTES del ajuste y el switch usa ese valor
    MaybeAdjustTaskOfGroupThreatEvent(e, pg, srcPed);
    switch (taskId) {
    case TASK_GROUP_KILL_THREATS_BASIC:  return ComputeKillThreatsBasicResponse(pg, srcPed, originator,  true);
    case TASK_GROUP_KILL_PLAYER_BASIC:   return ComputeKillPlayerBasicResponse(pg, srcPed, originator, true);
    case TASK_GROUP_FLEE_THREAT:         return ComputeFleePedResponse(pg, srcPed, originator, true);
    case TASK_GROUP_USE_MEMBER_DECISION: return ComputeMemberResponses(e, pg, originator);
    }
    return nullptr;
}

// 0x5F9530
CTaskAllocator* CGroupEventHandler::ComputeResponsLeaderQuitEnteringCar(const CEvent& e, CPedGroup* pg, CPed* originator) {
    auto& intelligence = pg->GetIntelligence();
    // Como el original: conservar los huecos vacios al calcular el retardo por indice.
    auto* members = reinterpret_cast<CPed**>(reinterpret_cast<uint8*>(pg) + 0xC);
    for (int32 i = 0; i < 7; ++i) {
        auto* ped = members[i];
        if (!ped) continue;
        CVehicle* vehicle = ped->m_pVehicle;
        bool sensible = ped->bInVehicle && vehicle;
        int32 delay = i * 500;
        if (!sensible) {
            const auto getCar = [](CTask* task) -> CVehicle* {
                return task ? *reinterpret_cast<CVehicle**>(reinterpret_cast<uint8*>(task) + 0xC) : nullptr;
            };
            vehicle = getCar(ped->GetIntelligence()->FindTaskByType(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_WAIT));
            if (!vehicle) vehicle = getCar(ped->GetIntelligence()->FindTaskByType(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER));
            if (!vehicle) continue;
            const float randomDelay = (float(CGeneral::GetRandomNumber()) * std::bit_cast<float>(0x38000000u)) * -500.0f;
            delay = delay - static_cast<int32>(randomDelay) + 250;
        }
        CTaskComplexLeaveCar task{vehicle, 0, delay, sensible, false};
        intelligence.SetTask(ped, task, intelligence.GetPedTaskPairs());
    }
    return nullptr;
}

// 0x5FAA50
CTaskAllocator* CGroupEventHandler::ComputeMemberResponses(const CEventEditableResponse& e, CPedGroup* pg, CPed* originator) {
    const std::unique_ptr<CEventEditableResponse> clone{ static_cast<CEventEditableResponse*>(const_cast<CEventEditableResponse&>(e).Clone()) };
    // Como el original (0x5FAA7B..0x5FAB2F): 8 ranuras (el lider incluido) y, si la respuesta es editable, se recalcula el tipo
    // (0x4B56C0) Y ADEMAS se calcula/asigna la tarea (no es un `else`).
    for (auto* const m : pg->GetMembership().GetMembers()) {
        if (!m->IsAlive()) {
            continue;
        }
        if (clone->HasEditableResponse()) {
            clone->m_TaskId = TASK_NONE;
            clone->ComputeResponseTaskType(m, true);
        }
        if (const auto rt = std::unique_ptr<CTask>(CEventHandler::ComputeEventResponseTask(*m, *clone))) {
            pg->GetIntelligence().SetEventResponseTask(m, *rt);
        }
    }
    return nullptr;
}

// 0x5F9B20
CTaskAllocator* CGroupEventHandler::ComputeLeanOnVehicleResponse(const CEventLeanOnVehicle& e, CPedGroup* pg, CPed* originator) {
    const auto leader = pg->GetMembership().GetLeader();
    if (!leader) {
        return nullptr;
    }
    pg->GetIntelligence().SetEventResponseTask(
        leader,
        CTaskGoToVehicleAndLean{e.m_vehicle, e.m_leanAnimDurationInMs}
    );
    return nullptr;
}

// 0x5FB590
CTaskAllocator* CGroupEventHandler::ComputeKillThreatsBasicResponse(CPedGroup* pg, CPed* threat, CPed* originator, bool bDamageOriginator) {
    if (!IsKillTaskAppropriate(pg, threat)) {
        return ComputeFleePedResponse(pg, threat, originator, false);
    }
    if (pg->m_bIsMissionGroup) {
        return new CTaskAllocatorKillThreatsBasic{threat};
    }
    return new CTaskAllocatorKillThreatsBasicRandomGroup{threat};
}

// 0x5FB670
CTaskAllocator* CGroupEventHandler::ComputeKillPlayerBasicResponse(CPedGroup* pg, CPed* threat, CPed* originator, bool bDamageOriginator) {
    if (!threat) {
        return nullptr;
    }
    if (!IsKillTaskAppropriate(pg, threat)) {
        return ComputeFleePedResponse(pg, threat, originator, false);
    }
    for (auto* const m : pg->GetMembership().GetMembers()) {
        pg->GetIntelligence().SetEventResponseTask(
            m,
            CTaskComplexKillPedOnFoot{ threat }
        );
    }
    return nullptr;
}

// 0x5F9D50
CTaskAllocator* CGroupEventHandler::ComputeHassleThreatResponse(CPedGroup* pg, CPed* threat, CPed* originator, bool bBeAggressive) {
    if (!threat) {
        return nullptr;
    }
    const auto leader = pg->GetMembership().GetLeader();
    if (!leader) {
        return nullptr;
    }
    if ((leader->GetPosition() - threat->GetPosition()).SquaredMagnitude() > sq(12.f)) { // Como el original (0x5F9DCF/0x5F9DE0): sale solo si es MAYOR que 144.0f (0x86C824)
        return nullptr;
    }
    for (auto* const m : pg->GetMembership().GetMembers()) {
        const auto SetTask = [&](const auto& t) {
            pg->GetIntelligence().SetEventResponseTask(m, t);
        };
        if (threat->IsInVehicle() && threat->m_pVehicle->IsSubAutomobile()) {
            if (!bBeAggressive) {
                if (threat->m_pVehicle->GetSpareHasslePosId() != -1) {
                    SetTask(CTaskGangHassleVehicle{
                        threat->m_pVehicle,
                        -1,
                        0,
                        0.25f,
                        0.65f
                    });
                }
            }
        } else {
            SetTask(CTaskGangHasslePed{
                threat,
                bBeAggressive ? 2 : 1, // TODO: Enum
                12'000,
                20'000
            });
        }
    }
    return nullptr;
}

// 0x5FA020
CTaskAllocator* CGroupEventHandler::ComputeHassleSexyPedResponse(CPedGroup* pg, CPed* sexyPed, CPed* originator) {
    if (!sexyPed) {
        return nullptr;
    }
    // Como el original (0x5FA065..0x5FA095): por cada una de las 7 ranuras (aunque este vacia) se consume un rand() y se exige
    // int(rand&0xFFFF * (1/32768) * 100.0f) > 75 (jle 0x4B); despues se mira si hay peaton.
    auto* const members = reinterpret_cast<CPed* const*>(reinterpret_cast<uint8*>(pg) + 0xC);
    for (int32 i = 0; i < 7; ++i) {
        const int32 roll = static_cast<int32>((float(CGeneral::GetRandomNumber()) * std::bit_cast<float>(0x38000000u)) * 100.f);
        if (roll <= 75) {
            continue;
        }
        auto* const m = members[i];
        if (!m) {
            continue;
        }
        pg->GetIntelligence().SetEventResponseTask(
            m,
            CTaskGangHasslePed{sexyPed, 0, 8'000, 12'000}
        );
    }
    return nullptr;
}

// 0x5FA820
CTaskAllocator* CGroupEventHandler::ComputeHandSignalResponse(CPedGroup* pg, CPed* signalAt, CPed* originator) {
    if (!signalAt) {
        return nullptr;
    }
    for (auto* const m : pg->GetMembership().GetMembers()) {
        if (m->IsPlayer()) {
            continue;
        }
        pg->GetIntelligence().SetEventResponseTask(
            m,
            CTaskComplexSignalAtPed{signalAt}
        );
    }
    return nullptr;
}

namespace {
// Como el original (0x5FA2D9..0x5FA3D3 y 0x5FA5A0..0x5FA694, ComputeDoDealResponse/ComputeGreetResponse comparten el prefijo):
// devuelve el seguidor mas cercano, o null si no se debe responder.
CPed* PickPartnerFollower(CPedGroup* pg, CPed* other) {
    if (!other->GetTaskManager().Has<TASK_COMPLEX_WANDER>()) { // FindActiveTaskByType(0x390)
        return nullptr;
    }
    const auto leader = pg->GetMembership().GetLeader();
    if (!leader) {
        return nullptr;
    }
    if (leader->m_nMoveState != PEDMOVE_STILL) { // cmp dword ptr [leader+0x534], 1
        return nullptr;
    }
    if (other->m_nPedType == PED_TYPE_PLAYER1 || other->m_nPedType == PED_TYPE_PLAYER2) { // test eax,eax / cmp eax,1 sobre +0x598
        return nullptr;
    }
    // Solo los 7 seguidores (no el lider); minimo inicial 999999.0f (0x497423F0); comparacion estricta `<` (fcom + test ah,5 + jp);
    // suma en el orden del EXE: (z*z + y*y) + x*x.
    auto* const members = reinterpret_cast<CPed* const*>(reinterpret_cast<uint8*>(pg) + 0xC);
    CPed* closest{};
    float closestDistSq = std::bit_cast<float>(0x497423F0u);
    for (int32 i = 0; i < 7; ++i) {
        auto* const m = members[i];
        if (!m) {
            continue;
        }
        const auto d = other->GetPosition() - m->GetPosition();
        const float distSq = d.z * d.z + d.y * d.y + d.x * d.x;
        if (distSq < closestDistSq) {
            closestDistSq = distSq;
            closest       = m;
        }
    }
    if (!closest) {
        return nullptr;
    }
    // fcomp 16.0f (0x8599D0) + jnp = sale si es menor; fcomp 100.0f (0x858628) + je = sale si es mayor; con NaN sigue
    if (closestDistSq < 16.f || closestDistSq > 100.f) {
        return nullptr;
    }
    return closest;
}

void PartnerLookAts(CPedGroup* pg, CPed* closest, const char* purpose) {
    for (auto* const m : pg->GetMembership().GetMembers()) {
        if (m == closest) {
            continue;
        }
        g_ikChainMan.LookAt(
            purpose,
            m,
            closest,
            CGeneral::GetRandomIntInRange(3000, 6000), // 0x407180
            BONE_HEAD,
            nullptr,
            true,
            0.25f,
            500,
            3,
            0
        );
    }
}
}

// 0x5FA550
CTaskAllocator* CGroupEventHandler::ComputeGreetResponse(CPedGroup* pg, CPed* toGreet, CPed* originator) {
    if (!toGreet) {
        return nullptr;
    }
    const auto closestToGreeter = PickPartnerFollower(pg, toGreet);
    if (!closestToGreeter) {
        return nullptr;
    }
    const auto partnerType = CGeneral::GetRandomIntInRange(0, 6); // 0x407180(0, 6)
    // Como el original: la tarea es una variable local que vive hasta el final de la funcion (su destructor limpia la referencia
    // de toGreet despues del evento), no un temporal destruido al terminar SetEventResponseTask
    CTaskComplexPartnerGreet task{
        "CompGreetResp",
        toGreet,
        true,
        0.f,
        partnerType,
        {}
    };
    pg->GetIntelligence().SetEventResponseTask(closestToGreeter, task);
    // Como el original: el evento es una variable local (se construye una vez, se anade por puntero y se destruye al final, tras
    // los LookAt y la escritura de `m_pPed`); pasarlo por valor a Add copiaria el puntero sin registrar la referencia
    CEventCreatePartnerTask event{
        partnerType + 1,
        closestToGreeter,
        false,
        0.f // Seems odd
    };
    toGreet->GetEventGroup().Add(&event);
    PartnerLookAts(pg, closestToGreeter, "CompGreetResp");
    pg->m_pPed = toGreet; // Como el original (0x5FA7DB): escribe el peaton en el primer campo del grupo
    return nullptr;
}

// 0x5FA290
CTaskAllocator* CGroupEventHandler::ComputeDoDealResponse(CPedGroup* pg, CPed* dealWith, CPed* originator) {
    if (!dealWith) {
        return nullptr;
    }
    const auto closestToDealer = PickPartnerFollower(pg, dealWith);
    if (!closestToDealer) {
        return nullptr;
    }
    // Como el original: NO consume ningun rand() para el tipo de socio; el evento lleva tipo 0 (push 0 en 0x5FA473)
    // Como el original: la tarea es un local que vive hasta el final (su destructor limpia la referencia tras el evento)
    CTaskComplexPartnerDeal task{
        "CompDoDealResp",
        dealWith,
        true,
        0.f,
        {}
    };
    pg->GetIntelligence().SetEventResponseTask(closestToDealer, task);
    CEventCreatePartnerTask event{
        0,
        closestToDealer,
        false,
        0.f
    }; // variable local, ver ComputeGreetResponse
    dealWith->GetEventGroup().Add(&event);
    PartnerLookAts(pg, closestToDealer, "CompDoDealResp"); // Como el original: la cadena de 0x86C828, no "CompGreetResp"
    pg->m_pPed = dealWith; // Como el original (0x5FA50E)
    return nullptr;
}

// 0x5FA130
CTaskAllocator* CGroupEventHandler::ComputeFleePedResponse(CPedGroup* pg, CPed* threat, CPed* originator, bool bDamageOriginator) {
    if (!threat) {
        return nullptr;
    }
    const auto leader = pg->GetMembership().GetLeader();
    if (!leader) {
        return nullptr;
    }

    // Como el original (0x5FA18B..0x5FA1BC): distancia 3D AL CUADRADO contra 64.0f (0x859A44), sale solo si es mayor; con NaN sigue
    if ((threat->GetPosition() - leader->GetPosition()).SquaredMagnitude() > 64.f) {
        return nullptr;
    }
    for (auto* const m : pg->GetMembership().GetMembers()) {
        (void)CGeneral::GetRandomNumber(); // Como el original (0x5FA1E0): rand() por cada miembro, resultado descartado
        pg->GetIntelligence().SetEventResponseTask(
            m,
            CTaskComplexSmartFleeEntity{
                threat,
                false,
                60.f,
                10'000,
                1'000,
                fEntityPosChangeThreshold
            }
        );
    }
    return nullptr;
}

// 0x5F7A00
CTaskAllocator* CGroupEventHandler::ComputeDrivebyResponse(CPedGroup* pg, CPed* threat, CPed* originator) {
    return new CTaskAllocatorKillThreatsDriveby{threat};
}

// 0x5FC200
CTaskAllocator* CGroupEventHandler::ComputeEventResponseTasks(const CEventGroupEvent& ge, CPedGroup* g) {
    const auto& p = ge.m_ped;
    const auto& e = ge.m_event;
    switch (e->GetEventType()) {
    case EVENT_DRAGGED_OUT_CAR:
        return ComputeResponseDraggedOutCar(static_cast<const CEventDraggedOutCar&>(*e), g, p);
    case EVENT_DAMAGE:
        return ComputeResponseDamage(static_cast<const CEventDamage&>(*e), g, p);
    case EVENT_SHOT_FIRED:
        return ComputeResponseShotFired(static_cast<const CEventGunShot&>(*e), g, p);
    case EVENT_SEXY_PED:
        return ComputeResponseSexyPed(static_cast<const CEventSexyPed&>(*e), g, p);
    case EVENT_GUN_AIMED_AT:
        return ComputeResponseGunAimedAt(static_cast<const CEventGunAimedAt&>(*e), g, p);
    case EVENT_ACQUAINTANCE_PED_HATE:
    case EVENT_ACQUAINTANCE_PED_DISLIKE:
        return ComputeResponsePedThreat(static_cast<const CEventAcquaintancePed&>(*e), g, p);
    case EVENT_ACQUAINTANCE_PED_LIKE:
    case EVENT_ACQUAINTANCE_PED_RESPECT:
        return ComputeResponsePedFriend(static_cast<const CEventAcquaintancePed&>(*e), g, p);
    case EVENT_VEHICLE_DAMAGE_WEAPON:
    case EVENT_VEHICLE_DAMAGE_COLLISION:
        return ComputeResponseVehicleDamage(static_cast<const CEventVehicleDamage&>(*e), g, p);
    case EVENT_LEADER_ENTERED_CAR_AS_DRIVER:
        return ComputeResponseLeaderEnteredCar(*e, g, p);
    case EVENT_LEADER_EXITED_CAR_AS_DRIVER:
        return ComputeResponseLeaderExitedCar(static_cast<const CEventLeaderExitedCarAsDriver&>(*e), g, p);
    case EVENT_LEADER_QUIT_ENTERING_CAR_AS_DRIVER:
        return ComputeResponsLeaderQuitEnteringCar(*e, g, p);
    case EVENT_PLAYER_COMMAND_TO_GROUP:
    case EVENT_PLAYER_COMMAND_TO_GROUP_GATHER:
        return ComputeResponsePlayerCommand(static_cast<const CEventPlayerCommandToGroup&>(*e), g, p);
    case EVENT_SEEN_COP:
        return ComputeResponseSeenCop(static_cast<const CEventSeenCop&>(*e), g, p);
    case EVENT_DANGER:
        return ComputeResponseDanger(static_cast<const CEventDanger&>(*e), g, p);
    case EVENT_LEADER_ENTRY_EXIT:
        return ComputeResponseLeaderEnterExit(static_cast<const CEventLeaderEntryExit&>(*e), g, p);
    case EVENT_NEW_GANG_MEMBER:
        return ComputeResponseNewGangMember(static_cast<const CEventNewGangMember&>(*e), g, p);
    case EVENT_LEAN_ON_VEHICLE:
        return ComputeLeanOnVehicleResponse(static_cast<const CEventLeanOnVehicle&>(*e), g, p);
    default:
        return nullptr;
    }
}
