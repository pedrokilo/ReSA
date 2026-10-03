/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleGetUp
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleGetUp.h"
#include "PedPlacement.h"

void CTaskSimpleGetUp::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleGetUp, 0x8703E0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x677F50);
    RH_ScopedInstall(StartAnim, 0x67C770);
    RH_ScopedInstall(FinishGetUpAnimCB, 0x678110);
    RH_ScopedVMTInstall(ProcessPed, 0x67FA80);
    RH_ScopedVMTInstall(MakeAbortable, 0x677FE0);
}

CTaskSimpleGetUp* CTaskSimpleGetUp::Constructor() {
    this->CTaskSimpleGetUp::CTaskSimpleGetUp();
    return this;
}

// 0x677F50
CTaskSimpleGetUp::CTaskSimpleGetUp() {
    m_bHasPedGotUp = false;
    m_bIsFinished = false;
    m_Anim = nullptr;
}

CTaskSimpleGetUp::~CTaskSimpleGetUp() {
    if (m_Anim)
        m_Anim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
}

// 0x67FA80
bool CTaskSimpleGetUp::ProcessPed(CPed* ped) {
    ped->m_pedIK.bSlopePitch = true;

    if (m_bIsFinished)
        return true;

    if (!m_Anim)
        StartAnim(ped);

    if (m_Anim)
        ped->bStuckUnderCar = false;

    if (!m_Anim || m_Anim->m_CurrentTime < m_Anim->m_BlendHier->m_fTotalTime * 0.75F)
        ped->bFallenDown = true;

    return false;
}

// 0x677FE0
bool CTaskSimpleGetUp::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {

    if (priority == ABORT_PRIORITY_URGENT) {
        bool bTooMuchTimePassed = false;
        bool bFatalDamage = false;

        if (event) {
            if (event->GetEventType() == EVENT_DAMAGE) {
                const auto eventDamage = static_cast<const CEventDamage*>(event);
                if (eventDamage->m_damageResponse.m_bHealthZero && eventDamage->m_bAddToEventGroup)
                    bFatalDamage = true;
                else {
                    // Como el original: (uint32)(int)(ms_fTimeStep * 0.02f * 1000.0f) pasado a float y * 3.0f, comparado
                    // con el tiempo transcurrido como entero sin signo (exacto si cabe en int32, redondeado a float si no)
                    const uint32 elapsed  = CTimer::GetTimeInMS() - eventDamage->m_nStartTime;
                    const uint32 stepMs   = static_cast<uint32>(static_cast<int32>(CTimer::GetTimeStep() * 0.02f * 1000.0f));
                    const float  maxDelay = static_cast<float>(stepMs) * 3.0f;
                    const double elapsedF = static_cast<int32>(elapsed) >= 0
                        ? static_cast<double>(elapsed)
                        : static_cast<double>(static_cast<float>(static_cast<double>(static_cast<int32>(elapsed)) + 4294967296.0));
                    if (elapsedF > static_cast<double>(maxDelay))
                        bTooMuchTimePassed = true;
                }
            } else if (event->GetEventPriority() < 61)
                return false;
        }

        if (m_Anim && !bTooMuchTimePassed) {
            m_Anim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_Anim = nullptr;
            m_bHasPedGotUp = true;
            m_bIsFinished = true;
        } else if (!m_bHasPedGotUp && !bFatalDamage)
            return false;

        ped->bStuckUnderCar = false;
        return true;
    }

    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_Anim)
            m_Anim->m_BlendDelta = -1000.0F;
        ped->bStuckUnderCar = false;
        return true;
    }

    return false;
}

// 0x67C770
bool CTaskSimpleGetUp::StartAnim(CPed* ped) {
    auto vehicle = CPedPlacement::IsPositionClearOfCars(ped);

    if (!vehicle
        || vehicle->IsBike()
        || vehicle->IsSubQuad()
        || vehicle == ped->m_pAttachedTo
        || vehicle == ped->m_standingOnEntity
    ) {
        auto& entity = ped->m_pEntityIgnoredCollision;
        if (!entity
            || !entity->GetIsTypeVehicle()
            || entity->AsVehicle()->IsBike()
            || entity->AsVehicle()->IsSubQuad()
            || !IsVehiclePointerValid(entity->AsVehicle())
            // Como el original: la colisión solo se vuelve a probar uno de cada 8 fotogramas; los demás sigue atascado
            || ((ped->m_nRandomSeed + CTimer::GetFrameCounter() - 3) % 8 == 0
            && CCollision::ProcessColModels(
                ped->GetMatrix(),
                *CModelInfo::GetModelInfo(ped->m_nModelIndex)->GetColModel(),
                entity->GetMatrix(),
                *CModelInfo::GetModelInfo(entity->m_nModelIndex)->GetColModel(),
                m_aColPoints,
                nullptr,
                nullptr,
                false
            ) <= 0)
        ) {
            ped->m_pEntityIgnoredCollision = nullptr;

            m_Anim = CAnimManager::BlendAnimation(
                ped->GetRpClump(),
                ANIM_GROUP_DEFAULT,
                RpAnimBlendClumpGetFirstAssociation(ped->GetRpClump(), ANIMATION_IS_FRONT) ? ANIM_ID_GETUP_FRONT : ANIM_ID_GETUP_0,
                1000.0F
            );

            m_Anim->SetFinishCallback(FinishGetUpAnimCB, this);
            ped->SetPedState(PEDSTATE_IDLE);
            return true;
        }
    }

    ped->bStuckUnderCar = true;

    float fDamage;

    if (ped->IsPlayer()) {
        if (CTimer::GetTimeStep() > 0.0F)
            fDamage = CTimer::GetTimeStep();
        else
            return false;
    } else {
        if (CPad::GetPad(0)->DisablePlayerControls)
            fDamage = 1000.0F;
        else
            return false;
    }

    CPedDamageResponseCalculator damageResponseCalculator(vehicle, fDamage, WEAPON_RUNOVERBYCAR, PED_PIECE_TORSO, false);
    CEventDamage eventDamage(vehicle, CTimer::GetTimeInMS(), WEAPON_RUNOVERBYCAR, PED_PIECE_TORSO, 0, false, ped->bInVehicle);

    if (eventDamage.AffectsPed(ped))
        damageResponseCalculator.ComputeDamageResponse(ped, eventDamage.m_damageResponse, true);
    else
        eventDamage.m_damageResponse.m_bDamageCalculated = true;

    ped->GetIntelligence()->m_eventGroup.Add(&eventDamage, false);

    return false;
}

// 0x678110
void CTaskSimpleGetUp::FinishGetUpAnimCB(CAnimBlendAssociation* blendAssoc, void* data) {
    auto task = reinterpret_cast<CTaskSimpleGetUp*>(data);
    task->m_bHasPedGotUp = true;
    task->m_bIsFinished = true;
    task->m_Anim->m_BlendDelta = -1000.0F;
    task->m_Anim = nullptr;
}
