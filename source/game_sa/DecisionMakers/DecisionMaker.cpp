/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDecisionMaker
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

#include "DecisionMaker.h"

void CDecisionMaker::InjectHooks() {
    RH_ScopedClass(CDecisionMaker);
    RH_ScopedCategory("DecisionMakers");

    RH_ScopedInstall(MakeDecision, 0x606060);
}

// 0x4650A0
CDecisionMaker::CDecisionMaker() {
    for (auto& decision : m_aDecisions) {
        decision.SetDefault();
    }
}

// 0x606060: ECX = this (primera CDecision del decision maker), nueve argumentos en pila (ret 24h). Delega en CDecision::MakeDecision (0x6040D0) de la decision
// this + eventIndex * 0x3C (imul de 32 bits que da la vuelta, sin comprobar limites) pasandole los otros ocho argumentos tal cual. CDecision::MakeDecision es la ultima
// llamada; el unico llamador del EXE (CDecisionMakerTypes::MakeDecision para peds) no lee EAX/ECX/EDX despues.
void CDecisionMaker::MakeDecision(int32 eventIndex, int32 eventSourceType, bool bIsPedInVehicle, int32 taskTypeToAvoid1, int32 taskTypeToAvoid2, int32 taskTypeToAvoid3, int32 taskTypeToSeek, int16& taskType, int16& facialTaskType) {
    auto* const decision = reinterpret_cast<CDecision*>(reinterpret_cast<uint8*>(this) + static_cast<uint32>(eventIndex) * static_cast<uint32>(sizeof(CDecision)));
    decision->MakeDecision(eventSourceType, bIsPedInVehicle, taskTypeToAvoid1, taskTypeToAvoid2, taskTypeToAvoid3, taskTypeToSeek, taskType, facialTaskType);
}
