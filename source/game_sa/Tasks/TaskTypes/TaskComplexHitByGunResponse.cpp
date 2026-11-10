/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexHitByGunResponse
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

#include "TaskComplexHitByGunResponse.h"
#include "TaskSimpleHitByGunFromFront.h"
#include "TaskSimpleHitByGunFromLeft.h"
#include "TaskSimpleHitByGunFromRear.h"
#include "TaskSimpleHitByGunFromRight.h"

namespace {
// 0x631E30 (lote 145A, banco Fn145A): cuerpo del destructor de CTaskComplexHitByGunResponse del EXE (con hook propio (Destructor); vtable 0x86E06C). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama
// el destructor con borrado como el `call 0x631E30` del EXE.
__declspec(noinline) void __fastcall CTaskComplexHitByGunResponseDestructorBody(CTaskComplexHitByGunResponse* self, void* /*edx*/) { self->CTaskComplexHitByGunResponse::~CTaskComplexHitByGunResponse(); }

// 0x638160 (lote 145A, banco Fn145A): destructor con borrado de CTaskComplexHitByGunResponse del EXE (ranura 0 de su vtable 0x86E06C; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x631E30; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexHitByGunResponse* __fastcall CTaskComplexHitByGunResponseScalarDeletingDestructor(CTaskComplexHitByGunResponse* self, void* /*edx*/, uint8 flags) {
    CTaskComplexHitByGunResponseDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote145ABody_CTaskComplexHitByGunResponse() { return reinterpret_cast<uintptr_t>(&CTaskComplexHitByGunResponseDestructorBody); } // para el banco Fn145A

void CTaskComplexHitByGunResponse::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexHitByGunResponse, 0x86e06c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexHitByGunResponseScalarDeletingDestructor, "ScalarDeletingDestructor", 0x638160); // lote 145A (banco Fn145A)

    RH_ScopedInstall(Constructor, 0x631DD0);
    RH_ScopedInstall(Destructor, 0x631E30);

    RH_ScopedVMTInstall(Clone, 0x636640);
    RH_ScopedVMTInstall(GetTaskType, 0x631DF0);
    RH_ScopedVMTInstall(MakeAbortable, 0x631E00);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x631E40);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x631E50);
    RH_ScopedVMTInstall(ControlSubTask, 0x631F70);
}

// 0x631DD0
CTaskComplexHitByGunResponse::CTaskComplexHitByGunResponse(eDirection hs) :
    m_HitSide{hs}
{
}

// 0x636640 (Clone): como el original, copia el dword completo del lado
CTaskComplexHitByGunResponse::CTaskComplexHitByGunResponse(const CTaskComplexHitByGunResponse& o) :
    CTaskComplex{},
    m_HitSide{ o.m_HitSide }
{
}

// 0x631E00
bool CTaskComplexHitByGunResponse::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    // Como el original: solo se aborta con prioridad urgente o inmediata
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE:
        return m_pSubTask->MakeAbortable(ped, priority, event);
    default:
        return false;
    }
}

// 0x631E50
CTask* CTaskComplexHitByGunResponse::CreateFirstSubTask(CPed* ped) {
    // Como el original: compara el dword completo sin signo (fuera de 0..3 devuelve nullptr)
    switch (static_cast<uint32>(m_HitSide.get_underlying())) {
    case 0:  return new CTaskSimpleHitByGunFromFront{};
    case 1:  return new CTaskSimpleHitByGunFromLeft{};
    case 2:  return new CTaskSimpleHitByGunFromRear{};
    case 3:  return new CTaskSimpleHitByGunFromRight{};
    default: return nullptr;
    }
}
