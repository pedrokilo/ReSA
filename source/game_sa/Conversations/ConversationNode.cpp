/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CConversationNode
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "ConversationNode.h"
#include "Conversations.h"

/** Registra la rutina reconstruida que limpia nodos de forma recursiva. */
void CConversationNode::InjectHooks() {
    RH_ScopedClass(CConversationNode);
    RH_ScopedCategory("Conversations");
    // 0x43A710 solo toca eax, ecx y esi; sus llamadores del EXE (CConversations::RemoveConversationForPed 0x43A960,
    // CConversationForPed::Clear 0x43A770) conservan el puntero del slot en edx. ConversationNodeCpp.cpp se compila
    // con /O2 y su objeto no escribe edx, por lo que ya no hace falta un envoltorio.
    RH_ScopedInstall(ClearRecursively, 0x43A710);
}
