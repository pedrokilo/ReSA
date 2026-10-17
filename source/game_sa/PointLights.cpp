/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "PointLights.h"
#include "X87Math.h"
#include "app_light.h"
#include "Camera.h"
#include "Clouds.h"
#include "CutsceneMgr.h"
#include "Scene.h"
#include "Sprite.h"
#include "Timer.h"
#include "Weather.h"
#include "World.h"

// PointLightsLightMultiplierDblCpp (X87Math.cpp) devuelve un double: F + S queda sin redondear en ST0 al final de GetLightMultiplier (0x6FFE70). Aqui se declara como float con otro
// nombre (alias del enlazador) para que el metodo devuelva ST0 tal cual, sin pasar por memoria como float.
extern "C" float __cdecl PointLightsLightMultiplierFltCpp(const float* point, const void* lights, int count);
#pragma comment(linker, "/alternatename:_PointLightsLightMultiplierFltCpp=_PointLightsLightMultiplierDblCpp")

void CPointLights::InjectHooks() {
    RH_ScopedClass(CPointLights);
    RH_ScopedCategoryGlobal();
    RH_ScopedInstall(Init, 0x6FFB40);
    RH_ScopedInstall(GenerateLightsAffectingObject, 0x6FFBB0);
    RH_ScopedInstall(GetLightMultiplier, 0x6FFE70);
    RH_ScopedInstall(RemoveLightsAffectingObject, 0x6FFFE0);
    RH_ScopedInstall(ProcessVerticalLineUsingCache, 0x6FFFF0);
    RH_ScopedInstall(AddLight, 0x7000E0);
    RH_ScopedInstall(RenderFogEffect, 0x7002D0);
}

// 0x6FFB40
void CPointLights::Init() {
    // Como el original (0x6FFB47/0x6FFB77/0x6FFB86): sólo reinicia las cachés, no NumLights.
    std::memset(aCachedMapReadResults, 0, sizeof(aCachedMapReadResults));
    NextCachedValue = 0;
    std::memset(aCachedMapReads, 0, sizeof(aCachedMapReads));
}

// 0x6FFBB0
float CPointLights::GenerateLightsAffectingObject(const CVector* point, float* totalLighting, CEntity* entity) {
    // Como el original (0x6FFBB0..0x6FFE68): acumulador de 1.0f en la pila, bucle con contador de 16 bits (MOVSX BX) que relee NumLights en cada vuelta y, por cada luz,
    // el tramo x87 de X87Math (PointLightsGenerateOneCpp); sólo si la luz aporta una dirección se llama a AddAnExtraDirectionalLight.
    float result = 1.0f;
    if (std::bit_cast<int32>(NumLights) > 0) {
        uint32 counter = 0;
        int32 index = 0;
        do {
            float args[6];
            if (PointLightsGenerateOneCpp(&aLights[index], &point->x, totalLighting, entity, &result, args)) {
                AddAnExtraDirectionalLight(Scene.m_pRpWorld, args[0], args[1], args[2], args[3], args[4], args[5]);
            }
            ++counter;
            index = static_cast<int16>(counter);
        } while (index < std::bit_cast<int32>(NumLights));
    }
    return result;
}

// 0x6FFE70
float CPointLights::GetLightMultiplier(const CVector* point) {
    // El EXE no tiene llamadores. Devuelve F + S (producto de las luces ANTILIGHT y suma de las demás) en ST0 sin redondear a float: ver PointLightsLightMultiplierDblCpp.
    return PointLightsLightMultiplierFltCpp(&point->x, aLights, std::bit_cast<int32>(NumLights));
}

// 0x6FFFE0
void CPointLights::RemoveLightsAffectingObject() {
    // Como el colaborador original 0x7359E0–0x735A1D: seis luces extra y contador.
    // El argumento Scene.m_pRpWorld de 0x6FFFE0 no se usa en ese colaborador.
    auto& extraLights = StaticRef<RpLight*[6]>(0xC886F0);
    for (auto* light : extraLights) {
        reinterpret_cast<uint8*>(light)[2] = 0; // RwObject::flags, sin cambiar type/subType.
    }
    StaticRef<uint32>(0xC88708) = 0;
}

// 0x6FFFF0
bool CPointLights::ProcessVerticalLineUsingCache(CVector point, float* outZ) {
    // Como el original: busca el punto en las 32 entradas de la caché (comparación x87 ordenada) y devuelve el z guardado tal cual (copia cruda); si no está, traza una línea
    // vertical de 20 unidades hacia abajo (sólo edificios y dummies) y guarda el z del impacto (FLD/FSTP: un NaN señalizante se silencia) con una escritura circular.
    const int32 hit = PointLightsCacheFindCpp(&aCachedMapReads[0].x, &point.x);
    if (hit >= 0) {
        std::memcpy(outZ, &aCachedMapReadResults[hit], sizeof(float));
        return true;
    }
    CColPoint colPoint;
    CEntity* entity;
    if (!CWorld::ProcessVerticalLine(point, PointLightsVerticalEndZCpp(point.z), colPoint, entity, true, false, false, false, true, false, nullptr)) {
        return false;
    }
    const int32 slot = NextCachedValue;
    PointLightsFloatCopyCpp(&colPoint.m_vecPoint.z, &aCachedMapReadResults[slot]);
    std::memcpy(&aCachedMapReads[slot], &point, sizeof(point));
    NextCachedValue = (slot + 1) % MAX_POINT_LIGHTS;
    PointLightsFloatCopyCpp(&colPoint.m_vecPoint.z, outZ);
    return true;
}

// 0x7000E0
void CPointLights::AddLight(uint8 lightType, CVector point, CVector direction, float radius, float red, float green, float blue, uint8 fogType, bool generateExtraShadows, CEntity* entityAffected) {
    // Como el original (0x7000E0): la cámara se lee de la matriz o, sin matriz, del placement; x e y se filtran antes de comprobar el
    // contador (0x700115..0x700170) y la distancia, con el orden z*z + y*y + x*x, se calcula después (0x700197..0x7001C7).
    // Las comparaciones y la atenuación (0x70027C..0x7002B9, con el coeficiente sin descargar a un float intermedio) viven en
    // X87Math: son estrictas, también para NaN, y /fp:fast no debe reordenarlas.
    const float* cameraPos = &TheCamera.GetPosition().x;
    float offsetX, offsetY, limit;
    if (!PointLightsInRangeXYCpp(cameraPos, point.x, point.y, radius, &offsetX, &offsetY, &limit)) {
        return;
    }
    if (std::bit_cast<int32>(NumLights) >= MAX_POINT_LIGHTS) {
        return;
    }
    // Atenúa red/green/blue en el sitio sólo si la luz entra en la franja de atenuación; si no, los deja intactos.
    if (!PointLightsDistanceFadeCpp(point.z, cameraPos, offsetX, offsetY, limit, &red, &green, &blue)) {
        return;
    }
    auto& light = aLights[NumLights++];
    // Como el original: copia cruda de los vectores y deja intacto el byte de relleno 0x2F.
    std::memcpy(&light.m_vecPosn, &point, sizeof(point));
    std::memcpy(&light.m_vecDirection, &direction, sizeof(direction));
    std::memcpy(&light.m_fRadius, &radius, sizeof(radius));
    std::memcpy(&light.m_fColorRed, &red, sizeof(red));
    std::memcpy(&light.m_fColorGreen, &green, sizeof(green));
    std::memcpy(&light.m_fColorBlue, &blue, sizeof(blue));
    light.m_pEntityToLight = entityAffected;
    light.m_nType = static_cast<ePointLightType>(lightType);
    light.m_nFogType = fogType;
    light.m_bGenerateShadows = generateExtraShadows;
}

// Llamada a CSprite::RenderBufferedOneXLUSprite_Rotate_Aspect con los argumentos que calcula PointLightsFogSpriteCpp (0x70076A..0x700817 y 0x700AAC..0x700B6B).
static void FogEmitSprite(const CPointLight& light, const float& alpha, const RwV3d& screen, float w, float h, int32 tab, uint32 timeValue, bool pointLight) {
    PointLightsFogSprite sprite;
    PointLightsFogSpriteCpp(&light, &alpha, &screen.x, w, h, &StaticRef<float>(0x8D5068), tab, static_cast<int32>(timeValue), pointLight, &sprite);
    CSprite::RenderBufferedOneXLUSprite_Rotate_Aspect(sprite.x, sprite.y, sprite.z, sprite.w, sprite.h, static_cast<uint8>(sprite.red), static_cast<uint8>(sprite.green),
        static_cast<uint8>(sprite.blue), static_cast<int16>(sprite.intensity), sprite.recipNearZ, sprite.angle, 255);
}

// Luz direccional (0x7003AB..0x700851): rejilla de 4 unidades alrededor del segmento de 12 unidades que sale de la luz.
static void FogDirectionalLight(const CPointLight& light, const float& intensity) {
    int32 bounds[4];
    if (!PointLightsFogDirBoundsCpp(&light, bounds)) {
        return;
    }
    for (int32 gx = bounds[0]; gx <= bounds[1]; gx += 4) {
        for (int32 gy = bounds[2]; gy <= bounds[3]; gy += 4) {
            float origin[3], endZ;
            int32 tab;
            if (!PointLightsFogDirCellPreCpp(&light, gx, gy, origin, &endZ, &tab)) {
                continue;
            }
            CColPoint colPoint;
            CEntity* entity;
            if (!CWorld::ProcessVerticalLine(*reinterpret_cast<const CVector*>(origin), endZ, colPoint, entity, true, false, false, false, true, false, nullptr)) {
                continue;
            }
            float posn[3], alpha;
            if (!PointLightsFogDirCellPostCpp(&light, origin, &colPoint.m_vecPoint.z, &intensity, posn, &alpha)) {
                continue;
            }
            RwV3d screen;
            float w, h;
            if (!CSprite::CalcScreenCoors(*reinterpret_cast<const RwV3d*>(posn), &screen, &w, &h, true, true)) {
                continue;
            }
            FogEmitSprite(light, alpha, screen, w, h, tab, CTimer::GetTimeInMS() & 0x1FFFu, false);
        }
    }
}

// Luces de punto y de sólo niebla (0x700856..0x700BA9): suelo de la luz con la caché y rejilla de 2 unidades en un cuadrado de lado 2·size.
static void FogPointLight(const CPointLight& light, const float& intensity, const float& size) {
    float groundZ;
    if (!CPointLights::ProcessVerticalLineUsingCache(light.m_vecPosn, &groundZ)) {
        return;
    }
    int32 bounds[4];
    if (!PointLightsFogPointBoundsCpp(&light, &size, bounds)) {
        return;
    }
    for (int32 gx = bounds[0]; gx <= bounds[1]; gx += 2) {
        for (int32 gy = bounds[2]; gy <= bounds[3]; gy += 2) {
            float posn[3], alpha;
            int32 tab;
            if (!PointLightsFogPointCellPreCpp(&light, gx, gy, &size, &TheCamera.GetPosition().x, &intensity, &groundZ, posn, &alpha, &tab)) {
                continue;
            }
            RwV3d screen;
            float w, h;
            if (!CSprite::CalcScreenCoors(*reinterpret_cast<const RwV3d*>(posn), &screen, &w, &h, true, true)) {
                continue;
            }
            FogEmitSprite(light, alpha, screen, w, h, tab, (static_cast<uint32>(tab) * 0x8FCu + CTimer::GetTimeInMS()) & 0x7FFFu, true);
        }
    }
}

// 0x7002D0
void CPointLights::RenderFogEffect() {
    ZoneScoped;

    // Como el original (0x7002D0..0x700BCD): fuera de cinemáticas, estados de render fijos, buffer de sprites y una pasada por las luces con niebla (tipos de niebla 1 y 2);
    // termina vaciando el buffer (0x70CF20, salto final en el EXE).
    if (CCutsceneMgr::IsRunning()) {
        return;
    }
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(RwTextureGetRaster(gpCloudTex)));
    CSprite::InitSpriteBuffer();

    if (std::bit_cast<int32>(NumLights) > 0) {
        int32 index = 0;
        do {
            const CPointLight& light = aLights[index];
            float size, intensity;
            bool visible = true;
            if (light.m_nFogType == 1) {
                size = 9.0f;
                visible = PointLightsFogIntensityCpp(&CWeather::Foggyness, &intensity);
            } else if (light.m_nFogType == 2) {
                intensity = 0.4f;
                size = 3.0f;
            } else {
                visible = false;
            }
            if (visible) {
                if (light.m_nType == PLTYPE_DIRECTIONAL) {
                    FogDirectionalLight(light, intensity);
                } else if (light.m_nType == PLTYPE_POINTLIGHT || light.m_nType == PLTYPE_ONLYFOGEFFECT_ALWAYS || light.m_nType == PLTYPE_ONLYFOGEFFECT) {
                    FogPointLight(light, intensity, size);
                }
            }
            ++index;
        } while (index < std::bit_cast<int32>(NumLights));
    }
    CSprite::FlushSpriteBuffer();
}
