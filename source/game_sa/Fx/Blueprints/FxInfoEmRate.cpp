/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxInfoEmRate_c
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

#include "FxInfoEmRate.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoEmRate_c::InjectHooks() {
    RH_ScopedClass(FxInfoEmRate_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A6A00);
    RH_ScopedInstall(Destructor, 0x004A6A90);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x004A6A70);
}

// 0x4A6A00
FxInfoEmRate_c::FxInfoEmRate_c() {
    m_nType = FX_INFO_EMRATE_DATA;
    m_InterpInfo.Allocate(1);
}

// 0x4A6A00 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoEmRate_c* FxInfoEmRate_c::Constructor() {
    this->FxInfoEmRate_c::FxInfoEmRate_c();
    return this;
}

// 0x4A6A90 (lote 216A, sin validar): cuerpo del destructor (thiscall): destruye el interp de +8 (deja su vtable base) y pone la vtable base de FxInfo_c (0x85A7DC). Solo lo llama el destructor con borrado 0x4A6A70.
__declspec(noinline) void FxInfoEmRate_c::Destructor() {
    // Como el EXE (0x4A89B0 / 0x4A8460 / 0x4A87F0 y despues el [this]): el destructor del interp deja su vtable base (0x85A968) y despues se deja la de FxInfo_c (0x85A7DC). El ~T() generado deja
    // vtables de la DLL y omite la del interp (destructor trivial).
    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;
    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;
}

// 0x4A6A70 (lote 216A, sin validar): destructor con borrado (ranura 0 de la vtable; thiscall, un argumento de pila, `ret 4`): `push esi; mov esi,ecx; call 0x4A6A90; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`. Libera this solo si el bit 0 del BYTE de banderas esta puesto y devuelve this.
FxInfoEmRate_c* FxInfoEmRate_c::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}

// 0x4A4B40
void FxInfoEmRate_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A4B60
void FxInfoEmRate_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    float values[16];
    m_InterpInfo.GetVal(values, currentTime);

    auto& emission = *static_cast<EmissionInfo_t*>(info);

    auto v10 = currentTime - totalTime;
    if (useConst) {
        emission.m_fCount = values[0] * totalTime;
    } else if (v10 >= 0.0f) {
        emission.m_fCount = m_InterpInfo.GetVal(0, currentTime, totalTime);
    } else {
        emission.m_fCount = m_InterpInfo.GetVal(0, len, -v10);
        emission.m_fCount = m_InterpInfo.GetVal(0, currentTime, v10 + totalTime) + emission.m_fCount;
    }
}
