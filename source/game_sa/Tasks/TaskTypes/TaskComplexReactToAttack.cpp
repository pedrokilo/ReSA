/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexReactToAttack
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexReactToAttack.h"
#include "TaskComplexFallAndGetUp.h"
#include "TaskSimpleBeHit.h"
#include "TaskSimpleNone.h"
#include "Pad.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

namespace {
static_assert(TASK_COMPLEX_REACT_TO_ATTACK == 0x3EE); // 0x620B80: `mov eax, 0x3EE; ret`
static_assert(TASK_NONE == 0xC8);
static_assert(TASK_COMPLEX_FALL_AND_GET_UP == 0xD0);
static_assert(TASK_SIMPLE_BE_HIT == 0x3F0);
static_assert(TASK_FINISHED == 0x516);
static_assert(offsetof(CPed, m_fHealth) == 0x540);

// La clase es una plantilla sin campos: se leen por desplazamiento (+0xC byte; +0xD byte de "terminada"; +0x10 tipo de arma (int32); +0x14 CEntity* agresor con referencia registrada; +0x1C e +0x20 estados (int32)).
template<typename T>
T& FieldAt(void* object, uint32 offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<uint8*>(object) + offset);
}

// 0x620C30 (lote 199A, sin validar; el listado decia CTaskSimpleBeHit; antes llamada al EXE por direccion fija): CTaskComplexReactToAttack::CreateSubTask(eTaskType), thiscall, un argumento de pila, `ret 4`, 451 bytes con marco SEH que
// solo libera la memoria si el constructor lanza (se omite). Cada rama reserva con CTask::operator new (0x61A5A0, con hook) ANTES de leer los campos y, sin memoria (nulo), devuelve nulo sin construir. Tipos (comparacion con signo):
//  - mayor que 0x3F0: nulo.
//  - TASK_SIMPLE_BE_HIT (0x3F0): 0x28 bytes, CTaskSimpleBeHit (ctor 0x620780, con hook) con (agresor +0x14 si no es nulo y su tipo de entidad (bits 0-2 del byte +0x36) es 3 (peaton), si no 0; +0x20; +0x1C;
//    (int)(float)+0x18 truncado con _ftol). Las dos ramas llaman al mismo constructor; solo cambia el primer argumento.
//  - TASK_NONE (0xC8): 8 bytes, CTaskSimpleNone (ctor de CTaskSimple 0x61A390 y vtable 0x859CFC).
//  - TASK_COMPLEX_FALL_AND_GET_UP (0xD0): 0x18 bytes, CTaskComplexFallAndGetUp (ctor 0x6786C0, con hook) con (animacion 0x1A si el estado +0x1C es 2 y 0x14 si no, grupo 0, tiempo 1000 (0x3E8)).
//  - cualquier otro: nulo (no escribe ningun campo).
CTask* __fastcall ReactToAttackCreateSubTask(CTaskComplexReactToAttack* self, void* /*edx*/, eTaskType type) {
    switch (type) {
    case TASK_SIMPLE_BE_HIT: {
        CEntity* const attacker = FieldAt<CEntity*>(self, 0x14);
        CPed* const attackerPed = (attacker && attacker->GetType() == ENTITY_TYPE_PED) ? static_cast<CPed*>(attacker) : nullptr;
        void* const memory = CTask::operator new(sizeof(CTaskSimpleBeHit));
        if (!memory) {
            return nullptr;
        }
        return static_cast<CTaskSimpleBeHit*>(memory)->Constructor(attackerPed, static_cast<ePedPieceTypes>(FieldAt<int32>(self, 0x20)), FieldAt<int32>(self, 0x1C), static_cast<int32>(static_cast<int64>(FieldAt<float>(self, 0x18))));   // _ftol del EXE (64 bits; se pasan los 32 bits bajos)
    }
    case TASK_NONE: {
        void* const memory = CTask::operator new(sizeof(CTaskSimpleNone));
        if (!memory) {
            return nullptr;
        }
        CTaskSimpleBody::Constructor(memory, nullptr);                         // 0x61A390, con hook: deja la vtable de CTaskSimple del EXE
        *reinterpret_cast<uint32*>(memory) = 0x859CFC;                          // vtable de CTaskSimpleNone del EXE (como el original)
        return static_cast<CTaskSimpleNone*>(memory);
    }
    case TASK_COMPLEX_FALL_AND_GET_UP: {
        const AnimationId animId = FieldAt<int32>(self, 0x1C) == 2 ? static_cast<AnimationId>(0x1A) : static_cast<AnimationId>(0x14);
        void* const memory = CTask::operator new(sizeof(CTaskComplexFallAndGetUp));
        if (!memory) {
            return nullptr;
        }
        return static_cast<CTaskComplexFallAndGetUp*>(memory)->Constructor(animId, ANIM_GROUP_DEFAULT, 1000);
    }
    default:
        return nullptr;
    }
}

// 0x620BF0 (lote 199A, sin validar; el listado decia CTaskSimpleBeHit; ranura 6 (MakeAbortable) de la vtable 0x86D868; thiscall, tres argumentos de pila (ped, prioridad, evento), `ret 0Ch`, 64 bytes). Si el byte +0xD (terminada) es distinto de
// cero devuelve true sin mirar nada mas. Si no, con una prioridad distinta de 1 (URGENT) y de 2 (IMMEDIATE) devuelve false sin tocar el byte. Si no, llama a MakeAbortable (ranura +0x18) de la subtarea (+8, sin comprobar nulo) con (ped, prioridad,
// evento), guarda el resultado en el byte +0xD y lo devuelve (AL).
bool __fastcall ReactToAttackMakeAbortable(CTaskComplexReactToAttack* self, void* /*edx*/, CPed* ped, int32 priority, const CEvent* event) {
    if (FieldAt<uint8>(self, 0xD)) {
        return true;
    }
    if (priority != 1 && priority != 2) {
        return false;
    }
    const bool aborted = FieldAt<CTask*>(self, 0x8)->MakeAbortable(ped, static_cast<eAbortPriority>(priority), event);
    FieldAt<uint8>(self, 0xD) = aborted;
    return aborted;
}

// 0x620E00 (lote 199A, sin validar; el listado decia CTaskSimpleBeHit; ranura 10 (ControlSubTask) de la vtable 0x86D868; thiscall, un argumento de pila (el ped), `ret 4`, 41 bytes). Si el byte +0xC es distinto de cero devuelve CreateFirstSubTask (ranura +0x24,
// llamada virtual) con el ped. Si no, llama a GetTaskType (ranura +0x10) de la subtarea (+8, sin comprobar nulo) IGNORANDO el resultado y devuelve la subtarea.
CTask* __fastcall ReactToAttackControlSubTask(CTaskComplexReactToAttack* self, void* /*edx*/, CPed* ped) {
    if (FieldAt<uint8>(self, 0xC)) {
        return self->CreateFirstSubTask(ped);
    }
    FieldAt<CTask*>(self, 0x8)->GetTaskType(); // llamada virtual cuyo resultado el EXE descarta
    return FieldAt<CTask*>(self, 0x8);         // el EXE relee +8 tras la llamada
}


// 0x623300 (lote 199A, sin validar; el listado decia CTaskSimpleBeHit; ranura 1 (Clone) de la vtable 0x86D868; thiscall, sin argumentos, `ret`, 112 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite)).
// Como el original: CTask::operator new (0x61A5A0, con hook) con 0x24 bytes; sin memoria devuelve nulo sin leer campos; si no, el constructor 0x620B00 (ReactToAttackConstructor) con cinco argumentos de pila tomados de +0x10, +0x14, +0x18, +0x1C y +0x20
// (en ese orden: tipo de arma, agresor, tercer, cuarto y quinto argumentos) y devuelve la tarea nueva.
CTask* __fastcall ReactToAttackClone(CTaskComplexReactToAttack* self, void* /*edx*/) {
    void* const memory = CTask::operator new(0x24);
    if (!memory) {
        return nullptr;
    }
    // 0x620B00 (lote 198A, sin validar; sin hook en esta rama): constructor del EXE, llamado por su direccion como hacia el EXE
    return plugin::CallMethodAndReturn<CTaskComplexReactToAttack*, 0x620B00, CTaskComplexReactToAttack*, int32, CEntity*, int32, int32, int32>(
        static_cast<CTaskComplexReactToAttack*>(memory), FieldAt<int32>(self, 0x10), FieldAt<CEntity*>(self, 0x14), FieldAt<int32>(self, 0x18), FieldAt<int32>(self, 0x1C), FieldAt<int32>(self, 0x20));
}
} // namespace

void CTaskComplexReactToAttack::InjectHooks() {
    RH_ScopedClass(CTaskComplexReactToAttack); // clase aun sin reconstruir: sin vtable exportada, solo hooks de funciones sueltas
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(ReactToAttackMakeAbortable, "MakeAbortable", 0x620BF0); // lote 199A (sin validar)
    RH_ScopedNamedGlobalInstall(ReactToAttackCreateSubTask, "CreateSubTask", 0x620C30); // lote 199A (sin validar)
    RH_ScopedNamedGlobalInstall(ReactToAttackControlSubTask, "ControlSubTask", 0x620E00); // lote 199A (sin validar)
    RH_ScopedNamedGlobalInstall(ReactToAttackClone, "Clone", 0x623300); // lote 199A (sin validar)
    // TODO: instalar las funciones conforme se reconstruyan.
}
