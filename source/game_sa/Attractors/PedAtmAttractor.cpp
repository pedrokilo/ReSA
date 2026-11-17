/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedATMAttractor
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedAtmAttractor.h"

namespace {
// Lote 220A (sin validar): 0x5EE470 (85 bytes, thiscall con tres argumentos de pila (efecto, entidad, estado de movimiento), `ret 0Ch`, devuelve `this`): constructor de CPedATMAttractor. Llama al constructor de CPedAttractor (0x5EDFB0, con hook)
// con (efecto, entidad, estado, 5 peatones, espaciado 1, 30000, 3000, 0.2, 0.15, 0.1, 0.1 (0x86C3EC..0x86C404)), leidos del EXE como constantes, y despues escribe su vtable (0x86C550). Aqui se construye con el constructor de la clase (misma lista de argumentos, comprobada contra los siete valores
// del EXE), que deja la vtable de la DLL. El marco SEH no existe en esta funcion.
CPedATMAttractor* __fastcall PedATMAttractorConstructor(CPedATMAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) {
    return std::construct_at(self, fx, entity, moveState);
}

// 0x5EE530 (11 bytes, thiscall sin argumentos): cuerpo del destructor de CPedATMAttractor: escribe su vtable (0x86C550) y salta al cuerpo del destructor de CPedAttractor (0x5EC410, con hook). Devuelve `this` en EAX (el valor que deja el salto).
CPedATMAttractor* __fastcall PedATMAttractorDestructorBody(CPedATMAttractor* self, void* /*edx*/) {
    // Como el EXE: escribe la vtable del EXE de la clase (0x86C550) y salta al cuerpo del destructor de CPedAttractor (0x5EC410); la llamada es a CPedAttractor::Destructor (con hook), no al destructor de C++
    // de la clase (que expandiria en linea la destruccion de los miembros sin pasar por 0x5EC410). La vtable del EXE en un objeto de la DLL solo dura hasta el final del destructor.
    *reinterpret_cast<uint32*>(self) = 0x86C550;
    self->Destructor();
    return self;
}

// 0x5EE4E0 (76 bytes, thiscall con el BYTE de banderas en la pila, `ret 4`): destructor con borrado de CPedATMAttractor (ranura 1 de su vtable 0x86C550). Llama al cuerpo del destructor (0x5EE530) y, solo si el bit 0 del BYTE de banderas esta puesto, libera
// `this` en el pool de atractores (0xB744C4) EN LINEA como CPedAttractor::operator delete (0x5EAFE0, con hook). Devuelve `this`.
CPedATMAttractor* __fastcall PedATMAttractorScalarDeletingDestructor(CPedATMAttractor* self, void* /*edx*/, uint8 flags) {
    PedATMAttractorDestructorBody(self, nullptr);
    if (flags & 1) {
        CPedAttractor::operator delete(self);
    }
    return self;
}
} // namespace

void CPedATMAttractor::InjectHooks() {
    RH_ScopedClass(CPedATMAttractor);
    RH_ScopedCategory("Attractors");

    // Lote 220A (sin validar)
    RH_ScopedNamedGlobalInstall(PedATMAttractorConstructor, "Constructor", 0x5EE470);
    RH_ScopedNamedGlobalInstall(PedATMAttractorScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5EE4E0);
    RH_ScopedNamedGlobalInstall(PedATMAttractorDestructorBody, "DestructorBody", 0x5EE530);
}
