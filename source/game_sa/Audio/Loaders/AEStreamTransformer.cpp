/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEStreamTransformer
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h" // TODO: Remove

#include <cstring>

#include "AEStreamTransformer.h"

auto& AEStreamTransformer = StaticRef<CAEStreamTransformer>(0xb612d8);

// 0x4f1750
void CAEStreamTransformer::Initialise() {
    static uint8 t[] = {0xEA, 0x3A, 0xC4, 0xA1, 0x9A, 0xA8, 0x14, 0xF3, 0x48, 0xB0, 0xD7, 0x23, 0x9D, 0xE8, 0xFF, 0xF1};
    memcpy(table, t, sizeof(t));
}

// 0x4f17d0
void CAEStreamTransformer::TransformBuffer(void* buffer, size_t size, uint32 position) {
    uint8* buf = reinterpret_cast<uint8*>(buffer);

    for (size_t i = 0; i < size; i++)
        buf[i] ^= table[(position + i) & 0xF];
}

void CAEStreamTransformer::InjectHooks() {
    RH_ScopedClass(CAEStreamTransformer);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Initialise, 0x4f1750);
    RH_ScopedInstall(TransformBuffer, 0x4f17d0);
}
