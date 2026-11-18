/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexInAirAndLand
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

#include "TaskComplexInAirAndLand.h"
#include "TaskSimpleGetUp.h"
#include "TaskSimpleFall.h"
#include "TaskSimpleLand.h"
#include "TaskSimpleClimb.h"
#include "TaskSimpleInAir.h"

#include "PedClothesDesc.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
}

namespace {
static_assert(TASK_COMPLEX_IN_AIR_AND_LAND == 240); // 0xF0 (0x678CB0)

// 0x678CC0 (lote 148A, banco Fn148A; thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],0x8704AC; jmp 0x61A3D0`; el listado lo atribuia a CTaskComplexInAirAndLand sin nombre): cuerpo no virtual de
// ~CTaskComplexInAirAndLand del EXE (ningun llamador directo: lo alcanzan funclets de desenrollado). Deja la vtable de la clase del EXE (0x8704AC) y sigue con ~CTaskComplex (0x61A3D0, con hook).
void __fastcall InAirAndLandDestructorBody(CTaskComplexInAirAndLand* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x8704AC;
    CTaskComplexBody::Destructor(self, nullptr);
}

// 0x67CC10 (lote 192A, sin validar; el listado decia CTaskComplexFallAndGetUp: es la ranura 0 de la vtable 0x8704AC de CTaskComplexInAirAndLand; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x678CC0;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo (0x678CC0, InAirAndLandDestructorBody), libera `this` con CTask::operator delete (0x61A5B0,
// con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CTaskComplexInAirAndLand* __fastcall InAirAndLandScalarDeletingDestructor(CTaskComplexInAirAndLand* self, void* /*edx*/, uint8 flags) {
    InAirAndLandDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexInAirAndLand::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexInAirAndLand, 0x8704AC, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x678C80);
    RH_ScopedNamedGlobalInstall(InAirAndLandDestructorBody, "Destructor", 0x678CC0); // lote 148A (banco Fn148A)
    RH_ScopedNamedGlobalInstall(InAirAndLandScalarDeletingDestructor, "ScalarDeletingDestructor", 0x67CC10); // lote 192A (sin validar)
    RH_ScopedVMTInstall(GetTaskType, 0x678CB0); // lote 148A (banco Fn148A): retorno de constante (240), ranura 4
    RH_ScopedVMTInstall(Clone, 0x67C2E0);       // lote 200A (sin validar)
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x67CC30);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x67CCB0);
    RH_ScopedVMTInstall(ControlSubTask, 0x67D230);
}

// 0x678C80
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

// 0x67C2E0 (lote 200A, sin validar; thiscall, sin argumentos, `ret`, 120 bytes con marco SEH que solo desenrolla): reserva 0x10 bytes con CTask::operator new (0x61A5A0; sin memoria devuelve nulo) y construye una copia: lee los
// bytes +0xC y +0xD y los normaliza con `!= 0` (setne) ANTES de construir; tras la base CTaskComplex (0x61A3B0) escribe +0xC, +0xD, vtable 0x8704AC y +0xE = 0 (el constructor 0x678C80).
CTask* CTaskComplexInAirAndLand::Clone() const {
    const bool usingJumpGlide = m_bUsingJumpGlide != 0;
    const bool usingFallGlide = m_bUsingFallGlide != 0;
    void* const memory = CTask::operator new(sizeof(CTaskComplexInAirAndLand));
    if (!memory) {
        return nullptr;
    }
    return ::new (memory) CTaskComplexInAirAndLand(usingJumpGlide, usingFallGlide);
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
