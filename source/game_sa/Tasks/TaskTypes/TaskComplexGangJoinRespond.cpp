/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGangJoinRespond
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

#include "TaskComplexGangJoinRespond.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskComplexGangLeader.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleStandStill.h"

namespace {
// 0x661720 (lote 145A, banco Fn145A): cuerpo del destructor de CTaskComplexGangJoinRespond del EXE (con hook propio (Destructor); vtable 0x86FB5C). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama
// el destructor con borrado como el `call 0x661720` del EXE.
__declspec(noinline) void __fastcall CTaskComplexGangJoinRespondDestructorBody(CTaskComplexGangJoinRespond* self, void* /*edx*/) { self->CTaskComplexGangJoinRespond::~CTaskComplexGangJoinRespond(); }

// 0x664A80 (lote 145A, banco Fn145A): destructor con borrado de CTaskComplexGangJoinRespond del EXE (ranura 0 de su vtable 0x86FB5C; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x661720; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexGangJoinRespond* __fastcall CTaskComplexGangJoinRespondScalarDeletingDestructor(CTaskComplexGangJoinRespond* self, void* /*edx*/, uint8 flags) {
    CTaskComplexGangJoinRespondDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote145ABody_CTaskComplexGangJoinRespond() { return reinterpret_cast<uintptr_t>(&CTaskComplexGangJoinRespondDestructorBody); } // para el banco Fn145A

void CTaskComplexGangJoinRespond::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGangJoinRespond, 0x86FB5C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexGangJoinRespondScalarDeletingDestructor, "ScalarDeletingDestructor", 0x664A80); // lote 145A (banco Fn145A)

    RH_ScopedInstall(Constructor, 0x6616F0);
    RH_ScopedInstall(Destructor, 0x661720);

    RH_ScopedVMTInstall(GetTaskType, 0x661710);
    RH_ScopedVMTInstall(Clone, 0x662290);
    RH_ScopedVMTInstall(MakeAbortable, 0x661790);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6617A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6618D0);
    RH_ScopedVMTInstall(ControlSubTask, 0x661950);
}

// 0x6616F0
CTaskComplexGangJoinRespond::CTaskComplexGangJoinRespond(bool response) :
    m_response{response}
{
}

// 0x661720
CTaskComplexGangJoinRespond::~CTaskComplexGangJoinRespond() {
    if (m_animsReferenced) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_animsReferenced = false;
    }
}

// 0x6617A0
CTask* CTaskComplexGangJoinRespond::CreateNextSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x190)) {
        return nullptr;
    }
    if (m_animsReferenced) {
        return new CTaskSimpleRunAnim(static_cast<AssocGroupId>(0x34), static_cast<AnimationId>(m_response == 1 ? 0x132 : 0x133), 4.0f, false);
    }
    if (++m_attempts > 10) {
        return nullptr;
    }
    return new CTaskSimpleStandStill(250, false, false, 8.0f);
}

// 0x6618D0
CTask* CTaskComplexGangJoinRespond::CreateFirstSubTask(CPed* ped) {
    m_attempts = 0;
    return new CTaskComplexTurnToFaceEntityOrCoord(FindPlayerPed(0), 0.5f, 0.2f);
}

// 0x661950
CTask* CTaskComplexGangJoinRespond::ControlSubTask(CPed* ped) {
    const auto& playerPos = FindPlayerPed(-1)->GetPosition();
    const auto& pedPos = ped->GetPosition();
    CVector direction{playerPos.x - pedPos.x, playerPos.y - pedPos.y, 0.0f};
    direction.Normalise();
    ped->m_fAimingRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(direction.x, direction.y, 0.0f, 0.0f));
    if (!m_animsReferenced) {
        if (CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            const auto index = CAnimManager::GetAnimationBlockIndex("gangs");
            if (CAnimManager::GetAnimBlocks()[index].IsLoaded == 1) {
                CAnimManager::AddAnimBlockRef(index);
                m_animsReferenced = true;
            } else {
                CStreaming::RequestModel(IFPToModelId(index), STREAMING_KEEP_IN_MEMORY);
            }
        }
    } else if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_animsReferenced = false;
    }
    return m_pSubTask;
}
