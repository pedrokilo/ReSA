#include "StdInc.h"

#include "MenuManager.h"
#include "MenuManager_Internal.h"
#include "World.h"
#include "MenuSystem.h"
#include "Pad.h"
#include "Streaming.h"
#include "AudioEngine.h"
#include "Radar.h"
#include "TheZones.h"
#include "PlaceName.h"
#include "UserDisplay.h"

namespace {
// Como el original: en estas páginas el exe inlinea StretchX/StretchY con una constante
// ya plegada en .rdata (x * 0.0015625f o y * 0.002232143f). El plegado no siempre coincide
// con el producto en float, así que se usa el patrón exacto de bits de la constante del exe.
float StretchXK(float x, uint32 kBits) {
    return RsGlobal.maximumWidth == DEFAULT_SCREEN_WIDTH ? x : (float)RsGlobal.maximumWidth * std::bit_cast<float>(kBits);
}

float StretchYK(float y, uint32 kBits) {
    return RsGlobal.maximumHeight == DEFAULT_SCREEN_HEIGHT ? y : (float)RsGlobal.maximumHeight * std::bit_cast<float>(kBits);
}
} // namespace

/**
 * @addr 0x573C90
 */
void CMenuManager::CalculateMapLimits(float& bottom, float& top, float& left, float& right) {
    bottom = m_vMapOrigin.y - m_fMapZoom;
    top    = m_vMapOrigin.y + m_fMapZoom;
    left   = m_vMapOrigin.x - m_fMapZoom;
    right  = m_vMapOrigin.x + m_fMapZoom;
}

// NOTSA
// from 0x577483 to 0x5775E5
void CMenuManager::PlaceRedMarker() {
    const auto pad = CPad::GetPad(m_nPlayerNumber);
    auto pressed = (
         pad->IsCirclePressed()
      || pad->IsMouseLButtonPressed()
      || pad->IsMouseRButtonPressed()
      || pad->IsStandardKeyJustPressed('T') || pad->IsStandardKeyJustPressed('t')
    );
    if (!pressed)
        return;

    if (CTheScripts::HideAllFrontEndMapBlips && CTheScripts::bPlayerIsOffTheMap)
        return;

    if (m_nTargetBlipIndex) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_BACK);
        CRadar::ClearBlip(m_nTargetBlipIndex);
        m_nTargetBlipIndex = 0;
    } else {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT);
        m_nTargetBlipIndex = CRadar::SetCoordBlip(BLIP_COORD, { m_vMousePos.x, m_vMousePos.y, 0.0f }, BLIP_COLOUR_RED, BLIP_DISPLAY_BLIPONLY, "CODEWAY");
        CRadar::SetBlipSprite(m_nTargetBlipIndex, RADAR_SPRITE_WAYPOINT);
    }
}

// NOTSA 0x5775E5 to 0x577918
void CMenuManager::RadarZoomIn() {
    const auto pad = CPad::GetPad(m_nPlayerNumber);
    auto pressed = ( // todo:
           pad->NewState.LeftShoulder2
        && !pad->NewState.RightShoulder2
        && CPad::NewMouseControllerState.isMouseWheelMovedUp
        && CPad::NewKeyState.pgup
    );
    if (!pressed)
        return;

    // todo:
    auto v103 = 320.0f - m_vMapOrigin.x;
    auto v115 = 224.0f - m_vMapOrigin.y;
    auto v5 = 1.0f / m_fMapZoom;
    auto x = v5 * v103;
    auto y = v5 * v115;

    if (CTimer::GetTimeInMSPauseMode() - m_LastActionTime <= 20)
        return;

    if (m_fMapZoom >= FRONTEND_MAP_RANGE_MAX) {
        m_fMapZoom = FRONTEND_MAP_RANGE_MAX;
    } else {
        m_fMapZoom += 7.0f;
        if (CPad::NewMouseControllerState.isMouseWheelMovedUp) {
            m_fMapZoom += 21.0f;
        }
        m_vMapOrigin.x -= (x * m_fMapZoom - v103);
        m_vMapOrigin.y -= (y * m_fMapZoom - v115);
    }

    auto radar = CRadar::TransformRealWorldPointToRadarSpace(m_vMousePos);
    CRadar::LimitRadarPoint(radar);
    auto screen = CRadar::TransformRadarPointToScreenSpace(radar);

    while (screen.x > 576.0f) {
        m_vMousePos.x = m_vMousePos.x - 1.0f;
        radar = CRadar::TransformRealWorldPointToRadarSpace(m_vMousePos);
        CRadar::LimitRadarPoint(radar);
        screen = CRadar::TransformRadarPointToScreenSpace(radar);
    }

    while (screen.x < 64.0f) {
        m_vMousePos.x = m_vMousePos.x + 1.0f;
        radar = CRadar::TransformRealWorldPointToRadarSpace(m_vMousePos);
        CRadar::LimitRadarPoint(radar);
        screen = CRadar::TransformRadarPointToScreenSpace(radar);
    }

    while (screen.y < 64.0f) {
        m_vMousePos.y = m_vMousePos.y - 1.0f;
        radar = CRadar::TransformRealWorldPointToRadarSpace(m_vMousePos);
        CRadar::LimitRadarPoint(radar);
        screen = CRadar::TransformRadarPointToScreenSpace(radar);
    }

    while (screen.y > 384.0f) {
        m_vMousePos.y = m_vMousePos.y + 1.0f;
        radar = CRadar::TransformRealWorldPointToRadarSpace(m_vMousePos);
        CRadar::LimitRadarPoint(radar);
        screen = CRadar::TransformRadarPointToScreenSpace(radar);
    }
}

// 0x575130
void CMenuManager::PrintMap() {
    const auto pad = CPad::GetPad(m_nPlayerNumber);
    if (CPad::NewKeyState.standardKeys['Z'] || CPad::NewKeyState.standardKeys['z']) {
        m_bMapLoaded = false;
        m_DisplayTheMouse = false;
    }
    m_bDrawingMap = true;
    CRadar::InitFrontEndMap();
    if (!m_bMapLoaded) {
        if (m_nSysMenu >= 0) { // Como el original (0x575193): comprueba >= 0, no != MENU_UNDEFINED
            CMenuSystem::SwitchOffMenu(m_nSysMenu);
            m_nSysMenu = CMenuSystem::MENU_UNDEFINED;
        }

        m_vMapOrigin = CVector2D(320.0f, 206.0f);
        m_fMapZoom = 140.0f;
        auto radar = CRadar::TransformRealWorldPointToRadarSpace(FindPlayerCentreOfWorld_NoSniperShift(0));
        CRadar::LimitRadarPoint(radar);
        const auto screen = CRadar::TransformRadarPointToScreenSpace(radar);

        // Como el original (0x575208): compara contra origen +/- 140 (no contra la diferencia)
        // y en el orden x>, y>, x<, y<
        const auto BOUNDARY = 140.0f;
        if (screen.x > m_vMapOrigin.x + BOUNDARY) {
            m_fMapZoom -= screen.x - (m_vMapOrigin.x + BOUNDARY);
        } else if (screen.y > m_vMapOrigin.y + BOUNDARY) {
            m_fMapZoom -= screen.y - (m_vMapOrigin.y + BOUNDARY);
        } else if (screen.x < m_vMapOrigin.x - BOUNDARY) {
            m_fMapZoom -= (m_vMapOrigin.x - BOUNDARY) - screen.x;
        } else if (screen.y < m_vMapOrigin.y - BOUNDARY) {
            m_fMapZoom -= (m_vMapOrigin.y - BOUNDARY) - screen.y;
        }
        m_fMapZoom = std::max(m_fMapZoom, 70.0f);
    }
    // Como el original (0x575246): multiplica por 1/6 y 0.2 en float en lugar de dividir
    const auto zoomFactorForStretchX = m_fMapZoom * std::bit_cast<float>(0x3E2AAAABu);
    const auto zoomFactorForStretchY = m_fMapZoom * 0.2f;
    // Como el original (0x5752F2): los límites del mapa se truncan a entero con _ftol2
    const auto mapTopI    = (int32)StretchYK(60.0f, 0x3E092492u);
    const auto mapBottomI = (int32)StretchYK(388.0f, 0x3F5DB6DBu);
    const auto mapLeftI   = (int32)StretchXK(60.0f, 0x3DC00000u);
    const auto mapRightI  = (int32)StretchXK(580.0f, 0x3F680000u);
    const CRect mapArea{
        (float)mapLeftI,
        (float)mapTopI,
        (float)mapRightI,
        (float)mapBottomI
    };
    const CVector2D mapOriginOffset{
        StretchX(m_vMapOrigin.x - m_fMapZoom),
        StretchY(m_vMapOrigin.y - m_fMapZoom)
    };

    if (m_bMapLoaded) {
        if (m_bStreamingDisabled && !m_bAllStreamingStuffLoaded) {
            m_RadarVisibilityChangeTime = CTimer::GetTimeInMSPauseMode();
            m_bViewRadar = false;
        }
        if (CTimer::GetTimeInMSPauseMode() - m_RadarVisibilityChangeTime > 400) {
            m_bViewRadar = true;
        }
    } else {
        m_RadarVisibilityChangeTime = CTimer::GetTimeInMSPauseMode();
        m_bViewRadar = false;
    }
    if (m_bMapLoaded) {
        if (m_bAllStreamingStuffLoaded) {
            m_bStreamingDisabled = false;
        }
        const CRect coords = { 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT };
        if (m_bViewRadar) {
            CSprite2d::DrawRect(coords, MENU_MAP_BACKGROUND);
            const auto stretchX = StretchX(zoomFactorForStretchX);
            const auto stretchY = StretchY(zoomFactorForStretchX);
            for (auto x = 0u; x < MAX_RADAR_WIDTH_TILES; x++) {
                for (auto y = 0u; y < MAX_RADAR_HEIGHT_TILES; y++) {
                    if (mapArea.left - stretchX < x * stretchX + mapOriginOffset.x) {
                        if (mapArea.bottom - stretchY < y * stretchY + mapOriginOffset.y) {
                            if (mapArea.right + stretchX > (x + 1) * stretchX + mapOriginOffset.x) {
                                if (mapArea.top + stretchY > (y + 1) * stretchY + mapOriginOffset.y) {
                                    CRadar::DrawRadarSectionMap(
                                        x, y,
                                        { StretchX(zoomFactorForStretchX) * x + mapOriginOffset.x,
                                          StretchY(zoomFactorForStretchX) * y + mapOriginOffset.y,
                                          StretchX(zoomFactorForStretchX) * (x + 1) + mapOriginOffset.x,
                                          StretchY(zoomFactorForStretchX) * (y + 1) + mapOriginOffset.y }
                                    );
                                }
                            }
                        }
                    }
                }
            }
        } else {
            CSprite2d::DrawRect(coords, MENU_BG);
            SmallMessageScreen("FEM_PWT");
            m_bAllStreamingStuffLoaded = true;
        }
    } else {
        CSprite2d::DrawRect(
            { StretchX(m_vMapOrigin.x - 145.0f),
              StretchY(m_vMapOrigin.y - 145.0f),
              StretchX(m_vMapOrigin.x + 145.0f),
              StretchY(m_vMapOrigin.y + 145.0f) },
            MENU_MAP_BORDER
        );

        CSprite2d::DrawRect(
            { StretchX(m_vMapOrigin.x - 141.0f),
              StretchY(m_vMapOrigin.y - 141.0f),
              StretchX(m_vMapOrigin.x + 141.0f),
              StretchY(m_vMapOrigin.y + 141.0f) },
            MENU_BG
        );

        CSprite2d::DrawRect(
            { StretchX(m_vMapOrigin.x - 140.0f),
              StretchY(m_vMapOrigin.y - 140.0f),
              StretchX(m_vMapOrigin.x + 140.0f),
              StretchY(m_vMapOrigin.y + 140.0f) },
            MENU_MAP_BACKGROUND
        );

        m_apBackgroundTextures[7].Draw(
            { StretchX(m_vMapOrigin.x - m_fMapZoom),
              StretchY(m_vMapOrigin.y - m_fMapZoom),
              StretchX(m_vMapOrigin.x + m_fMapZoom),
              StretchY(m_vMapOrigin.y + m_fMapZoom) },
            CRGBA(255, 255, 255, 255)
        );
    }

    if (m_bViewRadar || !m_bMapLoaded) {
        CRadar::DrawRadarGangOverlay(1);
        if ((uint32)CTheZones::ZonesRevealed < 80) { // Como el original: comparación sin signo
            for (auto x = 0u; x < MAX_RADAR_WIDTH_TILES - 2; x++) {
                for (auto y = 0u; y < MAX_RADAR_HEIGHT_TILES - 2; y++) {
                    if (!CTheZones::ZonesVisited[x][y]) {
                        CSprite2d::DrawRect(
                            { StretchX(zoomFactorForStretchY) * x + mapOriginOffset.x,
                              StretchY(zoomFactorForStretchY) * y + mapOriginOffset.y,
                              StretchX(zoomFactorForStretchY) * (x + 1) + mapOriginOffset.x,
                              StretchY(zoomFactorForStretchY) * (y + 1) + mapOriginOffset.y },
                            CRGBA(111, 137, 170, 200)
                        ); // radar fog
                    }
                }
            }
        }
        if (!CTheScripts::HideAllFrontEndMapBlips && !CTheScripts::bPlayerIsOffTheMap) {
            CRadar::DrawBlips();
        }
    }

    // Como el original (0x575B54): el nombre de zona se obtiene del CPlaceName global de
    // CUserDisplay siempre que el mapa esté cargado, antes de dibujar los bordes
    const GxtChar* placeNameText{};
    if ((m_bViewRadar || m_bMapLoaded) && m_bMapLoaded) {
        placeNameText = CUserDisplay::PlaceName.GetForMap(m_vMousePos);

        // Como el original (0x575B8A): el margen de 4 se trunca a entero y se guarda en 8 bits
        const auto border4X = (uint8)(int32)StretchXK(4.0f, 0x3BCCCCCDu);
        const auto border4Y = (uint8)(int32)StretchYK(4.0f, 0x3C124925u);

        // border between map and background
        CSprite2d::DrawRect(
            { 0.0f, 0.0f, SCREEN_WIDTH, mapArea.bottom },
            MENU_MAP_BORDER
        );

        CSprite2d::DrawRect(
            { 0.0f, mapArea.top, SCREEN_WIDTH, SCREEN_HEIGHT },
            MENU_MAP_BORDER
        );

        CSprite2d::DrawRect(
            { 0.0f, 0.0f, mapArea.left, SCREEN_HEIGHT },
            MENU_MAP_BORDER
        );

        CSprite2d::DrawRect(
            { mapArea.right, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT },
            MENU_MAP_BORDER
        );

        // background (Como el original: márgenes calculados en entero)
        CSprite2d::DrawRect(
            { 0.0f, 0.0f, SCREEN_WIDTH, (float)(mapTopI - border4Y) },
            MENU_BG
        );

        CSprite2d::DrawRect(
            { 0.0f, (float)(mapBottomI + border4Y), SCREEN_WIDTH, SCREEN_HEIGHT },
            MENU_BG
        );

        CSprite2d::DrawRect(
            { 0.0f, 0.0f, (float)(mapLeftI - border4X), SCREEN_HEIGHT },
            MENU_BG
        );

        CSprite2d::DrawRect(
            { (float)(mapRightI + border4X), 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT },
            MENU_BG
        );
    }

    // 0x575E21
    if (m_bViewRadar) {
        if ((uint32)CTheZones::ZonesRevealed >= 80
            || CTheZones::GetCurrentZoneLockedOrUnlocked(m_vMousePos)
                && !pad->NewMouseControllerState.isMouseLeftButtonPressed)
        {
            CFont::SetFontStyle(FONT_PRICEDOWN);
            CFont::SetWrapx(640.0);
            CFont::SetDropColor(MENU_BG);
            CFont::SetScale(StretchXK(0.8f, 0x3AA3D70Au), StretchYK(0.8f, 0x3AEA0EA1u));
            CFont::SetColor(MENU_TEXT_LIGHT_GRAY);
            CFont::SetEdge(2);
            CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
            CFont::PrintString(
                mapArea.right - StretchXK(30.0f, 0x3D400000u), mapArea.top - StretchYK(30.0f, 0x3D892492u),
                placeNameText
            );
        }

        if (m_bMapLegend) {
            // Como el original (0x575FA3): la altura de la ventana se acumula en entero,
            // truncando con _ftol2 en cada paso
            const auto midPoint = (CRadar::MapLegendCounter - 1) / 2;
            auto windowBottom = (int32)StretchYK(100.0f, 0x3E649249u);
            for (auto n = midPoint + 3; n > 0; n--) {
                windowBottom = (int32)((float)windowBottom + StretchYK(19.0f, 0x3D2DB6DBu));
            }

            DrawWindow({
                    StretchXK(95.0f, 0x3E180000u),
                    StretchYK(100.0f, 0x3E649249u),
                    StretchXK(555.0f, 0x3F5E0000u), // Como el original (0x576020): 555, no 550
                    (float)windowBottom
                }, "FE_MLG", 0, {0, 0, 0, 190}, true, true
            ); // Map Legend
            CFont::SetWrapx((float)(RsGlobal.maximumWidth - 40));
            CFont::SetRightJustifyWrap(84.0f);
            CFont::SetDropShadowPosition(1);
            CFont::SetDropColor(MENU_BG);
            CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
            CFont::SetFontStyle(FONT_MENU);
            CFont::SetScale(StretchXK(0.3f, 0x39F5C290u), StretchYK(0.55f, 0x3AA0EA0Fu));
            CFont::SetColor(MENU_TEXT_SELECTED);
            // Como el original (0x57619C): posiciones enteras; Y se acumula truncando en cada paso
            auto currentY = (int32)StretchYK(127.0f, 0x3E912492u);
            auto currentX = (int32)StretchXK(160.0f, 0x3E800000u);
            for (auto legend = 0; legend < CRadar::MapLegendCounter; ++legend) {
                CRadar::DrawLegend(
                    currentX,
                    currentY,
                    static_cast<eRadarSprite>(CRadar::MapLegendList[legend])
                );

                if (legend == midPoint) {
                    currentX = (int32)StretchXK(350.0f, 0x3F0C0000u);
                    currentY = (int32)StretchYK(127.0f, 0x3E912492u);
                } else {
                    currentY = (int32)((float)currentY + StretchYK(19.0f, 0x3D2DB6DBu));
                }
            }
        }
        if (m_nSysMenu >= 0) { // Como el original (0x57629F): comprueba >= 0
            CMenuSystem::Process(m_nSysMenu);
        }
    }
    m_bDrawingMap = false;
    CFont::SetWrapx(SCREEN_WIDTH - 10);
    CFont::SetRightJustifyWrap(10.0f);
    if (m_bMapLoaded) {
        if (m_nSysMenu >= 0) { // Como el original (0x5762E1): comprueba >= 0
            // CURSORS - MOVE UP/DOWN
            // RETURN - TOGGLE OPTION
            DisplayHelperText("FEH_MPB"); 
        } else {
            // LMB/CURSORS - SCROLL
            // PGUP/PGDN/MSWHEEL - ZOOM
            // Z - OVERVIEW , L - LEGEND
            // RMB/T - TARGET , SPACEBAR - BLIPS MENU
            // ESC - BACK
            DisplayHelperText("FEH_MPH"); 
        }
        
    }
    m_bMapLoaded = true;
}

// 0x574900
void CMenuManager::PrintStats() {
    static auto& currentStatId  = StaticRef<eStats>(0xB794CC);
    static auto& scrollPos      = StaticRef<float>(0x8CDFF4); // -120.0f
    static auto& prevScreenItem = StaticRef<int8>(0x8CDFF8);  // 15

    // Como el original (0x57490B): el ítem se limita a 8 en una copia local de 8 bits;
    // m_nCurrentScreenItem no se modifica.
    const auto screenItem = (uint8)std::min(m_nCurrentScreenItem, 8);
    // 0x574933
    if ((uint8)prevScreenItem != screenItem) {
        scrollPos = -120.0f;
        prevScreenItem = (int8)screenItem;
    }

    const auto numStats = CStats::ConstructStatLine(99'999, screenItem);
    CFont::SetFontStyle(eFontStyle::FONT_MENU);
    CFont::SetScale(StretchXK(0.3f, 0x39F5C290u), StretchYK(0.75f, 0x3ADB6DB7u));

    // 0x5749DA
    if (CTimer::GetTimeInMSPauseMode() - m_StatsScrollTime > 40) {
        if (m_fStatsScrollSpeed > 0.0f) {
            const float scrollDelta = StretchYK(100.0f, 0x3E649249u) / m_fStatsScrollSpeed;
            scrollPos += m_nStatsScrollDirection ? scrollDelta : -scrollDelta;
        }
        m_StatsScrollTime = CTimer::GetTimeInMSPauseMode();
    }

    const float minY = StretchYK(50.0f, 0x3DE49249u);
    const float maxY = SCREEN_HEIGHT - StretchYK(50.0f, 0x3DE49249u);
    const float visibleTop = StretchYK(50.0f, 0x3DE49249u);
    const float visibleBottom = StretchYK(360.0f, 0x3F4DB6DBu);

    // 0x574A4E
    for (auto i = 0; i < numStats; ++i) {
        float y = StretchYK(54.0f, 0x3DF6DB6Eu) * (float)i + minY - scrollPos;

        const float dy = StretchY(float(numStats + 7) * 54.0f);
        while (y < minY) {
            y += dy;
        }

        while (y > maxY) {
            y -= dy;
        }

        if (y > visibleTop && y < visibleBottom) {
            CStats::ConstructStatLine(i, screenItem);

            if (!gGxtString[0]) {
                y -= StretchYK(37.0f, 0x3DA92492u);
            }

            // 0x574BDF - Como el original: tres comprobaciones encadenadas y solo límite superior
            const float y80  = StretchYK(80.0f, 0x3E36DB6Eu);
            const float y110 = StretchYK(110.0f, 0x3E7B6DB7u);
            const float y300 = StretchYK(300.0f, 0x3F2B6DB7u);
            const float y330 = StretchYK(330.0f, 0x3F3C9249u);
            float alpha_f = 0.0f;
            if (y < y330 && y > y300) {
                // Fade Out
                alpha_f = (y330 - y) * 8.0f;
            }
            if (y > y80 && y < y110) {
                // Fade In
                alpha_f = (y - y80) * 8.0f;
            }
            if (!(y < y110) && y <= y300) {
                alpha_f = 255.0f;
            } else if (alpha_f > 255.0f) {
                alpha_f = 255.0f;
            }
            const auto alpha = (uint8)(int32)alpha_f; // _ftol2 y byte bajo

            // 0x574D3B
            CFont::SetDropColor(CRGBA(MENU_BG, alpha));
            CFont::SetEdge(1);
            CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
            CFont::SetColor(CRGBA(MENU_TEXT_LIGHT_GRAY, alpha));
            CFont::PrintString(StretchXK(450.0f, 0x3F340000u), y, gGxtString);

            const auto color = CRGBA(MENU_TEXT_SELECTED, alpha);
            // 0x574DD2
            if (currentStatId) {
                const float val = CStats::GetStatValue(currentStatId) * 0.001f * 100.0f;
                const float clamped = val < 1000.0f ? val : 1000.0f; // Como el original: NaN -> 1000

                // Como el original (0x574F02): ancho y alto sin comprobar 640x448,
                // como (W * 0.0015625f) * 100 y (H * 0.002232143f) * 10 truncados con _ftol2
                const auto height = (int32)((float)RsGlobal.maximumHeight * std::bit_cast<float>(0x3B124925u) * 10.0f);
                const auto width  = (int32)((float)RsGlobal.maximumWidth * std::bit_cast<float>(0x3ACCCCCDu) * 100.0f);
                CSprite2d::DrawBarChart(
                    StretchXK(400.0f, 0x3F200000u), StretchYK(17.0f, 0x3D1B6DB7u) + y,
                    (uint16)width, (uint8)height,
                    clamped, 0, 0, 1, color, CRGBA(0, 0, 0, 0)
                );
            } else {
                CFont::SetColor(color);
                CFont::PrintString(StretchXK(450.0f, 0x3F340000u), StretchYK(17.0f, 0x3D1B6DB7u) + y, gGxtString2);
            }
        }
    }

    // 0x574F7E
    CFont::SetEdge(1);
    CFont::SetDropColor(MENU_BG);
    CFont::SetColor(MENU_TEXT_LIGHT_GRAY);
    CFont::SetFontStyle(eFontStyle::FONT_PRICEDOWN);
    CFont::SetScale(StretchXK(0.6f, 0x3A75C290u), StretchYK(0.8f, 0x3AEA0EA1u));
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);

    // Como el original (0x57502F): sprintf en el búfer global gString y conversión a un búfer local
    GxtChar ratingText[80];
    std::sprintf(gString, "%d", CStats::FindCriminalRatingNumber());
    AsciiToGxtChar(gString, ratingText);
    GxtCharStrcpy(gGxtString, CStats::FindCriminalRatingString());
    CFont::PrintString(StretchXK(450.0f, 0x3F340000u), StretchYK(40.0f, 0x3DB6DB6Eu), gGxtString);
    CFont::PrintString(StretchXK(450.0f, 0x3F340000u), StretchYK(57.0f, 0x3E024925u), ratingText);
}

// 0x576320
void CMenuManager::PrintBriefs() {
    CFont::SetColor(MENU_TEXT_LIGHT_GRAY);
    CFont::SetDropColor(MENU_BG);
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
    CFont::SetScaleForCurrentLanguage(StretchXK(0.49f, 0x3A48B439u), StretchYK(0.7f, 0x3ACCCCCDu));
    CFont::SetWrapx(StretchXK(560.0f, 0x3F600000u));
    CFont::SetDropShadowPosition(1);
    CFont::SetJustify(false);

    static constexpr int32 BRIEFS_MAX_LINES_ON_SCREEN = 4;

    // Como el original (0x576418): bucle con índice con signo desde la fila seleccionada hasta
    // fila - 3, sin protección de desbordamiento; con fila < 3 lee índices negativos del array.
    float h = 100.0f;
    for (int32 i = m_nSelectedRow; i >= (int32)m_nSelectedRow - (BRIEFS_MAX_LINES_ON_SCREEN - 1); i--) {
        auto& brief = CMessages::PreviousBriefs.data()[i];
        if (!brief.Text)
            continue;

        CMessages::InsertNumberInString2(brief.Text, brief.NumbersToInsert, gGxtString);
        CMessages::InsertStringInString(gGxtString, brief.StringToInsert);
        CMessages::InsertPlayerControlKeysInString(gGxtString);
        CFont::PrintString(StretchXK(70.0f, 0x3DE00000u), StretchY(h), gGxtString);

        h += 70.0f; // 210 / (BRIEFS_MAX_LINES_ON_SCREEN - 1)
    }
    CFont::SetJustify(false); // redundant

    static auto& drawArrows = StaticRef<bool>(0x8CDFF9); // true

    if (!m_bMapLoaded)
        return;

    if (CTimer::GetTimeInMSPauseMode() - m_BriefsArrowBlinkTime > 700) {
        m_BriefsArrowBlinkTime = CTimer::GetTimeInMSPauseMode();
        drawArrows = !drawArrows;
    }

    if (drawArrows) {
        // up arrow
        if (m_nSelectedRow < 19u && CMessages::PreviousBriefs[m_nSelectedRow + 1].Text) {
            CSprite2d::Draw2DPolygon(
                StretchXK(50.0f, 0x3DA00000u), StretchYK(100.0f, 0x3E649249u),
                StretchXK(55.0f, 0x3DB00000u), StretchYK(110.0f, 0x3E7B6DB7u),
                StretchXK(45.0f, 0x3D900000u), StretchYK(110.0f, 0x3E7B6DB7u),
                StretchXK(50.0f, 0x3DA00000u), StretchYK(100.0f, 0x3E649249u),
                MENU_TEXT_LIGHT_GRAY
            );
        }

        // down arrow
        if (m_nSelectedRow > 3u) {
            CSprite2d::Draw2DPolygon(
                StretchXK(50.0f, 0x3DA00000u), StretchYK(348.0f, 0x3F46DB6Eu),
                StretchXK(55.0f, 0x3DB00000u), StretchYK(338.0f, 0x3F412492u),
                StretchXK(45.0f, 0x3D900000u), StretchYK(338.0f, 0x3F412492u),
                StretchXK(50.0f, 0x3DA00000u), StretchYK(348.0f, 0x3F46DB6Eu),
                MENU_TEXT_LIGHT_GRAY
            );
        }
    }
}

// 0x5746F0
void CMenuManager::PrintRadioStationList() {
    // draw all, except current
    for (auto i = 1u; i < std::size(m_apRadioSprites); i++) {
        if (m_nRadioStation == i) {
            continue;
        }

        m_apRadioSprites[i].Draw(
            StretchX(float(47 * (i - 1) + 44)),
            StretchYK(300.0f, 0x3F2B6DB7u),
            StretchXK(35.0f, 0x3D600000u),
            StretchYK(35.0f, 0x3DA00000u),
            CRGBA(MENU_TEXT_WHITE, 30) // Como el original: alfa 30 (0x5747B2)
        );
    }

    // highlight current radio station
    if (m_nRadioStation > 0 && m_nRadioStation < (int8)std::size(m_apRadioSprites)) {
        m_apRadioSprites[m_nRadioStation].Draw(
            StretchX(float(47 * m_nRadioStation - 15)),
            StretchYK(290.0f, 0x3F25B6DBu),
            StretchXK(60.0f, 0x3DC00000u),
            StretchYK(60.0f, 0x3E092492u),
            MENU_TEXT_WHITE
        );
    }
}
