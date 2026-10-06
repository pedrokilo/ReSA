/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CWeather
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 13/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "eWeatherType.h"
#include "game_sa/Data/Weather.def"

#include "FxPrtMult.h"
#include "CullZones.h"
#include "Pad.h"
#include "Replay.h"
#include "SpecialFX.h"
#include "CutsceneMgr.h"
#include "Clock.h"
#include "PostEffects.h"

// 0x8CCF30
std::array<float, 16> CWeather::saTreeWindOffsets = { 1.0f, 0.5f, 0.2f, 0.7f, 0.4f, 1.0f, 0.5f, 0.3f, 0.2f, 0.1f, 0.7f, 0.6f, 0.3f, 1.0f, 0.5f, 0.2f };

/// 0x8CCF70
std::array<float, 32> CWeather::saBannerWindOffsets = { 0.0f, 0.3f, 0.6f, 0.85f, 0.99f, 0.97f, 0.65f, 0.15f, -0.1f, 0.0f, 0.35f, 0.57f, 0.55f, 0.35f, 0.45f, 0.67f, 0.73f, 0.45f, 0.25f, 0.35f, 0.35f, 0.11f, 0.13f, 0.21f, 0.28f, 0.28f, 0.22f, 0.1f, 0.0f, -0.1f, -0.17f, -0.12f };

namespace {
// Como el original: variables estaticas del EXE que CWeather usa sin nombre en el resto del codigo.
auto& RainStreakPosX          = StaticRef<int32*>(0xC81420);
auto& RainStreakPosY          = StaticRef<int32*>(0xC8141C);
auto& RainStreakPosZ          = StaticRef<int32*>(0xC81418);
auto& RainStreakStrength      = StaticRef<uint8*>(0xC81414);
auto& WaterFogFXValue         = StaticRef<float>(0xC81444);   // valor suavizado del efecto de niebla de agua
auto& WaterFogFXHolding       = StaticRef<uint8>(0xC81440);   // 1 mientras el valor baja
auto& HeatHazeFXLevel         = StaticRef<float>(0xC81448);   // nivel acumulado de la calima
auto& HeatHazeLastMinute      = StaticRef<int32>(0x8D6078);   // ultimo minuto del juego visto por la calima
auto& HeatHazeFadeSpeed       = StaticRef<float>(0x8D50DC);   // 0.05
auto& HeatHazeInsideFadeSpeed = StaticRef<float>(0x8D50E0);   // 0.5
auto& RainSplashScale         = StaticRef<float>(0x8D5FF0);   // 1.0
auto& ClockSecondsByte        = StaticRef<uint8>(0xB70150);   // el EXE lee solo el byte bajo de los segundos
auto& WaterFogSourceWord      = StaticRef<uint16>(0xB7C540);  // origen (palabra) del efecto de niebla de agua
auto& CameraOrientation       = StaticRef<float>(0xB6F178);
auto& CameraForward984        = StaticRef<CVector>(0xB6F9AC); // vector hacia delante de TheCamera.m_mCameraMatrix
auto& TunnelPoint1            = StaticRef<CVector>(0xC81430);
auto& TunnelPoint2            = StaticRef<CVector>(0xC81424);
auto& TunnelInitFlags         = StaticRef<uint32>(0xC8143C);

// Como el original: 0x858C7C = 1 / 32767 (float exacto)
constexpr float RAND_SCALE = 3.0518509447574615e-05f;
constexpr float ONE_OVER_60 = 0.01666666753590107f;      // 0x859044
constexpr float PI_F        = 3.1415927410125732f;       // 0x858CB8

// Tablas del EXE (0x8D5E50, 0x8D5FF8 y 0x8D6038).
constexpr float WindPerWeather[23] = {
    0.0f, 0.25f, 0.0f, 0.2f, 0.7f, 0.25f, 0.0f, 0.7f, 1.0f, 0.0f, 0.2f, 0.0f,
    0.4f, 0.0f, 0.3f, 0.7f, 1.0f, 0.0f, 0.3f, 1.5f, 0.0f, 0.0f, 0.0f,
};
constexpr float WindGustTableA[16] = { // 0x8D5FF8
    1.0f, 0.5f, 1.0f, 0.2f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f, 0.8f, 0.0f, 1.0f, 1.0f, 0.7f, 1.0f, 1.0f,
};
constexpr float WindGustTableB[16] = { // 0x8D6038
    0.5f, -0.3f, 0.8f, 0.0f, -0.4f, -0.8f, 0.3f, -0.1f, -0.9f, -0.5f, 0.7f, 0.7f, 0.3f, 0.7f, 0.0f, -0.5f,
};

// Como el original: fsin/fcos del x87 con precision interna completa; el producto y la suma posteriores los redondea la FPU.
float SinMulAddX87(float fAng, float vScale, float vAddend) {
    float fOut;
    __asm {
        fld  fAng
        fsin
        fmul vScale
        fadd vAddend
        fstp fOut
    }
    return fOut;
}

float CosMulAddX87(float fAng, float vScale, float vAddend) {
    float fOut;
    __asm {
        fld  fAng
        fcos
        fmul vScale
        fadd vAddend
        fstp fOut
    }
    return fOut;
}

// 0.5 - cos(a) * 0.5 con el coseno sin redondear
float HalfMinusHalfCosX87(float fAng) {
    const float fHalf = 0.5f;
    float fOut;
    __asm {
        fld   fAng
        fcos
        fmul  fHalf
        fsubr fHalf
        fstp  fOut
    }
    return fOut;
}

// -sin(a) sin redondear antes de guardar
float NegSinX87(float fAng) {
    float fOut;
    __asm {
        fld  fAng
        fsin
        fchs
        fstp fOut
    }
    return fOut;
}

float CosX87(float fAng) {
    float fOut;
    __asm {
        fld  fAng
        fcos
        fstp fOut
    }
    return fOut;
}

// Como el original: rand() del EXE (0x821B1E)
inline int32 Rand() { return (int32)CGeneral::GetRandomNumber(); }

// ((rand * 1/32767) * mul + add) - sub, redondeando a float en cada paso (x87 a 24 bits)
inline float RandMulAddSub(float mul, float add, float sub) {
    const float fRand = (float)Rand();
    return ((fRand * RAND_SCALE) * mul + add) - sub;
}

// Como el original: _ftol2 (0x821B40) trunca
inline int32 Ftol(float f) { return (int32)f; }

inline bool IsRainyType(int16 w)  { return w == WEATHER_RAINY_COUNTRYSIDE || w == WEATHER_RAINY_SF; }
inline bool IsSunnyType(int16 w) {
    return w == WEATHER_SUNNY_LA || w == WEATHER_SUNNY_SMOG_LA || w == WEATHER_SUNNY_COUNTRYSIDE
        || w == WEATHER_SUNNY_SF || w == WEATHER_SUNNY_VEGAS || w == WEATHER_SUNNY_DESERT;
}
inline bool IsExtraSunnyType(int16 w) {
    return w == WEATHER_EXTRASUNNY_LA || w == WEATHER_EXTRASUNNY_SMOG_LA || w == WEATHER_EXTRASUNNY_COUNTRYSIDE
        || w == WEATHER_EXTRASUNNY_SF || w == WEATHER_EXTRASUNNY_VEGAS || w == WEATHER_EXTRASUNNY_DESERT;
}
inline bool IsHeatHazeType(int16 w) {
    return w == WEATHER_SUNNY_DESERT || w == WEATHER_EXTRASUNNY_DESERT || w == WEATHER_EXTRASUNNY_VEGAS || w == WEATHER_EXTRASUNNY_LA;
}

// Mezcla del EXE hacia un objetivo: si |d| < paso salta al objetivo; si no, suma o resta paso (d > 0 suma)
inline void StepTowards(float& value, float target, float step) {
    const float d = target - value;
    if (std::fabs(d) < step) {
        value = target;
    } else if (d > 0.0f) {
        value = value + step;
    } else {
        value = value - step;
    }
}
} // namespace

void CWeather::InjectHooks() {
    RH_ScopedClass(CWeather);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x72A480);
    RH_ScopedInstall(AddRain, 0x72A9A0);
    RH_ScopedInstall(AddSandStormParticles, 0x72A820);
    RH_ScopedInstall(FindWeatherTypesList, 0x72A520);
    RH_ScopedInstall(ForceWeather, 0x72A4E0);
    RH_ScopedInstall(ForceWeatherNow, 0x72A4F0);
    RH_ScopedInstall(ForecastWeather, 0x72A590);
    RH_ScopedInstall(ReleaseWeather, 0x72A510);
    RH_ScopedInstall(RenderRainStreaks, 0x72AF70);
    RH_ScopedInstall(SetWeatherToAppropriateTypeNow, 0x72A790);
    RH_ScopedInstall(Update, 0x72B850);
    RH_ScopedInstall(UpdateInTunnelness, 0x72B630);
    //RH_ScopedInstall(UpdateWeatherRegion, 0x72A640, true, { .reversed = false }); // bad
    RH_ScopedInstall(IsRainy, 0x4ABF50);
}

// 0x72A480
void CWeather::Init() {
    ZoneScoped;

    NewWeatherType = WEATHER_EXTRASUNNY_LA;
    OldWeatherType = WEATHER_EXTRASUNNY_LA;
    WeatherRegion  = WEATHER_REGION_DEFAULT;

    InterpolationValue = 0.0f;
    WeatherTypeInList = 0;
    ForcedWeatherType = WEATHER_UNDEFINED;
    WhenToPlayLightningSound = 0;
    bScriptsForceRain = false;
    Rain = 0.0f;
    Sandstorm = 0.0f;
    CurrentRainParticleStrength = 0;
    InTunnelness = 0.0f;
    LightningStartX = 0;
    LightningStartY = 0;
    StreamAfterRainTimer = 0;
}

// 0x72A9A0
void CWeather::AddRain() {
    // Como el original: el orden de las salidas es CamNoRain, PlayerNoRain, bajo el agua, area, area del jugador,
    // altura de la camara y cabina cerrada.
    if (CCullZones::CamNoRain()) {
        return;
    }
    if (CCullZones::PlayerNoRain()) {
        return;
    }
    if (UnderWaterness > 0.0f) {
        return;
    }
    if (CGame::currArea) {
        return;
    }
    if (FindPlayerPed(-1)) {
        if (FindPlayerPed(-1)->GetAreaCode() != 0) {
            return;
        }
    }
    if (TheCamera.GetPosition().z > 900.0f) {
        return;
    }
    if (TheCamera.GetLookingLRBFirstPerson()) {
        if (auto* veh = FindPlayerVehicle(-1, false)) {
            if (veh->CarHasRoof()) {
                return;
            }
        }
    }

    // Como el original: la bandera de "ha llovido hace poco" y su temporizador (800 fotogramas)
    if (Rain > 0.0f) {
        StreamAfterRainFlag = 1;
        StreamAfterRainTimer = 800;
    } else if (StreamAfterRainFlag) {
        if (StreamAfterRainTimer > 0) {
            StreamAfterRainTimer--;
        } else {
            StreamAfterRainFlag = 0;
            StreamAfterRainTimer = 800;
        }
    }

    if (Wind > 1.01f) {
        if (!CCullZones::CamNoRain() && !CCullZones::PlayerNoRain()) {
            if (UnderWaterness <= 0.0f) {
                AddSandStormParticles();
            }
        }
    }

    if (!(Rain > 0.1f)) {
        if (RainFXAlpha == 0.0f) {
            return;
        }
    }

    const int32 numDropGroups = Ftol(Rain * 5.0f);
    float       range         = Rain * 10.0f;
    if (40.0f > range) {
        range = 40.0f;
    }
    const int32 numDropsPerGroup = 15 - Ftol(Rain * -2.0f);

    if (numDropGroups > 0) {
        const float maxRadius = range * 0.5f;
        for (int32 g = 0; g < numDropGroups; g++) {
            const FxPrtMult_c fxMult(1.0f, 1.0f, 1.0f, 0.25f, 0.02f, 0.0f, 0.03f);
            const CVector     vel{0.0f, 0.0f, 0.0f};
            const float       radius = CGeneral::GetRandomNumberInRange(0.0f, maxRadius);

            // Como el original: con rand impar, angulo = (rand & 0xFF) * k; si no, se reutiliza el primer rand
            const int32 rand1 = Rand();
            float       angle;
            if (rand1 & 1) {
                angle = (float)(Rand() & 0xFF) * 0.02453125f;
            } else {
                angle = (float)((rand1 & 0xFF) - 0x80) * 0.00625f + CameraOrientation;
            }

            CVector probe;
            probe.x = SinMulAddX87(angle, radius, TheCamera.GetPosition().x);
            probe.y = CosMulAddX87(angle, radius, TheCamera.GetPosition().y);
            probe.z = 40.0f;

            CColPoint colPoint;
            CEntity*  hitEntity;
            if (CWorld::ProcessVerticalLine(probe, -40.0f, colPoint, hitEntity, true, false, false, false, true, false, nullptr)) {
                const float groundZ = colPoint.m_vecPoint.z + 0.1f;
                for (int32 i = 0; i < numDropsPerGroup; i++) {
                    CVector dropPos;
                    dropPos.x = RandMulAddSub(30.0f, probe.x, 15.0f);
                    dropPos.y = RandMulAddSub(30.0f, probe.y, 15.0f);
                    dropPos.z = groundZ;
                    (void)Rand(); // Como el original: tercer rand, el resultado no se usa
                    g_fx.m_Splash->AddParticle(dropPos, vel, 0.0f, fxMult);
                }
            }
        }
    }

    // Como el original: el brillo del efecto de lluvia sube o baja un paso hacia Rain * 0.2
    float       alpha  = RainFXAlpha;
    const float target = Rain * 0.2f;
    if (alpha < target) {
        alpha = alpha + 0.0025f;
    }
    if (alpha > target) {
        alpha = alpha - 0.0025f;
    }
    if (0.0f > alpha) {
        alpha = 0.0f;
    }
    alpha = alpha * RainSplashScale;
    if (1.0f < alpha) {
        RainFXAlpha = 1.0f;
    } else {
        RainFXAlpha = alpha;
    }

    const FxPrtMult_c fxMult2(0.9f, 0.9f, 1.0f, RainFXAlpha, 1.0f, 0.0f, 0.2f);
    {
        CVector pos;
        pos.x = CameraForward984.x * 10.0f + TheCamera.GetPosition().x;
        pos.y = CameraForward984.y * 10.0f + TheCamera.GetPosition().y;
        pos.z = TheCamera.GetPosition().z;
        pos.x = RandMulAddSub(40.0f, pos.x, 20.0f);
        pos.y = RandMulAddSub(40.0f, pos.y, 20.0f);
        pos.z = RandMulAddSub(7.0f, pos.z, 2.0f);
        const CVector vel{WindDir.x * 15.0f, WindDir.y * 15.0f, WindDir.z * 15.0f};
        g_fx.m_Sand2->AddParticle(pos, vel, 0.0f, fxMult2);
    }
}

// 0x72A820
void CWeather::AddSandStormParticles() {
    const FxPrtMult_c fxMult(0.67f, 0.65f, 0.55f, 0.25f, 1.0f, 0.0f, 0.2f);

    // Como el original: (frente * 10) + camara; luego ((rand * k * 40) + valor) - 20 (no valor + (rand - 20))
    CVector pos;
    pos.x = CameraForward984.x * 10.0f + TheCamera.GetPosition().x;
    pos.y = CameraForward984.y * 10.0f + TheCamera.GetPosition().y;
    pos.z = TheCamera.GetPosition().z;
    pos.x = RandMulAddSub(40.0f, pos.x, 20.0f);
    pos.y = RandMulAddSub(40.0f, pos.y, 20.0f);
    pos.z = RandMulAddSub(7.0f, pos.z, 2.0f);

    const CVector vel{WindDir.x * 25.0f, WindDir.y * 25.0f, WindDir.z * 25.0f};
    g_fx.m_Sand2->AddParticle(pos, vel, 0.0f, fxMult);
}

// 0x72A520
const int8* CWeather::FindWeatherTypesList() {
    switch (WeatherRegion) {
    case WEATHER_REGION_LA:     return WeatherTypesListLA;
    case WEATHER_REGION_SF:     return WeatherTypesListSF;
    case WEATHER_REGION_LV:     return WeatherTypesListVegas;
    case WEATHER_REGION_DESERT: return WeatherTypesListDesert;
    default:                    return WeatherTypesListDefault;
    }
}

// 0x72A4E0
void CWeather::ForceWeather(eWeatherType weatherType) {
    ForcedWeatherType = weatherType;
}

// 0x72A4F0
void CWeather::ForceWeatherNow(eWeatherType weatherType) {
    ForcedWeatherType = weatherType;
    OldWeatherType = weatherType;
    NewWeatherType = weatherType;
}

// 0x72A590
bool CWeather::ForecastWeather(eWeatherType weatherType, int32 numSteps) {
    if (numSteps < 0) {
        return false;
    }
    const int32 start = (int32)WeatherTypeInList;
    for (int32 i = 0; i <= numSteps; i++) {
        // Como el original: modulo 64 con signo (and 0x8000003F) sobre WeatherTypeInList + i
        const int32 idx = (start + i) % 64;
        if ((int32)FindWeatherTypesList()[idx] == (int32)weatherType) {
            return true;
        }
    }
    return false;
}

// 0x72A510
void CWeather::ReleaseWeather() {
    ForcedWeatherType = WEATHER_UNDEFINED;
}

// 0x72AF70
void CWeather::RenderRainStreaks() {
    if (CTimer::GetIsCodePaused())
        return;

    {
        // Como el original: (64 - FogReduction) * (int)(Rain * 110) * (1/64), truncado; la fuerza sube o baja de uno en uno
        const float fogTerm  = 64.0f - (float)CTimeCycle::m_FogReduction;
        const int32 rainTerm = Ftol(Rain * 110.0f);
        const int32 strength = Ftol((fogTerm * (float)rainTerm) * 0.015625f);
        int32       cur      = (int32)CurrentRainParticleStrength;
        if (cur < strength) {
            cur++;
        } else if (cur > strength) {
            cur--;
        }
        if (cur < 0) {
            cur = 0;
        }
        CurrentRainParticleStrength = (uint32)cur;
        if (cur == 0)
            return;
    }

    if (CCullZones::CamNoRain())
        return;
    if (CCullZones::PlayerNoRain())
        return;

    if (UnderWaterness > 0.0f)
        return;

    if (CGame::currArea)
        return;

    if (TheCamera.GetPosition().z > 900.0f)
        return;

    uiTempBufferIndicesStored = 0;
    uiTempBufferVerticesStored = 0;

    constexpr int32 RAIN_STREAK_COUNT = 32;

    if (!RainStreakPosX) {
        // Como el original: reserva Y, X, Z (128 bytes cada una) y la fuerza (32 bytes), en ese orden
        RainStreakPosY     = (int32*)operator new[](0x80);
        RainStreakPosX     = (int32*)operator new[](0x80);
        RainStreakPosZ     = (int32*)operator new[](0x80);
        RainStreakStrength = (uint8*)operator new[](0x20);

        for (int32 i = 0; i < RAIN_STREAK_COUNT; i++) {
            RainStreakPosY[i] = 0;
            RainStreakPosX[i] = 0;
            RainStreakPosZ[i] = 0;
            RainStreakStrength[i] = (uint8)Ftol((float)(int32)CurrentRainParticleStrength * 0.6f);
        }
    }

    for (int32 s = 0; s < RAIN_STREAK_COUNT; s++) {
        // Como el original: se vuelve a leer la posicion de la camara en cada uso
        const float relX = (float)RainStreakPosX[s] - TheCamera.GetPosition().x;
        const float relY = (float)RainStreakPosY[s] - TheCamera.GetPosition().y;
        const float fZ   = (float)RainStreakPosZ[s];
        const float relZ = fZ - TheCamera.GetPosition().z;

        bool respawn = true;
        if (RainStreakStrength[s] != 0 && fZ > 0.0f) {
            const CVector rel{relX, relY, relZ};
            if (!(rel.Magnitude() > 8.0f)) {
                respawn = false;
            }
        }
        if (respawn) {
            // Como el original: ((rand * k) * 5 + frente * 6) + camara - 2.5, truncado
            RainStreakPosX[s] = Ftol((((float)Rand() * RAND_SCALE) * 5.0f + CameraForward984.x * 6.0f + TheCamera.GetPosition().x) - 2.5f);
            RainStreakPosY[s] = Ftol((((float)Rand() * RAND_SCALE) * 5.0f + CameraForward984.y * 6.0f + TheCamera.GetPosition().y) - 2.5f);
            RainStreakPosZ[s] = Ftol((((float)Rand() * RAND_SCALE) * 5.0f + CameraForward984.z * 6.0f + TheCamera.GetPosition().z) - 2.5f);
            RainStreakStrength[s] = (uint8)Ftol((float)(int32)CurrentRainParticleStrength * 0.6f);
        }

        // Como el original: extremo lejano = posicion - viento * 0.1 (y por Rain en las rayas pares)
        float endX, endY, endZ;
        if (s & 1) {
            endX = (float)RainStreakPosX[s] - (WindDir.x * Wind) * 0.1f;
            endY = (float)RainStreakPosY[s] - (WindDir.y * Wind) * 0.1f;
            endZ = (float)RainStreakPosZ[s] - (WindDir.z * Wind) * 0.1f;
        } else {
            endX = (float)RainStreakPosX[s] - ((WindDir.x * Wind) * Rain) * 0.1f;
            endY = (float)RainStreakPosY[s] - ((WindDir.y * Wind) * Rain) * 0.1f;
            endZ = (float)RainStreakPosZ[s] - ((WindDir.z * Wind) * Rain) * 0.1f;
        }

        const int32 vtxBase = uiTempBufferVerticesStored;
        const int32 idxBase = uiTempBufferIndicesStored;
        aTempBufferIndices[idxBase]     = (RxVertexIndex)vtxBase;
        aTempBufferIndices[idxBase + 1] = (RxVertexIndex)(vtxBase + 1);

        const float r1 = (float)Rand() * RAND_SCALE;
        const float r2 = ((float)Rand() * RAND_SCALE) * 0.4f - 0.2f;
        const float r3 = ((float)Rand() * RAND_SCALE) * 0.4f - 0.2f;
        const float w  = (((float)Rand() * RAND_SCALE) * 0.2f) - 0.1f;

        RxObjSpace3DVertex* v0 = &TempBufferVertices.m_3d[vtxBase];
        RxObjSpace3DVertex* v1 = &TempBufferVertices.m_3d[vtxBase + 1];
        const uint32 strengthNow = RainStreakStrength[s];

        v0->color = (strengthNow << 24) | 0xD2D2E6u;
        v0->objVertex.x = (float)RainStreakPosX[s] + r2;
        v0->objVertex.y = (float)RainStreakPosY[s] + r3;
        v0->objVertex.z = (float)RainStreakPosZ[s] + w;

        v1->color = (strengthNow << 23) | 0xD2D2E6u;
        uiTempBufferVerticesStored = vtxBase + 2;
        v1->objVertex.x = r2 + endX;
        v1->objVertex.y = r3 + endY;
        v1->objVertex.z = (((r1 * 0.4f) + 0.1f) + w) + endZ;
        uiTempBufferIndicesStored = idxBase + 2;

        // Como el original: la raya cae (rand * k * 0.09 + 0.01) y pierde de 2 a 4 de fuerza (0 si no le alcanza)
        const float fall = ((float)Rand() * RAND_SCALE) * 0.09f + 0.01f;
        RainStreakPosZ[s] = Ftol((float)RainStreakPosZ[s] - fall);
        const int32 loss = CGeneral::GetRandomIntInRange(2, 5);
        if ((int32)RainStreakStrength[s] > loss) {
            RainStreakStrength[s] = (uint8)(RainStreakStrength[s] - (uint8)loss);
        } else {
            RainStreakStrength[s] = 0;
        }
    }

    if (uiTempBufferIndicesStored != 0) {
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
        RwRenderStateSet(rwRENDERSTATEFOGTYPE,           RWRSTATE(rwFOGTYPELINEAR));
        RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));

        if (RwIm3DTransform(TempBufferVertices.m_3d, uiTempBufferVerticesStored, nullptr, rwIM3D_VERTEXXYZ)) {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, aTempBufferIndices, uiTempBufferIndicesStored);
            RwIm3DEnd();
        }

        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    }

    uiTempBufferIndicesStored = 0;
    uiTempBufferVerticesStored = 0;
}

// 0x72A790
void CWeather::SetWeatherToAppropriateTypeNow() {
    CVector playerCoors = FindPlayerCoors();
    UpdateWeatherRegion(&playerCoors);

    const auto weatherType = (eWeatherType)(int16)FindWeatherTypesList()[0];
    ForcedWeatherType = WEATHER_UNDEFINED;
    OldWeatherType = weatherType;
    NewWeatherType = weatherType;
}

// 0x72B850
void CWeather::Update() {
    ZoneScoped;

    if ((CTimer::m_FrameCounter & 0xF) == 0) {
        UpdateWeatherRegion(nullptr);
    }

    if (CReplay::Mode != MODE_PLAYBACK) {
        // Como el original: ((segundos * 1/60) + minutos) * 1/60 con los segundos como byte
        const float t = ((float)ClockSecondsByte * ONE_OVER_60 + (float)CClock::ms_nGameClockMinutes) * ONE_OVER_60;
        if (t < InterpolationValue) {
            UpdateWeatherRegion(nullptr);
            OldWeatherType = NewWeatherType;
            if ((int16)ForcedWeatherType >= 0) {
                NewWeatherType = ForcedWeatherType;
            } else if (TheCamera.GetPosition().z < 950.0f) {
                WeatherTypeInList = (uint32)(((int32)WeatherTypeInList + 1) % 64);
                NewWeatherType = (eWeatherType)(int16)FindWeatherTypesList()[(int32)WeatherTypeInList];
            }
        }
        InterpolationValue = t;
    }

    const int16  oldW = (int16)OldWeatherType;
    const int16  newW = (int16)NewWeatherType;
    const uint32 time = CTimer::m_snTimeInMilliseconds;
    const float  IV   = InterpolationValue;

    // ---- Relampagos ----
    if (IsRainyType(newW) && IsRainyType(oldW)) {
        if (!CCullZones::CamNoRain() && !CCullZones::PlayerNoRain() && UnderWaterness <= 0.0f && !CGame::currArea) {
            if (LightningBurst) {
                const int32 rnd = Rand() & 0xFF;
                if (rnd < 24) {
                    const uint32 diff = CTimer::m_FrameCounter - LightningStart;
                    LightningBurst = false;
                    const uint32 duration = diff > 20u ? 20u : diff;
                    LightningDuration = duration;
                    WhenToPlayLightningSound = (int32)((20u - duration) * 150u + CTimer::m_snTimeInMilliseconds);
                    LightningFlash = false;
                } else if ((uint32)(time - (uint32)LightningFlashLastChange) > 0x32u) {
                    const bool previous = LightningFlash;
                    const bool current  = (Rand() & 1) != 0;
                    LightningFlash = current;
                    if (previous != current) {
                        LightningFlashLastChange = (int32)time;
                    }
                }
            } else {
                const int32 rnd = Rand() & 0xFFFF;
                if (rnd < 200) {
                    LightningStart = CTimer::m_FrameCounter;
                    LightningFlashLastChange = (int32)time;
                    LightningBurst = true;
                    LightningFlash = true;
                } else {
                    LightningFlash = false;
                }
            }
        } else {
            LightningFlash = false;
            LightningBurst = false;
        }
    } else {
        LightningFlash = false;
        LightningBurst = false;
    }

    if (WhenToPlayLightningSound != 0 && time > (uint32)WhenToPlayLightningSound) {
        m_WeatherAudioEntity.AddAudioEvent(AE_THUNDER);
        const uint32 duration = LightningDuration;
        const uint8  freq     = (uint8)(((uint8)duration + 0x28) << 1);
        CPad::GetPad(0)->StartShake((int16)(duration * 40u + 100u), freq, 0);
        WhenToPlayLightningSound = 0;
    }

    // ---- Carreteras mojadas ----
    if (IsRainyType(oldW)) {
        if (IsRainyType(newW)) {
            WetRoads = 1.0f;
        } else {
            WetRoads = 1.0f - IV;
        }
    } else if (IsRainyType(newW)) {
        WetRoads = IV;
    } else {
        WetRoads = 0.0f;
    }

    // ---- Lluvia y tormenta de arena ----
    float rainBlend = 0.0f;
    if (IsRainyType(newW)) {
        rainBlend = IV;
    }
    if (IsRainyType(oldW)) {
        rainBlend = rainBlend + (1.0f - IV);
    }
    const float gustFactor = (float)((time >> 13) & 3) * 0.1f + 0.7f;
    const float targetRain = gustFactor * rainBlend;
    const float step       = CTimer::ms_fTimeStep * 0.005f;
    StepTowards(Rain, targetRain, step);

    float sandBlend = 0.0f;
    if (newW == WEATHER_SANDSTORM_DESERT) {
        sandBlend = IV;
    }
    if (oldW == WEATHER_SANDSTORM_DESERT) {
        sandBlend = sandBlend + (1.0f - IV);
    }
    const float targetSand = gustFactor * sandBlend;
    StepTowards(Sandstorm, targetSand, step);

    // ---- Nubosidad, niebla, brillo ----
    if (IsSunnyType(oldW) || IsExtraSunnyType(oldW)) {
        CloudCoverage = 0.0f;
    } else {
        CloudCoverage = 1.0f - IV;
    }
    if (!(IsSunnyType(newW) || IsExtraSunnyType(newW))) {
        CloudCoverage = CloudCoverage + IV;
    }

    if (oldW == WEATHER_FOGGY_SF || oldW == WEATHER_SANDSTORM_DESERT) {
        Foggyness = 1.0f - IV;
    } else {
        Foggyness = 0.0f;
    }
    if (newW == WEATHER_FOGGY_SF || newW == WEATHER_SANDSTORM_DESERT) {
        Foggyness = Foggyness + IV;
    }

    if (oldW == WEATHER_FOGGY_SF) {
        Foggyness_SF = 1.0f - IV;
    } else {
        Foggyness_SF = 0.0f;
    }
    if (newW == WEATHER_FOGGY_SF) {
        Foggyness_SF = Foggyness_SF + IV;
    }

    if (IsExtraSunnyType(oldW)) {
        ExtraSunnyness = 1.0f - IV;
    } else {
        ExtraSunnyness = 0.0f;
    }
    if (IsExtraSunnyType(newW)) {
        ExtraSunnyness = ExtraSunnyness + IV;
    }

    // ---- Arco iris ----
    const uint8 hours = CClock::ms_nGameClockHours;
    if ((oldW == WEATHER_CLOUDY_LA || oldW == WEATHER_CLOUDY_COUNTRYSIDE || oldW == WEATHER_CLOUDY_VEGAS || oldW == WEATHER_CLOUDY_SF)
        && IsSunnyType(newW)
        && IV < 0.5f
        && hours > 6 && hours < 21) {
        float x = IV - 0.25f;
        if (x < 0.0f) {
            x = -x;
        }
        Rainbow = 1.0f - x * 4.0f;
    } else {
        Rainbow = 0.0f;
    }

    // ---- Reflejo del sol ----
    if (IsExtraSunnyType(oldW) || IsSunnyType(oldW)) {
        SunGlare = 1.0f - IV;
    } else {
        SunGlare = 0.0f;
    }
    if (IsExtraSunnyType(newW) || IsSunnyType(newW)) {
        SunGlare = SunGlare + IV;
    }
    if (SunGlare > 0.0f) {
        float sunZ = CTimeCycle::m_VectorToSun[CTimeCycle::m_CurrentStoredValue].z * 7.0f;
        if (1.0f < sunZ) {
            sunZ = 1.0f;
        }
        SunGlare = sunZ * SunGlare;
        if (0.0f > SunGlare) {
            SunGlare = 0.0f;
        } else if (1.0f < SunGlare) {
            SunGlare = 1.0f;
        }
        if (!CSpecialFX::bSnapShotActive) {
            const int32 rnd = Rand() & 0x1F;
            SunGlare = (1.0f - (float)rnd * 0.007f) * SunGlare;
        }
    }

    // ---- Calima ----
    if (IsHeatHazeType(oldW)) {
        HeatHaze = 1.0f - IV;
    } else {
        HeatHaze = 0.0f;
    }
    if (IsHeatHazeType(newW)) {
        HeatHaze = HeatHaze + IV;
    }
    if (HeatHaze > 0.0f) {
        const int32 minutes       = CClock::ms_nGameClockMinutes;
        const int32 hourStart     = CPostEffects::m_HeatHazeFXHourOfDayStart;
        const int32 hourEnd       = CPostEffects::m_HeatHazeFXHourOfDayEnd;
        const bool  minuteChanged = minutes != HeatHazeLastMinute;
        float       rate          = HeatHazeFadeSpeed;
        HeatHazeFXControl = 0.0f;

        const int32 hoursNow = CClock::ms_nGameClockHours;
        const bool  inWindow = hoursNow >= hourStart && hoursNow < hourEnd;
        const bool  pastEnd  = hoursNow >= hourEnd;

        bool  fade    = false; // true = ruta de bajada
        float hazeNow = 0.0f;
        if (CGame::currArea != 0) {
            rate = HeatHazeInsideFadeSpeed;
            fade = true;
        } else {
            auto* player = FindPlayerPed(-1);
            if (player->GetAreaCode() != 0 || CCullZones::CamNoRain() || CCullZones::PlayerNoRain()) {
                rate = HeatHazeInsideFadeSpeed;
                fade = true;
            } else {
                if (inWindow) {
                    if (minuteChanged) {
                        HeatHazeFXLevel = HeatHazeFXLevel + rate;
                    }
                    if (HeatHazeFXLevel > 1.0f) {
                        HeatHazeFXLevel = 1.0f;
                    }
                    hazeNow = HeatHazeFXLevel;
                } else {
                    hazeNow = HeatHazeFXControl;
                }
                if (pastEnd) {
                    fade = true;
                }
            }
        }
        if (fade) {
            if (minuteChanged) {
                HeatHazeFXLevel = HeatHazeFXLevel - rate;
            }
            if (HeatHazeFXLevel < 0.0f) {
                HeatHazeFXLevel = 0.0f;
            }
            hazeNow = HeatHazeFXLevel;
        }
        HeatHazeFXControl = hazeNow * HeatHaze;
        HeatHazeLastMinute = minutes;
    }

    // ---- Niebla de agua ----
    {
        const uint16 w = WaterFogSourceWord;
        const float  x = (float)w * 0.01f;
        bool         holding;
        if (w == 0) {
            WaterFogFXValue = 0.0f;
            holding = WaterFogFXHolding != 0;
        } else if (w < 0x5F) {
            holding = WaterFogFXHolding != 0;
        } else {
            WaterFogFXHolding = 1;
            holding = true;
        }
        if (!holding) {
            if (x > WaterFogFXValue) {
                WaterFogFXValue = x;
            }
        } else {
            if (x < WaterFogFXValue) {
                WaterFogFXValue = x;
            }
            if (WaterFogFXValue <= 0.0f) {
                WaterFogFXHolding = 0;
            }
        }
        WaterFogFXControl = WaterFogFXValue * 1.4f;
        if (WaterFogFXControl < 0.0f) {
            WaterFogFXControl = 0.0f;
        } else if (WaterFogFXControl > 1.0f) {
            WaterFogFXControl = 1.0f;
        }
    }

    // ---- Viento ----
    Wind = WindPerWeather[oldW] * (1.0f - IV) + WindPerWeather[newW] * IV;
    if (1.0f < Wind) {
        WindClipped = 1.0f;
    } else {
        WindClipped = Wind;
    }
    {
        const float wc = WindClipped;
        WindDir.x = wc * 0.7f;
        WindDir.y = wc * 0.7f;

        const int32 e = (int32)((time >> 10) & 0xF);
        const float a = HalfMinusHalfCosX87(((float)(time & 0x3FF) * 0.0009765625f) * PI_F);
        const float b = 1.0f - a;
        float X = (((a * WindGustTableB[(e + 1) & 0xF]) + (b * WindGustTableB[e])) * WindClipped) * 0.4f + WindDir.x;
        float Y = (((b * WindGustTableB[(e + 3) & 0xF]) + (a * WindGustTableB[(e + 4) & 0xF])) * WindClipped) * 0.4f + WindDir.y;
        WindDir.z = (((b * WindGustTableB[(e + 6) & 0xF]) + (a * WindGustTableB[(e + 7) & 0xF])) * WindClipped) * 0.2f;

        const float U = (WindClipped - 0.5f) * 0.4f;
        if (U > 0.0f) {
            const int32 e2 = (int32)((time >> 8) & 0xF);
            const float V  = (float)(time & 0xFF) * 0.00390625f;
            const float b2 = 1.0f - V;
            X = X + ((V * WindGustTableB[(e2 + 1) & 0xF]) + (b2 * WindGustTableB[e2])) * U;
            Y = Y + ((b2 * WindGustTableB[(e2 + 3) & 0xF]) + (V * WindGustTableB[(e2 + 4) & 0xF])) * U;
            WindDir.z = WindDir.z + ((b2 * WindGustTableB[(e2 + 6) & 0xF]) + (V * WindGustTableB[(e2 + 7) & 0xF])) * U;
        }

        const int32 e3 = (int32)((time >> 11) & 0xF);
        const float c  = HalfMinusHalfCosX87(((float)(time & 0x7FF) * 0.00048828125f) * PI_F);
        const float d  = 1.0f - c;
        const float m  = d * WindGustTableA[e3] + c * WindGustTableA[(e3 + 1) & 0xF];
        WindDir.x = m * X;
        WindDir.y = m * Y;
        WindDir.z = m * WindDir.z;
    }

    {
        const float s = WindClipped + 0.3f;
        if (1.0f < s) {
            Wavyness = 1.0f;
        } else {
            Wavyness = s;
        }
    }

    {
        const float limit = 1.0f - UnderWaterness;
        if (!(Rain < limit)) {
            Rain = limit;
        }
    }

    // ---- Semaforos y faros ----
    {
        const float minutesF = (float)CClock::ms_nGameClockMinutes * ONE_OVER_60;
        if (hours > 20) {
            TrafficLightsBrightness = 1.0f;
        } else if (hours > 19) {
            TrafficLightsBrightness = minutesF;
        } else if (hours > 6) {
            TrafficLightsBrightness = 0.0f;
        } else if (hours > 5) {
            TrafficLightsBrightness = 1.0f - minutesF;
        } else {
            TrafficLightsBrightness = 1.0f;
        }
    }

    {
        const float m = (Foggyness > Rain) ? Foggyness : Rain;
        if (1.0f < m) {
            HeadLightsSpectrum = 1.0f;
        } else {
            HeadLightsSpectrum = m;
        }
    }

    if (!(TrafficLightsBrightness > WetRoads)) {
        TrafficLightsBrightness = WetRoads;
    }
    if (!(TrafficLightsBrightness > Foggyness)) {
        TrafficLightsBrightness = Foggyness;
    }
    if (!(TrafficLightsBrightness > Rain)) {
        TrafficLightsBrightness = Rain;
    }

    AddRain();

    // Como el original: dos veces FindPlayerPed(-1) y la tarea mas simple; solo cuenta si es TASK_SIMPLE_CLIMB (254)
    bool climbing = false;
    if (CTask* task = FindPlayerPed(-1)->GetIntelligence()->m_TaskMgr.GetSimplestActiveTask()) {
        CTask* task2 = FindPlayerPed(-1)->GetIntelligence()->m_TaskMgr.GetSimplestActiveTask();
        if (task2->GetTaskType() == (eTaskType)254) {
            climbing = true;
        }
    }
    if (IsSunnyType(newW) || IsExtraSunnyType(newW)) {
        if (!CGame::currArea && !CCutsceneMgr::ms_running && (CTimer::m_FrameCounter & 7) == 0 && climbing) {
            FindPlayerPed(-1);
        }
    }

    UpdateInTunnelness();
    m_WeatherAudioEntity.Service();
}

// 0x72B630
void CWeather::UpdateInTunnelness() {
    ZoneScoped;

    float target = 0.0f;
    if (CCullZones::CurrentFlags_Camera & 0x2000) { // Bit 0x2000 de CurrentFlags_Camera: zona de tunel
        target = 1.0f;
        float dist = 100.0f;

        // Como el original: from = posicion de la camara con z = 0; frente (x, y, 0) normalizado
        const CVector& camPos = TheCamera.GetPosition();
        CVector        from{camPos.x, camPos.y, camPos.z};
        from.z = 0.0f;

        CVector fwd;
        if (TheCamera.m_matrix) {
            const CVector& mf = TheCamera.m_matrix->GetForward();
            fwd = CVector{mf.x, mf.y, mf.z};
        } else {
            fwd.x = NegSinX87(TheCamera.m_placement.m_fHeading);
            fwd.y = CosX87(TheCamera.m_placement.m_fHeading);
        }
        fwd.z = 0.0f;
        fwd.Normalise();

        const CVector to{
            fwd.x * 100.0f + from.x,
            from.y + fwd.y * 100.0f,
            fwd.z * 100.0f + from.z,
        };

        // Como el original: los dos puntos del tunel se inicializan la primera vez con guardas en 0xC8143C
        if (!(TunnelInitFlags & 1)) {
            TunnelInitFlags |= 1;
            TunnelPoint1 = CVector{85.0f, -1020.0f, 0.0f};
        }
        if (!(TunnelInitFlags & 2)) {
            TunnelInitFlags |= 2;
            TunnelPoint2 = CVector{1683.0f, -1956.0f, 0.0f};
        }

        const float d1 = CCollision::DistToLine(from, to, TunnelPoint1);
        if (!(100.0f < d1)) {
            dist = d1;
        }
        const float d2 = CCollision::DistToLine(from, to, TunnelPoint2);
        if (!(dist < d2)) {
            dist = d2;
        }
        const float scaled = dist * 0.01f;
        if (!(1.0f < scaled)) {
            target = scaled;
        }
    }

    // Como el original: se acerca al objetivo un paso de TimeStep * 0.01 (d < 0 resta, si no suma)
    const float d    = target - InTunnelness;
    const float step = CTimer::ms_fTimeStep * 0.01f;
    if (std::fabs(d) < step) {
        InTunnelness = target;
    } else if (d < 0.0f) {
        InTunnelness = InTunnelness - step;
    } else {
        InTunnelness = InTunnelness + step;
    }
}

// Based on 0x72A640
eWeatherRegion CWeather::FindWeatherRegion(CVector2D pos) {
    if (pos.x > 1000.0f && pos.y > 910.0f) {
        return WEATHER_REGION_LV;
    }
    if (pos.x > -850.0f && pos.x < 1000.0f && pos.y > 1280.0f) {
        return WEATHER_REGION_DESERT;
    }
    if (pos.x < -1430.0f && pos.y > -580.0f && pos.y < 1430.0f) {
        return WEATHER_REGION_SF;
    }
    if (pos.x > 250.0f && pos.x < 3000.0f && pos.y > -3000.0f && pos.y < -850.0f) {
        return WEATHER_REGION_LA;
    }
    return WEATHER_REGION_DEFAULT;
}

// 0x72A640
void CWeather::UpdateWeatherRegion(CVector* posn) {
    WeatherRegion = FindWeatherRegion(posn ? *posn : TheCamera.GetPosition());
}

// 0x4ABF50
bool CWeather::IsRainy() {
    return Rain >= 0.2f;
}
