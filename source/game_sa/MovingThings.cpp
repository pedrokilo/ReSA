/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CMovingThings
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

#include "MovingThings.h"
#include "PlaneTrails.h"
#include "Escalators.h"
#include "PointLights.h"

void CMovingThings::InjectHooks() {
    RH_ScopedClass(CMovingThings);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x717D00);
    RH_ScopedInstall(Shutdown, 0x717C20);
    RH_ScopedInstall(Update, 0x7185B0);
    RH_ScopedInstall(Render, 0x717150);
    RH_ScopedInstall(Render_BeforeClouds, 0x7178F0);
}

// 0x717D00
void CMovingThings::Init() {
    CPlaneTrails::Init();
    CPointLights::Init();
    CEscalators::Init();
}

// 0x717C20
void CMovingThings::Shutdown() {
    CEscalators::Shutdown();
}

// 0x7185B0
void CMovingThings::Update() {
    ZoneScoped;

    CPlaneTrails::Update();
    CEscalators::Update();
}

// 0x717150
void CMovingThings::Render() {
    ZoneScoped;

    // NOP
}

// 0x7178F0
void CMovingThings::Render_BeforeClouds() {
    CPlaneTrails::Render();
}
