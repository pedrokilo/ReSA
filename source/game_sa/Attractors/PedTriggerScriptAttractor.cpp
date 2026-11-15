#include "StdInc.h"

#include "PedTriggerScriptAttractor.h"

namespace {
// Lote 220A (sin validar): 0x5EE890 (85 bytes, thiscall con tres argumentos de pila (efecto, entidad, estado de movimiento), `ret 0Ch`, devuelve `this`): constructor de CPedTriggerScriptAttractor. Llama al constructor de CPedAttractor (0x5EDFB0, con hook)
// con (efecto, entidad, estado, 1 peaton, espaciado 1, 30000, 3000, 0.125, 0.1, 0.1, 0.1 (0x86C490..0x86C4A8)), leidos del EXE como constantes, y despues escribe su vtable (0x86C5CC). Aqui se construye con el constructor de la clase (misma lista de argumentos, comprobada contra los siete valores
// del EXE), que deja la vtable de la DLL. El marco SEH no existe en esta funcion.
CPedTriggerScriptAttractor* __fastcall PedTriggerScriptAttractorConstructor(CPedTriggerScriptAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) {
    return std::construct_at(self, fx, entity, moveState);
}

// 0x5EE950 (11 bytes, thiscall sin argumentos): cuerpo del destructor de CPedTriggerScriptAttractor: escribe su vtable (0x86C5CC) y salta al cuerpo del destructor de CPedAttractor (0x5EC410, con hook). Devuelve `this` en EAX (el valor que deja el salto).
CPedTriggerScriptAttractor* __fastcall PedTriggerScriptAttractorDestructorBody(CPedTriggerScriptAttractor* self, void* /*edx*/) {
    self->CPedTriggerScriptAttractor::~CPedTriggerScriptAttractor();
    return self;
}

// 0x5EE900 (76 bytes, thiscall con el BYTE de banderas en la pila, `ret 4`): destructor con borrado de CPedTriggerScriptAttractor (ranura 1 de su vtable 0x86C5CC). Llama al cuerpo del destructor (0x5EE950) y, solo si el bit 0 del BYTE de banderas esta puesto, libera
// `this` en el pool de atractores (0xB744C4) EN LINEA como CPedAttractor::operator delete (0x5EAFE0, con hook). Devuelve `this`.
CPedTriggerScriptAttractor* __fastcall PedTriggerScriptAttractorScalarDeletingDestructor(CPedTriggerScriptAttractor* self, void* /*edx*/, uint8 flags) {
    self->CPedTriggerScriptAttractor::~CPedTriggerScriptAttractor();
    if (flags & 1) {
        CPedAttractor::operator delete(self);
    }
    return self;
}
} // namespace

void CPedTriggerScriptAttractor::InjectHooks() {
    RH_ScopedClass(CPedTriggerScriptAttractor);
    RH_ScopedCategory("Attractors");

    // Lote 220A (sin validar)
    RH_ScopedNamedGlobalInstall(PedTriggerScriptAttractorConstructor, "Constructor", 0x5EE890);
    RH_ScopedNamedGlobalInstall(PedTriggerScriptAttractorScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5EE900);
    RH_ScopedNamedGlobalInstall(PedTriggerScriptAttractorDestructorBody, "DestructorBody", 0x5EE950);
}
