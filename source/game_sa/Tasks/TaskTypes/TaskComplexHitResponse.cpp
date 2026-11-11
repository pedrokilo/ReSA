/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexHitResponse
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

#include "TaskComplexHitResponse.h"
#include "TaskSimpleHitFromFront.h"
#include "TaskSimpleHitFromLeft.h"
#include "TaskSimpleHitFromBack.h"
#include "TaskSimpleHitFromRight.h"

namespace {
// 0x631DA0 (lote 157A, banco Fn157A): cuerpo del destructor de CTaskComplexHitResponse del EXE (con hook (Destructor)). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x631DA0` del EXE.
__declspec(noinline) void __fastcall CTaskComplexHitResponseDestructorBody(CTaskComplexHitResponse* self, void* /*edx*/) { self->CTaskComplexHitResponse::~CTaskComplexHitResponse(); }

// 0x638020 (lote 157A, banco Fn157A): destructor con borrado de CTaskComplexHitResponse del EXE (ranura 0 de su vtable 0x86E040; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x631DA0; test byte [esp+8],1; je ..; push esi;
// call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta
// puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexHitResponse* __fastcall CTaskComplexHitResponseScalarDeletingDestructor(CTaskComplexHitResponse* self, void* /*edx*/, uint8 flags) {
    CTaskComplexHitResponseDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote157ABody_CTaskComplexHitResponse() { return reinterpret_cast<uintptr_t>(&CTaskComplexHitResponseDestructorBody); } // para el banco Fn157A

void CTaskComplexHitResponse::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexHitResponse, 0x86e040, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexHitResponseScalarDeletingDestructor, "ScalarDeletingDestructor", 0x638020); // lote 157A (banco Fn157A)

    RH_ScopedInstall(Constructor, 0x631D70);
    RH_ScopedInstall(Destructor, 0x631DA0);

    RH_ScopedVMTInstall(Clone, 0x6365E0);
    RH_ScopedVMTInstall(GetTaskType, 0x631D90);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x631DB0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x638040);
    RH_ScopedVMTInstall(ControlSubTask, 0x631DC0);
}

// 0x631D70
CTaskComplexHitResponse::CTaskComplexHitResponse(eDirection hs) :
    m_HitSide{ hs }
{
}

CTaskComplexHitResponse::CTaskComplexHitResponse(const CTaskComplexHitResponse& o) :
    CTaskComplexHitResponse{ o.m_HitSide }
{
    m_HitSide.get_underlying_ref() = o.m_HitSide.get_underlying(); // Como el original: copia el dword completo
}

// 0x638040
CTask* CTaskComplexHitResponse::CreateFirstSubTask(CPed* ped) {
    switch (m_HitSide.get_underlying()) { // Como el original: `cmp ecx, 3; ja` sobre el dword completo
    case (int32)eDirection::FORWARD:  return new CTaskSimpleHitFromFront{};
    case (int32)eDirection::LEFT:     return new CTaskSimpleHitFromLeft{};
    case (int32)eDirection::BACKWARD: return new CTaskSimpleHitFromBack{};
    case (int32)eDirection::RIGHT:    return new CTaskSimpleHitFromRight{};
    default:                   return nullptr; // Como el original: `cmp ecx, 3; ja` devuelve nullptr
    }
}
