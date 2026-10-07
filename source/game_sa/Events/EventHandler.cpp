/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventHandler
 * Reconstrucción: INCOMPLETA — 69/72 funciones reconstruidas.
 * Pendientes de reconstruir: 3.
 * Funciones bloqueadas: 3.
 * Validación registrada: 69/72 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   ComputeKnockOffBikeResponse | 0x004B9FF0 | S
 *   ComputePedCollisionWithPedResponse | 0x004BDB80 | S
 *   ComputePedCollisionWithPlayerResponse | 0x004BE7D0 | S
 * BLOQUEADAS (nombre | dirección | tipo):
 *   ComputeKnockOffBikeResponse | 0x004B9FF0 | S
 *   ComputePedCollisionWithPedResponse | 0x004BDB80 | S
 *   ComputePedCollisionWithPlayerResponse | 0x004BE7D0 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventHandler.h"
#include "InterestingEvents.h"
#include "Ragdoll/IKChainManager.h"
#include "PedStats.h"

#include "Tasks/TaskTypes/TaskComplexFacial.h"
#include "Tasks/TaskTypes/TaskSimpleUseGun.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/TaskComplexInAirAndLand.h"
#include "Tasks/TaskTypes/TaskComplexStuckInAir.h"
#include "Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h"
#include "Tasks/TaskTypes/TaskComplexUseEffect.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFoot.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundObject.h"
#include "Tasks/TaskTypes/TaskSimpleGoTo.h"
#include "Tasks/TaskTypes/TaskSimpleHitHead.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.h"
#include "Tasks/TaskTypes/TaskComplexMoveBackAndJump.h"
#include "Tasks/TaskTypes/TaskComplexJump.h"
#include "Tasks/TaskTypes/TaskComplexGangFollower.h"
#include "Tasks/TaskTypes/TaskSimpleClimb.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskComplexPartnerChat.h"
#include "Tasks/TaskTypes/TaskComplexPartnerDeal.h"
#include "Tasks/TaskTypes/TaskComplexPartnerGreet.h"
#include "Tasks/TaskTypes/TaskComplexCarDriveMission.h"
#include "Tasks/TaskTypes/TaskComplexSmartFleeEntity.h"
#include "Tasks/TaskTypes/TaskComplexInvestigateDeadPed.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "Tasks/TaskTypes/TaskSimpleDead.h"
#include "Tasks/TaskTypes/TaskComplexGangJoinRespond.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsDriver.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h"
#include "Tasks/TaskTypes/TaskSimplePause.h"
#include "Tasks/TaskTypes/TaskSimpleAchieveHeading.h"
#include "Tasks/TaskTypes/TaskSimpleShakeFist.h"
#include "Tasks/TaskTypes/TaskComplexStealCar.h"
#include "Tasks/TaskTypes/TaskComplexDragPedFromCar.h"
#include "Tasks/TaskTypes/TaskComplexKillCriminal.h"
#include "Tasks/TaskTypes/TaskComplexDestroyCar.h"
#include "Tasks/TaskTypes/TaskComplexExtinguishFires.h"
#include "Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.h"
#include "Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.h"
#include "Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.h"
#include "Tasks/TaskTypes/TaskSimpleCower.h"
#include "Tasks/TaskTypes/TaskSimpleHandsUp.h"
#include "Tasks/TaskTypes/TaskComplexReactToGunAimedAt.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.h"
#include "Tasks/TaskTypes/TaskComplexCarDriveMission.h"
#include "Tasks/TaskTypes/TaskComplexFleeEntity.h"
#include "Tasks/TaskTypes/TaskComplexFleeAnyMeans.h"
#include "Tasks/TaskTypes/TaskComplexDie.h"
#include "Tasks/TaskTypes/TaskSimpleStealthKill.h"
#include "Tasks/TaskTypes/TaskComplexFallToDeath.h"
#include "Tasks/TaskTypes/TaskSimpleFall.h"
#include "Tasks/TaskTypes/TaskSimpleGetUp.h"
#include "Tasks/TaskTypes/TaskSimpleGunControl.h"
#include "Tasks/TaskTypes/TaskSimpleChoking.h"
#include "Tasks/TaskTypes/TaskSimpleBeHit.h"
#include "Tasks/TaskTypes/TaskComplexFallAndGetUp.h"
#include "Tasks/TaskTypes/TaskComplexInWater.h"
#include "Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h"
#include "Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundObject.h"
#include "Tasks/TaskTypes/TaskSimplePlayerOnFire.h"
#include "Tasks/TaskTypes/TaskComplexOnFire.h"
#include "Tasks/TaskTypes/TaskComplexPassObject.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCarAndWander.h"
#include "Tasks/TaskTypes/TaskComplexProstituteSolicit.h"
#include "Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h"
#include "Tasks/TaskTypes/TaskComplexSeekEntityAiming.h"
#include "Tasks/TaskTypes/TaskSimpleGangDriveBy.h"
#include "Tasks/TaskTypes/TaskComplexDriveWander.h"
#include "Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.h"
#include "Tasks/TaskTypes/TaskSimpleSay.h"
#include "Tasks/TaskTypes/TaskComplexHitResponse.h"
#include "Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.h"
#include "Tasks/TaskTypes/TaskComplexPartnerShove.h"
#include "Tasks/TaskTypes/TaskComplexSmartFleePoint.h"
#include "Tasks/TaskTypes/TaskComplexFleePoint.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundFire.h"
#include "Tasks/TaskTypes/TaskComplexGetUpAndStandStill.h"
#include "Tasks/TaskTypes/TaskComplexWander.h"
#include "Tasks/TaskTypes/TaskGangHasslePed.h"
#include "Tasks/TaskTypes/TaskComplexSignalAtPed.h"
#include "Tasks/TaskTypes/TaskComplexArrestPed.h"
#include "Tasks/TaskTypes/TaskComplexHitPedWithCar.h"
#include "Tasks/TaskTypes/TaskComplexEvasiveStep.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundCar.h"
#include "Tasks/TaskTypes/TaskComplexRoadRage.h"
#include "Tasks/TaskTypes/TaskComplexLeaveAnyCar.h"
#include "Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.h"
#include "Tasks/TaskTypes/TaskComplexPolicePursuit.h"

#include "Events/Event.h"
#include "Events/EventCarUpsideDown.h"
#include "Events/EventDamage.h"
#include "Events/EventAreaCodes.h"
#include "Events/EventAttractor.h"
#include "Events/EventBuildingCollision.h"
#include "Events/EventChatPartner.h"
#include "Events/EventCopCarBeingStolen.h"
#include "Events/EventCreatePartnerTask.h"
#include "Events/EventDanger.h"
#include "Events/EventDeadPed.h"
#include "Events/EventDeath.h"
#include "Events/EventDontJoinPlayerGroup.h"
#include "Events/EventDraggedOutCar.h"
#include "Events/EventFireNearby.h"
#include "Events/EventDamage.h"
#include "Events/EventGotKnockedOverByCar.h"
#include "Events/EventGunAimedAt.h"
#include "Events/EventHighAngerAtPlayer.h"
#include "Events/EventInWater.h"
#include "Events/EventInteriorUseInfo.h"
#include "Events/EventLowAngerAtPlayer.h"
#include "Events/EventHealthLow.h"
#include "Events/EventPotentialWalkIntoObject.h"
#include "Events/EventObjectCollision.h"
#include "Events/EventPotentialWalkIntoFire.h"
#include "Events/EventOnFire.h"
#include "Events/EventPassObject.h"
#include "Events/EventAcquaintancePed.h"
#include "Events/EventSoundQuiet.h"
#include "Events/EventAcquaintancePedHateBadlyLit.h"
#include "Events/EventAcquaintancePedHate.h"
#include "Events/EventPedToChase.h"
#include "Events/EventPedToFlee.h"
#include "Events/EventPlayerCollisionWithPed.h"
#include "Events/EventPotentialWalkIntoPed.h"
#include "Events/EventHealthReallyLow.h"
#include "Events/EventRevived.h"
#include "Events/EventScriptCommand.h"
#include "Events/EventSeenCop.h"
#include "Events/EventSeenPanickedPed.h"
#include "Events/EventSexyPed.h"
#include "Events/EventSexyVehicle.h"
#include "Events/EventGunShot.h"
#include "Events/EventGunShotWhizzedBy.h"
#include "Events/EventSignalAtPed.h"
#include "Events/EventSpecial.h"
#include "Events/EventVehicleCollision.h"
#include "Events/EventVehicleDamage.h"
#include "Events/EventVehicleDied.h"
#include "Events/EventVehicleHitAndRun.h"
#include "Events/EventVehicleOnFire.h"
#include "Events/EventPotentialGetRunOver.h"
#include "Events/EventPotentialWalkIntoVehicle.h"
#include "Events/EventHitByWaterCannon.h"
#include "Events/EventPedEnteredMyVehicle.h"
#include "Events/EventVehicleToSteal.h"

constexpr auto fSafeDistance = 60.f;

/*****
 * Refactor idea:
 * `Compute*` functions could just return:
 * `Response::Event ComputeBuildingCollisionResponse();`
 * So `SetEventResponseTask` and `ComputeEventResponseTask` could completely be replaced
 * And we could remove the ugly lambda wrapping from all the `Compute*` functions.
 *****
namespace Response {
enum class ResponseType {
    PHYSICAL,
    EVENT,
    SECONDARY_AIM,
    SECONDARY_SAY,
    SECONDARY_PARTIAL_ANIM,
};
template<ResponseType RespType>
struct Base {
    static inline constexpr auto Type = RespType;

    CTask* task;
};
struct Physical : Base<ResponseType::PHYSICAL> {};
struct Primary: Base<ResponseType::EVENT>{};
struct SecondaryAim: Base<ResponseType::SECONDARY_AIM>{};
struct SecondarySay : Base<ResponseType::SECONDARY_SAY>{};
struct SecondaryPartialAnim : Base<ResponseType::SECONDARY_PARTIAL_ANIM>{};

template<typename... Ts>
void ProcessEventResponse( // TODO: Make this a method
    CTask*& physical,
    CTask*& event,
    CTask*& secondaryAim,
    CTask*& secondarySay,
    CTask*& secondaryPartialAnim,
    std::tuple<Ts...> responses
) {
    std::apply([]<typename T>(T r){
        using enum ResponseType;
        switch (T::Type) {
        case PHYSICAL:               physical = r;             break;
        case EVENT:                  event = r;                break;
        case SECONDARY_AIM:          secondaryAim = r;         break;
        case SECONDARY_SAY:          secondarySay = r;         break;
        case SECONDARY_PARTIAL_ANIM: secondaryPartialAnim = r; break;
        default:                     NOTSA_UNREACHABLE();
        }
    }, responses);
}

template<typename T>
void Process( // TODO: Make this a method
    CTask*& physical,
    CTask*& event,
    CTask*& secondaryAim,
    CTask*& secondarySay,
    CTask*& secondaryPartialAnim,
    T       response
) {
    return Process(
        physical
        event
        secondaryAim
        secondarySay
        secondaryPartialAnim
        std::make_tuple(response)
    );
}
};*/

void CEventHandler::InjectHooks() {
    RH_ScopedClass(CEventHandler);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4C3E80);
    RH_ScopedInstall(Destructor, 0x4C3EC0);

    RH_ScopedInstall(Flush, 0x4C3790);
    RH_ScopedInstall(FlushImmediately, 0x4C3820);
    RH_ScopedInstall(GetCurrentEventType, 0x4B8CC0);
    RH_ScopedInstall(HandleEvents, 0x4C3F10);
    RH_ScopedInstall(IsKillTaskAppropriate, 0x4BC3E0);
    RH_ScopedInstall(IsTemporaryEvent, 0x4BC370);
    // RH_ScopedInstall(RecordActiveEvent, 0x0, { .reversed = false });
    // RH_ScopedInstall(RecordPassiveEvent, 0x0, { .reversed = false });
    RH_ScopedInstall(RegisterKill, 0x4B9340);
    RH_ScopedInstall(SetEventResponseTask, 0x4BC600);
    RH_ScopedInstall(ComputeAreaCodesResponse, 0x4BBF50);
    RH_ScopedInstall(ComputeAttractorResponse, 0x4B9BE0);
    // RH_ScopedInstall(ComputeBuildingCollisionPassiveResponse, 0x0, { .reversed = false });
    RH_ScopedInstall(ComputeBuildingCollisionResponse, 0x4BF2B0);
    RH_ScopedInstall(ComputeCarUpsideDownResponse, 0x4BBC30);
    RH_ScopedInstall(ComputeChatPartnerResponse, 0x4B98E0);
    RH_ScopedInstall(ComputeCopCarBeingStolenResponse, 0x4BB740);
    RH_ScopedInstall(ComputeCreatePartnerTaskResponse, 0x4BB130);
    RH_ScopedInstall(ComputeDamageResponse, 0x4C0170);
    RH_ScopedInstall(ComputeDangerResponse, 0x4BC230);
    RH_ScopedInstall(ComputeDeadPedResponse, 0x4B9470);
    RH_ScopedInstall(ComputeDeathResponse, 0x4B9400);
    RH_ScopedInstall(ComputeDontJoinGroupResponse, 0x4BC1D0);
    RH_ScopedInstall(ComputeDraggedOutCarResponse, 0x4BCC30);
    RH_ScopedInstall(ComputeFireNearbyResponse, 0x4BBFB0);
    RH_ScopedInstall(ComputeGotKnockedOverByCarResponse, 0x4C3430);
    RH_ScopedInstall(ComputeGunAimedAtResponse, 0x4C2840);
    RH_ScopedInstall(ComputeHighAngerAtPlayerResponse, 0x4BAC10);
    RH_ScopedInstall(ComputeInWaterResponse, 0x4BAF80);
    RH_ScopedInstall(ComputeInteriorUseInfoResponse, 0x4BAFE0);
    RH_ScopedInstall(ComputeKnockOffBikeResponse, 0x4B9FF0, { .reversed = false });
    RH_ScopedInstall(ComputeLowAngerAtPlayerResponse, 0x4BAAD0);
    RH_ScopedInstall(ComputeLowHealthResponse, 0x4BA990);
    RH_ScopedInstall(ComputeObjectCollisionPassiveResponse, 0x4BBB90);
    RH_ScopedInstall(ComputeObjectCollisionResponse, 0x4B92B0);
    RH_ScopedInstall(ComputeOnEscalatorResponse, 0x4BC150);
    RH_ScopedInstall(ComputeOnFireResponse, 0x4BAD50);
    RH_ScopedInstall(ComputePassObjectResponse, 0x4BB0C0);
    RH_ScopedInstall(ComputePedCollisionWithPedResponse, 0x4BDB80, { .reversed = false });
    RH_ScopedInstall(ComputePedCollisionWithPlayerResponse, 0x4BE7D0, { .reversed = false });
    RH_ScopedInstall(ComputePedEnteredVehicleResponse, 0x4C1590);
    RH_ScopedInstall(ComputePedFriendResponse, 0x4B9DD0);
    RH_ScopedInstall(ComputePedSoundQuietResponse, 0x4B9D40);
    RH_ScopedInstall(ComputePedThreatBadlyLitResponse, 0x4B9C90);
    RH_ScopedInstall(ComputePedThreatResponse, 0x4C19A0);
    RH_ScopedInstall(ComputePedToChaseResponse, 0x4C1910);
    RH_ScopedInstall(ComputePedToFleeResponse, 0x4B9B50);
    RH_ScopedInstall(ComputePersonalityResponseToDamage, 0x4BF9B0);
    RH_ScopedInstall(ComputePlayerCollisionWithPedResponse, 0x4B8CE0);
    RH_ScopedInstall(ComputePlayerWantedLevelResponse, 0x4BB280);
    RH_ScopedInstall(ComputePotentialPedCollideResponse, 0x4C2610);
    RH_ScopedInstall(ComputePotentialWalkIntoFireResponse, 0x4BBCD0);
    RH_ScopedInstall(ComputeReallyLowHealthResponse, 0x4BAA30);
    RH_ScopedInstall(ComputeReviveResponse, 0x4B97B0);
    RH_ScopedInstall(ComputeScriptCommandResponse, 0x4BA7C0);
    RH_ScopedInstall(ComputeSeenCopResponse, 0x4BC050);
    RH_ScopedInstall(ComputeSeenPanickedPedResponse, 0x4C35F0);
    RH_ScopedInstall(ComputeSexyPedResponse, 0x4B99F0);
    RH_ScopedInstall(ComputeSexyVehicleResponse, 0x4B9AA0);
    RH_ScopedInstall(ComputeShotFiredResponse, 0x4BC710);
    RH_ScopedInstall(ComputeShotFiredWhizzedByResponse, 0x4BBE30);
    RH_ScopedInstall(ComputeSignalAtPedResponse, 0x4BB050);
    RH_ScopedInstall(ComputeSpecialResponse, 0x4BB800);
    RH_ScopedInstall(ComputeVehicleCollisionResponse, 0x4BD6A0);
    RH_ScopedInstall(ComputeVehicleDamageResponse, 0x4C2FC0);
    RH_ScopedInstall(ComputeVehicleDiedResponse, 0x4BA8B0);
    // RH_ScopedInstall(ComputeVehicleHitAndRunResponse, 0x0, { .reversed = false });
    RH_ScopedInstall(ComputeVehicleOnFireResponse, 0x4BB2E0);
    RH_ScopedInstall(ComputeVehiclePotentialCollisionResponse, 0x4C0BD0);
    RH_ScopedInstall(ComputeVehiclePotentialPassiveCollisionResponse, 0x4B96D0);
    RH_ScopedInstall(ComputeVehicleToStealResponse, 0x4B9F80);
    RH_ScopedInstall(ComputeWaterCannonResponse, 0x4BAE30);

    RH_ScopedOverloadedInstall(ComputeEventResponseTask, "0", 0x4C3870, void (CEventHandler::*)(CEvent*, CTask*));
    RH_ScopedOverloadedInstall(ComputeEventResponseTask, "Ped", 0x4C4220, CTask*(*)(const CPed&, const CEvent&));
}

// 0x4C3E80
CEventHandler::CEventHandler(CPed* ped) :
    m_Ped{ped}
{
}

// 0x4C3F10
void CEventHandler::HandleEvents() {
    m_History.TickStoredEvent(m_Ped);

    const auto lastAbortedTask = m_History.GetAbortedTask();

    const auto pedTM                   = &m_Ped->GetTaskManager();
    const auto primaryTask             = m_Ped->GetIntelligence()->GetActivePrimaryTask();
    const auto tempEventRespTask       = pedTM->GetTemporaryEventResponseTask();
    const auto presistentEventRespTask = pedTM->GetPresistentEventResponseTask();

    // Como el original: el evento de mayor prioridad se lee ANTES de hacer TickEvents y no se vuelve a pedir despues
    const auto pedEG                     = &m_Ped->GetEventGroup();
    const auto pedEGHighestPriorityEvent = pedEG->GetHighestPriorityEvent();

    pedEG->TickEvents();

    if (lastAbortedTask) { // 0x4C3F96
        m_History.RecordAbortedTask(primaryTask && lastAbortedTask == primaryTask ? primaryTask : nullptr);
    }

    if (!tempEventRespTask) { // 0x4C3FB2
        if (!presistentEventRespTask) {
            m_History.RecordAbortedTask(nullptr);
        }
        m_History.ClearTempEvent();
    }

    if (!presistentEventRespTask) { // 0x4C3FDC
        m_History.ClearNonTempEvent();
    }

    if (!pedEGHighestPriorityEvent) { // 0x4C409D
        if (!primaryTask) {
            m_History.RecordAbortedTask(nullptr);
            m_History.ClearAllEvents();
        }
        return;
    }

    if (pedEGHighestPriorityEvent->GetEventType() == EVENT_DAMAGE) { // 0x4C4004
        m_Ped->SetIsStatic(false);
    }

    if (const auto currEvent = m_History.GetCurrentEvent()) { // 0x4C4015
        if (!currEvent->CanBeInterruptedBySameEvent() && currEvent->GetEventType() == pedEGHighestPriorityEvent->GetEventType()) {
            pedEG->Remove(pedEGHighestPriorityEvent);
            pedEG->RemoveInvalidEvents(false);
            pedEG->Reorganise();
            if (!primaryTask) {
                m_History.RecordAbortedTask(nullptr);
                m_History.ClearAllEvents();
            }
            return;
        }
    }

    if (m_History.TakesPriorityOverCurrentEvent(*pedEGHighestPriorityEvent)) { // 0x4C40DB
        const auto activeTask = pedTM->GetActiveTask();

        const auto hasStoppedTimers = [&] {
            if (!activeTask || activeTask->MakeAbortable(m_Ped, ABORT_PRIORITY_URGENT, pedEGHighestPriorityEvent)) {
                pedTM->StopTimers(pedEGHighestPriorityEvent);
                return true;
            }
            return false;
        }();

        // Como el original: se calcula la respuesta, se registra el evento actual y se guarda la tarea
        const auto RespondToEvent = [&](CTask* abortedTask) {
            ComputeEventResponseTask(pedEGHighestPriorityEvent, abortedTask);
            if (abortedTask || primaryTask) { // Como el original: la tarea abortada se borra DESPUES de calcular la respuesta
                m_History.RecordAbortedTask(nullptr);
            }
            if (m_EventResponseTask) {
                m_History.RecordCurrentEvent(m_Ped, *pedEGHighestPriorityEvent);
            }
            SetEventResponseTask(*pedEGHighestPriorityEvent);
            pedEG->Remove(pedEGHighestPriorityEvent);
        };

        if (primaryTask) { // 0x4C41A7
            if (hasStoppedTimers) {
                RespondToEvent(lastAbortedTask);
            } else {
                pedEGHighestPriorityEvent->UnTick();
                activeTask->MakeAbortable(m_Ped, ABORT_PRIORITY_LEISURE, pedEGHighestPriorityEvent);
                m_History.RecordAbortedTask(primaryTask); // Como el original: se guarda la tarea primaria, no la activa
            }
        } else if (lastAbortedTask) { // 0x4C4172
            RespondToEvent(lastAbortedTask);
        } else if (hasStoppedTimers) { // 0x4C413F
            RespondToEvent(nullptr);
        } else {
            pedEGHighestPriorityEvent->UnTick(); // 0x4C4137
        }
    }

    pedEG->RemoveInvalidEvents(false); // 0x4C41FF
    pedEG->Reorganise();
}

void CEventHandler::RecordActiveEvent(const CEvent& event) {
    assert(false);
}

void CEventHandler::RecordPassiveEvent(const CEvent& event) {
    assert(false);
}

// 0x4B9340
void CEventHandler::RegisterKill(const CPed* ped, const CEntity* inflictedBy, eWeaponType weaponUsed, bool wasHeadShot) {
    if (!ped) {
        return;
    }
    if (inflictedBy && inflictedBy->GetIsTypePed() && inflictedBy->AsPed()->IsPlayer() && inflictedBy != ped) {
        const auto pi = &FindPlayerInfo();
        pi->m_nHavocCaused += 10;
        pi->m_fCurrentChaseValue += 5.f;
        CDarkel::RegisterKillByPlayer(*ped, weaponUsed, wasHeadShot, inflictedBy->AsPed()->m_nPedType);
        CPedGroups::RegisterKillByPlayer();
    } else if (inflictedBy && inflictedBy->GetIsTypeVehicle() && inflictedBy == FindPlayerVehicle()) {
        CStats::IncrementStat(STAT_PEOPLE_YOUVE_WASTED);
    } else {
        CDarkel::RegisterKillNotByPlayer(ped);
    }
}

// 0x4BC600
void CEventHandler::SetEventResponseTask(const CEvent& event) {
    const auto i  = m_Ped->GetIntelligence();
    const auto tm = &i->GetTaskManager();

    if (m_PhysicalResponseTask) {
        i->AddTaskPhysResponse(m_PhysicalResponseTask, true);
    }

    if (m_EventResponseTask) {
        if (IsTemporaryEvent(event)) {
            i->AddTaskEventResponseTemp(m_EventResponseTask, false);
        } else {
            i->AddTaskEventResponseTemp(nullptr, 0);
            i->AddTaskEventResponseNonTemp(m_EventResponseTask, false);
        }
    }

    if (m_AttackTask) {
        if (const auto tAttack = tm->GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
            tAttack->MakeAbortable(m_Ped);
        }
        tm->SetTaskSecondary(m_AttackTask, TASK_SECONDARY_ATTACK);
    }

    if (m_SayTask) {
        tm->SetTaskSecondary(m_SayTask, TASK_SECONDARY_SAY);
    }

    if (m_PartialAnimTask) {
        tm->SetTaskSecondary(m_PartialAnimTask, TASK_SECONDARY_PARTIAL_ANIM);
    }

    if (event.HasEditableResponse()) {
        const auto eResponse = &static_cast<const CEventEditableResponse&>(event);
        if (const auto tfacial = static_cast<CTaskComplexFacial*>(tm->GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX))) { // Como el original (0x4BC6D5): lee el hueco sin comprobar el tipo
            if (eResponse->GetFacialExpressionType() != eFacialExpression::NONE) {
                tfacial->SetRequest(eResponse->GetFacialExpressionType(), 10'000, eFacialExpression::NONE, 0); // Como el original: la segunda duracion es 0, no -1
            }
        }
    }
}

// @addr unk
// NOTE/BUG: Weird, no `delete`s?
void CEventHandler::ResetResponse() {
    m_PhysicalResponseTask = nullptr;
    m_EventResponseTask    = nullptr;
    m_AttackTask           = nullptr;
    m_SayTask              = nullptr;
    m_PartialAnimTask      = nullptr;
}

// 0x4C3790
void CEventHandler::Flush() {
    delete m_PhysicalResponseTask;
    m_PhysicalResponseTask = nullptr;

    delete m_EventResponseTask;
    m_EventResponseTask = nullptr;

    delete m_AttackTask;
    m_AttackTask = nullptr;

    delete m_SayTask;
    m_SayTask = nullptr;

    delete m_PartialAnimTask;
    m_PartialAnimTask = nullptr;

    m_History.Flush();
}

// 0x4C3820
void CEventHandler::FlushImmediately() {
    m_History.Flush();
}

// 0x4B8CC0
eEventType CEventHandler::GetCurrentEventType() const {
    if (const auto e = m_History.GetCurrentEvent()) {
        return e->GetEventType();
    }
    return EVENT_NONE;
}

// 0x4BC370
bool CEventHandler::IsTemporaryEvent(const CEvent& event) {
    auto eventType = event.GetEventType();
    switch (eventType) {
    case EVENT_VEHICLE_COLLISION:
    case EVENT_PED_COLLISION_WITH_PED:
    case EVENT_PED_COLLISION_WITH_PLAYER:
    case EVENT_PLAYER_COLLISION_WITH_PED:
    case EVENT_OBJECT_COLLISION:
    case EVENT_BUILDING_COLLISION:
    case EVENT_IN_AIR:
    case EVENT_POTENTIAL_WALK_INTO_VEHICLE:
    case EVENT_POTENTIAL_WALK_INTO_BUILDING:
    case EVENT_POTENTIAL_WALK_INTO_OBJECT:
    case EVENT_POTENTIAL_WALK_INTO_PED:
    case EVENT_POTENTIAL_WALK_INTO_FIRE:
    case EVENT_AREA_CODES:
    case EVENT_STUCK_IN_AIR:
    case EVENT_POTENTIAL_GET_RUN_OVER:
    case EVENT_ON_ESCALATOR:
    case EVENT_FIRE_NEARBY:
        return true;
    default:
        return false;
    }
}

// 0x4BC3E0
bool CEventHandler::IsKillTaskAppropriate(CPed* ped1, CPed* ped2, const CEvent& event) {
    if (ped1->IsCreatedByMission()) {
        return true;
    }
    if (!ped1->GetActiveWeapon().IsTypeMelee() || ped2->GetActiveWeapon().IsTypeMelee()) {
        return true;
    }
    return false;
}

// 0x4BBF50
void CEventHandler::ComputeAreaCodesResponse(CEventAreaCodes* e, CTask* tactive, CTask* tsimplest) {
    if (!e->m_ped) {
        return;
    }
    m_EventResponseTask = new CTaskSimpleWaitUntilAreaCodesMatch{e->m_ped};
}

// 0x4B9BE0
void CEventHandler::ComputeAttractorResponse(CEventAttractor* e, CTask* tactive, CTask* tsimplest) {
    if (e->GetEventType() == EVENT_ATTRACTOR && !e->m_entity) {
        return;
    }
    if (e->m_TaskId == TASK_NONE) {
        m_EventResponseTask = nullptr;
    } else if (e->m_TaskId == TASK_COMPLEX_USE_EFFECT) {
        // Como el original: sin hueco libre la respuesta se borra (0x4B9C66); cualquier otra tarea no escribe nada
        if (!GetPedAttractorManager()->HasEmptySlot(e->m_2dEffect, e->m_entity)) {
            m_EventResponseTask = nullptr;
        } else {
            m_EventResponseTask = new CTaskComplexUseEffect{e->m_2dEffect, e->m_entity};
        }
    }
}

// 0x0
// EVENT_POTENTIAL_WALK_INTO_BUILDING
// ANDROID IDB 0x3025F8 (1.0)
void CEventHandler::ComputeBuildingCollisionPassiveResponse(CEvent* e, CTask* tactive, CTask* tsimplest) {
    // NOP
}

// 0x4BF2B0
void CEventHandler::ComputeBuildingCollisionResponse(CEventBuildingCollision* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: casi todas las salidas NO escriben m_EventResponseTask (solo las marcadas "borra" lo ponen a nullptr);
    // el id de la tarea activa solo se usa para comprobar que existe.
    if (!tactive) {
        return;
    }

    CBuilding* const building = e->m_building;
    if (!building) {
        return;
    }

    const bool isHeadOn = e->IsHeadOnCollision(m_Ped); // 0x4BF2FE (sin efectos laterales)

    const int32 moveState = e->m_moveState; // movsx
    if (moveState == PEDMOVE_STILL) {
        return;
    }

    const auto tm                = &m_Ped->GetTaskManager();
    const auto tKillPedOnFoot    = tm->Find<CTaskComplexKillPedOnFoot>(false);
    const auto tSeekEntity       = tm->Find<CTaskComplexSeekEntity<>>(false); // Hackery, because we don't know what `T_PosCalc` actually is...
    const auto tEnterCarPsgrWait = tm->Find<CTaskComplexEnterCarAsPassengerWait>(false);

    if (e->CanTreatBuildingAsObject(building)) { // 0x4BF36E
        if (!CTask::IsGoToTask(tsimplest)) {
            return;
        }
        m_EventResponseTask = new CTaskComplexWalkRoundObject{
            moveState,
            static_cast<CTaskSimpleGoTo*>(tsimplest)->m_vecTargetPoint,
            building
        };
        return;
    }

    // Como el original: se consume un numero aleatorio (0, 0x400) antes de mirar la tarea simple
    const bool randomHit = (CGeneral::GetRandomIntInRange(0, 0x400) & 0xF) == 0xF;

    const auto tGoToPoint = CTask::IsGoToTask(tsimplest)
        ? static_cast<CTaskSimpleGoTo*>(tsimplest)
        : nullptr;

    // 0x4BF3F5. Como el original: fcomp + test ah,41h; con NaN (C3 activo) se salta el borrado, igual que "<= 0,707"
    if (m_Ped->bIsStanding && e->m_impactNormal.z > 0.707f) {
        m_EventResponseTask = nullptr;
        return;
    }

    if (!tGoToPoint && (!tKillPedOnFoot || !tKillPedOnFoot->m_target)) { // 0x4BF416
        if (isHeadOn
            && !CPedGeometryAnalyser::CanPedJumpObstacle(*m_Ped, *building, e->m_impactNormal, e->m_impactPos)
            && moveState > (int32)PEDMOVE_WALK /* con signo, como jle */
        ) {
            m_EventResponseTask = new CTaskSimpleHitHead{};
        } else {
            m_EventResponseTask = nullptr;
        }
        return;
    }

    // 0x4BF474: posicion objetivo (copia cruda de 3 dwords)
    const CVector targetPos = (tKillPedOnFoot && tKillPedOnFoot->m_target)
        ? tKillPedOnFoot->m_target->GetPosition()
        : tGoToPoint->m_vecTargetPoint;

    if (!isHeadOn) { // 0x4BF821
        if (tKillPedOnFoot) {
            if (const auto target = tKillPedOnFoot->m_target) {
                m_EventResponseTask = new CTaskComplexWalkRoundBuildingAttempt{
                    (eMoveState)moveState,
                    target,
                    {},
                    e->m_impactPos,
                    e->m_impactNormal,
                    false
                };
            } else {
                m_EventResponseTask = new CTaskComplexWalkRoundBuildingAttempt{
                    (eMoveState)moveState,
                    targetPos,
                    e->m_impactPos,
                    e->m_impactNormal,
                    false
                };
            }
            return;
        }
        if (const auto seekTarget = tSeekEntity ? tSeekEntity->GetEntityToSeek() : nullptr) { // 0x4BF8BD
            m_EventResponseTask = new CTaskComplexWalkRoundBuildingAttempt{
                (eMoveState)moveState,
                seekTarget,
                {},
                e->m_impactPos,
                e->m_impactNormal,
                false
            };
        } else {
            m_EventResponseTask = new CTaskComplexWalkRoundBuildingAttempt{
                (eMoveState)moveState,
                targetPos,
                e->m_impactPos,
                e->m_impactNormal,
                false
            };
        }
        return;
    }

    // 0x4BF4C7: colision frontal
    if (!tSeekEntity && !tKillPedOnFoot && !tEnterCarPsgrWait && !randomHit) {
        m_EventResponseTask = new CTaskComplexWalkRoundBuildingAttempt{
            (eMoveState)moveState,
            targetPos,
            e->m_impactPos,
            e->m_impactNormal,
            isHeadOn
        };
        return;
    }

    if (CPedGeometryAnalyser::CanPedJumpObstacle(*m_Ped, *building, e->m_impactNormal, e->m_impactPos)) { // 0x4BF543
        if (moveState > (int32)PEDMOVE_WALK /* con signo, como jle */) {
            m_EventResponseTask = new CTaskComplexMoveBackAndJump;
        } else {
            m_EventResponseTask = new CTaskComplexJump{};
        }
        return;
    }

    if (tKillPedOnFoot) { // 0x4BF5C2
        const auto target = tKillPedOnFoot->m_target;
        if (!target) {
            m_EventResponseTask = nullptr;
            return;
        }
        if (CPedGeometryAnalyser::CanPedTargetPed(*m_Ped, *target, true)) {
            m_EventResponseTask = nullptr;
            return;
        }
        m_EventResponseTask = new CTaskComplexWalkRoundBuildingAttempt{
            (eMoveState)moveState,
            target,
            {},
            e->m_impactPos,
            e->m_impactNormal,
            isHeadOn
        };
        return;
    }

    CEntity* targetEntity{};
    if (tSeekEntity) { // 0x4BF68E
        targetEntity = tSeekEntity->GetEntityToSeek();
    } else if (tEnterCarPsgrWait) { // 0x4BF693
        const auto car = tEnterCarPsgrWait->GetCar();
        if (!car) {
            return;
        }
        targetEntity = car->m_pDriver;
    } else { // 0x4BF666
        m_EventResponseTask = new CTaskComplexWalkRoundBuildingAttempt{
            (eMoveState)moveState,
            targetPos,
            e->m_impactPos,
            e->m_impactNormal,
            isHeadOn
        };
        return;
    }

    if (!targetEntity) { // 0x4BF6A4
        return;
    }

    if (CTaskComplexGangFollower::ms_bUseClimbing) { // 0x4BF6AC
        if (CPedGroups::IsInPlayersGroup(m_Ped)) {
            CVector      climbPos;
            float        climbGrabHeading;
            eSurfaceType climbSurfaceType;
            if (const auto entityToClimb = CTaskSimpleClimb::TestForClimb(m_Ped, climbPos, climbGrabHeading, climbSurfaceType, true)) {
                m_EventResponseTask = new CTaskSimpleClimb{
                    entityToClimb,
                    climbPos,
                    climbGrabHeading,
                    climbSurfaceType,
                    climbPos.z - m_Ped->GetPosition().z < CTaskSimpleClimb::ms_fMinForStretchGrab ? CLIMB_PULLUP : CLIMB_GRAB,
                    true
                };
                return;
            }
        }
    }

    if (targetEntity->GetIsTypePed()) { // 0x4BF76A
        if (!g_ikChainMan.IsLooking(m_Ped)) {
            g_ikChainMan.LookAt(
                "CompBldgCollResp",
                m_Ped,
                targetEntity,
                3'000,
                BONE_HEAD,
                nullptr,
                true,
                0.25f,
                500,
                3,
                false
            );
        }
        /* nop GetTaskSecondary */
    }

    m_EventResponseTask = new CTaskComplexWalkRoundBuildingAttempt{ // 0x4BF7C6
        (eMoveState)moveState,
        targetEntity,
        {},
        e->m_impactPos,
        e->m_impactNormal,
        isHeadOn
    };
}

// 0x4BBC30
void CEventHandler::ComputeCarUpsideDownResponse(CEventCarUpsideDown* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: si el ped no esta en ese vehiculo no se escribe nada
    if (!m_Ped->bInVehicle || !m_Ped->m_pVehicle || m_Ped->m_pVehicle != e->m_vehicle) {
        return;
    }
    m_EventResponseTask = new CTaskComplexLeaveCar{
        e->m_vehicle,
        0,
        CGeneral::GetRandomIntInRange(100, 400),
        true,
        false
    };
}

// 0x4B98E0
void CEventHandler::ComputeChatPartnerResponse(CEventChatPartner* e, CTask* tactive, CTask* tsimplest) {
    if (!e->m_partner || m_Ped->GetTaskManager().Has<CTaskComplexPartnerChat>(false)) {
        m_EventResponseTask = nullptr;
        return;
    }
    // Como el original (0x4B9934): se consume un aleatorio (3, 7) cuyo resultado se descarta
    [[maybe_unused]] const auto discardedRandom = CGeneral::GetRandomIntInRange(3, 7);
    m_EventResponseTask = new CTaskComplexPartnerChat{
        "EventChatPartnerResponse",
        e->m_partner,
        e->m_leadSpeaker,
        0.5f,
        4,
        true,
        false,
        {}
    };
}

// 0x4BB740
void CEventHandler::ComputeCopCarBeingStolenResponse(CEventCopCarBeingStolen* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: si no se cumplen las condiciones no se escribe nada
    if (!e->m_hijacker || !e->m_vehicle || m_Ped->m_pVehicle != e->m_vehicle || !m_Ped->bInVehicle) {
        return;
    }
    m_EventResponseTask = new CTaskComplexLeaveCar{ e->m_vehicle, 0, 0, true, false };
    if (e->m_hijacker->IsPlayer()) {
        // Como el original (0x4BB7C5): el nivel de busqueda es el del ladron (su m_pPlayerData), no el del jugador 0
        const auto playerData = e->m_hijacker->GetPlayerData();
        const auto wanted     = playerData ? playerData->m_pWanted : nullptr;
        wanted->SetWantedLevelNoDrop(eWantedLevel::WANTED_LEVEL_1);
    }
}

// 0x4BB130
void CEventHandler::ComputeCreatePartnerTaskResponse(CEventCreatePartnerTask* e, CTask* tactive, CTask* tsimplest) {
    if (!e->m_partner) {
        m_EventResponseTask = nullptr;
        return;
    }
    if (e->m_partnerType == 0) {
        m_EventResponseTask = new CTaskComplexPartnerDeal{
            "CompPartnerTaskResp",
            e->m_partner,
            e->m_isLeadSpeaker,
            e->m_meetDist,
            {}
        };
    } else if (e->m_partnerType >= 1 && e->m_partnerType <= 6) { // Como el original: fuera de 0..6 no se escribe nada
        m_EventResponseTask = new CTaskComplexPartnerGreet{
            "CompPartnerTaskResp",
            e->m_partner,
            e->m_isLeadSpeaker,
            e->m_meetDist,
            e->m_partnerType - 1,
            {}
        };
    }
}

// 0x4C0170
void CEventHandler::ComputeDamageResponse(CEventDamage* e, CTask* tactive, CTask* tsimplest, CTask* abortedTaskEventResponse) {
    UNUSED(abortedTaskEventResponse);

    // Como el original (0x4C018D): un ped que ya no esta vivo no responde al dano
    if (!m_Ped->IsAlive()) {
        return;
    }

    const auto srcEntity  = e->m_pSourceEntity;
    const auto weaponType = e->m_weaponType;

    // Como el original (0x4C01BA / 0x4C0636): el evento actual es el mismo dano para la IA (se calcula antes de las demas comprobaciones)
    const auto IsSameDamageAsCurrentEvent = [&] {
        const auto ce = m_History.GetCurrentEvent();
        return ce && ce->GetEventType() == EVENT_DAMAGE && e->IsSameEventForAI(static_cast<CEventDamage&>(*ce));
    };

    if (!e->m_bAddToEventGroup) { // 0x4C01BA
        const auto sameAsCurrent = IsSameDamageAsCurrentEvent();
        if (!m_Ped->GetIntelligence()->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_REACT_TO_ATTACK)
            && !m_Ped->IsPlayer()
            && !sameAsCurrent
            && srcEntity
            && srcEntity->GetIsTypePed()
        ) {
            ComputePersonalityResponseToDamage(e, srcEntity->AsPed());
        }
        return;
    }

    // Como el original (0x4C062D): respuesta de personalidad final (esta usa GetSourceEntity virtual)
    const auto DoPersonalityResponse = [&] {
        const auto sameAsCurrent = IsSameDamageAsCurrentEvent();
        if (!m_Ped->GetIntelligence()->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_REACT_TO_ATTACK)
            && !m_Ped->IsPlayer()
            && !sameAsCurrent
            && e->GetSourceEntity()
            && e->GetSourceEntity()->GetIsTypePed()
        ) {
            ComputePersonalityResponseToDamage(e, e->GetSourceEntity()->AsPed());
        }
    };

    e->ProcessDamage(m_Ped);

    if (weaponType == WEAPON_RAMMEDBYCAR || weaponType == WEAPON_RUNOVERBYCAR) {
        g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_15, m_Ped);
    }

    // Como el original: en moto o quad (0x4C02A0)
    const auto IsOnBikeOrQuad = [&] {
        const auto v = m_Ped->GetVehicleIfInOne();
        return v && (v->IsBike() || v->IsSubQuad());
    };
    if (IsOnBikeOrQuad()) {
        if (e->m_bFallDown || e->HasKilledPed()) {
            ComputeKnockOffBikeResponse(e, tactive, tsimplest); // 0x4C02DE
        } else if (srcEntity && srcEntity->GetIsTypePed()) { // 0x4C0236
            ComputePersonalityResponseToDamage(e, srcEntity->AsPed());
        }
        return;
    }

    const auto bInVehicle = m_Ped->bInVehicle;

    // Como el original: el peaton muere (0x4C02E8). `m_fHealth < 1` es falso con NaN como el fcomp + test ah,5
    if (e->HasKilledPed() || m_Ped->m_fHealth < 1.f || (bInVehicle && e->m_bFallDown)) {
        // 0x4C07D0
        if (IsOnBikeOrQuad()) {
            ComputeKnockOffBikeResponse(e, tactive, tsimplest); // 0x4C07E5
            return;
        }

        bool  bFallToDeath         = false;
        int32 fallToDeathDir       = 0;
        bool  bFallToDeathOverRail = false;

        if (e->m_nAnimID == ANIM_ID_NO_ANIMATION_SET || e->m_nAnimID == ANIM_ID_DOOR_LHINGE_O) { // 0x4C07FF
            // Como el original: se parte de la tarea mas simple activa para decidir la animacion de muerte
            if (tsimplest) {
                switch (tsimplest->GetTaskType()) {
                case TASK_SIMPLE_FALL: { // 0x4C0834
                    if (weaponType == WEAPON_FALL) {
                        const auto dr = &e->m_damageResponse;
                        if (dr->m_fDamageArmor + dr->m_fDamageHealth > 20.f) { // 0x4C083E - sangre
                            for (auto i = 10; i-- > 0;) {
                                // Como el original: solo X e Y llevan aleatorio (dos tiradas por iteracion) y X va primero
                                CVector pos = m_Ped->GetPosition();
                                pos.x += CGeneral::GetRandomNumberInRange(-0.5f, 0.5f);
                                pos.y += CGeneral::GetRandomNumberInRange(-0.5f, 0.5f);
                                const CVector dir{ 0.f, 0.f, 2.f };
                                g_fx.AddBlood(pos, dir, 10, m_Ped->m_fContactSurfaceBrightness);
                            }
                        }
                    }
                    const auto tfall = static_cast<CTaskSimpleFall*>(tsimplest);
                    if (const auto a = tfall->m_pAnim; a && a->m_BlendAmount > 0.5f && a->m_BlendDelta >= 0.f && a->m_CurrentTime < a->m_BlendHier->m_fTotalTime) { // 0x4C08ED
                        e->m_nAnimGroup = a->m_AnimGroupId;
                        e->m_nAnimID    = a->m_AnimId;
                        e->m_fAnimBlend = 4.f;
                        e->m_fAnimSpeed = 1.f;
                    } else if (tfall->m_bIsFinished) { // 0x4C0938
                        e->m_fAnimSpeed = 1.f;
                        e->m_fAnimBlend = 4.f;
                        e->m_nAnimGroup = ANIM_GROUP_DEFAULT;
                        e->m_nAnimID    = RpAnimBlendClumpGetFirstAssociation(m_Ped->GetRpClump(), (uint32)ANIMATION_IS_FRONT)
                            ? ANIM_ID_FLOOR_HIT_F
                            : ANIM_ID_FLOOR_HIT;
                    }
                    break;
                }
                case TASK_SIMPLE_GET_UP: { // 0x4C0987
                    const auto tgup = static_cast<CTaskSimpleGetUp*>(tsimplest);
                    if (const auto a = tgup->m_Anim; a && a->m_CurrentTime / a->m_BlendHier->m_fTotalTime < 0.5f) {
                        e->m_fAnimSpeed = 1.f;
                        e->m_fAnimBlend = 4.f;
                        e->m_nAnimGroup = ANIM_GROUP_DEFAULT;
                        e->m_nAnimID    = a->m_AnimId == ANIM_ID_GETUP_FRONT
                            ? ANIM_ID_FLOOR_HIT_F
                            : ANIM_ID_FLOOR_HIT;
                    } else if (!tgup->m_bHasPedGotUp) { // 0x4C09DD
                        e->m_nAnimGroup = ANIM_GROUP_DEFAULT;
                        e->m_nAnimID    = RpAnimBlendClumpGetFirstAssociation(m_Ped->GetRpClump(), (uint32)ANIMATION_IS_FRONT)
                            ? ANIM_ID_FLOOR_HIT_F
                            : ANIM_ID_FLOOR_HIT;
                        e->m_fAnimBlend = 4.f;
                        e->m_fAnimSpeed = 1.f;
                    }
                    break;
                }
                default:
                    break;
                }
            }

            // 0x4C0A1C
            if (e->m_nAnimID == ANIM_ID_NO_ANIMATION_SET || e->m_nAnimID == ANIM_ID_DOOR_LHINGE_O) {
                e->ComputeDeathAnim(m_Ped, false);
            }
            const auto wt         = (int32)weaponType;
            const auto bFirearm   = (wt >= (int32)WEAPON_PISTOL && wt <= (int32)WEAPON_COUNTRYRIFLE) || wt == (int32)WEAPON_MINIGUN;
            if (!m_Ped->IsPlayer() && srcEntity && bFirearm && srcEntity->GetIsTypePed()) { // 0x4C0A5C
                const auto srcPed = srcEntity->AsPed();
                if (srcPed->IsPlayer() || srcPed->IsCreatedByMission()) {
                    bFallToDeath = CTaskComplexFallToDeath::CalcFall(m_Ped, fallToDeathDir, bFallToDeathOverRail);
                }
            }
        }

        // 0x4C0AA2
        const auto isBeingKilledByStealth = tactive
            && tactive->GetTaskType() == TASK_SIMPLE_STEALTH_KILL
            && !static_cast<CTaskSimpleStealthKill*>(tactive)->m_bKeepTargetAlive;
        // Como el original: la tarea de muerte se crea ANTES de quitar la respuesta fisica y de registrar la muerte
        m_EventResponseTask = new CTaskComplexDie{
            weaponType,
            e->m_nAnimGroup,
            e->m_nAnimID,
            e->m_fAnimBlend,
            e->m_fAnimSpeed,
            isBeingKilledByStealth,
            bFallToDeath,
            (eDirection)fallToDeathDir,
            bFallToDeathOverRail
        };
        g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_28, m_Ped);
        if (const auto tPhysResp = m_Ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PHYSICAL_RESPONSE)) { // 0x4C0B4B
            if (tPhysResp->GetTaskType() != TASK_SIMPLE_CHOKING
                || !(weaponType == WEAPON_SPRAYCAN || weaponType == WEAPON_EXTINGUISHER || weaponType == WEAPON_TEARGAS)
            ) {
                m_Ped->GetIntelligence()->AddTaskPhysResponse(nullptr, 0); // NB: esto puede borrar `tactive` o `tsimplest`
            }
        }
        const auto bHeadShot = e->WasHeadShot();
        if (srcEntity) {
            RegisterKill(m_Ped, srcEntity, weaponType, bHeadShot);
        }
        return;
    }

    if (!srcEntity) { // 0x4C031F
        return;
    }

    if ((int32)weaponType < (int32)WEAPON_LAST_WEAPON) { // 0x4C0327
        if (bInVehicle) { // 0x4C0330
            DoPersonalityResponse();
            return;
        }

        CPed* const eventSrcPed = srcEntity->GetIsTypePed()
            ? srcEntity->AsPed()
            : nullptr;

        if (eventSrcPed) { // 0x4C034D
            if (const auto tKiller = eventSrcPed->GetIntelligence()->GetTaskManager().GetActiveTask()) {
                if (tKiller->GetTaskType() == TASK_SIMPLE_STEALTH_KILL) {
                    m_PhysicalResponseTask = new CTaskSimpleStealthKill{ false, eventSrcPed, static_cast<CTaskSimpleStealthKill*>(tKiller)->m_animGrpId };
                    m_Ped->SetPedState(PEDSTATE_DIE_BY_STEALTH);
                    return;
                }
            }
        }

        if (e->m_nAnimID == ANIM_ID_NO_ANIMATION_SET) { // 0x4C03D6
            e->ComputeDamageAnim(m_Ped, false);
        }

        // Como el original (0x4C03F3): sin comprobar `eventSrcPed` (el EXE se cae con un origen que no es un ped)
        if (weaponType == WEAPON_SPRAYCAN || weaponType == WEAPON_EXTINGUISHER || weaponType == WEAPON_TEARGAS) {
            // Como el original (0x4C0403): GetTaskUseGun se llama dos veces si la primera no es nula
            const auto tGun = eventSrcPed->GetIntelligence()->GetTaskUseGun();
            if (!tGun || eventSrcPed->GetIntelligence()->GetTaskUseGun()->GetLastGunCommand() != eGunCommand::PISTOLWHIP) { // 0x4C05B9 - asfixia
                const auto bIsTearGas = weaponType == WEAPON_TEARGAS;
                const auto tPhysResp  = m_Ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PHYSICAL_RESPONSE);
                if (tPhysResp && tPhysResp->GetTaskType() == TASK_SIMPLE_CHOKING) {
                    static_cast<CTaskSimpleChoking*>(tPhysResp)->UpdateChoke(m_Ped, eventSrcPed, bIsTearGas);
                } else {
                    m_PhysicalResponseTask = new CTaskSimpleChoking{ eventSrcPed, bIsTearGas };
                }
                DoPersonalityResponse();
                return;
            }
        }

        if (e->m_bFallDown) { // 0x4C042F
            m_PhysicalResponseTask = new CTaskComplexFallAndGetUp{
                e->GetAnimId(),
                e->GetAnimGroup(),
                // Como el original: 1000 / (rate * 0.025f) con la constante del EXE y truncado con _ftol2
                (int32)(int64)(1000.f / ((float)m_Ped->m_nWeaponShootingRate * 0.025f))
            };
        } else if (e->m_nAnimID != ANIM_ID_NO_ANIMATION_SET && e->m_nAnimID != ANIM_ID_DOOR_LHINGE_O) { // 0x4C0488
            if (CAnimManager::GetAnimAssociation(e->m_nAnimGroup, e->m_nAnimID)->m_Flags & ANIMATION_DONT_ADD_TO_PARTIAL_BLEND) { // 0x4C04B7
                if (!e->GetAnimAdded()) {
                    if ((int32)e->m_nAnimID >= (int32)ANIM_ID_SHOT_PARTIAL && (int32)e->m_nAnimID <= (int32)ANIM_ID_SHOT_RIGHTP) { // 0x4C04CA
                        auto a = RpAnimBlendClumpGetAssociation(m_Ped->GetRpClump(), (uint32)e->m_nAnimID);
                        if (!a) {
                            a = CAnimManager::AddAnimation(m_Ped->GetRpClump(), e->m_nAnimGroup, e->m_nAnimID);
                        }
                        a->m_BlendAmount = 0.f;
                        a->m_BlendDelta  = e->m_fAnimBlend;
                        a->m_Speed       = e->m_fAnimSpeed;
                        a->SetCurrentTime(0.f);
                    } else { // 0x4C0524
                        const auto a = CAnimManager::BlendAnimation(m_Ped->GetRpClump(), e->m_nAnimGroup, e->m_nAnimID, e->m_fAnimBlend);
                        a->m_Speed = e->m_fAnimSpeed;
                        a->SetCurrentTime(0.f);
                    }
                    e->m_bAnimAdded = true; // Como el original: en las dos ramas (or [esi+25h],4)
                }
            } else { // 0x4C0559
                const auto tBeHit = new CTaskSimpleBeHit{
                    eventSrcPed,
                    e->m_pedPieceType,
                    e->m_ucDirection,
                    (int32)(int64)(e->m_damageResponse.m_fDamageArmor + e->m_damageResponse.m_fDamageHealth)
                };
                m_PhysicalResponseTask = tBeHit;

                tBeHit->m_nAnimId    = e->GetAnimId();
                tBeHit->m_nAnimGroup = e->GetAnimGroup();
                tBeHit->m_bAnimAdded = e->GetAnimAdded();
            }
        }

        DoPersonalityResponse();
        return;
    }

    // 0x4C06D7 - dano que no es de arma
    if (bInVehicle) {
        return;
    }
    if (e->m_nAnimID == ANIM_ID_NO_ANIMATION_SET || e->m_nAnimID == ANIM_ID_DOOR_LHINGE_O) {
        return;
    }
    if (weaponType != WEAPON_RAMMEDBYCAR && weaponType != WEAPON_RUNOVERBYCAR && weaponType != WEAPON_FALL) {
        return;
    }

    m_PhysicalResponseTask = new CTaskComplexFallAndGetUp{ // 0x4C070B
        e->GetAnimId(),
        e->GetAnimGroup(),
        m_Ped->GetPlayerData() ? 500 : 1000 // Como el original: se mira el puntero de datos de jugador, no IsPlayer()
    };

    CVehicle* knockedOverByVeh = nullptr;
    if (srcEntity->GetIsTypeVehicle()) {
        knockedOverByVeh = srcEntity->AsVehicle();
    } else if (srcEntity->GetIsTypePed()) {
        const auto srcPed = srcEntity->AsPed();
        if (srcPed->bInVehicle) {
            knockedOverByVeh = srcPed->m_pVehicle;
        }
    }
    if (knockedOverByVeh) { // 0x4C0795
        CEventGotKnockedOverByCar eKnockedOver{ knockedOverByVeh };
        m_Ped->GetEventGroup().Add(&eKnockedOver, false);
    }
}

// 0x4BC230
void CEventHandler::ComputeDangerResponse(CEventDanger* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: sin peligro o con otra tarea no se escribe nada
    if (!e->m_dangerFrom) {
        return;
    }
    if (e->m_TaskId == TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE) {
        if (m_Ped->bInVehicle) {
            if (!m_Ped->m_pVehicle->IsDriver(m_Ped)) {
                m_EventResponseTask = nullptr;
            } else {
                m_EventResponseTask = new CTaskComplexCarDriveMissionFleeScene{m_Ped->m_pVehicle};
            }
            return;
        }
    } else if (e->m_TaskId != TASK_COMPLEX_SMART_FLEE_ENTITY) {
        return;
    }
    m_EventResponseTask = new CTaskComplexSmartFleeEntity{ e->m_dangerFrom, true, 100'000.f };
}

// 0x4B9470
void CEventHandler::ComputeDeadPedResponse(CEventDeadPed* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: sin ped muerto NO se escribe m_EventResponseTask
    const auto deadPed = e->GetDeadPed();
    if (!deadPed) {
        return;
    }
    if (m_Ped->IsPlayer()) { // 0x4B94D8
        // Como el original: distancia 2D con (y*y + x*x) y comparacion con 0.5625001f (constante del EXE); con NaN no se hace nada
        const auto pedPos  = m_Ped->GetPosition();
        const auto deadPos = deadPed->GetPosition();
        const auto dx      = pedPos.x - deadPos.x;
        const auto dy      = pedPos.y - deadPos.y;
        const auto dist2D  = dy * dy + dx * dx;
        if (!(dist2D < std::bit_cast<float>(0x3F100001u))) {
            return;
        }
        if (m_Ped->GetIntelligence()->GetTaskFighting()) {
            return;
        }
        if (g_ikChainMan.IsLooking(m_Ped)) {
            return;
        }
        const auto c = &TheCamera.GetActiveCam();
        switch (c->m_nMode) {
        case MODE_SNIPER:
        case MODE_CAMERA:
        case MODE_ROCKETLAUNCHER:
        case MODE_ROCKETLAUNCHER_HS:
        case MODE_M16_1STPERSON:
        case MODE_1STPERSON:
        case MODE_HELICANNON_1STPERSON:
            return;
        }
        if (c->GetWeaponFirstPersonOn()) {
            return;
        }
        g_ikChainMan.LookAt(
            "CompDeadPedResp",
            m_Ped,
            deadPed,
            500,
            BONE_HEAD,
            nullptr,
            true,
            0.25f,
            500,
            3,
            false
        );
        return;
    }
    switch ((int16)e->m_TaskId) {
    case TASK_NONE: // 0x4B9622
        m_EventResponseTask = nullptr;
        return;
    case TASK_COMPLEX_INVESTIGATE_DEAD_PED: // 0x4B963D
        m_EventResponseTask = new CTaskComplexInvestigateDeadPed{ deadPed };
        return;
    case TASK_SIMPLE_DUCK_FOREVER: // 0x4B95EF
        m_EventResponseTask = new CTaskSimpleDuck{ DUCK_STANDALONE, 0xE0FF, -1 };
        return;
    case TASK_COMPLEX_SMART_FLEE_ENTITY: // 0x4B9668 - Como el original: distancia segura 60 (C18CEC) y el resto por defecto
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{ deadPed, true, fSafeDistance };
        return;
    default:
        return;
    }
}

// 0x4B9400
void CEventHandler::ComputeDeathResponse(CEventDeath* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = new CTaskSimpleDead{ e->GetDeathTime(), e->HasDrowned() };
}

// 0x4BC1D0
void CEventHandler::ComputeDontJoinGroupResponse(CEventDontJoinPlayerGroup* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = new CTaskComplexGangJoinRespond{false};
}

// 0x4BCC30
void CEventHandler::ComputeDraggedOutCarResponse(CEventDraggedOutCar* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: ped jugador o sin vehiculo no escribe nada
    const auto isDriverSeat = e->m_IsDriverSeat;
    const auto jacker       = e->m_CarJacker;
    const auto veh          = e->m_Vehicle;

    if (m_Ped->IsPlayer() || !veh) {
        return;
    }

    if (veh->GetAnimGroupId() == ANIM_GROUP_COACHCARANIMS) { // 0x4BCC7E
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{jacker, false, fSafeDistance};
        return;
    }

    // Como el original (0x4BCCF2): la tarea de bajarse del coche se crea ANTES del switch, aunque luego no se use
    CTask* leaveCarTask = nullptr;
    if (veh->m_pDriver == m_Ped || veh->m_apPassengers[0] == m_Ped) {
        leaveCarTask = new CTaskComplexLeaveCar{ veh, 0, 0, false, true };
    }

    m_EventResponseTask = nullptr;

    // 0x4BCDB9 / 0x4BD264 / 0x4BD482: pausa, o girarse hacia el ladron y sacudir el puno (rand del EXE & 0x3FF > 0x200)
    const auto AddPauseOrFaceJacker = [&](CTaskComplexSequence* seq) {
        if ((CGeneral::GetRandomNumber() & 0x3FF) > 0x200) {
            const auto& pedPos    = m_Ped->GetPosition();
            const auto& jackerPos = jacker->GetPosition(); // Como el original: con ladron nulo revienta igual que el EXE
            const auto  heading   = CGeneral::LimitRadianAngle(
                CGeneral::GetRadianAngleBetweenPoints(jackerPos.x, jackerPos.y, pedPos.x, pedPos.y)
            );
            seq->AddTask(new CTaskSimpleAchieveHeading{heading, 0.5f, 0.2f});
            seq->AddTask(new CTaskSimpleShakeFist{});
        } else {
            seq->AddTask(new CTaskSimplePause{CGeneral::GetRandomIntInRange(500, 1'500)});
        }
    };

    switch (e->m_TaskId) {
    case TASK_NONE: // 0x4BD042
        break;
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: { // 0x4BCFB2 (700)
        if (isDriverSeat) {
            m_EventResponseTask = new CTaskComplexEnterCarAsDriver{veh};
        } else {
            m_EventResponseTask = new CTaskComplexEnterCarAsPassenger{veh};
        }
        break;
    }
    case TASK_COMPLEX_STEAL_CAR: { // 0x4BCD83 (702)
        const auto seq = new CTaskComplexSequence;
        m_EventResponseTask = seq;
        AddPauseOrFaceJacker(seq);
        m_Ped->Say(CTX_GLOBAL_JACKED_ON_STREET); // 0x4BCEF7
        if (isDriverSeat) {
            seq->AddTask(new CTaskComplexStealCar{veh});
        } else {
            seq->AddTask(new CTaskComplexDragPedFromCar{jacker});
            seq->AddTask(new CTaskComplexEnterCarAsPassenger{veh});
        }
        break;
    }
    case TASK_COMPLEX_SMART_FLEE_ENTITY: { // 0x4BD04A (911)
        if (jacker) {
            m_EventResponseTask = new CTaskComplexSmartFleeEntity{jacker, false, fSafeDistance};
        } else if (!veh->m_pDriver) {
            m_EventResponseTask = new CTaskComplexStealCar{veh};
        }
        break;
    }
    case TASK_COMPLEX_KILL_CRIMINAL: { // 0x4BD117 (1105)
        if (jacker) {
            if (jacker->IsPlayer()) {
                m_EventResponseTask = nullptr;
            } else {
                m_EventResponseTask = new CTaskComplexKillCriminal{jacker};
            }
        }
        break;
    }
    case TASK_COMPLEX_DESTROY_CAR: { // 0x4BD171 (1003)
        m_EventResponseTask = new CTaskComplexDestroyCar{veh};
        break;
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4BD1B2 (1000)
        if (jacker) {
            if (!IsKillTaskAppropriate(m_Ped, jacker, *e)) { // 0x4BD1D1
                m_EventResponseTask = new CTaskComplexSmartFleeEntity{jacker, false, fSafeDistance};
                break;
            }
            const auto seq = new CTaskComplexSequence; // 0x4BD230
            m_EventResponseTask = seq;
            AddPauseOrFaceJacker(seq);
            seq->AddTask(new CTaskComplexKillPedOnFoot{jacker});
            m_Ped->Say(CTX_GLOBAL_JACKED_ON_STREET);
            if (isDriverSeat) {
                seq->AddTask(new CTaskComplexEnterCarAsDriver{veh});
            } else {
                seq->AddTask(new CTaskComplexEnterCarAsPassenger{veh});
            }
        } else { // 0x4BD44D: sin ladron no hay Say ni tarea de matar
            const auto seq = new CTaskComplexSequence;
            m_EventResponseTask = seq;
            AddPauseOrFaceJacker(seq);
            if (isDriverSeat) {
                seq->AddTask(new CTaskComplexStealCar{veh});
            } else {
                seq->AddTask(new CTaskComplexDragPedFromCar{nullptr});
                seq->AddTask(new CTaskComplexEnterCarAsPassenger{veh});
            }
        }
        break;
    }
    default:
        break;
    }

    if (leaveCarTask) { // 0x4BD636
        const auto seq = new CTaskComplexSequence;
        seq->AddTask(leaveCarTask);
        seq->AddTask(m_EventResponseTask);
        m_EventResponseTask = seq;
    }
}

// 0x4BBFB0
void CEventHandler::ComputeFireNearbyResponse(CEventFireNearby* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: otras tareas no escriben nada
    if (e->m_TaskId == TASK_NONE) {
        m_EventResponseTask = nullptr;
    } else if (e->m_TaskId == TASK_COMPLEX_EXTINGUISH_FIRES) {
        m_EventResponseTask = new CTaskComplexExtinguishFires;
    }
}

// 0x4C3430
void CEventHandler::ComputeGotKnockedOverByCarResponse(CEventGotKnockedOverByCar* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: sin vehiculo no se escribe nada; las tareas no contempladas tampoco
    if (!e->m_vehicle) {
        return;
    }
    const auto drvr = e->m_vehicle->m_pDriver;
    switch ((int16)e->m_TaskId) {
    case TASK_NONE: // 0x4C34E0
        m_EventResponseTask = nullptr;
        return;
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4C34FB
        if (!drvr) {
            return;
        }
        if (IsKillTaskAppropriate(m_Ped, drvr, *e)) {
            m_EventResponseTask = new CTaskComplexKillPedOnFoot{ drvr };
        } else {
            m_EventResponseTask = new CTaskComplexSmartFleeEntity{ drvr, false, fSafeDistance };
        }
        return;
    }
    case TASK_COMPLEX_SMART_FLEE_ENTITY: { // 0x4C3486 - Como el original: sin conductor huye del propio vehiculo
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{ drvr ? (CEntity*)drvr : (CEntity*)e->m_vehicle, false, fSafeDistance };
        return;
    }
    case TASK_COMPLEX_KILL_CRIMINAL: { // 0x4C358C
        if (!drvr) {
            return;
        }
        if (drvr->IsPlayer()) {
            m_EventResponseTask = nullptr;
            return;
        }
        m_EventResponseTask = new CTaskComplexKillCriminal{ drvr, false };
        return;
    }
    default:
        return;
    }
}

// 0x4C2840
void CEventHandler::ComputeGunAimedAtResponse(CEventGunAimedAt* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: ped jugador, tareas desconocidas y el agacharse con tarea de agacharse ya activa no escriben nada.
    // tsimplest no se usa (solo la tarea activa decide si se huye).
    const auto aimedBy = e->m_AimedBy;

    if (m_Ped->IsPlayer()) {
        return;
    }

    if (tactive) { // 0x4C287E
        const auto activeType = tactive->GetTaskType();
        if (activeType == TASK_COMPLEX_FLEE_ENTITY || activeType == TASK_COMPLEX_FLEE_POINT) {
            m_EventResponseTask = new CTaskComplexFleeEntity{aimedBy, false}; // 0x4C28FF
            return;
        }
        if (activeType == TASK_COMPLEX_SMART_FLEE_ENTITY || activeType == TASK_COMPLEX_SMART_FLEE_POINT) {
            m_EventResponseTask = new CTaskComplexSmartFleeEntity{aimedBy, false, fSafeDistance}; // 0x4C28B6
            return;
        }
    }

    if (m_Ped->IsCreatedBy(PED_GAME) && !m_Ped->bInVehicle && !m_Ped->GetActiveWeapon().IsTypeMelee()) { // 0x4C291A
        if (aimedBy && aimedBy->IsPlayer() && !m_Ped->GetIntelligence()->IsFriendlyWith(*aimedBy)) {
            e->m_TaskId = TASK_COMPLEX_KILL_PED_ON_FOOT; // 0x4C296F
        }
    }

    switch (e->m_TaskId) {
    case TASK_NONE:
        m_EventResponseTask = nullptr;
        break;
    case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR: // 0x4C2A58
        m_EventResponseTask = new CTaskComplexUseClosestFreeScriptedAttractor;
        break;
    case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN:
        m_EventResponseTask = new CTaskComplexUseClosestFreeScriptedAttractorRun{};
        break;
    case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT:
        m_EventResponseTask = new CTaskComplexUseClosestFreeScriptedAttractorSprint{};
        break;
    case TASK_SIMPLE_COWER:
        m_EventResponseTask = new CTaskSimpleCower{};
        break;
    case TASK_SIMPLE_HANDS_UP:
        m_EventResponseTask = new CTaskSimpleHandsUp{5'000};
        break;
    case TASK_SIMPLE_DUCK: { // 0x4C2A0D
        if (!m_Ped->GetIntelligence()->GetTaskDuck(true)) { // Como el original: si ya hay tarea de agacharse no se escribe nada
            m_EventResponseTask = new CTaskSimpleDuck{DUCK_STANDALONE, 5'000, -1};
        }
        break;
    }
    case TASK_SIMPLE_DUCK_FOREVER:
        m_EventResponseTask = new CTaskSimpleDuck{DUCK_STANDALONE, 0x967Fu, -1}; // are they trying to tell us something? "Fu" :D
        break;
    case TASK_COMPLEX_REACT_TO_GUN_AIMED_AT: // 0x4C2B0B
        m_EventResponseTask = new CTaskComplexReactToGunAimedAt{aimedBy};
        break;
    case TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH: // 0x4C2E13
        m_EventResponseTask = new CTaskComplexKillPedOnFootStealth{aimedBy};
        break;
    case TASK_COMPLEX_KILL_CRIMINAL: { // 0x4C2D7C
        if (aimedBy->IsPlayer()) {
            m_EventResponseTask = nullptr;
        } else if (IsKillTaskAppropriate(m_Ped, aimedBy, *e)) {
            m_EventResponseTask = new CTaskComplexKillCriminal{aimedBy};
        } else {
            m_EventResponseTask = new CTaskComplexFleeEntity{aimedBy, false};
        }
        break;
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4C2E37
        if (IsKillTaskAppropriate(m_Ped, aimedBy, *e)) {
            m_EventResponseTask = new CTaskComplexKillPedOnFoot{aimedBy};
        } else {
            m_EventResponseTask = new CTaskComplexFleeEntity{aimedBy, false};
        }
        break;
    }
    case TASK_COMPLEX_FLEE_ANY_MEANS: // 0x4C2D01
        m_EventResponseTask = new CTaskComplexFleeAnyMeans{aimedBy, true, fSafeDistance};
        break;
    case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE: { // 0x4C2BF8
        if (const auto v = m_Ped->m_pVehicle) {
            if (v->IsDriver(m_Ped)) {
                m_EventResponseTask = new CTaskComplexCarDriveMissionFleeScene{v};
                break;
            }
        }
        if (m_Ped->bWantedByPolice && aimedBy->IsCop()) {
            m_EventResponseTask = new CTaskComplexFleeAnyMeans{aimedBy, true, fSafeDistance};
        } else {
            m_EventResponseTask = new CTaskComplexSmartFleeEntity{aimedBy, false, fSafeDistance}; // Como el original: SmartFlee (0x65C430), no Flee
        }
        break;
    }
    case TASK_COMPLEX_FLEE_ENTITY: // 0x4C2BAF
        m_EventResponseTask = new CTaskComplexFleeEntity{aimedBy, false};
        break;
    case TASK_COMPLEX_SMART_FLEE_ENTITY: { // 0x4C2B62
        if (m_Ped->bWantedByPolice && aimedBy->IsCop()) {
            m_EventResponseTask = new CTaskComplexFleeAnyMeans{aimedBy, true, fSafeDistance};
        } else {
            m_EventResponseTask = new CTaskComplexSmartFleeEntity{aimedBy, false, fSafeDistance}; // Como el original: SmartFlee (0x65C430), no Flee
        }
        break;
    }
    default:
        break; // Como el original: sin escritura
    }
}

// 0x4BAC10
void CEventHandler::ComputeHighAngerAtPlayerResponse(CEventHighAngerAtPlayer* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: otras tareas no escriben nada
    switch (e->m_TaskId) {
    case TASK_COMPLEX_KILL_PED_ON_FOOT:
        m_EventResponseTask = new CTaskComplexKillPedOnFoot{FindPlayerPed()};
        break;
    case TASK_COMPLEX_SMART_FLEE_ENTITY:
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{FindPlayerPed(), false, 60.f};
        break;
    case TASK_NONE:
        m_EventResponseTask = nullptr;
        break;
    case TASK_SIMPLE_DUCK_FOREVER:
        m_EventResponseTask = new CTaskSimpleDuck{DUCK_STANDALONE, 0xE0FFu, -1};
        break;
    default:
        break;
    }
}

// 0x4BAF80
void CEventHandler::ComputeInWaterResponse(CEventInWater* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = new CTaskComplexInWater;
}

// 0x4BAFE0
void CEventHandler::ComputeInteriorUseInfoResponse(CEventInteriorUseInfo* e, CTaskInteriorUseInfo* tUseInfo, CTask* tsimplest) {
    // Como el original: crea una tarea nueva con los datos del evento (la tarea activa no se usa)
    m_EventResponseTask = new CTaskInteriorUseInfo{
        e->m_InteriorInfo,
        e->m_Interior,
        (int32)e->m_ActionAnimTime,
        e->m_nLoopAction != 0
    };
}

// 0x4B9FF0
void CEventHandler::ComputeKnockOffBikeResponse(CEvent* e, CTask* tactive, CTask* tsimplest) {
    plugin::CallMethod<0x4B9FF0, CEventHandler*, CEvent*, CTask*, CTask*>(this, e, tactive, tsimplest);
}

// 0x4BAAD0
void CEventHandler::ComputeLowAngerAtPlayerResponse(CEventLowAngerAtPlayer* e, CTask* tactive, CTask* tsimplest) {
    // Same code as `ComputeHighAngerAtPlayerResponse`. Como el original: otras tareas no escriben nada
    switch (e->m_TaskId) {
    case TASK_COMPLEX_KILL_PED_ON_FOOT:
        m_EventResponseTask = new CTaskComplexKillPedOnFoot{FindPlayerPed()};
        break;
    case TASK_COMPLEX_SMART_FLEE_ENTITY:
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{FindPlayerPed(), false, 60.f};
        break;
    case TASK_NONE:
        m_EventResponseTask = nullptr;
        break;
    case TASK_SIMPLE_DUCK_FOREVER:
        m_EventResponseTask = new CTaskSimpleDuck{DUCK_STANDALONE, 0xE0FFu, -1};
        break;
    default:
        break;
    }
}

// 0x4BA990
void CEventHandler::ComputeLowHealthResponse(CEventHealthLow* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: otras tareas no escriben nada
    if (e->m_TaskId == TASK_NONE) {
        m_EventResponseTask = nullptr;
    } else if (e->m_TaskId == TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP) {
        m_EventResponseTask = new CTaskComplexDiveFromAttachedEntityAndGetUp{0};
    }
}

// 0x4BBB90
void CEventHandler::ComputeObjectCollisionPassiveResponse(CEventPotentialWalkIntoObject* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: si no se cumplen las condiciones no se escribe nada
    if (!e->m_object || m_Ped->bInVehicle || e->m_moveState == PEDMOVE_STILL) {
        return;
    }
    if (!tsimplest || !CTask::IsGoToTask(tsimplest)) {
        return;
    }
    const auto tGoTo = static_cast<CTaskSimpleGoTo*>(tsimplest);
    m_EventResponseTask = new CTaskComplexWalkRoundObject{
        e->m_moveState,
        tGoTo->m_vecTargetPoint,
        e->m_object
    };
}

// 0x4B92B0
void CEventHandler::ComputeObjectCollisionResponse(CEventObjectCollision* e, CTask* tactive, CTask* tsimplest) {
    // Como el original (0x4B92B0): NO es igual que la version pasiva. Va primero la tarea simple y no se mira bInVehicle;
    // la llamada a 0x4ACE80 (IsHeadOnCollision, sin efectos) se descarta y si no se cumple algo no se escribe nada.
    if (!tsimplest || !CTask::IsGoToTask(tsimplest)) {
        return;
    }
    const int32 moveState = e->m_moveState; // movsx
    if (moveState == PEDMOVE_STILL) {
        return;
    }
    if (!e->m_object) {
        return;
    }
    const auto tGoTo = static_cast<CTaskSimpleGoTo*>(tsimplest);
    m_EventResponseTask = new CTaskComplexWalkRoundObject{
        moveState,
        tGoTo->m_vecTargetPoint,
        e->m_object
    };
}

// 0x4BC150
void CEventHandler::ComputeOnEscalatorResponse(CEvent* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: el EXE construye un CTaskSimpleStandStill{0, true, false, 8.f} y le cambia la vtable por la de
    // CTaskSimpleOnEscalator (0x85B8DC; misma talla 0x20). La clase de la DLL todavia es una plantilla sin constructor.
    const auto t = new CTaskSimpleStandStill{0, true, false, 8.f};
    *reinterpret_cast<uint32*>(t) = 0x85B8DC;
    m_EventResponseTask = t;
}

// 0x4BAD50
void CEventHandler::ComputeOnFireResponse(CEventOnFire* e, CTask* tactive, CTask* tsimplest) {
    if (m_Ped->IsPlayer()) {
        if (!m_Ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM)) {
            m_PartialAnimTask = new CTaskSimplePlayerOnFire{};
        }
    } else {
        m_EventResponseTask = new CTaskComplexOnFire{};
    }
}

// 0x4BB0C0
void CEventHandler::ComputePassObjectResponse(CEventPassObject* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: el byte +0x10 de la tarea se guarda crudo (el EXE copia el byte del evento sin normalizarlo a bool)
    const auto t = new CTaskComplexPassObject{e->m_giver, false};
    *reinterpret_cast<uint8*>(&t->m_isPasser) = *reinterpret_cast<const uint8*>(&e->m_dontPassObject);
    m_EventResponseTask = t;
}

// 0x4BDB80
void CEventHandler::ComputePedCollisionWithPedResponse(CEvent* e, CTask* tactive, CTask* tsimplest) {
    plugin::CallMethod<0x4BDB80, CEventHandler*, CEvent*, CTask*, CTask*>(this, e, tactive, tsimplest);
}

// 0x4BE7D0
void CEventHandler::ComputePedCollisionWithPlayerResponse(CEvent* e, CTask* tactive, CTask* tsimplest) {
    plugin::CallMethod<0x4BE7D0, CEventHandler*, CEvent*, CTask*, CTask*>(this, e, tactive, tsimplest);
}

// 0x4C1590
void CEventHandler::ComputePedEnteredVehicleResponse(CEventPedEnteredMyVehicle* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: sin vehiculo, sin peaton o sin estar el peaton en un vehiculo no se escribe nada.
    // (El EXE tambien llama a IsDriver y descarta el resultado; no tiene efectos.)
    const auto veh = e->m_Vehicle;
    const auto ped = e->m_PedThatEntered;
    if (!veh || !ped || !m_Ped->bInVehicle) {
        return;
    }
    if (veh->m_pHandlingData->GetAnimGroupId() == ANIM_GROUP_COACHCARANIMS) {
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{ ped, false, 60.f };
        return;
    }
    g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_20, veh);

    // Como el original (0x407180): una sola tirada antes del switch, se use o no
    const auto delay = CGeneral::GetRandomIntInRange(300, 600);
    const auto LeaveCarAndFlee = [&]() -> CTask* {
        return new CTaskComplexLeaveCarAndFlee{ veh, veh->GetPosition(), e->m_TargetDoor, delay };
    };
    switch (e->m_TaskId) {
    case TASK_COMPLEX_KILL_CRIMINAL: { // 0x4C17ED
        if (ped->IsPlayer()) {
            m_EventResponseTask = nullptr;
            return;
        }
        m_EventResponseTask = new CTaskComplexKillCriminal{ ped };
        return;
    }
    case TASK_COMPLEX_DESTROY_CAR: // 0x4C1849
        m_EventResponseTask = new CTaskComplexDestroyCar{ veh };
        return;
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4C186A
        if (!IsKillTaskAppropriate(m_Ped, ped, *e)) {
            m_EventResponseTask = LeaveCarAndFlee();
            return;
        }
        m_EventResponseTask = new CTaskComplexKillPedOnFoot{ ped };
        return;
    }
    case TASK_COMPLEX_SCREAM_IN_CAR_THEN_LEAVE: { // 0x4C1759
        if (ped->IsPlayer() && CTheScripts::IsPlayerOnAMission()) {
            m_EventResponseTask = LeaveCarAndFlee();
            return;
        }
        m_EventResponseTask = new CTaskComplexScreamInCarThenLeave{ veh, e->m_TargetDoor };
        return;
    }
    case TASK_COMPLEX_LEAVE_CAR: // 0x4C16CE
        m_EventResponseTask = new CTaskComplexLeaveCar{ veh, (int32)e->m_TargetDoor, delay, false, true };
        return;
    case TASK_COMPLEX_LEAVE_CAR_AND_FLEE: // 0x4C18C9
        m_EventResponseTask = LeaveCarAndFlee();
        return;
    case TASK_COMPLEX_LEAVE_CAR_AND_WANDER: // 0x4C1744
        m_EventResponseTask = new CTaskComplexLeaveCarAndWander{ veh, e->m_TargetDoor, delay, false };
        return;
    case TASK_NONE: // 0x4C16F1
        m_EventResponseTask = nullptr;
        return;
    default: // Como el original: otro tipo de tarea no escribe nada
        return;
    }
}

// 0x4B9DD0
void CEventHandler::ComputePedFriendResponse(CEventAcquaintancePed* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: solo se escribe la tarea de respuesta en las rutas que la crean (o TASK_NONE); el resto no toca nada
    switch (e->m_TaskId) {
    case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR:
        m_EventResponseTask = new CTaskComplexUseClosestFreeScriptedAttractor{};
        return;
    case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN:
        m_EventResponseTask = new CTaskComplexUseClosestFreeScriptedAttractorRun{};
        return;
    case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT:
        m_EventResponseTask = new CTaskComplexUseClosestFreeScriptedAttractorSprint{};
        return;
    case TASK_COMPLEX_PROSTITUTE_SOLICIT: {
        if (m_Ped->GetTaskManager().Find<CTaskComplexProstituteSolicit>(false)) {
            return;
        }
        if (m_Ped->bInVehicle) {
            return;
        }
        m_EventResponseTask = new CTaskComplexProstituteSolicit{e->m_AcquaintancePed};
        return;
    }
    case TASK_NONE:
        m_EventResponseTask = nullptr;
        return;
    default:
        return;
    }
}

// 0x4B9D40
void CEventHandler::ComputePedSoundQuietResponse(CEventSoundQuiet* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: sin entidad origen, o con otro tipo de tarea, no se escribe nada
    if (!e->GetSourceEntity()) {
        return;
    }
    switch (e->m_TaskId) {
    case TASK_NONE:
        m_EventResponseTask = nullptr;
        return;
    case TASK_COMPLEX_INVESTIGATE_DISTURBANCE:
        m_EventResponseTask = new CTaskComplexInvestigateDisturbance{e->m_position, e->GetSourceEntity()};
        return;
    default:
        return;
    }
}

// 0x4B9C90
void CEventHandler::ComputePedThreatBadlyLitResponse(CEventAcquaintancePedHateBadlyLit* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: sin peaton conocido, o con otro tipo de tarea, no se escribe nada
    if (!e->m_AcquaintancePed) {
        return;
    }
    switch (e->m_TaskId) {
    case TASK_NONE:
        m_EventResponseTask = nullptr;
        return;
    case TASK_COMPLEX_INVESTIGATE_DISTURBANCE:
        m_EventResponseTask = new CTaskComplexInvestigateDisturbance{e->m_point, e->m_AcquaintancePed};
        return;
    default:
        return;
    }
}

// 0x4C19A0
void CEventHandler::ComputePedThreatResponse(CEventAcquaintancePedHate* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: sin peaton conocido no se escribe nada, y solo se escribe m_EventResponseTask en las rutas
    // donde el EXE lo hace (tipos de tarea no soportados y rutas "no hacer nada" lo dejan intacto)
    const auto ped = e->m_AcquaintancePed;
    if (!ped) {
        return;
    }
    const auto Set = [&](CTask* t) { m_EventResponseTask = t; };
    const auto IsWantedByCop = [&] { return m_Ped->bWantedByPolice && ped->IsCop(); }; // 0x478 bit 11 y tipo de peaton 6

    // Las salidas de "no matar" comunes (0x4C1D98, 0x4C1EBB, 0x4C1F9D): huir de la policia, conducir lejos o huir andando
    const auto FleeInsteadOfKill = [&](bool driveAway) -> CTask* {
        if (IsWantedByCop()) {
            return new CTaskComplexFleeAnyMeans{ped, true, 60.f};
        }
        if (driveAway && m_Ped->bInVehicle && m_Ped->m_pVehicle && m_Ped->m_pVehicle->IsDriver(m_Ped)) {
            return new CTaskComplexCarDriveMissionFleeScene{m_Ped->m_pVehicle};
        }
        return new CTaskComplexSmartFleeEntity{ped, false, 60.f};
    };

    switch (e->m_TaskId) {
    case TASK_NONE: // 0x4C1A13
        Set(nullptr);
        return;
    case TASK_COMPLEX_SEQUENCE: { // 0x4C1AC0
        const int32 seqIdx = m_Ped->GetIntelligence()->m_NextEventResponseSequence; // char con signo
        if (seqIdx < 0) {
            return;
        }
        if (!CTaskSequences::ms_taskSequence[seqIdx].m_Tasks.data()[0]) {
            return;
        }
        Set(new CTaskComplexUseSequence{seqIdx});
        return;
    }
    case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR:
        Set(new CTaskComplexUseClosestFreeScriptedAttractor{});
        return;
    case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN:
        Set(new CTaskComplexUseClosestFreeScriptedAttractorRun{});
        return;
    case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT:
        Set(new CTaskComplexUseClosestFreeScriptedAttractorSprint{});
        return;
    case TASK_SIMPLE_COWER: // 0x4C1A1B
        Set(new CTaskSimpleCower{});
        return;
    case TASK_SIMPLE_HANDS_UP: // 0x4C1B14
        Set(new CTaskSimpleHandsUp{5'000});
        return;
    case TASK_SIMPLE_DUCK: { // 0x4C1B96: si ya hay tarea de agacharse no se escribe nada
        if (m_Ped->GetIntelligence()->GetTaskDuck(true)) {
            return;
        }
        Set(new CTaskSimpleDuck{DUCK_STANDALONE, 5'000, -1});
        return;
    }
    case TASK_SIMPLE_DUCK_FOREVER: // 0x4C1B64
        Set(new CTaskSimpleDuck{DUCK_STANDALONE, 0x967Fu, -1});
        return;
    case TASK_COMPLEX_LEAVE_CAR: { // 0x4C1BE0
        if (!m_Ped->m_pVehicle || !m_Ped->bInVehicle) {
            return;
        }
        Set(new CTaskComplexLeaveCar{m_Ped->m_pVehicle, 0, 0, false, true});
        return;
    }
    case TASK_COMPLEX_FLEE_ENTITY: // 0x4C1CEA: es CTaskComplexFleeEntity (0x65B930), no la variante "smart"
        Set(new CTaskComplexFleeEntity{ped, false, 60.f});
        return;
    case TASK_COMPLEX_SMART_FLEE_ENTITY: { // 0x4C1C4B
        if (IsWantedByCop()) {
            Set(new CTaskComplexFleeAnyMeans{ped, true, 60.f});
            return;
        }
        // Como el original: el estado de movimiento se deja en caminar solo para el evento "no gusta" (el EXE lo ajusta tras crear la tarea)
        const CEvent& ev = *e;
        Set(new CTaskComplexSmartFleeEntity{
            ped,
            false,
            60.f,
            1'000'000,
            1'000,
            1.f,
            ev.GetEventType() == EVENT_ACQUAINTANCE_PED_DISLIKE ? PEDMOVE_WALK : PEDMOVE_SPRINT
        });
        return;
    }
    case TASK_COMPLEX_FLEE_ANY_MEANS: // 0x4C1D32
        Set(new CTaskComplexFleeAnyMeans{ped, true, 60.f});
        return;
    case TASK_COMPLEX_SEEK_ENTITY_AIMING: // 0x4C212F
        Set(new CTaskComplexSeekEntityAiming{ped, 10.f, 5.f});
        return;
    case TASK_SMART_FLEE_ENTITY_WALKING: // 0x4C22D0: igual que la smart pero con estado de movimiento andando
        Set(new CTaskComplexSmartFleeEntity{ped, false, 60.f, 1'000'000, 1'000, 1.f, PEDMOVE_WALK});
        return;
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4C1D6B
        if (IsKillTaskAppropriate(m_Ped, ped, *e)) {
            Set(new CTaskComplexKillPedOnFoot{ped, -1});
            return;
        }
        Set(FleeInsteadOfKill(true));
        return;
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT_TIMED: { // 0x4C1E74
        if (IsKillTaskAppropriate(m_Ped, ped, *e)) {
            Set(new CTaskComplexKillPedOnFoot{ped, 10'000});
            return;
        }
        Set(FleeInsteadOfKill(true));
        return;
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT_STAND_STILL: { // 0x4C1F5B
        if (IsKillTaskAppropriate(m_Ped, ped, *e)) {
            m_Ped->bStayInSamePlace = true;
            Set(new CTaskComplexKillPedOnFoot{ped, -1});
            return;
        }
        Set(FleeInsteadOfKill(false));
        return;
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT_KINDA_STAND_STILL: { // 0x4C1FE7
        if (IsKillTaskAppropriate(m_Ped, ped, *e)) {
            m_Ped->bKindaStayInSamePlace = true;
            Set(new CTaskComplexKillPedOnFoot{ped, -1});
            return;
        }
        Set(FleeInsteadOfKill(false));
        return;
    }
    case TASK_SIMPLE_GUN_CTRL: // 0x4C20FA
        Set(new CTaskSimpleGunControl{
            ped,
            {},
            {},
            eGunCommand::NONE,
            1,
            10'000
        });
        return;
    case TASK_SIMPLE_GANG_DRIVEBY: { // 0x4C2164
        // Como el original: solo para pasajeros (el conductor sale), con la tarea simple de conducir activa
        if (!m_Ped->bInVehicle || !m_Ped->m_pVehicle || m_Ped == m_Ped->m_pVehicle->m_pDriver) {
            return;
        }
        if (!m_Ped->GetTaskManager().FindActiveTaskByType(TASK_SIMPLE_CAR_DRIVE)) {
            return;
        }
        // El EXE pide la informacion del arma con habilidad STD (no la del peaton)
        const auto wi = CWeaponInfo::GetWeaponInfo(m_Ped->GetActiveWeapon().m_Type, eWeaponSkill::STD);
        if (wi->GetFireType() != WEAPON_FIRE_INSTANT_HIT) {
            return;
        }
        const float targetRange = wi->m_fTargetRange;
        const float fovRange    = m_Ped->GetIntelligence()->GetPedFOVRange() + 5.f;
        const float maxRange    = targetRange > fovRange ? targetRange : fovRange; // x87: NaN y empate dejan fovRange
        const float dist        = (ped->GetPosition() - m_Ped->GetPosition()).Magnitude(); // con sqrt
        if (!(dist < maxRange)) {
            Set(nullptr);
            return;
        }
        Set(new CTaskSimpleGangDriveBy{ // 0x4C229B
            ped,
            nullptr,
            maxRange,
            90,
            eDrivebyStyle::AI_ALL_DIRN,
            false
        });
        return;
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH: // 0x4C22A5
        Set(new CTaskComplexKillPedOnFootStealth{ped});
        return;
    case TASK_COMPLEX_KILL_PED_AND_REENTER_CAR: { // 0x4C232F
        if (!m_Ped->m_pVehicle) {
            Set(new CTaskComplexKillPedOnFoot{ped, -1});
            return;
        }
        // Como el original: primero la secuencia, luego matar, luego conducir sin rumbo
        const auto seq = new CTaskComplexSequence;
        seq->AddTask(new CTaskComplexKillPedOnFoot{ped, -1});
        seq->AddTask(new CTaskComplexCarDriveWander{m_Ped->m_pVehicle});
        Set(seq);
        return;
    }
    case TASK_COMPLEX_KILL_CRIMINAL: { // 0x4C1E38
        if (ped->IsPlayer()) {
            Set(nullptr);
            return;
        }
        Set(new CTaskComplexKillCriminal{ped, true});
        return;
    }
    default: // Como el original: el resto de tipos no escribe nada
        return;
    }
}

// 0x4C1910
void CEventHandler::ComputePedToChaseResponse(CEventPedToChase* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: sin peaton no se escribe la tarea de respuesta
    if (!e->m_ped) {
        return;
    }
    m_EventResponseTask = new CTaskComplexSeekEntity<>{
        e->m_ped,
        30'000,
        1'000,
        1.f,
        2.f,
        2.f,
        true,
        true
    };
}

// 0x4B9B50
void CEventHandler::ComputePedToFleeResponse(CEventPedToFlee* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: sin peaton no se escribe la tarea de respuesta
    if (!e->m_ped) {
        return;
    }
    m_Ped->Say(CTX_GLOBAL_CRIMINAL_PLEAD);
    m_EventResponseTask = new CTaskComplexSmartFleeEntity{e->m_ped, true, 100.f, -1};
}

// 0x4BF9B0
void CEventHandler::ComputePersonalityResponseToDamage(CEventDamage* e, CPed* src) {
    // Como el original: una tarea no contemplada NO escribe m_EventResponseTask (solo TASK_NONE y TASK_SIMPLE_BE_DAMAGED escriben null)
    switch ((int16)e->m_TaskId) {
    case TASK_NONE: // 0x4BFA10
    case TASK_SIMPLE_BE_DAMAGED:
        m_EventResponseTask = nullptr;
        return;
    case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR: // 0x4BFA2B - sin {}: el EXE no pone a cero la tarea
        m_EventResponseTask = new CTaskComplexUseClosestFreeScriptedAttractor;
        return;
    case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN: // 0x4BFA57
        m_EventResponseTask = new CTaskComplexUseClosestFreeScriptedAttractorRun;
        return;
    case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT: // 0x4BFA81
        m_EventResponseTask = new CTaskComplexUseClosestFreeScriptedAttractorSprint;
        return;
    case TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP: // 0x4BFAAB
        m_EventResponseTask = new CTaskComplexDiveFromAttachedEntityAndGetUp{ 0 };
        return;
    case TASK_SIMPLE_DUCK_FOREVER: // 0x4BFAEC
        m_EventResponseTask = new CTaskSimpleDuck{ DUCK_STANDALONE, 0xE0FF, -1 };
        return;
    case TASK_SIMPLE_DUCK: { // 0x4BFB1F - si ya hay una tarea de agacharse no se escribe nada
        if (m_Ped->GetIntelligence()->GetTaskDuck(true)) {
            return;
        }
        m_EventResponseTask = new CTaskSimpleDuck{ DUCK_STANDALONE, (uint16)CGeneral::GetRandomIntInRange(2000, 5000), -1 };
        return;
    }
    case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE: { // 0x4BFB79
        const auto veh = m_Ped->m_pVehicle;
        if (veh && veh->IsDriver(m_Ped)) {
            m_EventResponseTask = new CTaskComplexCarDriveMissionFleeScene{ veh };
            return;
        }
        if (m_Ped->bWantedByPolice && src->m_nPedType == PED_TYPE_COP) { // 0x4BFBAA
            m_EventResponseTask = new CTaskComplexFleeAnyMeans{ src, true, fSafeDistance };
            return;
        }
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{ src, false, fSafeDistance }; // 0x4BFBDF
        return;
    }
    case TASK_COMPLEX_CAR_DRIVE_MISSION_KILL_PED: { // 0x4BFDD7
        if (m_Ped->bInVehicle && m_Ped->m_pVehicle) {
            m_EventResponseTask = new CTaskComplexCarDriveMissionKillPed{ m_Ped->m_pVehicle, src };
        } else {
            m_EventResponseTask = new CTaskComplexKillPedOnFoot{ src }; // 0x4BFE26
        }
        return;
    }
    case TASK_COMPLEX_FLEE_ENTITY: // 0x4BFD1B
        if (m_Ped->bWantedByPolice) {
            m_EventResponseTask = new CTaskComplexFleeAnyMeans{ src, true, fSafeDistance };
        } else {
            m_EventResponseTask = new CTaskComplexFleeEntity{ src, false, fSafeDistance };
        }
        break;
    case TASK_COMPLEX_SMART_FLEE_ENTITY: // 0x4BFC58
        if (m_Ped->bWantedByPolice && src->m_nPedType == PED_TYPE_COP) {
            m_EventResponseTask = new CTaskComplexFleeAnyMeans{ src, true, fSafeDistance };
        } else {
            m_EventResponseTask = new CTaskComplexSmartFleeEntity{ src, false, fSafeDistance };
        }
        break;
    case TASK_COMPLEX_FLEE_ANY_MEANS: // 0x4BFE5F
        m_EventResponseTask = new CTaskComplexFleeAnyMeans{ src, true, fSafeDistance };
        break;
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4BFFDA
        if (IsKillTaskAppropriate(m_Ped, src, *e)) {
            m_EventResponseTask = new CTaskComplexKillPedOnFoot{ src };
            return;
        }
        if (m_Ped->bWantedByPolice && src->m_nPedType == PED_TYPE_COP) { // 0x4C0024
            m_EventResponseTask = new CTaskComplexFleeAnyMeans{ src, true, fSafeDistance };
            return;
        }
        const auto veh = m_Ped->bInVehicle ? m_Ped->m_pVehicle : nullptr; // 0x4C0095
        if (veh && veh->IsDriver(m_Ped)) {
            m_EventResponseTask = new CTaskComplexCarDriveMissionFleeScene{ veh };
            return;
        }
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{ src, false, fSafeDistance }; // 0x4C00E0
        return;
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH: // 0x4BFFAB
        m_EventResponseTask = new CTaskComplexKillPedOnFootStealth{ src };
        return;
    case TASK_COMPLEX_KILL_CRIMINAL: // 0x4BFF11
        if (src->IsPlayer()) {
            m_EventResponseTask = nullptr;
            return;
        }
        if (IsKillTaskAppropriate(m_Ped, src, *e)) {
            m_EventResponseTask = new CTaskComplexKillCriminal{ src, false };
        } else {
            m_EventResponseTask = new CTaskComplexSmartFleeEntity{ src, false, fSafeDistance };
        }
        return;
    default:
        return;
    }

    // Como el original (0x4BFEBC): las huidas hacen que el ped diga algo despues de guardar la tarea
    m_Ped->Say(CTX_GLOBAL_RUN_FROM_FIGHT);
}

// 0x4B8CE0
void CEventHandler::ComputePlayerCollisionWithPedResponse(CEventPlayerCollisionWithPed* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: sin victima no se escribe nada (ni la tarea de respuesta ni la de "decir")
    const auto victim = e->m_victim;
    if (!victim) {
        return;
    }
    const auto plyr = m_Ped;

    // Como el original: producto escalar (z*fz + y*fy) + x*fx en float; "delante" = !(dot < 0 || NaN), "detras" = (dot <= 0 y no NaN)
    const auto toVictim = victim->GetPosition() - plyr->GetPosition();
    const auto Dot      = [&](const CVector& f) { return (toVictim.z * f.z + toVictim.y * f.y) + toVictim.x * f.x; };
    const bool front    = Dot(plyr->GetForward()) >= 0.f;
    const bool behind   = Dot(victim->GetForward()) <= 0.f;

    // Como el original: si la victima sigue a un lider en formacion solo se dice algo y la respuesta se pone a null
    if (victim->GetTaskManager().GetActiveTaskAs<CTaskComplexFollowLeaderInFormation>()) {
        m_SayTask           = new CTaskSimpleSay{CTX_GLOBAL_BUMP};
        m_EventResponseTask = nullptr;
        return;
    }

    CTask* response = nullptr;
    bool   annoy    = false;

    const auto Say  = [&] { m_SayTask = new CTaskSimpleSay{CTX_GLOBAL_BUMP}; };
    const auto Look = [&](int32 time) {
        g_ikChainMan.LookAt("CompPlayerCollPedResp", plyr, victim, time, BONE_HEAD, nullptr, true, 0.25f, 500, 3, false);
    };
    const auto HitSide = [&] { return CPedGeometryAnalyser::ComputePedHitSide(*victim, *plyr); };

    const int32 m = static_cast<int32>(e->GetMoveState());
    const int32 v = static_cast<int32>(e->GetVictimMoveState());
    const auto  VictimMoving = [&] { return v == PEDMOVE_WALK || v == PEDMOVE_RUN || v == PEDMOVE_SPRINT; }; // 4, 6, 7

    // 0x4B8FBF
    const auto SprintTail = [&] {
        if ((m == PEDMOVE_RUN || m == PEDMOVE_SPRINT) && (v == PEDMOVE_STILL || v == PEDMOVE_NONE) && front && m == PEDMOVE_SPRINT) {
            Look(1000);
            Say();
        }
    };

    if (m == PEDMOVE_WALK) { // 0x4B8DF1
        if (v == PEDMOVE_WALK) {
            if (front) { // 0x4B8E07: la mirada va antes de la tarea de decir y no molesta al jugador
                Look(2000);
                Say();
            } else if (behind) { // 0x4B8E59
                Look(2000);
                Say();
                annoy = true;
            }
        } else {
            SprintTail();
        }
    } else if ((m == PEDMOVE_RUN || m == PEDMOVE_SPRINT) && VictimMoving()) { // 0x4B904D
        if (front) {
            if (behind) { // 0x4B9065
                bool fall = false;
                if (v != PEDMOVE_WALK) { // 0x4B906A
                    const float pw = plyr->m_pStats->m_fDefendWeakness;
                    if (pw > 1.5f) {
                        const float vw = victim->m_pStats->m_fDefendWeakness;
                        fall           = !(vw > 1.5f) || pw > vw;
                    }
                }
                if (fall) { // 0x4B90DD
                    response = new CTaskComplexFallAndGetUp{(int32)HitSide(), false};
                } else { // 0x4B90A6
                    Say();
                    Look(2000);
                    if (m == PEDMOVE_SPRINT) {
                        response = new CTaskComplexHitResponse{(eDirection)HitSide()};
                    }
                }
                annoy = true;
            } else { // 0x4B91BA
                Say();
                Look(2000);
            }
        } else if (behind) { // 0x4B921D
            Say();
            Look(2000);
            annoy = true;
        }
    } else if ((m == PEDMOVE_STILL || m == PEDMOVE_NONE) && VictimMoving() && behind) { // 0x4B8F2A
        const auto side = HitSide();
        if (v != PEDMOVE_WALK) {
            response = new CTaskComplexHitResponse{(eDirection)side};
        }
        Say();
        Look(2000);
        annoy = true;
    } else {
        SprintTail();
    }

    if (annoy) { // 0x4B9282
        static_cast<CPlayerPed*>(plyr)->AnnoyPlayerPed(false);
    }
    m_EventResponseTask = response;
}

// 0x4BB280
void CEventHandler::ComputePlayerWantedLevelResponse(CEvent* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: el EXE crea siempre un CTaskComplexPolicePursuit (0x68BA70), sin inicializacion por valor
    m_EventResponseTask = new CTaskComplexPolicePursuit;
}

// 0x4C2610
void CEventHandler::ComputePotentialPedCollideResponse(CEventPotentialWalkIntoPed* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: sin peaton no se escribe nada; los tipos de tarea no soportados tampoco
    if (!e->m_ped) {
        return;
    }
    int32 moveState = PEDMOVE_WALK;
    if (tsimplest && CTask::IsGoToTask(tsimplest)) {
        moveState = static_cast<int32>(e->m_moveState);
    }
    switch (e->m_TaskId) {
    case TASK_NONE: // 0x4C281B
        m_EventResponseTask = nullptr;
        return;
    case TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING: { // 0x4C26F7
        const auto tGangFollower = m_Ped->GetTaskManager().Find<CTaskComplexGangFollower>();
        // Como el original: comparacion con signo; la distancia al lider es 2D (z a cero, (x*x + y*y) + z*z) y sin sqrt
        if (moveState > static_cast<int32>(PEDMOVE_WALK) && tGangFollower && tGangFollower->m_Leader) {
            const auto gangLeader   = tGangFollower->m_Leader;
            const int32 leaderMoveState = gangLeader->GetPlayerData() // El EXE comprueba m_pPlayerData, no el tipo de peaton
                ? static_cast<int32>(gangLeader->m_nMoveState)
                : static_cast<int32>(gangLeader->GetIntelligence()->GetMoveStateFromGoToTask());
            const auto tSeekEntity = m_Ped->GetTaskManager().Find<CTaskComplexSeekEntity<>>();
            if (tSeekEntity && leaderMoveState < static_cast<int32>(PEDMOVE_RUN)) {
                auto pedToLeader = gangLeader->GetPosition() - m_Ped->GetPosition();
                pedToLeader.z    = 0.f;
                const float distSq = (pedToLeader.x * pedToLeader.x + pedToLeader.y * pedToLeader.y) + pedToLeader.z * pedToLeader.z;
                const float radius = tSeekEntity->GetMoveStateRadius();
                const float radiusSq = radius * radius;
                if (distSq - radiusSq < radiusSq) {
                    moveState = PEDMOVE_WALK;
                }
            }
        }
        m_EventResponseTask = new CTaskComplexAvoidOtherPedWhileWandering{e->m_ped, e->m_targetPoint, static_cast<eMoveState>(moveState), tGangFollower != nullptr};
        return;
    }
    case TASK_COMPLEX_PARTNER_SHOVE:
        m_EventResponseTask = new CTaskComplexPartnerShove{
            "CompPotPedCollResp",
            e->m_ped,
            true,
            0.5f,
            1,
            {}
        };
        return;
    default:
        return;
    }
}

// 0x4BBCD0
void CEventHandler::ComputePotentialWalkIntoFireResponse(CEventPotentialWalkIntoFire* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: sin tarea simple de ir a un punto no se escribe nada, y los tipos no soportados tampoco
    if (!tsimplest || !CTask::IsGoToTask(tsimplest)) {
        return;
    }
    switch (e->m_TaskId) {
    case TASK_COMPLEX_SMART_FLEE_POINT:
        m_EventResponseTask = new CTaskComplexSmartFleePoint{e->m_firePos, false, 60.f, 1'000'000};
        return;
    case TASK_COMPLEX_WALK_ROUND_FIRE:
        m_EventResponseTask = new CTaskComplexWalkRoundFire{
            e->m_moveState,
            e->m_firePos,
            e->m_radius,
            static_cast<CTaskSimpleGoTo*>(tsimplest)->m_vecTargetPoint
        };
        return;
    case TASK_COMPLEX_FLEE_POINT:
        m_EventResponseTask = new CTaskComplexFleePoint{e->m_firePos, false, 60.f, 1'000'000};
        return;
    case TASK_NONE:
        m_EventResponseTask = nullptr;
        return;
    default:
        return;
    }
}

// 0x4BAA30
void CEventHandler::ComputeReallyLowHealthResponse(CEventHealthReallyLow* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: solo se escribe la tarea para TASK_NONE y TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP
    switch (e->m_TaskId) {
    case TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP:
        m_EventResponseTask = new CTaskComplexDiveFromAttachedEntityAndGetUp{0};
        return;
    case TASK_NONE:
        m_EventResponseTask = nullptr;
        return;
    default:
        return;
    }
}

// 0x4B97B0
void CEventHandler::ComputeReviveResponse(CEventRevived* e, CTask* tactive, CTask* tsimplest) {
    m_Ped->m_fHealth                = 100.f;
    m_Ped->SetUsesCollision(true);
    m_Ped->bKnockedUpIntoAir        = false;
    m_Ped->bKnockedOffBike          = false;
    m_Ped->bKilledByStealth         = false;
    m_Ped->physicalFlags.bRenderScorched = false;

    m_Ped->SetPedState(PEDSTATE_IDLE);
    m_Ped->RestartNonPartialAnims();

    // Como el original: primero se crean las tareas de levantarse y de decir, y solo despues la de deambular
    m_EventResponseTask = new CTaskComplexGetUpAndStandStill{};
    m_SayTask           = new CTaskSimpleSay{CTX_GLOBAL_SAVED};

    const auto tm = &m_Ped->GetTaskManager();
    if (!tm->GetTaskPrimary(TASK_PRIMARY_PRIMARY) && !tm->GetTaskPrimary(TASK_PRIMARY_DEFAULT)) {
        tm->SetTask(
            CTaskComplexWander::GetWanderTaskByPedType(m_Ped),
            TASK_PRIMARY_DEFAULT
        );
    }
}

// 0x4BA7C0
void CEventHandler::ComputeScriptCommandResponse(CEventScriptCommand* e, CTask* tactive, CTask* tsimplest) {
    const auto tm = &m_Ped->GetTaskManager();
    const auto taskIdx = e->m_primaryTaskIndex == TASK_PRIMARY_PRIMARY
        ? TASK_PRIMARY_PRIMARY
        : TASK_PRIMARY_DEFAULT;
    const auto t = tm->GetTaskPrimary(taskIdx);
    if (t && !t->MakeAbortable(m_Ped, ABORT_PRIORITY_URGENT, e)) {
        t->MakeAbortable(m_Ped, ABORT_PRIORITY_LEISURE, e);
        const auto ce = static_cast<CEventScriptCommand*>(m_Ped->GetEventGroup().Add(e));
        if (const auto tr = CPedScriptedTaskRecord::GetRecordAssociatedWithEvent(e)) {
            tr->AssociateWithEvent(ce);
        }
    } else {
        const auto ct = e->CloneScriptTask();
        tm->ClearTaskEventResponse();
        tm->SetTask(ct, taskIdx, true);
        if (const auto tr = CPedScriptedTaskRecord::GetRecordAssociatedWithEvent(e)) {
            tr->AssociateWithTask(ct);
        }
    }
}

// 0x4BC050
void CEventHandler::ComputeSeenCopResponse(CEventSeenCop* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: sin ped o con una tarea no contemplada no se escribe m_EventResponseTask
    if (!e->m_AcquaintancePed) {
        return;
    }
    switch (e->m_TaskId) {
    case TASK_NONE: // Como el original: 0x4BC135 (el EXE compara con 200 = TASK_NONE)
        m_EventResponseTask = nullptr;
        return;
    case TASK_COMPLEX_SMART_FLEE_ENTITY:
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{ e->m_AcquaintancePed, false, 60.f };
        return;
    case TASK_COMPLEX_KILL_PED_ON_FOOT:
        m_EventResponseTask = new CTaskComplexKillPedOnFoot{ e->m_AcquaintancePed };
        return;
    default:
        return;
    }
}

// 0x4C35F0
void CEventHandler::ComputeSeenPanickedPedResponse(CEventSeenPanickedPed* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: el historial es el del PED ORIGEN del evento (intel+0x34 -> +4), no el de este manejador
    const auto thisEventSrc = e->GetSourceEntity();
    if (!thisEventSrc) {
        return;
    }
    const auto currEvnt = static_cast<CPed*>(thisEventSrc)->GetEventHandlerHistory().GetCurrentEvent();
    if (!currEvnt) {
        return;
    }
    const auto currEvntSrc = currEvnt->GetSourceEntity();
    if (!currEvntSrc) {
        return;
    }
    switch (e->m_TaskId) {
    case TASK_SIMPLE_DUCK_FOREVER:
        m_EventResponseTask = new CTaskSimpleDuck{ DUCK_STANDALONE, 57599u, -1 };
        return;
    case TASK_COMPLEX_FLEE_ENTITY:
        m_EventResponseTask = new CTaskComplexFleeEntity{ currEvntSrc, true, 45.f };
        return;
    case TASK_COMPLEX_SMART_FLEE_ENTITY:
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{ currEvntSrc, true, 45.f };
        return;
    default:
        return;
    }
}

// 0x4B99F0
void CEventHandler::ComputeSexyPedResponse(CEventSexyPed* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: sin ped o con una tarea no contemplada no se escribe m_EventResponseTask
    if (!e->m_SexyPed) {
        return;
    }
    switch (e->m_TaskId) {
    case TASK_NONE:
        m_EventResponseTask = nullptr;
        return;
    case TASK_COMPLEX_GANG_HASSLE_PED:
        m_EventResponseTask = new CTaskGangHasslePed{ e->m_SexyPed, 0, 10'000, 30'000 };
        return;
    default:
        return;
    }
}

// 0x4B9AA0
void CEventHandler::ComputeSexyVehicleResponse(CEventSexyVehicle* e, CTask* tactive, CTask* tsimplest) {
    if (!e->m_vehicle) {
        return;
    }
    g_InterestingEvents.Add(CInterestingEvents::EType::INTERESTING_EVENT_8, e->m_vehicle);
    // Como el original: la tarea se crea ANTES del LookAt y se guarda después
    const auto task = new CTaskSimpleStandStill(5000, false, false, 8.0f);
    g_ikChainMan.LookAt("CompSexyVhclResp", m_Ped, e->m_vehicle, 5000, BONE_UNKNOWN, nullptr, true, 0.25f, 500, 3, false);
    m_EventResponseTask = task;
}

// 0x4BC710
void CEventHandler::ComputeShotFiredResponse(CEventGunShot* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: solo se escribe m_EventResponseTask en las rutas que lo hacen (el resto lo deja intacto)
    if (!e->m_firedBy) {
        return;
    }
    switch (e->m_TaskId) {
    case TASK_COMPLEX_TURN_TO_FACE_ENTITY: // 0x4BCAE2
        m_EventResponseTask = new CTaskComplexTurnToFaceEntityOrCoord{ e->m_firedBy, 0.5f, 0.2f };
        return;
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4BC8F6
        if (!e->m_firedBy->GetIsTypePed()) {
            return;
        }
        const auto firedByPed = e->m_firedBy->AsPed();
        if (firedByPed->m_nPedType == m_Ped->m_nPedType) {
            return;
        }
        if (IsKillTaskAppropriate(m_Ped, firedByPed, *e)) {
            m_EventResponseTask = new CTaskComplexKillPedOnFoot{ firedByPed }; // 0x4BC950
            return;
        }
        if (m_Ped->IsInVehicle()) {
            if (m_Ped->m_pVehicle->IsDriver(m_Ped)) {
                m_EventResponseTask = new CTaskComplexCarDriveMissionFleeScene{ m_Ped->m_pVehicle }; // 0x4BC9A8
                return;
            }
            if (m_Ped->m_pVehicle->m_pDriver) {
                m_EventResponseTask = nullptr; // 0x4BCA44
                return;
            }
        }
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{ e->m_firedBy, false, 60.f }; // 0x4BC9CE
        return;
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH: // 0x4BCB14 - Como el original: NO comprueba que sea ped
        m_EventResponseTask = new CTaskComplexKillPedOnFootStealth{ static_cast<CPed*>(e->m_firedBy) };
        return;
    case TASK_COMPLEX_KILL_CRIMINAL: { // 0x4BCA16
        if (!e->m_firedBy->GetIsTypePed()) {
            return;
        }
        const auto firedByPed = e->m_firedBy->AsPed();
        if (firedByPed->m_nPedType == m_Ped->m_nPedType) {
            return;
        }
        if (firedByPed->IsPlayer()) {
            m_EventResponseTask = nullptr; // 0x4BCA44
            return;
        }
        if (IsKillTaskAppropriate(m_Ped, firedByPed, *e)) { // 0x4BCA51
            m_EventResponseTask = new CTaskComplexKillCriminal{ firedByPed, false }; // 0x4BCA7F
            return;
        }
        // Como el original: 0x4BCAC8 guarda la tarea y luego fuerza m_moveState = PEDMOVE_WALK (4), no el SPRINT del constructor
        const auto tFlee = new CTaskComplexSmartFleeEntity{ firedByPed, false, 60.f };
        m_EventResponseTask = tFlee;
        tFlee->m_moveState = PEDMOVE_WALK;
        return;
    }
    case TASK_COMPLEX_SMART_FLEE_ENTITY: // 0x4BC88F
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{ e->m_firedBy, true, 60.f };
        return;
    case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE: { // 0x4BC7EB
        const auto v = m_Ped->m_pVehicle;
        if (v && v->IsDriver(m_Ped)) {
            m_EventResponseTask = new CTaskComplexCarDriveMissionFleeScene{ v }; // 0x4BC835
            return;
        }
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{ v, false, 60.f }; // 0x4BC885
        return;
    }
    case TASK_SIMPLE_DUCK_FOREVER: // 0x4BC7B9
        m_EventResponseTask = new CTaskSimpleDuck{ DUCK_STANDALONE, 0x967Fu, -1 };
        return;
    case TASK_NONE: // 0x4BCA44
        m_EventResponseTask = nullptr;
        return;
    case TASK_SIMPLE_DUCK: { // 0x4BC769 - Como el original: si YA hay una tarea de agacharse NO se escribe nada
        if (m_Ped->GetIntelligence()->GetTaskDuck(true)) {
            return;
        }
        m_EventResponseTask = new CTaskSimpleDuck{ DUCK_STANDALONE, 3000, -1 };
        return;
    }
    default:
        return;
    }
}

// 0x4BBE30
void CEventHandler::ComputeShotFiredWhizzedByResponse(CEventGunShotWhizzedBy* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: GetSourceEntity se evalúa siempre; sin origen o con tarea no contemplada no se escribe nada
    const auto esrc = e->GetSourceEntity();
    if (!esrc) {
        return;
    }
    switch (e->m_TaskId) {
    case TASK_NONE:
        m_EventResponseTask = nullptr; // 0x4BBE91
        return;
    case TASK_SIMPLE_DUCK_WHILE_SHOTS_WHIZZING: { // 0x4BBE6A
        // Como el original: solo se reinicia si el contador de disparos es >= 0 (jle con signo); si no, se sigue al código siguiente
        if (tsimplest && tsimplest->GetTaskType() == TASK_SIMPLE_DUCK) {
            const auto tDuck = static_cast<CTaskSimpleDuck*>(tsimplest);
            if (tDuck->m_ShotWhizzingCounter >= 0) {
                tDuck->RestartTask(m_Ped);
                m_EventResponseTask = nullptr;
                return;
            }
        }
        if (const auto tDuck = m_Ped->GetIntelligence()->GetTaskDuck(true)) {
            if (tDuck->m_ShotWhizzingCounter >= 0) {
                tDuck->RestartTask(m_Ped);
                m_EventResponseTask = nullptr;
                return;
            }
        }
        // Como el original: 0x407180 = min + (int)(rand * 1/32768 * (max - min)) (el truncado a uint16 lo hace el constructor)
        m_EventResponseTask = new CTaskSimpleDuck{ DUCK_STANDALONE_WEAPON_CROUCH, static_cast<uint16>(CGeneral::GetRandomIntInRange(3'000, 5'000)), 1'000 };
        return;
    }
    default:
        return;
    }
}

// 0x4BB050
void CEventHandler::ComputeSignalAtPedResponse(CEventSignalAtPed* e, CTask* tactive, CTask* tsimplest) {
    m_EventResponseTask = [&]() -> CTask* {
        return new CTaskComplexSignalAtPed{ e->m_SignalAt, e->m_InitialPause, e->m_bPlayAnimAtEnd };
    }();
}

// 0x4BB800
void CEventHandler::ComputeSpecialResponse(CEventSpecial* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: las rutas sin vehículo o con una tarea no contemplada NO escriben m_EventResponseTask
    switch (e->m_TaskId) {
    case TASK_NONE:
    case TASK_COMPLEX_KILL_CRIMINAL: // 0x4BBB22 - Como el original: 1105 también escribe null
        m_EventResponseTask = nullptr;
        return;
    case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR: // 0x4BB882 - sin {}: el EXE no pone a cero la tarea
        m_EventResponseTask = new CTaskComplexUseClosestFreeScriptedAttractor;
        return;
    case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN: // 0x4BB858
        m_EventResponseTask = new CTaskComplexUseClosestFreeScriptedAttractorRun;
        return;
    case TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT: // 0x4BB8AE
        m_EventResponseTask = new CTaskComplexUseClosestFreeScriptedAttractorSprint;
        return;
    case TASK_COMPLEX_LEAVE_CAR_AND_FLEE: { // 0x4BB8E8
        const auto v = m_Ped->m_pVehicle;
        if (!v) {
            return;
        }
        m_EventResponseTask = new CTaskComplexLeaveCarAndFlee{ v, FindPlayerPed()->GetPosition(), TARGET_DOOR_FRONT_LEFT, 0, false }; // 0x4BB943
        return;
    }
    case TASK_COMPLEX_LEAVE_CAR: { // 0x4BB94D
        const auto v = m_Ped->m_pVehicle;
        if (!v) {
            return;
        }
        m_EventResponseTask = new CTaskComplexLeaveCar{ v, TARGET_DOOR_FRONT_LEFT, 0, true, false }; // 0x4BB98E
        return;
    }
    case TASK_COMPLEX_LEAVE_CAR_AND_WANDER: { // 0x4BB998
        const auto v = m_Ped->m_pVehicle;
        if (!v) {
            return;
        }
        m_EventResponseTask = new CTaskComplexLeaveCarAndWander{ v }; // 0x4BB9DC
        return;
    }
    case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE: // 0x4BBAB0 - Como el original: sin vehículo (nullptr), no tiene sentido
        m_EventResponseTask = new CTaskComplexCarDriveMissionFleeScene{ nullptr };
        return;
    case TASK_COMPLEX_SMART_FLEE_POINT: // 0x4BBA5E
        m_EventResponseTask = new CTaskComplexSmartFleePoint{ FindPlayerPed()->GetPosition(), false, 1000.f, 100'000 };
        return;
    case TASK_COMPLEX_SMART_FLEE_ENTITY: // 0x4BBA05
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{ FindPlayerPed(), false, 60.f };
        return;
    case TASK_COMPLEX_KILL_PED_ON_FOOT: // 0x4BBADC
        m_EventResponseTask = new CTaskComplexKillPedOnFoot{ FindPlayerPed() };
        return;
    case TASK_COMPLEX_ARREST_PED: // 0x4BBB3C
        m_EventResponseTask = new CTaskComplexArrestPed{ FindPlayerPed() };
        return;
    default:
        return;
    }
}

// 0x4BD6A0
void CEventHandler::ComputeVehicleCollisionResponse(CEventVehicleCollision* e, CTask* tactive, CTask* tsimplest) {
    const auto veh = e->m_vehicle;
    if (!veh) { // 0x4BD6C6
        return;
    }

    m_Ped->bPushedAlongByCar = false;

    // Como el original: velocidad del vehiculo = sqrt((x*x + y*y) + z*z) en float
    const auto& vehMoveSpeed = veh->GetMoveSpeed();
    const float vehSpeed     = sqrtf((vehMoveSpeed.x * vehMoveSpeed.x + vehMoveSpeed.y * vehMoveSpeed.y) + vehMoveSpeed.z * vehMoveSpeed.z);

    if (e->m_evadeType == VEHICLE_EVADE_BY_HITSIDE) { // 0x4BD715
        m_EventResponseTask = new CTaskComplexEvasiveStep{ veh, CTaskComplexHitPedWithCar::ComputeEvasiveStepMoveDir(m_Ped, veh) }; // 0x4BD74A
    } else if (vehSpeed > 0.05f) { // 0x4BD754 (constante 0x85AAD0)
        float damage = e->m_fDamageIntensity;
        if (m_Ped->IsPlayer() && m_Ped->bIsStanding) { // 0x4BD76B
            // Como el original: (adelante * desplazamiento.y) . normal, con el producto escalar (z + y) + x
            const auto& fwd   = m_Ped->GetForward();
            const auto  shift = m_Ped->m_vecAnimMovingShiftLocal.y;
            const CVector v{ fwd.x * shift, fwd.y * shift, fwd.z * shift };
            const auto& n   = e->m_impactNormal;
            const auto  dot = (v.z * n.z + v.y * n.y) + v.x * n.x;
            if (dot < 0.f) {
                const auto val = dot * m_Ped->m_fMass + damage;
                damage = (0.f > val) ? 0.f : val; // 0x4BD7B4
            }
        }
        if (m_Ped->IsPlayer()) { // 0x4BD7D9
            if (damage > 20.f) {
                damage = 20.f;
            }
            m_EventResponseTask = new CTaskComplexHitPedWithCar{ veh, damage };
        } else {
            m_EventResponseTask = new CTaskComplexHitPedWithCar{ veh, e->m_fDamageIntensity }; // 0x4BD82C
        }
        if (m_Ped->m_pEntityIgnoredCollision != veh) { // 0x4BD861
            m_Ped->bPushedAlongByCar = true;
        }
    } else { // 0x4BD884
        if ((!m_Ped->IsPlayer() || m_Ped->GetIntelligence()->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PRIMARY))
            && tsimplest
            && CTask::IsGoToTask(tsimplest)
        ) { // 0x4BD8AB
            if (veh == m_Ped->m_standingOnEntity) { // 0x4BD8CA
                // Como el original: compara la rotacion actual con la de apuntado
                if (std::fabs(m_Ped->m_fCurrentRotation - m_Ped->m_fAimingRotation) < 0.01f && CPedGeometryAnalyser::CanPedJumpObstacle(*m_Ped, *veh)) {
                    m_EventResponseTask = new CTaskComplexJump{ CTaskComplexJump::eForceClimb::OK }; // 0x4BD903
                }
            } else {
                const auto tGoTo     = static_cast<CTaskSimpleGoTo*>(tsimplest);
                auto       moveState = (eMoveState)(int32)e->m_moveState; // 0x4BD936 (byte con signo)
                if (moveState == PEDMOVE_STILL) {
                    return; // Como el original: sale sin pasar por el final
                }
                if (m_Ped->GetGroup()) {
                    moveState = PEDMOVE_RUN;
                }
                const auto isGoingForDoor = m_Ped->GetIntelligence()->IsPedGoingForCarDoor();
                m_EventResponseTask = new CTaskComplexWalkRoundCar{ moveState, tGoTo->m_vecTargetPoint, veh, isGoingForDoor, (uint8)e->m_DirectionToWalkRoundCar };
            }
        } else if (m_Ped->IsPlayer() && !m_Ped->bIsInTheAir) { // 0x4BD9A1
            if (m_Ped->GetPlayerData()->m_fMoveBlendRatio == 0.f && veh->m_pDriver) { // 0x4BD9DB
                m_Ped->AsPlayer()->AnnoyPlayerPed(false);
                g_ikChainMan.LookAt("CompVehicleCollResp", m_Ped, veh, 1300, BONE_UNKNOWN, nullptr, true, 0.25f, 500, 3, false);
                const auto wt = m_Ped->GetActiveWeapon().GetType();
                if (wt == WEAPON_UNARMED) {
                    m_PartialAnimTask = new CTaskSimpleShakeFist;
                } else {
                    const auto slot = m_Ped->GetWeaponSlot(wt);
                    if (slot == 2 || slot == 4 || slot == 1) { // 0x4BDA3B
                        m_PartialAnimTask = new CTaskSimpleShakeFist;
                    }
                }
            } else { // 0x4BDAAD
                g_ikChainMan.LookAt("CompVehicleCollResp", m_Ped, veh, 800, BONE_UNKNOWN, nullptr, true, 0.25f, 500, 3, false);
            }
        } else if (vehSpeed > 0.001f) { // 0x4BDAD9
            m_EventResponseTask = new CTaskComplexHitPedWithCar{ veh, e->m_fDamageIntensity };
        }
    }

    // Como el original (0x4BDB25): el que sufre el atropello se enfada con el jugador si este conduce el vehiculo
    if (m_EventResponseTask && m_EventResponseTask->GetTaskType() == TASK_COMPLEX_HIT_PED_WITH_CAR) {
        const auto drvr = veh->m_pDriver;
        if (drvr && drvr->IsPlayer()) {
            m_Ped->GetIntelligence()->IncrementAngerAtPlayer(2);
        }
    }
}

// 0x4C2FC0
void CEventHandler::ComputeVehicleDamageResponse(CEventVehicleDamage* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: las rutas que no crean tarea NO escriben m_EventResponseTask (solo se escribe donde el EXE guarda algo)
    const auto pedVeh   = m_Ped->m_pVehicle;
    const auto attacker = e->m_attacker;
    if (!pedVeh || pedVeh != e->m_vehicle) {
        return;
    }
    switch ((int16)e->m_TaskId) {
    case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE: { // 0x4C302D
        if (pedVeh->IsDriver(m_Ped)) {
            m_EventResponseTask = new CTaskComplexCarDriveMissionFleeScene{ m_Ped->m_pVehicle };
        } else {
            m_EventResponseTask = new CTaskComplexSmartFleeEntity{ m_Ped->m_pVehicle, false, fSafeDistance }; // 0x4C3052
        }
        return;
    }
    case TASK_COMPLEX_LEAVE_CAR_AND_FLEE: { // 0x4C306D - Como el original: el punto de huida es la posicion del VEHICULO
        const auto t = new CTaskComplexLeaveCarAndFlee{ m_Ped->m_pVehicle, m_Ped->m_pVehicle->GetPosition(), TARGET_DOOR_FRONT_LEFT, 0, false };
        m_EventResponseTask = t;
        return;
    }
    case TASK_COMPLEX_SEQUENCE: { // 0x4C30BE
        const auto seqIdx = m_Ped->GetIntelligence()->m_NextEventResponseSequence;
        if (seqIdx < 0 || !CTaskSequences::Get(seqIdx)->IsActive()) {
            return;
        }
        m_EventResponseTask = new CTaskComplexUseSequence{ seqIdx };
        return;
    }
    case TASK_COMPLEX_CAR_DRIVE_MISSION_KILL_PED: { // 0x4C310F
        if (!attacker || !(attacker->GetIsTypePed() || attacker->GetIsTypeVehicle())) {
            return;
        }
        if (e->GetSourceEntity() == FindPlayerPed(-1)
            && CTheScripts::IsPlayerOnAMission()
            && m_Ped->IsCreatedBy(PED_GAME)
            && e->m_weaponType == WEAPON_RAMMEDBYCAR
        ) {
            m_EventResponseTask = new CTaskComplexCarDriveMissionFleeScene{ m_Ped->m_pVehicle }; // 0x4C3159
            return;
        }
        m_EventResponseTask = new CTaskComplexCarDriveMissionKillPed{ m_Ped->m_pVehicle, attacker }; // 0x4C318C
        return;
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x4C32EF
        if (!attacker) {
            return;
        }
        if (attacker->GetIsTypePed()) {
            if (IsKillTaskAppropriate(m_Ped, attacker->AsPed(), *e)) { // 0x4C33AD
                m_EventResponseTask = new CTaskComplexKillPedOnFoot{ attacker->AsPed() };
                return;
            }
            const auto v = m_Ped->m_pVehicle; // 0x4C3317
            if (v && v->IsDriver(m_Ped)) {
                m_EventResponseTask = new CTaskComplexCarDriveMissionFleeScene{ m_Ped->m_pVehicle };
            } else {
                m_EventResponseTask = new CTaskComplexSmartFleeEntity{ m_Ped->m_pVehicle, false, fSafeDistance }; // 0x4C3360
            }
            return;
        }
        if (attacker->GetIsTypeVehicle()) { // 0x4C33E1 - Como el original: tambien con conductor nulo
            m_EventResponseTask = new CTaskComplexKillPedOnFoot{ attacker->AsVehicle()->m_pDriver };
        }
        return;
    }
    case TASK_COMPLEX_ROAD_RAGE: { // 0x4C327A
        if (!attacker) {
            return;
        }
        if (attacker->GetIsTypePed()) {
            m_EventResponseTask = new CTaskComplexRoadRage{ attacker->AsPed() };
        } else if (attacker->GetIsTypeVehicle()) {
            m_EventResponseTask = new CTaskComplexRoadRage{ attacker->AsVehicle()->m_pDriver };
        }
        return;
    }
    case TASK_COMPLEX_KILL_CRIMINAL: { // 0x4C31DD
        if (!attacker) {
            return;
        }
        if (attacker->GetIsTypePed()) {
            if (attacker->AsPed()->IsPlayer()) { // 0x4C3240
                m_EventResponseTask = nullptr;
            } else {
                m_EventResponseTask = new CTaskComplexKillCriminal{ attacker->AsPed(), false };
            }
        } else if (attacker->GetIsTypeVehicle()) {
            const auto drvr = attacker->AsVehicle()->m_pDriver;
            if (drvr && drvr->IsPlayer()) {
                m_EventResponseTask = nullptr;
            } else {
                m_EventResponseTask = new CTaskComplexKillCriminal{ drvr, false }; // Como el original: tambien con conductor nulo
            }
        }
        return;
    }
    default:
        return;
    }
}

// 0x4BA8B0
void CEventHandler::ComputeVehicleDiedResponse(CEventVehicleDied* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: sin vehículo NO se escribe m_EventResponseTask
    if (!e->m_vehicle) {
        return;
    }
    m_Ped->physicalFlags.bRenderScorched = true;
    m_Ped->m_fHealth = 0.f;
    if (m_Ped->IsInVehicle() && (m_Ped->m_pVehicle->IsBike() || m_Ped->m_pVehicle->IsSubQuad())) {
        ComputeKnockOffBikeResponse(e, tactive, tsimplest); // 0x4BA922
        return;
    }
    m_EventResponseTask = new CTaskComplexDie{ WEAPON_EXPLOSION, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0 };
}

// 0x?
void CEventHandler::ComputeVehicleHitAndRunResponse(CEventVehicleHitAndRun* e, CTask* tactive, CTask* tsimplest) {
    NOTSA_UNREACHABLE();
}

// 0x4BB2E0
void CEventHandler::ComputeVehicleOnFireResponse(CEventVehicleOnFire* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: las rutas no contempladas NO escriben m_EventResponseTask
    const auto veh = e->m_vehicle;
    if (!veh) {
        return;
    }
    // Como el original: el vehículo del evento debe ser EL MISMO en el que va el ped (0x4BB33C)
    if ((veh->GetStatus() == STATUS_WRECKED || veh->m_fHealth <= 0.f) && m_Ped->bInVehicle && m_Ped->m_pVehicle == veh) {
        if (veh->IsBike() || veh->IsSubQuad()) {
            ComputeKnockOffBikeResponse(e, tactive, tsimplest); // 0x4BB363
            return;
        }
        m_EventResponseTask = new CTaskComplexDie{ WEAPON_UNARMED, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0 }; // 0x4BB397 - Como el original: arma 0 (push esi), no WEAPON_EXPLOSION como en VehicleDied
        return;
    }
    switch (e->m_TaskId) {
    case TASK_COMPLEX_LEAVE_CAR: // 0x4BB3C3 - Como el original: (0, false, forceGetOut = true)
        m_EventResponseTask = new CTaskComplexLeaveAnyCar{ 0, false, true };
        return;
    case TASK_COMPLEX_LEAVE_CAR_AND_FLEE: { // 0x4BB3F1 - la secuencia se reserva ANTES que sus tareas
        const auto seq = new CTaskComplexSequence;
        seq->AddTask(new CTaskComplexLeaveAnyCar{ 0, false, true }); // 0x4BB43D
        seq->AddTask(new CTaskComplexSmartFleeEntity{ veh, false, 15.f }); // 0x4BB48B
        m_EventResponseTask = seq;
        return;
    }
    case TASK_COMPLEX_LEAVE_CAR_AND_WANDER: { // 0x4BB4BA
        const auto seq = new CTaskComplexSequence;
        seq->AddTask(new CTaskComplexLeaveAnyCar{ 0, false, true }); // 0x4BB506
        m_EventResponseTask = seq;
        return;
    }
    case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE: { // 0x4BB5AB
        const auto v = m_Ped->m_pVehicle;
        if (v && v->IsDriver(m_Ped)) {
            m_EventResponseTask = new CTaskComplexCarDriveMissionFleeScene{ v };
            return;
        }
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{ v, false, 60.f }; // 0x4BB5EF
        return;
    }
    case TASK_COMPLEX_FLEE_ENTITY: // 0x4BB565
        m_EventResponseTask = new CTaskComplexFleeEntity{ veh, false, 15.f };
        return;
    case TASK_COMPLEX_SMART_FLEE_ENTITY: // 0x4BB51F - Como el original: huye del vehículo del EVENTO
        m_EventResponseTask = new CTaskComplexSmartFleeEntity{ veh, false, 15.f };
        return;
    default:
        return;
    }
}

// 0x4C0BD0
void CEventHandler::ComputeVehiclePotentialCollisionResponse(CEventPotentialGetRunOver* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: las salidas sin tarea NO escriben m_EventResponseTask (solo se escribe donde el EXE guarda algo)
    const auto veh = e->m_Vehicle;
    if (!veh || m_Ped->bInVehicle) {
        return;
    }

    // 0x4C0C0F - mitad del ancho del modelo del vehiculo (el EXE usa el CColModel del modelo, no el de la instancia)
    const float vehBBMaxX = CModelInfo::GetModelInfo(veh->m_nModelIndex)->GetColModel()->GetBoundingBox().m_vecMax.x;

    const auto& pedPos = m_Ped->GetPosition();
    const auto& vehPos = veh->GetPosition();

    // 0x4C0C44 - el vehiculo esta delante del ped: (dy * fy + dz * fz) + dx * fx > 0 con el vector adelante del PED
    const auto pedFwd = m_Ped->GetForward();
    const auto dVeh   = vehPos - pedPos;
    const bool bVehInFront = ((dVeh.y * pedFwd.y + dVeh.z * pedFwd.z) + dVeh.x * pedFwd.x) > 0.f;

    // Como el original: arrays de CVector sin inicializar y llamadas que se hacen siempre
    CVector vehBB[4];
    CVector vehBBPlanes[4];
    float   vehBBPlanesDot[4];
    CVector dirToAvoidVehicle;
    CPedGeometryAnalyser::ComputeEntityBoundingBoxCorners(pedPos.z, *veh, vehBB);
    CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(pedPos.z, *veh, &vehBBPlanes, vehBBPlanesDot); // Como el original: la version con cache (0x5F3660)
    CPedGeometryAnalyser::ComputeMoveDirToAvoidEntity(*m_Ped, *veh, dirToAvoidVehicle);

    // 0x4C0D0A - centro del lado delantero (3 + 0) o trasero (2 + 1) del vehiculo
    const CVector frontCenter{ (vehBB[3].x + vehBB[0].x) * 0.5f, (vehBB[3].y + vehBB[0].y) * 0.5f, (vehBB[3].z + vehBB[0].z) * 0.5f };
    const CVector backCenter{ (vehBB[2].x + vehBB[1].x) * 0.5f, (vehBB[2].y + vehBB[1].y) * 0.5f, (vehBB[2].z + vehBB[1].z) * 0.5f };

    // 0x4C0D8E - velocidad del vehiculo en su eje adelante: (z + y) + x
    const auto  vehFwd   = veh->GetForward();
    const auto  vehRight = veh->GetRight();
    const auto& moveSpd  = veh->GetMoveSpeed();
    const float vehFwdSpeed = (vehFwd.z * moveSpd.z + vehFwd.y * moveSpd.y) + vehFwd.x * moveSpd.x;

    const auto& center = vehFwdSpeed > 0.f ? frontCenter : backCenter;
    const float cdx    = pedPos.x - center.x;
    const float cdy    = pedPos.y - center.y;
    const float cdz    = pedPos.z - center.z;
    const float pedToCenterFwdDot   = (cdz * vehFwd.z + cdy * vehFwd.y) + cdx * vehFwd.x;
    const float pedToCenterRightDot = (cdz * vehRight.z + cdy * vehRight.y) + cdx * vehRight.x;

    if (m_Ped->IsPlayer()) { // 0x4C0E21
        if (!(std::fabs(pedToCenterFwdDot) < 3.f)) {
            return;
        }
        if (!(std::fabs(pedToCenterFwdDot) > std::fabs(vehFwdSpeed * 50.f))) {
            return;
        }
        if (!(m_Ped->GetPlayerData()->m_fMoveBlendRatio < 1.f)) {
            return;
        }
        if (g_ikChainMan.IsLooking(m_Ped)) { // Como el original: si ya mira algo no hace nada
            return;
        }
        m_Ped->AsPlayer()->AnnoyPlayerPed(false);
        g_ikChainMan.LookAt("CompVhclPotCollResp", m_Ped, veh, 1500, BONE_UNKNOWN, nullptr, true, 0.25f, 500, 3, false);
        switch (m_Ped->GetActiveWeapon().GetType()) { // 0x4C0EC6
        case WEAPON_UNARMED:
        case WEAPON_BASEBALLBAT:
        case WEAPON_PISTOL:
        case WEAPON_PISTOL_SILENCED:
        case WEAPON_MICRO_UZI:
            m_PartialAnimTask = new CTaskSimpleShakeFist; // 0x4C0EF5
        }
        return;
    }

    // 0x4C0F2B
    if (std::fabs(pedToCenterFwdDot) < std::fabs((2.f * vehFwdSpeed) * 50.f)) {
        const float rnd    = CGeneral::GetRandomNumberInRange(0.f, 1.f); // 0x4C0F59
        const float absSpd = std::fabs(vehFwdSpeed);
        float chanceEvadeBack, chanceHorn, chanceFront, minPedDistX;
        if (absSpd > 0.3f) {
            chanceEvadeBack = 0.05f;
            chanceHorn      = 0.1f;
            chanceFront     = 0.2f;
            minPedDistX     = 0.175f;
        } else {
            if (!(absSpd > 0.05f)) { // Demasiado lento
                return;
            }
            chanceEvadeBack = 0.02f;
            chanceHorn      = 0.05f;
            chanceFront     = 0.1f;
            minPedDistX     = 0.7f;
        }

        // Como el original: angulo hacia el vehiculo con GetRadianAngleBetweenPoints + LimitRadianAngle
        const auto ComputeHeadingToVeh = [&] {
            return CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(vehPos.x, vehPos.y, pedPos.x, pedPos.y));
        };

        if (bVehInFront) { // 0x4C0FB7
            if (vehBBMaxX - minPedDistX < std::fabs(pedToCenterRightDot)) {
                if (rnd < chanceFront * 0.5f) { // 0x4C0FEB
                    m_Ped->m_fAimingRotation = ComputeHeadingToVeh();
                    m_EventResponseTask = new CTaskSimpleShakeFist; // 0x4C1063
                } else { // 0x4C10B6
                    m_EventResponseTask = new CTaskComplexEvasiveStep{ veh, dirToAvoidVehicle };
                }
            } else if (rnd < chanceFront) { // 0x4C10E3
                const auto heading = ComputeHeadingToVeh();
                m_Ped->m_fAimingRotation  = heading;
                m_Ped->m_fCurrentRotation = heading;
                m_Ped->SetHeading(heading);
                m_EventResponseTask = new CTaskSimpleHandsUp{ 3000 }; // 0x4C117B
            } else { // 0x4C118D
                m_EventResponseTask = new CTaskComplexEvasiveDiveAndGetUp{ veh, 0, dirToAvoidVehicle, true };
            }
        } else { // 0x4C11C7
            if (rnd > chanceHorn && veh->m_HornCounter == 0) {
                m_EventResponseTask = nullptr;
                return;
            }
            if (rnd > chanceEvadeBack) { // 0x4C121E
                m_EventResponseTask = new CTaskComplexEvasiveStep{ veh, dirToAvoidVehicle };
            } else { // 0x4C122B
                m_EventResponseTask = new CTaskComplexEvasiveDiveAndGetUp{ veh, 0, dirToAvoidVehicle, true };
            }
        }
        return;
    }

    // 0x4C1265
    if (veh->GetStatus() != STATUS_PLAYER || !m_Ped->IsCreatedBy(PED_GAME)) {
        return;
    }
    const auto currPedEvent = m_Ped->GetIntelligence()->GetEventHandler().GetHistory().GetCurrentEvent();
    if (veh->m_HornCounter == 0) {
        return;
    }
    if (currPedEvent && currPedEvent->GetEventType() == EVENT_POTENTIAL_GET_RUN_OVER) {
        return;
    }

    const auto rndNum = CGeneral::GetRandomIntInRange(0, 1000); // 0x4C12B3
    if (rndNum < 25 && m_Ped->m_nPedType != PED_TYPE_COP) { // 0x4C12C0
        const auto fleeTime = CGeneral::GetRandomIntInRange(0, 500) + 750;
        m_EventResponseTask = new CTaskComplexFleeEntity{ veh, false, fSafeDistance, fleeTime };
    } else if (rndNum < 100) { // 0x4C132A
        const auto seq = new CTaskComplexSequence;
        m_EventResponseTask = seq;
        seq->AddTask(new CTaskSimpleAchieveHeading{ CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(vehPos.x, vehPos.y, pedPos.x, pedPos.y)) });
        seq->AddTask(new CTaskSimpleShakeFist);
    } else if (rndNum < 200) { // 0x4C145C
        m_EventResponseTask = new CTaskSimpleAchieveHeading{ CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(vehPos.x, vehPos.y, pedPos.x, pedPos.y)) };
        g_ikChainMan.LookAt("CompVehiclePotCollResp", m_Ped, veh, 1300, BONE_UNKNOWN, nullptr, true, 0.25f, 500, 3, false);
    } else if (rndNum < 400) { // 0x4C1534
        g_ikChainMan.LookAt("CompVehiclePotCollResp", m_Ped, veh, 1300, BONE_UNKNOWN, nullptr, true, 0.25f, 500, 3, false);
    }
}

// 0x4B96D0
void CEventHandler::ComputeVehiclePotentialPassiveCollisionResponse(CEventPotentialWalkIntoVehicle* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: cualquier ruta de salida NO escribe m_EventResponseTask; el estado de movimiento que se compara es el del EVENTO
    if (!e->m_vehicle || m_Ped->bInVehicle) {
        return;
    }
    if (!tsimplest || !CTask::IsGoToTask(tsimplest)) {
        return;
    }
    if (e->m_moveState == PEDMOVE_STILL) {
        return;
    }
    const auto tGoTo = static_cast<CTaskSimpleGoTo*>(tsimplest);
    const auto moveState = m_Ped->GetGroup()
        ? PEDMOVE_RUN
        : (eMoveState)e->m_moveState;
    const auto isGoingForDoor = m_Ped->GetIntelligence()->IsPedGoingForCarDoor(); // 0x4B974A - antes de reservar la tarea
    m_EventResponseTask = new CTaskComplexWalkRoundCar{ moveState, tGoTo->m_vecTargetPoint, e->m_vehicle, isGoingForDoor };
}

// 0x4B9F80
void CEventHandler::ComputeVehicleToStealResponse(CEventVehicleToSteal* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: si el ped YA va en un vehículo (o no hay vehículo) no se escribe nada
    if (m_Ped->bInVehicle || !e->m_vehicle) {
        return;
    }
    m_EventResponseTask = new CTaskComplexStealCar{ e->m_vehicle };
}

// 0x4BAE30
void CEventHandler::ComputeWaterCannonResponse(CEventHitByWaterCannon* e, CTask* tactive, CTask* tsimplest) {
    // Como el original: Say -> lado del golpe -> tarea (se guarda ya) -> fuerza y velocidad
    m_Ped->Say(CTX_GLOBAL_PAIN_HIGH);
    const auto hitSide = CPedGeometryAnalyser::ComputePedHitSide(*m_Ped, e->m_moveSpeed);
    m_EventResponseTask = new CTaskComplexFallAndGetUp{ hitSide, 0 };
    m_Ped->ApplyMoveForce({ 0.f, 0.f, CTimer::GetTimeStep() * 2.f });
    m_Ped->SetMoveSpeedXY((CVector2D{e->m_moveSpeed} * 0.6f + CVector2D{m_Ped->GetMoveSpeed()}) / 2.f);
    // Como el original: 0x4BAF2C compara con "> 0.2" (fcom + test ah,41h)
    if (const auto speed = m_Ped->GetMoveSpeed().Magnitude2D(); speed > 0.2f) {
        m_Ped->SetMoveSpeedXY(0.2f / speed * CVector2D{m_Ped->GetMoveSpeed()});
    }
}

// 0x4C3870
void CEventHandler::ComputeEventResponseTask(CEvent* e, CTask* pAbortedTaskEventResponse) {
    m_PhysicalResponseTask = nullptr;
    m_EventResponseTask    = nullptr;
    m_AttackTask           = nullptr;
    m_SayTask              = nullptr;
    m_PartialAnimTask      = nullptr;

    const auto tactive = m_Ped->GetTaskManager().GetActiveTask();
    const auto tsimplest = tactive
        ? m_Ped->GetTaskManager().GetSimplestActiveTask()
        : nullptr;
    
    //NOTSA_LOG_DEBUG("event: {} tactive: {} tsimplest: {}", (int32)event->GetEventType(), (int32)tactive->GetTaskType(), (int32)tsimplest->GetTaskType()); // NOTSA

    switch (e->GetEventType()) {
    case EVENT_VEHICLE_COLLISION:
        ComputeVehicleCollisionResponse(static_cast<CEventVehicleCollision*>(e), tactive, tsimplest);
        break;
    case EVENT_PED_COLLISION_WITH_PED:
        ComputePedCollisionWithPedResponse(e, tactive, tsimplest);
        break;
    case EVENT_PED_COLLISION_WITH_PLAYER:
        ComputePedCollisionWithPlayerResponse(e, tactive, tsimplest);
        break;
    case EVENT_PLAYER_COLLISION_WITH_PED:
        ComputePlayerCollisionWithPedResponse(static_cast<CEventPlayerCollisionWithPed*>(e), tactive, tsimplest);
        break;
    case EVENT_OBJECT_COLLISION:
        ComputeObjectCollisionResponse(static_cast<CEventObjectCollision*>(e), tactive, tsimplest);
        break;
    case EVENT_BUILDING_COLLISION:
        ComputeBuildingCollisionResponse(static_cast<CEventBuildingCollision*>(e), tactive, tsimplest);
        break;
    case EVENT_DRAGGED_OUT_CAR:
        ComputeDraggedOutCarResponse(static_cast<CEventDraggedOutCar*>(e), tactive, tsimplest);
        break;
    case EVENT_KNOCK_OFF_BIKE:
        ComputeKnockOffBikeResponse(e, tactive, tsimplest);
        break;
    case EVENT_DAMAGE:
        ComputeDamageResponse(static_cast<CEventDamage*>(e), tactive, tsimplest, pAbortedTaskEventResponse);
        break;
    case EVENT_DEATH:
        ComputeDeathResponse(static_cast<CEventDeath*>(e), tactive, tsimplest);
        break;
    case EVENT_DEAD_PED:
        ComputeDeadPedResponse(static_cast<CEventDeadPed*>(e), tactive, tsimplest);
        break;
    case EVENT_POTENTIAL_GET_RUN_OVER:
        ComputeVehiclePotentialCollisionResponse(static_cast<CEventPotentialGetRunOver*>(e), tactive, tsimplest);
        break;
    case EVENT_POTENTIAL_WALK_INTO_PED:
        ComputePotentialPedCollideResponse(static_cast<CEventPotentialWalkIntoPed*>(e), tactive, tsimplest);
        break;
    case EVENT_SHOT_FIRED:
        ComputeShotFiredResponse(static_cast<CEventGunShot*>(e), tactive, tsimplest);
        break;
    case EVENT_COP_CAR_BEING_STOLEN:
        ComputeCopCarBeingStolenResponse(static_cast<CEventCopCarBeingStolen*>(e), tactive, tsimplest);
        break;
    case EVENT_PED_ENTERED_MY_VEHICLE:
        ComputePedEnteredVehicleResponse(static_cast<CEventPedEnteredMyVehicle*>(e), tactive, tsimplest);
        break;
    case EVENT_REVIVE:
        ComputeReviveResponse(static_cast<CEventRevived*>(e), tactive, tsimplest);
        break;
    case EVENT_CHAT_PARTNER:
        ComputeChatPartnerResponse(static_cast<CEventChatPartner*>(e), tactive, tsimplest);
        break;
    case EVENT_SEXY_PED:
        ComputeSexyPedResponse(static_cast<CEventSexyPed*>(e), tactive, tsimplest);
        break;
    case EVENT_SEXY_VEHICLE:
        ComputeSexyVehicleResponse(static_cast<CEventSexyVehicle*>(e), tactive, tsimplest);
        break;
    case EVENT_PED_TO_CHASE:
        ComputePedToChaseResponse(static_cast<CEventPedToChase*>(e), tactive, tsimplest);
        break;
    case EVENT_PED_TO_FLEE:
        ComputePedToFleeResponse(static_cast<CEventPedToFlee*>(e), tactive, tsimplest);
        break;
    case EVENT_ATTRACTOR:
    case EVENT_SCRIPTED_ATTRACTOR:
        ComputeAttractorResponse(static_cast<CEventAttractor*>(e), tactive, tsimplest);
        break;
    case EVENT_VEHICLE_TO_STEAL:
        ComputeVehicleToStealResponse(static_cast<CEventVehicleToSteal*>(e), tactive, tsimplest);
        break;
    case EVENT_GUN_AIMED_AT:
        ComputeGunAimedAtResponse(static_cast<CEventGunAimedAt*>(e), tactive, tsimplest);
        break;
    case EVENT_SCRIPT_COMMAND:
        ComputeScriptCommandResponse(static_cast<CEventScriptCommand*>(e), tactive, tsimplest);
        break;
    case EVENT_IN_AIR:
        if (!m_Ped->bIsStanding) {
            m_EventResponseTask = new CTaskComplexInAirAndLand(false, false);
        }
        break;
    case EVENT_VEHICLE_DIED:
        ComputeVehicleDiedResponse(static_cast<CEventVehicleDied*>(e), tactive, tsimplest);
        break;
    case EVENT_ACQUAINTANCE_PED_HATE:
    case EVENT_ACQUAINTANCE_PED_DISLIKE:
        ComputePedThreatResponse(static_cast<CEventAcquaintancePedHate*>(e), tactive, tsimplest);
        break;
    case EVENT_ACQUAINTANCE_PED_LIKE:
    case EVENT_ACQUAINTANCE_PED_RESPECT:
        ComputePedFriendResponse(static_cast<CEventAcquaintancePed*>(e), tactive, tsimplest);
        break;
    case EVENT_VEHICLE_DAMAGE_WEAPON:
    case EVENT_VEHICLE_DAMAGE_COLLISION:
        ComputeVehicleDamageResponse(static_cast<CEventVehicleDamage*>(e), tactive, tsimplest);
        break;
    case EVENT_SPECIAL:
        ComputeSpecialResponse(static_cast<CEventSpecial*>(e), tactive, tsimplest);
        break;
    case EVENT_GOT_KNOCKED_OVER_BY_CAR:
        ComputeGotKnockedOverByCarResponse(static_cast<CEventGotKnockedOverByCar*>(e), tactive, tsimplest);
        break;
    case EVENT_POTENTIAL_WALK_INTO_OBJECT:
        ComputeObjectCollisionPassiveResponse(static_cast<CEventPotentialWalkIntoObject*>(e), tactive, tsimplest);
        break;
    case EVENT_CAR_UPSIDE_DOWN:
        ComputeCarUpsideDownResponse(static_cast<CEventCarUpsideDown*>(e), tactive, tsimplest);
        break;
    case EVENT_POTENTIAL_WALK_INTO_FIRE:
        ComputePotentialWalkIntoFireResponse(static_cast<CEventPotentialWalkIntoFire*>(e), tactive, tsimplest);
        break;
    case EVENT_SHOT_FIRED_WHIZZED_BY:
        ComputeShotFiredWhizzedByResponse(static_cast<CEventGunShotWhizzedBy*>(e), tactive, tsimplest);
        break;
    case EVENT_LOW_ANGER_AT_PLAYER:
        ComputeLowAngerAtPlayerResponse(static_cast<CEventLowAngerAtPlayer*>(e), tactive, tsimplest);
        break;
    case EVENT_HIGH_ANGER_AT_PLAYER:
        ComputeHighAngerAtPlayerResponse(static_cast<CEventHighAngerAtPlayer*>(e), tactive, tsimplest);
        break;
    case EVENT_HEALTH_REALLY_LOW:
        ComputeReallyLowHealthResponse(static_cast<CEventHealthReallyLow*>(e), tactive, tsimplest);
        break;
    case EVENT_HEALTH_LOW:
        ComputeLowHealthResponse(static_cast<CEventHealthLow*>(e), tactive, tsimplest);
        break;
    case EVENT_POTENTIAL_WALK_INTO_VEHICLE:
        ComputeVehiclePotentialPassiveCollisionResponse(static_cast<CEventPotentialWalkIntoVehicle*>(e), tactive, tsimplest);
        break;
    case EVENT_ON_FIRE:
        ComputeOnFireResponse(static_cast<CEventOnFire*>(e), tactive, tsimplest);
        break;
    case EVENT_FIRE_NEARBY:
        ComputeFireNearbyResponse(static_cast<CEventFireNearby*>(e), tactive, tsimplest);
        break;
    case EVENT_SOUND_QUIET:
        ComputePedSoundQuietResponse(static_cast<CEventSoundQuiet*>(e), tactive, tsimplest);
        break;
    case EVENT_ACQUAINTANCE_PED_HATE_BADLY_LIT:
        ComputePedThreatBadlyLitResponse(static_cast<CEventAcquaintancePedHateBadlyLit*>(e), tactive, tsimplest);
        break;
    case EVENT_WATER_CANNON:
        ComputeWaterCannonResponse(static_cast<CEventHitByWaterCannon*>(e), tactive, tsimplest);
        break;
    case EVENT_SEEN_PANICKED_PED:
        ComputeSeenPanickedPedResponse(static_cast<CEventSeenPanickedPed*>(e), tactive, tsimplest);
        break;
    case EVENT_IN_WATER:
        ComputeInWaterResponse(static_cast<CEventInWater*>(e), tactive, tsimplest);
        break;
    case EVENT_AREA_CODES:
        ComputeAreaCodesResponse(static_cast<CEventAreaCodes*>(e), tactive, tsimplest);
        break;
    case EVENT_PLAYER_WANTED_LEVEL:
        ComputePlayerWantedLevelResponse(e, tactive, tsimplest);
        break;
    case EVENT_CREATE_PARTNER_TASK:
        ComputeCreatePartnerTaskResponse(static_cast<CEventCreatePartnerTask*>(e), tactive, tsimplest);
        break;
    case EVENT_SEEN_COP:
        ComputeSeenCopResponse(static_cast<CEventSeenCop*>(e), tactive, tsimplest);
        break;
    case EVENT_ON_ESCALATOR:
        ComputeOnEscalatorResponse(e, tactive, tsimplest);
        break;
    case EVENT_DANGER:
        ComputeDangerResponse(static_cast<CEventDanger*>(e), tactive, tsimplest);
        break;
    case EVENT_VEHICLE_ON_FIRE:
        ComputeVehicleOnFireResponse(static_cast<CEventVehicleOnFire*>(e), tactive, tsimplest);
        break;
    case EVENT_INTERIOR_USE_INFO:
        ComputeInteriorUseInfoResponse(static_cast<CEventInteriorUseInfo*>(e), static_cast<CTaskInteriorUseInfo*>(tactive), tsimplest); // Como el original: sin comprobar el tipo de la tarea activa
        break;
    case EVENT_SIGNAL_AT_PED:
        ComputeSignalAtPedResponse(static_cast<CEventSignalAtPed*>(e), tactive, tsimplest);
        break;
    case EVENT_PASS_OBJECT:
        ComputePassObjectResponse(static_cast<CEventPassObject*>(e), tactive, tsimplest);
        break;
    case EVENT_STUCK_IN_AIR:
        m_EventResponseTask = new CTaskComplexStuckInAir();
        break;
    case EVENT_DONT_JOIN_GROUP:
        ComputeDontJoinGroupResponse(static_cast<CEventDontJoinPlayerGroup*>(e), tactive, tsimplest);
        break;
    default:
        return;
    }
}

// 0x4C4220
CTask* CEventHandler::ComputeEventResponseTask(const CPed& ped, const CEvent& e) {
    CTask* task = nullptr;

    CEventHandler eh{const_cast<CPed*>(&ped)};
    eh.ComputeEventResponseTask(const_cast<CEvent*>(&e), nullptr);
    if (eh.m_EventResponseTask) {
        task = eh.m_EventResponseTask;
        eh.m_EventResponseTask = nullptr;
    }
    eh.Flush();
    return task;
}
