/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCoronas
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include <reversiblebugfixes/Bugs.hpp>
#include "Coronas.h"

constexpr std::array<const char[26], eCoronaType::CORONATYPE_COUNT> aCoronaSpriteNames = {
    "coronastar",
    "coronastar",
    "coronamoon",
    "coronareflect",
    "coronaheadlightline",
    "",
    "",
    "",
    "",
    "coronaringb"
}; // 0x8D4950

constexpr std::array<const char[26], eCoronaType::CORONATYPE_COUNT> aCoronaSpriteNamesm = {
    "",
    "",
    "",
    "coronareflectm",
    "",
    "",
    "",
    "",
    "",
    ""
}; // 0x8D4A58

constexpr CFlareDefinition HeadLightsFlareDef[27]{
    {  4.00f, 5.0f, { 60,  60,  60  }, 200, 4 },
    {  3.00f, 7.0f, { 40,  40,  40  }, 200, 4 },
    {  2.00f, 5.0f, { 40,  40,  40  }, 200, 4 },
    {  1.50f, 6.0f, { 90,  90,  90  }, 200, 4 },
    {  1.25f, 5.0f, { 40,  40,  40  }, 200, 4 },
    {  0.80f, 14.f, { 60,  60,  60  }, 200, 4 },
    {  0.60f, 4.0f, { 40,  40,  40  }, 200, 4 },
    {  0.25f, 10.f, { 60,  60,  60  }, 200, 4 },
    {  0.10f, 6.0f, { 30,  30,  30  }, 200, 4 },
    {  0.05f, 14.f, { 50,  50,  50  }, 200, 4 },
    { -0.03f, 3.0f, { 30,  30,  30  }, 200, 4 },
    { -0.10f, 6.0f, { 60,  60,  60  }, 200, 4 },
    { -0.30f, 5.0f, { 30,  30,  30  }, 200, 4 },
    { -0.40f, 60.f, { 30,  30,  30  }, 200, 4 },
    { -0.55f, 4.0f, { 40,  40,  40  }, 200, 4 },
    { -0.75f, 14.f, { 50,  50,  50  }, 200, 4 },
    { -0.90f, 5.2f, { 35,  35,  35  }, 200, 4 },
    { -1.00f, 11.f, { 55,  55,  55  }, 200, 4 },
    { -1.20f, 3.5f, { 35,  35,  35  }, 200, 4 },
    { -1.35f, 9.0f, { 50,  50,  50  }, 200, 4 },
    { -1.70f, 54.f, { 35,  35,  35  }, 200, 4 },
    { -2.00f, 5.0f, { 50,  50,  50  }, 200, 4 },
    { -2.50f, 4.5f, { 35,  35,  35  }, 200, 4 },
    { -3.00f, 14.f, { 50,  50,  50  }, 200, 4 },
    { -6.00f, 24.f, { 70,  70,  70  }, 200, 4 },
    { -9.00f, 14.f, { 70,  50,  70  }, 200, 4 },
    {  0.00f, 0.0f, { 255, 255, 255 }, 255, 0 }
}; // 0x8D4D88

constexpr CFlareDefinition SunFlareDef[27]{
    {  4.00f, 8.00f, { 36,  30,  24  }, 200, 4 },
    {  3.00f, 11.2f, { 24,  18,  15  }, 200, 4 },
    {  2.00f, 8.00f, { 24,  12,  12  }, 200, 4 },
    {  1.50f, 9.60f, { 54,  54,  48  }, 200, 4 },
    {  1.25f, 8.00f, { 24,  24,  18  }, 200, 4 },
    {  0.80f, 22.4f, { 36,  30,  24  }, 200, 4 },
    {  0.60f, 6.40f, { 24,  15,  12  }, 200, 4 },
    {  0.25f, 16.0f, { 36,  30,  30  }, 200, 4 },
    {  0.10f, 9.60f, { 18,  18,  18  }, 200, 4 },
    {  0.05f, 22.4f, { 36,  30,  24  }, 200, 4 },
    { -0.03f, 4.80f, { 18,  18,  18  }, 200, 4 },
    { -0.10f, 9.60f, { 42,  42,  42  }, 200, 4 },
    { -0.30f, 8.00f, { 18,  6,   6   }, 200, 4 },
    { -0.40f, 96.0f, { 18,  12,  9   }, 200, 4 },
    { -0.55f, 6.40f, { 18,  18,  12  }, 200, 4 },
    { -0.75f, 22.4f, { 42,  24,  18  }, 200, 4 },
    { -0.90f, 8.32f, { 21,  12,  18  }, 200, 4 },
    { -1.00f, 17.6f, { 42,  13,  18  }, 200, 4 },
    { -1.20f, 5.60f, { 21,  12,  12  }, 200, 4 },
    { -1.35f, 14.4f, { 42,  42,  24  }, 200, 4 },
    { -1.70f, 86.8f, { 21,  15,  15  }, 200, 4 },
    { -2.00f, 8.00f, { 48,  30,  30  }, 200, 4 },
    { -2.50f, 7.20f, { 21,  15,  12  }, 200, 4 },
    { -3.00f, 22.4f, { 42,  30,  24  }, 200, 4 },
    { -6.00f, 38.4f, { 42,  42,  30  }, 200, 4 },
    { -9.00f, 22.4f, { 42,  30,  36  }, 200, 4 },
    {  0.00f, 0.00f, { 255, 255, 255 }, 255, 0 }
}; // 0x8D4B68

constexpr std::array<uint16, MAX_NUM_CORONAS> RandomWindowVals = {
     2984, 38394, 49320,   292, 10295, 29434, 40292, 65304,
    55555, 30249, 59303, 10234, 13405, 24949, 52045, 49624,
    22984, 34394,   320, 10292, 20295, 39434, 20292, 63304,
    46555, 20249,  9303, 50234, 28405, 14949, 59045, 62624,
    12984, 34394, 55320, 12292, 14295, 39434, 30292, 55304,
    15555, 60249, 51303, 19234, 19405, 44949, 52045, 59624,
    42984, 64394, 50320, 30292,   295, 19434, 54292, 43304,
    56555, 60249, 39303, 10234, 39405, 19949, 19045, 32624
}; // 0x8D4FA8, unused

void CCoronas::InjectHooks() {
    RH_ScopedClass(CCoronas);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(RenderOutGeometryBufferForReflections, 0x6FAA10);
    RH_ScopedInstall(Init, 0x6FAA70);
    RH_ScopedInstall(Shutdown, 0x6FAB00);
    RH_ScopedInstall(Update, 0x6FADF0);
    RH_ScopedInstall(Render, 0x6FAEC0);
    RH_ScopedInstall(RenderReflections, 0x6FB630);
    RH_ScopedInstall(RenderSunReflection, 0x6FBAA0);
    RH_ScopedOverloadedInstall(RegisterCorona, "type", 0x6FC180, void(*)(uint32, CEntity*, uint8, uint8, uint8, uint8, const CVector&, float, float, RwTexture*, eCoronaFlareType, eCoronaReflType, eCoronaLOSCheck, eCoronaTrail, float, bool, float, bool, float, bool, bool));
    RH_ScopedOverloadedInstall(RegisterCorona, "texture", 0x6FC580, void(*)(uint32, CEntity*, uint8, uint8, uint8, uint8, const CVector&, float, float, eCoronaType, eCoronaFlareType, eCoronaReflType, eCoronaLOSCheck, eCoronaTrail, float, bool, float, bool, float, bool, bool));

    RH_ScopedInstall(UpdateCoronaCoors, 0x6FC4D0);
    RH_ScopedInstall(DoSunAndMoon, 0x6FC5A0);
}

static const float s_CoronaTen = 10.0f; // 0x85862C

namespace {
// Como el original: los contadores del buffer temporal son dwords en el EXE (common.h los declara uint16)
auto& s_CoronaTempIndicesStored  = StaticRef<uint32>(0xC4B954);
auto& s_CoronaTempVerticesStored = StaticRef<uint32>(0xC4B950);

// Como el original: LastCamLook es un global del EXE (0xC3EF58), no un estatico local
auto& s_CoronaLastCamLook = StaticRef<int32>(0xC3EF58);

// Como el original: el render lee las tablas de destellos del EXE (color y multiplicador de intensidad crudos)
struct CoronaFlareDefRaw {
    float Position;
    float Size;
    int16 ColorMult[3];
    int16 IntensityMult;
    int16 Sprite;
};
VALIDATE_SIZE(CoronaFlareDefRaw, 0x14);
auto& s_SunFlareDefRaw        = StaticRef<CoronaFlareDefRaw[27]>(0x8D4B68);
auto& s_HeadLightsFlareDefRaw = StaticRef<CoronaFlareDefRaw[27]>(0x8D4D88);

// Como el original: constantes del EXE que no son el float mas cercano al literal
const float CORONA_RAND_RECIPROCAL  = std::bit_cast<float>(0x38000100u); // 0x858C7C, 1 / 32767
const float SUN_REFLECTION_SCALE    = std::bit_cast<float>(0x40554FDFu); // 0x872530, 3.333f (no 1 / 0.3)
const float SUN_REFLECTION_ANGLE    = std::bit_cast<float>(0x3B490FDBu); // 0x87252C, 2 * PI / 2048
const float NEON_FADE_RECIPROCAL    = std::bit_cast<float>(0x3D888889u); // 0x863E0C, 1 / 15

// Como el original: MultiplyMatrixWithVector (0x59C890) con el orden de sumas del EXE
CVector CoronaMatrixTimesPoint(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x + m.GetPosition().x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetPosition().y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetPosition().z
    };
}

// Como el original: posicion inline de Render (0x6FAF67) y RenderReflections (0x6FB6FB); solo Render mira la
// matriz de inclinacion de las motos (vehiculo con subtipo 9 en +0x594, matriz en +0x5CC)
CVector CoronaWorldPosition(const CRegisteredCorona& c, bool checkBikeLean) {
    auto* const entity = c.m_pAttachedTo;
    if (!entity) {
        return c.m_vPosn;
    }
    const auto* const raw = reinterpret_cast<const uint8*>(entity);
    if (checkBikeLean && entity->GetIsTypeVehicle() && *reinterpret_cast<const int32*>(raw + 0x594) == 9) {
        return CoronaMatrixTimesPoint(*reinterpret_cast<const CMatrix*>(raw + 0x5CC), c.m_vPosn);
    }
    return CoronaMatrixTimesPoint(entity->GetMatrix(), c.m_vPosn);
}

// Como el original: fsin a precision completa y el x10 redondeado en la FPU (0x6FBF5C/0x6FBFBC)
float SinTimesTenX87(float angle) {
    float result;
    __asm {
        fld    angle
        fsin
        fmul   s_CoronaTen
        fstp   result
    }
    return result;
}

// Como el original: escritura directa en TempBufferVertices (RwIm3DVertex: posicion, normal sin tocar, color, u, v)
void SetSunReflectionVertex(uint32 index, float x, float y, float z, uint32 color, float u, float v) {
    auto* const vertex = reinterpret_cast<uint8*>(&TempBufferVertices) + index * 0x24;
    *reinterpret_cast<float*>(vertex + 0x00)  = x;
    *reinterpret_cast<float*>(vertex + 0x04)  = y;
    *reinterpret_cast<float*>(vertex + 0x08)  = z;
    *reinterpret_cast<uint32*>(vertex + 0x18) = color;
    *reinterpret_cast<float*>(vertex + 0x1C)  = u;
    *reinterpret_cast<float*>(vertex + 0x20)  = v;
}

// 0x6FBA20: factor de atenuacion por cercania a un punto (distancia 2D a la camara)
float SunReflectionDistanceFactor(CVector point, float radius) {
    const auto& camPos = TheCamera.GetPosition();
    const float dx     = camPos.x - point.x;
    const float dy     = camPos.y - point.y;
    const float dist   = std::sqrt(dy * dy + dx * dx);
    if (!(radius + 100.0f > dist)) {
        return 1.0f;
    }
    if (dist <= radius) { // Como el original: con NaN se usa la formula
        return 0.0f;
    }
    return (dist - radius) * 0.01f;
}
} // namespace

// Initialises coronas
// 0x6FAA70
void CCoronas::Init() {
    // Como el original: se guarda el TXD actual antes de buscar "particle" y no se lee ninguna textura sin nombre
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));
    for (auto i = 0u; i < (uint32)CORONATYPE_COUNT; i++) {
        if (!gpCoronaTexture[i] && aCoronaSpriteNames[i][0]) {
            gpCoronaTexture[i] = RwTextureRead(aCoronaSpriteNames[i], aCoronaSpriteNamesm[i]);
        }
    }
    CTxdStore::PopCurrentTxd();

    // Como el original: solo se borra el identificador de cada corona y el contador
    for (auto& corona : aCoronas) {
        corona.m_dwId = 0;
    }
    NumCoronas = 0;
}

// Terminates coronas
// 0x6FAB00
void CCoronas::Shutdown() {
    for (auto& t : gpCoronaTexture) {
        if (t) {
            RwTextureDestroy(std::exchange(t, nullptr));
        }
    }
}

enum CameraLookFlags : int32 {
    LOOK_LEFT      = 1 << 0,
    LOOK_RIGHT     = 1 << 1,
    LOOK_BACK      = 1 << 2,
    LOOK_DIRECTION = 1 << 3
};

// Updates coronas
// 0x6FADF0
void CCoronas::Update() {
    ZoneScoped;

    LightsMult = std::min(CTimer::GetTimeStep() * 0.03f + LightsMult, 1.f);

    const auto& cc = TheCamera.m_aCams[TheCamera.m_nActiveCam];

    int32 CamLook = 0;
    if (cc.m_bLookingLeft) {
        CamLook |= LOOK_LEFT;
    }
    if (cc.m_bLookingRight) {
        CamLook |= LOOK_RIGHT;
    }
    if (cc.m_bLookingBehind) {
        CamLook |= LOOK_BACK;
    }
    if (!TheCamera.GetLookDirection()) {
        CamLook |= LOOK_DIRECTION;
    }

    if (CamLook == s_CoronaLastCamLook) {
        bChangeBrightnessImmediately = std::max(bChangeBrightnessImmediately - 1, 0);
    } else {
        bChangeBrightnessImmediately = 3;
    }
    s_CoronaLastCamLook = CamLook;

    for (auto& corona : aCoronas) {
        if (corona.IsActive()) {
            corona.Update();
        }
    }
}

// 0x6FAEC0
void CCoronas::Render() {
    ZoneScoped;

    // Como el original: mitades enteras del raster (division con signo) para centrar los destellos
    const auto  raster     = RwCameraGetRaster(Scene.m_pRwCamera);
    const int32 width      = RwRasterGetWidth(raster);
    const int32 height     = RwRasterGetHeight(raster);
    const int32 halfWidth  = width / 2;
    const int32 halfHeight = height / 2;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));

    bool zTestEnable = true;
    const CoronaFlareDefRaw* flareDef = s_SunFlareDefRaw; // En el EXE el registro queda sin inicializar si el tipo no es 1 ni 2
    for (auto& c : aCoronas) {
        if (!c.m_dwId) {
            continue;
        }
        if (!c.m_FadedIntensity && !c.m_Color.a) {
            continue;
        }

        const auto pos = CoronaWorldPosition(c, true);

        CVector scrPos;
        float   scrW, scrH;
        if (!CSprite::CalcScreenCoors(pos, &scrPos, &scrW, &scrH, true, true)) {
            c.m_bOffScreen = true;
            continue;
        }

        // Como el original: fuera de pantalla si x < 0, y < 0, x > ancho o y > alto (NaN no)
        c.m_bOffScreen = false;
        if (scrPos.x < 0.0f || scrPos.y < 0.0f || (float)width < scrPos.x || (float)height < scrPos.y) {
            c.m_bOffScreen = true;
        }

        if (c.m_FadedIntensity == 0) {
            continue;
        }
        if (!(scrPos.z < c.m_fFarClip)) { // Como el original: con NaN tampoco se pinta
            continue;
        }

        const float rz = 1.0f / scrPos.z;

        // Como el original: se apaga a partir de la mitad del far clip con 1 - (z - mitad) / mitad
        const float halfFarClip = c.m_fFarClip * 0.5f;
        const float fade        = scrPos.z < halfFarClip ? 1.0f : 1.0f - (scrPos.z - halfFarClip) / halfFarClip;
        const int32 intensity   = (int32)((float)c.m_FadedIntensity * fade);

        if (c.m_bCheckObstacles == zTestEnable) {
            zTestEnable = !zTestEnable;
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(zTestEnable));
        }

        if (c.m_pTexture) {
            const float clampedZ = scrPos.z < 40.0f ? scrPos.z : 40.0f;
            const float scale    = clampedZ * CWeather::Foggyness * 0.025f + 1.0f;

            if (c.m_dwId == 1) { // Como el original: se escribe aunque la proyeccion siguiente la sobrescriba
                scrPos.z = RwCameraGetFarClipPlane(Scene.m_pRwCamera) * 0.95f;
            }

            RwRenderStateSet(rwRENDERSTATEZTESTENABLE,   RWRSTATE(TRUE));
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(c.m_pTexture)));

            // Como el original: la posicion se acerca a la camara con la direccion normalizada por CVector::Normalise
            const auto& camPos = TheCamera.GetPosition();
            CVector     toCam{ pos.x - camPos.x, pos.y - camPos.y, pos.z - camPos.z };
            toCam.Normalise();
            const CVector pulled{
                pos.x - toCam.x * c.m_fNearClip,
                pos.y - toCam.y * c.m_fNearClip,
                pos.z - toCam.z * c.m_fNearClip
            };
            if (CSprite::CalcScreenCoors(pulled, &scrPos, &scrW, &scrH, true, true)) {
                const float invScale = 1.0f / scale;
                CSprite::RenderOneXLUSprite_Rotate_Aspect(
                    scrPos,
                    { scrW * c.m_fSize, scale * c.m_fSize * scrH },
                    (uint8)(int32)((float)c.m_Color.r * invScale),
                    (uint8)(int32)((float)c.m_Color.g * invScale),
                    (uint8)(int32)((float)c.m_Color.b * invScale),
                    (int16)intensity,
                    rz * 20.0f,
                    0.0f,
                    255
                );
            }
        }

        if (c.m_nFlareType == FLARETYPE_NONE) {
            continue;
        }
        if (c.m_nFlareType == FLARETYPE_SUN) {
            flareDef = s_SunFlareDefRaw;
        } else if (c.m_nFlareType == FLARETYPE_HEADLIGHTS) {
            flareDef = s_HeadLightsFlareDefRaw;
        }

        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
        // Como el original: rand() * (1 / 32767) * 0.3 + 0.7, por la intensidad y por 1 / 65536 (el color es crudo)
        const float colorMult = ((float)CGeneral::GetRandomNumber() * CORONA_RAND_RECIPROCAL * 0.3f + 0.7f) * (float)c.m_FadedIntensity * (1.0f / 65536.0f);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCoronaTexture[CORONATYPE_SHINYSTAR])));

        for (auto* def = flareDef; def->Sprite; def++) {
            const RwRGBA color{
                (uint8)(int32)((float)(c.m_Color.r * def->ColorMult[0]) * colorMult),
                (uint8)(int32)((float)(c.m_Color.g * def->ColorMult[1]) * colorMult),
                (uint8)(int32)((float)(c.m_Color.b * def->ColorMult[2]) * colorMult),
                255
            };
            CColPoint hitCP;
            CEntity*  hitEntity;
            if (!CWorld::ProcessLineOfSight(pos, TheCamera.GetPosition(), hitCP, hitEntity, false, true, true, false, false, false, false, true)) {
                const float size  = def->Size * 4.0f;
                const float halfH = (float)halfHeight;
                const float halfW = (float)halfWidth;
                CSprite::RenderBufferedOneXLUSprite2D(
                    { (scrPos.x - halfW) * def->Position + halfW, (scrPos.y - halfH) * def->Position + halfH },
                    { size, size },
                    color,
                    255,
                    255
                );
            }
        }

        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));

        if (c.m_nFlareType == FLARETYPE_HEADLIGHTS && CWeather::HeadLightsSpectrum != 0.0f && CGame::currArea == AREA_CODE_NORMAL_WORLD && s_HeadLightsFlareDefRaw[0].Sprite) {
            const float halfH       = (float)halfHeight;
            const float halfW       = (float)halfWidth;
            const int32 intensity16 = (int16)intensity; // Como el original: movsx del valor truncado
            for (auto* def = s_HeadLightsFlareDefRaw; def->Sprite; def++) {
                const int16 spriteIntensity = (int16)((def->IntensityMult * intensity16) >> 8);

                const uint8 red    = (uint8)(int32)((float)(c.m_Color.r * def->ColorMult[0]) * CWeather::HeadLightsSpectrum * (1.0f / 256.0f));
                const float redPos = def->Position + 0.05f;
                CSprite::RenderBufferedOneXLUSprite2D(
                    { (scrPos.x - halfW) * redPos + halfW, (scrPos.y - halfH) * redPos + halfH },
                    { def->Size, def->Size },
                    RwRGBA{ red, 0, 0, 255 },
                    spriteIntensity,
                    255
                );

                const uint8 blue    = (uint8)(int32)((float)(c.m_Color.b * def->ColorMult[2]) * CWeather::HeadLightsSpectrum * (1.0f / 256.0f));
                const float bluePos = def->Position - 0.05f;
                CSprite::RenderBufferedOneXLUSprite2D(
                    { (scrPos.x - halfW) * bluePos + halfW, (scrPos.y - halfH) * bluePos + halfH },
                    { def->Size, def->Size },
                    RwRGBA{ 0, 0, blue, 255 },
                    spriteIntensity,
                    255
                );
            }
        }
    }
    CSprite::FlushSpriteBuffer();

    /* NOTE/BUG: Renderstates not restored? */
}

// 0x6FB630
void CCoronas::RenderReflections() {
    if (s_DebugSettings.DisableWetRoadReflections) {
        return;
    }

    if (!s_DebugSettings.AlwaysRenderWetRoadReflections) {
        // Check if the roads are wet enough
        if (CWeather::WetRoads <= 0.f) {
            for (auto& c : aCoronas) {
                c.m_bHasValidHeightAboveGround = false;
            }
            return;
        }
    }

    CSprite::InitSpriteBuffer(); // Como el original (0x6FB665)

    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(RwTextureGetRaster(gpCoronaTexture[CORONATYPE_REFLECTION])));

    for (auto i = 0u; i < (uint32)MAX_NUM_CORONAS; i++) {
        auto& c = aCoronas[i];
        // Como el original: solo coronas activas, visibles (intensidad o alfa) y con reflejo
        if (!c.m_dwId) {
            continue;
        }
        if (!c.m_FadedIntensity && !c.m_Color.a) {
            continue;
        }
        if (!c.m_bUsesReflection) {
            continue;
        }

        const auto pos = CoronaWorldPosition(c, false);

        // Como el original: sin altura valida se busca siempre; con altura valida, uno de cada 16 fotogramas
        if (!c.m_bHasValidHeightAboveGround || ((CTimer::GetFrameCounter() + i) & 0xF) == 0) {
            CColPoint colPoint;
            CEntity*  colEntity;
            if (CWorld::ProcessVerticalLine(pos, -1000.0f, colPoint, colEntity, true, false, false, false, true, false, nullptr)) {
                c.m_bHasValidHeightAboveGround = true;
                c.m_fHeightAboveGround         = pos.z - colPoint.m_vecPoint.z;
            }
        }
        if (!c.m_bHasValidHeightAboveGround) {
            continue;
        }

        const float heightAboveGround = c.m_fHeightAboveGround;
        if (!(heightAboveGround < 20.0f)) {
            continue;
        }
        if (!(pos.z - heightAboveGround <= TheCamera.GetPosition().z)) { // Como el original: con NaN se descarta
            continue;
        }

        CVector scrPos;
        float   scrW, scrH;
        if (!CSprite::CalcScreenCoors({ pos.x, pos.y, pos.z - (heightAboveGround + heightAboveGround) }, &scrPos, &scrW, &scrH, true, true)) {
            continue;
        }

        const float farClip        = c.m_fFarClip * 0.75f;
        const float clampedFarClip = 55.0f < farClip ? 55.0f : farClip;
        if (!(scrPos.z < clampedFarClip)) {
            continue;
        }

        // Como el original: 1 - (z - mitad) / mitad acotado a [0, 1] (NaN se conserva)
        const float halfFarClip = clampedFarClip * 0.5f;
        float       fade        = 1.0f;
        if (!(scrPos.z < halfFarClip)) {
            fade = 1.0f - (scrPos.z - halfFarClip) / halfFarClip;
            if (1.0f < fade) {
                fade = 1.0f;
            } else if (fade < 0.0f) {
                fade = 0.0f;
            }
        }

        // Como el original: intensidad entera (wet * fade * (20 - altura) * 230 * 0.05) y canales (c * int16) >> 8
        const float wetRoads = s_DebugSettings.AlwaysRenderWetRoadReflections ? 1.f : CWeather::WetRoads;
        const int32 intensity = (int16)(int32)(wetRoads * fade * (20.0f - heightAboveGround) * 230.0f * 0.05f);
        CSprite::RenderBufferedOneXLUSprite(
            { scrPos.x, scrPos.y, notsa::bugfixes::PS2CoronaRendering ? scrPos.z : RwIm2DGetNearScreenZMacro() },
            { scrW * c.m_fSize * 0.75f, scrH * c.m_fSize * 2.0f },
            (uint8)((c.m_Color.r * intensity) >> 8),
            (uint8)((c.m_Color.g * intensity) >> 8),
            (uint8)((c.m_Color.b * intensity) >> 8),
            128,
            1.f / RwCameraGetNearClipPlane(Scene.m_pRwCamera),
            255
        );
    }
    CSprite::FlushSpriteBuffer();

    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
}

// 0x6FAA10
void CCoronas::RenderOutGeometryBufferForReflections() {
    if (s_CoronaTempIndicesStored) {
        LittleTest();
        if (RwIm3DTransform(TempBufferVertices.m_3d, s_CoronaTempVerticesStored, 0, 1u)) {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, s_CoronaTempIndicesStored);
            RwIm3DEnd();
        }
        s_CoronaTempVerticesStored = 0;
        s_CoronaTempIndicesStored  = 0;
    }
}

// 0x6FBAA0
void CCoronas::RenderSunReflection() {
    const auto& vecToSun = CTimeCycle::m_VectorToSun.data()[CTimeCycle::m_CurrentStoredValue];
    if (!(vecToSun.z > -0.05f)) { // Como el original: con NaN tampoco
        return;
    }

    float distToPeak = vecToSun.z - 0.25f;
    if (distToPeak < 0.0f) {
        distToPeak = -distToPeak;
    }
    float t = (0.3f - distToPeak) * SUN_REFLECTION_SCALE
            * (1.f - CWeather::CloudCoverage)
            * (1.f - CWeather::Foggyness)
            * (1.f - CWeather::Wind);
    if (!(t > 0.f)) {
        return;
    }

    // Como el original: cinco zonas (la cuarta con radio 350) y factor = (d - radio) * 0.01 hasta radio + 100
    t = SunReflectionDistanceFactor({  611.0f,   875.0f, 0.0f }, 250.0f) * t;
    t = SunReflectionDistanceFactor({ -929.0f,  2364.0f, 0.0f }, 250.0f) * t;
    t = SunReflectionDistanceFactor({ -1034.0f, 2640.0f, 0.0f }, 250.0f) * t;
    t = SunReflectionDistanceFactor({ 2372.0f, -1854.0f, 0.0f }, 350.0f) * t;
    t = SunReflectionDistanceFactor({ -1633.0f,  106.0f, 0.0f }, 250.0f) * t;
    t *= 0.25f;

    const auto& cc = CTimeCycle::m_CurrentColours;
    const auto  r  = (uint8)(int32)((float)(cc.m_nSunCoronaRed + cc.m_nSunCoreRed) * t);
    const auto  g  = (uint8)(int32)((float)(cc.m_nSunCoronaGreen + cc.m_nSunCoreGreen) * t);
    const auto  b  = (uint8)(int32)((float)(cc.m_nSunCoronaBlue + cc.m_nSunCoreBlue) * t);
    const uint32 color = 0xFF000000u | ((uint32)r << 16) | ((uint32)g << 8) | (uint32)b;

    const auto& camPos   = TheCamera.GetPosition();
    const float centerX  = vecToSun.x * 40.0f + camPos.x;
    const float centerY  = vecToSun.y * 40.0f + camPos.y;
    CVector     dir{ vecToSun.x, vecToSun.y, 0.0f };
    dir.Normalise();
    const float posZ = CWeather::Wind * 0.5f + 0.1f;

    // Parte cercana a la camara
    {
        const float a = dir.y * 30.0f;
        const float b30 = dir.x * 30.0f;
        const float c60 = dir.x * 60.0f;
        const float d60 = dir.y * 60.0f;
        SetSunReflectionVertex(0, centerX + a, centerY - b30, posZ, color, 0.0f, 1.0f);
        SetSunReflectionVertex(1, centerX - a, centerY + b30, posZ, color, 1.0f, 1.0f);
        SetSunReflectionVertex(2, c60 + a + centerX, d60 + centerY - b30, posZ, color, 0.0f, 0.5f);
        SetSunReflectionVertex(3, c60 + centerX - a, d60 + b30 + centerY, posZ, color, 1.0f, 0.5f);
        constexpr RxVertexIndex nearIndices[]{ 2, 1, 0, 2, 3, 1 };
        rng::copy(nearIndices, aTempBufferIndices);
    }

    //< 0x6FBF14 - La parte que cambia de posicion (imita el agua que fluye)
    const auto time = CTimer::GetTimeInMS();
    for (auto i = 0; i < 20; i++) {
        const auto  vertex   = (RxVertexIndex)(4 + 2 * i);
        const float offset   = (float)(i * 1'440 / 20 + 60);
        const float angle    = (float)((time + 900 * i) & 0x7FF) * SUN_REFLECTION_ANGLE;
        const float forward  = SinTimesTenX87(angle) + (float)(i * 970 / 20 + 30);
        const float offsetX  = offset * dir.x;
        const float forwardY = dir.y * forward;
        const float offsetY  = offset * dir.y;
        const float forwardX = forward * dir.x;
        SetSunReflectionVertex(vertex, forwardY + offsetX + centerX, offsetY + centerY - forwardX, posZ, color, 0.0f, 0.5f);
        SetSunReflectionVertex(vertex + 1, offsetX + centerX - forwardY, forwardX + offsetY + centerY, posZ, color, 1.0f, 0.5f);

        auto* const indices = &aTempBufferIndices[6 + 6 * i];
        indices[0] = vertex;
        indices[1] = vertex - 1;
        indices[2] = vertex - 2;
        indices[3] = vertex;
        indices[4] = vertex + 1;
        indices[5] = vertex - 1;
    }
    s_CoronaTempIndicesStored  = 6 + 6 * 20;
    s_CoronaTempVerticesStored = 4 + 2 * 20;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEFOGTYPE,           RWRSTATE(rwFOGTYPELINEAR));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(RwTextureGetRaster(gpCoronaTexture[CORONATYPE_HEADLIGHTLINE])));

    if (RwIm3DTransform(TempBufferVertices.m_3d, s_CoronaTempVerticesStored, nullptr, 1u)) {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, s_CoronaTempIndicesStored);
        RwIm3DEnd();
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));

    s_CoronaTempIndicesStored  = 0;
    s_CoronaTempVerticesStored = 0;
}

// Registers a corona effect with a custom texture.
// Creates or updates a light corona in the game world with specified properties.
// 0x6FC180
void CCoronas::RegisterCorona(
    uint32           id,
    CEntity*         attachTo,
    uint8            red,
    uint8            green,
    uint8            blue,
    uint8            intensity,
    const CVector&   pos,
    float            size,
    float            range,
    RwTexture*       texture,
    eCoronaFlareType flareType,
    eCoronaReflType  reflType,
    eCoronaLOSCheck  checkLOS,
    eCoronaTrail     usesTrails, // unused
    float            normalAngle,
    bool             neonFade,
    float            pullTowardsCam,
    bool             fullBrightAtStart,
    float            fadeSpeed,
    bool             onlyFromBelow,
    bool             whiteCore
) {
    const auto worldPos = attachTo
        ? CoronaMatrixTimesPoint(attachTo->GetMatrix(), pos)
        : pos;

    // Como el original: distancia 2D sumando y, x
    const auto& camPos = TheCamera.GetPosition();
    const float dx     = camPos.x - worldPos.x;
    const float dy     = camPos.y - worldPos.y;
    if (range * range < dy * dy + dx * dx) {
        return; // Corona is beyond far clip distance
    }

    // 0x6FC24A
    uint8 adjustedAlpha = intensity;
    if (neonFade) {
        // Como el original: distancia 3D sumando z, y, x y alfa = (d - 35) * intensidad * (1 / 15) truncado
        const float dz       = camPos.z - worldPos.z;
        const float distance = std::sqrt(dz * dz + dy * dy + dx * dx);
        if (distance < 35.0f) {
            return;
        }
        if (distance < 50.0f) {
            adjustedAlpha = (uint8)(int32)((distance - 35.0f) * (float)intensity * NEON_FADE_RECIPROCAL);
        }
    }

    // Find or allocate a corona slot
    CRegisteredCorona* corona = GetCoronaByID(id);
    if (corona) {
        // Como el original: se libera si la intensidad atenuada (no el alfa) y el alfa nuevo son cero
        if (!corona->m_FadedIntensity && !adjustedAlpha) {
            corona->SetInactive();
            --NumCoronas;
            return;
        }
    } else {
        if (!adjustedAlpha) { // Como el original: no se crea una corona nueva con alfa cero
            return;
        }
        corona = GetFree();
        if (!corona) {
            return;
        }

        // 0x6FC33D
        corona->m_FadedIntensity             = fullBrightAtStart ? 255 : 0;
        corona->m_bOffScreen                 = true;
        corona->m_bHasValidHeightAboveGround = false;
        corona->m_bJustCreated               = true;
        corona->m_dwId                       = id;
        NumCoronas++;
    }

    // Update corona properties
    corona->m_Color.Set(red, green, blue, adjustedAlpha);
    corona->m_vPosn                = pos;
    corona->m_fSize                = size;
    corona->m_fAngle               = normalAngle;
    corona->m_bRegisteredThisFrame = true;
    corona->m_fFarClip             = range;
    corona->m_fNearClip            = pullTowardsCam;
    corona->m_pTexture             = texture;
    corona->m_nFlareType           = flareType;
    corona->m_bUsesReflection      = reflType;
    corona->m_bCheckObstacles      = checkLOS;
    corona->m_fFadeSpeed           = fadeSpeed;

    // 0x6FC401
    corona->m_bFlashWhileFading  = neonFade;
    corona->m_bOnlyFromBelow     = onlyFromBelow;
    corona->m_bDrawWithWhiteCore = whiteCore;

    if (attachTo) {
        // Como el original: se registra la referencia sin limpiar la anterior
        corona->m_bAttached   = true;
        corona->m_pAttachedTo = attachTo;
        attachTo->RegisterReference(&corona->m_pAttachedTo);
    } else {
        corona->m_bAttached   = false;
        corona->m_pAttachedTo = nullptr;
    }
}

// Registers a corona effect using a predefined corona type.
// Delegates to the main RegisterCorona function with a texture from the type.
// 0x6FC580
void CCoronas::RegisterCorona(uint32 id, CEntity* attachTo, uint8 red, uint8 green, uint8 blue, uint8 intensity, const CVector& pos, float size, float range, eCoronaType coronaType, eCoronaFlareType flareType, eCoronaReflType reflType, eCoronaLOSCheck checkLOS, eCoronaTrail usesTrails, float normalAngle, bool neonFade, float pullTowardsCam, bool fullBrightAtStart, float fadeSpeed, bool onlyFromBelow, bool whiteCore) {
    // Como el original: el tipo se lee como byte (movzx)
    RegisterCorona(id, attachTo, red, green, blue, intensity, pos, size, range, gpCoronaTexture.data()[(uint8)coronaType], flareType, reflType, checkLOS, usesTrails, normalAngle, neonFade, pullTowardsCam, fullBrightAtStart, fadeSpeed, onlyFromBelow, whiteCore);
}

// 0x6FC4D0
void CCoronas::UpdateCoronaCoors(uint32 id, const CVector& pos, float range, float normalAngle) {
    // Como el original: distancia 2D sumando y, x; solo se descarta si range^2 es estrictamente menor (NaN sigue)
    const auto& camPos = TheCamera.GetPosition();
    const float dx     = camPos.x - pos.x;
    const float dy     = camPos.y - pos.y;
    if (range * range < dy * dy + dx * dx) {
        return;
    }
    if (auto* corona = GetCoronaByID(id)) {
        corona->m_vPosn  = pos;
        corona->m_fAngle = normalAngle;
    }
}

// Draw sun (Moon went to CClouds since SA)
// 0x6FC5A0
void CCoronas::DoSunAndMoon() {
    ZoneScoped;

    // Constants for sun corona parts
    constexpr float sunGlowSizeMultiplier = 2.7335f; // 0x872534
    constexpr float sunFlareSizeMultiplier = 6.0f;

    const auto& vecToSun  = CTimeCycle::m_VectorToSun.data()[CTimeCycle::m_CurrentStoredValue];
    const float distance  = CDraw::GetFarClipZ() * 0.95f;
    const auto& camPos    = TheCamera.GetPosition();
    const CVector coronaPos{ vecToSun.x * distance + camPos.x, vecToSun.y * distance + camPos.y, vecToSun.z * distance + camPos.z };

    if (!CGame::CanSeeOutSideFromCurrArea()) {
        return;
    }

    if (vecToSun.z > -0.1f) {
        const auto DoRegisterCorona = [&coronaPos](uint32 id, eCoronaFlareType flareType, float size, eCoronaLOSCheck checkLOS) {
            const auto& cc = CTimeCycle::m_CurrentColours;
            RegisterCorona(
                id,
                nullptr,
                checkLOS ? (uint8)cc.m_nSunCoronaRed : (uint8)cc.m_nSunCoreRed,
                checkLOS ? (uint8)cc.m_nSunCoronaGreen : (uint8)cc.m_nSunCoreGreen,
                checkLOS ? (uint8)cc.m_nSunCoronaBlue : (uint8)cc.m_nSunCoreBlue,
                255u,
                coronaPos,
                cc.m_fSunSize * size,
                999999.88f,
                gpCoronaTexture[CORONATYPE_SHINYSTAR],
                flareType,
                CORREFL_NONE,
                checkLOS,
                TRAIL_OFF,
                0.f,
                false,
                1.5f,
                0,
                15.f,
                false,
                false
            );
        };

        DoRegisterCorona(1, FLARETYPE_NONE, sunGlowSizeMultiplier, LOSCHECK_OFF);
        if (vecToSun.z > 0.f && CGame::currArea == AREA_CODE_NORMAL_WORLD) { // Como el original: comprueba el area de nuevo
            DoRegisterCorona(2, FLARETYPE_SUN, sunFlareSizeMultiplier, LOSCHECK_ON);
        }
    }

    CVector screenPos;
    float pX, pY;
    if (CSprite::CalcScreenCoors(coronaPos, &screenPos, &pX, &pY, true, true)) {
        SunScreenX = screenPos.x;
        SunScreenY = screenPos.y;
    } else {
        SunScreenY = 1000000.0f;
        SunScreenX = 1000000.0f;
    }
}

// Inlined, code from 0x6FC524 to 0x6FC53C
inline CRegisteredCorona* CCoronas::GetCoronaByID(int32 id) {
    for (auto& corona : aCoronas) {
        if (corona.m_dwId == id) {
            return &corona;
        }
    }
    return nullptr;
}

// Inlined, code from 0x6FC309 to 0x6FC31E
inline CRegisteredCorona* CCoronas::GetFree() {
    for (auto& corona : aCoronas) {
        if (!corona.IsActive()) {
            return &corona; /* Caller has to increase `NumCoronas` */
        }
    }
    return nullptr;
}
