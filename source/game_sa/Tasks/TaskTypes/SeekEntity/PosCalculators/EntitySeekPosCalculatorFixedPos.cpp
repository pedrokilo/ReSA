/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEntitySeekPosCalculatorFixedPos
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EntitySeekPosCalculatorFixedPos.h"
#include "PedGeometryAnalyser.h"

void CEntitySeekPosCalculatorFixedPos::InjectHooks() {
    RH_ScopedVirtualClass(CEntitySeekPosCalculatorFixedPos, 0x0086FD18, 2);
    RH_ScopedCategory("Tasks/TaskTypes/SeekPosCalculators");

    RH_ScopedVMTInstall(ComputeEntitySeekPos, 0x694830);
}

CEntitySeekPosCalculatorFixedPos::CEntitySeekPosCalculatorFixedPos(const CVector& pos) :
    m_fixedPos{ pos }
{
}

void CEntitySeekPosCalculatorFixedPos::ComputeEntitySeekPos(const CPed& seeker, const CEntity& target, CVector& outPos) {
    CPedGeometryAnalyser::ComputeClearTarget(seeker, m_fixedPos, outPos);
}
