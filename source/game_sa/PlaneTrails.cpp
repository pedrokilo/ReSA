/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPlaneTrails
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PlaneTrails.h"

namespace {
// Como el original: fsin/fcos del x87 (precisión interna completa) y producto redondeado por la FPU.
float __declspec(naked) __cdecl PlaneTrailsSinMulX87(float angle, float k) {
    __asm {
        fld  dword ptr [esp + 4]
        fsin
        fmul dword ptr [esp + 8]
        ret
    }
}
float __declspec(naked) __cdecl PlaneTrailsCosMulX87(float angle, float k) {
    __asm {
        fld  dword ptr [esp + 4]
        fcos
        fmul dword ptr [esp + 8]
        ret
    }
}
} // namespace

void CPlaneTrails::InjectHooks() {
    RH_ScopedClass(CPlaneTrails);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x717370);
    RH_ScopedInstall(Render, 0x7173A0);
    RH_ScopedInstall(RegisterPoint, 0x7174C0);
    RH_ScopedInstall(Update, 0x7174F0);
}

// 0x717370
void CPlaneTrails::Init() {
    for (auto& pt : aArray) {
        pt.Init();
    }
}

// 0x7173A0
void CPlaneTrails::Render() {
    // Como el original: mínimo de (1 - nubes, 1 - niebla) y luego con (1 - lluvia), eligiendo el segundo en empates o NaN.
    const float clouds = 1.0f - CWeather::CloudCoverage;
    const float fog    = 1.0f - CWeather::Foggyness;
    float visibilityIntensity = clouds < fog ? clouds : fog;
    const float rain = 1.0f - CWeather::Rain;
    if (!(visibilityIntensity < rain)) {
        visibilityIntensity = rain;
    }

    // Como el original: componente máxima (uint16, sin signo) por 1/256 (0x859AA0).
    const CColourSet& cc = CTimeCycle::m_CurrentColours;
    const uint16 redGreen          = cc.m_nSkyBottomRed > cc.m_nSkyBottomGreen ? cc.m_nSkyBottomRed : cc.m_nSkyBottomGreen;
    const uint16 maxComponent      = redGreen > cc.m_nSkyBottomBlue ? redGreen : cc.m_nSkyBottomBlue;
    const float  maxColorComponent = static_cast<float>(static_cast<int32>(maxComponent)) * (1.0f / 256.0f);
    const float  finalIntensity    = maxColorComponent > visibilityIntensity ? visibilityIntensity : maxColorComponent;

    if (finalIntensity > 0.0001f) {
        for (auto& pt : aArray) {
            pt.Render(finalIntensity);
        }
    }
}

// 0x7174C0
void CPlaneTrails::RegisterPoint(CVector point, uint32 trailIndex) {
    aArray.data()[trailIndex].RegisterPoint(point); // Como el original: sin comprobar el índice
}

// 0x7174F0
void CPlaneTrails::Update() {
    const uint32 coronaBaseId = 100; // TODO: Magic number

    // Como el original: (tiempo & 0x1FFFF) * 2pi/131072 (0x8631C4), y cada eje es fsin/fcos del x87 por la constante.
    const auto Angle = [] { return static_cast<float>(static_cast<int32>(CTimer::GetTimeInMS() & 0x1FFFF)) * std::bit_cast<float>(0x38490FDBu); };

    float t = Angle();
    aArray[0].Update(CVector{ PlaneTrailsSinMulX87(t, 2590.0f), PlaneTrailsCosMulX87(t, +2200.0f), 550.0f }, CRGBA{ 255,   0,   0, 255 }, coronaBaseId + 1,   0, 22, 7);
    t = Angle();
    aArray[1].Update(CVector{ PlaneTrailsSinMulX87(t, 2000.0f), PlaneTrailsCosMulX87(t, -2600.0f), 500.0f }, CRGBA{ 255, 255, 128, 255 }, coronaBaseId + 2, 350, 21, 7);
    t = Angle();
    // Como el original: el tercer avión usa el coseno para X y el seno para Y.
    aArray[2].Update(CVector{ PlaneTrailsCosMulX87(t, 2100.0f), PlaneTrailsSinMulX87(t, +1300.0f), 600.0f }, CRGBA{ 255, 255, 255, 255 }, coronaBaseId + 3, 200, 20, 6);
}
