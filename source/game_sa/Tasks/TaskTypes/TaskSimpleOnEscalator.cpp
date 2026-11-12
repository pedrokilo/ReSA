/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleOnEscalator
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleOnEscalator.h"
#include "ModelIndices.h"
#include "General.h"

namespace {
constexpr uint32 kOnEscalatorVTable = 0x85B8DC;

// 0x4B8750 (lote 171A, banco Fn171A; thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],0x85B8DC; jmp 0x62F360`): cuerpo no virtual de ~CTaskSimpleOnEscalator del EXE. Deja la vtable de
// la clase del EXE y salta al cuerpo de ~CTaskSimpleStandStill (0x62F360, con hook), que a su vez deja la de CTask. Funcion propia sin expansion en linea para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall OnEscalatorDestructorBody(CTaskSimpleOnEscalator* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = kOnEscalatorVTable;
    CTaskSimpleStandStillBody::StandStillDestructorBody(self, nullptr);
}

// 0x4B8730 (lote 171A): destructor con borrado de CTaskSimpleOnEscalator del EXE (ranura 0 de su vtable 0x85B8DC; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x4B8750; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this`
// con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleOnEscalator* __fastcall OnEscalatorScalarDeletingDestructor(CTaskSimpleOnEscalator* self, void* /*edx*/, uint8 flags) {
    OnEscalatorDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote171ABody_CTaskSimpleOnEscalator() { return reinterpret_cast<uintptr_t>(&OnEscalatorDestructorBody); } // para el banco Fn171A

void CTaskSimpleOnEscalator::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleOnEscalator, kOnEscalatorVTable, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(OnEscalatorScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B8730); // lote 171A (banco Fn171A)
    RH_ScopedNamedGlobalInstall(OnEscalatorDestructorBody, "DestructorBody", 0x4B8750);                      // lote 171A

    RH_ScopedVMTInstall(Clone, 0x4B86C0);
    RH_ScopedVMTInstall(GetTaskType, 0x4B8720);
    RH_ScopedVMTInstall(ProcessPed, 0x62F520);
}

// 0x4B8720 (lote 171A, banco Fn171A; thiscall, `ret`, 6 bytes: `mov eax,51Ch`): TASK_SIMPLE_ON_ESCALATOR (1308); no lee el objeto (ranura 4 de la vtable 0x85B8DC).
eTaskType CTaskSimpleOnEscalator::GetTaskType() const {
    static_assert(Type == 0x51C);
    return Type;
}

// 0x4B86C0 (lote 171A; 89 bytes con marco SEH que solo desenrolla): CTask::operator new(0x20) (0x61A5A0); si no es nulo, el constructor de CTaskSimpleStandStill (0x62F310) con (0, true, false, 8.0f) y
// la vtable del EXE de esta clase (0x85B8DC) encima de la que deja el constructor; devuelve la memoria (no el EAX del constructor) o nulo si no hubo memoria.
CTask* CTaskSimpleOnEscalator::Clone() const {
    void* const memory = CTask::operator new(sizeof(CTaskSimpleOnEscalator));
    if (!memory) {
        return nullptr;
    }
    static_cast<CTaskSimpleStandStill*>(memory)->Constructor(0, true, false, 8.0f);
    *reinterpret_cast<uint32*>(memory) = kOnEscalatorVTable;
    return static_cast<CTask*>(memory);
}

// 0x62F520 (lote 171A; thiscall, un puntero a ped en la pila, `ret 4`, 99 bytes): ejecuta la parte de CTaskSimpleStandStill (0x62F370, con hook; su resultado se ignora). Si el peaton pisa un modelo de
// escalon (el indice de modelo del contacto, con signo, igual a alguno de los dos modelos de escalon), apunta el peaton en la direccion de la velocidad del contacto (angulo entre (vx, vy) y (0, 0),
// 0x53CBE0, limitado a [-pi, pi] por 0x53CB50) y devuelve false (la tarea sigue); si no, devuelve true.
bool CTaskSimpleOnEscalator::ProcessPed(CPed* ped) {
    CTaskSimpleStandStill::ProcessPed(ped);
    if (CEntity* const contact = ped->m_pContactEntity) {
        const int32 model = static_cast<int16>(contact->m_nModelIndex);
        if (model == static_cast<int32>(static_cast<eModelID>(ModelIndices::MI_ESCALATORSTEP)) || model == static_cast<int32>(static_cast<eModelID>(ModelIndices::MI_ESCALATORSTEP8))) {
            const CVector& speed = static_cast<CPhysical*>(contact)->m_vecMoveSpeed;
            ped->m_fAimingRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(speed.x, speed.y, 0.0f, 0.0f));
            return false;
        }
    }
    return true;
}
