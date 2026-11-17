/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexTurnToFaceEntityOrCoord
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleAchieveHeading.h"

namespace {
// 0x670830 (lote 210A, sin validar; el listado lo atribuia a otra clase): destructor con borrado de CTaskComplexTurnToFaceEntityOrCoord del EXE (ranura 0 de su vtable; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x66B960;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del
// BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskComplexTurnToFaceEntityOrCoord* __fastcall TurnToFaceScalarDeletingDestructor(CTaskComplexTurnToFaceEntityOrCoord* self, void* /*edx*/, uint8 flags) {
    self->Destructor(); // cuerpo enganchado (con hook)
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexTurnToFaceEntityOrCoord::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexTurnToFaceEntityOrCoord, 0x86ff94, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedOverloadedInstall(Constructor, "Coords", 0x66B890, CTaskComplexTurnToFaceEntityOrCoord*(CTaskComplexTurnToFaceEntityOrCoord::*)(CEntity*, float, float));
    RH_ScopedOverloadedInstall(Constructor, "Entity", 0x66B910, CTaskComplexTurnToFaceEntityOrCoord*(CTaskComplexTurnToFaceEntityOrCoord::*)(CVector const&, float, float));
    RH_ScopedInstall(Destructor, 0x66B960);
    RH_ScopedNamedGlobalInstall(TurnToFaceScalarDeletingDestructor, "ScalarDeletingDestructor", 0x670830); // lote 210A (sin validar)

    RH_ScopedInstall(ComputeTargetHeading, 0x66B9D0);

    RH_ScopedVMTInstall(Clone, 0x66D250);
    RH_ScopedVMTInstall(GetTaskType, 0x66B900);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66B9C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x670850);
    RH_ScopedVMTInstall(ControlSubTask, 0x670920);
}

// 0x66B890
CTaskComplexTurnToFaceEntityOrCoord::CTaskComplexTurnToFaceEntityOrCoord(CEntity* entity, float changeRateMult, float maxHeading) :
    m_EntityToFace{ entity },
    m_fChangeRateMult{ changeRateMult },
    m_fMaxHeading{ maxHeading },
    m_bFaceEntity{ true }
{
    assert((uintptr_t)entity != (uintptr_t)-1);
    CEntity::SafeRegisterRef(m_EntityToFace);
}

// 0x66B910
CTaskComplexTurnToFaceEntityOrCoord::CTaskComplexTurnToFaceEntityOrCoord(CVector const& coords, float changeRateMult, float maxHeading) :
    m_CoordsToFace{ coords },
    m_fChangeRateMult{ changeRateMult },
    m_fMaxHeading{ maxHeading },
    m_bFaceEntity{ false }
{
}

// 0x66B960
CTaskComplexTurnToFaceEntityOrCoord::~CTaskComplexTurnToFaceEntityOrCoord() {
    CEntity::SafeCleanUpRef(m_EntityToFace);
}

// 0x66D250
CTask* CTaskComplexTurnToFaceEntityOrCoord::Clone() const {
    return m_bFaceEntity
        ? new CTaskComplexTurnToFaceEntityOrCoord(m_EntityToFace, m_fChangeRateMult, m_fMaxHeading)
        : new CTaskComplexTurnToFaceEntityOrCoord(m_CoordsToFace, m_fChangeRateMult, m_fMaxHeading);
}

// 0x66B9D0
float CTaskComplexTurnToFaceEntityOrCoord::ComputeTargetHeading(CPed* ped) const {
    // 0x66BA2C: normalised direction, then the angle from it towards the origin (not `CVector::Heading`)
    auto dir = ComputeTargetPos() - ped->GetPosition();
    dir.Normalise();
    return CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.f, 0.f);
}

CVector CTaskComplexTurnToFaceEntityOrCoord::ComputeTargetPos() const {
    return m_bFaceEntity
        ? m_EntityToFace->GetPosition()
        : m_CoordsToFace;
}

// 0x670850
CTask* CTaskComplexTurnToFaceEntityOrCoord::CreateFirstSubTask(CPed* ped) {
    return m_bFaceEntity && !m_EntityToFace
        ? nullptr
        : new CTaskSimpleAchieveHeading(ComputeTargetHeading(ped), m_fChangeRateMult, m_fMaxHeading);
}

// 0x670920
CTask* CTaskComplexTurnToFaceEntityOrCoord::ControlSubTask(CPed* ped) {
    if (m_bFaceEntity) {
        const auto heading = m_EntityToFace
            ? ComputeTargetHeading(ped)
            : ped->m_fCurrentRotation;
        notsa::cast<CTaskSimpleAchieveHeading>(m_pSubTask)->SetHeading(heading, m_fMaxHeading, m_fChangeRateMult);
    }
    return m_pSubTask;
}
