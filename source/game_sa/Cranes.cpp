/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCranes
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

#include "Cranes.h"

void CCranes::InjectHooks() {
    RH_ScopedClass(CCranes);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(InitCranes, 0x6F3FC0);
    RH_ScopedInstall(UpdateCranes, 0x6F3FE0);
    RH_ScopedInstall(IsThisCarBeingCarriedByAnyCrane, 0x6F3FF0);
    RH_ScopedInstall(IsThisCarBeingTargettedByAnyCrane, 0x6F4000);
    RH_ScopedInstall(IsThisCarPickedUp, 0x6F3FD0);
}

// 0x6F3FC0
void CCranes::InitCranes() {
    // NOP
}

// 0x6F3FE0
void CCranes::UpdateCranes() {
    ZoneScoped;

    // NOP
}

// 0x6F3FF0
bool CCranes::IsThisCarBeingCarriedByAnyCrane(CVehicle* vehicle) {
    return false;
}

// 0x6F4000
bool CCranes::IsThisCarBeingTargettedByAnyCrane(CVehicle* vehicle) {
    return false;
}

// 0x6F3FD0
bool CCranes::IsThisCarPickedUp(float a1, float a2, CVehicle* vehicle) {
    return false;
}
