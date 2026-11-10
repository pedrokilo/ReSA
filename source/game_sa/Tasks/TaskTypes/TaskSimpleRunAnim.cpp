/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleRunAnim
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleRunAnim.h"

namespace {
// 0x48DE60 (lote 150A, banco Fn150A): cuerpo del destructor de CTaskSimpleRunAnim del EXE (sin hook: es el destructor de la clase; vtable 0x86D528). Funcion propia sin expansion en linea para que el banco pueda sustituirla;
// la llama el destructor con borrado como el `call 0x48DE60` del EXE.
__declspec(noinline) void __fastcall CTaskSimpleRunAnimDestructorBody(CTaskSimpleRunAnim* self, void* /*edx*/) { self->CTaskSimpleRunAnim::~CTaskSimpleRunAnim(); }

// 0x61BAA0 (lote 150A, banco Fn150A): destructor con borrado de CTaskSimpleRunAnim del EXE (ranura 0 de su vtable 0x86D528; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x48DE60; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleRunAnim* __fastcall CTaskSimpleRunAnimScalarDeletingDestructor(CTaskSimpleRunAnim* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleRunAnimDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote150ABody_CTaskSimpleRunAnim() { return reinterpret_cast<uintptr_t>(&CTaskSimpleRunAnimDestructorBody); } // para el banco Fn150A

void CTaskSimpleRunAnim::InjectHooks()
{
    RH_ScopedVirtualClass(CTaskSimpleRunAnim, 0x86D528, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleRunAnimScalarDeletingDestructor, "ScalarDeletingDestructor", 0x61BAA0); // lote 150A (banco Fn150A)
    RH_ScopedInstall(Constructor, 0x61A8B0);
    RH_ScopedInstall(Constructor2, 0x61A900);
    RH_ScopedInstall(StartAnim, 0x61A950);
    //VTABLE
    RH_ScopedVMTInstall(GetTaskType, 0x48DEA0); // lote 147A (banco Fn147A): devuelve m_nTaskType (word en +0x1C, ampliado a cero); el EXE comparte esta copia con las tareas derivadas
    RH_ScopedVMTInstall(Clone, 0x61B6D0);
    RH_ScopedVMTInstall(ProcessPed, 0x61BAC0);
}

// 0x48DEA0 (lote 147A, banco Fn147A; thiscall, `ret`, 5 bytes: `movzx eax,word [ecx+1Ch]`): devuelve m_nTaskType (word sin signo en +0x1C, ampliado a cero). El EXE comparte esta copia con las tareas derivadas.
eTaskType CTaskSimpleRunAnim::GetTaskType() const {
    return static_cast<eTaskType>(m_nTaskType);
}

CTaskSimpleRunAnim* CTaskSimpleRunAnim::Constructor(AssocGroupId animGroup, AnimationId animId, float fBlendDelta, bool bHoldLastFrame)
{
    this->CTaskSimpleRunAnim::CTaskSimpleRunAnim(animGroup, animId, fBlendDelta, bHoldLastFrame);
    return this;
}

CTaskSimpleRunAnim* CTaskSimpleRunAnim::Constructor2(AssocGroupId animGroup, AnimationId animId, float fBlendDelta, int32 nTaskType, const char* taskName, bool bHoldLastFrame)
{
    this->CTaskSimpleRunAnim::CTaskSimpleRunAnim(animGroup, animId, fBlendDelta, nTaskType, taskName, bHoldLastFrame);
    return this;
}

// 0x61A900
CTaskSimpleRunAnim::CTaskSimpleRunAnim(AssocGroupId animGroup, AnimationId animId, float fBlendDelta, int32 nTaskType, const char* taskName _IGNORED_, bool bHoldLastFrame) : CTaskSimpleAnim(bHoldLastFrame)
{
    m_nAnimGroup = animGroup;
    m_nAnimId = animId;
    m_fBlendDelta = fBlendDelta;
    m_nTaskType = nTaskType;
}

// 0x61A8B0
CTaskSimpleRunAnim::CTaskSimpleRunAnim(AssocGroupId animGroup, AnimationId animId, float fBlendDelta, bool bHoldLastFrame) : CTaskSimpleAnim(bHoldLastFrame)
{
    m_nAnimGroup = animGroup;
    m_nAnimId = animId;
    m_fBlendDelta = fBlendDelta;
    m_nTaskType = TASK_SIMPLE_ANIM;
}

// 0x61B6D0
CTask* CTaskSimpleRunAnim::Clone() const {
    return new CTaskSimpleRunAnim(m_nAnimGroup, m_nAnimId, m_fBlendDelta, m_bHoldLastFrame);
}

// 0x61BAC0
bool CTaskSimpleRunAnim::ProcessPed(CPed* ped)
{
    if (m_bIsFinished)
        return true;

    if (!m_pAnim && !RpAnimBlendClumpGetAssociation(ped->GetRpClump(), m_nAnimId))
        StartAnim(ped);

    return m_bIsFinished;
}

// 0x61A950
void CTaskSimpleRunAnim::StartAnim(CPed* ped)
{
    m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), m_nAnimGroup, m_nAnimId, m_fBlendDelta);
    m_pAnim->SetFinishCallback(CTaskSimpleAnim::FinishRunAnimCB, this);
}

