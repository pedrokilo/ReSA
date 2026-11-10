/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxPrim_c
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "FxPrim.h"

void FxPrim_c::InjectHooks() {
    RH_ScopedClass(FxPrim_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x4A9F20); // lote 148A (banco Fn148A)
    RH_ScopedInstall(Destructor, 0x4A9F40);  // lote 148A (banco Fn148A)
    RH_ScopedInstall(Enable, 0x4A9F50);      // lote 148A (banco Fn148A)
}

// 0x4A9F20 (lote 148A, banco Fn148A; thiscall, sin argumentos de pila, `ret`, 21 bytes: `mov eax,ecx; xor ecx,ecx; mov [eax],85A9E0h; mov [eax+4],ecx; mov [eax+8],ecx; mov byte [eax+0Ch],1; ret`;
// el listado lo atribuia a FxPrimBP_c): constructor de FxPrim_c del EXE. Escribe la vtable de la clase del EXE (0x85A9E0, todas sus ranuras son _purecall; la de la DLL no vale), los dos punteros a cero y
// m_bEnabled a 1 (no escribe el relleno +0xD..+0xF) y devuelve this.
FxPrim_c* FxPrim_c::Constructor() {
    *reinterpret_cast<uint32*>(this) = 0x85A9E0;
    m_PrimBP   = nullptr;
    m_System   = nullptr;
    m_bEnabled = true;
    return this;
}

// 0x4A9F40 (lote 148A, banco Fn148A; thiscall, sin argumentos de pila, `ret`, 15 bytes: `xor eax,eax; mov [ecx],85A9E0h; mov [ecx+4],eax; mov [ecx+8],eax; ret`): cuerpo no virtual del destructor de
// FxPrim_c del EXE (los destructores de las derivadas lo alcanzan con un salto; EAX sale a cero, el contrato es void). Deja la vtable del EXE y anula m_PrimBP y m_System; no toca m_bEnabled.
void FxPrim_c::Destructor() {
    *reinterpret_cast<uint32*>(this) = 0x85A9E0;
    m_PrimBP = nullptr;
    m_System = nullptr;
}

// 0x4A9F20
FxPrim_c::FxPrim_c() {
    m_PrimBP   = nullptr;
    m_System   = nullptr;
    m_bEnabled = true;
}

// 0x4A9F40
FxPrim_c::~FxPrim_c() {
    m_PrimBP = nullptr;
    m_System = nullptr;
}

// 0x4A9F50 (lote 148A, banco Fn148A; thiscall, un argumento de pila, `ret 4`, 10 bytes: `mov al,[esp+4]; mov [ecx+0Ch],al; ret 4`): escribe el BYTE bajo del argumento en m_bEnabled (el EXE no normaliza
// a 0/1 si el llamador empuja otro valor; la DLL recibe un bool).
void FxPrim_c::Enable(bool enabled) {
    m_bEnabled = enabled;
}
