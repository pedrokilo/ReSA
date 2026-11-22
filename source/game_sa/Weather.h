/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
#include "DatosExe/DataBssG3.h"
#include "DatosExe/DataBssE3.h"
#include "DatosExe/DataBssE2.h"
#include "DatosExe/DataBssE1.h"
#include "DatosExe/DataInicializadaB.h"

#include "Vector.h"
#include "AEWeatherAudioEntity.h"
#include "eWeatherType.h"

enum eWeatherRegion : int16 {
    WEATHER_REGION_DEFAULT = 0,
    WEATHER_REGION_LA = 1,
    WEATHER_REGION_SF = 2,
    WEATHER_REGION_LV = 3,
    WEATHER_REGION_DESERT = 4
};

class CWeather {
public:
    static inline auto& TrafficLightsBrightness = DatosExe::Bss::bC812A8;
    static inline auto& bScriptsForceRain = DatosExe::Bss::bC812AC;
    static inline auto& Earthquake = StaticRef<float>(0xC81340);
    static inline auto& CurrentRainParticleStrength = DatosExe::Bss::bC812B0;
    static inline auto& LightningStartY = DatosExe::Bss::bC812B4; // only initialized (0), not used
    static inline auto& LightningStartX = DatosExe::Bss::bC812B8; // only initialized (0), not used
    static inline auto& LightningFlashLastChange = DatosExe::Bss::bC812BC;
    static inline auto& WhenToPlayLightningSound = DatosExe::Bss::bC812C0;
    static inline auto& LightningDuration = DatosExe::Bss::bC812C4;
    static inline auto& LightningStart = DatosExe::Bss::bC812C8; // frame number
    static inline auto& LightningFlash = DatosExe::Bss::bC812CC;
    static inline auto& LightningBurst = DatosExe::Bss::bC812CD;
    static inline auto& HeadLightsSpectrum = DatosExe::Bss::bC812D0;
    static inline auto& WaterFogFXControl = StaticRef<float>(0xC81338);
    static inline auto& HeatHazeFXControl = DatosExe::Bss::bC812D8;
    static inline auto& HeatHaze = DatosExe::Bss::bC812DC;
    static inline auto& SunGlare = DatosExe::Bss::bC812E0;
    static inline auto& Rainbow = DatosExe::Bss::bC812E4;
    static inline auto& Wavyness = DatosExe::Bss::bC812E8;
    static inline auto& WindClipped = DatosExe::Bss::bC812EC;
    static inline auto& WindDir = StaticRef<CVector>(0xC813E0);
    static inline auto& Wind = DatosExe::Bss::bC812F0;
    static inline auto& Sandstorm = DatosExe::Bss::bC812F4;
    static inline auto& Rain = StaticRef<float>(0xC81324);
    static inline auto& InTunnelness = StaticRef<float>(0xC81334);
    static inline auto& WaterDepth = StaticRef<float>(0xC81330);
    static inline auto& UnderWaterness = StaticRef<float>(0xC8132C);
    static inline auto& ExtraSunnyness = DatosExe::Bss::bC812F8;
    static inline auto& Foggyness_SF = DatosExe::Bss::bC812FC;
    static inline auto& Foggyness = DatosExe::Bss::bC81300;
    static inline auto& CloudCoverage = DatosExe::Bss::bC81304;
    static inline auto& WetRoads = DatosExe::Bss::bC81308;
    static inline auto& InterpolationValue = DatosExe::Bss::bC8130C;
    static inline auto& WeatherTypeInList = DatosExe::Bss::bC81310;
    static inline auto& WeatherRegion = StaticRef<eWeatherRegion>(0xC81314);
    static inline auto& ForcedWeatherType = StaticRef<eWeatherType>(0xC81318);
    static inline auto& NewWeatherType = StaticRef<eWeatherType>(0xC8131C);
    static inline auto& OldWeatherType = StaticRef<eWeatherType>(0xC81320);
    static inline auto& m_WeatherAudioEntity = StaticRef<CAEWeatherAudioEntity>(0xC81360);
    static inline auto& StreamAfterRainTimer = DatosExe::Data::v8D5EAC;
    static inline auto& StreamAfterRainFlag = StaticRef<int32>(0xC81328); // != 0 mientras ha llovido hace poco
    static inline auto& RainFXAlpha = DatosExe::Bss::bC81410;        // alfa del efecto de particulas de lluvia

    // in entity.cpp:

    static std::array<float, 16> saTreeWindOffsets; // orig WindTabel
    static std::array<float, 32> saBannerWindOffsets; // orig BannerWindTabel

public:
    static void InjectHooks();

    static void Init();
    static void AddRain();
    static void AddSandStormParticles();
    static const int8* FindWeatherTypesList(); // Como el original: tabla de 64 bytes (int8) por tipo de tiempo
    static void ForceWeather(eWeatherType weatherType);
    static void ForceWeatherNow(eWeatherType weatherType);
    static bool ForecastWeather(eWeatherType weatherType, int32 numSteps);
    static void ReleaseWeather();
    static void RenderRainStreaks();
    static void SetWeatherToAppropriateTypeNow();
    static void Update();
    static void UpdateInTunnelness();
    /*!
    * @notsa
    * @details Based on code @ `0x72A640`
    * @return The corresponding weather region at a given 2D position, or `WEATHER_REGION_DEFAULT` if no specific region was found
    */
    static eWeatherRegion FindWeatherRegion(CVector2D pos);
    static void UpdateWeatherRegion(CVector* posn);
    static bool IsRainy();

    static bool IsUnderWater() { return UnderWaterness <= 0.0f; } // NOTSA
};
