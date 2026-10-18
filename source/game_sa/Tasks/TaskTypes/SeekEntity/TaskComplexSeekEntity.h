/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#pragma once

#include <concepts>

#include "TaskComplex.h"
#include "World.h"
#include "Ped.h"
#include "General.h"
#include "TaskTimer.h"
#include "eMoveState.h"
#include "extensions/utility.hpp"
#include "PedPlacement.h"
#include "TaskSimpleCarDriveTimed.h"
#include "TaskSimpleTired.h"
#include "TaskComplexFollowNodeRoute.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexSequence.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleTired.h"
#include "PosCalculators/EntitySeekPosCalculator.h"
#include "PosCalculators/EntitySeekPosCalculatorStandard.h"
#include "PosCalculators/EntitySeekPosCalculatorXYOffset.h"

// 0x4BC470 con los registros del EXE (TaskComplexSeekEntityXYOffset.cpp); el argumento llega como los bits del float.
uint32 __fastcall SetMaxEntityDist2DRegs(void* self, uint32 entryEdx, uint32 distanceBits);

template <std::derived_from<CEntitySeekPosCalculator> T_PosCalc = CEntitySeekPosCalculatorStandard>
class NOTSA_EXPORT_VTABLE CTaskComplexSeekEntity : public CTaskComplex {
    CEntity* m_entityToSeek{};
    int32 m_seekInterval{};
    int32 m_scanInterval{};
    float m_maxEntityDist2D{};
    float m_moveStateRadius{};
    float m_minEntityDist2D{};
    float m_followNodeThresholdHeightChange{};
    CTaskTimer m_scanTimer; // Como el original: sin {} para no poner a cero el relleno de los temporizadores
    CTaskTimer m_seekTimer;
    T_PosCalc m_entitySeekPosCalculator{};

    // Everything here mustn't be accesed without the proper `T_PosCalc` type!
    eMoveState m_moveState{ PEDMOVE_RUN };
    bool m_bPlayTiredAnim : 1{};
    bool m_bFaceEntityWhenDone : 1{};
    bool m_bAchievedSeekEntity : 1{}; /// True when the distance between the `GetSeekPos()` and the `ped` is less than `m_maxEntityDist2D`
    bool m_bTrackingEntity : 1{};
public:
    /*!
    * NOTE: Since this task is templated but uses the same task type for all templated tasks
    *       our template magic stuff in TaskManager might not work properly with it,
    *       because at runtime all templated tasks will have the same type.
    *       Accessing anything before `m_entitySeekPosCalculator` is fine either way
    *       but anything after it depends on the actual `T_PosCalc` used [which can't [easily] be figured out at runtime]
    */
    static constexpr auto Type = eTaskType::TASK_COMPLEX_SEEK_ENTITY;

    static void InjectHooks() {
        RH_ScopedCategory("Tasks/TaskTypes/SeekEntity");
        if constexpr (std::is_same_v<T_PosCalc, CEntitySeekPosCalculatorXYOffset>) {
            RH_ScopedVirtualClass(CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>, 0x86FBB4, 11);

            RH_ScopedNamedGlobalInstall(SetMaxEntityDist2DRegs, "SetMaxEntityDist2D", 0x4BC470);
            RH_ScopedInstall(Constructor, 0x661DC0);
            RH_ScopedInstall(Destructor, 0x661F30);

            RH_ScopedInstall(CreateSubTask, 0x665270); // Como el original: 0x496DC0 es el GetSeekPos de RadiusAngleOffset (lo llama 0x496B9A)

            RH_ScopedVMTInstall(Clone, 0x664AF0);
            RH_ScopedVMTInstall(GetTaskType, 0x661EA0);
            RH_ScopedVMTInstall(MakeAbortable, 0x661EB0);
            RH_ScopedVMTInstall(CreateNextSubTask, 0x665080);
            RH_ScopedVMTInstall(CreateFirstSubTask, 0x665600);
            RH_ScopedVMTInstall(ControlSubTask, 0x665760);
        }
        // TODO: Add other specializations
    }

    CTaskComplexSeekEntity(
        CEntity*  entity,
        int32     seekInterval,
        int32     scanInterval,
        float     maxEntityDist2D,
        float     moveStateRadius,
        float     followNodeThresholdHeightChange,
        bool      bPlayTiredAnim,
        bool      bFaceEntityWhenDone,
        T_PosCalc seekPosCalculator = {} // NOTSA
    ) :
        m_entityToSeek{ entity },
        m_seekInterval{ seekInterval },
        m_scanInterval{ scanInterval },
        m_maxEntityDist2D{ maxEntityDist2D },
        m_moveStateRadius{ moveStateRadius },
        m_followNodeThresholdHeightChange{ followNodeThresholdHeightChange },
        m_bPlayTiredAnim{ bPlayTiredAnim },
        m_bFaceEntityWhenDone{ bFaceEntityWhenDone },
        m_entitySeekPosCalculator{ seekPosCalculator }
    {
        CEntity::SafeRegisterRef(m_entityToSeek);
    }

    CTaskComplexSeekEntity(const CTaskComplexSeekEntity& o) :
        m_entityToSeek{ o.m_entityToSeek },
        m_seekInterval{ o.m_seekInterval },
        m_scanInterval{ o.m_scanInterval },
        m_maxEntityDist2D{ o.m_maxEntityDist2D },
        m_moveStateRadius{ o.m_moveStateRadius },
        m_followNodeThresholdHeightChange{ o.m_followNodeThresholdHeightChange },
        m_bPlayTiredAnim{ o.m_bPlayTiredAnim },
        m_bFaceEntityWhenDone{ true },

        m_entitySeekPosCalculator{ o.m_entitySeekPosCalculator },
        m_moveState{ o.m_moveState },
        m_bTrackingEntity{ o.m_bTrackingEntity },
        m_minEntityDist2D{ o.m_minEntityDist2D }
    {
        CEntity::SafeRegisterRef(m_entityToSeek);
    }

    ~CTaskComplexSeekEntity() override {
        CEntity::SafeCleanUpRef(m_entityToSeek);
    }

    CTask* CreateSubTask(eTaskType type, CPed* ped) {
        switch (type) {
        case TASK_FINISHED:
            return nullptr;
        case TASK_COMPLEX_TURN_TO_FACE_ENTITY:
            return new CTaskComplexTurnToFaceEntityOrCoord{ m_entityToSeek };
        case TASK_COMPLEX_FOLLOW_NODE_ROUTE:
            return new CTaskComplexFollowNodeRoute{
                m_moveState,
                GetSeekPos(ped),
                m_maxEntityDist2D,
                m_moveStateRadius,
                m_followNodeThresholdHeightChange,
                false,
                -1,
                true
            };
        case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
            return new CTaskComplexGoToPointAndStandStill{
                m_moveState,
                GetSeekPos(ped),
                m_maxEntityDist2D,
                m_moveStateRadius,
                false,
                false,
                m_bTrackingEntity
            };
        case TASK_SIMPLE_CAR_DRIVE_TIMED:
            return new CTaskSimpleCarDriveTimed{ ped->m_pVehicle, 2000 };
        case TASK_COMPLEX_LEAVE_CAR:
            return new CTaskComplexLeaveCar{ ped->m_pVehicle, TARGET_DOOR_FRONT_LEFT, 0, true, false };
        case TASK_COMPLEX_SEQUENCE: {
            const auto seq = new CTaskComplexSequence; // Como el original (0x632BD0): sin value-init, el relleno tras m_bFlushTasks (+0x39..+0x3B) no se escribe
            if (m_bFaceEntityWhenDone) {
                seq->AddTask(new CTaskComplexTurnToFaceEntityOrCoord{ m_entityToSeek });
            }
            seq->AddTask(new CTaskSimpleStandStill{ 100 });
            return seq;
        }
        case TASK_SIMPLE_STAND_STILL:
            return new CTaskSimpleStandStill{ 2000 };
        case TASK_SIMPLE_TIRED:
            return new CTaskSimpleTired{ 1000u };
        default:
            return nullptr; // Como el original: un tipo desconocido devuelve nulo
        }
    }

    CTask* Clone() const override {
        return new CTaskComplexSeekEntity{ *this };
    }

    eTaskType GetTaskType() const override {
        return eTaskType::TASK_COMPLEX_SEEK_ENTITY;
    }

    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override {
        // Como el original (0x46ACF0): con prioridad de ocio se anula el intervalo de búsqueda (+0x10) y vence su temporizador
        if (priority == ABORT_PRIORITY_LEISURE) {
            m_seekInterval = -1;
            m_seekTimer.SetAsOutOfTime();
        }

        // Como el original: el temporizador de escaneo se arranca con intervalo 0 (no -1)
        if (m_pSubTask->MakeAbortable(ped, priority, event)) {
            m_scanTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_scanTimer.m_nInterval  = 0;
            m_scanTimer.m_bStarted   = true;
            return true;
        }

        return false;
    }

    CTask* CreateNextSubTask(CPed* ped) override {
        // Como el original (0x46EFF0): sin entidad termina sin consultar la subtarea
        if (!m_entityToSeek) {
            return CreateSubTask(TASK_FINISHED, ped);
        }
        switch (m_pSubTask->GetTaskType()) {
        case TASK_COMPLEX_SEQUENCE:
        case TASK_COMPLEX_TURN_TO_FACE_ENTITY:
            return CreateSubTask(TASK_FINISHED, ped);
        case TASK_COMPLEX_FOLLOW_NODE_ROUTE:
        case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
            if (const auto dist2DSq = (ped->GetPosition() - GetSeekPos(ped)).SquaredMagnitude2D(); dist2DSq > sq(m_maxEntityDist2D)) {
                return CreateSubTaskWhenPedIsTooFarFromEntity(ped, dist2DSq);
            }
            m_bAchievedSeekEntity = true;
            return CreateSubTask(m_bFaceEntityWhenDone ? TASK_COMPLEX_TURN_TO_FACE_ENTITY : TASK_FINISHED, ped); // Ternary inverted
        }
        case TASK_SIMPLE_CAR_DRIVE_TIMED:
            return CreateFirstSubTask(ped);
        case TASK_COMPLEX_LEAVE_CAR:
            return CreateSubTask(ped->bInVehicle ? TASK_FINISHED : TASK_SIMPLE_CAR_DRIVE_TIMED, ped);
        case TASK_SIMPLE_STAND_STILL: // Como el original: solo se sigue quieto mientras el peatón esté enganchado (+0xFC)
            return ped->m_pAttachedTo ? CreateSubTask(TASK_SIMPLE_STAND_STILL, ped) : CreateFirstSubTask(ped);
        case TASK_SIMPLE_TIRED: {
            CTaskSimpleStandStill task{ -1 };
            task.ProcessPed(ped);
            return CreateSubTask(TASK_FINISHED, ped);
        }
        default:
            return nullptr; // Como el original: un tipo desconocido devuelve nulo
        }
    }

    CTask* CreateFirstSubTask(CPed* ped) override {
        // Como el original (0x46F6A0): el escaneo arranca siempre y la búsqueda salvo con intervalo -1 exacto
        m_scanTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_scanTimer.m_nInterval  = m_scanInterval;
        m_scanTimer.m_bStarted   = true;
        if (m_seekInterval != -1) {
            m_seekTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_seekTimer.m_nInterval  = m_seekInterval;
            m_seekTimer.m_bStarted   = true;
        }

        if (!m_entityToSeek) {
            return CreateSubTask(TASK_SIMPLE_TIRED, ped);
        }

        if (ped->IsInVehicle()) {
            return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
        }

        // Como el original: solo se considera alcanzada con distancia estrictamente menor (igual o NaN sigue lejos, 0x46F767)
        if (const auto dist2DSq = (ped->GetPosition2D() - GetSeekPos2D(ped)).SquaredMagnitude(); !(dist2DSq < sq(m_maxEntityDist2D))) {
            return CreateSubTaskWhenPedIsTooFarFromEntity(ped, dist2DSq);
        }

        m_bAchievedSeekEntity = true;
        ped->SetMoveState(PEDMOVE_STILL);
        return CreateSubTask(TASK_COMPLEX_SEQUENCE, ped);
    }

    CTask* ControlSubTask(CPed* ped) override {
        // Como el original (0x46F800): el barco común solo se busca si la entidad es un peatón del mismo grupo
        // (AreInSameGroup(ped, entidad)); sin eso no se toca el enganche del peatón
        if (m_entityToSeek && m_entityToSeek->GetIsTypePed() && CPedGroups::AreInSameGroup(ped, m_entityToSeek->AsPed())) {
            const auto toSeekPed  = m_entityToSeek->AsPed();
            CVehicle*  commonBoat = nullptr;
            if (!toSeekPed->physicalFlags.bSubmergedInWater) {
                const auto IsCommonVehicle = [ped](CEntity* entity) {
                    return entity && entity->GetIsTypeVehicle() && (ped->m_standingOnEntity == entity || ped->m_pContactEntity == entity);
                };
                // Como el original: si la primera entidad es un vehículo común que no es barco, no se mira la segunda
                if (IsCommonVehicle(toSeekPed->m_standingOnEntity)) {
                    if (toSeekPed->m_standingOnEntity->AsVehicle()->IsBoat()) {
                        commonBoat = toSeekPed->m_standingOnEntity->AsVehicle();
                    }
                } else if (IsCommonVehicle(toSeekPed->m_pContactEntity) && toSeekPed->m_pContactEntity->AsVehicle()->IsBoat()) {
                    commonBoat = toSeekPed->m_pContactEntity->AsVehicle();
                }
            }
            if (ped->m_pAttachedTo) {
                if (!commonBoat) {
                    ped->DettachPedFromEntity();
                }
            } else if (commonBoat && m_pSubTask->GetTaskType() != TASK_SIMPLE_STAND_STILL && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
                return AttachPedToCommonBoat(ped, commonBoat);
            }
        }

        // Como el original: la frase de seguimiento solo se comprueba en los retornos que no crean una subtarea nueva
        // por cambio de ruta (0x46FDB2); esos saltan directamente al final
        bool checkSay = true;
        CTask* const result = [&]() -> CTask* {
            if (m_pSubTask->GetTaskType() != TASK_COMPLEX_FOLLOW_NODE_ROUTE && m_pSubTask->GetTaskType() != TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) {
                return m_pSubTask; // Como el original: otras subtareas siguen (no se devuelve nulo)
            }

            if (!m_entityToSeek) {
                return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr) ? nullptr : m_pSubTask;
            }

            if (m_seekTimer.IsOutOfTime()) {
                if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
                    return m_pSubTask;
                }
                return m_bPlayTiredAnim ? new CTaskSimpleTired{ 1000u } : nullptr;
            }

            if (!m_scanTimer.IsOutOfTime()) {
                return m_pSubTask;
            }

            // Como el original: el escaneo se rearranca sin comprobar el signo del intervalo
            m_scanTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_scanTimer.m_nInterval  = m_scanInterval;
            m_scanTimer.m_bStarted   = true;

            const auto seekPos = GetSeekPos(ped);
            CVector    pedToSeekPos;
            pedToSeekPos.Difference(ped->GetPosition(), seekPos);
            const float pedToSeekPosDist2dSq = pedToSeekPos.x * pedToSeekPos.x + pedToSeekPos.y * pedToSeekPos.y;

            if (m_pSubTask->GetTaskType() == TASK_COMPLEX_FOLLOW_NODE_ROUTE) {
                if (m_minEntityDist2D != 0.f) {
                    const float minDist = m_minEntityDist2D - 1.f;
                    if (pedToSeekPosDist2dSq < minDist * minDist) { // Como el original: NaN actualiza el destino (0x46FCB4)
                        if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
                            return m_pSubTask;
                        }
                        checkSay = false;
                        return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL, ped);
                    }
                }
                static_cast<CTaskComplexFollowNodeRoute*>(m_pSubTask)->SetTarget(ped, seekPos, m_maxEntityDist2D, m_moveStateRadius, m_followNodeThresholdHeightChange, false);
                return m_pSubTask;
            }

            const auto subTaskStandStill = static_cast<CTaskComplexGoToPointAndStandStill*>(m_pSubTask);
            if (subTaskStandStill->m_bWasSuccessful) {
                return m_pSubTask;
            }
            if (m_minEntityDist2D != 0.f) {
                const float maxDist = m_minEntityDist2D + 1.f;
                if (pedToSeekPosDist2dSq > maxDist * maxDist) { // Como el original: NaN sigue con el mismo punto (0x46FD33)
                    if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
                        return m_pSubTask;
                    }
                    checkSay = false;
                    return CreateSubTask(TASK_COMPLEX_FOLLOW_NODE_ROUTE, ped);
                }
            }
            subTaskStandStill->GoToPoint(seekPos, m_maxEntityDist2D, m_moveStateRadius, false);
            return m_pSubTask;
        }();

        if (checkSay && m_entityToSeek && m_entityToSeek->GetIsTypePed()) {
            if (m_entityToSeek->AsPed()->IsPlayer()) { // Entity to seek is a player
                if (CPedGroups::ms_groups.data()[FindPlayerPed()->GetPlayerData()->m_nPlayerGroup].GetMembership().IsFollower(ped)) { // Como el original (0x46FDDA): ms_groups sin comprobar si el grupo está activo
                    if (ped->IsJoggingOrFaster()) {
                        if (((uint16)CTimer::GetFrameCounter() + ped->m_nRandomSeed) % 16384 == 0) {
                            ped->Say(CTX_GLOBAL_FOLLOW_CONSTANT);
                        }
                    }
                }
            }
        }
        return result;
    }

    // NOTSA Helpers
    CVector GetSeekPos(CPed* ped) {
        CVector seekPos{};
        m_entitySeekPosCalculator.ComputeEntitySeekPos(*ped, *m_entityToSeek, seekPos);
        return seekPos;
    }

    CVector2D GetSeekPos2D(CPed* ped) {
        return CVector2D{GetSeekPos(ped)};
    }

    void SetMoveState(eMoveState ms) {
        m_moveState = ms;
    }

    void SetIsTrackingEntity(bool v) {
        m_bTrackingEntity = v;
    }

    auto& GetSeekPosCalculator() {
        return m_entitySeekPosCalculator;
    }

    void SetMaxEntityDist2D(float); // Common prefix; native distance + scan timer reset.

    void SetEntityMinDist2D(float v) {
        m_minEntityDist2D = v;
    }

    auto GetEntityToSeek() const {
        return m_entityToSeek;
    }

    auto GetMoveStateRadius() const {
        return m_moveStateRadius;
    }

private:
    // Como el original (0x46F94A): se engancha al barco por la dirección peatón->barco, desplazado 0.25 hacia él
    CTask* AttachPedToCommonBoat(CPed* ped, CVehicle* commonBoat) {
        const auto& boatPos = commonBoat->GetPosition();
        CVector     pedToBoat;
        pedToBoat.Difference(boatPos, ped->GetPosition());
        CVector pedToBoatDir = pedToBoat;
        pedToBoatDir.Normalise();

        CVector newPedPos = boatPos - (pedToBoat - pedToBoatDir * 0.25f);
        if (const auto [groundPos, found] = CPedPlacement::FindZCoorForPed(newPedPos); found) { // Como el original: solo se usa si encuentra suelo
            newPedPos = groundPos;
        }

        CMatrix invBoatMat;
        Invert(*commonBoat->m_matrix, invBoatMat); // Como el original: 0x59B920 con la matriz del barco
        CVector offset;
        offset.FromMultiply3x3(invBoatMat, newPedPos - boatPos);
        const float heading = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(pedToBoatDir.x, pedToBoatDir.y, 0.f, 0.f));
        ped->AttachPedToEntity(commonBoat, offset, (uint16)(int32)heading, 0.2f, ped->GetActiveWeapon().m_Type); // Como el original: _ftol2 del ángulo
        return new CTaskSimpleStandStill{ 2000 };
    }

    CTask* CreateSubTaskWhenPedIsTooFarFromEntity(CPed* ped, float pedToSeekPosDist2DSq) {
        return CreateSubTask(
            m_minEntityDist2D == 0.f || pedToSeekPosDist2DSq > sq(m_minEntityDist2D) ? TASK_COMPLEX_FOLLOW_NODE_ROUTE : TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,
            ped
        );
    }

protected: // Wrappers for hooks
    // 0x493730
    CTaskComplexSeekEntity* Constructor(
        CEntity* entity,
        int32    taskTimer2Interval,
        int32    taskTimer1Interval,
        float    maxEntityDist2D,
        float    unk1,
        float    unk2,
        bool     flag0,
        bool     flag1
    ) {
        // Como el original (0x661DC0): el 2.º argumento va a +0x10 (m_seekInterval) y el 3.º a +0x14; antes se intercambiaban
        this->CTaskComplexSeekEntity::CTaskComplexSeekEntity(entity, taskTimer2Interval, taskTimer1Interval, maxEntityDist2D, unk1, unk2, flag0, flag1);
        return this;
    }

    // 0x493890
    CTaskComplexSeekEntity* Destructor() {
        this->CTaskComplexSeekEntity::~CTaskComplexSeekEntity();
        return this;
    }
};

template<> void CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>::SetMaxEntityDist2D(float);
