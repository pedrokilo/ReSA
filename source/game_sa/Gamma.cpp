/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CGamma
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

#include "Gamma.h"

void CGamma::InjectHooks() {
    RH_ScopedClass(CGamma);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x747180);
    RH_ScopedInstall(SetGamma, 0x747200);
}

// 0x747180
void CGamma::Init() {
    // Como el original: m_IsEnabled se escribe como dword (también pone a cero el relleno).
    *reinterpret_cast<uint32*>(&m_IsEnabled) = 1;
    m_CurrentLevel = 0.5f;
    m_TargetLevel  = -1000.0f;
    m_PrevLevel    = 1000.0f;

    // Como el original: primero consulta las capacidades del dispositivo y después las de RW.
    auto* dev  = (IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice();
    D3DCAPS9 devCaps;
    dev->GetDeviceCaps(&devCaps);

    auto* caps = (D3DCAPS9*)const_cast<void*>(RwD3D9GetCaps());
    if (caps->DeclTypes & D3DDTCAPS_DEC3N) {
        caps->DeclTypes &= ~D3DDTCAPS_DEC3N;
    }

    if (!(devCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA) && !(devCaps.Caps2 & D3DCAPS2_CANCALIBRATEGAMMA)) {
        *reinterpret_cast<uint32*>(&m_IsEnabled) = 0; // No gamma support
    }
}

// 0x747200
void CGamma::SetGamma(float gammaLevel, bool fade) {
    const float level = gammaLevel * 0.7f;
    // Como el original: m_IsEnabled se lee como dword.
    if (!*reinterpret_cast<const uint32*>(&m_IsEnabled)) {
        return;
    }
    // Como el original: m_PrevLevel recibe siempre el nivel nuevo, antes de compararlo.
    m_PrevLevel = level;
    if (level != m_TargetLevel) {
        m_TargetLevel = level;
    } else if (!fade) {
        return;
    }

    auto* dev = (IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice();
    if (!dev) {
        return;
    }

    const float power = 1.0f - level + 0.2f;
    if (!gbGammaChanged) {
        gbGammaChanged = true;
        // Como el original: vuelve a pedir el dispositivo a RW.
        ((IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice())->GetGammaRamp(0, &ms_SavedGamma);
    }

    // Como el original: pow de la CRT en doble precisión y el producto por 65535.0 redondeado por la x87
    // a 24 bits; la conversión a entero es la truncación de 64 bits de _ftol2.
    static const double k65535 = 65535.0;
    for (auto i = 0; i < 256; i++) {
        const double p = std::pow((double)((float)(i + 1) * 0.00390625f), (double)power);
        float scaled;
        __asm {
            fld   p
            fmul  k65535
            fstp  scaled
        }
        const auto gamma = (int16)(int64)scaled;
        ms_GammaTable.blue[i] = ms_GammaTable.green[i] = ms_GammaTable.red[i] = gamma;
    }
    dev->SetGammaRamp(0, D3DSGR_CALIBRATE, &ms_GammaTable);
}
