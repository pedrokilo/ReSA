/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "PointLights.h"
#include "X87Math.h"

void CPointLights::InjectHooks() {
    RH_ScopedClass(CPointLights);
    RH_ScopedCategoryGlobal();
    RH_ScopedInstall(Init, 0x6FFB40);
    RH_ScopedInstall(RemoveLightsAffectingObject, 0x6FFFE0);
    RH_ScopedInstall(AddLight, 0x7000E0);
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
    return plugin::CallAndReturn<float, 0x6FFBB0, const CVector*, float*, CEntity*>(point, totalLighting, entity);
}

// 0x6FFE70
float CPointLights::GetLightMultiplier(const CVector* point) {
    return plugin::CallAndReturn<float, 0x6FFE70, const CVector*>(point);
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
    return plugin::CallAndReturn<bool, 0x6FFFF0, CVector, float*>(point, outZ);
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

// 0x7002D0
void CPointLights::RenderFogEffect() {
    ZoneScoped;

    plugin::Call<0x7002D0>();
}
