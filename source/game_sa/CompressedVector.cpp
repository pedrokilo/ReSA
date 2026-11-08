/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CompressedVector
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

#include "CompressedVector.h"
#include "X87Math.h"

// 0x411590 (lote 105A; cdecl, puntero de retorno oculto como primer argumento): descomprime un CompressedVector (tres int16, 1/128) en `out`. Como el EXE, cada componente se lee (movsx de 16 bits),
// se convierte (fild) y se multiplica por 1/128 (0x858B88, exacto en float para cualquier int16) y se escribe ANTES de leer la siguiente: si `out` solapa el origen, las escrituras anteriores se ven.
// El EXE deja en EAX la ultima componente leida (un entero), que ningun llamador lee.
void UncompressVectorExe(CVector* out, const int16* source)
{
    const volatile int16* const s = source;
    volatile float* const o = &out->x;
    o[0] = float(double(int32(s[0])) * 0.0078125);
    o[1] = float(double(int32(s[1])) * 0.0078125);
    o[2] = float(double(int32(s[2])) * 0.0078125);
}

// 0x536E90 (lote 130A, banco Fn130A; cdecl, dos punteros en la pila, `ret`, void): comprime un vector (tres floats) en tres int16 (x128). Como el EXE, cada componente se lee (fld dword), se multiplica por
// 128.0f y se trunca con _ftol2 (0x821B40, entero de 64 bits: se guarda el word bajo del resultado, sin saturar a 16 bits), y se escribe ANTES de leer la siguiente: si `out` solapa el origen, las
// escrituras anteriores se ven. El EXE deja en EAX el ultimo resultado de _ftol2, que ningun llamador lee.
void CompressVectorExe(int16* out, const float* source)
{
    volatile int16* const o = out;
    const volatile float* const s = source;
    o[0] = static_cast<int16>(CompressVectorWordCpp(s[0]));
    o[1] = static_cast<int16>(CompressVectorWordCpp(s[1]));
    o[2] = static_cast<int16>(CompressVectorWordCpp(s[2]));
}

void CompressedVectorInjectHooksLote105A()
{
    RH_ScopedNamespaceName("CompressedVector");
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(UncompressVectorExe, 0x411590);
    RH_ScopedGlobalInstall(CompressVectorExe, 0x536E90); // lote 130A (banco Fn130A)
}
