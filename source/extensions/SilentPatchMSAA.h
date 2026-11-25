#pragma once

#include <algorithm>
#include <bit>
#include <cstdint>

#include "SilentPatchConfig.h"

/*
 * SilentPatch (MIT, Copyright (c) 2024 Adrian Zdanowicz (Silent); ver docs/THIRD_PARTY_NOTICES.md): opciones de Anti-Aliasing (MSAA) de 2x hasta 16x (L66 de CHANGELOG-SA.md).
 *
 * Opcion `MSAAFix` (sin entrada en SilentPatchSA.ini; por defecto encendida; GTASA_SILENTPATCH_MSAAFIX=0 o SilentPatch_MSAAFix=0).
 * Con ella APAGADA todo es identico al EXE: CMenuManager::m_nPrefsAntialiasing (offset 0xC8, el valor que se guarda en gta_sa.set; m_nDisplayAntialiasing, offset 0xCC, es el valor que se edita en el menu) es directamente el numero de
 * muestras (1 = apagado, 2, 3, 4), el menu tiene un tope fijo de 4 y el texto del ajuste es el numero (1, 2, 3).
 * Con ella ENCENDIDA el mismo campo pasa a ser un INDICE de nivel: 1 = apagado, 2 = 2x, 3 = 4x, 4 = 8x, 5 = 16x (muestras = 1 << (nivel - 1)); el tope del menu es el
 * mayor indice cuya potencia de dos soporta la tarjeta (RwD3D9EngineGetMaxMultiSamplingLevels: el mayor tipo D3DMULTISAMPLE_TYPE 2..16 admitido por
 * CheckDeviceMultiSampleType). El formato de gta_sa.set no cambia (el mismo int de 32 bits); un valor guardado mayor que lo que admite la tarjeta (o fuera de 1..5) se acota.
 */
namespace SilentPatchMSAA {

inline bool Enabled() {
    static const bool enabled = SilentPatchOption("MSAAFix", true);
    return enabled;
}

// Mayor indice de nivel que admite la tarjeta (>= 1) a partir del maximo de muestras que da el driver (0 si aun no se han consultado los niveles).
inline int MaxLevelFromSamples(uint32_t maxSamples) {
    if (maxSamples < 2) {
        return 1;
    }
    // Mayor potencia de dos <= maxSamples (SilentPatch usa _BitScanForward, el bit MAS BAJO: con un maximo no potencia de dos, p. ej. 6, daria 2x en vez de 4x)
    return std::min(static_cast<int>(std::bit_width(maxSamples)), 5);
}

// Acota un indice de nivel a 1..maximo de la tarjeta (si el maximo aun es desconocido, solo a 1..5).
inline int ClampLevel(int level, uint32_t maxSamples) {
    const int upper = maxSamples >= 2 ? MaxLevelFromSamples(maxSamples) : 5;
    return std::clamp(level, 1, upper);
}

// Indice de nivel -> numero de muestras que se pasa a RwD3D9EngineSetMultiSamplingLevels / RwD3D9ChangeMultiSamplingLevels.
inline uint32_t SamplesFromLevel(int level) {
    return 1u << (std::clamp(level, 1, 5) - 1);
}

// Valor de m_nPrefsAntialiasing -> argumento del driver D3D9: con la opcion apagada el propio valor (el EXE); encendida, las muestras del indice.
inline uint32_t DriverSamples(int prefsValue) {
    return Enabled() ? SamplesFromLevel(prefsValue) : static_cast<uint32_t>(prefsValue);
}

} // namespace SilentPatchMSAA
