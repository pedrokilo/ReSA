/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "PointLights.h"

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
    const auto* cameraPos = &TheCamera.GetPosition();
    const float pointX = point.x, pointY = point.y, pointZ = point.z;
    float offsetX, offsetY, limit, lowerLimit, distance;
    const float margin = 15.0f, fadeStart = 0.75f, fadeRange = 4.0f, one = 1.0f;
    bool inside = false;
    // Como el original: comparaciones x87 estrictas, también para NaN (0x700115/0x700132).
    // Se conserva el orden z*z + y*y + x*x, y el coeficiente de color permanece en ST0.
    __asm {
        mov ecx, cameraPos
        fld pointX
        fsub dword ptr [ecx]
        fstp offsetX
        fld radius
        fadd margin
        fstp limit
        fld offsetX
        fcomp limit
        fnstsw ax
        test ah, 5
        jp pointOutside
        fld limit
        fchs
        fstp lowerLimit
        fld offsetX
        fcomp lowerLimit
        fnstsw ax
        test ah, 41h
        jne pointOutside
        fld pointY
        fsub dword ptr [ecx + 4]
        fst offsetY
        fcomp limit
        fnstsw ax
        test ah, 5
        jp pointOutside
        fld offsetY
        fcomp lowerLimit
        fnstsw ax
        test ah, 41h
        jne pointOutside
        mov inside, 1
    pointOutside:
    }
    if (!inside || std::bit_cast<int32>(NumLights) >= MAX_POINT_LIGHTS) {
        return;
    }
    inside = false;
    __asm {
        mov ecx, cameraPos
        fld pointZ
        fsub dword ptr [ecx + 8]
        fld pointZ
        fsub dword ptr [ecx + 8]
        fmulp st(1), st
        fld offsetY
        fmul offsetY
        faddp st(1), st
        fld offsetX
        fmul offsetX
        faddp st(1), st
        fsqrt
        fst distance
        fcomp limit
        fnstsw ax
        test ah, 5
        jp lightOutside
        mov inside, 1
        fld limit
        fmul fadeStart
        fcomp distance
        fnstsw ax
        test ah, 41h
        jne attenuateLight
        jmp lightOutside
    attenuateLight:
        fld distance
        fdiv limit
        fsub fadeStart
        fmul fadeRange
        fsubr one
        fld red
        fmul st, st(1)
        fstp red
        fld green
        fmul st, st(1)
        fstp green
        fmul blue
        fstp blue
    lightOutside:
    }
    if (!inside) {
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
