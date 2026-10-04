/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTimeCycle
 * Reconstrucción: COMPLETA — 20/20 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 20/20 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TimeCycle.h"
#include "PostEffects.h"
#include "Shadows.h"

auto& TunnelWeather = StaticRef<int>(0x8CDEE0); // 9 = WEATHER_FOGGY_SF, unchanged

void CTimeCycle::InjectHooks() {
    RH_ScopedClass(CTimeCycle);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5BBAC0);
    RH_ScopedInstall(InitForRestart, 0x5601F0);
    RH_ScopedInstall(Shutdown, 0x5601E0);
    RH_ScopedInstall(Update, 0x561760);
    RH_ScopedInstall(StartExtraColour, 0x55FEC0);
    RH_ScopedInstall(StopExtraColour, 0x55FF20);
    RH_ScopedInstall(AddOne, 0x55FF40);
    RH_ScopedInstall(CalcColoursForPoint, 0x5603D0);
    RH_ScopedInstall(FindFarClipForCoors, 0x5616E0);
    RH_ScopedInstall(FindTimeCycleBox, 0x55FFD0);
    RH_ScopedInstall(SetConstantParametersForPostFX, 0x560210);
    RH_ScopedInstall(GetAmbientRed, 0x560330);
    RH_ScopedInstall(GetAmbientGreen, 0x560340);
    RH_ScopedInstall(GetAmbientBlue, 0x560350);
    RH_ScopedInstall(GetAmbientRed_BeforeBrightness, 0x560390);
    RH_ScopedInstall(GetAmbientGreen_BeforeBrightness, 0x5603A0);
    RH_ScopedInstall(GetAmbientBlue_BeforeBrightness, 0x5603B0);
    RH_ScopedInstall(GetAmbientRed_Obj, 0x560360);
    RH_ScopedInstall(GetAmbientGreen_Obj, 0x560370);
    RH_ScopedInstall(GetAmbientBlue_Obj, 0x560380);
}


// Como el original: las conversiones a entero pasan por _ftol2 (0x821B40), truncación de 64 bits.
static int64 Ftol(float value) {
    return (int64)value;
}

// Acceso crudo [hora][clima] a las tablas de colores (el EXE indexa hora * 23 + clima sin comprobar límites).
template<typename A>
static auto& TC(A& arr, int32 hour, int32 weather) {
    using T = std::remove_cvref_t<decltype(arr[0][0])>;
    return reinterpret_cast<T*>(&arr)[hour * NUM_WEATHERS + weather];
}

// Constantes del EXE
static const float s_TcOneSixtieth   = std::bit_cast<float>(0x3C888889u); // 0x859044
static const float s_TcOne3600th     = std::bit_cast<float>(0x3991A2B4u); // 0x864E20
static const float s_TcMaxHours      = std::bit_cast<float>(0x41BFFDF4u); // 0x864E1C (23.999)
static const float s_TcSmogMult      = std::bit_cast<float>(0x3BA3D70Au); // 0x858B4C (0.005)
static const float s_TcSunAngle      = std::bit_cast<float>(0x3B8EFA35u); // 0x864E14 (pi / 720)
static const float s_TcExtraInc      = std::bit_cast<float>(0x3C088889u); // 0x864E0C (1 / 120)
static const float s_TcInv255        = std::bit_cast<float>(0x3B808081u); // 0x859A3C (1 / 255)
static const float s_TcInv300        = std::bit_cast<float>(0x3B5A740Eu); // 0x864E08 (1 / 300)
static const float s_TcThird         = std::bit_cast<float>(0x3EAAAAABu); // 0x859040 (1 / 3)
static const float s_TcCentesimal    = std::bit_cast<float>(0x3C23D70Au); // 0x858C58 (0.01)
static const float s_TcMilli         = std::bit_cast<float>(0x3A83126Fu); // 0x858CDC (0.001)
static const float s_TcSixHundredths = std::bit_cast<float>(0x3D75C28Fu); // 0x859934 (0.06)
static const float s_TcFwdLimit      = std::bit_cast<float>(0xBF666666u); // 0x858CAC (-0.9)
static float s_TcSunX          = std::bit_cast<float>(0x3F333333u); // 0x858CB0 (0.7)
static const float s_TcSunY          = std::bit_cast<float>(0xBF333333u); // -0.7
static float s_TcSunZ          = std::bit_cast<float>(0x3E4CCCCDu); // 0x858CC4 (0.2)
static const float s_TcBrightLow     = std::bit_cast<float>(0x3F4CCCCDu); // 0x858C98 (0.8)

// 0x86A6D0 y 0x86A6C8: ángulos (double) de la dirección de la luz hacia el sol
static double s_TcDirnAngleA = std::bit_cast<double>(0x3FE921FB60000000ull); // pi / 4 (float)
static double s_TcDirnAngleB = std::bit_cast<double>(0xC002D97C80000000ull); // -3 * pi / 4 (float)

// Como el original (0x5BC026): fcos/fsin con precisión interna completa, productos redondeados a float.
static void __declspec(naked) __cdecl CalcDirnLightToSunX87(CVector* out) {
    __asm {
        mov  eax, dword ptr [esp + 4]
        fld  qword ptr [s_TcDirnAngleA]
        fcos
        fld  qword ptr [s_TcDirnAngleB]
        fcos
        fmul st, st(1)
        fstp dword ptr [eax]
        fld  qword ptr [s_TcDirnAngleB]
        fsin
        fmul st, st(1)
        fstp dword ptr [eax + 4]
        fstp st(0)
        fld  qword ptr [s_TcDirnAngleA]
        fsin
        fstp dword ptr [eax + 8]
        ret
    }
}

// Como el original (0x560A80): x = 0.7 + fsin(a), z = 0.2 - fcos(a) con el seno/coseno sin redondear.
static void __declspec(naked) __cdecl CalcVectorToSunX87(float angle, float* outX, float* outZ) {
    __asm {
        fld   dword ptr [esp + 4]
        fld   st(0)
        fsin
        fadd  dword ptr [s_TcSunX]
        mov   eax, dword ptr [esp + 8]
        fstp  dword ptr [eax]
        fcos
        fsubr dword ptr [s_TcSunZ]
        mov   eax, dword ptr [esp + 12]
        fstp  dword ptr [eax]
        ret
    }
}

// 0x5BBAC0
void CTimeCycle::Initialise(bool padFile) {
    CFileMgr::SetDir("DATA");
    auto file = CFileMgr::OpenFile("TIMECYC.DAT", "rb");
    CFileMgr::SetDir("");

    if (!file) { // NOTSA
        NOTSA_LOG_WARN("[CTimeCycle] Failed to open TIMECYC.DAT");
        CFileMgr::CloseFile(file);
        return;
    }

    int32 ambR, ambG, ambB;
    int32 ambObjR, ambObjG, ambObjB;
    int32 dirR, dirG, dirB;
    int32 skyTopR, skyTopG, skyTopB;
    int32 skyBotR, skyBotG, skyBotB;
    int32 sunCoreR, sunCoreG, sunCoreB;
    int32 sunCoronaR, sunCoronaG, sunCoronaB;
    float sunSize, spriteSize, spriteBrightness;
    int32 shadowStrength, lightShadowStrength, poleShadowStrength;
    float farClip, fogStart, lightOnGround;
    int32 lowCloudR, lowCloudG, lowCloudB;
    int32 bottomCloudR, bottomCloudG, bottomCloudB;
    float waterR, waterG, waterB, waterA;
    float postFx1A, postFx1R, postFx1G, postFx1B;
    float postFx2A, postFx2R, postFx2G, postFx2B;
    float cloudAlpha;
    int32 highLightMinIntensity;
    int32 waterFogAlpha;
    float dirMult;

    char* line;
    for (auto w = 0; w < NUM_WEATHERS; w++) {
        for (auto h = 0; h < NUM_HOURS; h++) {
            while (line = CFileLoader::LoadLine(file)) {
                if (line[0] != '/' && line[0] != '\0') {
                    break;
                }
            }

            const auto n = sscanf(line,
                "%d %d %d " // Static ambience color
                "%d %d %d " // Dynamic ambience color
                "%d %d %d " // Direct light color - NOP
                "%d %d %d " // Sky top color
                "%d %d %d " // Sky bottom color
                "%d %d %d " // Sun core color
                "%d %d %d " // Sun corona color
                "%f "       // Sun core size
                "%f "       // Sun corona size
                "%f "       // Sprite brightness
                "%d "       // Pole shading value
                "%d "       // Light shading value
                "%d "       // Pole shading value
                "%f "       // Far clipping offset
                "%f "       // Fog start offset
                "%f "       // Light on ground
                "%d %d %d " // Lower clouds color
                "%d %d %d " // Upper clouds bottom color
                "%f %f %f " // Water color
                "%f "       // Water alpha level
                "%f "       // Color correction 1 alpha
                "%f %f %f " // Color correction 1
                "%f "       // Color correction 2 alpha
                "%f %f %f " // Color correction 2
                "%f "       // Lower clouds alpha level
                "%d "       // Highlight min intensity
                "%d "       // Water fog alpha
                "%f",       // Directional multiplier
                &ambR, &ambG, &ambB,
                &ambObjR, &ambObjG, &ambObjB,
                &dirR, &dirG, &dirB,
                &skyTopR, &skyTopG, &skyTopB,
                &skyBotR, &skyBotG, &skyBotB,
                &sunCoreR, &sunCoreG, &sunCoreB,
                &sunCoronaR, &sunCoronaG, &sunCoronaB,
                &sunSize, &spriteSize, &spriteBrightness,
                &shadowStrength, &lightShadowStrength, &poleShadowStrength,
                &farClip, &fogStart, &lightOnGround,
                &lowCloudR, &lowCloudG, &lowCloudB,
                &bottomCloudR, &bottomCloudG, &bottomCloudB,
                &waterR, &waterG, &waterB, &waterA,
                &postFx1A, &postFx1R, &postFx1G, &postFx1B,
                &postFx2A, &postFx2R, &postFx2G, &postFx2B,
                &cloudAlpha, &highLightMinIntensity, &waterFogAlpha, &dirMult
            );
            if (n < 52) {
                // TODO:
                // R* made a mistake in line 320:
                // instead of the first 3 RGB values, only 255 is specified,
                // which causes the entire line to shift and be read incorrectly
                NOTSA_LOG_WARN("Bad timecyc line: '{}'", line);
            }

            m_nAmbientRed[h][w]   = ambR;
            m_nAmbientGreen[h][w] = ambG;
            m_nAmbientBlue[h][w]  = ambB;

            m_nAmbientRed_Obj[h][w]   = ambObjR;
            m_nAmbientGreen_Obj[h][w] = ambObjG;
            m_nAmbientBlue_Obj[h][w]  = ambObjB;

            // code dir RGB?

            m_nSkyTopRed[h][w]   = skyTopR;
            m_nSkyTopGreen[h][w] = skyTopG;
            m_nSkyTopBlue[h][w]  = skyTopB;

            m_nSkyBottomRed[h][w]   = skyBotR;
            m_nSkyBottomGreen[h][w] = skyBotG;
            m_nSkyBottomBlue[h][w]  = skyBotB;

            m_nSunCoreRed[h][w]   = sunCoreR;
            m_nSunCoreGreen[h][w] = sunCoreG;
            m_nSunCoreBlue[h][w]  = sunCoreB;

            m_nSunCoronaRed[h][w]   = sunCoronaR;
            m_nSunCoronaGreen[h][w] = sunCoronaG;
            m_nSunCoronaBlue[h][w]  = sunCoronaB;

            // Como el original: fmul 10.0, fadd 0.5 y _ftol2 (byte bajo)
            m_fSunSize[h][w]          = (int8)Ftol(sunSize * 10.0f + 0.5f);
            m_fSpriteSize[h][w]       = (int8)Ftol(spriteSize * 10.0f + 0.5f);
            m_fSpriteBrightness[h][w] = (int8)Ftol(spriteBrightness * 10.0f + 0.5f);

            m_nShadowStrength[h][w]      = shadowStrength;
            m_nLightShadowStrength[h][w] = lightShadowStrength;
            m_nPoleShadowStrength[h][w]  = poleShadowStrength;

            m_fFarClip[h][w]  = (int16)Ftol(farClip);
            m_fFogStart[h][w] = (int16)Ftol(fogStart);
            m_fLightsOnGroundBrightness[h][w] = (uint8)Ftol(lightOnGround * 10.0f + 0.5f);

            m_nLowCloudsRed[h][w]   = lowCloudR;
            m_nLowCloudsGreen[h][w] = lowCloudG;
            m_nLowCloudsBlue[h][w]  = lowCloudB;

            m_nFluffyCloudsBottomRed[h][w]   = (uint8)bottomCloudR;
            m_nFluffyCloudsBottomGreen[h][w] = (uint8)bottomCloudG;
            m_nFluffyCloudsBottomBlue[h][w]  = (uint8)bottomCloudB;

            m_fWaterRed[h][w]   = (uint8)Ftol(waterR);
            m_fWaterGreen[h][w] = (uint8)Ftol(waterG);
            m_fWaterBlue[h][w]  = (uint8)Ftol(waterB);
            m_fWaterAlpha[h][w] = (uint8)Ftol(waterA);

            m_fPostFx1Red[h][w]   = (uint8)Ftol(postFx1R);
            m_fPostFx1Green[h][w] = (uint8)Ftol(postFx1G);
            m_fPostFx1Blue[h][w]  = (uint8)Ftol(postFx1B);

            m_fPostFx2Red[h][w]   = (uint8)Ftol(postFx2R);
            m_fPostFx2Green[h][w] = (uint8)Ftol(postFx2G);
            m_fPostFx2Blue[h][w]  = (uint8)Ftol(postFx2B);

            // Como el original: fadd st, st (a + a)
            m_fPostFx1Alpha[h][w] = (uint8)Ftol(postFx1A + postFx1A);
            m_fPostFx2Alpha[h][w] = (uint8)Ftol(postFx2A + postFx2A);

            m_fCloudAlpha[h][w]            = (uint8)Ftol(cloudAlpha);
            m_nHighLightMinIntensity[h][w] = (uint8)highLightMinIntensity;
            m_nWaterFogAlpha[h][w]         = (uint8)waterFogAlpha;
            m_nDirectionalMult[h][w]       = (uint8)Ftol((float)(uint8)Ftol(dirMult) * 100.0f);
        }
    }

    CFileMgr::CloseFile(file);

    // Como el original: cos/sin x87 de los ángulos double del EXE y CVector::Normalise (0x59C910)
    CalcDirnLightToSunX87(&m_vecDirnLightToSun);
    m_vecDirnLightToSun.Normalise();
    m_FogReduction = 0;
    m_bExtraColourOn = false;
}

// 0x5601F0
void CTimeCycle::InitForRestart() {
    StopExtraColour(false);
}

// 0x5601E0
void CTimeCycle::Shutdown() {
    // NOP
}

// 0x561760
void CTimeCycle::Update() {
    ZoneScoped;

    CalcColoursForPoint(TheCamera.GetPosition(), &m_CurrentColours);
}

// 0x55FEC0
void CTimeCycle::StartExtraColour(int32 color, bool bNoExtraColorInterior) {
    m_ExtraColourWeatherType = color / NUM_HOURS + WEATHER_EXTRA_START;
    m_ExtraColour = color % NUM_HOURS;
    m_bExtraColourOn = true;
    m_ExtraColourInter = 0.0f;
    if (!bNoExtraColorInterior) {
        m_ExtraColourInter = 1.0f;
    }
}

// 0x55FF20
void CTimeCycle::StopExtraColour(bool bNoExtraColorInterior) {
    m_bExtraColourOn = false;
    if (!bNoExtraColorInterior) {
        m_ExtraColourInter = 0.0f;
    }
}

// 0x55FF40
void CTimeCycle::AddOne(CBox& box, int16 farClip, int32 extraColor, float strength, float falloff, float lodDistMult) {
    auto& b = m_aBoxes.data()[m_NumBoxes]; // Como el original: sin comprobar el límite de 20
    std::memcpy(&b.Box, &box, sizeof(CBox)); // rep movs
    b.FarClip    = farClip;
    b.ExtraColor = extraColor;
    b.Strength   = strength * s_TcCentesimal; // Como el original: fmul 0.01f en vez de dividir entre 100
    b.Falloff    = falloff;
    if (lodDistMult > 4.0f) { // NaN no se recorta
        lodDistMult = 4.0f;
    }
    b.LodDistMult = (uint8)Ftol(lodDistMult * 32.0f);
    m_NumBoxes++;
}

// 0x5603D0
void CTimeCycle::CalcColoursForPoint(CVector point, CColourSet* set) {
    constexpr auto TimeSamples         = std::to_array({ 0, 5, 6, 7, 12, 19, 20, 22, 24 }); // 0x8CDECC
    constexpr auto GreyValuesDuringDay = std::to_array({ 30, 30, 30, 50, 60, 60, 50, 35 }); // 0x8CDED8

    CTimeCycleBox *lodBoxA, *farBoxA, *farBoxB, *weatherBox;
    float lodBoxA_T, farBoxA_T, farBoxB_T, weatherBox_T;

    // Find LOD box
    FindTimeCycleBox(point, &lodBoxA, &lodBoxA_T, true, false, nullptr);

    // Find far boxes
    FindTimeCycleBox(point, &farBoxA, &farBoxA_T, false, true, nullptr);
    if (farBoxA) {
        FindTimeCycleBox(point, &farBoxB, &farBoxB_T, false, true, farBoxA);
        if (farBoxB && farBoxB->Box.m_vecMax.x - farBoxB->Box.m_vecMin.x > farBoxA->Box.m_vecMax.x - farBoxA->Box.m_vecMin.x) {
            std::swap(farBoxA, farBoxB);
            std::swap(farBoxA_T, farBoxB_T);
        }
    } else {
        farBoxB = nullptr;
    }

    // Find current weather box
    FindTimeCycleBox(point, &weatherBox, &weatherBox_T, false, false, nullptr);

    // 0x5604C8 - Como el original: los segundos se leen como byte (movzx byte 0xB70150)
    float hours = (float)CClock::ms_nGameClockMinutes * s_TcOneSixtieth
                + (float)(uint8)CClock::ms_nGameClockSeconds * s_TcOne3600th
                + (float)CClock::ms_nGameClockHours;
    if (!(hours < s_TcMaxHours)) { // NaN -> 23.999
        hours = s_TcMaxHours;
    }

    // 0x560528 - Find sample index for current hour
    int32 currSampleIdx = 0;
    while (hours >= (float)(TimeSamples[currSampleIdx + 1])) {
        currSampleIdx++;
    }
    const auto nextSampleIdx = (currSampleIdx + 1) % NUM_HOURS;

    const float timeT    = (hours - (float)TimeSamples[currSampleIdx]) / (float)(TimeSamples[currSampleIdx + 1] - TimeSamples[currSampleIdx]);
    const float invTimeT = 1.0f - timeT;

    const float t = CWeather::InterpolationValue;

    // 0x5605B3 - Como el original
    m_nCurrentOldWeatherType = static_cast<int16>(CWeather::OldWeatherType);
    m_nCurrentNewWeatherType = static_cast<int16>(CWeather::NewWeatherType);

    // 0x5605D5
    int32 boxWeather{ WEATHER_UNDEFINED };
    int32 boxHour{};
    if (weatherBox) {
        boxHour    = weatherBox->ExtraColor % 8;
        boxWeather = weatherBox->ExtraColor >= 8
            ? WEATHER_EXTRACOLOURS_2
            : WEATHER_EXTRACOLOURS_1;
    }

    const float invT = 1.0f - t;

    CColourSet currentOld(currSampleIdx, CWeather::OldWeatherType);
    CColourSet nextOld(nextSampleIdx,   CWeather::OldWeatherType);

    CColourSet currentNew(currSampleIdx, CWeather::NewWeatherType);
    CColourSet nextNew(nextSampleIdx,   CWeather::NewWeatherType);

    { // 0x560646 - Smog de LA según la altura de la cámara
        float f = (TheCamera.GetPosition().z - 20.0f) * s_TcSmogMult;
        bool doSmog;
        if (0.0f > f) {
            f      = 0.0f;
            doSmog = false;
        } else if (1.0f < f) {
            f      = 1.0f;
            doSmog = true;
        } else {
            doSmog = f > 0.0f; // Como el original: con f == 0 o NaN no interpola
        }

        if (doSmog) {
            const float invF = 1.0f - f;
            const auto ApplySmog = [&](int32 weather, CColourSet& current, CColourSet& next) {
                int32 clear;
                switch (weather) {
                case WEATHER_EXTRASUNNY_SMOG_LA: clear = WEATHER_EXTRASUNNY_LA; break;
                case WEATHER_SUNNY_SMOG_LA:      clear = WEATHER_SUNNY_LA;      break;
                default:                         return;
                }
                CColourSet set1(currSampleIdx, clear);
                current.Interpolate(&current, &set1, invF, f, false);

                CColourSet set2(nextSampleIdx, clear);
                next.Interpolate(&next, &set2, invF, f, false);
            };
            ApplySmog(CWeather::OldWeatherType, currentOld, nextOld);
            ApplySmog(CWeather::NewWeatherType, currentNew, nextNew);
        }
    }

    { // 0x560857
        CColourSet a, b;
        a.Interpolate(&currentOld, &nextOld, invTimeT, timeT, false);
        b.Interpolate(&currentNew, &nextNew, invTimeT, timeT, false);
        set->Interpolate(&a, &b, invT, t, false);
    }

    // 0x5608C6 - Calculate sky colors
    {
        const float lightMult = (1.0f / CCoronas::LightsMult + 3.0f) * 0.25f;
        const auto Sky = [&](uint16& c) {
            const auto v = (uint16)Ftol((float)c * lightMult);
            c = v < 255 ? v : 255; // Como el original: comparación sin signo de 16 bits
        };
        Sky(set->m_nSkyTopRed);
        Sky(set->m_nSkyTopGreen);
        Sky(set->m_nSkyTopBlue);
        Sky(set->m_nSkyBottomRed);
        Sky(set->m_nSkyBottomGreen);
        Sky(set->m_nSkyBottomBlue);
    }

    if (m_FogReduction) {
        const float reduced = (float)m_FogReduction * 10.15625f;
        if (!(set->m_fFarClip > reduced)) { // NaN -> reduced
            set->m_fFarClip = reduced;
        }
    }

    // 0x560A26
    m_CurrentStoredValue = (m_CurrentStoredValue + 1) & 15;

    // 0x560A44 - Como el original: minutos del día como entero + segundos (word) / 60
    {
        const float minutes = (float)(int32)(CClock::ms_nGameClockHours * 60 + CClock::ms_nGameClockMinutes)
                            + (float)CClock::ms_nGameClockSeconds * s_TcOneSixtieth;
        const float sunAngle = minutes * s_TcSunAngle;
        auto& sun = m_VectorToSun.data()[m_CurrentStoredValue];
        float x, z;
        CalcVectorToSunX87(sunAngle, &x, &z);
        sun.x = x;
        sun.y = s_TcSunY;
        sun.z = z;
        sun.Normalise(); // 0x59C910
    }

    // 0x560AB3 - Como el original: las tablas son bytes y se comparan con -1, así que 255 no salta la mezcla
    if (weatherBox && weatherBox->ExtraColor >= 0) {
        const float boxf    = weatherBox_T * weatherBox->Strength;
        const float invboxf = 1.0f - boxf;

        const auto Word = [&](uint16& c, Colors<uint8>& table) {
            c = (uint16)Ftol((float)c * invboxf + (float)TC(table, boxHour, boxWeather) * boxf);
        };
        const auto Float = [&](float& c, Colors<uint8>& table) {
            c = (float)TC(table, boxHour, boxWeather) * boxf + invboxf * c;
        };

        Word(set->m_nSkyTopRed, m_nSkyTopRed);
        Word(set->m_nSkyTopGreen, m_nSkyTopGreen);
        Word(set->m_nSkyTopBlue, m_nSkyTopBlue);

        Word(set->m_nSkyBottomRed, m_nSkyBottomRed);
        Word(set->m_nSkyBottomGreen, m_nSkyBottomGreen);
        Word(set->m_nSkyBottomBlue, m_nSkyBottomBlue);

        Float(set->m_fWaterRed, m_fWaterRed);
        Float(set->m_fWaterGreen, m_fWaterGreen);
        Float(set->m_fWaterBlue, m_fWaterBlue);
        Float(set->m_fWaterAlpha, m_fWaterAlpha);

        Float(set->m_fAmbientRed, m_nAmbientRed);
        Float(set->m_fAmbientGreen, m_nAmbientGreen);
        Float(set->m_fAmbientBlue, m_nAmbientBlue);

        Float(set->m_fAmbientRed_Obj, m_nAmbientRed_Obj);
        Float(set->m_fAmbientGreen_Obj, m_nAmbientGreen_Obj);
        Float(set->m_fAmbientBlue_Obj, m_nAmbientBlue_Obj);

        // 0x560D08 - Como el original: siempre mezcla con min(tabla, actual)
        if (const auto fc = TC(m_fFarClip, boxHour, boxWeather); fc != -1) {
            float x = (float)fc;
            if (!(x < set->m_fFarClip)) {
                x = set->m_fFarClip;
            }
            set->m_fFarClip = invboxf * set->m_fFarClip + x * boxf;
        }
        if (const auto fs = TC(m_fFogStart, boxHour, boxWeather); fs != -1) {
            set->m_fFogStart = (float)fs * boxf + invboxf * set->m_fFogStart;
        }

        Float(set->m_fPostFx1Red, m_fPostFx1Red);
        Float(set->m_fPostFx1Green, m_fPostFx1Green);
        Float(set->m_fPostFx1Blue, m_fPostFx1Blue);
        Float(set->m_fPostFx1Alpha, m_fPostFx1Alpha);

        Float(set->m_fPostFx2Red, m_fPostFx2Red);
        Float(set->m_fPostFx2Green, m_fPostFx2Green);
        Float(set->m_fPostFx2Blue, m_fPostFx2Blue);
        Float(set->m_fPostFx2Alpha, m_fPostFx2Alpha);
    }

    if (lodBoxA) {
        set->m_fLodDistMult = (float)lodBoxA->LodDistMult * 0.03125f * lodBoxA_T + (1.0f - lodBoxA_T) * set->m_fLodDistMult;
    }

    // 0x560EA5 - Como el original: siempre mezcla con min(caja, actual)
    const auto FarBox = [&](const CTimeCycleBox* box, float boxT) {
        float x = (float)box->FarClip;
        if (!(x < set->m_fFarClip)) {
            x = set->m_fFarClip;
        }
        set->m_fFarClip = (1.0f - boxT) * set->m_fFarClip + x * boxT;
    };
    if (farBoxA) {
        FarBox(farBoxA, farBoxA_T);
    }
    if (farBoxB) {
        FarBox(farBoxB, farBoxB_T);
    }

    // 0x560F1B - Como el original: solo se recorta por el lado hacia el que avanza
    const float inc = CTimer::GetTimeStep() * s_TcExtraInc;
    if (m_bExtraColourOn) {
        const float v = inc + m_ExtraColourInter;
        m_ExtraColourInter = 1.0f < v ? 1.0f : v;
    } else {
        const float v = m_ExtraColourInter - inc;
        m_ExtraColourInter = 0.0f > v ? 0.0f : v;
    }

    if (m_ExtraColourInter > 0.0f) {
        const float invInter = 1.0f - m_ExtraColourInter;
        CColourSet extra(m_ExtraColour, m_ExtraColourWeatherType);
        const bool ignoreSky = TC(m_nSkyTopRed, m_ExtraColour, m_ExtraColourWeatherType) == 0
                            && TC(m_nSkyTopGreen, m_ExtraColour, m_ExtraColourWeatherType) == 0
                            && TC(m_nSkyTopBlue, m_ExtraColour, m_ExtraColourWeatherType) == 0;
        set->Interpolate(set, &extra, invInter, m_ExtraColourInter, ignoreSky);
    }

    if (CWeather::UnderWaterness > 0.0f) {
        CColourSet current(currSampleIdx, WEATHER_UNDERWATER);
        CColourSet next(nextSampleIdx, WEATHER_UNDERWATER);
        CColourSet tmp;
        tmp.Interpolate(&current, &next, invTimeT, timeT, false);
        set->Interpolate(set, &tmp, 1.0f - CWeather::UnderWaterness, CWeather::UnderWaterness, false);
    }

    // 0x561124 - Como el original: el cielo se ignora según la entrada del túnel, no la del color extra
    if (CWeather::InTunnelness > 0.0f) {
        const float invTunnel = 1.0f - CWeather::InTunnelness;
        const int32 tunnelHour    = TunnelWeather % NUM_HOURS;
        const int32 tunnelWeather = TunnelWeather / NUM_HOURS + WEATHER_EXTRA_START;
        CColourSet tunnel(tunnelHour, tunnelWeather);
        const bool ignoreSky = TC(m_nSkyTopRed, tunnelHour, tunnelWeather) == 0
                            && TC(m_nSkyTopGreen, tunnelHour, tunnelWeather) == 0
                            && TC(m_nSkyTopBlue, tunnelHour, tunnelWeather) == 0;
        set->Interpolate(set, &tunnel, invTunnel, CWeather::InTunnelness, ignoreSky);
    }

    // 0x5611C2 - Como el original: trunca con _ftol2 y multiplica por 1/255
    const auto Ambient = [](float& c) { c = (float)(int32)Ftol(c) * s_TcInv255; };
    Ambient(set->m_fAmbientRed);
    Ambient(set->m_fAmbientGreen);
    Ambient(set->m_fAmbientBlue);
    Ambient(set->m_fAmbientRed_Obj);
    Ambient(set->m_fAmbientGreen_Obj);
    Ambient(set->m_fAmbientBlue_Obj);

    // 0x5612AA
    CShadows::CalcPedShadowValues(
        m_VectorToSun[m_CurrentStoredValue],
        m_fShadowFrontX[m_CurrentStoredValue],
        m_fShadowFrontY[m_CurrentStoredValue],
        m_fShadowSideX[m_CurrentStoredValue],
        m_fShadowSideY[m_CurrentStoredValue],
        m_fShadowDisplacementX[m_CurrentStoredValue],
        m_fShadowDisplacementY[m_CurrentStoredValue]
    );

    if (TheCamera.m_mCameraMatrix.GetForward().z < s_TcFwdLimit || !CWeather::bScriptsForceRain
        && (CCullZones::PlayerNoRain() || CCullZones::CamNoRain() || CCutsceneMgr::ms_running)) {
        m_FogReduction = std::min(m_FogReduction + 1, 64);
    } else {
        m_FogReduction = std::max(m_FogReduction - 1, 0);
    }

    // 0x56131B - Como el original: entra con z >= 200 (o NaN) y multiplica por 1/300
    const float camZ = TheCamera.GetPosition().z;
    if (!(camZ < 200.0f)) {
        if (camZ > 500.0f) {
            if (!(set->m_fFarClip < 1000.0f)) {
                set->m_fFarClip = 1000.0f;
            }
        } else if (set->m_fFarClip > 1000.0f) {
            const float altT = (camZ - 200.0f) * s_TcInv300;
            set->m_fFarClip = (1.0f - altT) * set->m_fFarClip + altT * 1000.0f;
        }
    }

    // 0x5613A6 - Como el original: el gris se trunca a byte antes de mezclarlo
    {
        const auto grey = (uint8)Ftol((float)GreyValuesDuringDay[currSampleIdx] * invTimeT + (float)GreyValuesDuringDay[nextSampleIdx] * timeT);
        const float greyPart = (1.0f - CWeather::UnderWaterness) * (float)grey;
        m_BelowHorizonGrey.red   = (uint8)Ftol((float)m_CurrentColours.m_nSkyBottomRed * CWeather::UnderWaterness + greyPart);
        m_BelowHorizonGrey.green = (uint8)Ftol((float)m_CurrentColours.m_nSkyBottomGreen * CWeather::UnderWaterness + greyPart);
        m_BelowHorizonGrey.blue  = (uint8)Ftol((float)m_CurrentColours.m_nSkyBottomBlue * CWeather::UnderWaterness + greyPart);
    }

    set->m_fAmbientBeforeBrightnessRed   = set->m_fAmbientRed;
    set->m_fAmbientBeforeBrightnessGreen = set->m_fAmbientGreen;
    set->m_fAmbientBeforeBrightnessBlue  = set->m_fAmbientBlue;

    // Como el original: max = (R > G ? R : G) > B ? ... : B
    const auto MaxAmbient = [&] {
        float m = set->m_fAmbientRed > set->m_fAmbientGreen ? set->m_fAmbientRed : set->m_fAmbientGreen;
        if (!(m > set->m_fAmbientBlue)) {
            m = set->m_fAmbientBlue;
        }
        return m;
    };

    // 0x561468
    float f;
    const auto brightness = (float)FrontEndMenuManager.m_PrefsBrightness;
    if (!(brightness < 256.0f)) {
        f = (brightness - 256.0f) * 0.0078125f + 1.0f;
        const float m   = MaxAmbient();
        const float add = m * f - m;
        set->m_fAmbientRed   = add + set->m_fAmbientRed;
        set->m_fAmbientGreen = add + set->m_fAmbientGreen;
        set->m_fAmbientBlue  = add + set->m_fAmbientBlue;
    } else {
        f = brightness * 0.00390625f * s_TcBrightLow + s_TcSunZ;
        set->m_fAmbientRed   = f * set->m_fAmbientRed;
        set->m_fAmbientGreen = f * set->m_fAmbientGreen;
        set->m_fAmbientBlue  = f * set->m_fAmbientBlue;
    }

    if (f > 1.0f) {
        f = (f - 1.0f) * s_TcSixHundredths;
        float max = MaxAmbient();
        const float r = set->m_fAmbientRed;
        const float g = set->m_fAmbientGreen;
        const float b = set->m_fAmbientBlue;
        if (max == 0.0f) {
            max = s_TcMilli;
            set->m_fAmbientRed   = s_TcMilli;
            set->m_fAmbientGreen = s_TcMilli;
            set->m_fAmbientBlue  = s_TcMilli;
        }
        if (f > max) {
            f /= max;
            set->m_fAmbientRed   = f * set->m_fAmbientRed;
            set->m_fAmbientGreen = f * set->m_fAmbientGreen;
            set->m_fAmbientBlue  = f * set->m_fAmbientBlue;
        }
        m_BrightnessAddedToAmbientRed   = set->m_fAmbientRed   - r;
        m_BrightnessAddedToAmbientGreen = set->m_fAmbientGreen - g;
        m_BrightnessAddedToAmbientBlue  = set->m_fAmbientBlue  - b;
    }

    // 0x5615D2 - Como el original: distancia fuera de los límites del mundo
    float outside = 0.0f;
    if (point.x < -3000.0f) { // todo: WORLD_BOUNDS
        outside = -3000.0f - point.x;
    } else if (point.x > 3000.0f) {
        outside = point.x - 3000.0f;
    }

    if (point.y < -3000.0f) {
        outside = outside - (point.y + 3000.0f);
    } else if (point.y > 3000.0f) {
        outside = (point.y - 3000.0f) + outside;
    }

    if (outside >= 1000.0f) {
        set->m_fLodDistMult = set->m_fLodDistMult + set->m_fLodDistMult;
    } else if (outside >= 0.0f) {
        set->m_fLodDistMult = (outside * s_TcMilli + 1.0f) * set->m_fLodDistMult;
    }

    SetConstantParametersForPostFX();
}

// 0x5616E0
float CTimeCycle::FindFarClipForCoors(CVector cameraPos) {
    CColourSet set; // Como el original: sin inicializar
    bool extraOn = m_bExtraColourOn != 0;
    float extraInter = m_ExtraColourInter;
    m_bExtraColourOn = 0;
    m_ExtraColourInter = 0.0f;
    CalcColoursForPoint(cameraPos, &set);
    m_bExtraColourOn = extraOn;
    m_ExtraColourInter = extraInter;
    return set.m_fFarClip;
}

// 0x55FFD0
void CTimeCycle::FindTimeCycleBox(
    CVector         pos,
    CTimeCycleBox** curr,
    float*          interpolation,
    bool            isLOD,
    bool            isFarClip,
    CTimeCycleBox*  ignored
) {
    *curr          = nullptr;
    *interpolation = 0.0f;

    for (int32 i = 0; i < (int32)m_NumBoxes; i++) { // Como el original: comparación con signo
        auto& v = m_aBoxes.data()[i];
        if (isLOD && v.LodDistMult == 32) { // 0x560013
            continue;
        }
        if (isFarClip && !v.FarClip) { // 0x560038
            continue;
        }
        if (ignored == &v) {
            continue;
        }

        // Como el original: el punto debe estar dentro de la caja ampliada por `Falloff` (z por Falloff * 1/3);
        // las comparaciones con NaN descartan la caja
        const float falloff  = v.Falloff;
        const float falloffZ = falloff * s_TcThird;
        if (!(v.Box.m_vecMin.x - falloff <= pos.x) || !(v.Box.m_vecMin.y - falloff <= pos.y) || !(v.Box.m_vecMin.z - falloffZ <= pos.z)) {
            continue;
        }
        if (!(falloff + v.Box.m_vecMax.x >= pos.x) || !(falloff + v.Box.m_vecMax.y >= pos.y) || !(falloffZ + v.Box.m_vecMax.z >= pos.z)) {
            continue;
        }

        // 0x5600E6 - Distancia a la caja por eje, como el original
        const auto Axis = [](float p, float mn, float mx) {
            if (p > mx) {
                return p - mx;
            }
            if (p < mn) {
                return mn - p;
            }
            return 0.0f;
        };
        const float dx = Axis(pos.x, v.Box.m_vecMin.x, v.Box.m_vecMax.x);
        const float dy = Axis(pos.y, v.Box.m_vecMin.y, v.Box.m_vecMax.y);
        const float dz = Axis(pos.z, v.Box.m_vecMin.z, v.Box.m_vecMax.z) * 3.0f;
        const float dist = std::sqrt(dz * dz + dy * dy + dx * dx); // 0x560188: orden z, y, x
        if (dist <= 0.0f) { // Point inside the box
            *curr          = &v;
            *interpolation = 1.0f;
        } else { // Point not inside the box, but within `FallOff` (o NaN)
            const float t = 1.0f - dist / falloff;
            if (t > *interpolation) {
                *curr          = &v;
                *interpolation = t;
            }
        }
    }
}

// 0x560210
void CTimeCycle::SetConstantParametersForPostFX() {
    if (!CPostEffects::IsVisionFXActive())
        return;

    if (CPostEffects::m_bNightVision) {
        m_CurrentColours.m_nShadowStrength = 0;
        m_CurrentColours.m_nLightShadowStrength = 0;
        m_CurrentColours.m_nPoleShadowStrength = 0;
        m_CurrentColours.m_fAmbientRed = 0.0f;
        m_CurrentColours.m_fAmbientGreen = 0.4f;
        m_CurrentColours.m_fAmbientBlue = 0.0f;
        m_CurrentColours.m_fAmbientRed_Obj = 0.0f;
        m_CurrentColours.m_fAmbientGreen_Obj = 0.4f;
        m_CurrentColours.m_fAmbientBlue_Obj = 0.0f;
        m_CurrentColours.m_nSkyTopRed = 0;
        m_CurrentColours.m_nSkyTopGreen = 128;
        m_CurrentColours.m_nSkyTopBlue = 0;
        m_CurrentColours.m_nSkyBottomRed = 0;
        m_CurrentColours.m_nSkyBottomGreen = 128;
        m_CurrentColours.m_nSkyBottomBlue = 0;
    }

    if (CPostEffects::m_bInfraredVision) {
        m_CurrentColours.m_nShadowStrength = 0;
        m_CurrentColours.m_nLightShadowStrength = 0;
        m_CurrentColours.m_nPoleShadowStrength = 0;
        m_CurrentColours.m_fLightsOnGroundBrightness = 0.0f;
        m_CurrentColours.m_nHighLightMinIntensity = 0;
        m_CurrentColours.m_nWaterFogAlpha = 0;
        m_CurrentColours.m_fAmbientRed = 0.0f;
        m_CurrentColours.m_fAmbientGreen = 0.0f;
        m_CurrentColours.m_fAmbientBlue = 1.0f;
        m_CurrentColours.m_fAmbientRed_Obj = 0.0f;
        m_CurrentColours.m_fAmbientGreen_Obj = 0.0f;
        m_CurrentColours.m_fAmbientBlue_Obj = 1.0f;
        m_CurrentColours.m_nSkyTopRed = 0;
        m_CurrentColours.m_nSkyTopGreen = 0;
        m_CurrentColours.m_nSkyTopBlue = 128;
        m_CurrentColours.m_nSkyBottomRed = 0;
        m_CurrentColours.m_nSkyBottomGreen = 0;
        m_CurrentColours.m_nSkyBottomBlue = 128;
    }
}

float CTimeCycle::GetAmbientRed()   { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientRed; }   // 0x560330
float CTimeCycle::GetAmbientGreen() { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientGreen; } // 0x560340
float CTimeCycle::GetAmbientBlue()  { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientBlue; }  // 0x560350

float CTimeCycle::GetAmbientRed_Obj()   { return m_CurrentColours.m_fAmbientRed_Obj; }   // 0x560360
float CTimeCycle::GetAmbientGreen_Obj() { return m_CurrentColours.m_fAmbientGreen_Obj; } // 0x560370
float CTimeCycle::GetAmbientBlue_Obj()  { return m_CurrentColours.m_fAmbientBlue_Obj; }  // 0x560380

float CTimeCycle::GetAmbientRed_BeforeBrightness()   { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientBeforeBrightnessRed; }   // 0x560390
float CTimeCycle::GetAmbientGreen_BeforeBrightness() { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientBeforeBrightnessGreen; } // 0x5603A0
float CTimeCycle::GetAmbientBlue_BeforeBrightness()  { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientBeforeBrightnessBlue; }  // 0x5603B0
