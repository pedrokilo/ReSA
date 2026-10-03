/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CSpecialFX
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "MotionBlurStreaks.h"
#include "Checkpoints.h"
#include "SpecialFX.h"

void CSpecialFX::InjectHooks() {
    RH_ScopedClass(CSpecialFX);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x7268F0);
    RH_ScopedInstall(Update, 0x726AA0);
    RH_ScopedInstall(Shutdown, 0x723390);
    RH_ScopedInstall(AddWeaponStreak, 0x7233F0);
    RH_ScopedInstall(Render, 0x726AD0);
    RH_ScopedInstall(Render2DFXs, 0x721660);
    RH_ScopedInstall(ReplayStarted, 0x721D30);
}

// 0x7268F0
void CSpecialFX::Init() {
    CBulletTraces::Init();
    C3dMarkers::User3dMarkerDeleteAll();
    C3dMarkers::DirectionArrowsInit();

    constexpr RxVertexIndex MBSIndices[] = { 0, 1, 2, 1, 3, 2, 0, 2, 1, 1, 2, 3 };
    rng::copy(MBSIndices, CMotionBlurStreaks::aIndices);

    for (auto i = 0; i < 2; i++) {
        RxObjSpace3DVertexSetU(&CMotionBlurStreaks::aStreakVertices[2 * i + 0], 0.0f);
        RxObjSpace3DVertexSetV(&CMotionBlurStreaks::aStreakVertices[2 * i + 0], 0.0f);

        RxObjSpace3DVertexSetU(&CMotionBlurStreaks::aStreakVertices[2 * i + 1], 1.0f);
        RxObjSpace3DVertexSetV(&CMotionBlurStreaks::aStreakVertices[2 * i + 1], 0.0f);
    }

    CMotionBlurStreaks::Init();
    CBrightLights::Init();
    CShinyTexts::Init();
    C3dMarkers::Init();
    CCheckpoints::Init();

    bVideoCam       = false;
    bLiftCam        = false;
    bSnapShotActive = false;
    SnapShotFrames  = 0;

    // Como el original: Push, Find, Set (sin el assert de ScopedTXDSlot)
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));
    if (!gpFinishFlagTex) {
        gpFinishFlagTex = RwTextureRead("finishFlag", nullptr);
    }
    CTxdStore::PopCurrentTxd();

    CMirrors::Init();
}

// 0x726AA0
void CSpecialFX::Update() {
    ZoneScoped;

    for (auto& mbs : CMotionBlurStreaks::aStreaks) {
        if (mbs.m_nId) {
            mbs.Update();
        }
    }
    CBulletTraces::Update();
    CCheckpoints::Update();
    C3dMarkers::Update();
}

// 0x723390
void CSpecialFX::Shutdown() {
    C3dMarkers::Shutdown();
    if (gpFinishFlagTex) {
        RwTextureDestroy(std::exchange(gpFinishFlagTex, nullptr));
    }
    CMirrors::ShutDown();
}

// Como el original: CMatrix * CVector (0x59C890) con su orden de sumas
static CVector MultiplyPointLikeExe(const CMatrix& m, const CVector& v) {
    return {
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}

// unused function
// 0x7233F0
void CSpecialFX::AddWeaponStreak(eWeaponType weaponType) {
    // Como el original: la matriz es la estática del EXE (0xC803E8) con su guarda de inicialización
    auto& guard     = StaticRef<uint32>(0xC80430);
    auto& attachMat = StaticRef<CMatrix>(0xC803E8);
    if (!(guard & 1)) {
        guard |= 1;
        StaticRef<uint32>(0xC80428) = 0; // m_pAttachMatrix
        StaticRef<uint32>(0xC8042C) = 0; // m_bOwnsAttachedMatrix
        reinterpret_cast<int32(__cdecl*)(void(__cdecl*)())>(0x821D1E)(reinterpret_cast<void(__cdecl*)()>(0x856BC0)); // atexit(~attachMat)
    }

    if (!FindPlayerPed() || !FindPlayerPed()->m_pWeaponObject) {
        return;
    }
    const auto weaponObject = FindPlayerPed()->m_pWeaponObject;

    CVector endO;
    switch (weaponType) {
    case WEAPON_BASEBALLBAT: endO = CVector{ std::bit_cast<float>(0x3E7BE76Du), std::bit_cast<float>(0x3D051EB8u), std::bit_cast<float>(0x3F4BC6A8u) }; break; // 0.246, 0.0325, 0.796
    case WEAPON_GOLFCLUB:    endO = CVector{ std::bit_cast<float>(0xBD5D2F1Bu), std::bit_cast<float>(0x3D051EB8u), std::bit_cast<float>(0x3F4BC6A8u) }; break; // -0.054, 0.0325, 0.796
    case WEAPON_KATANA:      endO = CVector{ std::bit_cast<float>(0x3DC49BA6u), std::bit_cast<float>(0xBC8F5C29u), std::bit_cast<float>(0x3F8C49BAu) }; break; // 0.096, -0.0175, 1.096
    default:                 return; // OG: No streak is created
    }

    // Como el original: ambos extremos se transforman como puntos (con traslación)
    attachMat = CMatrix(RwFrameGetLTM(RpAtomicGetFrame(weaponObject)), false);
    const CVector start = MultiplyPointLikeExe(attachMat, CVector{ std::bit_cast<float>(0x3CA3D70Au), std::bit_cast<float>(0x3D4CCCCDu), std::bit_cast<float>(0x3D8F5C29u) }); // 0.02, 0.05, 0.07
    const CVector end   = MultiplyPointLikeExe(attachMat, endO);
    CMotionBlurStreaks::RegisterStreak(reinterpret_cast<uint32>(weaponObject), 100, 100, 100, 255, start, end);
}

// 0x726AD0
void CSpecialFX::Render() {
    ZoneScoped;

    CMotionBlurStreaks::Render();
    CBulletTraces::Render();
    CBrightLights::Render();
    CShinyTexts::Render();
    C3dMarkers::Render();
    CCheckpoints::Render();
}

// 0x721660
void CSpecialFX::Render2DFXs() {
    if (bVideoCam) {
        // Como el original: ancho * (1/640) * 1,5 y alto * (1/448) * 1,5
        CFont::SetScale(SCREEN_WIDTH * (1.0f / 640.0f) * 1.5f, SCREEN_HEIGHT * (1.0f / 448.0f) * 1.5f);
        CFont::SetBackground(false, false);
        CFont::SetProportional(true);
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        CFont::SetColor({ 0, 255, 0, 200 });
        CFont::SetFontStyle(FONT_SUBTITLES);
        notsa::format_to_sz(gString, "{}", CTimer::GetFrameCounter() % 64);
        AsciiToGxtChar(gString, gGxtString);
        // Como el original: posición entera (ancho * 8 / 10, alto * 8 / 10)
        CFont::PrintString((float)(RsGlobal.maximumWidth * 8 / 10), (float)(RsGlobal.maximumHeight * 8 / 10), gGxtString);

        for (auto y = 3.0f; y - 3.0f < SCREEN_HEIGHT; y += 4.0f) {
            RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDONE));
            RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));
            CSprite2d::Draw2DPolygon(
                0.0f,         y - 3.0f,
                SCREEN_WIDTH, y - 3.0f,
                0.0f,         y - 2.0f,
                SCREEN_WIDTH, y - 2.0f,
                { 0, 100, 0, 100 }
            );
            RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
            RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
            CSprite2d::Draw2DPolygon(
                0.0f,         y - 1.0f,
                SCREEN_WIDTH, y - 1.0f,
                0.0f,         y,
                SCREEN_WIDTH, y,
                { 0, 0, 0, 150 }
            );
        }
        RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));

        // Como el original: se trunca el producto y luego se resta 70
        const auto ytBase = (int32)((SCREEN_HEIGHT + 70.0f) * (1.0f / 2048.0f) * float(CTimer::GetTimeInMS() % 2'048));
        const auto yt = (float)(int32)((float)ytBase - 70.0f);
        CSprite2d::Draw2DPolygon(
            0.0f,         yt,
            SCREEN_WIDTH, yt,
            0.0f,         yt + 70.0f,
            SCREEN_WIDTH, yt + 70.0f,
            { 0, 100, 0, 60 }
        );
    }

    if (bLiftCam) {
        CFont::SetScale(SCREEN_WIDTH * (1.0f / 640.0f) * 1.5f, SCREEN_HEIGHT * (1.0f / 448.0f) * 1.5f);
        CFont::SetBackground(false, false);
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT); // Como el original: orientación antes que proporcional
        CFont::SetProportional(true);
        CFont::SetColor({ 100, 100, 100, 200 });
        CFont::SetFontStyle(FONT_SUBTITLES);
        // ...nothing to print?

        for (auto y = 3.0f; y - 3.0f < SCREEN_HEIGHT; y += 4.0f) {
            RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDONE));
            RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));
            CSprite2d::Draw2DPolygon(
                0.0f,         y - 3.0f,
                SCREEN_WIDTH, y - 3.0f,
                0.0f,         y - 2.0f,
                SCREEN_WIDTH, y - 2.0f,
                { 100, 100, 100, 100 }
            );
            RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
            RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
            CSprite2d::Draw2DPolygon(
                0.0f,         y - 1.0f,
                SCREEN_WIDTH, y - 1.0f,
                0.0f,         y,
                SCREEN_WIDTH, y,
                { 0, 0, 0, 150 }
            );
        }

        // Como el original: se trunca el producto y luego se resta 70
        const auto ytBase = (int32)((SCREEN_HEIGHT + 70.0f) * (1.0f / 2048.0f) * float(CTimer::GetTimeInMS() % 2'048));
        const auto yt = (float)(int32)((float)ytBase - 70.0f);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA)); // Como el original: también aquí
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
        CSprite2d::Draw2DPolygon(
            0.0f,         yt,
            SCREEN_WIDTH, yt,
            0.0f,         yt + 70.0f,
            SCREEN_WIDTH, yt + 70.0f,
            { 100, 100, 100, 60 }
        );

        for (auto i = 0; i < 200; i++) {
            // Como el original: rand() % ancho y rand() % alto (división con signo)
            const int32 x = (int32)CGeneral::GetRandomNumber() % RsGlobal.maximumWidth;
            const int32 y = (int32)CGeneral::GetRandomNumber() % RsGlobal.maximumHeight;
            CSprite2d::DrawRect(
                CRect{
                    float(x),
                    float(y),
                    float(x + 20),
                    float(y + 2)
                },
                { 255, 255, 255, 64 }
            );
        }
    }

    if (bSnapShotActive) {
        TheCamera.GetActiveCam().m_fAlphaSpeed = 0.0f;
        TheCamera.GetActiveCam().m_fBetaSpeed  = 0.0f;

        if (++SnapShotFrames <= 20) {
            CTimer::SetTimeScale(0.0f);
            if (SnapShotFrames < 10) {
                RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDONE));
                RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));

                // Como el original: (255 - 255 * frames / 10) * 0,65, truncado
                const uint8 shade = (uint8)(int32)((float)(255 - 255 * SnapShotFrames / 10) * std::bit_cast<float>(0x3F266666u));
                CSprite2d::Draw2DPolygon(
                    0.0f,         0.0f,
                    SCREEN_WIDTH, 0.0f,
                    0.0f,         SCREEN_HEIGHT,
                    SCREEN_WIDTH, SCREEN_HEIGHT,
                    { shade, shade, shade, shade }
                );

                RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
                RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
            }
        } else {
            bSnapShotActive = false;
            CTimer::ResetTimeScale();
        }
    }
}

// 0x721D30
void CSpecialFX::ReplayStarted() {
    if (bSnapShotActive) {
        bSnapShotActive = false;
        CTimer::ResetTimeScale();
    }
}
