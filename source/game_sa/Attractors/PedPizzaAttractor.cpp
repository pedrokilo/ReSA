#include "StdInc.h"

#include "PedPizzaAttractor.h"

namespace {
// Lote 220A (sin validar): 0x5EE6E0 (85 bytes, thiscall con tres argumentos de pila (efecto, entidad, estado de movimiento), `ret 0Ch`, devuelve `this`): constructor de CPedPizzaAttractor. Llama al constructor de CPedAttractor (0x5EDFB0, con hook)
// con (efecto, entidad, estado, 5 peatones, espaciado 1, 30000, 3000, 0.2, 0.1, 0.1, 0.1 (0x86C44C..0x86C464)), leidos del EXE como constantes, y despues escribe su vtable (0x86C598). Aqui se construye con el constructor de la clase (misma lista de argumentos, comprobada contra los siete valores
// del EXE), que deja la vtable de la DLL. El marco SEH no existe en esta funcion.
CPedPizzaAttractor* __fastcall PedPizzaAttractorConstructor(CPedPizzaAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) {
    return std::construct_at(self, fx, entity, moveState);
}

// 0x5EE7B0 (11 bytes, thiscall sin argumentos): cuerpo del destructor de CPedPizzaAttractor: escribe su vtable (0x86C598) y salta al cuerpo del destructor de CPedAttractor (0x5EC410, con hook). Devuelve `this` en EAX (el valor que deja el salto).
CPedPizzaAttractor* __fastcall PedPizzaAttractorDestructorBody(CPedPizzaAttractor* self, void* /*edx*/) {
    self->CPedPizzaAttractor::~CPedPizzaAttractor();
    return self;
}

// 0x5EE760 (76 bytes, thiscall con el BYTE de banderas en la pila, `ret 4`): destructor con borrado de CPedPizzaAttractor (ranura 1 de su vtable 0x86C598). Llama al cuerpo del destructor (0x5EE7B0) y, solo si el bit 0 del BYTE de banderas esta puesto, libera
// `this` en el pool de atractores (0xB744C4) EN LINEA como CPedAttractor::operator delete (0x5EAFE0, con hook). Devuelve `this`.
CPedPizzaAttractor* __fastcall PedPizzaAttractorScalarDeletingDestructor(CPedPizzaAttractor* self, void* /*edx*/, uint8 flags) {
    self->CPedPizzaAttractor::~CPedPizzaAttractor();
    if (flags & 1) {
        CPedAttractor::operator delete(self);
    }
    return self;
}
} // namespace

void CPedPizzaAttractor::InjectHooks() {
    RH_ScopedClass(CPedPizzaAttractor);
    RH_ScopedCategory("Attractors");

    // Lote 220A (sin validar)
    RH_ScopedNamedGlobalInstall(PedPizzaAttractorConstructor, "Constructor", 0x5EE6E0);
    RH_ScopedNamedGlobalInstall(PedPizzaAttractorScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5EE760);
    RH_ScopedNamedGlobalInstall(PedPizzaAttractorDestructorBody, "DestructorBody", 0x5EE7B0);
}
