/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCustomBuildingRenderer
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "CustomBuildingRenderer.h"

#include "CustomBuildingDNPipeline.h"
#include "CustomBuildingPipeline.h"

void CCustomBuildingRenderer::InjectHooks() {
    RH_ScopedClass(CCustomBuildingRenderer);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5D7EC0);
    RH_ScopedInstall(Shutdown, 0x5D7EE0);
    RH_ScopedInstall(PluginAttach, 0x5D7EF0);
    RH_ScopedInstall(AtomicSetup, 0x5D7F00);
    RH_ScopedInstall(IsCBPCPipelineAttached, 0x5D7F40);
    RH_ScopedInstall(UpdateDayNightBalanceParam, 0x5D7F80);
    RH_ScopedInstall(Update, 0x5D8050);
}

// 0x5D7EC0
bool CCustomBuildingRenderer::Initialise() {
    auto pipe = CCustomBuildingPipeline::CreatePipe();
    if (pipe)
        return CCustomBuildingDNPipeline::CreatePipe();

    return pipe;
}

// 0x5D7EE0
void CCustomBuildingRenderer::Shutdown() {
    CCustomBuildingPipeline::DestroyPipe();
    CCustomBuildingDNPipeline::DestroyPipe();
}

// 0x5D7EF0
bool CCustomBuildingRenderer::PluginAttach() {
    return CCustomBuildingDNPipeline::ExtraVertColourPluginAttach();
}

// 0x5D7F00
void CCustomBuildingRenderer::AtomicSetup(RpAtomic* atomic) {
    auto* const geometry = RpAtomicGetGeometry(atomic);
    if (CCustomBuildingDNPipeline::GetExtraVertColourPtr(geometry)->NightColors && RpGeometryGetPreLightColors(geometry)) {
        CCustomBuildingDNPipeline::CustomPipeAtomicSetup(atomic);
    } else {
        CCustomBuildingPipeline::CustomPipeAtomicSetup(atomic);
    }
}

// 0x5D7F40
bool CCustomBuildingRenderer::IsCBPCPipelineAttached(RpAtomic* atomic) {
    const auto id = GetPipelineID(atomic);
    if (id == 0x53F2009C || id == 0x53F20098) {
        return true;
    }
    auto* const geometry = RpAtomicGetGeometry(atomic);
    return CCustomBuildingDNPipeline::GetExtraVertColourPtr(geometry)->NightColors && RpGeometryGetPreLightColors(geometry);
}

// 0x5D7F80
void CCustomBuildingRenderer::UpdateDayNightBalanceParam() {
    // The executable multiplies seconds by this float reciprocal before storing
    // the minute value to float. Preserve that rounding and the x87 products.
    constexpr float invMinute = 1.0f / 60.0f;
    const float fractionalMinutes = float(double(CClock::GetGameClockSeconds()) * invMinute);
    const float minutes = float(CClock::GetGameClockHours() * 60 + CClock::GetGameClockMinutes()) + fractionalMinutes;
    auto& balance = CCustomBuildingDNPipeline::m_fDNBalanceParam;
    if (minutes < 360.0f) {
        balance = 1.0f;
    } else if (minutes < 420.0f) {
        balance = float(double(420.0f - minutes) * invMinute);
    } else if (minutes < 1200.0f) {
        balance = 0.0f;
    } else if (minutes < 1260.0f) {
        // GTA uses single-precision x87 arithmetic here: round the product
        // before subtracting it from one, including at exactly 20:00.
        const float remainingDay = float(double(1260.0f - minutes) * invMinute);
        balance = 1.0f - remainingDay;
    } else {
        balance = 1.0f;
    }
}

// 0x5D8050
void CCustomBuildingRenderer::Update() {
    ZoneScoped;
    UpdateDayNightBalanceParam();
    auto& workingIndex = StaticRef<uint32>(0xC02C14);
    workingIndex = (workingIndex + 1u) & 15u;
    StaticRef<uint32>(0xC02C18) = 0;
}
