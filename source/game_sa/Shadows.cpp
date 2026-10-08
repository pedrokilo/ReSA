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
 * Validación registrada: 10/31 entradas.
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
    CRect rect{ {minX, minY}, {maxX, maxY} };
    for (auto& shadow : aPermanentShadows) {
        switch (shadow.m_nType) {
        case SHADOW_OIL_1:
        case SHADOW_OIL_5:
            break;
        default:
            continue;
        }
        if (rect.IsPointInside(shadow.m_vecPosn)) {
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

    sideX = -sunPosn.y * recip / 2.0f;
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
        const auto mult = std::min(
            0.4f + 0.6f * (1.f - CCustomBuildingDNPipeline::m_fDNBalanceParam),
            0.3f + 0.7f * lerp(
                (float)(dayNightIntensity >> 0 & 0b1111) / 30.f,
                (float)(dayNightIntensity >> 4 & 0b1111) / 30.f,
                CCustomBuildingDNPipeline::m_fDNBalanceParam
            )
        );
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
uint16 CalculateShadowStrength(float currDist, float maxDist, uint16 maxStrength) {
    assert(maxDist >= currDist); // Otherwise integer underflow will occur

    const auto halfMaxDist = maxDist / 2.f;
    if (currDist >= halfMaxDist) { // Anything further than half the distance is faded out
        return (uint16)((1.f - (currDist - halfMaxDist) / halfMaxDist) * maxStrength);
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

    const auto  bonePos           = ped->GetBonePosition(BONE_ROOT);
    const auto& camPos            = TheCamera.GetPosition();
    const auto  boneToCamDist2DSq = (bonePos - camPos).SquaredMagnitude2D();

    // Check if ped is close enough
    if (boneToCamDist2DSq >= MAX_DISTANCE_PED_SHADOWS_SQR) {
        return;
    }

    const auto isPlayerPed = FindPlayerPed() == ped;

    // Check if ped is visible to the camera
    if (!isPlayerPed) {  // Optimization: Assume player ped is always visible
        if (!TheCamera.IsSphereVisible(ped->GetPosition(), 2.f)) {
            return;
        }
    }

    // Now store a shadow to be rendered
    const auto strength = (uint8)CalculateShadowStrength(std::sqrt(boneToCamDist2DSq), MAX_DISTANCE_PED_SHADOWS, CTimeCycle::m_CurrentColours.m_nShadowStrength);
    StoreShadowToBeRendered(
        SHADOW_DEFAULT,
        gpShadowPedTex,
        bonePos + CVector{displacementX, displacementY, 0.f},
        frontX, frontY,
        sideX, sideY,
        strength,
        strength, strength, strength,
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
    const auto  shdwPos = physical->GetIsTypePed()
        ? physical->AsPed()->GetBonePosition(BONE_ROOT)
        : physical->GetPosition();
    const auto shdwToCamDist2DSq = (shdwPos - camPos).SquaredMagnitude2D();

    // Check distance to camera
    if (shdwToCamDist2DSq > MAX_DISTANCE_PED_SHADOWS_SQR) {
        return;
    }

    // Check if the object is visible to the camera
    if (FindPlayerPed() != physical) {  // Optimization: Assume player ped is always visible
        if (!TheCamera.IsSphereVisible(shdwPos, 2.f)) {
            return;
        }
    }

    const auto strength = (float)CalculateShadowStrength(std::sqrt(shdwToCamDist2DSq), MAX_DISTANCE_PED_SHADOWS, CTimeCycle::m_CurrentColours.m_nShadowStrength);
    const auto cc       = (uint8)((float)rtshdw->m_nIntensity / 100.f * strength);

    const auto& vecToSun = CTimeCycle::m_VectorToSun[CTimeCycle::m_CurrentStoredValue];
    const auto lightFrame = rtshdw->SetLightProperties(
        RWRAD2DEG(+std::atan2(-vecToSun.x, -vecToSun.y)),
        RWRAD2DEG(-std::atan2(+vecToSun.x, -vecToSun.z)),
        true
    );
    CalcPedShadowValues(
        *RwMatrixGetAt(RwFrameGetMatrix(lightFrame)),
        displacementX, displacementY,
        frontX, frontY,
        sideX, sideY
    );
    StoreShadowToBeRendered(
        SHADOW_INVCOLOR,
        rtshdw->GetShadowRwTexture(),
        shdwPos - CVector{ displacementX, displacementY, 0.f } * 2.5f,
        frontX * 1.5f, frontY * 1.5f,
        sideX * 1.5f, sideY * 1.5f,
        cc,
        cc, cc, cc,
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
    if (!CTimeCycle::m_CurrentColours.m_nShadowStrength) {
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

    const auto  plyrPos    = FindPlayerCoors();
    const auto& plyrVehPos = plyrVeh->GetPosition();
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

        const auto lightToPlyr{ ptl.m_vecPosn - plyrPos };
        const auto lightToPlyrDist = lightToPlyr.Magnitude();
        if (lightToPlyrDist >= ptl.m_fRadius || lightToPlyrDist == 0.f) { // NOTSA: Zero check to prevent (possible) division-by-zero
            continue;
        }

        const auto lightToPlyrDir  = lightToPlyr / lightToPlyrDist; // Normalize vector

        const auto& plyrVehBB = plyrVeh->GetColModel()->GetBoundingBox();
        const auto  shdwSize  = CVector2D{ plyrVehBB.GetSize() } / 2.f;

        StoreShadowToBeRendered(
            SHADOW_DEFAULT,
            gpShadowCarTex,
            plyrVehPos - CVector{
                  CVector2D{ lightToPlyr / lightToPlyrDist } * 1.2f // Compensate for elvation of the light
                - CVector2D{ plyVehMat.GetForward() } * (shdwSize.y - plyrVehBB.m_vecMax.y) // Move point to center (as the shadow's position is it's center)
            },
            CVector2D{ plyVehMat.GetForward() } * shdwSize.y,
            CVector2D{ plyVehMat.GetRight() } * (plyVehMat.GetUp().z >= 0.f ? shdwSize.x : -shdwSize.x), // If vehicle is flipped, we gotta flip the `right` vector back
            CalculateShadowStrength( // 0x70809A
                lightToPlyrDist,
                ptl.m_fRadius,
                5 * CTimeCycle::m_CurrentColours.m_nShadowStrength / 8u // Same as mult by `0.625` and then casting to int
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
        for (auto& ishdw : aStaticShadows) {
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
                const auto a = (uint8)((float)ishdw.m_nIntensity * (1.f - CWeather::Foggyness * 0.5f));
                for (auto i{ 0 }; i < poly->m_wNumVerts; i++, vtxIt++) {
                    const auto& pos = poly->m_avecPosn[i];
                    RwIm3DVertexSetPos(vtxIt, pos.x, pos.y, pos.z + 0.06f);
                    RwIm3DVertexSetRGBA(vtxIt, r, g, b, a);
                    RwIm3DVertexSetU(vtxIt, (float)poly->m_aU[i] / 200.f);
                    RwIm3DVertexSetV(vtxIt, (float)poly->m_aV[i] / 200.f);
                }

                // 0x7085BC: Write indices  (`if` not necessary, it's part of the loop condition)
                for (auto i = 0; i < totalNoIdx; i++) {
                    *vtxIdxIt++ = g_ShadowVertices[i];
                }
                
                // Finish storing 
                RenderBuffer::StopStoring();

                // Mark this as rendered
                ishdw.m_bRendered = true;
            }
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
    const CRect shadowRect{
        cornerAX, cornerAY,
        cornerBX, cornerBY
    };
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
        if (entity->GetMatrix().GetUp().z <= 0.97f) {
            continue;
        }

        // 0x70A526
        if (!entity->GetBoundRect().OverlapsWith(shadowRect)) {
            continue;
        }

        // Quick Z height check of the bounding box
        const auto& cm         = entity->GetColModel();
        const auto  entityPosZ = entity->GetPosition().z;
        if (cm->m_boundBox.m_vecMax.z + entityPosZ <= posn->z - zDistance) {
            continue;
        }
        if (cm->m_boundBox.m_vecMin.z + entityPosZ >= posn->z) {
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
void CShadows::RenderStoredShadows() {
    // Originally renderstates are still set even though there are no shadows to be rendered
    // I don't think this is necessary, so we early-out here
    if (!CShadows::ShadowsStoredToBeRendered) {
        return;
    }

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

    const auto GetShadowRect = [](CRegisteredShadow& shdw) {
        return CRect{
            shdw.m_vecPosn,
            shdw.m_vecPosn + abs(shdw.m_Side) + abs(shdw.m_Front)
        };
    };

    for (auto o = 0; o < ShadowsStoredToBeRendered; o++) {
        auto& oshdw = asShadowsStored[o];

        // Setup additional render states for this shadow (and others in the batch below)
        SetRenderModeForShadowType(oshdw.m_nType);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(oshdw.m_pTexture)));

        //
        // Render onto ground (If not already)
        //
        if (!oshdw.m_bAlreadyRenderedInBatch) {
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

                CWorld::AdvanceCurrentScanCode();

                const auto ishdwRect = GetShadowRect(ishdw);

                // Do shadow casting
                CWorld::IterateSectorsOverlappedByRect(
                    ishdwRect,
                    [&] (int32 x, int32 y) -> bool {
                        auto& sector = CWorld::GetSector(x, y);

                        if (const auto rtshdw = ishdw.m_pRTShadow) {
                            CastRealTimeShadowSectorList(
                                sector.Buildings,
                                ishdwRect.left, ishdwRect.bottom,
                                ishdwRect.right, ishdwRect.top,
                                &ishdw.m_vecPosn,
                                ishdw.m_Front.x, ishdw.m_Front.y,
                                ishdw.m_Side.x, ishdw.m_Side.y,
                                ishdw.m_nIntensity,
                                ishdw.m_nRed, ishdw.m_nGreen, ishdw.m_nBlue,
                                ishdw.m_fZDistance,
                                ishdw.m_fScale,
                                nullptr,
                                rtshdw,
                                nullptr // Unused
                            );
                        } else {
                            uint8 unused{};
                            if (ishdw.m_bDrawOnBuildings) {
                                CastShadowSectorList(
                                    sector.Buildings,
                                    ishdwRect.left, ishdwRect.bottom,
                                    ishdwRect.right, ishdwRect.top,
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
                                    sector.Buildings,
                                    ishdwRect.left, ishdwRect.bottom,
                                    ishdwRect.right, ishdwRect.top,
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
                    }
                );

                // Mark this as rendered
                ishdw.m_bAlreadyRenderedInBatch = true;
            }

            // Render out shadows (Can't batch together with next iteration, as renderstates change)
            RenderBuffer::RenderStuffInBuffer();
        }
        
        // Render onto water (If needed)
        if (oshdw.m_bDrawOnWater) {  
            float waterLevelZ{}, bigWavesZ{}, smallWavesZ{};
            if (!CWaterLevel::GetWaterLevelNoWaves(oshdw.m_vecPosn, &waterLevelZ, &bigWavesZ, &smallWavesZ)) {
                continue;
            }

            // Shadow under water?
            if (waterLevelZ >= oshdw.m_vecPosn.z) {
                continue;
            }

            // Clear the buffer as this is a new render pass (separate from the other)
            RenderBuffer::ClearRenderBuffer();

            // The smaller this value the detailed the shadows, but also (exponentially?) slower
            // This value basically represents the side of a square that is projected onto the 
            const auto STEP_SIZE = 2;

            const auto shdwSideMagSq = oshdw.m_Side.SquaredMagnitude();
            const auto shdwFrontMagSq = oshdw.m_Front.SquaredMagnitude();

            const auto oshdwRect = GetShadowRect(oshdw);

            // Iterate shadow sectors (Using `CWorld::IterateSectors` for convenience)
            CWorld::IterateSectors(
                (int32)(std::floor(oshdwRect.left / (float)STEP_SIZE)),
                (int32)(std::floor(oshdwRect.top / (float)STEP_SIZE)),
                (int32)(std::ceil(oshdwRect.right / (float)STEP_SIZE)),
                (int32)(std::ceil(oshdwRect.bottom / (float)STEP_SIZE)),
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
                        RwIm3DVertexSetRGBA(vtxIt, oshdw.m_nRed, oshdw.m_nGreen, oshdw.m_nBlue, (uint8)((float)oshdw.m_nIntensity * 0.6f));

                        // Set texture coords (UV)
                        const auto CalcTexCoord  = [
                            currPosToShdw = CVector2D{ currPos - oshdw.m_vecPosn }
                        ](CVector2D v, float vmagsq) {
                            return (currPosToShdw.Dot(v) / vmagsq + 1.f) * 0.5f;
                        };
                        RwIm3DVertexSetU(vtxIt, CalcTexCoord(oshdw.m_Side, shdwSideMagSq));
                        RwIm3DVertexSetV(vtxIt, CalcTexCoord(oshdw.m_Front, shdwFrontMagSq));

                        // Set position
                        RwIm3DVertexSetPos(
                            vtxIt,
                            currPos.x,
                            currPos.y,
                            CWaterLevel::CalculateWavesOnlyForCoordinate2_Direct(x, y, waterLevelZ, bigWavesZ, smallWavesZ) + 0.06f // Z pos of the wave at the given coord
                        );
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
                }
            );

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
static int32 ShadowSectorIndex(float coordinate, bool maximum) {
    const float multiplier = 0.02f, bias = 60.0f;
    float saved;
    double extended;
    __asm {
        fld coordinate
        fmul multiplier
        fadd bias
        fst saved
        fstp extended
    }
    const double first = std::floor(extended);
    if (maximum ? first >= 119.0 : first <= 0.0) return maximum ? 119 : 0;
    return int32(std::floor(double(saved)));
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
        const auto shdwToCam2D       = CVector2D{ TheCamera.GetPosition() - posn };
        const auto shdwToCamDist2DSq = shdwToCam2D.SquaredMagnitude();

        if (shdwToCamDist2DSq >= sq(MAX_CAM_TO_LIGHT_DIST)) {
            return;
        }

        // Check if the camera is facing the lights closely (in which case the camera can't see the shadow)
        if (shdwToCam2D.Dot(TheCamera.GetFrontNormal2D()) > maxViewAngleCosine) {
            return;
        }

        // If far enough from the camera, start fading out
        if (const auto dist = std::sqrt(shdwToCamDist2DSq); dist >= MAX_CAM_TO_LIGHT_DIST * 0.75f) {
            const auto t = 1.f - invLerp(MAX_CAM_TO_LIGHT_DIST * (2.f / 3.f), MAX_CAM_TO_LIGHT_DIST, dist);
            red   = (uint8)((float)red * t);
            green = (uint8)((float)green * t);
            blue  = (uint8)((float)blue * t);
        }
    }

    const auto isPlyrVeh = FindPlayerVehicle() == vehicle;
    if (isPlyrVeh || vehicle->GetMoveSpeed().Magnitude() * CTimer::GetTimeStep() >= 0.4f) {
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

    const auto intensity = 2.f * (mat.GetUp().z - 0.5f) * CTimeCycle::m_CurrentColours.m_nPoleShadowStrength;

    const auto front     = CVector2D{ CTimeCycle::GetVectorToSun() } * (-poleHeight / 2.f);
    const auto right     = CVector2D{ CTimeCycle::GetShadowSide() } * poleWidth;

    StoreStaticShadow(
        reinterpret_cast<uint32>(entity->GetLod()) + localId + 3,
        SHADOW_DEFAULT,
        gpPostShadowTex,
        mat.GetPosition() + CVector{ front, 0.f } + CVector{
            offsetX * mat.GetRight().x + offsetY * mat.GetForward().x, // Simplified matrix transform (Ignoring the Z axis)
            offsetX * mat.GetRight().y + offsetY * mat.GetForward().y, // >^^^
            offsetZ
        },
        front.x, front.y,
        right.x, right.y,
        2 * (int16)intensity / 3,
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
    const auto size = std::max(frontX, -sideX);
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
