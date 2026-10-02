/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CUserDisplay
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "UserDisplay.h"
#include "CurrentVehicle.h"
#include "PlaceName.h"

void CUserDisplay::InjectHooks() {
    RH_ScopedClass(CUserDisplay);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x571EE0);
    RH_ScopedInstall(Process, 0x5720A0);
}

// 0x571EE0
void CUserDisplay::Init() {
    ZoneScoped;

    PlaceName.Init();
    OnscnTimer.Init();
    CurrentVehicle.Init();
}

// 0x5720A0
void CUserDisplay::Process() {
    ZoneScoped;

    PlaceName.Process();
    OnscnTimer.Process();
    CurrentVehicle.Process();
}
