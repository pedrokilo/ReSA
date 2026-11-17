/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxInfoSpriteRect_c
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

#include "FxInfoSpriteRect.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoSpriteRect_c::InjectHooks() {
    RH_ScopedClass(FxInfoSpriteRect_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A60B0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x4A76A0);
    RH_ScopedInstall(Destructor, 0x004A76C0);
}

// 0x4A60B0
FxInfoSpriteRect_c::FxInfoSpriteRect_c() : FxInfo_c() {
    m_nType = FX_INFO_SPRITERECT_DATA;
    m_InterpInfo.Allocate(4);
}

// 0x4A60B0 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoSpriteRect_c* FxInfoSpriteRect_c::Constructor() {
    this->FxInfoSpriteRect_c::FxInfoSpriteRect_c();
    return this;
}

// 0x4A76C0 (lote 218A, sin validar): cuerpo del destructor (thiscall): destruye el interp de +8 (deja su vtable base) y pone la vtable base de FxInfo_c (0x85A7DC). Lo llama el destructor con borrado 0x4A76A0 (lote 217A).
__declspec(noinline) void FxInfoSpriteRect_c::Destructor() {
    this->FxInfoSpriteRect_c::~FxInfoSpriteRect_c();
}

// 0x4A6120
void FxInfoSpriteRect_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A6140
void FxInfoSpriteRect_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[4];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.m_fSpriteTop    = values[0];
    render.m_fSpriteBottom = values[1];
    render.m_fSpriteLeft   = values[2];
    render.m_fSpriteRight  = values[3];
}



// 0x4A76A0 (27 bytes, thiscall, un argumento de pila, `ret 4`): destructor con borrado de FxInfoSpriteRect_c del EXE (ranura 0 de su vtable). `push esi; mov esi,ecx; call 0x4A76C0; test byte [esp+8],1; je ..;
// push esi; call 0x8214BD (operator delete); add esp,4; mov eax,esi; pop esi; ret 4`: destruye con el cuerpo, libera `this` solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor virtual).
FxInfoSpriteRect_c* FxInfoSpriteRect_c::ScalarDeletingDestructor(uint8 flags) {
    reinterpret_cast<void(__thiscall*)(void*)>(0x4A76C0)(this); // cuerpo del destructor del EXE (0x4A76C0, fuera de este lote: llamada al EXE por direccion fija)
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}
