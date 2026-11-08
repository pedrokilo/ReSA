/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSequences
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

void CTaskSequences::InjectHooks() {
    RH_ScopedClass(CTaskSequences);
    RH_ScopedCategory("Tasks");

    RH_ScopedInstall(Init, 0x632D90);
    RH_ScopedInstall(CleanUpForShutdown, 0x632DD0);
    RH_ScopedInstall(GetAvailableSlot, 0x632E00); // lote 130A (banco Fn130A)
}

// 0x632D90
void CTaskSequences::Init() {
    ms_iActiveSequence = -1;
    CleanUpForShutdown();
}

// 0x632DD0
void CTaskSequences::CleanUpForShutdown() {
    for (auto i = 0; i < NUM_SEQUENCES; i++) {
        ms_bIsOpened[i] = false;
        auto taskSequence = &ms_taskSequence[i];
        taskSequence->Flush();
    }
}

// 0x632E00
// Lote 130A (banco Fn130A): cdecl, un argumento de pila (el EXE lee solo su BYTE bajo), `ret`. Como el original: con el byte a cero busca en los slots 0..31 y con cualquier otro valor en 32..63;
// devuelve el primero cuyo byte de "abierto" es cero Y cuya primera tarea (+0x10) es nula, o -1.
int32 CTaskSequences::GetAvailableSlot(uint8 slot) {
    const int32 first = slot ? 32 : 0;
    const int32 last  = slot ? 64 : 32;
    for (int32 i = first; i < last; i++) {
        if (reinterpret_cast<const uint8*>(ms_bIsOpened)[i] == 0 && ms_taskSequence[i].m_Tasks[0] == nullptr) {
            return i;
        }
    }
    return -1;
}
