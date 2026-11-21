/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: D3dxErrorLog
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <cstring>

#include "D3dxErrorLog.h"
#include "Crt/CrtHeap.h"

// Registro de errores del ensamblador de shaders de D3DX9 del EXE (lote NEWDEL). Ver D3dxErrorLog.h y reversing/FnNEWDEL/README.md.

namespace {
using CreateBufferFn = int32(__stdcall*)(uint32 size, void** ppBuffer); // 0x780E8C: D3DXCreateBuffer (el EXE la deja a cargo del llamado: sin `add esp`)
constexpr uintptr_t D3DX_CREATE_BUFFER = 0x780E8C;

// ID3DXBuffer::GetBufferPointer: ranura 3 de la vtable, __stdcall con this como unico argumento
inline void* BufferPointer(void* buffer) {
    void** const vtbl = *static_cast<void***>(buffer);
    return reinterpret_cast<void*(__stdcall*)(void*)>(vtbl[3])(buffer);
}
} // namespace

// 0x76E5FB
int32 D3dxErrorLog::ToBuffer(void** ppBuffer) {
    if (!ppBuffer) {
        return 0;
    }
    if (totalSize == 0) {
        *ppBuffer = nullptr;
        return 0;
    }
    const int32 hr = reinterpret_cast<CreateBufferFn>(D3DX_CREATE_BUFFER)(totalSize + 1, ppBuffer);
    if (hr < 0) {
        return hr;
    }
    // Rellena de atras hacia delante: el terminador en el final y, antes, cada texto empezando por la cabeza de la cadena (el mas reciente va al final)
    char* end = static_cast<char*>(BufferPointer(*ppBuffer)) + totalSize;
    *end      = '\0';
    for (void* node = head; node;) {
        const char* const text = reinterpret_cast<const char*>(node) + 4;
        const size_t      len  = strlen(text);
        end -= len;
        memcpy(end, text, len);
        node = *static_cast<void**>(node);
    }
    return 0;
}

void D3dxErrorLog::InjectHooks() {
    RH_ScopedClass(D3dxErrorLog);
    RH_ScopedCategory("Plugins/D3DX9");

    RH_ScopedInstall(ToBuffer, 0x76E5FB);
}
