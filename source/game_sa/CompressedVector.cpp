#include "StdInc.h"

#include "CompressedVector.h"

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

void CompressedVectorInjectHooksLote105A()
{
    RH_ScopedNamespaceName("CompressedVector");
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(UncompressVectorExe, 0x411590);
}
