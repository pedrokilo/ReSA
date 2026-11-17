/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedLookAtAttractor
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

#include "PedLookAtAttractor.h"

void CPedLookAtAttractor::InjectHooks() {
    RH_ScopedClass(CPedLookAtAttractor);
    RH_ScopedCategory("Attractors");

    // Lote 221A (sin validar)
    RH_ScopedInstall(Constructor, 0x5EE960);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x5EE9D0);
    RH_ScopedInstall(Destructor, 0x5EEA20);
}

// 0x5EE960 (lote 221A, sin validar; thiscall, tres argumentos de pila, `ret 0Ch`, 85 bytes): constructor del EXE. Llama al constructor de CPedAttractor (0x5EDFB0) con (efecto, entidad, estado de movimiento, 1 (maximo de peatones) y siete
// flotantes leidos de globales del EXE que valen 1.0, 30000.0, 3000.0, 0.125, 0.1, 0.1 y 0.1) y deja la vtable de la clase (0x86C5E4); devuelve this. El constructor de C++ de la cabecera pasa esos mismos valores.
CPedLookAtAttractor* CPedLookAtAttractor::Constructor(C2dEffectPedAttractor* effect, CEntity* entity, eMoveState moveState) {
    return std::construct_at(this, effect, entity, moveState);
}

// 0x5EEA20 (lote 221A, sin validar; thiscall, sin argumentos de pila, `ret`, 11 bytes): cuerpo del destructor del EXE: `mov [ecx], 0x86C5E4; jmp 0x5EC410` (deja la vtable de la clase y sigue con el cuerpo del destructor de
// CPedAttractor, con hook). Funcion propia sin expansion en linea; solo la llama el destructor con borrado. Llamada no virtual al destructor de la clase.
__declspec(noinline) void CPedLookAtAttractor::Destructor() {
    this->CPedLookAtAttractor::~CPedLookAtAttractor();
}

// 0x5EE9D0 (lote 221A, sin validar; ranura 1 de la vtable 0x86C5E4, tras GetType; thiscall, un argumento de pila, `ret 4`, 76 bytes): destructor con borrado. Llama al cuerpo 0x5EEA20 y, solo si el bit 0 del BYTE de banderas esta puesto,
// libera `this` en el pool de atractores EN LINEA (indice = (this - almacenamiento) / 0xC4 con signo, estado |= 0x80, cursor = min con signo; como CPedAttractor::operator delete, 0x5EAFE0, con hook). Devuelve this.
CPedLookAtAttractor* CPedLookAtAttractor::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CPedAttractor::operator delete(this);
    }
    return this;
}
