/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CBridge
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Bridge.h"

void CBridge::InjectHooks() {
    RH_ScopedClass(CBridge);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x41BC70);
    RH_ScopedInstall(Update, 0x41BC80);
    RH_ScopedInstall(FindBridgeEntities, 0x41BCA0);
    RH_ScopedInstall(ShouldLightsBeFlashing, 0x41BC90);
}

// 0x41BC70
void CBridge::Init() {
    // NOP
}

// 0x41BC80
void CBridge::Update() {
    ZoneScoped;

    // NOP
}

// used in CEntity::ProcessLightsForEntity
// always false
// 0x41BC90
bool CBridge::ShouldLightsBeFlashing() {
    return false;
}

// 0x41BCA0
void CBridge::FindBridgeEntities() {
    // NOP
}

// used in CPlayerInfo::Process
// 0x41BCB0
bool CBridge::ThisIsABridgeObjectMovingUp(uint32) {
    return false;
}
