/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CSearchLight
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "SearchLight.h"

void CSearchLight::InjectHooks() {
    RH_ScopedClass(CSearchLight);
    RH_ScopedCategory("Scripts");

    RH_ScopedInstall(IsSpottedEntity, 0x493900);
}

void CSearchLight::SetTravelToPoint() {
    assert(0);
}

void CSearchLight::SetFollowEntity() {
    assert(0);
}

void CSearchLight::SetPathBetween() {
    assert(0);
}

void CSearchLight::IsLookingAtPos() {
    assert(0);
}

void CSearchLight::GetOnEntity() {
    assert(0);
}

// 0x493900
bool CSearchLight::IsSpottedEntity(uint32 index, const CEntity& entity) {
    const auto searchLightIdx = CTheScripts::GetActualScriptThingIndex(index, SCRIPT_THING_SEARCH_LIGHT);
    if (searchLightIdx < 0) {
        return false;
    }

    // El EXE copia la posicion (matriz o placement) a una variable local y llama a 0x493280 (cdecl: posicion, indice; sin reconstruir todavia).
    // Como el original, devuelve 1/0 segun AL (no pasa el byte tal cual).
    CVector pos = entity.GetPosition();
    return plugin::CallAndReturn<uint8, 0x493280, CVector*, int32>(&pos, searchLightIdx) != 0;
}
