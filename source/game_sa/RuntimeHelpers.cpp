/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: VectorConstructorIterator
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

#include "RuntimeHelpers.h"

// 0x403D60 (stdcall, `ret 10h`): dec count; si es negativo (con signo) sale; si no, repite `count` veces (ECX = puntero, que avanza `size` bytes con aritmetica de 32 bits). EBX, EBP, ESI y EDI se conservan.
void __stdcall VectorConstructorIteratorExe(void* base, uint32 size, int32 count, void(__thiscall* ctor)(void*))
{
    if (int32(uint32(count) - 1u) < 0) {
        return;
    }
    uint32 address = uint32(reinterpret_cast<uintptr_t>(base));
    for (uint32 left = uint32(count); left != 0; --left) {
        ctor(reinterpret_cast<void*>(uintptr_t(address)));
        address += size;
    }
}

void RuntimeHelpersInjectHooksLote105A()
{
    RH_ScopedNamespaceName("VectorConstructorIterator");
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(VectorConstructorIteratorExe, 0x403D60);
}
