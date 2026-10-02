/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CMaths
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

#include "Maths.h"

// Tabla original en 0xBB3E00 (256 entradas). Se rellena en InitMathsTables como el original
// (fild i · 2π/256 en float y fsin del x87); el inicializador estático solo deja valores
// válidos hasta entonces.
constexpr size_t SIN_LUT_SIZE = 256;
constexpr float  SIN_LUT_STEP = TWO_PI / (float)(SIN_LUT_SIZE);
const auto& SIN_LUT = StaticRef<std::array<float, SIN_LUT_SIZE>>(0xBB3E00) = []() {
    std::array<float, SIN_LUT_SIZE> lut{};
    for (size_t i = 0; i < SIN_LUT_SIZE; ++i) {
        lut[i] = std::sin((float)(i) * SIN_LUT_STEP);
    }
    return lut;
}();

// 0x4A1340: índice = trunc(rad · 40.743664) & 0xFF
float CMaths::GetSinFast(float rad) {
    return SIN_LUT[(uint32)(int64)(rad * std::bit_cast<float>(0x4222F983u)) & 0xFF];
}

// 0x4A1360: índice = trunc(rad · 40.743664 + 64) & 0xFF
float CMaths::GetCosFast(float rad) {
    return SIN_LUT[(uint32)(int64)(rad * std::bit_cast<float>(0x4222F983u) + 64.0f) & 0xFF];
}

// 0x59AC90
void CMaths::InitMathsTables() {
    ZoneScoped;

    auto* lut = StaticRef<std::array<float, SIN_LUT_SIZE>>(0xBB3E00).data();
    static const float kStep = std::bit_cast<float>(0x3CC90FDBu); // 0x859BBC
    for (int32 i = 0; i < (int32)SIN_LUT_SIZE; i++) {
        float v;
        __asm {
            fild  i
            fmul  kStep
            fsin
            fstp  v
        }
        lut[i] = v;
    }
}

void CMaths::InjectHooks() {
    RH_ScopedClass(CMaths);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(InitMathsTables, 0x59AC90);
    RH_ScopedInstall(GetSinFast, 0x4A1340); // No callers in the game, inlined in every single instance
    RH_ScopedInstall(GetCosFast, 0x4A1360); // No callers in the game, inlined in every single instance
}
