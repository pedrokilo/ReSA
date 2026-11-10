/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWalkRoundFire
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
#include "TaskComplexWalkRoundFire.h"
#include "TaskSimpleGoToPoint.h"

namespace {
// 0x655780 (lote 147A, banco Fn147A): cuerpo del destructor de CTaskComplexWalkRoundFire del EXE (con hook propio (Destructor); vtable 0x86F3E8). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor
// con borrado como el `call 0x655780` del EXE.
__declspec(noinline) void __fastcall CTaskComplexWalkRoundFireDestructorBody(CTaskComplexWalkRoundFire* self, void* /*edx*/) { self->CTaskComplexWalkRoundFire::~CTaskComplexWalkRoundFire(); }

// 0x6579F0 (lote 147A, banco Fn147A): destructor con borrado de CTaskComplexWalkRoundFire del EXE (ranura 0 de su vtable 0x86F3E8; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x655780; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexWalkRoundFire* __fastcall CTaskComplexWalkRoundFireScalarDeletingDestructor(CTaskComplexWalkRoundFire* self, void* /*edx*/, uint8 flags) {
    CTaskComplexWalkRoundFireDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote147ABody_CTaskComplexWalkRoundFire() { return reinterpret_cast<uintptr_t>(&CTaskComplexWalkRoundFireDestructorBody); } // para el banco Fn147A

void CTaskComplexWalkRoundFire::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkRoundFire, 0x86f3e8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexWalkRoundFireScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6579F0); // lote 147A (banco Fn147A)

    RH_ScopedInstall(Constructor, 0x655720);
    RH_ScopedInstall(Destructor, 0x655780);

    RH_ScopedInstall(ComputeDetourTarget, 0x6581B0);

    RH_ScopedVMTInstall(Clone, 0x655D40);
    RH_ScopedVMTInstall(GetTaskType, 0x655770);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x655790);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6584E0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6585C0);
}

// 0x655720
CTaskComplexWalkRoundFire::CTaskComplexWalkRoundFire(eMoveState moveState, CVector const& firePos, float fireRadius, CVector const& targetPos) :
    m_fireBoundSphere{firePos, fireRadius},
    m_moveState{moveState},
    m_targetPos{targetPos}
{
}

CTaskComplexWalkRoundFire::CTaskComplexWalkRoundFire(const CTaskComplexWalkRoundFire& o) :
    CTaskComplexWalkRoundFire{o.m_moveState, o.m_fireBoundSphere.m_vecCenter, o.m_fireBoundSphere.m_fRadius, o.m_targetPos}
{
}

// 0x6581B0
bool CTaskComplexWalkRoundFire::ComputeDetourTarget(const CPed& ped, CVector& outTarget) const {
    // Como el original: CColSphere en la pila con Set(radio, centro, 0, 0, 0xFF) (0x40FD10)
    CColSphere sphere;
    sphere.Set(m_fireBoundSphere.m_fRadius, m_fireBoundSphere.m_vecCenter, SURFACE_DEFAULT, 0, tColLighting(0xFF));
    CVector unused;
    return CPedGeometryAnalyser::ComputeRouteRoundSphere(
        ped,
        sphere,
        m_initialPedPos,
        m_targetPos,
        unused,
        outTarget
    );
}

// 0x6584E0
CTask* CTaskComplexWalkRoundFire::CreateFirstSubTask(CPed* ped) {
    m_initialPedPos = ped->GetPosition();
    const auto target = GetDetourTarget(*ped); // Como el original: la ruta se calcula antes de reservar la tarea
    return new CTaskSimpleGoToPoint{ m_moveState, target };
}

// 0x6585C0
CTask* CTaskComplexWalkRoundFire::ControlSubTask(CPed* ped) {
    static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->UpdatePoint(GetDetourTarget(*ped));
    return m_pSubTask;
}

// NOTSA
CVector CTaskComplexWalkRoundFire::GetDetourTarget(const CPed& ped) const {
    CVector detourTargetPos{};
    (void)ComputeDetourTarget(ped, detourTargetPos); // TODO: Use a macro to check if return value is true (in debug mode)
    return detourTargetPos;
}
