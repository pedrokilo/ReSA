/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CClouds
 * Reconstrucción: COMPLETA — 21/21 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 21/21 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "StdInc.h"
#include "Clouds.h"
#include "X87Math.h"
#include "Camera.h"
#include "PostEffects.h"
#include "Sprite.h"
#include "Coronas.h"
#include "CullZones.h"
#include "TxdStore.h"
#include "General.h"
#include "Draw.h"
#include "Clock.h"
#include "Timer.h"
#include "TimeCycle.h"
#include "BrightLights.h"
#include "Game.h"
#include "Scene.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

auto& CurrentFogIntensity = StaticRef<float>(0x8D5798);

auto& gpMoonMask = StaticRef<RwTexture*>(0xC6AA74);
auto& gpCloudTex = StaticRef<RwTexture*>(0xC6AA78);
auto& gpCloudMaskTex = StaticRef<RwTexture*>(0xC6AA7C);

namespace {
// Como el original: _ftol2 (0x821B40) trunca a 64 bits y se usa la parte baja.
int32 CloudsFtol(float value) {
    return static_cast<int32>(static_cast<int64>(value));
}

// Como el original (0x41CC70, CPlaceable de la cámara): derecha de la matriz o (cos, sin, 0) con fsin/fcos del x87.
CVector CloudsCameraRight() {
    if (const auto* mat = TheCamera.m_matrix) {
        return mat->GetRight();
    }
    const float heading = TheCamera.m_placement.m_fHeading;
    return CVector{ TrigSimpleCosCpp(heading), TrigSimpleSinCpp(heading), 0.0f };
}
} // namespace

void CClouds::InjectHooks() {
    RH_ScopedClass(CClouds);
    RH_ScopedCategoryGlobal();

    // Clouds
    RH_ScopedInstall(Init, 0x7138D0);
    RH_ScopedInstall(Update, 0x712FF0);
    RH_ScopedInstall(Shutdown, 0x712FA0);
    RH_ScopedInstall(SetUpOneSkyPoly, 0x713060);
    RH_ScopedInstall(Render, 0x713950);
    RH_ScopedInstall(RenderSkyPolys, 0x714650);
    RH_ScopedInstall(RenderBottomFromHeight, 0x7154B0);

    // Moving fog
    RH_ScopedInstall(MovingFogInit, 0x713660);
    RH_ScopedInstall(MovingFog_Create, 0x713760);
    RH_ScopedInstall(MovingFog_Delete, 0x713730);
    RH_ScopedInstall(MovingFog_Update, 0x716B10);
    RH_ScopedInstall(MovingFog_GetFXIntensity, 0x7136D0);
    RH_ScopedInstall(MovingFog_GetWind, 0x7136E0);
    RH_ScopedInstall(MovingFog_GetFirstFreeSlot, 0x713710);
    RH_ScopedInstall(MovingFogRender, 0x716C90);

    // Volumetric clouds
    RH_ScopedInstall(VolumetricCloudsInit, 0x7131C0);
    RH_ScopedInstall(VolumetricClouds_Create, 0x715F40);
    RH_ScopedInstall(VolumetricClouds_Delete, 0x7135F0);
    RH_ScopedInstall(VolumetricClouds_GetFirstFreeSlot, 0x7135C0);
    RH_ScopedInstall(VolumetricCloudsGetMaxDistance, 0x713630);
    RH_ScopedInstall(VolumetricCloudsRender, 0x716380);
}

// 0x7138D0
void CClouds::Init() {
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));
    gpCloudTex = RwTextureRead("cloud1", nullptr);
    gpCloudMaskTex = RwTextureRead("cloudmasked", nullptr);
    gpMoonMask = RwTextureRead("lunar", "lunarm");
    ms_vc.texture = RwTextureRead("cloudhigh", "cloudhighm");
    CTxdStore::PopCurrentTxd();
    CloudRotation = 0.0f;
    VolumetricCloudsInit();
    MovingFogInit();
}

// 0x712FF0
void CClouds::Update() {
    ZoneScoped;

    // Como el original: fsin del x87 y la rotación individual se suma como flotante (dword sin signo) y se trunca con _ftol2.
    CloudRotation = TrigSimpleSinMulCpp(TheCamera.m_fOrientation - 0.85f, CWeather::Wind) * 0.001f + CloudRotation;
    const float delta = (CTimer::GetTimeStep() * CWeather::Wind * 0.5f + 0.3f) * 60.0f;
    IndividualRotation = CloudsFtol(UnsignedPlusFloatCpp(IndividualRotation, delta));
}

// 0x712FA0
void CClouds::Shutdown() {
    RwTextureDestroy(gpCloudTex);
    gpCloudTex = nullptr;

    RwTextureDestroy(gpCloudMaskTex);
    gpCloudMaskTex = nullptr;

    RwTextureDestroy(ms_vc.texture);
    ms_vc.texture = nullptr;
}

// 0x713060
void CClouds::SetUpOneSkyPoly(CVector vert1pos, CVector vert2pos, CVector vert3pos, CVector vert4pos, uint8 topRed, uint8 topGreen, uint8 topBlue,
                              uint8 bottomRed, uint8 bottomGreen, uint8 bottomBlue) {
    // Como el original: los contadores se usan como dwords completos (sin truncar a 16 bits).
    const int32 uiStartVertex = uiTempBufferVerticesStored;
    const int32 iStartIndex = uiTempBufferIndicesStored;

    RwIm3DVertexSetRGBA(&TempBufferVertices.m_3d[uiStartVertex + 0], topRed, topGreen, topBlue, 0xFF);
    RwIm3DVertexSetPos(&TempBufferVertices.m_3d[uiStartVertex + 0], vert1pos.x, vert1pos.y, vert1pos.z);
    RwIm3DVertexSetU(&TempBufferVertices.m_3d[uiStartVertex + 0], 0.f);
    RwIm3DVertexSetV(&TempBufferVertices.m_3d[uiStartVertex + 0], 0.f);

    RwIm3DVertexSetRGBA(&TempBufferVertices.m_3d[uiStartVertex + 1], topRed, topGreen, topBlue, 0xFF);
    RwIm3DVertexSetPos(&TempBufferVertices.m_3d[uiStartVertex + 1], vert2pos.x, vert2pos.y, vert2pos.z);
    RwIm3DVertexSetU(&TempBufferVertices.m_3d[uiStartVertex + 1], 0.f);
    RwIm3DVertexSetV(&TempBufferVertices.m_3d[uiStartVertex + 1], 0.f);

    RwIm3DVertexSetRGBA(&TempBufferVertices.m_3d[uiStartVertex + 2], bottomRed, bottomGreen, bottomBlue, 0xFF);
    RwIm3DVertexSetPos(&TempBufferVertices.m_3d[uiStartVertex + 2], vert3pos.x, vert3pos.y, vert3pos.z);
    RwIm3DVertexSetU(&TempBufferVertices.m_3d[uiStartVertex + 2], 0.f);
    RwIm3DVertexSetV(&TempBufferVertices.m_3d[uiStartVertex + 2], 0.f);

    RwIm3DVertexSetRGBA(&TempBufferVertices.m_3d[uiStartVertex + 3], bottomRed, bottomGreen, bottomBlue, 0xFF);
    RwIm3DVertexSetPos(&TempBufferVertices.m_3d[uiStartVertex + 3], vert4pos.x, vert4pos.y, vert4pos.z);
    RwIm3DVertexSetU(&TempBufferVertices.m_3d[uiStartVertex + 3], 0.f);
    RwIm3DVertexSetV(&TempBufferVertices.m_3d[uiStartVertex + 3], 0.f);

    aTempBufferIndices[iStartIndex + 0] = static_cast<RxVertexIndex>(uiStartVertex);
    aTempBufferIndices[iStartIndex + 1] = static_cast<RxVertexIndex>(uiStartVertex + 2);
    aTempBufferIndices[iStartIndex + 2] = static_cast<RxVertexIndex>(uiStartVertex + 1);
    aTempBufferIndices[iStartIndex + 3] = static_cast<RxVertexIndex>(uiStartVertex + 1);
    aTempBufferIndices[iStartIndex + 4] = static_cast<RxVertexIndex>(uiStartVertex + 2);
    aTempBufferIndices[iStartIndex + 5] = static_cast<RxVertexIndex>(uiStartVertex + 3);

    uiTempBufferIndicesStored += 6;
    uiTempBufferVerticesStored += 4;
}

// 0x713660
void CClouds::MovingFogInit() {
    // Como el original: solo se limpian los 350 bytes de huecos (rep stos); el resto de la tabla conserva su contenido.
    ms_mf.m_nPrimIndices = { 0, 1, 2, 0, 2, 3 };
    rng::fill(ms_mf.m_bFogSlots, false);
    ms_mf.m_vecWind = CVector(0.06f, 0.06f, 0.0f);
}

// 0x713760
void CClouds::MovingFog_Create(CVector* posn) {
    int32 slotId = MovingFog_GetFirstFreeSlot();
    if (slotId == -1)
        return;

    ms_mf.m_vecPosn[slotId] = CVector{
        CGeneral::GetRandomNumberInRange(-58.0f, 58.0f),
        CGeneral::GetRandomNumberInRange(-58.0f, 58.0f),
        CGeneral::GetRandomNumberInRange(-5.0f,  5.0f)
    };
    ms_mf.m_vecPosn[slotId] += *posn;
    ms_mf.m_fSize[slotId] = CGeneral::GetRandomNumberInRange(4.0f, 10.0f);
    ms_mf.m_fIntensity[slotId] = 1.0f;
    ms_mf.m_fMaxIntensity[slotId] = CGeneral::GetRandomNumberInRange(8.0f, 20.0f);
    ms_mf.m_Speed[slotId] = CGeneral::GetRandomNumberInRange(0.5f, 1.2f);
    ms_mf.m_bFogSlots[slotId] = true;
}

// 0x713730
void CClouds::MovingFog_Delete(int32 fogSlotIndex) {
    fogSlotIndex = std::clamp(fogSlotIndex, 0, MAX_MOVING_FOG - 1);
    ms_mf.m_bFogSlots[fogSlotIndex] = false;
}

// 0x716B10
void CClouds::MovingFog_Update() {
    if (CloudsEqualOrderedCpp(0.f, MovingFog_GetFXIntensity()))
        return;

    // Como el original: la cámara y el viento se copian una vez antes del bucle.
    CVector camPos = TheCamera.GetPosition();
    const CVector wind = ms_mf.m_vecWind;
    for (int32 i = 0; i < MAX_MOVING_FOG; i++) {
        CVector& fogPosn = ms_mf.m_vecPosn[i];
        // Como el original: la distancia usa la posición anterior al desplazamiento por el viento.
        const float dx = fogPosn.x - camPos.x;
        const float dy = fogPosn.y - camPos.y;
        const float dz = fogPosn.z - camPos.z;

        if (!ms_mf.m_bFogSlots[i]) {
            MovingFog_Create(&camPos);
            continue;
        }

        fogPosn.x = wind.x * ms_mf.m_Speed[i] + fogPosn.x;
        fogPosn.y = wind.y * ms_mf.m_Speed[i] + fogPosn.y;

        // Como el original: magnitud sumada en el orden z, y, x; NaN cuenta como cerca.
        const float dist = std::sqrt(dz * dz + dy * dy + dx * dx);
        if (!(60.f < dist)) {
            ms_mf.m_fIntensity[i] = CTimer::GetTimeStep() + ms_mf.m_fIntensity[i];
            if (CloudsLessOrderedCpp(ms_mf.m_fMaxIntensity[i], ms_mf.m_fIntensity[i])) {
                ms_mf.m_fIntensity[i] = ms_mf.m_fMaxIntensity[i];
            }
        } else {
            ms_mf.m_fIntensity[i] = ms_mf.m_fIntensity[i] - CTimer::GetTimeStep();
            if (0.f >= ms_mf.m_fIntensity[i]) {
                MovingFog_Delete(i);
            }
        }
    }
}

// 0x7136D0
float CClouds::MovingFog_GetFXIntensity() {
    return CWeather::Foggyness_SF;
}

// 0x7136E0
CVector CClouds::MovingFog_GetWind() {
    return ms_mf.m_vecWind;
}

// 0x713710
int32 CClouds::MovingFog_GetFirstFreeSlot() {
    int32 result = 0;
    while (ms_mf.m_bFogSlots[result]) {
        if (++result >= MAX_MOVING_FOG)
            return -1;
    }
    return result;
}

// 0x716C90
void CClouds::MovingFogRender() {
    ZoneScoped;

    if (CloudsEqualOrderedCpp(0.f, MovingFog_GetFXIntensity()))
        return;

    // Como el original: se sale si el área actual o la del jugador no es el mundo exterior.
    if (static_cast<int32>(CGame::currArea) != 0 || int(FindPlayerPed()->GetAreaCode()) != AREA_CODE_NORMAL_WORLD)
        return;

    // Adjust fog intensity
    {
        // Como el original: paso = timestep * (1/300) (0x864E08), no una división.
        const float step = CTimer::GetTimeStep() * (1.0f / 300.0f);
        if (CCullZones::CamNoRain() && CCullZones::PlayerNoRain()) {
            CurrentFogIntensity = CurrentFogIntensity - step;
            if (0.f > CurrentFogIntensity) {
                CurrentFogIntensity = 0.f;
            }
        } else {
            CurrentFogIntensity = CurrentFogIntensity + step;
            if (1.f < CurrentFogIntensity) {
                CurrentFogIntensity = 1.f;
            }
        }

        if (CPostEffects::m_fWaterFXStartUnderWaterness <= CWeather::UnderWaterness) {
            CurrentFogIntensity = 0.f;
            return;
        }

        if (CurrentFogIntensity == 0.f) {
            return;
        }
    }

    // Como el original: arriba (0x50E420) y derecha (0x41CC70) del CPlaceable de la cámara.
    const CVector camUp = TheCamera.GetUpVector();
    const CVector camRight = CloudsCameraRight();

    CPostEffects::ImmediateModeRenderStatesStore();
    CPostEffects::ImmediateModeRenderStatesSet();

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,   RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCloudMaskTex)));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));

    const int32 red   = std::min(CTimeCycle::m_CurrentColours.m_nSkyBottomRed + 132, 255);
    const int32 green = std::min(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen + 132, 255);
    const int32 blue  = std::min(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue + 132, 255);

    int32 numVerts = 0;
    const auto RenderVertices = [&] {
        if (RwIm3DTransform(TempBufferVertices.m_3d, numVerts, nullptr, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXUV)) {
            RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
            RwIm3DEnd();
        }
        numVerts = 0;
    };

    // Como el original: un índice fuera de 0..3 deja la posición y las UV del vértice anterior.
    CVector vertPos{ 0.f, 0.f, 0.f };
    float   vertU{}, vertV{};
    for (int32 fogIdx = 0; fogIdx < MAX_MOVING_FOG; fogIdx++) {
        if (!ms_mf.m_bFogSlots[fogIdx])
            continue;

        const CVector pos      = ms_mf.m_vecPosn[fogIdx];
        const float   halfSize = ms_mf.m_fSize[fogIdx] * 0.5f;
        const CVector fogUp    = CVector{ camUp.x * halfSize, camUp.y * halfSize, camUp.z * halfSize };
        const CVector fogRight = CVector{ camRight.x * halfSize, camRight.y * halfSize, camRight.z * halfSize };

        const int32  alpha = CloudsFtol(CWeather::Foggyness_SF * ms_mf.m_fIntensity[fogIdx] * CurrentFogIntensity);
        const uint32 color = (static_cast<uint32>(alpha) << 24) | (static_cast<uint32>(red) << 16) | (static_cast<uint32>(green) << 8) | static_cast<uint32>(blue);
        for (int32 k = 0; k < 6; k++) {
            // Como el original: cada esquina con el orden de sumas del EXE (la primera es (derecha + arriba) + posición).
            switch (ms_mf.m_nPrimIndices[k]) {
            case 0:
                vertPos = CVector{ fogRight.x + fogUp.x + pos.x, fogRight.y + fogUp.y + pos.y, fogRight.z + fogUp.z + pos.z };
                vertU = 0.f; vertV = 0.f;
                break;
            case 1:
                vertPos = CVector{ fogRight.x + pos.x - fogUp.x, fogRight.y + pos.y - fogUp.y, fogRight.z + pos.z - fogUp.z };
                vertU = 1.f; vertV = 0.f;
                break;
            case 2:
                vertPos = CVector{ pos.x - fogRight.x - fogUp.x, pos.y - fogRight.y - fogUp.y, pos.z - fogRight.z - fogUp.z };
                vertU = 1.f; vertV = 1.f;
                break;
            case 3:
                vertPos = CVector{ pos.x - fogRight.x + fogUp.x, pos.y - fogRight.y + fogUp.y, pos.z - fogRight.z + fogUp.z };
                vertU = 0.f; vertV = 1.f;
                break;
            default:
                break;
            }

            auto& vert = TempBufferVertices.m_3d[numVerts++];
            RwIm3DVertexSetPos(&vert, vertPos.x, vertPos.y, vertPos.z);
            vert.color = color;
            RwIm3DVertexSetU(&vert, vertU);
            RwIm3DVertexSetV(&vert, vertV);

            // Flush buffer if it's getting full
            if (numVerts == TOTAL_TEMP_BUFFER_3DVERTICES - 2) {
                RenderVertices();
            }
        }
    }

    // Render all remaining (if any)
    if (numVerts > 0) {
        RenderVertices();
    }

    CPostEffects::ImmediateModeRenderStatesReStore();
    MovingFog_Update();
}

// 0x713950
void CClouds::Render() {
    if (!CGame::CanSeeOutSideFromCurrArea()) {
        return;
    }

    CCoronas::SunBlockedByClouds = false;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDONE));

    CSprite::InitSpriteBuffer();

    const auto& cc = CTimeCycle::m_CurrentColours;
    const uint8 hours = CClock::ms_nGameClockHours;

    //
    // Luna [0x7139B2 - 0x713D2A]
    //
    {
        // Como el original: minutos del día en float, |t - 220| truncado con _ftol2 y comparado con signo.
        float t = static_cast<float>(static_cast<int32>(hours) * 60 + CClock::ms_nGameClockMinutes)
                + static_cast<float>(CClock::ms_nGameClockSeconds) * (1.0f / 60.0f) - 220.0f;
        if (t < 0.f) {
            t = -t;
        }
        const int32 moonDist = CloudsFtol(t);
        if (moonDist < 220 && s_DebugSettings.Moon.Enabled) {
            const int32 colorB = 220 - moonDist;
            // Como el original: max(nubes, niebla) con la comparación del EXE (iguales o NaN -> niebla).
            const float balance = CWeather::CloudCoverage > CWeather::Foggyness ? CWeather::CloudCoverage : CWeather::Foggyness;
            const int32 colorRG = CloudsFtol((1.0f - balance) * static_cast<float>(colorB));
            if (colorRG > 0) {
                const auto& camPos = TheCamera.GetPosition();
                // Como el original: X se copia tal cual (sin sumar 0).
                const CVector moonPos{ camPos.x, camPos.y - 100.0f, camPos.z + 15.0f };
                CVector moonPosScr;
                float   scrW, scrH;
                if (CSprite::CalcScreenCoors(moonPos, &moonPosScr, &scrW, &scrH, false, true)) {
                    const float daysFrac = static_cast<float>(CClock::ms_nGameClockDays) * (1.0f / 31.0f);
                    const float z        = CDraw::GetFarClipZ();
                    const auto  MoonMult = [] { return static_cast<float>(CCoronas::MoonSize) * 2.0f + 4.0f; };

                    // Sprite negro sin textura
                    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
                    RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDSRCALPHA));
                    RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDONE));
                    {
                        const float mult = MoonMult();
                        CSprite::RenderOneXLUSprite({ moonPosScr.x, moonPosScr.y, z }, { mult * scrW, scrH * mult }, 0, 0, 0, 255, 1.0f / z, 255, 0, 0);
                    }

                    // Máscara de la luna
                    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpMoonMask)));
                    RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDSRCALPHA));
                    RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDONE));
                    {
                        // Como el original: X se desplaza con el día (ancho * 5,4) e Y con alto * 0,7 (0x872838); el tamaño es 1,7 veces.
                        const float mult  = MoonMult();
                        const float moonH = scrH * mult;
                        const float moonW = mult * scrW;
                        CSprite::RenderOneXLUSprite(
                            { moonW * 5.4f * (daysFrac - 0.5f) + moonPosScr.x, moonH * std::bit_cast<float>(0x3F333334u) + moonPosScr.y, z },
                            { moonW * 1.7f, 1.7f * moonH },
                            0, 0, 0, 0, 1.0f / z, 255, 0, 0
                        );
                    }

                    // Textura de la luna
                    // Como el original (0x713C51): mezcla y escritura Z antes de fijar el raster de la luna.
                    RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDDESTALPHA));
                    RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDONE));
                    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,  RWRSTATE(FALSE));
                    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCoronaTexture[CORONATYPE_MOON])));
                    {
                        const float mult  = MoonMult();
                        // Como el original (0x713CC6): el azul parte del color ya balanceado ([esp+18h] se sobrescribe en 0x713A4E).
                        const int32 moonB = CloudsFtol(static_cast<float>(colorRG) * 0.85f);
                        CSprite::RenderOneXLUSprite({ moonPosScr.x, moonPosScr.y, z }, { mult * scrW, scrH * mult },
                            static_cast<uint8>(colorRG), static_cast<uint8>(colorRG), static_cast<uint8>(moonB), 255, 1.0f / z, 255, 0, 0);
                    }

                    RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDONE));
                    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));
                }
            }
        }
    }

    //
    // Logotipo R* [0x713D2A - 0x714019]
    //
    if ((hours >= 22 || hours <= 5 || s_DebugSettings.Rockstar.Force) && s_DebugSettings.Rockstar.Enabled) {
        int32 t = 255;
        if (hours == 22) {
            t = CClock::ms_nGameClockMinutes * 255 / 60;
        } else if (hours == 5) {
            t = (60 - CClock::ms_nGameClockMinutes) * 255 / 60;
        }
        if (t != 0) {
            const float balance = CWeather::CloudCoverage > CWeather::Foggyness ? CWeather::CloudCoverage : CWeather::Foggyness;
            // Como el original: la base de color ya incluye el balance de nubes/niebla.
            const int32 colorC = CloudsFtol((1.0f - balance) * static_cast<float>(t));

            const auto& starsY     = StaticRef<std::array<float, 9>>(0x8D55EC);
            const auto& starsZ     = StaticRef<std::array<float, 9>>(0x8D5610);
            const auto& starsSizes = StaticRef<std::array<float, 9>>(0x8D5634);

            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCoronaTexture[CORONATYPE_SHINYSTAR])));

            for (int32 i = 0; i < 11; i++) {
                const auto& camPos = TheCamera.GetPosition();
                const float offsetX = i >= 9 ? -100.0f : 100.0f;
                const int32 posIdx  = i % 9;
                // Como el original: Z = (cam.z + 10) + Z·80.
                const CVector starPos{ offsetX + camPos.x, camPos.y - starsY[posIdx] * 90.0f, camPos.z + 10.0f + starsZ[posIdx] * 80.0f };
                CVector starPosScr;
                float   w, h;
                if (!CSprite::CalcScreenCoors(starPos, &starPosScr, &w, &h, false, true)) {
                    continue;
                }
                const float rhw   = 1.0f / starPosScr.z;
                const int32 color = CloudsFtol((1.0f - static_cast<float>(CGeneral::GetRandomNumber() & 0x1F) * 0.015f) * static_cast<float>(colorC));
                CSprite::RenderBufferedOneXLUSprite(starPosScr, { w * starsSizes[posIdx] * 0.8f, h * starsSizes[posIdx] * 0.8f },
                    static_cast<uint8>(color), static_cast<uint8>(color), static_cast<uint8>(color), 255, rhw, 255);
            }

            {
                const auto& camPos = TheCamera.GetPosition();
                const CVector starPos{ camPos.x + 100.0f, camPos.y - 90.0f, camPos.z + 10.0f };
                // Como el original: el rand del asterisco se pide antes de proyectar.
                const int32 color = CloudsFtol((static_cast<float>(CGeneral::GetRandomNumber() & 0x7F) * 0.0015625f + 0.5f) * static_cast<float>(colorC));
                CVector starPosScr;
                float   w, h;
                if (CSprite::CalcScreenCoors(starPos, &starPosScr, &w, &h, false, true)) {
                    CSprite::RenderOneXLUSprite(starPosScr, { w * 5.0f, h * 5.0f },
                        static_cast<uint8>(color), static_cast<uint8>(color), static_cast<uint8>(color), 255, 1.0f / starPosScr.z, 255, 0, 0);
                }
            }

            CSprite::FlushSpriteBuffer();
        }
    }

    //
    // Nubes bajas [0x71401E - 0x71422A]
    //
    {
        // Como el original: max(niebla, nubes) y luego max(ExtraSunny, eso) con las comparaciones del EXE.
        const float fogCloud = CWeather::Foggyness > CWeather::CloudCoverage ? CWeather::Foggyness : CWeather::CloudCoverage;
        const float balance  = CWeather::ExtraSunnyness > fogCloud ? CWeather::ExtraSunnyness : fogCloud;
        const float inv      = 1.0f - balance;
        // Como el original: el color uint16 completo por (1 - balance), truncado y guardado como byte.
        const auto colorR = static_cast<uint8>(CloudsFtol(static_cast<float>(cc.m_nLowCloudsRed) * inv));
        const auto colorG = static_cast<uint8>(CloudsFtol(static_cast<float>(cc.m_nLowCloudsGreen) * inv));
        const auto colorB = static_cast<uint8>(CloudsFtol(static_cast<float>(cc.m_nLowCloudsBlue) * inv));

        if (s_DebugSettings.LowClouds.Enabled) {
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCloudTex)));

            ms_cameraRoll = TheCamera.GetRoll();

            const auto& coordsX = StaticRef<std::array<float, 12>>(0x8D5394);
            const auto& coordsY = StaticRef<std::array<float, 12>>(0x8D53C4);
            const auto& coordsZ = StaticRef<std::array<float, 12>>(0x8D53F4);
            for (int32 i = 0; i < 12; i++) {
                const auto& camPos = TheCamera.GetPosition();
                // Como el original: Z no suma la cámara.
                const CVector cloudPos{ coordsX[i] * 800.0f + camPos.x, coordsY[i] * 800.0f + camPos.y, coordsZ[i] * 60.0f + 40.0f };
                CVector cloudPosScr;
                float   w, h;
                if (!CSprite::CalcScreenCoors(cloudPos, &cloudPosScr, &w, &h, false, true)) {
                    continue;
                }
                CSprite::RenderBufferedOneXLUSprite_Rotate_Dimension(cloudPosScr, { w * 320.0f, h * 40.0f }, colorR, colorG, colorB, 255,
                    1.0f / cloudPosScr.z, ms_cameraRoll, 255);
            }

            CSprite::FlushSpriteBuffer();
        }
    }

    //
    // Arcoíris [0x71422F - 0x714387]
    //
    if (s_DebugSettings.Rainbow.Enabled && (s_DebugSettings.Rainbow.Force || CWeather::Rainbow != 0.0f)) {
        const auto& lineR = StaticRef<std::array<uint8, 6>>(0x8D5370);
        const auto& lineG = StaticRef<std::array<uint8, 6>>(0x8D5378);
        const auto& lineB = StaticRef<std::array<uint8, 6>>(0x8D5380);

        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCoronaTexture[CORONATYPE_SHINYSTAR])));

        for (int32 i = 0; i < 6; i++) {
            const auto& camPos = TheCamera.GetPosition();
            const CVector linePos{ static_cast<float>(i) * 1.5f + camPos.x, camPos.y + 100.0f, camPos.z + 5.0f };
            CVector linePosScr;
            float   w, h;
            if (!CSprite::CalcScreenCoors(linePos, &linePosScr, &w, &h, false, true)) {
                continue;
            }
            const float rhw = 1.0f / linePosScr.z;
            const auto  b   = static_cast<uint8>(CloudsFtol(static_cast<float>(lineB[i]) * CWeather::Rainbow));
            const auto  g   = static_cast<uint8>(CloudsFtol(static_cast<float>(lineG[i]) * CWeather::Rainbow));
            const auto  r   = static_cast<uint8>(CloudsFtol(static_cast<float>(lineR[i]) * CWeather::Rainbow));
            CSprite::RenderBufferedOneXLUSprite(linePosScr, { w * 2.0f, h * 50.0f }, r, g, b, 255, rhw, 255);
        }

        CSprite::FlushSpriteBuffer();
    }

    //
    // Estelas [0x71438C - 0x714640]
    //
    RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));

    if (!s_DebugSettings.Streaks.Enabled) {
        return;
    }
    if (!s_DebugSettings.Streaks.Force) {
        if (CClock::ms_nGameClockHours >= 5) {
            return;
        }
        if (!IsExtraSunny(CWeather::OldWeatherType) || !IsExtraSunny(CWeather::NewWeatherType)) {
            return;
        }
    }

    const uint32 timeMS      = CTimer::GetTimeInMS();
    const int32  repeat64    = static_cast<int32>((timeMS >> 13) & 63);
    const int32  repeatDelta = static_cast<int32>(timeMS & 0x1FFF);
    if (repeatDelta >= 800) {
        return;
    }

    const auto& camPos = TheCamera.GetPosition();

    CVector size{ static_cast<float>(repeat64 % 7 - 3) * 0.1f, static_cast<float>(repeat64 % 8 - 4) * 0.1f, 1.0f };
    size.Normalise();

    CVector offsetDir{ static_cast<float>(repeat64 % 9 - 5), static_cast<float>(repeat64 % 10 - 5), 0.1f };
    offsetDir.Normalise();

    const CVector basePos{ offsetDir.x * 1000.0f + camPos.x, offsetDir.y * 1000.0f + camPos.y, offsetDir.z * 1000.0f + camPos.z };
    const float   scale  = static_cast<float>((400 - repeatDelta) * 2);
    const float   scale2 = scale + 50.0f;

    // Como el original: dos vértices en el inicio del búfer temporal sin tocar sus contadores; índices de 0x8D579C.
    auto& v0 = TempBufferVertices.m_3d[0];
    auto& v1 = TempBufferVertices.m_3d[1];
    RwIm3DVertexSetPos(&v0, size.x * scale + basePos.x, size.y * scale + basePos.y, size.z * scale + basePos.z);
    v0.color = 0xE1FFFFFF;
    RwIm3DVertexSetPos(&v1, size.x * scale2 + basePos.x, size.y * scale2 + basePos.y, size.z * scale2 + basePos.z);
    v1.color = 0x00FFFFFF;

    if (RwIm3DTransform(TempBufferVertices.m_3d, 2, nullptr, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA)) {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPEPOLYLINE, StaticRef<std::array<RxVertexIndex, 2>>(0x8D579C).data(), 2);
        RwIm3DEnd();
    }
}

// 0x714650
void CClouds::RenderSkyPolys() {
    CVector norm{}, pos{};

    if (TheCamera.m_matrix) {
        pos = TheCamera.m_matrix->GetPosition();
        norm = TheCamera.m_matrix->GetForward();
    } else {
        pos = TheCamera.m_placement.m_vPosn;
        float fHeading = TheCamera.m_placement.m_fHeading;
        // Como el original: fsin/fcos del x87 a precisión completa.
        norm.x = -static_cast<float>(TrigSimpleSinCpp(fHeading));
        norm.y = TrigSimpleCosCpp(fHeading);
    }
    norm.z = 0.f;
    norm.Normalise();

    CVector invNorm = -norm;
    float fBlendFactor = (pos.z - 25.0f) * (1.0f / 80.0f);
    // Como el original: límites y máximo con las comparaciones del EXE (NaN acaba en la niebla).
    if (fBlendFactor > 1.0f) {
        fBlendFactor = 1.0f;
    } else if (fBlendFactor < 0.0f) {
        fBlendFactor = 0.0f;
    }
    if (!(CWeather::Foggyness < fBlendFactor)) {
        fBlendFactor = CWeather::Foggyness;
    }

    // Como el original: gris·(1 - f) + inferior·f, truncado con _ftol2 y guardado como byte.
    const RwRGBA grey = CTimeCycle::m_BelowHorizonGrey;
    const float  greyInv = 1.0f - fBlendFactor;
    RwRGBA belowHorizonGrey = grey;
    belowHorizonGrey.red   = static_cast<RwUInt8>(CloudsFtol(static_cast<float>(grey.red) * greyInv + static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyBottomRed) * fBlendFactor));
    belowHorizonGrey.green = static_cast<RwUInt8>(CloudsFtol(static_cast<float>(grey.green) * greyInv + static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen) * fBlendFactor));
    belowHorizonGrey.blue  = static_cast<RwUInt8>(CloudsFtol(static_cast<float>(grey.blue) * greyInv + static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue) * fBlendFactor));

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLNONE));

    uiTempBufferIndicesStored = 0;
    uiTempBufferVerticesStored = 0;

    CVector2D scale{norm.y * SKYP_WIDTH_MULT, invNorm.x * SKYP_WIDTH_MULT}, aPos2D[4] = {};
    CVector aVertices[4] = {};
    float aPosZ[6] = {};

    aPos2D[0].x = pos.x + (invNorm.x - scale.x) * SKYP_CAM_DIST;
    aPos2D[0].y = pos.y + (invNorm.y - scale.y) * SKYP_CAM_DIST;
    aPos2D[1].x = pos.x + (invNorm.x + scale.x) * SKYP_CAM_DIST;
    aPos2D[1].y = pos.y + (invNorm.y + scale.y) * SKYP_CAM_DIST;
    aPos2D[2].x = pos.x + (norm.x - scale.x) * SKYP_CAM_DIST;
    aPos2D[2].y = pos.y + (norm.y - scale.y) * SKYP_CAM_DIST;
    aPos2D[3].x = pos.x + (norm.x + scale.x) * SKYP_CAM_DIST;
    aPos2D[3].y = pos.y + (norm.y + scale.y) * SKYP_CAM_DIST;

    aPosZ[0] = pos.z + (invNorm.z + SKYP_ABOVE_HORIZON_Z) * SKYP_CAM_DIST;
    aPosZ[1] = pos.z + (norm.z + SKYP_ABOVE_HORIZON_Z) * SKYP_CAM_DIST;
    aPosZ[2] = pos.z + (norm.z + SKYP_HORIZON_Z) * SKYP_CAM_DIST;
    aPosZ[3] = pos.z + (norm.z + SKYP_SEA_HORIZON_Z) * SKYP_CAM_DIST;
    aPosZ[4] = pos.z + (norm.z + SKYP_BELOW_HORIZON_Z) * SKYP_CAM_DIST;
    aPosZ[5] = pos.z + (invNorm.z + SKYP_BELOW_HORIZON_Z) * SKYP_CAM_DIST;

    aVertices[0].x = aPos2D[2].x;
    aVertices[0].y = aPos2D[2].y;
    aVertices[0].z = aPosZ[1];

    aVertices[1].x = aPos2D[3].x;
    aVertices[1].y = aPos2D[3].y;
    aVertices[1].z = aPosZ[1];

    aVertices[2].x = aPos2D[2].x;
    aVertices[2].y = aPos2D[2].y;
    aVertices[2].z = aPosZ[2];

    aVertices[3].x = aPos2D[3].x;
    aVertices[3].y = aPos2D[3].y;
    aVertices[3].z = aPosZ[2];

    SetUpOneSkyPoly(
        aVertices[0], aVertices[1], aVertices[2], aVertices[3],
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopRed),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopGreen),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopBlue),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomRed),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue)
    );

    aVertices[0].z = aPosZ[2];
    aVertices[1].z = aPosZ[2];
    aVertices[2].z = aPosZ[3];
    aVertices[3].z = aPosZ[3];

    SetUpOneSkyPoly(
        aVertices[0], aVertices[1], aVertices[2], aVertices[3],
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomRed),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomRed),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue)
    );

    aVertices[0].z = aPosZ[3];
    aVertices[1].z = aPosZ[3];
    aVertices[2].z = aPosZ[4];
    aVertices[3].z = aPosZ[4];

    SetUpOneSkyPoly(
        aVertices[0], aVertices[1], aVertices[2], aVertices[3],
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomRed),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue),
        belowHorizonGrey.red,
        belowHorizonGrey.green,
        belowHorizonGrey.blue
    );

    aVertices[0].z = aPosZ[1];
    aVertices[1].z = aPosZ[1];
    aVertices[2].x = aPos2D[0].x;
    aVertices[2].y = aPos2D[0].y;
    aVertices[2].z = aPosZ[0];
    aVertices[3].x = aPos2D[1].x;
    aVertices[3].y = aPos2D[1].y;
    aVertices[3].z = aPosZ[0];

    SetUpOneSkyPoly(
        aVertices[0], aVertices[1], aVertices[2], aVertices[3],
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopRed),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopGreen),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopBlue),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopRed),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopGreen),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopBlue)
    );

    aVertices[0].z = aPosZ[4];
    aVertices[1].x = aPos2D[3].x;
    aVertices[1].y = aPos2D[3].y;
    aVertices[1].z = aPosZ[4];
    aVertices[2].z = aPosZ[5];
    aVertices[3].z = aPosZ[5];

    SetUpOneSkyPoly(
        aVertices[0], aVertices[1], aVertices[2], aVertices[3],
        belowHorizonGrey.red,
        belowHorizonGrey.green,
        belowHorizonGrey.blue,
        belowHorizonGrey.red,
        belowHorizonGrey.green,
        belowHorizonGrey.blue
    );

    CBrightLights::RenderOutGeometryBuffer();
}

// 0x7154B0
void CClouds::RenderBottomFromHeight() {
    const auto& colours = CTimeCycle::m_CurrentColours;
    const auto red = uint8(std::min(colours.m_nFluffyCloudsBottomRed * 2.f + 20.f, 255.f));
    const auto green = uint8(std::min(colours.m_nFluffyCloudsBottomGreen * 1.5f, 255.f));
    const auto blue = uint8(std::min(colours.m_nFluffyCloudsBottomBlue * 1.5f, 255.f));
    const auto camera = TheCamera.GetPosition();
    if (camera.z < -90.f) {
        return;
    }

    // The rendering thread uses x87 24-bit precision; preserve single-precision arithmetic.
    float low = 160.f, high = 190.f;
    const float rise = (camera.z - 190.f - 10.f) * 0.3f;
    if (rise > 0.f) {
        low = 160.f + rise;
        high = 190.f + rise;
    }
    auto& windShift = StaticRef<float>(0xC6E954);
    windShift = CTimer::GetTimeStep() * CWeather::Wind * 0.25f + windShift;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCloudMaskTex)));

    const int32 baseAlpha = int32(colours.m_fCloudAlpha);
    const auto& random = StaticRef<std::array<float, 80>>(0x8D5658);
    const auto sample = [&](int i) { return random[i % random.size()]; };
    const auto blockCentre = [](float value, int32 mask, int32 half) { return (int32(value) & mask) + half; };
    auto& vertices = StaticRef<uint32>(0xC4B950);
    auto& indices = StaticRef<uint32>(0xC4B954);
    const float thickness = high - low;
    if (baseAlpha != 0) {
        vertices = indices = 0;
        for (int i = 0; i < 28; ++i) {
            const float shift = (sample(i + 6) * 0.5f + 1.f) * windShift;
            const float dx = sample(i) * 512.f + shift - camera.x;
            const float dy = sample(i + 1) * 512.f + shift - camera.y;
            const float wrappedX = dx - blockCentre(dx, ~511, 256);
            const float x = wrappedX + camera.x;
            const float y = dy - blockCentre(dy, ~511, 256) + camera.y;
            const float z = thickness * sample(i + 2) + low;
            const float height = z - camera.z;
            int32 alpha = height > 0.f ? int32((1.f - height * 0.004f) * baseAlpha) : baseAlpha;
            if (alpha <= 0) continue;
            const float distance = std::max(std::abs(camera.x - x), std::abs(camera.y - y)) / 256.f;
            if (distance > 0.75f) alpha = int32((1.f - (distance - 0.75f) * 4.f) * alpha);
            if (alpha <= 0) continue;
            const float width = sample(i + 3) * 100.f + 60.f;
            const float depth = sample(i + 4) * 100.f + 60.f;
            const CVector positions[] = {
                {x - width, y + depth, z},
                {x + width, y + depth, z},
                {x - width, y - depth, z},
                {x + width, y - depth, z}
            };
            for (const auto offset : {2, 1, 0, 2, 3, 1}) aTempBufferIndices[indices++] = RxVertexIndex(vertices + offset);
            for (int j = 0; j < 4; ++j) {
                auto& v = TempBufferVertices.m_3d[vertices++];
                RwIm3DVertexSetPos(&v, positions[j].x, positions[j].y, positions[j].z);
                // Only the first corner uses the brighter colour in the original.
                RwIm3DVertexSetRGBA(&v, j == 0 ? red : colours.m_nFluffyCloudsBottomRed,
                    j == 0 ? green : colours.m_nFluffyCloudsBottomGreen,
                    j == 0 ? blue : colours.m_nFluffyCloudsBottomBlue, alpha);
                RwIm3DVertexSetU(&v, j >= 2 ? 1.f : 0.f);
                RwIm3DVertexSetV(&v, j % 2 ? 1.f : 0.f);
            }
        }
        if (indices && RwIm3DTransform(TempBufferVertices.m_3d, vertices, nullptr, rwIM3D_VERTEXUV | rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA)) {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, indices);
            RwIm3DEnd();
        }
        vertices = indices = 0;
    }

    CSprite::InitSpriteBuffer();
    if (baseAlpha > 0) {
        const auto& xs = StaticRef<std::array<float, 30>>(0x8D5818);
        const auto& ys = StaticRef<std::array<float, 30>>(0x8D57A0);
        for (int i = 0; i < 30; ++i) {
            const float shift = windShift * 0.5f;
            const float dx = shift + xs[i] - camera.x;
            const float dy = shift + ys[i] - camera.y;
            const float wrappedX = dx - blockCentre(dx, ~255, 128);
            const CVector pos{wrappedX + camera.x, dy - blockCentre(dy, ~255, 128) + camera.y, thickness * sample(i) + low};
            int32 alpha = int32((1.f - std::abs(camera.z - pos.z) * 0.004f) * baseAlpha);
            if (alpha <= 0) continue;
            const float size = (sample(i + 1) + 1.f) * 12.f;
            const float distance = std::min(std::max(std::abs(camera.x - pos.x), std::abs(camera.y - pos.y)) / 128.f, 1.f);
            if (distance > 0.75f) alpha = int32((1.f - (distance - 0.75f) * 4.f) * alpha);
            if (distance <= 0.05f) continue;
            if (distance < 0.1f) alpha = int32((distance - 0.05f) * alpha * 20.f);
            CVector screen; float w, h;
            if (CSprite::CalcScreenCoors(pos, &screen, &w, &h, false, true)) {
                CSprite::RenderBufferedOneXLUSprite(screen, {w * size, h * size},
                    uint8(colours.m_nFluffyCloudsBottomRed), uint8(colours.m_nFluffyCloudsBottomGreen),
                    uint8(colours.m_nFluffyCloudsBottomBlue), 256, 1.f / screen.z, uint8(alpha));
            }
        }
    }
    CSprite::FlushSpriteBuffer();
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(nullptr));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
}

//
// -- VOLUMETRIC CLOUDS --
//

// 0x7131C0
void CClouds::VolumetricCloudsInit() {
    ms_vc.quadNormal[0] = CVector(0.0f, 1.0f, 0.0f);
    ms_vc.quadNormal[1] = CVector(0.0f, 0.0f, 1.0f);
    ms_vc.quadNormal[2] = CVector(1.0f, 0.0f, 0.0f);

    ms_vc.modelX[0] = -0.5f;
    ms_vc.modelY[0] = 0.0f;
    ms_vc.modelZ[0] = 0.5f;
    ms_vc.modelU[0] = 0.0f;
    ms_vc.modelV[0] = 0.0f;

    ms_vc.modelX[1] = 0.5f;
    ms_vc.modelY[1] = 0.0f;
    ms_vc.modelZ[1] = 0.5f;
    ms_vc.modelU[1] = 1.0f;
    ms_vc.modelV[1] = 0.0f;

    ms_vc.modelX[2] = -0.5f;
    ms_vc.modelY[2] = 0.0f;
    ms_vc.modelZ[2] = -0.5f;
    ms_vc.modelU[2] = 0.0f;
    ms_vc.modelV[2] = 1.0f;

    ms_vc.modelX[3] = 0.5f;
    ms_vc.modelY[3] = 0.0f;
    ms_vc.modelZ[3] = 0.5f;
    ms_vc.modelU[3] = 1.0f;
    ms_vc.modelV[3] = 0.0f;

    ms_vc.modelX[4] = 0.5f;
    ms_vc.modelY[4] = 0.0f;
    ms_vc.modelZ[4] = -0.5f;
    ms_vc.modelU[4] = 1.0f;
    ms_vc.modelV[4] = 1.0f;

    ms_vc.modelX[5] = -0.5f;
    ms_vc.modelY[5] = 0.0f;
    ms_vc.modelZ[5] = -0.5f;
    ms_vc.modelU[5] = 0.0f;
    ms_vc.modelV[5] = 1.0f;

    ms_vc.modelX[6] = -0.5f;
    ms_vc.modelY[6] = 0.5f;
    ms_vc.modelZ[6] = 0.0f;
    ms_vc.modelU[6] = 0.0f;
    ms_vc.modelV[6] = 0.0f;

    ms_vc.modelX[7] = 0.5f;
    ms_vc.modelY[7] = 0.5f;
    ms_vc.modelZ[7] = 0.0f;
    ms_vc.modelU[7] = 1.0f;
    ms_vc.modelV[7] = 0.0f;

    ms_vc.modelX[8] = -0.5f;
    ms_vc.modelY[8] = -0.5f;
    ms_vc.modelZ[8] = 0.0f;
    ms_vc.modelU[8] = 0.0f;
    ms_vc.modelV[8] = 1.0f;

    ms_vc.modelX[9] = 0.5f;
    ms_vc.modelY[9] = 0.5f;
    ms_vc.modelZ[9] = 0.0f;
    ms_vc.modelU[9] = 1.0f;
    ms_vc.modelV[9] = 0.0f;

    ms_vc.modelX[10] = 0.5f;
    ms_vc.modelY[10] = -0.5f;
    ms_vc.modelZ[10] = 0.0f;
    ms_vc.modelU[10] = 1.0f;
    ms_vc.modelV[10] = 1.0f;

    ms_vc.modelX[11] = -0.5f;
    ms_vc.modelY[11] = -0.5f;
    ms_vc.modelZ[11] = 0.0f;
    ms_vc.modelU[11] = 0.0f;
    ms_vc.modelV[11] = 1.0f;

    ms_vc.modelX[12] = 0.0f;
    ms_vc.modelY[12] = -0.5f;
    ms_vc.modelZ[12] = 0.5f;
    ms_vc.modelU[12] = 0.0f;
    ms_vc.modelV[12] = 0.0f;

    ms_vc.modelX[13] = 0.0f;
    ms_vc.modelY[13] = 0.5f;
    ms_vc.modelZ[13] = 0.5f;
    ms_vc.modelU[13] = 1.0f;
    ms_vc.modelV[13] = 0.0f;

    ms_vc.modelX[14] = 0.0f;
    ms_vc.modelY[14] = -0.5f;
    ms_vc.modelZ[14] = -0.5f;
    ms_vc.modelU[14] = 0.0f;
    ms_vc.modelV[14] = 1.0f;

    ms_vc.modelX[15] = 0.0f;
    ms_vc.modelY[15] = 0.5f;
    ms_vc.modelZ[15] = 0.5f;
    ms_vc.modelU[15] = 1.0f;
    ms_vc.modelV[15] = 0.0f;

    ms_vc.modelX[16] = 0.0f;
    ms_vc.modelY[16] = 0.5f;
    ms_vc.modelZ[16] = -0.5f;
    ms_vc.modelU[16] = 1.0f;
    ms_vc.modelV[16] = 1.0f;

    ms_vc.modelX[17] = 0.0f;
    ms_vc.modelY[17] = -0.5f;
    ms_vc.modelZ[17] = -0.5f;
    ms_vc.modelU[17] = 0.0f;
    ms_vc.modelV[17] = 1.0f;

    for (auto i = 0u; i < MAX_VOLUMETRIC_CLOUDS; ++i) {
        ms_vc.bUsed[i] = false;
        ms_vc.bJustCreated[i] = 0;
    }
}

// 0x715F40
void CClouds::VolumetricClouds_Create(CVector* posn) {
    using CGeneral::GetRandomNumberInRange;

    const float rand1To5 = GetRandomNumberInRange(1.f, 5.f);
    const float minX = 20.f * rand1To5, maxX = rand1To5 * 100.f;
    const float minY = 20.f * rand1To5, maxY = rand1To5 * 100.f;
    const float minZ = 20.f * rand1To5, maxZ = 40.f * rand1To5;

    // Como el original: alfa = 36 - trunc(rand/32768 · -92) y tamaño; la posición se pide después (orden de rand del EXE).
    const auto AddVolumetricCloud = [&](int32 vcidx) {
        ms_vc.bUsed[vcidx]        = true;
        ms_vc.bJustCreated[vcidx] = true;
        ms_vc.alpha[vcidx]        = 36 - CloudsFtol(static_cast<float>(CGeneral::GetRandomNumber() & 0xFFFF) * (1.0f / 32768.0f) * -92.0f);

        auto& size = ms_vc.size[vcidx];
        size.x = GetRandomNumberInRange(minX, maxX);
        size.y = GetRandomNumberInRange(minY, maxY);
        size.z = GetRandomNumberInRange(minZ, maxZ);
    };

    if (posn) {
        // Como el original: desplazamiento entero en [-n, n) con n = trunc(r·3); se avanza sobre una copia local de *posn.
        const int32 range = CloudsFtol(rand1To5 * 3.f);
        CVector cur = *posn;
        for (int32 i = 0; i < 5; i++) {
            const int32 vcidx = VolumetricClouds_GetFirstFreeSlot();
            if (vcidx == -1) {
                return;
            }
            AddVolumetricCloud(vcidx);

            auto& pos = ms_vc.pos[vcidx];
            pos.x = static_cast<float>(CGeneral::GetRandomIntInRange(-range, range)) + cur.x;
            pos.y = static_cast<float>(CGeneral::GetRandomIntInRange(-range, range)) + cur.y;
            pos.z = static_cast<float>(CGeneral::GetRandomIntInRange(-range, range)) + cur.z;
            cur = pos;
        }
    } else {
        const CVector camPos  = TheCamera.GetPosition();
        const float   maxDist = m_fVolumetricCloudMaxDistance;
        for (int32 i = 0; i < MAX_VOLUMETRIC_CLOUDS; i++) {
            AddVolumetricCloud(i);

            auto& pos = ms_vc.pos[i];
            pos.x = GetRandomNumberInRange(-maxDist, maxDist) + camPos.x;
            pos.y = GetRandomNumberInRange(-maxDist, maxDist) + camPos.y;
            pos.z = GetRandomNumberInRange(-(maxDist * 0.25f), maxDist * 0.25f) + camPos.z;
        }
    }
}

// 0x7135F0
void CClouds::VolumetricClouds_Delete(int32 vcSlotIndex) {
    vcSlotIndex = std::clamp(vcSlotIndex, 0, MAX_VOLUMETRIC_CLOUDS - 1);
    ms_vc.bUsed[vcSlotIndex]        = false;
    ms_vc.bJustCreated[vcSlotIndex] = false;
}

// unused
// inlined into VolumetricClouds_Create
// 0x7135C0
int32 CClouds::VolumetricClouds_GetFirstFreeSlot() {
    // Como el original: el número de nubes usadas se compara con signo.
    for (int32 i = 0; i < static_cast<int32>(m_VolumetricCloudsUsedNum); i++) {
        if (!ms_vc.bUsed[i]) {
            return i;
        }
    }
    return -1;
}

// 0x713630
float CClouds::VolumetricCloudsGetMaxDistance() {
    // Como el original: solo se devuelve el plano lejano si es menor que 600 (NaN -> 600).
    const float farClip = RwCameraGetFarClipPlane(Scene.m_pRwCamera);
    return farClip < 600.f ? farClip : 600.f;
}

// 0x716380
void CClouds::VolumetricCloudsRender() {
    ZoneScoped;

    if (!s_DebugSettings.VolumetricClouds.Enabled) {
        return;
    }

    // Como el original: se sale si el área actual o la del jugador no es el mundo exterior.
    if (!s_DebugSettings.VolumetricClouds.Force) {
        if (static_cast<int32>(CGame::currArea) != 0 || int(FindPlayerPed()->GetAreaCode()) != AREA_CODE_NORMAL_WORLD) {
            return;
        }
    }

    // Como el original: calidad media 0,66 (0x3F28F5C3), baja 0,5, el resto 1.
    switch (int(g_fx.GetFxQuality())) {
    case 1: m_fVolumetricCloudDensity = std::bit_cast<float>(0x3F28F5C3u); break; // FX_QUALITY_MEDIUM
    case 0: m_fVolumetricCloudDensity = 0.5f; break; // FX_QUALITY_LOW
    default:                m_fVolumetricCloudDensity = 1.f; break;
    }

    using UsedNumT = std::remove_cvref_t<decltype(m_VolumetricCloudsUsedNum)>;
    const int32 usedNum = CloudsFtol(m_fVolumetricCloudDensity * 180.f);
    m_VolumetricCloudsUsedNum = static_cast<UsedNumT>(usedNum);
    if (usedNum <= 0) {
        return;
    }
    if (usedNum > MAX_VOLUMETRIC_CLOUDS) {
        m_VolumetricCloudsUsedNum = static_cast<UsedNumT>(MAX_VOLUMETRIC_CLOUDS);
    }

    m_fVolumetricCloudMaxDistance = CClouds::VolumetricCloudsGetMaxDistance();

    const CVector camPos = TheCamera.GetPosition();

    // Como el original: banderas de inicialización de las estáticas locales (bits 0 y 1 de 0xC6E974).
    auto& staticGuards = StaticRef<uint32>(0xC6E974);
    staticGuards |= 1;

    auto& gfVolumetricCloudFader = StaticRef<float>(0xC6E970);
    if (m_bVolumetricCloudHeightSwitch) {
        const auto delta = CTimer::GetTimeStep() * 4.f;
        if (camPos.z < 220.f) {
            gfVolumetricCloudFader = delta + gfVolumetricCloudFader;
            if (gfVolumetricCloudFader >= 255.f) {
                gfVolumetricCloudFader = 255.f;
                return;
            }
        } else {
            gfVolumetricCloudFader = gfVolumetricCloudFader - delta;
            if (gfVolumetricCloudFader < 0.f) {
                gfVolumetricCloudFader = 0.f;
            }
        }
    } else {
        gfVolumetricCloudFader = 0.f;
    }

    CPostEffects::ImmediateModeRenderStatesStore();
    CPostEffects::ImmediateModeRenderStatesSet();

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,   RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(ms_vc.texture)));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));

    // Como el original: primero el vehículo y luego las coordenadas; no se toca la velocidad del jugador.
    const auto plyrVeh = FindPlayerVehicle(-1, false);
    const auto plyrPos = FindPlayerCoors(-1);
    staticGuards |= 2;

    //> 0x71653F
    auto& gVecCameraCoors = StaticRef<CVector>(0xC6E964);
    auto& gVecPlayerCoors = StaticRef<CVector>(0xC6E958);

    bool moved = false;
    if (camPos.x != gVecCameraCoors.x || camPos.y != gVecCameraCoors.y || camPos.z != gVecCameraCoors.z) {
        moved = true;
    }
    if (plyrPos.x != gVecPlayerCoors.x || plyrPos.y != gVecPlayerCoors.y || plyrPos.z != gVecPlayerCoors.z) {
        moved = true;
    }

    gVecCameraCoors = camPos;
    gVecPlayerCoors = plyrPos;

    if (moved) {
        const float fadeOutDist = m_fVolumetricCloudMaxDistance + 200.f;
        CVector base;
        if (plyrVeh) {
            // Como el original: el frente sale siempre de la matriz; la posición de la matriz o del placement.
            CMatrix* mat = plyrVeh->m_matrix;
            const CVector& fwd = mat->GetForward();
            const CVector& pos = mat ? mat->GetPosition() : plyrVeh->m_placement.m_vPosn;
            base = CVector{ pos.x + fadeOutDist * fwd.x, pos.y + fadeOutDist * fwd.y, pos.z + fadeOutDist * fwd.z };
        } else {
            // Como el original: frente de TheCamera + 0x984 (0xB6F9AC) sumado a la posición de la cámara.
            const auto& fwd = StaticRef<CVector>(0xB6F9AC);
            base = CVector{ camPos.x + fadeOutDist * fwd.x, camPos.y + fadeOutDist * fwd.y, fadeOutDist * fwd.z + camPos.z };
        }
        base.x = CGeneral::GetRandomNumberInRange(-200.f, 200.f) + base.x;
        base.y = CGeneral::GetRandomNumberInRange(-200.f, 200.f) + base.y;
        base.z = CGeneral::GetRandomNumberInRange(-50.f, 50.f) + base.z;
        VolumetricClouds_Create(&base);
    }

    //> 0x7166DD -  Calculate color
    const auto& cc = CTimeCycle::m_CurrentColours;
    const int32 colorSum = cc.m_nSkyTopRed + cc.m_nSkyTopGreen + cc.m_nSkyTopBlue + cc.m_nSkyBottomRed + cc.m_nSkyBottomGreen + cc.m_nSkyBottomBlue;
    const auto  vcClr    = static_cast<uint32>(std::min(colorSum / 6 + 64, 255));

    int32 numVerts = 0;
    const auto RenderOutBuffer = [&] {
        if (RwIm3DTransform(TempBufferVertices.m_3d, numVerts, nullptr, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXUV)) {
            RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
            RwIm3DEnd();
        }
        numVerts = 0;
    };

    for (int32 vcidx = 0; vcidx < static_cast<int32>(m_VolumetricCloudsUsedNum); vcidx++) {
        if (!ms_vc.bUsed[vcidx]) {
            continue;
        }

        auto& vcPos = ms_vc.pos[vcidx];

        // Como el original: distancia antes del viento, sumada en el orden z, y, x.
        const float ddx = vcPos.x - camPos.x, ddy = vcPos.y - camPos.y, ddz = vcPos.z - camPos.z;
        const float vcDistToCam = std::sqrt(ddz * ddz + ddy * ddy + ddx * ddx);

        //> 0x71674E - Adjust vc position by wind
        vcPos.x = m_fVolumetricCloudWindMoveFactor * CWeather::WindDir.x + vcPos.x;
        vcPos.y = CWeather::WindDir.y * m_fVolumetricCloudWindMoveFactor + vcPos.y;

        //> 0x716772 - VC too far, delete it
        if (!ms_vc.bJustCreated[vcidx] && vcDistToCam > m_fVolumetricCloudMaxDistance) {
            VolumetricClouds_Delete(vcidx);
            continue;
        }

        // Como el original: comparaciones estrictas.
        if (vcDistToCam < m_fVolumetricCloudMaxDistance || m_fVolumetricCloudMaxDistance + 200.f < vcDistToCam) {
            ms_vc.bJustCreated[vcidx] = false;
        }

        //> 0x7167DC - Alpha calculation
        int32 vcAlpha = ms_vc.alpha[vcidx] - CloudsFtol(gfVolumetricCloudFader);
        if (vcAlpha < 0) {
            vcAlpha = 0;
        }
        const float fadeOutBeginDist = m_fVolumetricCloudMaxDistance - 100.f;
        if (vcDistToCam > fadeOutBeginDist) {
            if (!vcAlpha) {
                continue;
            }
            if (vcDistToCam > m_fVolumetricCloudMaxDistance) {
                continue;
            }
            const float fadeRange = m_fVolumetricCloudMaxDistance - fadeOutBeginDist;
            int32 distAlpha = CloudsFtol((fadeRange - (vcDistToCam - fadeOutBeginDist)) * static_cast<float>(vcAlpha) / fadeRange);
            if (distAlpha < 0) {
                distAlpha = 0;
            }
            if (distAlpha > vcAlpha) {
                distAlpha = vcAlpha;
            }
            vcAlpha = distAlpha;
            if (!vcAlpha) {
                continue;
            }
        }

        const CVector vcSz   = ms_vc.size[vcidx];
        const CVector vcPos2 = vcPos;
        const float   alphaF = static_cast<float>(vcAlpha);

        //> 0x7168F0 - Calculate quad colors
        uint32 quadColors[3];
        for (auto i = 0; i < 3; i++) {
            // Como el original: dirección normalizada (0x59C910) salvo si es nula; producto escalar en el orden z, y, x.
            CVector dir{ vcPos2.x - camPos.x, vcPos2.y - camPos.y, vcPos2.z - camPos.z };
            if (!(dir.x == 0.f) || !(dir.y == 0.f) || !(dir.z == 0.f)) {
                dir.Normalise();
            }
            const CVector& n = ms_vc.quadNormal[i];
            const float dot = n.z * dir.z + n.y * dir.y + n.x * dir.x;
            const auto quadAlpha = static_cast<uint32>(static_cast<uint8>(CloudsFtol(std::fabs(dot) * alphaF)));
            quadColors[i] = (quadAlpha << 24) | (vcClr << 16) | (vcClr << 8) | vcClr;
        }

        //> 0x7169F0
        for (auto k = 0; k < 3 * 6; k++) {
            auto& vert = TempBufferVertices.m_3d[numVerts];
            RwIm3DVertexSetPos(&vert, vcSz.x * ms_vc.modelX[k] + vcPos2.x, vcSz.y * ms_vc.modelY[k] + vcPos2.y, vcSz.z * ms_vc.modelZ[k] + vcPos2.z);
            vert.color = quadColors[k / 6];
            RwIm3DVertexSetU(&vert, ms_vc.modelU[k]);
            RwIm3DVertexSetV(&vert, ms_vc.modelV[k]);

            // Como el original: se vacía al llegar a 2034 vértices.
            if (++numVerts == 2034) {
                RenderOutBuffer();
            }
        }
    }

    if (numVerts > 0) {
        RenderOutBuffer();
    }

    CPostEffects::ImmediateModeRenderStatesReStore();
}
