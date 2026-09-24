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
 * Evidencia aprobada vinculada: 0/2 entradas.
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
    m_IsEnabled    = true;
    m_CurrentLevel = 0.5f;
    m_TargetLevel  = -1000.0f;
    m_PrevLevel    = 1000.0f;

    auto* caps = (D3DCAPS9*)const_cast<void*>(RwD3D9GetCaps());
    auto* dev  = (IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice();
    D3DCAPS9 devCaps{}; // TODO: what's the difference with `caps`?
    dev->GetDeviceCaps(&devCaps);

    if (caps->DeclTypes & D3DDTCAPS_DEC3N) {
        caps->DeclTypes &= ~D3DDTCAPS_DEC3N;
    }

    if (!(devCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA) && !(devCaps.Caps2 & D3DCAPS2_CANCALIBRATEGAMMA)) {
        m_IsEnabled = false; // No gamma support
    }
}

// 0x747200
void CGamma::SetGamma(float gammaLevel, bool fade) {
    if (!m_IsEnabled) {
        return;
    }
    const auto level = gammaLevel * 0.7f;
    if (level != m_TargetLevel) {
        m_TargetLevel = level;
    } else if (!fade) {
        return;
    }

    auto* dev = (IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice();
    if (!dev) {
        return;
    }

    if (!gbGammaChanged) {
        gbGammaChanged = true;
        dev->GetGammaRamp(0, &ms_SavedGamma);
    }

    const auto power = 1.0f - level + 0.2f;
    // TODO: rngv::zip
    for (auto i = 0; i < 256; i++) {
        const auto gamma = static_cast<int16>(std::pow(float(i + 1) / 256.0f, power) * 65535.0f);
        ms_GammaTable.red[i] = ms_GammaTable.green[i] = ms_GammaTable.blue[i] = gamma;
    }
    dev->SetGammaRamp(0, D3DSGR_CALIBRATE, &ms_GammaTable);
}
