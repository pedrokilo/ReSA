/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexTreatAccident
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexTreatAccident.h"

#include "TaskSimpleGiveCPR.h"
#include "TaskSimpleNone.h"
#include "TaskSimpleAchieveHeading.h"
#include "Ragdoll/IKChainManager.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
}

namespace {
// 0x658AE0 (lote 143A, banco Fn143A; thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CTaskComplexTreatAccident del EXE (`mov [ecx],0x86F4EC; jmp 0x61A3D0`). Deja la vtable de
// CTaskComplexTreatAccident del EXE (0x86F4EC) y sigue con el cuerpo de ~CTaskComplex (0x61A3D0, con hook).
void __fastcall TreatAccidentDestructorBody(CTaskComplexTreatAccident* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86F4EC;
    CTaskComplexBody::Destructor(self, nullptr);
}

// 0x659E70 (lote 193A, sin validar; el listado decia CTaskComplexUseWaterCannon: es la ranura 0 de la vtable 0x86F4EC de CTaskComplexTreatAccident; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x658AE0;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x658AE0, TreatAccidentDestructorBody, con hook), libera `this` con
// CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskComplexTreatAccident* __fastcall TreatAccidentScalarDeletingDestructor(CTaskComplexTreatAccident* self, void* /*edx*/, uint8 flags) {
    TreatAccidentDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexTreatAccident::InjectHooks()
{
    RH_ScopedVirtualClass(CTaskComplexTreatAccident, 0x86F4EC, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(TreatAccidentDestructorBody, "Destructor", 0x658AE0); // lote 143A (banco Fn143A)
    RH_ScopedNamedGlobalInstall(TreatAccidentScalarDeletingDestructor, "ScalarDeletingDestructor", 0x659E70); // lote 193A (sin validar)
    RH_ScopedVMTInstall(GetTaskType, 0x658AD0);                                       // lote 143A: retorno de constante (101)
    RH_ScopedInstall(Constructor, 0x658AB0);
    RH_ScopedInstall(CreateSubTask, 0x659E90);
    RH_ScopedInstall(ComputeHeading, 0x658AF0);
    RH_ScopedVMTInstall(Clone, 0x659A90);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65A8F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65A830);
    RH_ScopedVMTInstall(ControlSubTask, 0x658B90);
}

// 0x658AB0
CTaskComplexTreatAccident* CTaskComplexTreatAccident::Constructor(CAccident* pAcc)
{
    this->CTaskComplexTreatAccident::CTaskComplexTreatAccident(pAcc);
    return this;
}

// 0x658AB0
CTaskComplexTreatAccident::CTaskComplexTreatAccident(CAccident* accident)
{
    m_pAccident = accident;
}

// 0x65A830
CTask* CTaskComplexTreatAccident::CreateNextSubTask(CPed* ped)
{
    eTaskType subTaskId = m_pSubTask->GetTaskType();

    if (subTaskId == TASK_SIMPLE_GIVE_CPR || subTaskId == TASK_NONE)
        return CreateSubTask(TASK_FINISHED, ped);

    if (subTaskId == TASK_SIMPLE_ACHIEVE_HEADING)
    {
        CPed* targetPed = m_pAccident->m_pPed;

        if (targetPed && !targetPed->bFadeOut)
        {
            targetPed->m_nDeathTimeMS = CTimer::GetTimeInMS();
            auto newSubTask = CreateSubTask(TASK_SIMPLE_GIVE_CPR, ped);
            ped->m_fCurrentRotation = ComputeHeading(ped);
            ped->m_fAimingRotation = ped->m_fCurrentRotation;
            return newSubTask;
        }
        else
            return CreateSubTask(TASK_FINISHED, ped);
    }

    return nullptr;
}

// 0x65A8F0
CTask* CTaskComplexTreatAccident::CreateFirstSubTask(CPed* ped)
{
    CPed* targetPed = m_pAccident->m_pPed;

    if (targetPed && !targetPed->bFadeOut)
    {
        targetPed->m_nDeathTimeMS = CTimer::GetTimeInMS();
        ped->Say(CTX_GLOBAL_VICTIM, 0, 1.0F, false, false, false);
        g_ikChainMan.LookAt("TaskTreatAccident", ped, targetPed, 5000, BONE_HEAD, nullptr, true, 0.25F, 500, 3, false);
        return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING, ped);
    }
    else
        return CreateSubTask(TASK_NONE, ped);
}

// 0x658B90
CTask* CTaskComplexTreatAccident::ControlSubTask(CPed* ped)
{
    return m_pSubTask;
}

// 0x659A90
CTask* CTaskComplexTreatAccident::Clone() const {
    return new CTaskComplexTreatAccident(m_pAccident);
}

// 0x659E90
CTask* CTaskComplexTreatAccident::CreateSubTask(eTaskType taskType, CPed* ped)
{
    switch (taskType)
    {
    case TASK_SIMPLE_GIVE_CPR:
        return new CTaskSimpleGiveCPR(m_pAccident);
    case TASK_NONE:
        return new CTaskSimpleNone();
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return new CTaskSimpleAchieveHeading(ComputeHeading(ped), 0.5F, 0.2F);
    default:
        return nullptr;
    }
}

// 0x658AF0
float CTaskComplexTreatAccident::ComputeHeading(CPed* ped) {
    // Head first, then pelvis (the bone queries may update the skeleton); only X/Y of the midpoint are used.
    const auto head   = m_pAccident->m_pPed->GetBonePosition(BONE_HEAD);
    const auto pelvis = m_pAccident->m_pPed->GetBonePosition(BONE_PELVIS);
    const auto& pos   = ped->GetPosition();
    return CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints((head.x + pelvis.x) * 0.5f, (head.y + pelvis.y) * 0.5f, pos.x, pos.y));
}
