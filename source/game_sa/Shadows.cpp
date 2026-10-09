/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CShadows
 * Reconstrucción: COMPLETA — 31/31 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 10.
 * Validación registrada: 31/31 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   AddPermanentShadow | 0x00706F60 | S
 *   CastRealTimeShadowSectorList<CPtrListSingleLink<CPhysical*>> | 0x0070A7E0 | S
 *   CastShadowEntityXY | 0x007086B0 | S
 *   CastShadowEntityXYZ | 0x0070A040 | S
 *   CastShadowSectorList<CPtrListSingleLink<CPhysical*>> | 0x0070A630 | S
 *   GeneratePolysForStaticShadow | 0x0070B730 | S
 *   ShadowRenderTriangleCB | 0x00709CF0 | S
 *   StoreShadowForVehicle | 0x0070BDA0 | S
 *   StoreStaticShadow | 0x0070BA00 | S
 *   UpdatePermanentShadows | 0x0070C950 | S
 *
 * ESTADO: CStaticShadow
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
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
#include <imgui.h>
#include "Shadows.h"
#include "FireManager.h"
#include <CustomBuildingDNPipeline.h>

static void ShadowRenderTriangleRegisterEntry();

void CShadows::InjectHooks() {
    RH_ScopedClass(CShadows);
    RH_ScopedCategory("Shadows");

    RH_ScopedInstall(Init, 0x706CD0);
    RH_ScopedInstall(Shutdown, 0x706ED0);
    RH_ScopedInstall(TidyUpShadows, 0x707770);
    RH_ScopedInstall(AddPermanentShadow, 0x706F60);
    RH_ScopedInstall(UpdatePermanentShadows, 0x70C950);
    RH_ScopedOverloadedInstall(StoreShadowToBeRendered, "Texture", 0x707390, void(*)(uint8, RwTexture*, const CVector&, float, float, float, float, int16, uint8, uint8, uint8, float, bool, float, CRealTimeShadow*, bool));
    RH_ScopedOverloadedInstall(StoreShadowToBeRendered, "Type", 0x707930, void(*)(uint8, const CVector&, float, float, float, float, int16, uint8, uint8, uint8));
    RH_ScopedInstall(SetRenderModeForShadowType, 0x707460);
    RH_ScopedInstall(RemoveOilInArea, 0x7074F0);
    RH_ScopedInstall(GunShotSetsOilOnFire, 0x707550);
    RH_ScopedInstall(PrintDebugPoly, 0x7076B0);
    RH_ScopedInstall(CalcPedShadowValues, 0x7076C0);
    RH_ScopedInstall(AffectColourWithLighting, 0x707850);
    RH_ScopedInstall(StoreShadowForPedObject, 0x707B40);
    RH_ScopedInstall(StoreRealTimeShadow, 0x707CA0);
    RH_ScopedInstall(UpdateStaticShadows, 0x707F40);
    RH_ScopedInstall(RenderExtraPlayerShadows, 0x707FA0);
    RH_ScopedInstall(RenderStaticShadows, 0x708300);
    RH_ScopedInstall(CastShadowEntityXY, 0x7086B0);
    RH_ScopedInstall(CastShadowEntityXYZ, 0x70A040);
    RH_ScopedInstall(CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>>, 0x70A470);
    RH_ScopedInstall(CastShadowSectorList<CPtrListSingleLink<CPhysical*>>, 0x70A630);
    RH_ScopedInstall(CastRealTimeShadowSectorList<CPtrListSingleLink<CPhysical*>>, 0x70A7E0);
    RH_ScopedInstall(RenderStoredShadows, 0x70A960);
    RH_ScopedInstall(GeneratePolysForStaticShadow, 0x70B730);
    RH_ScopedInstall(StoreStaticShadow, 0x70BA00);
    RH_ScopedInstall(StoreShadowForVehicle, 0x70BDA0);
    RH_ScopedInstall(StoreCarLightShadow, 0x70C500);
    RH_ScopedInstall(StoreShadowForPole, 0x70C750);
    RH_ScopedInstall(RenderIndicatorShadow, 0x70CCB0);
    RH_ScopedNamedGlobalInstall(ShadowRenderTriangleRegisterEntry, "ShadowRenderTriangleCB", 0x709CF0);
}

void CStaticShadow::InjectHooks() {
    RH_ScopedClass(CStaticShadow);
    RH_ScopedCategory("Shadows");

    RH_ScopedInstall(Free, 0x707670);
    //RH_ScopedInstall(Init, 0x0);
}

// 0x707670 
void CStaticShadow::Free() {
    if (m_pPolyBunch) {
        const auto prevHead = CShadows::pEmptyBunchList;
        CShadows::pEmptyBunchList = m_pPolyBunch;

        // Find last in the list and make it point to the previous head
        auto it{ m_pPolyBunch };
        while (it->m_pNext) {
            it = static_cast<CPolyBunch*>(it->m_pNext);
        }
        it->m_pNext = prevHead;
    }
    // Como el original: el puntero y el ID se ponen a cero siempre, también cuando no había poly bunch.
    m_pPolyBunch = nullptr;
    m_nId = 0;
}

// 0x706CD0
void CShadows::Init() {
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));

    gpShadowCarTex         = RwTextureRead("shad_car",     nullptr);
    gpShadowPedTex         = RwTextureRead("shad_ped",     nullptr);
    gpShadowHeliTex        = RwTextureRead("shad_heli",    nullptr);
    gpShadowBikeTex        = RwTextureRead("shad_bike",    nullptr);
    gpShadowBaronTex       = RwTextureRead("shad_rcbaron", nullptr);
    gpShadowExplosionTex   = RwTextureRead("shad_exp",     nullptr);
    gpShadowHeadLightsTex  = RwTextureRead("headlight",    nullptr);
    gpShadowHeadLightsTex2 = RwTextureRead("headlight1",   nullptr);
    gpBloodPoolTex         = RwTextureRead("bloodpool_64", nullptr);
    gpHandManTex           = RwTextureRead("handman",      nullptr);
    gpCrackedGlassTex      = RwTextureRead("wincrack_32",  nullptr);
    gpPostShadowTex        = RwTextureRead("lamp_shad_64", nullptr);

    CTxdStore::PopCurrentTxd();

    g_ShadowVertices = { 0, 2, 1, 0, 3, 2, 0, 4, 3, 0, 5, 4, 0, 6, 5, 0, 7, 6, 0, 8, 7, 0, 9, 8 };

    std::ranges::for_each(aStaticShadows, [&](auto& shadow) { shadow.Init(); });

    pEmptyBunchList = aPolyBunches.data();

    for (auto i = 0u; i < aPolyBunches.size() - 1; i++) {
        aPolyBunches[i].m_pNext = &aPolyBunches[i + 1];
    }
    aPolyBunches.back().m_pNext = nullptr;

    std::ranges::for_each(aPermanentShadows, [&](auto& shadow) { shadow.Init(); });
}

// 0x706ED0
void CShadows::Shutdown() {
    RwTextureDestroy(gpShadowCarTex);
    RwTextureDestroy(gpShadowPedTex);
    RwTextureDestroy(gpShadowHeliTex);
    RwTextureDestroy(gpShadowBikeTex);
    RwTextureDestroy(gpShadowBaronTex);
    RwTextureDestroy(gpShadowExplosionTex);
    RwTextureDestroy(gpShadowHeadLightsTex);
    RwTextureDestroy(gpShadowHeadLightsTex2);
    RwTextureDestroy(gpBloodPoolTex);
    RwTextureDestroy(gpHandManTex);
    RwTextureDestroy(gpCrackedGlassTex);
    RwTextureDestroy(gpPostShadowTex);
}

// 0x707770
void CShadows::TidyUpShadows() {
    std::ranges::for_each(
        aPermanentShadows,
        [&](auto& shadow) { shadow.m_nType = SHADOW_NONE; }
    );
}

// 0x706F60
void CShadows::AddPermanentShadow(uint8 type, RwTexture* texture, CVector* posn, float topX, float topY, float rightX, float rightY, int16 intensity, uint8 red, uint8 greeb, uint8 blue, float drawDistance, uint32 time, float upDistance) {
    size_t chosen = aPermanentShadows.size();
    for (size_t i = 0; i < aPermanentShadows.size(); ++i) {
        if (aPermanentShadows[i].m_nType == SHADOW_NONE) {chosen = i; break;}
    }
    if (chosen == aPermanentShadows.size()) {
        uint32 oldest = UINT32_MAX;
        for (size_t i = 0; i < aPermanentShadows.size(); ++i) {
            const auto& shadow = aPermanentShadows[i];
            if (double(shadow.m_fFrontX) * shadow.m_fFrontX + double(shadow.m_fFrontY) * shadow.m_fFrontY < 0.25
                && double(shadow.m_fSideX) * shadow.m_fSideX + double(shadow.m_fSideY) * shadow.m_fSideY < 0.25
                && shadow.m_nTimeCreated < oldest) {chosen = i; oldest = shadow.m_nTimeCreated;}
        }
    }
    if (chosen == aPermanentShadows.size()) return;
    auto& shadow = aPermanentShadows[chosen];
    shadow.m_nType = static_cast<eShadowType>(type);
    shadow.m_pTexture = texture;
    shadow.m_vecPosn = *posn;
    shadow.m_fFrontX = topX; shadow.m_fFrontY = topY;
    shadow.m_fSideX = rightX; shadow.m_fSideY = rightY;
    shadow.m_nIntensity = uint16(intensity);
    shadow.m_nRed = red; shadow.m_nGreen = greeb; shadow.m_nBlue = blue;
    shadow.m_fZDistance = drawDistance;
    shadow.m_nTimeDuration = time;
    shadow.m_fScale = upDistance;
    shadow.m_nTimeCreated = CTimer::GetTimeInMS();
}

static int32 PermanentShadowFadeInteger(double elapsed, double duration, int32 value) {
    int32 result; uint16 saved, truncated;
    __asm {
        fld elapsed
        fld duration
        fdivp st(1), st
        fsubr dword ptr ds:[0x858624]
        fild value
        fmulp st(1), st
        fnstcw saved
        mov ax, saved
        or ax, 0x0C00
        mov truncated, ax
        fldcw truncated
        fistp result
        fldcw saved
    }
    return result;
}

// 0x70C950
void CShadows::UpdatePermanentShadows() {
    ZoneScoped;

    const auto now = CTimer::GetTimeInMS();
    for (auto& shadow : aPermanentShadows) {
        if (shadow.m_nType == SHADOW_NONE) continue;
        const auto elapsed = now - shadow.m_nTimeCreated;
        if (elapsed >= shadow.m_nTimeDuration) {shadow.m_nType = SHADOW_NONE; continue;}
        const uint32 fadeStart = (shadow.m_nTimeDuration * 3u) >> 2;
        int16 intensity = int16(shadow.m_nIntensity);
        uint8 red = shadow.m_nRed, green = shadow.m_nGreen, blue = shadow.m_nBlue;
        if (elapsed >= fadeStart) {
            const double fadeElapsed = double(elapsed - fadeStart), fadeDuration = double(shadow.m_nTimeDuration >> 2);
            intensity = int16(PermanentShadowFadeInteger(fadeElapsed, fadeDuration, intensity));
            red = uint8(PermanentShadowFadeInteger(fadeElapsed, fadeDuration, red)); green = uint8(PermanentShadowFadeInteger(fadeElapsed, fadeDuration, green)); blue = uint8(PermanentShadowFadeInteger(fadeElapsed, fadeDuration, blue));
        }
        if (!StoreStaticShadow(uint32(uintptr_t(&shadow)), shadow.m_nType, shadow.m_pTexture, shadow.m_vecPosn,
            shadow.m_fFrontX, shadow.m_fFrontY, shadow.m_fSideX, shadow.m_fSideY, intensity, red, green, blue,
            shadow.m_fZDistance, 1.0f, 40.0f, false, 0.0f) && shadow.m_nType != SHADOW_OIL_5) {
            shadow.m_nType = SHADOW_NONE;
        }
    }
    if (CTimer::GetFrameCounter() & 3) return;
    for (auto& source : aPermanentShadows) {
        if (source.m_nType != SHADOW_OIL_2) continue;
        source.m_nType = SHADOW_OIL_3; source.m_nTimeCreated = now; source.m_nTimeDuration = 2000;
        int32 first = -1; float shortest = 3.0f;
        for (size_t i = 0; i < aPermanentShadows.size(); ++i) {
            const auto& shadow = aPermanentShadows[i];
            if (shadow.m_nType != SHADOW_OIL_1 && shadow.m_nType != SHADOW_OIL_5) continue;
            const float distance = (source.m_vecPosn - shadow.m_vecPosn).Magnitude();
            if (distance < shortest) {first = int32(i); shortest = distance;}
        }
        if (first < 0) continue;
        auto& firstShadow = aPermanentShadows[first];
        firstShadow.m_nType = SHADOW_OIL_4;
        gFireManager.StartFire(firstShadow.m_vecPosn, 1.8f, 0, nullptr, 2000, 0, 1);
        int32 second = -1; shortest = 3.0f;
        for (size_t i = 0; i < aPermanentShadows.size(); ++i) {
            const auto& shadow = aPermanentShadows[i];
            if (shadow.m_nType != SHADOW_OIL_1 && shadow.m_nType != SHADOW_OIL_5) continue;
            const float distance = (source.m_vecPosn - shadow.m_vecPosn).Magnitude();
            const auto& p = shadow.m_vecPosn; const auto& origin = source.m_vecPosn; const auto& firstPos = firstShadow.m_vecPosn;
            const double dot = (double(p.x) - origin.x) * (double(firstPos.x) - origin.x)
                + (double(p.y) - origin.y) * (double(firstPos.y) - origin.y);
            if (distance < shortest && dot < 0.0) {second = int32(i); shortest = distance;}
        }
        if (second >= 0) {auto& shadow = aPermanentShadows[second]; shadow.m_nType = SHADOW_OIL_4; gFireManager.StartFire(shadow.m_vecPosn, 1.8f, 0, nullptr, 2000, 0, 1);}
    }
    for (auto& shadow : aPermanentShadows) {
        if (shadow.m_nType == SHADOW_OIL_4) shadow.m_nType = SHADOW_OIL_2;
    }
}

// 0x707930
void CShadows::StoreShadowToBeRendered(uint8 type, const CVector& posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue) {
    const auto Store = [=](auto mtype, auto texture) {
        StoreShadowToBeRendered(mtype, texture, posn, frontX, frontY, sideX, sideY, intensity, red, green, blue, 15.0f, 0, 1.0f, nullptr, 0);
    };

    switch (type) {
    case SHADOW_DEFAULT:
        Store(SHADOW_TEX_CAR, gpShadowCarTex);
        break;
    case SHADOW_ADDITIVE:
        Store(SHADOW_TEX_CAR, gpShadowPedTex);
        break;
    case SHADOW_INVCOLOR:
        Store(SHADOW_TEX_PED, gpShadowExplosionTex);
        break;
    case SHADOW_OIL_1:
        Store(SHADOW_TEX_CAR, gpShadowHeliTex);
        break;
    case SHADOW_OIL_2:
        Store(SHADOW_TEX_PED, gpShadowHeadLightsTex);
        break;
    case SHADOW_OIL_3:
        Store(SHADOW_TEX_CAR, gpBloodPoolTex);
        break;
    default:
        return;
    }
}

// 0x707390
void CShadows::StoreShadowToBeRendered(uint8 type, RwTexture* texture, const CVector& posn, float topX, float topY, float rightX, float rightY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, bool drawOnWater, float scale, CRealTimeShadow* realTimeShadow, bool drawOnBuildings) {
    if (ShadowsStoredToBeRendered >= asShadowsStored.size())
        return;

    auto& shadow = asShadowsStored[ShadowsStoredToBeRendered];

    shadow.m_nType      = (eShadowType)type;
    shadow.m_pTexture   = texture;
    shadow.m_vecPosn    = posn;
    shadow.m_Front.x    = topX;
    shadow.m_Front.y    = topY;
    shadow.m_Side.x     = rightX;
    shadow.m_Side.y     = rightY;
    shadow.m_nIntensity = intensity;
    shadow.m_nRed       = red;
    shadow.m_nGreen     = green;
    shadow.m_nBlue      = blue;
    shadow.m_fZDistance = zDistance;
    shadow.m_bDrawOnWater     = drawOnWater;
    shadow.m_bDrawOnBuildings = drawOnBuildings;
    shadow.m_fScale     = scale;
    shadow.m_pRTShadow  = realTimeShadow;

    ShadowsStoredToBeRendered++;
}

void CShadows::StoreShadowToBeRendered(eShadowType type, RwTexture* tex, const CVector& posn, CVector2D top, CVector2D right, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, bool drawOnWater, float scale, CRealTimeShadow* realTimeShadow, bool drawOnBuildings) {
    StoreShadowToBeRendered(
        type,
        tex,
        posn,
        top.x, top.y,
        right.x, right.y,
        intensity,
        red, green, blue,
        zDistance,
        drawOnWater,
        scale,
        realTimeShadow,
        drawOnBuildings
    );
}

// 0x707460
void CShadows::SetRenderModeForShadowType(eShadowType type) {
    switch (type) {
    case SHADOW_DEFAULT:
    case SHADOW_OIL_1:
    case SHADOW_OIL_2:
    case SHADOW_OIL_3:
    /* case SHADOW_OIL_4: */ // missing
    case SHADOW_OIL_5:
        RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
        break;
    case SHADOW_ADDITIVE:
        RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDONE));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));
        break;
    case SHADOW_INVCOLOR:
        RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDZERO));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCCOLOR));
        break;
    default:
        return;
    }
}

// 0x7074F0
void CShadows::RemoveOilInArea(float minX, float maxX, float minY, float maxY) {
    for (auto& shadow : aPermanentShadows) {
        switch (shadow.m_nType) {
        case SHADOW_OIL_1:
        case SHADOW_OIL_5:
            break;
        default:
            continue;
        }
        // Como el original (0x707502-0x707538): comparaciones estrictas (fcomp + test ah,41h / test ah,5), un NaN cuenta como fuera.
        if (shadow.m_vecPosn.x > minX && shadow.m_vecPosn.x < maxX && shadow.m_vecPosn.y > minY && shadow.m_vecPosn.y < maxY) {
            shadow.m_nType = SHADOW_NONE;
        }
    }
}

// 0x707550
void CShadows::GunShotSetsOilOnFire(const CVector& shotOrigin, const CVector& shotTarget) {
    const auto V3DChangeZ = [] (const CVector& pos) {
        return CVector{ pos.x, pos.y, pos.z * 0.27f };
    };

    const CColLine line{V3DChangeZ(shotOrigin), V3DChangeZ(shotTarget)};

    // Find closest shadow (but not further than 1 unit)
    CPermanentShadow* closest{};
    auto              closestDist{1.f};
    for (auto& shdw : aPermanentShadows) {
        switch (shdw.m_nType) {
        case SHADOW_OIL_1:
        case SHADOW_OIL_5:
            break;
        default:
            continue;
        }

        const auto dist = line.DistTo(V3DChangeZ(shdw.m_vecPosn));
        if (dist < closestDist) {
            closestDist = dist;
            closest     = &shdw;
        }
    }

    if (closest) {
        closest->m_nType = SHADOW_OIL_2;
        gFireManager.StartFire(closest->m_vecPosn, 1.8f, 0, nullptr, 2000u, 0, 1);
    }
}

// 0x7076B0
void CShadows::PrintDebugPoly(CVector* a, CVector* b, CVector* c) {
    // NOP
}

// 0x7076C0
void CShadows::CalcPedShadowValues(
    CVector sunPosn,
    float& frontX,        float& frontY,
    float& sideX,         float& sideY,
    float& displacementX, float& displacementY
) {
    const auto sunDist = sunPosn.Magnitude2D();
    const auto recip = 1.0f / sunDist;

    const auto mult = (sunDist + 1.0f) * recip;
    frontX = -sunPosn.x * mult / 2.0f;
    frontY = -sunPosn.y * mult / 2.0f;

    // Como el original (0x707719-0x707721): primero y * recip y despues fchs, asi el signo del NaN (0 * inf) queda positivo.
    sideX = -(sunPosn.y * recip) / 2.0f;
    sideY = +sunPosn.x * recip / 2.0f;

    displacementX = -sunPosn.x / 2.0f;
    displacementY = -sunPosn.y / 2.0f;
}

// 0x707850
void CShadows::AffectColourWithLighting(
    eShadowType shadowType,
    uint8 dayNightIntensity, // packed 2x4 bits for day/night
    uint8 r, uint8 g, uint8 b,
    uint8& outR, uint8& outG, uint8& outB
) {
    if (shadowType != SHADOW_ADDITIVE) {
        // Como el original (0x70785E-0x7078D1): los nibbles se multiplican por la constante 1/30 (0x858F10 = 0x3D088889), no se dividen,
        // y el minimo es (A < B) ? A : B (fcom + test ah,5 + jnp), que con NaN devuelve B.
        constexpr float ONE_THIRTIETH = std::bit_cast<float>(0x3D088889u);
        const float t = CCustomBuildingDNPipeline::m_fDNBalanceParam;
        const float A = 0.4f + 0.6f * (1.f - t);
        const float B = 0.3f + 0.7f * lerp(
            (float)(dayNightIntensity >> 0 & 0b1111) * ONE_THIRTIETH,
            (float)(dayNightIntensity >> 4 & 0b1111) * ONE_THIRTIETH,
            t
        );
        const float mult = (A < B) ? A : B;
        outR = (uint8)((float)r * mult);
        outG = (uint8)((float)g * mult);
        outB = (uint8)((float)b * mult);
    } else {
        outR = r;
        outG = g;
        outB = b;
    }
}

// NOTSA
// Como el original (0x707C0D-0x707C31, 0x70809A-0x7080F0): maxStrength es un entero con signo (movsx word) y la mitad es 0.5*maxDist.
uint16 CalculateShadowStrength(float currDist, float maxDist, int32 maxStrength) {
    assert(maxDist >= currDist); // Otherwise integer underflow will occur

    const auto halfMaxDist = maxDist / 2.f;
    if (currDist >= halfMaxDist) { // Anything further than half the distance is faded out
        return (uint16)(int32)((1.f - (currDist - halfMaxDist) / halfMaxDist) * (float)maxStrength);
    } else { // Anything closer than half the max distance is full strength
        return (uint16)maxStrength;
    }
}

// 0x707B40
void CShadows::StoreShadowForPedObject(CPed* ped, float displacementX, float displacementY, float frontX, float frontY, float sideX, float sideY) {
    // Okay, so.
    // This function is called from `CCutsceneObject::PreRender`
    // And you might ask "what the fuck, an object is not a ped!!"
    // well, in R* world it is.
    // it has bones (as anything can have bones actually, that's a known fact)
    // the `GetBonePosition` below *just works* because it doesn't access any
    // `CPed` specific member variables.
    // But we really should fix this in a sensible way in the future.
    assert(ped->GetIsTypePed() || ped->GetIsTypeObject());

    // Como el original (0x707B54): llama a la sobrecarga con puntero de salida (0x5E4280), no a la que devuelve por valor.
    CVector bonePos;
    ped->GetBonePosition(&bonePos, BONE_ROOT, false);
    const auto& camPos            = TheCamera.GetPosition();
    const auto  boneToCamDist2DSq = (bonePos - camPos).SquaredMagnitude2D();

    // Check if ped is close enough
    // Como el original (0x707B9C-0x707BA7): fcomp + test ah,5 + jp -> solo sigue si dist < max (un NaN o la igualdad salen).
    if (!(boneToCamDist2DSq < MAX_DISTANCE_PED_SHADOWS_SQR)) {
        return;
    }

    const auto isPlayerPed = FindPlayerPed() == ped;

    // Check if ped is visible to the camera
    if (!isPlayerPed) {  // Optimization: Assume player ped is always visible
        if (!TheCamera.IsSphereVisible(ped->GetPosition(), 2.f)) {
            return;
        }
    }

    // Como el original: la fuerza es el valor completo (intensidad = 16 bits bajos, r/g/b = 8 bits bajos), no se trunca antes a uint8.
    const auto strength = CalculateShadowStrength(std::sqrt(boneToCamDist2DSq), MAX_DISTANCE_PED_SHADOWS, CTimeCycle::m_CurrentColours.m_nShadowStrength);
    // Como el original (0x707C3C-0x707C65): z queda tal cual (sin sumar 0.0f, que cambiaria un -0.0).
    StoreShadowToBeRendered(
        SHADOW_DEFAULT,
        gpShadowPedTex,
        CVector{bonePos.x + displacementX, bonePos.y + displacementY, bonePos.z},
        frontX, frontY,
        sideX, sideY,
        (int16)strength,
        (uint8)strength, (uint8)strength, (uint8)strength,
        4.f,
        false,
        1.f,
        nullptr,
        isPlayerPed || g_fx.GetFxQuality() >= FX_QUALITY_VERY_HIGH // NOTSA: At higher FX quality draw all ped's shadows
    );
}

// 0x707CA0
void CShadows::StoreRealTimeShadow(CPhysical* physical, float displacementX, float displacementY, float frontX, float frontY, float sideX, float sideY) {
    const auto rtshdw = physical->m_pShadowData;
    if (!rtshdw) {
        return;
    }
    const auto& camPos = TheCamera.GetPosition();
    // Como el original (0x707CF1): sobrecarga con puntero de salida (0x5E4280).
    CVector shdwPos;
    if (physical->GetIsTypePed()) {
        physical->AsPed()->GetBonePosition(&shdwPos, BONE_ROOT, false);
    } else {
        shdwPos = physical->GetPosition();
    }
    const auto shdwToCamDist2DSq = (shdwPos - camPos).SquaredMagnitude2D();

    // Check distance to camera
    // Como el original (0x707D35-0x707D44): sale si !(dist < max) (igualdad y NaN incluidos).
    if (!(shdwToCamDist2DSq < MAX_DISTANCE_PED_SHADOWS_SQR)) {
        return;
    }

    // Check if the object is visible to the camera
    if (FindPlayerPed() != physical) {  // Optimization: Assume player ped is always visible
        if (!TheCamera.IsSphereVisible(shdwPos, 2.f)) {
            return;
        }
    }

    // Como el original (0x707DBA-0x707DD9): intensidad con signo (movsx byte), la constante 0.01 se multiplica (no se divide),
    // la fuerza es un int16 con signo (movsx ax) y el entero resultante (_ftol2) no se trunca a uint8 antes de usarlo.
    const auto strength = (int16)CalculateShadowStrength(std::sqrt(shdwToCamDist2DSq), MAX_DISTANCE_PED_SHADOWS, CTimeCycle::m_CurrentColours.m_nShadowStrength);
    const auto cc       = (int32)((float)(int8)rtshdw->m_nIntensity * 0.01f * (float)strength);

    // Como el original (0x707DDE-0x707E04): textura y borde negro opaco (0x705A00) antes de calcular la luz.
    const auto tex = rtshdw->GetShadowRwTexture();
    rtshdw->DrawBorderAroundTexture(RwRGBA{0, 0, 0, 255});

    // Como el original (0x707E09-0x707E4C): fpatan x87 a precision completa; elevacion = -deg(atan2(-y, -z)) (no usa x).
    const auto& vecToSun = CTimeCycle::m_VectorToSun[CTimeCycle::m_CurrentStoredValue];
    float azimuth, elevation;
    {
        float vx = vecToSun.x, vy = vecToSun.y, vz = vecToSun.z;
        __asm {
            fld vx
            fchs
            fld vy
            fchs
            fld vz
            fchs
            fld st(1)
            fxch st(1)
            fpatan
            fmul dword ptr ds:[0x859878]
            fchs
            fstp elevation
            fpatan
            fmul dword ptr ds:[0x859878]
            fstp azimuth
        }
    }
    const auto lightFrame = rtshdw->SetLightProperties(azimuth, elevation, true);
    // Como el original (0x707E54-0x707E6F): el vector "at" del frame se normaliza (CVector::Normalise 0x59C910) antes de CalcPedShadowValues.
    CVector sunDir = *RwMatrixGetAt(RwFrameGetMatrix(lightFrame));
    sunDir.Normalise();
    // Como el original (0x707E74-0x707EAB): orden de argumentos (front, side, displacement).
    CalcPedShadowValues(
        sunDir,
        frontX, frontY,
        sideX, sideY,
        displacementX, displacementY
    );
    StoreShadowToBeRendered(
        SHADOW_INVCOLOR,
        tex,
        CVector{ shdwPos.x - displacementX * 2.5f, shdwPos.y - displacementY * 2.5f, shdwPos.z },
        frontX * 1.5f, frontY * 1.5f,
        sideX * 1.5f, sideY * 1.5f,
        (int16)cc,
        (uint8)cc, (uint8)cc, (uint8)cc,
        4.f,
        false,
        1.f,
        rtshdw,
        g_fx.GetFxQuality() >= FX_QUALITY_VERY_HIGH // NOTSA: At higher FX quality draw all shadows on buildings too
    );
}

// 0x707F40
void CShadows::UpdateStaticShadows() {
    // Remove shadows that have no polies/are temporary and have expired
    for (auto& sshdw : aStaticShadows) {
        if (!sshdw.m_pPolyBunch || sshdw.m_bJustCreated) {
            goto skip; // Not even created fully
        }

        if (sshdw.m_bTemporaryShadow && CTimer::GetTimeInMS() <= sshdw.m_nTimeCreated + 5000u) {
            goto skip; // Not expired yet
        }

        sshdw.Free();

    skip:
        sshdw.m_bJustCreated = false;
    }
}

// 0x707FA0
void CShadows::RenderExtraPlayerShadows() {
    // Como el original (0x707FA3): lee 0xB7C4EA = m_nLightShadowStrength (no m_nShadowStrength, que es 0xB7C4E8).
    if (!CTimeCycle::m_CurrentColours.m_nLightShadowStrength) {
        return;
    }

    const auto plyrVeh = FindPlayerVehicle();
    if (!plyrVeh) {
        return;
    }

    if (plyrVeh->m_nModelIndex == eModelID::MODEL_RCBANDIT) {
        return;
    }

    if (plyrVeh->GetVehicleAppearance() == VEHICLE_APPEARANCE_BIKE) {
        return;
    }

    switch (plyrVeh->m_nVehicleType) {
    case VEHICLE_TYPE_BIKE:
    case VEHICLE_TYPE_FPLANE:
    case VEHICLE_TYPE_BOAT:
        return;
    }

    const auto& plyVehMat  = plyrVeh->GetMatrix();
    for (auto& ptl : CPointLights::GetActiveLights()) {
        if (ptl.m_nType != ePointLightType::PLTYPE_POINTLIGHT) {
            continue;
        }
        if (!ptl.m_bGenerateShadows) {
            continue;
        }
        if (ptl.m_fColorRed == 0.f && ptl.m_fColorGreen == 0.f && ptl.m_fColorBlue == 0.f) { // If it's black => ignore
            continue;
        }

        // Como el original (0x70806B-0x708072): FindPlayerCoors se llama por luz, tras los filtros.
        const auto plyrPos = FindPlayerCoors();
        const auto lightToPlyr{ ptl.m_vecPosn - plyrPos };
        const auto lightToPlyrDist = lightToPlyr.Magnitude();
        // Como el original (0x708092-0x70809A): fcom + test ah,5 + jp -> sigue solo si dist < radio (NaN fuera).
        if (!(lightToPlyrDist < ptl.m_fRadius) || lightToPlyrDist == 0.f) { // NOTSA: Zero check to prevent (possible) division-by-zero
            continue;
        }

        // Como el original (0x7080F2-0x708104): la direccion se multiplica por 1/dist (CVector::operator*= 0x40FEF0), no se divide.
        const float invDist = 1.0f / lightToPlyrDist;
        const CVector lightToPlyrDir{ lightToPlyr.x * invDist, lightToPlyr.y * invDist, lightToPlyr.z * invDist };

        // Como el original (0x70810D-0x708196): FindPlayerVehicle de nuevo (posicion) y GetColModel 5 veces (max.y, min.y, max.x, min.x, max.y).
        const auto& plyrVehPos = FindPlayerVehicle()->GetPosition();
        const auto& plyrVehBB = plyrVeh->GetColModel()->GetBoundingBox();
        (void)plyrVeh->GetColModel(); (void)plyrVeh->GetColModel();
        (void)plyrVeh->GetColModel(); (void)plyrVeh->GetColModel();
        // Como el original (0x70816C-0x7081C4): tamano = max - min y mitades con * 0.5.
        const float sizeY = plyrVehBB.m_vecMax.y - plyrVehBB.m_vecMin.y;
        const float sizeX = plyrVehBB.m_vecMax.x - plyrVehBB.m_vecMin.x;
        const float halfY = sizeY * 0.5f;
        const float halfX = sizeX * 0.5f;
        const float centerOffset = halfY - plyrVehBB.m_vecMax.y;

        // Como el original (0x7080F2-0x7081D6): (pos - dir*1.2) - centerOffset*forward (antes se SUMABA el termino de forward).
        const float px = (plyrVehPos.x - lightToPlyrDir.x * 1.2f) - centerOffset * plyVehMat.GetForward().x;
        const float py = (plyrVehPos.y - lightToPlyrDir.y * 1.2f) - centerOffset * plyVehMat.GetForward().y;

        // Como el original (0x7081DA-0x7081F7): up.z > 0 (igualdad y NaN voltean el vector right).
        const bool flipped = !(plyVehMat.GetUp().z > 0.f);
        float sideX = halfX * plyVehMat.GetRight().x;
        float sideY = halfX * plyVehMat.GetRight().y;
        if (flipped) { sideX = -sideX; sideY = -sideY; }

        // Como el original (0x7080B2-0x7080C9): 5*fuerza/8 con signo (cdq/and 7/add/sar 3), no con divisor sin signo.
        const int32 maxStrength = 5 * (int32)CTimeCycle::m_CurrentColours.m_nLightShadowStrength / 8;

        StoreShadowToBeRendered(
            SHADOW_DEFAULT,
            gpShadowCarTex,
            CVector{ px, py, plyrVehPos.z },
            plyVehMat.GetForward().x * halfY, plyVehMat.GetForward().y * halfY,
            sideX, sideY,
            (int16)CalculateShadowStrength( // 0x70809A
                lightToPlyrDist,
                ptl.m_fRadius,
                maxStrength
            ),
            0, 0, 0,
            4.5f,
            false,
            1.f,
            nullptr,
            false
        );
    }
}

// 0x708300
void CShadows::RenderStaticShadows() {
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,        RWRSTATE(rwFILTERLINEAR));

    RenderBuffer::ClearRenderBuffer();

    // Mark all as not-yet-rendered
    for (auto& shdw : aStaticShadows) {
        shdw.m_bRendered = false;
    }

    // Render all in batches
    for (auto& oshdw : aStaticShadows) {
        if (!oshdw.m_pPolyBunch || oshdw.m_bRendered) {
            continue;
        }

        // Setup additional render states for this shadow
        SetRenderModeForShadowType(oshdw.m_nType);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(oshdw.m_pTexture)));
         
        // Batch all other shadows with the same texture and type into the buffer
        // Como el original (0x7083C8-0x70861E): el bucle interno empieza en la sombra actual (no en la primera) y llega hasta el final.
        for (auto ip = &oshdw; ip != aStaticShadows.data() + aStaticShadows.size(); ++ip) {
            auto& ishdw = *ip;
            if (!ishdw.m_pPolyBunch) {
                continue;
            }
            if (ishdw.m_nType != oshdw.m_nType) {
                continue;
            }
            if (ishdw.m_pTexture != oshdw.m_pTexture) {
                continue;
            }
            // No need to check if this one was rendered (because of the batching)
            
            // Render polies of this shadow
            for (auto poly = ishdw.m_pPolyBunch; poly; poly = poly->m_pNext) {
                // 0x70841F: Calculate color
                uint8 r, g, b;
                CShadows::AffectColourWithLighting(
                    ishdw.m_nType,
                    ishdw.m_nDayNightIntensity,
                    ishdw.m_nRed, ishdw.m_nGreen, ishdw.m_nBlue,
                    r, g, b
                );

                const auto totalNoIdx = 3 * (poly->m_wNumVerts - 2); // Total no. of indices we'll use

                // 0x708432: Begin render buffer store
                RwIm3DVertex*    vtxIt{};
                RwImVertexIndex* vtxIdxIt{};
                RenderBuffer::StartStoring(
                    totalNoIdx,
                    poly->m_wNumVerts,
                    vtxIdxIt, vtxIt
                );

                // 0x70851D: Write vertices (`if` not necessary, it's part of the loop condition)
                // Como el original (0x7084D6-0x708518): intensidad con signo (movsx word) y 1.0 - niebla*0.5; _ftol2 y se usa el byte bajo.
                const auto a = (uint8)(int32)((1.f - CWeather::Foggyness * 0.5f) * (float)(int16)ishdw.m_nIntensity);
                for (auto i{ 0 }; i < poly->m_wNumVerts; i++, vtxIt++) {
                    const auto& pos = poly->m_avecPosn[i];
                    RwIm3DVertexSetPos(vtxIt, pos.x, pos.y, pos.z + 0.06f);
                    RwIm3DVertexSetRGBA(vtxIt, r, g, b, a);
                    RwIm3DVertexSetU(vtxIt, (float)poly->m_aU[i] * 0.005f); // Como el original: fmul por la constante 0x858B4C (no divide por 200)
                    RwIm3DVertexSetV(vtxIt, (float)poly->m_aV[i] * 0.005f);
                }

                // 0x7085BC: Write indices  (`if` not necessary, it's part of the loop condition)
                for (auto i = 0; i < totalNoIdx; i++) {
                    *vtxIdxIt++ = g_ShadowVertices[i];
                }
                
                // Finish storing 
                RenderBuffer::StopStoring();
            }
            // Mark this as rendered (Como el original, 0x70860D: una vez tras recorrer todos los poligonos)
            ishdw.m_bRendered = true;
        }

        // Render out this batch
        RenderBuffer::RenderStuffInBuffer();
    }

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
}

// 0x7086B0
static double ShadowExtendedProduct(float a, float b);
static int32 ShadowFadeInteger(double fade, int32 value);
static float ShadowXYDot(float x,float a,float y,float b) {
    float result;
    __asm { fld x }
    __asm { fmul a }
    __asm { fld y }
    __asm { fmul b }
    __asm { faddp st(1),st(0) }
    __asm { fstp result }
    return result;
}
static float ShadowXYCross(const CVector& p,const CVector& origin,float edgeX,float edgeY) {
    float result;
    const float x=p.x,y=p.y,ox=origin.x,oy=origin.y;
    __asm { fld x }
    __asm { fsub ox }
    __asm { fmul edgeY }
    __asm { fld y }
    __asm { fsub oy }
    __asm { fmul edgeX }
    __asm { fsubp st(1),st(0) }
    __asm { fstp result }
    return result;
}
static float ShadowXYBlend(float previous,float current,float ratio) {
    const float one=1.0f; float result;
    __asm { fld one }
    __asm { fsub ratio }
    __asm { fmul previous }
    __asm { fld ratio }
    __asm { fmul current }
    __asm { faddp st(1),st(0) }
    __asm { fstp result }
    return result;
}
static float ShadowXYPlaneDistance(const CVector& n,const CVector& vertex) {
    const float nx=n.x,ny=n.y,nz=n.z,x=vertex.x,y=vertex.y,z=vertex.z;float result;
    __asm {
        fld nz
        fmul z
        fld ny
        fmul y
        faddp st(1),st(0)
        fld nx
        fmul x
        faddp st(1),st(0)
        fstp result
    }
    return result;
}
static float ShadowXYPlaneHeight(const CVector& n,float x,float y,float distance) {
    const float nx=n.x,ny=n.y,nz=n.z,minusOne=-1.0f;float result;
    __asm {
        fld minusOne
        fdiv nz
        fld ny
        fmul y
        fld nx
        fmul x
        faddp st(1),st(0)
        fsub distance
        fmulp st(1),st(0)
        fstp result
    }
    return result;
}
static int ShadowXYClip(CVector* positions,CVector* uv,int count,CVector* outPositions,CVector* outUV,const CVector& origin,const CVector& destination) {
    const float ex=destination.x-origin.x,ey=destination.y-origin.y;
    int stored=0,state=0;
    auto intersect=[&](int previous,int current,float value){
        const float before=std::abs(ShadowXYCross(positions[previous],origin,ex,ey));
        const float ratio=before/(std::abs(value)+before);
        outPositions[stored].x=ShadowXYBlend(positions[previous].x,positions[current].x,ratio);
        outPositions[stored].y=ShadowXYBlend(positions[previous].y,positions[current].y,ratio);
        outUV[stored].x=ShadowXYBlend(uv[previous].x,uv[current].x,ratio);
        outUV[stored].y=ShadowXYBlend(uv[previous].y,uv[current].y,ratio);
        ++stored;
    };
    for(int k=0;k<count;++k){
        const float value=ShadowXYCross(positions[k],origin,ex,ey);
        if(value<=0.0f){if(state==1)intersect(k-1,k,value);state=2;}
        else{
            if(state==2)intersect(k-1,k,value);
            outPositions[stored].x=positions[k].x;outPositions[stored].y=positions[k].y;
            outUV[stored].x=uv[k].x;outUV[stored].y=uv[k].y;++stored;state=1;
        }
    }
    if(count){const float first=ShadowXYCross(positions[0],origin,ex,ey);if((first>0.0f&&state==2)||(first<=0.0f&&state==1))intersect(count-1,0,first);}
    return stored;
}
void CShadows::CastShadowEntityXY(CEntity* entity, float conrerAX, float cornerAY, float cornerBX, float cornerBY, CVector* posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, float scale, CPolyBunch** ppPolyBunch, uint8* pDayNightIntensity, int32 shadowType) {
    *reinterpret_cast<uint32*>(0xC4B8C8)|=7;
    auto* model=entity->GetColModel();auto* data=model->m_pColData;
    if(!data||!data->m_nNumTriangles)return;
    CCollision::CalculateTrianglePlanes(model);
    auto& matrix=entity->GetMatrix();
    const float fx=ShadowXYDot(frontX,matrix.GetRight().x,frontY,matrix.GetRight().y);
    const float fy=ShadowXYDot(frontX,matrix.GetForward().x,frontY,matrix.GetForward().y);
    const float sx=ShadowXYDot(sideX,matrix.GetRight().x,sideY,matrix.GetRight().y);
    const float sy=ShadowXYDot(sideX,matrix.GetForward().x,sideY,matrix.GetForward().y);
    const float dx=posn->x-matrix.GetPosition().x,dy=posn->y-matrix.GetPosition().y;
    const float px=ShadowXYDot(dx,matrix.GetRight().x,dy,matrix.GetRight().y);
    const float py=ShadowXYDot(dx,matrix.GetForward().x,dy,matrix.GetForward().y);
    auto* corners=reinterpret_cast<CVector*>(0xC4B898);
    corners[0].x=(px+fx)-sx;corners[0].y=(fy+py)-sy;
    corners[1].x=px+sx+fx;corners[1].y=sy+py+fy;
    corners[2].x=sx+(px-fx);corners[2].y=(py-fy)+sy;
    corners[3].x=(px-fx)-sx;corners[3].y=(py-fy)-sy;
    const float lowX=std::min({corners[0].x,corners[1].x,corners[2].x,corners[3].x});
    const float highX=std::max({corners[0].x,corners[1].x,corners[2].x,corners[3].x});
    const float lowY=std::min({corners[0].y,corners[1].y,corners[2].y,corners[3].y});
    const float highY=std::max({corners[0].y,corners[1].y,corners[2].y,corners[3].y});
    const float roof=posn->z-matrix.GetPosition().z,floorZ=roof-zDistance;
    auto* uvA=reinterpret_cast<CVector*>(0xC4B6B8);auto* uvB=reinterpret_cast<CVector*>(0xC4B730);
    auto* positionsA=reinterpret_cast<CVector*>(0xC4B7A8);auto* positionsB=reinterpret_cast<CVector*>(0xC4B820);
    const int groups=data->bHasFaceGroups?*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(data->m_pTriangles)-4):1;
    for(int group=0;group<groups;++group){
        int first=0,last=data->m_nNumTriangles;
        if(data->bHasFaceGroups){const auto* bytes=reinterpret_cast<const uint8*>(data->m_pTriangles)-group*28;const auto* bounds=reinterpret_cast<const float*>(bytes-32);
            if(!(bounds[0]<highX&&lowX<bounds[3]&&bounds[1]<highY&&lowY<bounds[4]))continue;
            first=*reinterpret_cast<const int16*>(bytes-8);last=*reinterpret_cast<const int16*>(bytes-6)+1;
        }
        for(int index=first;index<last;++index){
            const auto& tri=data->m_pTriangles[index];const auto& plane=data->m_pTrianglePlanes[index];const CVector normal=plane.GetNormal();
            if(!(std::abs(normal.z)>.1f))continue;
            CVector vertices[3];data->GetTrianglePoint(vertices[0],tri.vA);data->GetTrianglePoint(vertices[1],tri.vB);data->GetTrianglePoint(vertices[2],tri.vC);
            if(!(rng::any_of(vertices,[&](auto&v){return lowX<v.x;})&&rng::any_of(vertices,[&](auto&v){return v.x<highX;})&&rng::any_of(vertices,[&](auto&v){return lowY<v.y;})&&rng::any_of(vertices,[&](auto&v){return v.y<highY;})&&rng::any_of(vertices,[&](auto&v){return v.z<roof;})&&rng::any_of(vertices,[&](auto&v){return floorZ<v.z;})))continue;
            for(int k=0;k<4;++k){positionsA[k].x=corners[k].x;positionsA[k].y=corners[k].y;uvA[k].x=k==1||k==2?1.0f:0.0f;uvA[k].y=k>=2?1.0f:0.0f;}
            int count=ShadowXYClip(positionsA,uvA,4,positionsB,uvB,vertices[0],vertices[1]);
            count=ShadowXYClip(positionsB,uvB,count,positionsA,uvA,vertices[1],vertices[2]);
            count=ShadowXYClip(positionsA,uvA,count,positionsB,uvB,vertices[2],vertices[0]);
            if(count<=2)continue;
            const float planeDistance=ShadowXYPlaneDistance(normal,vertices[0]);
            for(int k=0;k<count;++k){auto&v=positionsB[k];v.z=ShadowXYPlaneHeight(normal,v.x,v.y,planeDistance);const float localX=v.x;v.x=ShadowXYDot(matrix.GetRight().x,v.x,matrix.GetForward().x,v.y)+matrix.GetPosition().x;v.y=ShadowXYDot(localX,matrix.GetRight().y,matrix.GetForward().y,v.y)+matrix.GetPosition().y;v.z+=matrix.GetPosition().z;}
            *pDayNightIntensity=std::bit_cast<uint8>(tri.m_nLight);
            if(ppPolyBunch){if(!pEmptyBunchList)continue;auto*bunch=pEmptyBunchList;pEmptyBunchList=bunch->m_pNext;bunch->m_pNext=*ppPolyBunch;*ppPolyBunch=bunch;bunch->m_wNumVerts=int16(count);for(int k=0;k<count;++k){bunch->m_avecPosn[k]=positionsB[k];bunch->m_aU[k]=uint8(ShadowFadeInteger(ShadowExtendedProduct(uvB[k].x,200.0f),1));bunch->m_aV[k]=uint8(ShadowFadeInteger(ShadowExtendedProduct(uvB[k].y,200.0f),1));}}
            else{
                RwImVertexIndex* indices;RwIm3DVertex* verticesOut;RenderBuffer::StartStoring(count*3-6,count,indices,verticesOut);uint8 r,g,b;AffectColourWithLighting(static_cast<eShadowType>(shadowType),*pDayNightIntensity,red,green,blue,r,g,b);
                for(int k=0;k<count;++k){auto&v=verticesOut[k];v.objVertex={positionsB[k].x,positionsB[k].y,positionsB[k].z+.06f};v.color=(uint32(uint8(intensity))<<24)|(uint32(r)<<16)|(uint32(g)<<8)|b;v.u=scale*uvB[k].x;v.v=scale*uvB[k].y;}
                memcpy(indices,reinterpret_cast<const void*>(0xC403A8),size_t(count*3-6)*sizeof(*indices));RenderBuffer::StopStoring();
            }
        }
    }
}

// 0x70A040
static float ShadowProjectionRatio(float numerator, float value, float multiplier) {
    float result;
    __asm { fld value }
    __asm { fmul multiplier }
    __asm { fdivr numerator }
    __asm { fstp result }
    return result;
}
static float ShadowProjectionCoordinate(float base, float value, float multiplier) {
    float result;
    __asm { fld value }
    __asm { fmul multiplier }
    __asm { fadd base }
    __asm { fstp result }
    return result;
}
static double ShadowNormalOffset(float value) {
    const float multiplier = 0.028f;
    double result;
    __asm { fld value }
    __asm { fmul multiplier }
    __asm { fstp result }
    return result;
}
void CShadows::CastShadowEntityXYZ(CEntity* entity, CVector* posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, float scale, CPolyBunch** ppPolyBunch, CRealTimeShadow* realTimeShadow) {
    if (!realTimeShadow) return;
    auto* shadowCamera = realTimeShadow->GetShadowCamera();
    auto* model = entity->GetColModel();
    auto* data = model->m_pColData;
    if (!data) return;
    CCollision::CalculateTrianglePlanes(model);
    auto* camera = shadowCamera->m_pRwCamera;
    RwMatrix cameraMatrix = RwFrameGetMatrix(RwCameraGetFrame(camera))[0];
    const RwV3d axis{1.0f, 0.0f, 0.0f};
    RwMatrixRotate(&cameraMatrix, &axis, -45.0f, rwCOMBINEPRECONCAT);
    _ProjectionParam projection;
    projection.at = cameraMatrix.at;
    entity->GetMatrix().CopyToRwMatrix(&projection.entityMatrix);
    RwMatrixInvert(&projection.invMatrix, &cameraMatrix);
    const float xyScale = ShadowProjectionRatio(-0.5f, camera->viewWindow.x, 0.9f);
    const RwV3d stretch{xyScale, xyScale, ShadowProjectionRatio(1.0f, camera->viewWindow.x, 0.8f)};
    RwMatrixScale(&projection.invMatrix, &stretch, rwCOMBINEPOSTCONCAT);
    const RwV3d shift{0.5f, 0.0f, 0.0f};
    RwMatrixTranslate(&projection.invMatrix, &shift, rwCOMBINEPOSTCONCAT);
    projection.fade = false;
    projection.shadowValue = uint8(intensity);
    RwMatrix entityMatrix, inverseEntity;
    entity->GetMatrix().CopyToRwMatrix(&entityMatrix);
    RwMatrixInvert(&inverseEntity, &entityMatrix);
    const auto center = realTimeShadow->m_baseSphere.m_vecCenter;
    const CVector sphereWorld{ShadowProjectionCoordinate(center.x, frontX, -1.1f),
        ShadowProjectionCoordinate(center.y, frontY, -1.1f), float(double(center.z) - 0.5)};
    CVector sphereLocal;
    RwV3dTransformPoints(reinterpret_cast<RwV3d*>(&sphereLocal), reinterpret_cast<const RwV3d*>(&sphereWorld), 1, &inverseEntity);
    CColSphere sphere;
    sphere.Set(2.0f, sphereLocal, static_cast<eSurfaceType>(0), 0, tColLighting{0xFF});
    for (int32 triangle = 0; triangle < int16(data->m_nNumTriangles); ++triangle) {
        const auto& tri = data->m_pTriangles[triangle];
        CVector vertices[3];
        data->GetTrianglePoint(vertices[0], tri.vA);
        data->GetTrianglePoint(vertices[1], tri.vB);
        data->GetTrianglePoint(vertices[2], tri.vC);
        const auto& plane = data->m_pTrianglePlanes[triangle];
        if (!CCollision::TestSphereTriangle(sphere, data->m_pVertices, tri, plane)) continue;
        CVector normal = plane.GetNormal();
        const double offsetX = ShadowNormalOffset(normal.x), offsetY = ShadowNormalOffset(normal.y);
        const float offsetZ = float(ShadowNormalOffset(normal.z));
        for (auto& vertex : vertices) {
            vertex.x = float(offsetX + double(vertex.x));
            vertex.y = float(double(vertex.y) + offsetY);
            vertex.z = float(double(vertex.z) + offsetZ);
        }
        if (!ShadowRenderTriangleCB(&normal, vertices, &projection)) return;
    }
}

// 0x70A470
template<typename PtrListType>
void CShadows::CastPlayerShadowSectorList(
    PtrListType& ptrList,
    float cornerAX,
    float cornerAY,
    float cornerBX,
    float cornerBY,
    CVector* posn,
    float frontX,
    float frontY,
    float sideX,
    float sideY,
    int16 intensity,
    uint8 red,
    uint8 green,
    uint8 blue,
    float zDistance,
    float scale,
    CPolyBunch** ppPolyBunch,
    uint8* pDayNightIntensity,
    int32 shadowType
) {
    for (auto* const entity : ptrList) {
        if (entity->IsScanCodeCurrent()) {
            continue;
        }
        entity->SetCurrentScanCode();

        if (!entity->m_bUsesCollision || entity->m_bDontCastShadowsOn) {
            continue;
        }

        if (!entity->IsInCurrentArea()) {
            continue;
        }

        // If slightly tilted, ignore
        // Como el original (0x70A506-0x70A514): fcomp + test ah,41h + jne -> se salta con up.z <= 0.97 o NaN.
        if (!(entity->GetMatrix().GetUp().z > 0.97f)) {
            continue;
        }

        // 0x70A526
        // Como el original (0x70A526-0x70A56C): comparaciones estrictas contra los campos de memoria del CRect
        // (+0 left, +4 top, +8 right, +0xC bottom); un NaN descarta la entidad.
        const CRect boundRect = entity->GetBoundRect();
        if (!(cornerAX < boundRect.right)) {
            continue;
        }
        if (!(cornerBX > boundRect.left)) {
            continue;
        }
        if (!(cornerAY < boundRect.top)) {
            continue;
        }
        if (!(cornerBY > boundRect.bottom)) {
            continue;
        }

        // Quick Z height check of the bounding box
        // Como el original (0x70A57F-0x70A5BB): la colision sale del model info (no CEntity::GetColModel, que mira la especial del vehiculo)
        // y las comparaciones con NaN descartan.
        // Como el original (0x70A57F movsx edx,word ptr [esi+22h]): el indice de modelo se extiende con signo.
        const auto* cm         = CModelInfo::GetModelInfo(int32(int16(entity->m_nModelIndex)))->GetColModel();
        const auto  entityPosZ = entity->GetPosition().z;
        if (!((cm->m_boundBox.m_vecMax.z + entityPosZ) > (posn->z - zDistance))) {
            continue;
        }
        if (!((cm->m_boundBox.m_vecMin.z + entityPosZ) < posn->z)) {
            continue;
        }

        CastShadowEntityXY(
            entity,
            cornerAX,
            cornerAY,
            cornerBX,
            cornerBY,
            posn,
            frontX,
            frontY,
            sideX,
            sideY,
            intensity,
            red,
            green,
            blue,
            zDistance,
            scale,
            ppPolyBunch,
            pDayNightIntensity,
            shadowType
        );
    }
}

// 0x70A630
static double ShadowHeightSum(float a, float b) {
    double result;
    __asm { fld a }
    __asm { fadd b }
    __asm { fstp result }
    return result;
}
static double ShadowHeightDifference(float a, float b) {
    double result;
    __asm { fld a }
    __asm { fsub b }
    __asm { fstp result }
    return result;
}
template<typename PtrListType>
void CShadows::CastShadowSectorList(PtrListType& ptrList, float conrerAX, float cornerAY, float cornerBX, float cornerBY, CVector* posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, float scale, CPolyBunch** ppPolyBunch, uint8* pDayNightIntensity, int32 shadowType) {
    auto* node = ptrList.GetNode();
    while (node) {
        auto* entity = node->Item;
        node = node->Next;
        if (entity->IsScanCodeCurrent()) continue;
        entity->SetCurrentScanCode();
        if (!entity->m_bUsesCollision || !entity->IsInCurrentArea()) continue;
        if (!(entity->GetMatrix().GetUp().z > 0.97f)) continue;
        const auto rect = entity->GetBoundRect();
        if (!(conrerAX < rect.right && rect.left < cornerBX && cornerAY < rect.top && rect.bottom < cornerBY)) continue;
        const auto* col = entity->GetColModel();
        const float height = entity->GetPosition().z;
        if (!(ShadowHeightDifference(posn->z, zDistance) < ShadowHeightSum(col->m_boundBox.m_vecMax.z, height))) continue;
        if (!(ShadowHeightSum(col->m_boundBox.m_vecMin.z, height) < posn->z)) continue;
        CastShadowEntityXY(entity, conrerAX, cornerAY, cornerBX, cornerBY, posn, frontX, frontY, sideX, sideY, intensity, red, green, blue, zDistance, scale, ppPolyBunch, pDayNightIntensity, shadowType);
    }
}

// 0x70A7E0
template<typename PtrListType>
void CShadows::CastRealTimeShadowSectorList(PtrListType& ptrList, float conrerAX, float cornerAY, float cornerBX, float cornerBY, CVector* posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistance, float scale, CPolyBunch** ppPolyBunch, CRealTimeShadow* realTimeShadow, uint8* pDayNightIntensity) {
    auto* node = ptrList.GetNode();
    while (node) {
        auto* entity = node->Item;
        node = node->Next;
        if (entity->IsScanCodeCurrent()) continue;
        entity->SetCurrentScanCode();
        if (!entity->m_bUsesCollision || entity->m_bDontCastShadowsOn || !entity->IsInCurrentArea()) continue;
        const auto rect = entity->GetBoundRect();
        if (!(conrerAX < rect.right && rect.left < cornerBX && cornerAY < rect.top && rect.bottom < cornerBY)) continue;
        const auto* col = entity->GetColModel();
        const float height = entity->GetPosition().z;
        if (!(ShadowHeightDifference(posn->z, zDistance) < ShadowHeightSum(col->m_boundBox.m_vecMax.z, height))) continue;
        if (!(ShadowHeightSum(col->m_boundBox.m_vecMin.z, height) < posn->z)) continue;
        CastShadowEntityXYZ(entity, posn, frontX, frontY, sideX, sideY, intensity, red, green, blue, zDistance, scale, ppPolyBunch, realTimeShadow);
    }
}

// 0x70A960
static float ShadowRectangleEdge(float position, float front, float side, bool maximum);
static int32 ShadowSectorIndex(float coordinate, bool maximum);
static int32 ShadowFtol(double value);
void CShadows::RenderStoredShadows() {
    // Como el original (0x70A960-0x70A9E0): los render states y el buffer se preparan aunque no haya sombras guardadas
    // (antes habia un return anticipado que omitia esas llamadas).
    RenderBuffer::ClearRenderBuffer();

    for (auto i = 0; i < ShadowsStoredToBeRendered; i++) {
        asShadowsStored[i].m_bAlreadyRenderedInBatch = false;
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(rwRENDERSTATENARENDERSTATE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,       RWRSTATE(rwTEXTUREADDRESSCLAMP));
    RwRenderStateSet(rwRENDERSTATECULLMODE,             RWRSTATE(rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,        RWRSTATE(rwFILTERLINEAR));

    // Como el original (0x70AA76-0x70AAD6): rectangulo SIMETRICO posicion -/+ (|side| + |front|) (antes era posicion .. posicion + extension).
    struct ShadowRect { float minX, minY, maxX, maxY; };
    const auto GetShadowRect = [](CRegisteredShadow& shdw) {
        return ShadowRect{
            ShadowRectangleEdge(shdw.m_vecPosn.x, shdw.m_Front.x, shdw.m_Side.x, false),
            ShadowRectangleEdge(shdw.m_vecPosn.y, shdw.m_Front.y, shdw.m_Side.y, false),
            ShadowRectangleEdge(shdw.m_vecPosn.x, shdw.m_Front.x, shdw.m_Side.x, true),
            ShadowRectangleEdge(shdw.m_vecPosn.y, shdw.m_Front.y, shdw.m_Side.y, true)
        };
    };

    // Como el original (0x70AA89-0x70AAB4 copia la posicion de la sombra del lote en [esp+58h..60h] y 0x70AE65-0x70AE83 la pasa a GetWaterLevelNoWaves):
    // la consulta de agua usa la posicion de la ULTIMA sombra recorrida por el bucle de lote (no la de la sombra externa), valor que persiste entre iteraciones.
    CVector batchPos{};

    for (auto o = 0; o < ShadowsStoredToBeRendered; o++) {
        auto& oshdw = asShadowsStored[o];

        //
        // Render onto ground (If not already)
        //
        if (!oshdw.m_bAlreadyRenderedInBatch) {
            // Como el original (0x70AA10-0x70AA3C): si la sombra ya se dibujo en un lote (bit 2 de 0x70AA16 -> jne 0x70AE5B) se salta
            // tanto SetRenderModeForShadowType como el RwRenderStateSet de la textura (antes se hacian siempre).
            // Setup additional render states for this shadow (and others in the batch below)
            SetRenderModeForShadowType(oshdw.m_nType);
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(oshdw.m_pTexture)));

            // We do a batched rendering here:
            // All shadows of the same type and texture are rendered together to save on drawcalls

            for (auto i = o; i < ShadowsStoredToBeRendered; i++) {
                auto& ishdw = asShadowsStored[i];

                if (&ishdw != &oshdw) {
                    if (ishdw.m_nType != oshdw.m_nType) {
                        continue;
                    }
                    if (ishdw.m_pTexture != oshdw.m_pTexture) {
                        continue;
                    }
                }

                batchPos = ishdw.m_vecPosn;

                CWorld::AdvanceCurrentScanCode();

                const auto ishdwRect = GetShadowRect(ishdw);

                // Do shadow casting
                // Como el original (0x70AAE2-0x70ACA8): indices de sector floor(v*0.02+60) con las mismas ramas de recorte que
                // GeneratePolysForStaticShadow; y exterior, x interior.
                const auto castInSector =
                    [&] (int32 x, int32 y) -> bool {
                        auto& sector = CWorld::GetSector(x, y);
                        // Como el original (0x70ACB2: lista = 0xB7D0B8 + indice*8): se usa la instancia de plantilla <CPhysical*> (la de 0x70A470/0x70A630/0x70A7E0),
                        // igual que GeneratePolysForStaticShadow; <CBuilding*> era otra instancia distinta de la enganchada.
                        auto& buildings = reinterpret_cast<CPtrListSingleLink<CPhysical*>&>(sector.Buildings);

                        // Como el original (0x70ACB9 lea edx,[esp+23h]): el ultimo argumento (pDayNight) de las TRES funciones es un puntero no nulo a un byte de pila.
                        uint8 unused{};
                        if (const auto rtshdw = ishdw.m_pRTShadow) {
                            CastRealTimeShadowSectorList(
                                buildings,
                                ishdwRect.minX, ishdwRect.minY,
                                ishdwRect.maxX, ishdwRect.maxY,
                                &ishdw.m_vecPosn,
                                ishdw.m_Front.x, ishdw.m_Front.y,
                                ishdw.m_Side.x, ishdw.m_Side.y,
                                ishdw.m_nIntensity,
                                ishdw.m_nRed, ishdw.m_nGreen, ishdw.m_nBlue,
                                ishdw.m_fZDistance,
                                ishdw.m_fScale,
                                nullptr,
                                rtshdw,
                                &unused
                            );
                        } else {
                            if (ishdw.m_bDrawOnBuildings) {
                                CastShadowSectorList(
                                    buildings,
                                    ishdwRect.minX, ishdwRect.minY,
                                    ishdwRect.maxX, ishdwRect.maxY,
                                    &ishdw.m_vecPosn,
                                    ishdw.m_Front.x, ishdw.m_Front.y,
                                    ishdw.m_Side.x, ishdw.m_Side.y,
                                    ishdw.m_nIntensity,
                                    ishdw.m_nRed, ishdw.m_nGreen, ishdw.m_nBlue,
                                    ishdw.m_fZDistance,
                                    ishdw.m_fScale,
                                    nullptr,
                                    &unused,
                                    ishdw.m_nType
                                );
                            } else {
                                CastPlayerShadowSectorList(
                                    buildings,
                                    ishdwRect.minX, ishdwRect.minY,
                                    ishdwRect.maxX, ishdwRect.maxY,
                                    &ishdw.m_vecPosn,
                                    ishdw.m_Front.x, ishdw.m_Front.y,
                                    ishdw.m_Side.x, ishdw.m_Side.y,
                                    ishdw.m_nIntensity,
                                    ishdw.m_nRed, ishdw.m_nGreen, ishdw.m_nBlue,
                                    ishdw.m_fZDistance,
                                    ishdw.m_fScale,
                                    nullptr,
                                    &unused,
                                    ishdw.m_nType
                                );
                            }
                        }
                        return true; // Inisde lambda -> Continue sector loop
                    };
                {
                    const int32 firstX = ShadowSectorIndex(ishdwRect.minX, false), firstY = ShadowSectorIndex(ishdwRect.minY, false);
                    const int32 lastX = ShadowSectorIndex(ishdwRect.maxX, true), lastY = ShadowSectorIndex(ishdwRect.maxY, true);
                    for (int32 y = firstY; y <= lastY; ++y) {
                        for (int32 x = firstX; x <= lastX; ++x) {
                            castInSector(x, y);
                        }
                    }
                }

                // Mark this as rendered
                ishdw.m_bAlreadyRenderedInBatch = true;
            }

            // Render out shadows (Can't batch together with next iteration, as renderstates change)
            RenderBuffer::RenderStuffInBuffer();
        }
        
        // Render onto water (If needed)
        if (oshdw.m_bDrawOnWater) {  
            float waterLevelZ{}, bigWavesZ{}, smallWavesZ{};
            if (!CWaterLevel::GetWaterLevelNoWaves(batchPos, &waterLevelZ, &bigWavesZ, &smallWavesZ)) {
                continue;
            }

            // Shadow under water?
            // Como el original (0x70AE93-0x70AE9F): fcomp + test ah,5 + jp -> sigue solo si agua < z (NaN fuera).
            if (!(waterLevelZ < oshdw.m_vecPosn.z)) {
                continue;
            }

            // Clear the buffer as this is a new render pass (separate from the other)
            RenderBuffer::ClearRenderBuffer();

            // The smaller this value the detailed the shadows, but also (exponentially?) slower
            // This value basically represents the side of a square that is projected onto the 
            const auto STEP_SIZE = 2;

            // Como el original (0x70AF4F-0x70AFE3): |v|^2 = v.y*v.y + v.x*v.x en float y recíprocos 1.0f / |v|^2 (0x858624) precalculados;
            // las coordenadas UV MULTIPLICAN por el recíproco (no dividen).
            const auto shdwSideMagSq = oshdw.m_Side.y * oshdw.m_Side.y + oshdw.m_Side.x * oshdw.m_Side.x;
            const auto shdwFrontMagSq = oshdw.m_Front.y * oshdw.m_Front.y + oshdw.m_Front.x * oshdw.m_Front.x;
            const float shdwSideRecip = 1.0f / shdwSideMagSq, shdwFrontRecip = 1.0f / shdwFrontMagSq;

            const auto oshdwRect = GetShadowRect(oshdw);

            // Como el original (0x70AEA5-0x70AF4B, floor 0x8219F0 / ceil 0x823820 + _ftol2 0x821B40: 64 bits, palabra baja): limites = floor/ceil((pos -/+ extension) * 0.5) * 2 (floor/ceil en doble precision) y
            // el bucle exterior recorre X y el interior Y, de 2 en 2 (antes: IterateSectors con y exterior y rectangulo asimetrico).
            const int32 waterMinX = ShadowFtol(std::floor((double)(oshdwRect.minX * 0.5f)));
            const int32 waterMinY = ShadowFtol(std::floor((double)(oshdwRect.minY * 0.5f)));
            const int32 waterMaxX = ShadowFtol(std::ceil((double)(oshdwRect.maxX * 0.5f)));
            const int32 waterMaxY = ShadowFtol(std::ceil((double)(oshdwRect.maxY * 0.5f)));
            const auto waterQuad =
                [&](int32 ox, int32 oy) {
                    // 0x70B000 - 0x70B0DA: Start storing 
                    RwIm3DVertex* vtxIt{};
                    RwImVertexIndex* vtxIdxIt{};
                    RenderBuffer::StartStoring(6, 4, vtxIdxIt, vtxIt);

                    // Function to process 1 vertex (out of the 4)
                    const auto ProcessOneVertex = [&] (int32 x, int32 y) {
                        // The x, y passed in are sector coords, so make them into world coords
                        x *= STEP_SIZE;
                        y *= STEP_SIZE;

                        const auto currPos = CVector2D{ (float)x, (float)y };

                        // Set color
                        RwIm3DVertexSetRGBA(vtxIt, oshdw.m_nRed, oshdw.m_nGreen, oshdw.m_nBlue, (uint8)(int32)((float)(int16)oshdw.m_nIntensity * 0.6f));

                        // Set texture coords (UV)
                        // Como el original (0x70B147-0x70B214 y analogos de los otros 3 vertices): d = (x,y) - pos (float); el producto escalar es
                        // (d.x*v.x + d.y*v.y) + (-pos.z)*0.0f (0x858B50; fld pos.z/fchs/fmul/faddp) y
                        //   U = ((dot(side) * 1/|side|^2) + 1.0f) * 0.5f          (fmul, fadd 0x858624, fmul 0x858B8C -> [vertice+0x1C])
                        //   V = 0.5f - ((dot(front) * 1/|front|^2) * 0.5f)        (fmul, fmul 0x858B8C, fsubr 0x858B8C -> [vertice+0x20])
                        // (antes: (dot / |v|^2 + 1) * 0.5 para ambos, sin el termino z*0.0 y con V sin invertir).
                        const float dx = (float)x - oshdw.m_vecPosn.x;
                        const float dy = (float)y - oshdw.m_vecPosn.y;
                        const float zTerm = (-oshdw.m_vecPosn.z) * 0.0f;
                        const float dotSide  = (dx * oshdw.m_Side.x  + dy * oshdw.m_Side.y)  + zTerm;
                        const float dotFront = (dx * oshdw.m_Front.x + dy * oshdw.m_Front.y) + zTerm;
                        RwIm3DVertexSetU(vtxIt, ((dotSide * shdwSideRecip) + 1.0f) * 0.5f);
                        RwIm3DVertexSetV(vtxIt, 0.5f - ((dotFront * shdwFrontRecip) * 0.5f));

                        // Set position
                        RwIm3DVertexSetPos(
                            vtxIt,
                            currPos.x,
                            currPos.y,
                            CWaterLevel::CalculateWavesOnlyForCoordinate2_Direct(x, y, waterLevelZ, bigWavesZ, smallWavesZ) + 0.06f // Z pos of the wave at the given coord
                        );
                        // Como el original (0x70B1AD/0x70B2ED/0x70B43E/0x70B5B8: vertices en edi+0, +24h, +48h, +6Ch): cada vertice va a su hueco (antes faltaba el avance y los 4 se escribian en el primero).
                        vtxIt++;
                    };

                    // Process 4 vertices of this square
                    ProcessOneVertex(ox,     oy    ); // top left
                    ProcessOneVertex(ox + 1, oy    ); // top right
                    ProcessOneVertex(ox,     oy + 1); // bottom left
                    ProcessOneVertex(ox + 1, oy + 1); // bottom right

                    // Copy vertices into index buffer
                    rng::copy(std::to_array({ 0, 1, 2, 1, 3, 2 }), vtxIdxIt);

                    // And we're done with this one... onto the next!
                    RenderBuffer::StopStoring();

                    // Continue iteration...
                    return true;
                };
            for (int32 wx = waterMinX; wx <= waterMaxX; ++wx) {
                for (int32 wy = waterMinY; wy <= waterMaxY; ++wy) {
                    waterQuad(wx, wy);
                }
            }

            // Render it all
            RenderBuffer::RenderStuffInBuffer();
        }
    }

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,    RWRSTATE(rwTEXTUREADDRESSWRAP));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLBACK));

    ShadowsStoredToBeRendered = 0;
}

// 0x70B730
static float ShadowRectangleEdge(float position, float front, float side, bool maximum) {
    float result;
    __asm {
        fld side
        fabs
        fld front
        fabs
        faddp st(1), st(0)
    }
    if (maximum) { __asm { fadd position } }
    else { __asm { fsubr position } }
    __asm { fstp result }
    return result;
}
static int32 ShadowFtol(double value) {
    // Como el original (_ftol2 0x821B40): truncamiento a 64 bits y se usa la palabra baja (fuera de rango / NaN -> 0x8000000000000000 -> 0)
    return static_cast<int32>(static_cast<int64>(value));
}
static int32 ShadowSectorIndex(float coordinate, bool maximum) {
    const float multiplier = 0.02f, bias = 60.0f;
    float saved;
    __asm {
        fld coordinate
        fmul multiplier
        fadd bias
        fstp saved
    }
    // Como el original (0x70AAE2-0x70ABF3 y 0x70B7B4-0x70B8E8): floor (0x8219F0) + _ftol2 (0x821B40) y recorte con compares de enteros
    // (jle 0 / jge 0x77); si no recorta se repite floor+_ftol2 sobre el mismo float (mismo valor).
    const int32 value = ShadowFtol(std::floor(double(saved)));
    if (maximum ? value >= 119 : value <= 0) return maximum ? 119 : 0;
    return value;
}
void CShadows::GeneratePolysForStaticShadow(int16 staticShadowIndex) {
    auto& shadow = aStaticShadows[staticShadowIndex];
    CVector position = shadow.m_vecPosn;
    const float lowX = ShadowRectangleEdge(position.x, shadow.m_fFrontX, shadow.m_fSideX, false);
    const float highX = ShadowRectangleEdge(position.x, shadow.m_fFrontX, shadow.m_fSideX, true);
    const float lowY = ShadowRectangleEdge(position.y, shadow.m_fFrontY, shadow.m_fSideY, false);
    const float highY = ShadowRectangleEdge(position.y, shadow.m_fFrontY, shadow.m_fSideY, true);
    const int32 firstX = ShadowSectorIndex(lowX, false), firstY = ShadowSectorIndex(lowY, false);
    const int32 lastX = ShadowSectorIndex(highX, true), lastY = ShadowSectorIndex(highY, true);
    CWorld::AdvanceCurrentScanCode();
    for (int32 y = firstY; y <= lastY; ++y) {
        for (int32 x = firstX; x <= lastX; ++x) {
            auto& list = reinterpret_cast<CPtrListSingleLink<CPhysical*>&>(CWorld::GetSector(x, y).Buildings);
            CastPlayerShadowSectorList(list, lowX, lowY, highX, highY, &position,
                shadow.m_fFrontX, shadow.m_fFrontY, shadow.m_fSideX, shadow.m_fSideY,
                0, 0, 0, 0, shadow.m_fZDistance, shadow.m_fScale,
                &shadow.m_pPolyBunch, &shadow.m_nDayNightIntensity, 0);
        }
    }
}

// 0x70BA00
static float ShadowCameraDistanceSquared(const CVector& position, const CVector& camera) {
    float result;
    const auto* p = &position;
    const auto* c = &camera;
    __asm {
        mov eax, p
        mov ecx, c
        fld dword ptr [eax+4]
        fsub dword ptr [ecx+4]
        fld dword ptr [eax+4]
        fsub dword ptr [ecx+4]
        fmulp st(1), st(0)
        fld dword ptr [eax]
        fsub dword ptr [ecx]
        fld dword ptr [eax]
        fsub dword ptr [ecx]
        fmulp st(1), st(0)
        faddp st(1), st(0)
        fstp result
    }
    return result;
}
static double ShadowDistanceFade(float squared, float distance) {
    const float threeQuarters = 0.75f, four = 4.0f, one = 1.0f;
    double result;
    __asm {
        fld squared
        fsqrt
        fld distance
        fmul threeQuarters
        fsubp st(1), st(0)
        fld four
        fdiv distance
        fmulp st(1), st(0)
        fsubr one
        fstp result
    }
    return result;
}
static double ShadowExtendedProduct(float a, float b) {
    double result;
    __asm { fld a }
    __asm { fmul b }
    __asm { fstp result }
    return result;
}
static double ShadowExtendedSqrt(float value) {
    double result;
    __asm { fld value }
    __asm { fsqrt }
    __asm { fstp result }
    return result;
}
static int32 ShadowFadeInteger(double fade, int32 value) {
    int32 result;
    __asm {
        fld fade
        fild value
        fmulp st(1), st(0)
        mov eax, 0821B40h
        call eax
        mov result, eax
    }
    return result;
}
bool CShadows::StoreStaticShadow(uint32 id, eShadowType type, RwTexture* texture, const CVector& posn, float frontX, float frontY, float sideX, float sideY, int16 intensity, uint8 red, uint8 green, uint8 blue, float zDistane, float scale, float drawDistance, bool temporaryShadow, float upDistance) {
    const float squared = ShadowCameraDistanceSquared(posn, TheCamera.GetPosition());
    if (drawDistance != 0.0f) {
        const double radiusSquared = ShadowExtendedProduct(drawDistance, drawDistance);
        if (!(squared < radiusSquared)) return true;
        if (!(ShadowExtendedSqrt(squared) < ShadowExtendedProduct(drawDistance, 0.75f))) {
            const double fade = ShadowDistanceFade(squared, drawDistance);
            intensity = int16(ShadowFadeInteger(fade, intensity));
            red = uint8(ShadowFadeInteger(fade, red));
            green = uint8(ShadowFadeInteger(fade, green));
            blue = uint8(ShadowFadeInteger(fade, blue));
        }
    }
    int32 index = -1;
    bool rebuild = true;
    for (int32 i = 0; i < 48; ++i) {
        auto& shadow = aStaticShadows[i];
        if (shadow.m_nId != id || !shadow.m_pPolyBunch) continue;
        index = i;
        const double dx = std::abs(ShadowHeightDifference(posn.x, shadow.m_vecPosn.x));
        const double dy = std::abs(ShadowHeightDifference(posn.y, shadow.m_vecPosn.y));
        const double dz = std::abs(ShadowHeightDifference(posn.z, shadow.m_vecPosn.z));
        rebuild = !((dx < upDistance && dy < upDistance) ||
            (dx < double(0.05f) && dy < double(0.05f) && dz < 2.0 &&
            frontX == shadow.m_fFrontX && frontY == shadow.m_fFrontY && sideX == shadow.m_fSideX && sideY == shadow.m_fSideY));
        if (rebuild) shadow.Free();
        break;
    }
    if (index == -1) {
        for (int32 i = 0; i < 48; ++i) if (!aStaticShadows[i].m_pPolyBunch) { index = i; break; }
        if (index == -1) return true;
    }
    auto& shadow = aStaticShadows[index];
    shadow.m_pTexture = texture;
    shadow.m_nType = type;
    shadow.m_nIntensity = uint16(intensity);
    shadow.m_nRed = red;
    shadow.m_nGreen = green;
    shadow.m_nBlue = blue;
    shadow.m_fZDistance = zDistane;
    shadow.m_fScale = scale;
    shadow.m_nTimeCreated = CTimer::GetTimeInMS();
    shadow.m_bJustCreated = true;
    shadow.m_bTemporaryShadow = temporaryShadow;
    if (!rebuild) return true;
    shadow.m_nId = id;
    shadow.m_vecPosn = posn;
    shadow.m_fFrontX = frontX;
    shadow.m_fFrontY = frontY;
    shadow.m_fSideX = sideX;
    shadow.m_fSideY = sideY;
    GeneratePolysForStaticShadow(int16(index));
    return shadow.m_pPolyBunch != nullptr;
}

// 0x70BDA0
static void ShadowVehiclePosition(CVector& position,float extentLength,float maximumY,float shift,float forwardX,float forwardY) {
    const float half=.5f;float x=position.x,y=position.y;
    __asm {
        fld extentLength
        fmul half
        fsub maximumY
        fmul shift
        fld st(0)
        fmul forwardX
        fsubr x
        fstp x
        fmul forwardY
        fsubr y
        fstp y
    }
    position.x=x;position.y=y;
}
void CShadows::StoreShadowForVehicle(CVehicle* vehicle, VEH_SHD_TYPE vehShadowType) {
    const int16 strength = CTimeCycle::GetShadowStrength();
    if (GraphicsHighQuality() || !strength) return;
    CVector position = vehicle->GetPosition();
    float squared = ShadowCameraDistanceSquared(position, TheCamera.GetPosition());
    if (CCutsceneMgr::IsRunning()) squared = float(double(squared) / (double(TheCamera.m_fLODDistMultiplier) * TheCamera.m_fLODDistMultiplier * 4.0));
    const float distance = vehShadowType == VEH_SHD_BIG_PLANE ? 288.0f :
        (vehShadowType >= VEH_SHD_HELI && vehShadowType <= VEH_SHD_RC ? 144.0f : 18.0f);
    if (!(squared < distance * distance)) return;
    int16 intensity = strength;
    if (!(ShadowExtendedSqrt(squared) < ShadowExtendedProduct(distance, .75f))) intensity = int16(ShadowFadeInteger(ShadowDistanceFade(squared, distance), strength));
    const auto& box = vehicle->GetColModel()->GetBoundingBox();
    float length = box.m_vecMax.y - box.m_vecMin.y;
    float breadth = box.m_vecMax.x - box.m_vecMin.x;
    float shift = 1.0f;
    switch (int32(int16(vehicle->m_nModelIndex))) {
    case 539: return;
    case -2: length *= .9f; breadth *= .4f; break;
    case 417: case 425: case 447: case 469: case 487: case 497:
        shift = .5f; breadth *= 3.0f; length *= 1.4f; break;
    case 465: case 501: shift = .2f; length *= 1.5f; breadth += breadth; break;
    case 463: case 468: case 523: shift = .03f; length *= 1.5f; break;
    case 448: case 461: case 462: case 481: case 509: case 510: case 521: case 522:
        shift = .05f; length *= 1.2f; break;
    }
    auto& matrix = vehicle->GetMatrix();
    ShadowVehiclePosition(position,length,box.m_vecMax.y,shift,matrix.GetForward().x,matrix.GetForward().y);
    RwTexture* texture = gpShadowCarTex;
    float height = 4.5f;
    switch (vehShadowType) {
    case VEH_SHD_CAR: break;
    case VEH_SHD_BIKE: {
        float factor = float(double(std::abs(*reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(vehicle) + 0x648))) * 5.092958f + 1.0);
        if (vehicle->GetStatus() == STATUS_ABANDONED && std::abs(matrix.GetRight().z) > .6f) factor = float(double(std::abs(matrix.GetRight().z)) * 4.0 + factor);
        breadth = float(double(factor) * breadth);
        texture = gpShadowBikeTex;
        break;
    }
    case VEH_SHD_HELI:
        texture = gpShadowHeliTex;
        if (FindPlayerVehicle() == vehicle) height = 50.0f;
        break;
    case VEH_SHD_PLANE: case VEH_SHD_BIG_PLANE:
        texture = gpShadowBaronTex; intensity = strength;
        if (FindPlayerVehicle() == vehicle) height = 50.0f;
        break;
    case VEH_SHD_RC: length *= 1.5f; breadth *= 2.2f; texture = gpShadowBaronTex; break;
    }
    float frontX = matrix.GetForward().x, frontY = matrix.GetForward().y;
    CVector vertical{0,0,1};
    CVector side;
    CVector forward = matrix.GetForward();
    CrossProduct(&side, &forward, &vertical);
    float sideX = side.x, sideY = side.y;
    const float magnitude = float(std::sqrt(double(side.x) * side.x + double(side.y) * side.y));
    if (magnitude < .5f) { sideX = float(double(side.x) / magnitude * .5); sideY = float(double(side.y) / magnitude * .5); }
    if (matrix.GetUp().z < 0.0f) { sideX = -sideX; sideY = -sideY; }
    if (vehShadowType == VEH_SHD_BIKE && std::abs(matrix.GetRight().z) > .6f) { sideX = matrix.GetUp().x; sideY = matrix.GetUp().y; }
    if (vehShadowType == VEH_SHD_HELI) {
        if (std::abs(matrix.GetRight().z) > .57f) { sideX = matrix.GetUp().x; sideY = matrix.GetUp().y; }
        if (std::abs(matrix.GetForward().z) > .57f) { frontX = matrix.GetUp().x; frontY = matrix.GetUp().y; }
    }
    const bool player = (reinterpret_cast<const uint8*>(vehicle)[0x42C] & 0x80) || FindPlayerVehicle() == vehicle;
    const float movement = float(std::sqrt(double(vehicle->m_vecMoveSpeed.x) * vehicle->m_vecMoveSpeed.x + double(vehicle->m_vecMoveSpeed.y) * vehicle->m_vecMoveSpeed.y + double(vehicle->m_vecMoveSpeed.z) * vehicle->m_vecMoveSpeed.z));
    breadth *= .5f; length *= .5f;
    const bool inverted = matrix.GetUp().z <= 0.0f;
    const float sx = float(double(breadth) * sideX), sy = float(double(breadth) * sideY);
    if (double(movement) * CTimer::GetTimeStep() <= .1f && !player) {
        StoreStaticShadow(uint32(vehicle) + 1, SHADOW_DEFAULT, texture, position, length * frontX, length * frontY, inverted ? -sx : sx, inverted ? -sy : sy, intensity, uint8(intensity), uint8(intensity), uint8(intensity), 4.5f, 1.0f, 0.0f, false, .1f);
    } else {
        StoreShadowToBeRendered(SHADOW_DEFAULT, texture, position, length * frontX, length * frontY, inverted ? -sx : sx, inverted ? -sy : sy, intensity, uint8(intensity), uint8(intensity), uint8(intensity), height, player, 1.0f, nullptr, player);
    }
}

// 0x70C500
void CShadows::StoreCarLightShadow(CVehicle* vehicle, int32 id, RwTexture* texture, const CVector& posn, float frontX, float frontY, float sideX, float sideY, uint8 red, uint8 green, uint8 blue, float maxViewAngleCosine) {
    // Maximum distance (from camera to `posn`) after which shadows aren't stored (and rendered)
    constexpr auto MAX_CAM_TO_LIGHT_DIST = 27.f;

    if ([] { // Maybe ignore camera distance?
        switch (CCamera::GetActiveCamera().m_nMode) {
        case MODE_TOPDOWN:
        case MODE_TOP_DOWN_PED:
            return false;
        }

        return !CCutsceneMgr::IsRunning();
    }()) {
        // Como el original (0x70C526-0x70C5D2): el vector es posicion - camara (antes camara - posicion), las comparaciones son
        // negadas (un NaN sale) y el producto escalar se compara contra -maxViewAngleCosine con '>' estricto.
        const auto camPos            = TheCamera.GetPosition();
        const auto shdwToCam2D       = CVector2D{ posn.x - camPos.x, posn.y - camPos.y };
        const auto shdwToCamDist2DSq = shdwToCam2D.y * shdwToCam2D.y + shdwToCam2D.x * shdwToCam2D.x;

        if (!(shdwToCamDist2DSq < sq(MAX_CAM_TO_LIGHT_DIST))) {
            return;
        }

        // Check if the camera is facing the lights closely (in which case the camera can't see the shadow)
        const auto camFront = TheCamera.GetFrontNormal2D();
        if (!((shdwToCam2D.y * camFront.y + shdwToCam2D.x * camFront.x) > -maxViewAngleCosine)) {
            return;
        }

        // If far enough from the camera, start fading out
        // Como el original (0x70C5D8-0x70C64F): '!(dist < 20.25)' y (dist - 18) * 0.11111111 (constante 0x872740, no se divide por 9).
        if (const auto dist = std::sqrt(shdwToCamDist2DSq); !(dist < MAX_CAM_TO_LIGHT_DIST * 0.75f)) {
            const auto t = 1.f - (dist - 18.0f) * 0.11111111f;
            red   = (uint8)((float)red * t);
            green = (uint8)((float)green * t);
            blue  = (uint8)((float)blue * t);
        }
    }

    // Como el original (0x70C653-0x70C6F4): primero la velocidad (> 0.4 estricto), luego FindPlayerVehicle (otra vez dentro de la rama de
    // StoreShadowToBeRendered para el flag de edificios).
    if (vehicle->GetMoveSpeed().Magnitude() * CTimer::GetTimeStep() > 0.4f || FindPlayerVehicle() == vehicle) {
        const auto isPlyrVeh = FindPlayerVehicle() == vehicle;
        StoreShadowToBeRendered(
            SHADOW_ADDITIVE,
            texture,
            posn,
            frontX, frontY,
            sideX, sideY,
            128,
            red, green, blue,
            6.f,
            false,
            1.f,
            nullptr,
            isPlyrVeh || g_fx.GetFxQuality() >= FX_QUALITY_VERY_HIGH // NOTSA: At higher FX quality draw all vehicles's shadows on buildings too
        );
    } else {
        StoreStaticShadow(
            reinterpret_cast<uint32>(vehicle) + id,
            SHADOW_ADDITIVE,
            texture,
            posn,
            frontX, frontY,
            sideX, sideY,
            128,
            red, green, blue,
            6.f,
            1.f,
            0.f,
            false,
            0.4f
        );
    }
}

// 0x70C750
void CShadows::StoreShadowForPole(CEntity* entity, float offsetX, float offsetY, float offsetZ, float poleHeight, float poleWidth, uint32 localId) {
    if (GraphicsHighQuality() || !CTimeCycle::m_CurrentColours.m_nPoleShadowStrength) {
        return;
    }

    const auto& mat = entity->GetMatrix();

    if (mat.GetUp().z < .5f) { // More than 45 deg tilted
        return;
    }

    // Como el original (0x70C7B5-0x70C7C4): ((up.z - 0.5) * fuerza) duplicado, _ftol2 a int32 completo; luego 2*int/3 con signo.
    const int32 intensity = (int32)(((mat.GetUp().z - 0.5f) * (float)CTimeCycle::m_CurrentColours.m_nPoleShadowStrength) * 2.f);

    const auto front     = CVector2D{ CTimeCycle::GetVectorToSun() } * (-poleHeight / 2.f);
    const auto right     = CVector2D{ CTimeCycle::GetShadowSide() } * poleWidth;

    // Como el original (0x70C81C-0x70C8EB): offset = (offsetY*fwd + offsetX*right) + pos, y DESPUES se suma front (antes: (pos + front) + offset);
    // el id es entity + localId + 0x33 (lea, sin leer m_pLod).
    const CVector& matPos = mat.GetPosition();
    const float posX = front.x + ((offsetY * mat.GetForward().x + offsetX * mat.GetRight().x) + matPos.x); // Simplified matrix transform (Ignoring the Z axis)
    const float posY = front.y + ((offsetX * mat.GetRight().y + offsetY * mat.GetForward().y) + matPos.y);
    StoreStaticShadow(
        reinterpret_cast<uint32>(entity) + localId + 0x33,
        SHADOW_DEFAULT,
        gpPostShadowTex,
        CVector{ posX, posY, matPos.z + offsetZ },
        front.x, front.y,
        right.x, right.y,
        (int16)(2 * intensity / 3),
        0, 0, 0,
        9.f,
        1.f,
        40.f,
        false,
        0.f
    );
}

// 0x70CCB0
void CShadows::RenderIndicatorShadow(
    uint32 id,
    eShadowType,
    RwTexture*,
    const CVector& posn,
    float frontX, float frontY,
    float sideX, float sideY,
    int16 /*intensity*/
) {
    // Como el original (0x70CCB3-0x70CCEA): fld [esp+2Ch] = sideY, fchs; compara frontX con -sideY (NO sideX): (frontX > -sideY) ? frontX : -sideY.
    const float size = (frontX > -sideY) ? frontX : -sideY;
    for (auto mult : { 0.8f, 0.9f, 1.0f }) {
        CVector markerPos = posn;
        C3dMarkers::PlaceMarkerSet(id, MARKER3D_CYLINDER, markerPos, size * mult, 255, 0, 0, 255u, 2048, 0.2f, 0);
    }
}

// 0x709CF0
#ifdef _MSC_VER
static uint8 ShadowTriangleAlpha(float depth, uint32 intensity) {
    uint8 result;
    const float one = 1.0f, zero = 0.0f;
    __asm {
        fld depth
        fmul depth
        fsubr one
        fcom zero
        fnstsw ax
        test ah, 5
        jp calculate
        mov result, 0
        fstp st(0)
        jmp finished
    calculate:
        fild intensity
        fmul st(0), st(1)
        mov eax, 0821B40h
        call eax
        mov result, al
        fstp st(0)
    finished:
    }
    return result;
}

CVector* ShadowRenderTriangleCB(CVector* normal, CVector* trianglePos, _ProjectionParam* param) {
    RwV3d world[3], projected[3];
    RwV3dTransformPoints(world, reinterpret_cast<RwV3d*>(trianglePos), 3, &param->entityMatrix);
    double facing;
    auto* direction = &param->at;
    __asm {
        mov eax, normal
        mov edx, direction
        fld dword ptr [eax+4]
        fmul dword ptr [edx+4]
        fld dword ptr [eax+8]
        fmul dword ptr [edx+8]
        faddp st(1), st(0)
        fld dword ptr [eax]
        fmul dword ptr [edx]
        faddp st(1), st(0)
        fstp facing
    }
    if (facing > 0.0) return trianglePos;
    RwV3dTransformPoints(projected, world, 3, &param->invMatrix);
    const auto any = [&](auto condition) { return condition(projected[0]) || condition(projected[1]) || condition(projected[2]); };
    if (!any([](auto p) { return !(p.z < 0.0f); }) ||
        !any([](auto p) { return !(p.x < 0.0f); }) ||
        !any([](auto p) { return !(p.x > 1.0f); }) ||
        !any([](auto p) { return !(p.y < 0.0f); }) ||
        !any([](auto p) { return !(p.y > 1.0f); })) return trianglePos;
    RwImVertexIndex* indices;
    RwIm3DVertex* vertices;
    RenderBuffer::StartStoring(3, 3, indices, vertices);
    for (int32 vertex = 0; vertex < 3; ++vertex) {
        vertices[vertex].objVertex.x = world[vertex].x;
        vertices[vertex].objVertex.y = world[vertex].y;
        vertices[vertex].objVertex.z = float(double(world[vertex].z) + double(0.06f));
        vertices[vertex].u = projected[vertex].x;
        vertices[vertex].v = projected[vertex].y;
        const uint8 intensity = param->fade ? ShadowTriangleAlpha(projected[vertex].z, param->shadowValue) : param->shadowValue;
        vertices[vertex].color = uint32(intensity) * 0x01010101u;
        indices[vertex] = uint16(vertex);
    }
    RenderBuffer::StopStoring();
    return trianglePos;
}

static void __declspec(naked) ShadowRenderTriangleRegisterEntry() {
    __asm {
        push edi
        push ebx
        push eax
        call ShadowRenderTriangleCB
        add esp, 12
        ret
    }
}
#endif
