/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleAchieveHeading
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
#include "X87Math.h"

#include "TaskSimpleAchieveHeading.h"
#include "Ragdoll/IKChainManager.h"

namespace {
// 0x667E70 (lote 122A, banco Fn122A): cuerpo no virtual de ~CTaskSimpleAchieveHeading del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx], 0x86FD74 ; jmp 0x61A3A0`).
// Deja la vtable de CTaskSimpleAchieveHeading del EXE (0x86FD74) y salta a CTask::Destructor (0x61A3A0), que deja la vtable de CTask (0x86D48C). Lo llaman el funclet de limpieza (SEH) y el destructor con borrado.
__declspec(noinline) void __fastcall AchieveHeadingDestructorBody(CTaskSimpleAchieveHeading* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86FD74;
    self->CTask::Destructor();
}

// 0x66DB60 (lote 146A, banco Fn146A): destructor con borrado de CTaskSimpleAchieveHeading del EXE (ranura 0 de su vtable 0x86FD74; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x667E70; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x667E70, con hook; aqui
// su funcion AchieveHeadingDestructorBody, sin expansion en linea), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleAchieveHeading* __fastcall AchieveHeadingScalarDeletingDestructor(CTaskSimpleAchieveHeading* self, void* /*edx*/, uint8 flags) {
    AchieveHeadingDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimpleAchieveHeading::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleAchieveHeading, 0x86fd74, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(AchieveHeadingDestructorBody, "Destructor", 0x667E70);
    RH_ScopedNamedGlobalInstall(AchieveHeadingScalarDeletingDestructor, "ScalarDeletingDestructor", 0x66DB60); // lote 146A (banco Fn146A)
    RH_ScopedInstall(Constructor, 0x667E20);
    RH_ScopedInstall(SetUpIK, 0x667F20);
    RH_ScopedVMTInstall(Clone, 0x66CCF0);
    RH_ScopedVMTInstall(GetTaskType, 0x667E60);
    RH_ScopedVMTInstall(MakeAbortable, 0x667EB0);
    RH_ScopedVMTInstall(ProcessPed, 0x668060);
}

// 0x667E20
CTaskSimpleAchieveHeading::CTaskSimpleAchieveHeading(float desiredHeading, float changeRateMult, float tolerance) :
    m_DesiredHeadingRad{ desiredHeading },
    m_HeadingChangeRate{ changeRateMult },
    m_HeadingToleranceRad{ tolerance }
{
}

//! @notsa
CTaskSimpleAchieveHeading::CTaskSimpleAchieveHeading(CPed* looker, CEntity* lookTowards, float changeRateMult, float tolerance) :
    CTaskSimpleAchieveHeading{
        (lookTowards->GetPosition2D() - looker->GetPosition2D()).Heading(),
        changeRateMult,
        tolerance
    }
{
}

// 0x668060
bool CTaskSimpleAchieveHeading::ProcessPed(CPed* ped) {
    if (ped->bInVehicle) {
        return true;
    }

    ped->RestoreHeadingRate();
    ped->m_fHeadingChangeRate *= m_HeadingChangeRate;
    ped->SetMoveState(PEDMOVE_STILL);
    ped->SetMoveAnim();
    ped->m_fAimingRotation = m_DesiredHeadingRad;

    if (std::fabs(CGeneral::LimitRadianAngle(std::fabs(m_DesiredHeadingRad - ped->m_fCurrentRotation))) < m_HeadingToleranceRad) {
        ped->m_fAimingRotation = ped->m_fCurrentRotation;
        ped->RestoreHeadingRate();
        QuitIK(ped);
        return true;
    }

    SetUpIK(ped);
    return false;
}

// 0x667EB0
bool CTaskSimpleAchieveHeading::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    QuitIK(ped);

    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE:
        ped->m_fAimingRotation = ped->m_fCurrentRotation;
        ped->RestoreHeadingRate();
        return true;
    default:
        m_HeadingToleranceRad  = std::bit_cast<float>(0x40C90FDBu); // Como el original: 2*pi exacto (TWO_PI de common.h es impreciso)
        ped->m_fAimingRotation = ped->m_fCurrentRotation;
        return false;
    }
}

// 0x667E80
void CTaskSimpleAchieveHeading::QuitIK(CPed* ped) const {
    if (m_IsDoingIK && g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.AbortLookAt(ped, 250);
    }
}

// 0x667F20
void CTaskSimpleAchieveHeading::SetUpIK(CPed* ped) {
    if (ped == FindPlayerPed() && !CPad::GetPad()->DisablePlayerControls) {
        return;
    }

    if (!ped->GetIsOnScreen()) {
        return;
    }

    if (m_IsDoingIK || g_ikChainMan.GetLookAtEntity(ped) || ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_IK)) {
        return;
    }

    // Como el original: GetTaskType del padre se llama dos veces (primero contra AVOID_OTHER_PED_WHILE_WANDERING)
    if (m_Parent && (m_Parent->GetTaskType() == TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING || m_Parent->GetTaskType() == TASK_COMPLEX_AVOID_ENTITY)) {
        return;
    }

    // Como el original: fld h; fsin/fcos (precisión interna completa); fadd st,st; fadd pos (redondeo de la FPU)
    const CVector& pos = ped->GetPosition();
    const float heading = m_DesiredHeadingRad, posX = pos.x, posY = pos.y;
    const float lookX = TrigCompoundSinMulAdd(heading, 2.0f, posX); // X87Math.cpp
    const float lookY = TrigCompoundCosMulAdd(heading, 2.0f, posY);
    CVector lookAt{ lookX, lookY, pos.z + 0.61f };
    g_ikChainMan.LookAt("TaskAchvHeading", ped, nullptr, 5'000, BONE_UNKNOWN, &lookAt, false, 0.25f, 500, 3, false);
    m_IsDoingIK = true;
}

// @addr unk
void CTaskSimpleAchieveHeading::SetHeading(float heading, float maxHeading, float changeRateMult) {
    m_DesiredHeadingRad   = heading;
    m_HeadingChangeRate   = changeRateMult;
    m_HeadingToleranceRad = maxHeading;
}
