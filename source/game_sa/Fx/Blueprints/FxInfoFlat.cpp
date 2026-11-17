/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxInfoFlat_c
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

#include "FxInfoFlat.h"
#include "FxManager.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoFlat_c::InjectHooks() {
    RH_ScopedClass(FxInfoFlat_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A6320);
    RH_ScopedInstall(Destructor, 0x004A7810);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x004A77F0);
}

// 0x4A6320
FxInfoFlat_c::FxInfoFlat_c() : m_InterpInfo() {
    m_nType = FX_INFO_FLAT_DATA;
    m_InterpInfo.Allocate(9);
}

// 0x4A6320 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoFlat_c* FxInfoFlat_c::Constructor() {
    this->FxInfoFlat_c::FxInfoFlat_c();
    return this;
}

// 0x4A7810 (lote 218A, sin validar): cuerpo del destructor (thiscall): destruye el interp de +8 (deja su vtable base) y pone la vtable base de FxInfo_c (0x85A7DC). Solo lo llama el destructor con borrado 0x4A77F0.
__declspec(noinline) void FxInfoFlat_c::Destructor() {
    // Como el EXE (el destructor del interp 0x4A8B70 / 0x4A87F0 / 0x4A8460 / 0x4A89B0 deja su vtable base 0x85A968 y despues se deja la de FxInfo_c, 0x85A7DC). El ~T() generado
    // dejaria vtables de la DLL y omitiria la del interp (destructor trivial): el banco Fn218A lo detecto.
    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;
    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;
}

// 0x4A77F0 (lote 218A, sin validar): destructor con borrado (ranura 0 de la vtable; thiscall, un argumento de pila, `ret 4`): `push esi; mov esi,ecx; call 0x4A7810; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`. Libera this solo si el bit 0 del BYTE de banderas esta puesto y devuelve this.
FxInfoFlat_c* FxInfoFlat_c::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}

// 0x4A6390
void FxInfoFlat_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 004A63B0
void FxInfoFlat_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if ( !m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[9];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.m_FlatMatrix.right = *(RwV3d *)&values[0];
    render.m_FlatMatrix.up    = *(RwV3d *)&values[3];
    render.m_FlatMatrix.at    = *(RwV3d *)&values[6];
    render.m_bIsFlat = true;
    RwMatrixUpdate(&render.m_FlatMatrix);
}
