/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxInterpInfo255_c
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

#include "FxInterpInfo255.h"
#include "FxManager.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

void FxInterpInfo255_c::InjectHooks() {
    RH_ScopedClass(FxInterpInfo255_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(GetVal, 0x4A8B80);
}

// 0x4A8B50
FxInterpInfo255_c::FxInterpInfo255_c() : FxInterpInfo_c() {
    m_Keys = nullptr;
}

// 0x5C1D30
void FxInterpInfo255_c::Load(FILESTREAM file) {
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
            m_pTimes[j] = uint16(ReadField<float>(file) * 256.f);
            m_Keys[i][j] = uint16(ReadField<float>(file) * 128.f);
        }
    }
}

// NOTSA
void FxInterpInfo255_c::Allocate(int32 count) {
    m_nCount = count;
    m_Keys = g_fxMan.Allocate<int16*>(count);
}

// 0x4A8B80
void FxInterpInfo255_c::GetVal(float* outValues, float delta) {
    InterpolateKeys(m_Keys, outValues, delta, 0.0078125f);
}
