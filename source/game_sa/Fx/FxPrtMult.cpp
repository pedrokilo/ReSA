/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxPrtMult_c
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "FxPrtMult.h"

void FxPrtMult_c::InjectHooks() {
    RH_ScopedClass(FxPrtMult_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x4AB270);
    RH_ScopedInstall(Constructor7f, 0x4AB290);
}

// 0x4AB270
FxPrtMult_c* FxPrtMult_c::Constructor() {
    this->FxPrtMult_c::FxPrtMult_c();
    return this;
}

// 0x4AB270
FxPrtMult_c::FxPrtMult_c() {
    SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_fSize = 1.0f;
    m_Rot = 1.0f;
    m_fLife = 1.0f;
}

// 0x4AB290 (hook: Constructor7f)
FxPrtMult_c* FxPrtMult_c::Constructor7f(float red, float green, float blue, float alpha, float size, float rot, float life) {
    this->FxPrtMult_c::FxPrtMult_c(red, green, blue, alpha, size, rot, life);
    return this;
}

// 0x4AB290
FxPrtMult_c::FxPrtMult_c(float red, float green, float blue, float alpha, float size, float rot, float life) {
    SetUp(red, green, blue, alpha, size, rot, life);
}

// 0x4AB2D0
void FxPrtMult_c::SetUp(float red, float green, float blue, float alpha, float size, float rot, float life) {
    SetColor(red, green, blue, alpha);
    m_fSize = size;
    m_Rot = rot;
    m_fLife = life;
}
