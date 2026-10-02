/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CMirrors
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Mirrors.h"

#include "MovingThings.h"
#include "BreakManager_c.h"
#include "PlantMgr.h"
#include "Clouds.h"
#include "PostEffects.h"
#include "Shadows.h"
#include "CarFXRenderer.h"

auto& bFudgeNow = StaticRef<bool>(0xC7C72A);

/*!
 * Screen positions in Los Santos stadium. Odd job called as "8-Track"
 * 0x8D5DD8
 * */
constexpr CVector Screens8Track[2][4] = {
    {
        { -1333.453f, -221.898f, 1079.141f },
        { -1333.453f, -189.898f, 1079.141f },
        { -1333.453f, -189.898f, 1067.141f },
        { -1333.453f, -221.898f, 1067.141f },
    },
    {
        { -1477.845f, -189.969f, 1079.141f },
        { -1477.845f, -221.969f, 1079.141f },
        { -1477.845f, -221.969f, 1067.141f },
        { -1477.845f, -189.969f, 1067.141f },
    }
};

void CMirrors::InjectHooks() {
    RH_ScopedClass(CMirrors);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x723000);
    RH_ScopedInstall(ShutDown, 0x723050);
    RH_ScopedInstall(CreateBuffer, 0x7230A0);
    RH_ScopedInstall(BuildCamMatrix, 0x723150);
    RH_ScopedInstall(RenderMirrorBuffer, 0x726090);
    RH_ScopedInstall(BuildCameraMatrixForScreens, 0x7266B0);
    RH_ScopedInstall(BeforeConstructRenderList, 0x726DF0);
    RH_ScopedInstall(BeforeMainRender, 0x727140);
}

// 0x723000
void CMirrors::Init() {
    ShutDown();
}

// 0x723050
void CMirrors::ShutDown() {
    if (pBuffer) {
        RwRasterDestroy(pBuffer);
        pBuffer = nullptr;
    }
    if (pZBuffer) {
        RwRasterDestroy(pZBuffer);
        pZBuffer = nullptr;
    }
    TypeOfMirror = MIRROR_TYPE_NONE;
    MirrorFlags = 0;
}

// 0x7230A0
// Como el original: con calidad media o superior intenta 1024x512 y, si falla cualquiera de los dos
// rásteres, cae a 512x256 (sin comprobar el resultado).
void CMirrors::CreateBuffer() {
    if (pBuffer)
        return;

    const auto depth = RwRasterGetDepth(RwCameraGetRaster(Scene.m_pRwCamera));
    if (g_fx.GetFxQuality() >= FX_QUALITY_MEDIUM) {
        pBuffer = RwRasterCreate(1024, 512, depth, rwRASTERTYPECAMERATEXTURE);
        if (pBuffer) {
            pZBuffer = RwRasterCreate(1024, 512, depth, rwRASTERTYPEZBUFFER);
            if (pZBuffer) {
                return; // All needed buffers created
            }
            RwRasterDestroy(pBuffer);
            pBuffer = nullptr;
        }
    }
    pBuffer  = RwRasterCreate(512, 256, depth, rwRASTERTYPECAMERATEXTURE);
    pZBuffer = RwRasterCreate(512, 256, depth, rwRASTERTYPEZBUFFER);
}

// 0x723150
// Como el original: escribe (0,0,1) en up, normaliza forward y también right.
void CMirrors::BuildCamMatrix(CMatrix& mat, CVector pointA, CVector pointB) {
    mat.SetTranslateOnly(pointA);
    mat.GetForward() = pointB - pointA;
    mat.GetForward().Normalise();
    mat.GetUp() = CVector{ 0.0f, 0.0f, 1.0f };
    mat.GetRight() = CrossProduct(mat.GetUp(), mat.GetForward());
    mat.GetRight().Normalise();
    mat.GetUp() = CrossProduct(mat.GetForward(), mat.GetRight());
}

// 0x726090
// Como el original: los vértices van a los búferes globales del EXE (Im2D en 0xC7C5A8, TempBufferVertices
// e índices copiados de 0x8D5E38), y el quad especial cambia la mezcla antes de dibujarse.
void CMirrors::RenderMirrorBuffer() {
    ZoneScoped;

    if (TypeOfMirror == MIRROR_TYPE_NONE)
        return;

    static auto& im2dVertices = StaticRef<std::array<RwIm2DVertex, 4>>(0xC7C5A8);
    static auto& mirrorIndices = StaticRef<std::array<RwImVertexIndex, 6>>(0x8D5E38);
    static auto& screens8Track = StaticRef<CVector[2][4]>(0x8D5DD8);

    RwRaster* raster = RwCameraGetRaster(Scene.m_pRwCamera);
    const float rasterWidth = (float)RwRasterGetWidth(raster), rasterHeight = (float)RwRasterGetHeight(raster);

    DefinedState();

    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(pBuffer));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDZERO));

    // Rellena TempBufferVertices (posición, color y UV) y copia los 6 índices globales
    const auto DrawQuad3D = [&](const CVector (&pos)[4]) {
        constexpr float uvs[4][2]{ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        for (auto i = 0u; i < 4; i++) {
            auto& v = TempBufferVertices.m_3d[i];
            RwV3dAssign(RwIm3DVertexGetPos(&v), &pos[i]);
            RwIm3DVertexSetRGBA(&v, 255, 255, 255, 255);
            RwIm3DVertexSetU(&v, uvs[i][0]);
            RwIm3DVertexSetV(&v, uvs[i][1]);
        }
        for (auto i = 0u; i < 6; i++) {
            aTempBufferIndices[i] = mirrorIndices[i];
        }
        *reinterpret_cast<uint32*>(&uiTempBufferIndicesStored)  = 6; // el original escribe la palabra completa
        *reinterpret_cast<uint32*>(&uiTempBufferVerticesStored) = 4;
        if (RwIm3DTransform(TempBufferVertices.m_3d, 4, nullptr, rwIM3D_VERTEXUV)) {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, *reinterpret_cast<uint32*>(&uiTempBufferIndicesStored));
            RwIm3DEnd();
        }
    };

    if ((MirrorFlags & eZoneAttributes::STAIRS) || bFudgeNow) {
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
        RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
        for (auto x = 0u; x < 2; x++) {
            DrawQuad3D(screens8Track[x]);
        }
    } else {
        const float pos[4][2]{ { 0.0f, 0.0f }, { 0.0f, rasterHeight }, { rasterWidth, rasterHeight }, { rasterWidth, 0.0f } };
        constexpr float uvs[4][2]{ { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f } };
        for (auto i = 0u; i < 4; i++) {
            auto& v = im2dVertices[i];
            RwIm2DVertexSetScreenX(&v, pos[i][0]);
            RwIm2DVertexSetScreenY(&v, pos[i][1]);
            RwIm2DVertexSetScreenZ(&v, RwIm2DGetNearScreenZ());
            RwIm2DVertexSetRecipCameraZ(&v, 1.0f / RwCameraGetNearClipPlane(Scene.m_pRwCamera));
            RwIm2DVertexSetIntRGBA(&v, 255, 255, 255, 255);
            RwIm2DVertexSetU(&v, uvs[i][0], 0.0f);
            RwIm2DVertexSetV(&v, uvs[i][1], 0.0f);
        }
        RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, im2dVertices.data(), 4, mirrorIndices.data(), 6);
    }

    // Como el original: distancia a (216, -42, 1003) con la suma (z² + y²) + x²
    const auto& cam = TheCamera.GetPosition();
    const float dx = cam.x - 216.0f, dy = cam.y - -42.0f, dz = cam.z - 1003.0f;
    if (std::sqrt(dz * dz + dy * dy + dx * dx) < 50.0f) {
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
        RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDZERO));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDONE));
        const CVector pos[4]{
            { 216.0f, -45.0f, 1000.0f },
            { 216.0f, -45.0f, 1006.0f },
            { 216.0f, -39.0f, 1006.0f },
            { 216.0f, -39.0f, 1000.0f }
        };
        DrawQuad3D(pos);
    }

    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
}

// ANDROID 1.0 0x5078B8
void CMirrors::RenderReflBuffer(bool) {

}

// ANDROID 1.0 0x507B2C
void CMirrors::RenderReflections() {

}

// 0x7266B0
// Como el original: milisegundos · 0.001 (no / 1000) y giro con la constante π/8192 del EXE.
void CMirrors::BuildCameraMatrixForScreens(CMatrix & mat) {
    const uint32 timeSeconds = (CTimer::GetTimeInMS() / 1000) % 32; // Wrapped to (0, 32]
    const uint32 timeMsLeftInSecond = CTimer::GetTimeInMS() % 1000;

    switch (timeSeconds) {
    case 0:
    case 1:
    case 2:
    case 3: {
        BuildCamMatrix(
            mat,
            { ((float)(int32)timeMsLeftInSecond * 0.001f + (float)(int32)timeSeconds) * 6.0f - 1249.3f, -224.5f, 1064.2f },
            { -1265.4f, -207.5f, 1053.2f }
        );
        break;
    }
    case 10:
    case 11:
    case 12:
    case 13: {
        BuildCamMatrix(mat,
            { -1406.4f, -135.3f, 1045.65f },
            { -1402.5f, -146.8f, 1043.10f }
        );
        break;
    }
    case 22:
    case 23:
    case 24:
    case 25: {
        BuildCamMatrix(mat,
            { -1479.0f, -290.5f, 1099.54f },
            { -1428.0f, -256.7f, ((float)(int32)timeMsLeftInSecond * 0.001f + (float)(int32)(timeSeconds - 22)) * 3.0f + 1057.3f }
        );
        break;
    }
    default: {
        mat.SetRotateZOnly((float)(CTimer::GetTimeInMS() % 16384) * std::bit_cast<float>(0x39C90FDBu)); // 0x8729F8
        mat.SetTranslateOnly({ -1397.0f, -219.0f, 1054.0f });
        break;
    }
    }
}

// NOTSA
bool CMirrors::ShouldRenderPeds() {
    return bRenderingReflection && TypeOfMirror != MIRROR_TYPE_FLOOR;
}

// 0x726DF0
// Como el original: recorre siempre las dos pantallas (centroide con el orden de sumas del EXE),
// normal = componentes · 0.01, MirrorFlags con signo y matriz de pantallas sin inicializar por valor.
void CMirrors::BeforeConstructRenderList() {
    if (d3dRestored) {
        d3dRestored = false;
        Init();
    }

    static auto& screens8Track = StaticRef<CVector[2][4]>(0x8D5DD8);
    CMirrorAttributeZone* mirrorAttrs = nullptr;

    const auto mirrorActive = [&](){
        // Check player is in heli/plane
        if (auto* vehicle = FindPlayerVehicle()) {
            if (vehicle->IsSubHeli() || vehicle->IsSubPlane()) {
                return false;
            }
        }

        mirrorAttrs = CCullZones::FindMirrorAttributesForCoors(TheCamera.GetPosition());
        if (!mirrorAttrs) {
            return false;
        }

        if ((mirrorAttrs->flags & eZoneAttributes::STAIRS) == 0) {
            return true;
        }

        bool visible = false;
        for (const auto& t : screens8Track) {
            const CVector centre{
                (((t[0].x + t[1].x) + t[2].x) + t[3].x) * 0.25f,
                (((t[0].y + t[1].y) + t[3].y) + t[2].y) * 0.25f,
                (((t[0].z + t[1].z) + t[3].z) + t[2].z) * 0.25f,
            };
            TheCamera.m_bMirrorActive = false;
            if (TheCamera.IsSphereVisible(centre, 8.0f)) {
                visible = true;
            }
        }
        return visible;
    }();

    if (mirrorActive) {
        // Actually update cam
        assert(mirrorAttrs);

        MirrorV = mirrorAttrs->mirrorV;
        MirrorNormal = CVector{ (float)mirrorAttrs->mirrorNormalX * 0.01f, (float)mirrorAttrs->mirrorNormalY * 0.01f, (float)mirrorAttrs->mirrorNormalZ * 0.01f };
        MirrorFlags = (int8)mirrorAttrs->flags;

        TypeOfMirror = std::fabs(MirrorNormal.z) <= 0.7f ? MIRROR_TYPE_WALL : MIRROR_TYPE_FLOOR;
        CreateBuffer();
    } else {
        ShutDown();
    }

    if ((MirrorFlags & eZoneAttributes::STAIRS) != 0 || bFudgeNow) {
        CMatrix mat;
        BuildCameraMatrixForScreens(mat);
        TheCamera.DealWithMirrorBeforeConstructRenderList(mirrorActive, MirrorNormal, MirrorV, &mat);
    } else {
        TheCamera.DealWithMirrorBeforeConstructRenderList(mirrorActive, MirrorNormal, MirrorV, nullptr);
    }
}

// 0x727140
void CMirrors::BeforeMainRender() {
    ZoneScoped;

    if (TypeOfMirror == MIRROR_TYPE_NONE)
        return;

    RwRaster* prevCamRaster  = RwCameraGetRaster(Scene.m_pRwCamera);
    RwRaster* prevCamZRaster = RwCameraGetZRaster(Scene.m_pRwCamera);

    RwCameraSetRaster(Scene.m_pRwCamera, pBuffer);
    RwCameraSetZRaster(Scene.m_pRwCamera, pZBuffer);

    TheCamera.SetCameraUpForMirror();

    RwRGBA color{ 0, 0, 0, 255 };
    RwCameraClear(Scene.m_pRwCamera, &color, rwCAMERACLEARZ | rwCAMERACLEARIMAGE | (GraphicsHighQuality() ? rwCAMERACLEARSTENCIL : 0));
    if (RsCameraBeginUpdate(Scene.m_pRwCamera)) {
        bRenderingReflection = true;
        DefinedState();
        RenderScene();
        CVisibilityPlugins::RenderWeaponPedsForPC();
        CVisibilityPlugins::ResetWeaponPedsForPC();
        bRenderingReflection = false;

        RwCameraEndUpdate(Scene.m_pRwCamera);

        RwCameraSetRaster(Scene.m_pRwCamera, prevCamRaster);
        RwCameraSetZRaster(Scene.m_pRwCamera, prevCamZRaster);

        TheCamera.RestoreCameraAfterMirror();
    }
}
