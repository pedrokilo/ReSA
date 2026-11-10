/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexPassObject
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
#include "TaskComplexPassObject.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "Ragdoll/IKChainManager.h"

namespace {
// 0x661230 (lote 156A, banco Fn156A): cuerpo del destructor de CTaskComplexPassObject del EXE (con hook propio, Destructor; vtable 0x86FB18). Funcion propia sin expansion en linea para que el banco pueda
// sustituirla; la llama el destructor con borrado como el `call 0x661230` del EXE.
__declspec(noinline) void __fastcall PassObjectDestructorBody(CTaskComplexPassObject* self, void* /*edx*/) { self->CTaskComplexPassObject::~CTaskComplexPassObject(); }

// 0x664A30 (lote 156A, banco Fn156A; el listado decia CTaskLeanOnVehicle): destructor con borrado de CTaskComplexPassObject del EXE (ranura 0 de su vtable 0x86FB18; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x661230; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskComplexPassObject* __fastcall PassObjectScalarDeletingDestructor(CTaskComplexPassObject* self, void* /*edx*/, uint8 flags) {
    PassObjectDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote156ABody_CTaskComplexPassObject() { return reinterpret_cast<uintptr_t>(&PassObjectDestructorBody); } // para el banco Fn156A

void CTaskComplexPassObject::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPassObject, 0x86fb18, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(PassObjectScalarDeletingDestructor, "ScalarDeletingDestructor", 0x664A30); // lote 156A (banco Fn156A)
    RH_ScopedInstall(Constructor, 0x6611B0);
    RH_ScopedInstall(Destructor, 0x661230);

    RH_ScopedInstall(AbortIK, 0x661290);

    RH_ScopedVMTInstall(Clone, 0x662220);
    RH_ScopedVMTInstall(GetTaskType, 0x661220);
    RH_ScopedVMTInstall(MakeAbortable, 0x664A50);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6612C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6613A0);
    RH_ScopedVMTInstall(ControlSubTask, 0x661570);
}

// 0x6611B0
CTaskComplexPassObject::CTaskComplexPassObject(CPed* ped, bool isPasser) :
    m_ped{ped},
    m_isPasser{isPasser}
{
    CEntity::SafeRegisterRef(m_ped);
}

CTaskComplexPassObject::CTaskComplexPassObject(const CTaskComplexPassObject& o) :
    CTaskComplexPassObject{ o.m_ped, o.m_isPasser }
{
}

// 0x661230
CTaskComplexPassObject::~CTaskComplexPassObject() {
    CEntity::SafeCleanUpRef(m_ped); // Como el original: m_entityToPass no se limpia
}

// 0x661290
void CTaskComplexPassObject::AbortIK(CPed* ped) {
    if (g_ikChainMan.IsArmPointing(eIKArm::IK_ARM_RIGHT, ped)) {
        g_ikChainMan.AbortPointArm(eIKArm::IK_ARM_RIGHT, ped, 500);
    }
}

// 0x664A50
bool CTaskComplexPassObject::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    AbortIK(ped);
    return true;
}

// 0x6612C0
CTask* CTaskComplexPassObject::CreateNextSubTask(CPed* ped) {
    if (m_ped && m_pSubTask->GetTaskType() == TASK_COMPLEX_TURN_TO_FACE_ENTITY) {
        g_ikChainMan.PointArm("CTaskComplexPassObject", eIKArm::IK_ARM_RIGHT, ped, nullptr, eBoneTag::BONE_UNKNOWN, &m_targetPos, 0.25f, 250, 30.f);
        return new CTaskSimpleStandStill{ 999'999 };
    }
    AbortIK(ped);
    return nullptr;
}

// 0x6613A0
CTask* CTaskComplexPassObject::CreateFirstSubTask(CPed* ped) {
    // Como el original: primero se comprueba `ped` y despues `m_ped`
    if (m_ped && !ped->IsPlayer() && !m_ped->IsPlayer()) {
        // Find entity to pass
        {
            const auto passer = m_isPasser ? ped : m_ped;
            m_entityToPass = passer->GetEntityThatThisPedIsHolding();
            CEntity::SafeRegisterRef(m_entityToPass); // Como el original: puede ser nulo
        }
        // Como el original: (m_ped - ped), suma (x*x + y*y) + z*z comparada con 1.5^2 (NaN no pasa)
        const auto& pedPos = ped->GetPosition();
        const auto& otherPos = m_ped->GetPosition();
        const float dz = otherPos.z - pedPos.z, dy = otherPos.y - pedPos.y, dx = otherPos.x - pedPos.x;
        if (dx * dx + dy * dy + dz * dz <= sq(1.5f)) {
            m_timer.Start(1'000); // Como el original: arranca el temporizador de 1 s
            const auto& pos = m_ped->GetPosition();
            m_targetPos = CVector{ dx * 0.5f + pos.x, dy * 0.5f + pos.y, dz * 0.5f + pos.z }; // Position inbetween the 2 peds
            return new CTaskComplexTurnToFaceEntityOrCoord{ m_ped }; // Como el original: mira al otro peaton (0.5, 0.2)
        }
    }
    AbortIK(ped);
    return nullptr;
}

// 0x661570
CTask* CTaskComplexPassObject::ControlSubTask(CPed* ped) {
    if (m_ped) {
        if (!m_timer.IsOutOfTime()) {
            return m_pSubTask;
        }

        if (!m_isPasser && m_entityToPass) {
            if (const auto holdTask = static_cast<CTaskSimpleHoldEntity*>(m_ped->GetTaskManager().FindActiveTaskByType(TASK_SIMPLE_HOLD_ENTITY))) {
                auto heldEntity = holdTask->m_pEntityToHold;
                holdTask->DropEntity(m_ped, false);
                const CVector offset{ 0.f, 0.f, 0.f };
                ped->GetTaskManager().SetTaskSecondary(
                    new CTaskSimpleHoldEntity{ heldEntity, &offset, (uint8)BONE_L_BROW, HOLD_ENTITY_UPDATE_TRANSLATION_ONLY, ANIM_ID_NO_ANIMATION_SET, ANIM_GROUP_DEFAULT, true },
                    TASK_SECONDARY_PARTIAL_ANIM
                );
            }
        }
    }

    AbortIK(ped);
    return nullptr;
}
