/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxInfoAnimTexture_c
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "FxInfoAnimTexture.h"
#include "RenderInfo.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoAnimTexture_c::InjectHooks() {
    RH_ScopedClass(FxInfoAnimTexture_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A6550);
    RH_ScopedInstall(Destructor, 0x004A78F0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x004A78D0);
}

// 0x4A6550
FxInfoAnimTexture_c::FxInfoAnimTexture_c() {
    m_nType = FX_INFO_ANIMTEX_DATA;
    m_InterpInfo.Allocate(1);
}

// 0x4A6550 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoAnimTexture_c* FxInfoAnimTexture_c::Constructor() {
    this->FxInfoAnimTexture_c::FxInfoAnimTexture_c();
    return this;
}

// 0x4A78F0 (lote 218A, sin validar): cuerpo del destructor (thiscall): destruye el interp de +8 (deja su vtable base) y pone la vtable base de FxInfo_c (0x85A7DC). Solo lo llama el destructor con borrado 0x4A78D0.
__declspec(noinline) void FxInfoAnimTexture_c::Destructor() {
    this->FxInfoAnimTexture_c::~FxInfoAnimTexture_c();
}

// 0x4A78D0 (lote 218A, sin validar): destructor con borrado (ranura 0 de la vtable; thiscall, un argumento de pila, `ret 4`): `push esi; mov esi,ecx; call 0x4A78F0; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`. Libera this solo si el bit 0 del BYTE de banderas esta puesto y devuelve this.
FxInfoAnimTexture_c* FxInfoAnimTexture_c::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}

// 0x4A65C0
void FxInfoAnimTexture_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A65E0
void FxInfoAnimTexture_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[16];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.m_bHasAnimTextures = true;
    render.m_nCurrentTexId = static_cast<int8>(values[0]);
}
