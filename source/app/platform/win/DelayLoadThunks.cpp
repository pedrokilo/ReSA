/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: DelayLoadThunks
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "DelayLoadThunks.h"

// Ranuras de la IAT de carga retardada del EXE (direcciones): 0x8E3EC4 DirectInput8Create (DINPUT8.dll), 0x8E3ECC DirectSoundCreate8 (DSOUND.dll, ordinal 11). La de DirectDrawCreateEx (0x8E3EBC, thunk 0x81805C) ya no se engancha: el juego no usa DirectDraw (docs/RenderWareExe/directdraw-sustitucion.md).
// Los ayudantes 0x817508, 0x818020 y 0x818041 (que cargan la biblioteca y vuelven con `jmp eax`) no se pueden escribir en C++: ver reversing/PendienteValidar/R8A.md.
namespace {
constexpr uintptr_t kDirectInput8CreateSlot  = 0x8E3EC4;
constexpr uintptr_t kDirectSoundCreate8Slot  = 0x8E3ECC;

using DirectInput8CreateFn = HRESULT(WINAPI*)(HINSTANCE, DWORD, const void*, LPVOID*, void*);
using DirectSoundCreate8Fn = HRESULT(WINAPI*)(const void*, void**, void*);
} // namespace

// 0x817523 (lote R8A, sin validar; stdcall, 5 argumentos, 6 bytes: `jmp dword ptr [0x8E3EC4]`; llamadores: 0x746985 y 0x748801): DirectInput8Create(hinst, version, iid, ppvOut, punkOuter).
// Salta a la direccion de la ranura de la IAT con la misma pila; aqui se llama por la ranura y se devuelve su resultado.
HRESULT WINAPI DirectInput8CreateThunk(HINSTANCE hinst, DWORD version, const void* riid, LPVOID* ppvOut, void* punkOuter) {
    return (*reinterpret_cast<DirectInput8CreateFn*>(kDirectInput8CreateSlot))(hinst, version, riid, ppvOut, punkOuter);
}

// 0x81803B (lote R8A, sin validar; stdcall, 3 argumentos, 6 bytes: `jmp dword ptr [0x8E3ECC]`; llamador: 0x74585A): DirectSoundCreate8(guid, ppDS8, punkOuter) (importacion por ordinal 11 de DSOUND.dll).
HRESULT WINAPI DirectSoundCreate8Thunk(const void* guid, void** ppDS8, void* punkOuter) {
    return (*reinterpret_cast<DirectSoundCreate8Fn*>(kDirectSoundCreate8Slot))(guid, ppDS8, punkOuter);
}

void DelayLoadThunksInjectHooks() {
    RH_ScopedCategory("App");
    RH_ScopedNamespaceName("DelayLoadThunks");

    RH_ScopedGlobalInstall(DirectInput8CreateThunk, 0x817523);
    RH_ScopedGlobalInstall(DirectSoundCreate8Thunk, 0x81803B);
}
