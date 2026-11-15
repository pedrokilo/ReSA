/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxInterpInfo32_c
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

// Este archivo se compila con /O2 /Oi /arch:IA32 /fp:fast /Zi, sin PCH ni unity (ver source/CMakeLists.txt): GetVal usa la aritmetica x87 del EXE.
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "StdInc.h"

#include "FxInterpInfo32.h"
#include "FxManager.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

void FxInterpInfo32_c::InjectHooks() {
    RH_ScopedClass(FxInterpInfo32_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A8990);
    RH_ScopedInstall(Destructor, 0x004A89B0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x004A8D70);
    RH_ScopedInstall(GetVal, 0x4A89C0);
}

// 0x4A8990
FxInterpInfo32_c::FxInterpInfo32_c() : FxInterpInfo_c() {
    m_Keys = nullptr;
}

// 0x4A8990 (lote 218A, sin validar): constructor del EXE (thiscall, devuelve this): escribe +4 (byte) = 0, +8 = 0, vtable y +0xC = 0 (no toca +5 ni +6). Llama al constructor C++ de arriba, que reproduce esas escrituras.
FxInterpInfo32_c* FxInterpInfo32_c::Constructor() {
    this->FxInterpInfo32_c::FxInterpInfo32_c();
    return this;
}

// 0x4A89B0 (lote 218A, sin validar): cuerpo del destructor (thiscall, `mov dword ptr [ecx],0x85A968; ret`): solo deja la vtable base de FxInterpInfo_c. Lo llama el destructor con borrado 0x4A8D70 y los destructores de los FxInfo*_c.
__declspec(noinline) void FxInterpInfo32_c::Destructor() {
    this->FxInterpInfo32_c::~FxInterpInfo32_c();
}

// 0x4A8D70 (lote 218A, sin validar): destructor con borrado (ranura 0 de la vtable; thiscall, un argumento de pila, `ret 4`): `push esi; mov esi,ecx; call 0x4A89B0; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`. Libera this solo si el bit 0 del BYTE de banderas esta puesto y devuelve this.
FxInterpInfo32_c* FxInterpInfo32_c::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}

// 0x5C1B10
void FxInterpInfo32_c::Load(FILESTREAM file) {
    for (auto i = 0; i < m_nCount; i++) {
        ReadField<void>(file);
        ReadField<void>(file, "FX_INTERP_DATA:");

        m_bLooped  = ReadField<bool>(file);
        m_nNumKeys = ReadField<int8>(file);

        if (i == 0) {
            m_pTimes = g_fxMan.Allocate<uint16>(m_nNumKeys);
        }

        m_Keys[i] = g_fxMan.Allocate<int16>(m_nNumKeys);
        for (auto j = 0; j < m_nNumKeys; j++) {
            ReadField<void>(file, "FX_KEYFLOAT_DATA:");
            m_pTimes[j]     = uint16(ReadField<float>(file) * 256.f);
            m_Keys[i][j] = int16(ReadField<float>(file) * 1000.f);
        }
    }
}

// NOTSA
void FxInterpInfo32_c::Allocate(int32 count) {
    m_nCount  = count;
    m_Keys = g_fxMan.Allocate<int16*>(count);
}

// 0x4A89C0
void FxInterpInfo32_c::GetVal(float* outValues, float delta) {
    InterpolateKeys(m_Keys, outValues, delta, 0.0010000000474974513f);
}
