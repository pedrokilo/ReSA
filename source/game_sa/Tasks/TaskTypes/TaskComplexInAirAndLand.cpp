/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexInAirAndLand
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexInAirAndLand.h"
#include "TaskSimpleGetUp.h"
#include "TaskSimpleFall.h"
#include "TaskSimpleLand.h"
#include "TaskSimpleClimb.h"
#include "TaskSimpleInAir.h"

#include "PedClothesDesc.h"

void CTaskComplexInAirAndLand::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexInAirAndLand, 0x8704AC, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x678C80);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x67CC30);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x67CCB0);
    RH_ScopedVMTInstall(ControlSubTask, 0x67D230);
}

CTaskComplexInAirAndLand* CTaskComplexInAirAndLand::Constructor(bool bUsingJumpGlide, bool bUsingFallGlide) {
    this->CTaskComplexInAirAndLand::CTaskComplexInAirAndLand(bUsingJumpGlide, bUsingFallGlide);
    return this;
}

// 0x678C80
CTaskComplexInAirAndLand::CTaskComplexInAirAndLand(bool bUsingJumpGlide, bool bUsingFallGlide) : CTaskComplex() {
    m_bUsingJumpGlide = bUsingJumpGlide;
    m_bUsingFallGlide = bUsingFallGlide;
    m_bInvalidClimb   = false;
}

// 0x67CC30
CTask* CTaskComplexInAirAndLand::CreateFirstSubTask(CPed* ped) {
    return new CTaskSimpleInAir(m_bUsingJumpGlide, m_bUsingFallGlide, false);
}

// 0x67CCB0
CTask* CTaskComplexInAirAndLand::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GET_UP:
    case TASK_SIMPLE_LAND:
    case TASK_SIMPLE_CLIMB:
        return nullptr;
    case TASK_SIMPLE_FALL:
        return new CTaskSimpleGetUp();
    case TASK_SIMPLE_IN_AIR: {
        auto subTask = reinterpret_cast<CTaskSimpleInAir*>(m_pSubTask);

        if (m_bUsingFallGlide) {
            if (subTask->m_pAnim) {
                subTask->m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
                subTask->m_pAnim->m_BlendDelta = -8.0F;
                subTask->m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
                subTask->m_pAnim = nullptr;
            }

            return new CTaskSimpleLand(ped->m_vecMoveSpeed.z < -0.1F ? ANIM_ID_KO_SKID_BACK : (AnimationId)-1);
        } else if (subTask->m_pAnim && subTask->m_pAnim->m_AnimId == ANIM_ID_FALL_FALL) {
            CTask* newTask;

            if (subTask->m_fMinZSpeed < -0.4F)
                newTask = new CTaskSimpleFall(ANIM_ID_KO_SKID_BACK, ANIM_GROUP_DEFAULT, 700);
            else
                newTask = new CTaskSimpleLand(ANIM_ID_FALL_COLLAPSE);

            ped->GetAE().AddAudioEvent(AE_PED_COLLAPSE_AFTER_FALL);

            if (ped->GetPlayerData()) {
                CVector          empty{};
                CEventSoundQuiet eventSound(ped, ped->GetPlayerData()->m_pPedClothesDesc->GetIsWearingBalaclava() ? 75.0F : 60.0F, -1, empty);
                GetEventGlobalGroup()->Add(&eventSound, false);
            }

            return newTask;
        } else {
            AnimationId landAnimId;

            if (!ped->IsPlayer())
                landAnimId = ANIM_ID_FALL_LAND;
            else {
                // Como el original: GetPadFromPlayer (0x609560) se llama antes de comprobar la animación
                auto pad = ped->AsPlayer()->GetPadFromPlayer();
                if (subTask->m_pAnim && subTask->m_pAnim->m_AnimId == ANIM_ID_IDLE_HBHB_1)
                    landAnimId = ANIM_ID_IDLE_TIRED;
                // Como el original: 0x53FC90 (izquierda/derecha) antes que 0x53FD30 (arriba/abajo)
                else if (ped->GetPlayerData()->m_fMoveBlendRatio > 1.5F && pad && (pad->GetPedWalkLeftRight() != 0.0F || pad->GetPedWalkUpDown() != 0.0F))
                    landAnimId = ANIM_ID_JUMP_LAND;
                else
                    landAnimId = ANIM_ID_FALL_LAND;
            }

            auto newTask = new CTaskSimpleLand(landAnimId);

            ped->GetAE().AddAudioEvent(AE_PED_LAND_ON_FEET_AFTER_FALL);

            if (ped->GetPlayerData()) {
                CVector          empty{};
                CEventSoundQuiet eventSound(ped, ped->GetPlayerData()->m_pPedClothesDesc->GetIsWearingBalaclava() ? 70.0F : 55.0F, -1, empty);
                GetEventGlobalGroup()->Add(&eventSound, false);
            }

            return newTask;
        }
    }
    default:
        return nullptr;
    }
}

// 0x67D230
CTask* CTaskComplexInAirAndLand::ControlSubTask(CPed* ped) {
    if (!m_bUsingFallGlide && m_pSubTask && m_pSubTask->GetTaskType() == TASK_SIMPLE_IN_AIR) {
        auto subTask = reinterpret_cast<CTaskSimpleInAir*>(m_pSubTask);

        if (!m_bInvalidClimb && subTask->m_pClimbEntity)
            return new CTaskSimpleClimb(
                subTask->m_pClimbEntity,
                subTask->m_vecPosn,
                subTask->m_fAngle,
                subTask->m_nSurfaceType,
                subTask->m_vecPosn.z - ped->GetPosition().z < CTaskSimpleClimb::ms_fMinForStretchGrab - 0.3F ? CLIMB_PULLUP : CLIMB_GRAB,
                false
            );
    }

    return m_pSubTask;
}
