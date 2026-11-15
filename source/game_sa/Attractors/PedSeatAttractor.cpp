#include "StdInc.h"

#include "PedSeatAttractor.h"

namespace {
// Lote 220A (sin validar): 0x5EE540 (85 bytes, thiscall con tres argumentos de pila (efecto, entidad, estado de movimiento), `ret 0Ch`, devuelve `this`): constructor de CPedSeatAttractor. Llama al constructor de CPedAttractor (0x5EDFB0, con hook)
// con (efecto, entidad, estado, 1 peaton, espaciado 1, 30000, 3000, 0.125, 0.1, 0.1, 0.1 (0x86C40C..0x86C424)), leidos del EXE como constantes, y despues escribe su vtable (0x86C568). Aqui se construye con el constructor de la clase (misma lista de argumentos, comprobada contra los siete valores
// del EXE), que deja la vtable de la DLL. El marco SEH no existe en esta funcion.
CPedSeatAttractor* __fastcall PedSeatAttractorConstructor(CPedSeatAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) {
    return std::construct_at(self, fx, entity, moveState);
}

// 0x5EE600 (11 bytes, thiscall sin argumentos): cuerpo del destructor de CPedSeatAttractor: escribe su vtable (0x86C568) y salta al cuerpo del destructor de CPedAttractor (0x5EC410, con hook). Devuelve `this` en EAX (el valor que deja el salto).
CPedSeatAttractor* __fastcall PedSeatAttractorDestructorBody(CPedSeatAttractor* self, void* /*edx*/) {
    self->CPedSeatAttractor::~CPedSeatAttractor();
    return self;
}

// 0x5EE5B0 (76 bytes, thiscall con el BYTE de banderas en la pila, `ret 4`): destructor con borrado de CPedSeatAttractor (ranura 1 de su vtable 0x86C568). Llama al cuerpo del destructor (0x5EE600) y, solo si el bit 0 del BYTE de banderas esta puesto, libera
// `this` en el pool de atractores (0xB744C4) EN LINEA como CPedAttractor::operator delete (0x5EAFE0, con hook). Devuelve `this`.
CPedSeatAttractor* __fastcall PedSeatAttractorScalarDeletingDestructor(CPedSeatAttractor* self, void* /*edx*/, uint8 flags) {
    self->CPedSeatAttractor::~CPedSeatAttractor();
    if (flags & 1) {
        CPedAttractor::operator delete(self);
    }
    return self;
}
} // namespace

void CPedSeatAttractor::InjectHooks() {
    RH_ScopedClass(CPedSeatAttractor);
    RH_ScopedCategory("Attractors");

    // Lote 220A (sin validar)
    RH_ScopedNamedGlobalInstall(PedSeatAttractorConstructor, "Constructor", 0x5EE540);
    RH_ScopedNamedGlobalInstall(PedSeatAttractorScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5EE5B0);
    RH_ScopedNamedGlobalInstall(PedSeatAttractorDestructorBody, "DestructorBody", 0x5EE600);
}
