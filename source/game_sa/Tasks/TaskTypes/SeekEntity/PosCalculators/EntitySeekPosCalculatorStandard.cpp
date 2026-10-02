/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEntitySeekPosCalculatorStandard
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

#include "EntitySeekPosCalculatorStandard.h"

void CEntitySeekPosCalculatorStandard::InjectHooks() {
    RH_ScopedVirtualClass(CEntitySeekPosCalculatorStandard, 0x859DC8, 1); // Como el original: vtable de una sola entrada (0x46A602 escribe 0x859DC8)
    RH_ScopedCategory("Tasks/TaskTypes/SeekPosCalculators");

    RH_ScopedVMTInstall(ComputeEntitySeekPos, 0x46af20);
}

/*
// 0x46AC10
CEntitySeekPosCalculatorStandard::CEntitySeekPosCalculatorStandard(CEntity* entity, int32 time, int32 a3, float radius, float a5, float a6, bool a7, bool a8) : CTaskComplex() {
    plugin::CallMethod<0x46AC10, CEntitySeekPosCalculatorStandard*, CEntity*, int32, int32, float, float, float, bool, bool>(this, entity, time, a3, radius, a5, a6, a7, a8);
}
*/

// 0x46AF20
void CEntitySeekPosCalculatorStandard::ComputeEntitySeekPos(const CPed& seeker, const CEntity& target, CVector& outPos) {
    outPos = target.GetPosition();
}
