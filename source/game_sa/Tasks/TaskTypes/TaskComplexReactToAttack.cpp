/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexReactToAttack
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

// Sin PCH (CMakeLists.txt): StdInc.h redefine NOMINMAX y el aviso C4005 es error; mismo recurso que Fx.cpp.
#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)

#include "TaskComplexReactToAttack.h"
#include "TaskComplexFallAndGetUp.h"
#include "TaskSimpleBeHit.h"
#include "TaskSimpleNone.h"
#include "Pad.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

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

// 0x620B90 (lote 193A, sin validar): cuerpo del destructor de CTaskComplexReactToAttack del EXE (thiscall, sin argumentos de pila, `ret`, con marco SEH que solo desenrolla y aqui se omite; ranura 0 de la vtable 0x86D868). Deja la vtable
// de la clase del EXE (0x86D868); si el agresor (+0x14) no es nulo, CEntity::CleanUpOldReference (0x571A00) con la direccion de +0x14 (sin poner el puntero a cero); y sigue con ~CTaskComplex (0x61A3D0, con hook).
__declspec(noinline) void __fastcall ReactToAttackDestructorBody(CTaskComplexReactToAttack* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86D868;
    CEntity::SafeCleanUpRef(FieldAt<CEntity*>(self, 0x14));
    CTaskComplexBody::Destructor(self, nullptr);
}

// 0x620B00 (lote 198A, sin validar; el listado decia CTaskSimpleBeHit: es el constructor de CTaskComplexReactToAttack, 0x24 bytes, que llama el Clone 0x623300; thiscall, cinco argumentos de pila, `ret 14h`, 121 bytes con SEH que solo desenrolla y
// aqui se omite). Como el original: ~CTaskComplex (0x61A3B0, con hook: vtable de la base y subtarea nula); +0x18 = arg3, +0x10 = arg1 (tipo de arma), +0x1C = arg4, vtable de la clase del EXE (0x86D868), bytes +0xC y +0xD a cero, +0x14 = arg2 (agresor),
// +0x20 = arg5; y SOLO si el agresor no es nulo, CEntity::RegisterReference (0x571B70, con hook) con ECX = el agresor y la direccion de +0x14. Devuelve `this`. Los significados de arg3, arg4 y arg5 se deducen de CreateFirstSubTask (arg5 = estado +0x20 que se
// compara con 3; arg4 = +0x1C); se llevan tal cual. La clase no tiene campos declarados: las escrituras son por desplazamiento.
CTaskComplexReactToAttack* __fastcall ReactToAttackConstructor(CTaskComplexReactToAttack* self, void* /*edx*/, int32 weaponType, CEntity* attacker, int32 arg3, int32 arg4, int32 arg5) {
    CTaskComplexBody::Constructor(self, nullptr);
    FieldAt<int32>(self, 0x18) = arg3;
    FieldAt<int32>(self, 0x10) = weaponType;
    FieldAt<int32>(self, 0x1C) = arg4;
    *reinterpret_cast<uint32*>(self) = 0x86D868;
    FieldAt<uint8>(self, 0xC) = 0;
    FieldAt<uint8>(self, 0xD) = 0;
    FieldAt<CEntity*>(self, 0x14) = attacker;
    FieldAt<int32>(self, 0x20) = arg5;
    CEntity::SafeRegisterRef(FieldAt<CEntity*>(self, 0x14));
    return self;
}

// 0x620B80 (lote 198A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,0x3EE`): TASK_COMPLEX_REACT_TO_ATTACK (1006); no lee el objeto (ranura 4 de la vtable 0x86D868). Se engancha como S: la clase es abstracta aqui y no tiene vtable propia que buscar.
eTaskType __fastcall ReactToAttackGetTaskType(CTaskComplexReactToAttack* /*self*/, void* /*edx*/) {
    return TASK_COMPLEX_REACT_TO_ATTACK;
}

// 0x625C00 (lote 193A, sin validar; el listado decia CTaskComplexReactToAttack: es la ranura 0 de la vtable 0x86D868; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x620B90; test byte [esp+8],1; je ..;
// push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo (0x620B90, ReactToAttackDestructorBody), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0
// del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (clase sin reconstruir: sin vtable exportada).
CTaskComplexReactToAttack* __fastcall ReactToAttackScalarDeletingDestructor(CTaskComplexReactToAttack* self, void* /*edx*/, uint8 flags) {
    ReactToAttackDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}

// 0x625C20 (lote 193A, sin validar; ranura 8 (CreateNextSubTask) de la vtable 0x86D868; thiscall, un argumento de pila (el ped, que no se lee), `ret 4`, 68 bytes): si el byte +0xD es distinto de cero devuelve nulo sin mirar la subtarea.
// Si no, pide el tipo de la subtarea (llamada virtual, ranura 4, sin comprobar si es nula); con TASK_NONE (0xC8), TASK_COMPLEX_FALL_AND_GET_UP (0xD0) o TASK_SIMPLE_BE_HIT (0x3F0) devuelve CreateSubTask(TASK_FINISHED = 0x516)
// (0x620C30, sin reconstruir: llamada al EXE por direccion fija); con otro tipo, nulo. No escribe ningun campo.
CTask* __fastcall ReactToAttackCreateNextSubTask(CTaskComplexReactToAttack* self, void* /*edx*/, CPed* /*ped*/) {
    if (FieldAt<uint8>(self, 0xD)) {
        return nullptr;
    }
    const eTaskType subTaskType = self->GetSubTask()->GetTaskType();
    if (subTaskType == TASK_NONE || subTaskType == TASK_COMPLEX_FALL_AND_GET_UP || subTaskType == TASK_SIMPLE_BE_HIT) {
        return ReactToAttackCreateSubTask(self, nullptr, TASK_FINISHED);
    }
    return nullptr;
}

// 0x625C70 (lote 193A, sin validar; ranura 9 (CreateFirstSubTask) de la vtable 0x86D868; thiscall, un argumento de pila (el ped), `ret 4`, 429 bytes): pone a cero el byte +0xC. Si el ped es el jugador (CPed::IsPlayer 0x5DF8F0) y el tipo de
// arma (+0x10, int32 con signo) es mayor que 9: con el pad 0 sin controles desactivados (palabra +0x10E, DisablePlayerControls), si CPad::GetTarget (0x540670) o alguno de los ejes/cruceta (palabras +0, +2, +0x10, +0x12, +0x14, +0x16
// del estado nuevo) es distinto de cero, devuelve CreateSubTask(TASK_NONE = 0xC8). Despues, si el estado (+0x20) es 3: con la bandera 0x4000000 de la palabra +0x470 del ped y arma 0x1B o 0x19 (o 0x18 si el ped no es el jugador), calcula el
// vector 2D (agresor - ped) con las posiciones de ambos (matriz +0x14 o posicion propia), lo normaliza con CVector2D::Normalise del EXE (0x44E480, x87; la de la DLL difiere con NaN), apaga el bit 0 de la palabra +0x46C del ped
// (bIsStanding), le aplica CPhysical::ApplyMoveForce (0x5429F0, con hook) de (-5 * x, -5 * y, 5) y devuelve CreateSubTask(TASK_COMPLEX_FALL_AND_GET_UP = 0xD0); si no (o con arma menor que 9 y salud menor que 30.0f en comparacion
// ordenada) devuelve CreateSubTask(0xD0); en cualquier otro caso CreateSubTask(TASK_SIMPLE_BE_HIT = 0x3F0). x87 a 24 bits: restas y productos redondeados a float. El agresor no se comprueba contra nulo (igual que el EXE).
CTask* __fastcall ReactToAttackCreateFirstSubTask(CTaskComplexReactToAttack* self, void* /*edx*/, CPed* ped) {
    FieldAt<uint8>(self, 0xC) = 0;

    if (ped->IsPlayer() && FieldAt<int32>(self, 0x10) > 9) {
        CPad* const pad = CPad::GetPad(0); // 0x53FB70
        if (pad->DisablePlayerControls == 0) {
            const CControllerState& state = pad->NewState;
            if (pad->GetTarget()
                || state.LeftStickX != 0 || state.LeftStickY != 0
                || state.DPadUp != 0 || state.DPadDown != 0 || state.DPadLeft != 0 || state.DPadRight != 0) {
                return ReactToAttackCreateSubTask(self, nullptr, TASK_NONE);
            }
        }
    }

    if (FieldAt<int32>(self, 0x20) == 3) {
        const int32 weapon = FieldAt<int32>(self, 0x10);
        // Bit 26 de la segunda palabra de banderas del ped (+0x470; bCanBeShotInVehicle segun Ped.h)
        const bool flagSet = (FieldAt<uint32>(ped, 0x470) & 0x4000000u) != 0;
        if (flagSet && (weapon == 0x1B || weapon == 0x19 || (weapon == 0x18 && !ped->IsPlayer()))) {
            CEntity* const attacker = FieldAt<CEntity*>(self, 0x14);
            CVector2D direction;
            direction.x = attacker->GetPosition().x - ped->GetPosition().x;
            direction.y = attacker->GetPosition().y - ped->GetPosition().y;
            plugin::CallMethod<0x44E480, CVector2D*>(&direction); // CVector2D::Normalise del EXE
            const uint32 clearedFlags = FieldAt<uint32>(ped, 0x46C) & 0xFFFFFFFEu;
            const float forceX = direction.x * -5.0f;
            const float forceY = direction.y * -5.0f;
            const CVector force{ forceX, forceY, 5.0f };
            FieldAt<uint32>(ped, 0x46C) = clearedFlags;
            ped->ApplyMoveForce(force);
            return ReactToAttackCreateSubTask(self, nullptr, TASK_COMPLEX_FALL_AND_GET_UP);
        }
        if (weapon < 9 && ped->m_fHealth < 30.0f) {
            return ReactToAttackCreateSubTask(self, nullptr, TASK_COMPLEX_FALL_AND_GET_UP);
        }
    }
    return ReactToAttackCreateSubTask(self, nullptr, TASK_SIMPLE_BE_HIT);
}
} // namespace

void CTaskComplexReactToAttack::InjectHooks() {
    RH_ScopedClass(CTaskComplexReactToAttack); // clase aun sin reconstruir: sin vtable exportada, solo hooks de funciones sueltas
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(ReactToAttackMakeAbortable, "MakeAbortable", 0x620BF0); // lote 199A (sin validar)
    RH_ScopedNamedGlobalInstall(ReactToAttackCreateSubTask, "CreateSubTask", 0x620C30); // lote 199A (sin validar)
    RH_ScopedNamedGlobalInstall(ReactToAttackControlSubTask, "ControlSubTask", 0x620E00); // lote 199A (sin validar)
    RH_ScopedNamedGlobalInstall(ReactToAttackClone, "Clone", 0x623300); // lote 199A (sin validar)
    RH_ScopedNamedGlobalInstall(ReactToAttackDestructorBody, "DestructorBody", 0x620B90); // lote 193A (sin validar)
    RH_ScopedNamedGlobalInstall(ReactToAttackScalarDeletingDestructor, "ScalarDeletingDestructor", 0x625C00); // lote 193A (sin validar)
    RH_ScopedNamedGlobalInstall(ReactToAttackCreateNextSubTask, "CreateNextSubTask", 0x625C20); // lote 193A (sin validar)
    RH_ScopedNamedGlobalInstall(ReactToAttackCreateFirstSubTask, "CreateFirstSubTask", 0x625C70); // lote 193A (sin validar)
    RH_ScopedNamedGlobalInstall(ReactToAttackConstructor, "Constructor", 0x620B00); // lote 198A (sin validar)
    RH_ScopedNamedGlobalInstall(ReactToAttackGetTaskType, "GetTaskType", 0x620B80); // lote 198A (sin validar)
    // TODO: instalar las funciones conforme se reconstruyan.
}
