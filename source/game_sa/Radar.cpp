/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRadar
 * Reconstrucción: COMPLETA — 64/64 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 64/64 entradas.
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

#include "Radar.h"
#include "EntryExitManager.h"
#include "X87Math.h"

namespace {
// Constantes float leidas del EXE (algunas no son el float mas cercano al literal decimal).
inline float RadarExeFloat(uintptr_t address) {
    return std::bit_cast<float>(*reinterpret_cast<const uint32*>(address));
}
// Como el original: fild RsGlobal.maximumWidth/Height * 0x859520/0x859524 (x87 a 24 bits); despues el EXE multiplica por la constante de cada elemento
// (SCREEN_STRETCH_X/Y de common.h hace valor * ancho / 640, que redondea distinto).
inline float RadarStretchBaseX() { return (float)RsGlobal.maximumWidth * RadarExeFloat(0x859520); }
inline float RadarStretchBaseY() { return (float)RsGlobal.maximumHeight * RadarExeFloat(0x859524); }
// _ftol2 (0x821B40): truncamiento a 64 bits; las funciones usan solo la parte baja.
inline int32 RadarFtol(float v) { return (int32)(int64)v; }
// Envoltura de hook de 0x583480 (definida junto al cuerpo): conserva EDX y deja EAX = entrada, ECX = salida como el EXE.
CVector2D* __cdecl RadarTransformScreenExe(CVector2D* out, const CVector2D* in);
}

constexpr std::array<airstrip_info, NUM_AIRSTRIPS> airstrip_table = { // 0x8D06E0
    airstrip_info{ { +1750.0f,  -2494.0f }, 180.0f, 1000.0f }, // AIRSTRIP_LS_AIRPORT
    airstrip_info{ { -1373.0f,  +120.00f }, 315.0f, 1500.0f }, // AIRSTRIP_SF_AIRPORT
    airstrip_info{ { +1478.0f,  +1461.0f }, 90.0f,  1200.0f }, // AIRSTRIP_LV_AIRPORT
    airstrip_info{ { +175.00f,  +2502.0f }, 180.0f, 1000.0f }  // AIRSTRIP_VERDANT_MEADOWS
};

// Array of TXD slot indices for each radar section's texture
// Index using y, x (In that order)
static auto& gRadarTextures = StaticRef<std::array<std::array<int32, MAX_RADAR_WIDTH_TILES>, MAX_RADAR_HEIGHT_TILES>>(0xBA8478);

// 0x8D0720: como el original las dos primeras entradas y todas las mascaras apuntan a la cadena vacia 0x858B54 (no a nullptr)
SpriteFileName CRadar::RadarBlipFileNames[] = {
    // name,                 maskName
    { "",               "" },
    { "",               "" },
    { "radar_centre",        "" },
    { "arrow",               "" },
    { "radar_north",         "" },
    { "radar_airYard",       "" },
    { "radar_ammugun",       "" },
    { "radar_barbers",       "" },
    { "radar_BIGSMOKE",      "" },
    { "radar_boatyard",      "" },
    { "radar_burgerShot",    "" },
    { "radar_bulldozer",     "" },
    { "radar_CATALINAPINK",  "" },
    { "radar_CESARVIAPANDO", "" },
    { "radar_chicken",       "" },
    { "radar_CJ",            "" },
    { "radar_CRASH1",        "" },
    { "radar_diner",         "" },
    { "radar_emmetGun",      "" },
    { "radar_enemyAttack",   "" },
    { "radar_fire",          "" },
    { "radar_girlfriend",    "" },
    { "radar_hostpitaL",     "" },
    { "radar_LocoSyndicate", "" },
    { "radar_MADDOG",        "" },
    { "radar_mafiaCasino",   "" },
    { "radar_MCSTRAP",       "" },
    { "radar_modGarage",     "" },
    { "radar_OGLOC",         "" },
    { "radar_pizza",         "" },
    { "radar_police",        "" },
    { "radar_propertyG",     "" },
    { "radar_propertyR",     "" },
    { "radar_race",          "" },
    { "radar_RYDER",         "" },
    { "radar_saveGame",      "" },
    { "radar_school",        "" },
    { "radar_qmark",         "" },
    { "radar_SWEET",         "" },
    { "radar_tattoo",        "" },
    { "radar_THETRUTH",      "" },
    { "radar_waypoint",      "" },
    { "radar_TorenoRanch",   "" },
    { "radar_triads",        "" },
    { "radar_triadsCasino",  "" },
    { "radar_tshirt",        "" },
    { "radar_WOOZIE",        "" },
    { "radar_ZERO",          "" },
    { "radar_dateDisco",     "" },
    { "radar_dateDrink",     "" },
    { "radar_dateFood",      "" },
    { "radar_truck",         "" },
    { "radar_cash",          "" },
    { "radar_flag",          "" },
    { "radar_gym",           "" },
    { "radar_impound",       "" },
    { "radar_light",         "" },
    { "radar_runway",        "" },
    { "radar_gangB",         "" },
    { "radar_gangP",         "" },
    { "radar_gangY",         "" },
    { "radar_gangN",         "" },
    { "radar_gangG",         "" },
    { "radar_spray",         "" }
};

constexpr float RADAR_MIN_RANGE = 180.0f;
constexpr float RADAR_MAX_RANGE = 350.0f;
constexpr float RADAR_MIN_SPEED = 0.3f;
constexpr float RADAR_MAX_SPEED = 0.9f;

void CRadar::InjectHooks() {
    RH_ScopedClass(CRadar);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x587FB0);
    RH_ScopedInstall(LoadTextures, 0x5827D0);
    RH_ScopedInstall(DrawLegend, 0x5828A0);
    RH_ScopedInstall(LimitRadarPoint, 0x5832F0);
    RH_ScopedInstall(Shutdown, 0x585940);
    RH_ScopedInstall(SetMapCentreToPlayerCoords, 0x585B20);
    RH_ScopedInstall(InitFrontEndMap, 0x585960);
    RH_ScopedInstall(CalculateBlipAlpha, 0x583420);
    RH_ScopedNamedGlobalInstall(RadarTransformScreenExe, "TransformRadarPointToScreenSpace", 0x583480); // Como el original: EDX conservado, EAX/ECX de salida
    RH_ScopedInstall(TransformRealWorldPointToRadarSpace, 0x583530);
    RH_ScopedInstall(CalculateCachedSinCos, 0x583670);
    RH_ScopedInstall(SetBlipSprite, 0x583D70);
    RH_ScopedInstall(SetBlipAlwaysDisplayInZoom, 0x583DB0);
    RH_ScopedInstall(DrawYouAreHereSprite, 0x584960);
    RH_ScopedInstall(ChangeBlipColour, 0x583AB0);
    RH_ScopedOverloadedInstall(ClearActualBlip, "OG", 0x587C10, void (*)(int32));
    RH_ScopedInstall(ClearBlip, 0x587CE0);
    RH_ScopedOverloadedInstall(ClearBlipForEntity, "OG", 0x587C60, void(*)(eBlipType, int32));
    RH_ScopedInstall(RequestMapSection, 0x584B50);
    RH_ScopedInstall(RemoveMapSection, 0x584BB0);
    RH_ScopedInstall(RemoveRadarSections, 0x584BF0);
    RH_ScopedInstall(DrawRadarSprite, 0x585FF0);
    RH_ScopedInstall(DrawMap, 0x586B00);
    RH_ScopedInstall(DrawRadarMap, 0x586880);
    RH_ScopedOverloadedInstall(StreamRadarSections, "vec", 0x5858D0, void (*)(const CVector&));
    RH_ScopedInstall(SetupRadarRect, 0x584A80);
    RH_ScopedInstall(GetActualBlipArrayIndex, 0x582870);
    RH_ScopedInstall(LimitToMap, 0x583350);
    RH_ScopedInstall(DrawRotatingRadarSprite, 0x584850);
    RH_ScopedInstall(SetBlipFriendly, 0x583EB0);
    RH_ScopedInstall(ChangeBlipDisplay, 0x583D20);
    RH_ScopedInstall(SetBlipEntryExit, 0x583F00);
    RH_ScopedInstall(SetShortRangeCoordBlip, 0x583920);
    RH_ScopedInstall(ChangeBlipScale, 0x583CC0);
    RH_ScopedInstall(GetRadarTraceColour, 0x584770);
    RH_ScopedInstall(ShowRadarMarker, 0x584480);
    RH_ScopedInstall(DrawRadarMask, 0x585700);
    RH_ScopedInstall(Load, 0x5D53C0);
    RH_ScopedInstall(Save, 0x5D5860);
    RH_ScopedInstall(SetBlipFade, 0x583E00); // unused
    RH_ScopedInstall(SetCoordBlipAppearance, 0x583E50);
    RH_ScopedInstall(SetCoordBlip, 0x583820);
    RH_ScopedInstall(SetEntityBlip, 0x5839A0);
    RH_ScopedInstall(DisplayThisBlip, 0x583B40);
    RH_ScopedInstall(ChangeBlipBrightness, 0x583C70);
    RH_ScopedInstall(ShowRadarTrace, 0x583F40);
    RH_ScopedInstall(ShowRadarTraceWithHeight, 0x584070);
    RH_ScopedInstall(DrawCoordBlip, 0x586D60);

    RH_ScopedInstall(SetupAirstripBlips, 0x587D20); // TEST
    RH_ScopedInstall(DrawBlips, 0x588050);
    RH_ScopedInstall(ClipRadarPoly, 0x585040);
    RH_ScopedInstall(DrawAreaOnRadar, 0x5853D0);
    RH_ScopedOverloadedInstall(StreamRadarSections, "xy", 0x584C50, void (*)(int32, int32));
    RH_ScopedInstall(AddBlipToLegendList, 0x5859F0);
    RH_ScopedInstall(Draw3dMarkers, 0x585BF0);
    RH_ScopedInstall(DrawRadarSection, 0x586110);
    RH_ScopedInstall(DrawRadarSectionMap, 0x586520);
    RH_ScopedInstall(DrawRadarGangOverlay, 0x586650);
    // RH_ScopedInstall(DrawEntityBlip, 0x587000);
    RH_ScopedGlobalInstall(LineRadarBoxCollision, 0x584E00);

    // unused
    RH_ScopedInstall(GetNewUniqueBlipIndex, 0x582820);
    RH_ScopedInstall(TransformRadarPointToRealWorldSpace, 0x5835A0);
    RH_ScopedGlobalInstall(IsPointInsideRadar, 0x584D40);
    RH_ScopedGlobalInstall(GetTextureCorners, 0x584D90);
    RH_ScopedGlobalInstall(ClipRadarTileCoords, 0x584B00);    
}

// 0x587FB0
void CRadar::Initialise() {
    airstrip_blip = 0;
    airstrip_location = AIRSTRIP_LS_AIRPORT;

    for (auto& trace : ms_RadarTrace) {
        ClearActualBlip(trace);
        trace.m_nCounter = 1; // Como el original: 0x587FF2 mov word ptr [eax-8],cx (cx = 1) para las 175 entradas
    }

    m_radarRange = 350.0f;

    for (auto y = 0u; y < MAX_RADAR_HEIGHT_TILES; y++) {
        for (auto x = 0u; x < MAX_RADAR_WIDTH_TILES; x++) {
            gRadarTextures[y][x] = CTxdStore::FindTxdSlot(std::format("radar{:02d}", y * MAX_RADAR_WIDTH_TILES + x).c_str());
        }
    }
}

// 0x585940
void CRadar::Shutdown() {
    rng::for_each(RadarBlipSprites, &CSprite2d::Delete);
    RemoveRadarSections();
}

// 0x5827D0
void CRadar::LoadTextures() {
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("hud"));

    for (auto i = 0; i < MAX_RADAR_SPRITES; i++) {
        const auto [texture, mask] = RadarBlipFileNames[i];
        RadarBlipSprites[i].SetTexture(texture); // mask unused
    }

    CTxdStore::PopCurrentTxd();
}

/*!
 * @brief Creates a unique blip handle by blip index from `ms_RadarTrace`. (Inlined)
 * @addr 0x582820
 * @returns Unique blip handle
 */
tBlipHandle CRadar::GetNewUniqueBlipIndex(int32 index) {
    auto& trace = ms_RadarTrace[index];
    if (trace.m_nCounter >= std::numeric_limits<uint16>::max() - 1)
        trace.m_nCounter = 1; // Wrap back to 1
    else
        trace.m_nCounter++; // Increment
    return index | (trace.m_nCounter << 16);
}

/*!
 * @param blip Blip handle
 * @addr 0x582870
 * @returns Index of the handle in `ms_RadarTrace`
 */
int32 CRadar::GetActualBlipArrayIndex(tBlipHandle blip) {
    if (blip == -1)
        return -1;

    const auto  traceIndex = static_cast<uint16>(blip);
    const auto& trace      = ms_RadarTrace.data()[traceIndex]; // Como el original: sin comprobar el indice (0x58287B..0x582890)
    const auto  counter    = static_cast<uint16>(blip >> 16);
    if (counter != trace.m_nCounter || !trace.m_bTrackingBlip)
        return -1;

    return traceIndex;
}

// 0x5828A0
void CRadar::DrawLegend(int32 x, int32 y, eRadarSprite blipType) {
    // Como el original: sx = fild ancho * 0x859520 y sy = fild alto * 0x859524 (x87 a 24 bits); cada coordenada es sx/sy * constante.
    const float sx = RadarStretchBaseX(), sy = RadarStretchBaseY();

    // 0x582DAB..0x582DEE: x + sx * 20 y y + sy * 3 (0x858BA4 = 20.0, 0x858B3C = 3.0)
    CFont::PrintString((float)x + sx * 20.0f, (float)y + sy * 3.0f, GetBlipName(blipType));

    if (blipType >= 0) { // The blip is a sprite, so just draw it.
        // 0x582DFA..0x582E58: ambos lados con sx * 16 (0x8599D0): izquierda x, +4 = y + lado, derecha x + lado, +0xC = y
        const float side = sx * 16.0f;
        RadarBlipSprites.data()[blipType].Draw(CRect{ (float)x, (float)y, (float)x + side, (float)y + side }, CRGBA{ 255, 255, 255, 255 });
        return;
    }

    static auto& legendTraceHeight = StaticRef<eRadarTraceHeight>(0xBAA350); // = eRadarTraceHeight::RADAR_TRACE_LOW;
    static auto& legendTraceTimer  = StaticRef<uint32>(0xBAA354); // = CTimer::GetTimeInMS();

    // 0x582E62..0x582E78: el color de la flecha se lee antes de actualizar el temporizador
    const CRGBA arrowColor = ArrowBlipColour.data()[-blipType];

    if (CTimer::GetTimeInMSPauseMode() - legendTraceTimer > 600) {
        legendTraceTimer = CTimer::GetTimeInMSPauseMode();
        // 0x582E92..0x582EA4: cmp byte [0BAA350h],2 -> 0, si no inc
        legendTraceHeight = (uint8(legendTraceHeight) == uint8(RADAR_TRACE_NORMAL)) ? RADAR_TRACE_LOW : eRadarTraceHeight(uint8(legendTraceHeight) + 1);
    }

    // 0x582EAA..0x582EF0: _ftol2 trunca (no redondea) sx * 8 + x y sy * 8 + y
    const float posX = (float)RadarFtol(sx * 8.0f + (float)x);
    const float posY = (float)RadarFtol(sy * 8.0f + (float)y);

    const CRGBA outlineColor{ 0, 0, 0, 255 };

    switch (uint8(legendTraceHeight)) {
    case RADAR_TRACE_LOW: { // 0x5830D9..0x5831D3
        const float x7 = sx * 7.0f, x5 = sx * 5.0f;
        const float yTop = posY - sy * 7.0f;  // 0x5830F6..0x583104: fld 7.0 (0x858F48); fmul sy; fsubr posY
        const float yBase = sy * 6.0f + posY; // fmul 6.0 (0x858B44); fadd posY
        CSprite2d::Draw2DPolygon(
            x7 + posX, yBase,
            posX - x7, yBase,
            posX,      yTop,
            posX,      yTop,
            outlineColor
        );
        const float y5 = sy * 5.0f;
        CSprite2d::Draw2DPolygon(
            x5 + posX, y5 + posY,
            posX - x5, y5 + posY,
            posX,      posY - y5,
            posX,      posY - y5,
            arrowColor
        );
        break;
    }
    case RADAR_TRACE_HIGH: { // 0x582FD8..0x5830D8
        const float x7 = sx * 7.0f, x5 = sx * 5.0f;
        const float yBase = posY - sy * 6.0f; // fld 6.0 (0x858B44); fmul sy; fsubr posY
        const float yApex = sy * 7.0f + posY; // fmul 7.0 (0x858F48); fadd posY
        CSprite2d::Draw2DPolygon(
            posX,      yApex,
            posX,      yApex,
            x7 + posX, yBase,
            posX - x7, yBase,
            outlineColor
        );
        const float y5 = sy * 5.0f;
        CSprite2d::Draw2DPolygon(
            posX,      y5 + posY,
            posX,      y5 + posY,
            x5 + posX, posY - y5,
            posX - x5, posY - y5,
            arrowColor
        );
        break;
    }
    case RADAR_TRACE_NORMAL: { // 0x582F0E..0x582FD3: los desplazamientos en Y usan sy (el codigo anterior usaba sx)
        const float x5 = sx * 5.0f, y5 = sy * 5.0f;
        CSprite2d::DrawRect(CRect{ posX - x5, posY - y5, x5 + posX, y5 + posY }, outlineColor);
        const float x4 = sx * 4.0f, y4 = sy * 4.0f;
        CSprite2d::DrawRect(CRect{ posX - x4, posY - y4, x4 + posX, y4 + posY }, arrowColor);
        break;
    }
    default:
        break;
    }
}

/*!
 * @brief Limits a 2D vector to the radar. (which is a unit circle)
 * @brief This function does not effect the vector if the map is being drawn.
 * @param point The vector to be limited.
 * @addr 0x5832F0
 * @returns Magnitude of the vector before limiting.
 */
float CRadar::LimitRadarPoint(CVector2D& point) {
    const auto mag = point.Magnitude();

    if (FrontEndMenuManager.m_bDrawingMap)
        return mag;

    if (mag > 1.0f) {
        // not in unit circle
        point.Normalise();
    }

    return mag;
}

// 0x583350
void CRadar::LimitToMap(float& x, float& y) {
    // Como el original (0x583351..0x583370): m_fMapZoom pasa por _ftol2 y movsx eax,ax (entero de 16 bits con signo); sin mapa cargado vale 140.
    const float zoom = FrontEndMenuManager.m_bMapLoaded ? (float)(int16)(int64)FrontEndMenuManager.m_fMapZoom : 140.0f;
    const float sx = RadarStretchBaseX(), sy = RadarStretchBaseY();

    // 0x583377..0x5833A0: (origen.x - zoom) * sx; fcom [x]: solo se sustituye si el limite es mayor que x
    const float minX = (FrontEndMenuManager.m_vMapOrigin.x - zoom) * sx;
    if (minX > x) {
        x = minX;
    }
    // 0x5833A0..0x5833C3: (zoom + origen.x) * sx; test ah,5 / jp: solo se sustituye si el limite es menor que x
    const float maxX = (zoom + FrontEndMenuManager.m_vMapOrigin.x) * sx;
    if (maxX < x) {
        x = maxX;
    }

    // 0x5833C5..0x5833EE
    const float minY = (FrontEndMenuManager.m_vMapOrigin.y - zoom) * sy;
    if (minY > y) {
        y = minY;
    }
    // 0x5833EE..0x58340F
    const float maxY = (zoom + FrontEndMenuManager.m_vMapOrigin.y) * sy;
    if (maxY < y) {
        y = maxY;
    }
}

/*!
 * @brief Calculates transparency of a blip based on distance from the player.
 * @brief This function returns full opacity value if the map is being drawn.
 * @brief Blip transparency is not used in SA. It's a VC feature.
 * @param distance Distance from the player
 * @addr 0x583420
 * @returns Transparency value in range of [70,255].
 */
uint8 CRadar::CalculateBlipAlpha(float distance) {
    if (FrontEndMenuManager.m_bDrawingMap) {
        return 255;
    }

    // Como el original (0x583432..0x583476): distancia * 0x85F0A0 (1/6 en float) * 255.0 (0x859AAC), _ftol2, 255 - eso como entero sin signo
    // (fild + fadd 2^32 si es negativo) y maximo con 70.0 (0x858CE0): se queda 70 solo si 70 > alpha.
    const int32 scaled = RadarFtol(distance * RadarExeFloat(0x85F0A0) * 255.0f);
    const int32 value  = 255 - scaled;
    float alpha = (float)value;
    if (value < 0) {
        alpha += 4294967296.0f;
    }
    if (70.0f > alpha) {
        alpha = 70.0f;
    }
    return (uint8)(int64)alpha;
}

// Como el original (0x583480): solo toca EAX y ECX (EAX = puntero de entrada, ECX = puntero de salida) y conserva EDX; DrawRadarSection/DrawAreaOnRadar/
// DrawRadarMask y los llamadores 0x575203, 0x57770B.. del EXE lo usan como contador de bucle. El cuerpo esta en TransformRadarPointToScreenSpaceImpl; el hook del EXE
// (RadarTransformScreenExe) reproduce los registros con CarEnterExitEntryEdxCpp/ResidueRegsCpp y el metodo de la clase (llamado desde la DLL) no los modela.
static void __cdecl TransformRadarPointToScreenSpaceImpl(CVector2D* out, const CVector2D* in) {
    if (FrontEndMenuManager.m_bDrawingMap) {
        // 0x583491..0x5834B3: zoom * in.x + origen.x ; origen.y - zoom * in.y
        out->x = FrontEndMenuManager.m_fMapZoom * in->x + FrontEndMenuManager.m_vMapOrigin.x;
        out->y = FrontEndMenuManager.m_vMapOrigin.y - FrontEndMenuManager.m_fMapZoom * in->y;
        return;
    }

    // 0x5834B4..0x583522 (x87 a 24 bits, cada operacion redondea a float)
    const float sx  = RadarStretchBaseX();
    const float s94 = RadarExeFloat(0x866B78) * sx;                         // 94.0 * sx
    const float h1  = RadarExeFloat(0x858B8C) * s94;                        // 0.5 * (94 * sx)
    out->x = (h1 + sx * RadarExeFloat(0x858A10)) + (s94 * in->x) * RadarExeFloat(0x858B8C); // + 40 * sx ; + (94 * sx * in.x) * 0.5

    const float sy   = RadarExeFloat(0x859524) * (float)RsGlobal.maximumHeight;
    const float s76  = RadarExeFloat(0x866B74) * sy;                        // 76.0 * sy
    const float d104 = sy * RadarExeFloat(0x866B70);                        // sy * 104.0
    const float base = ((float)RsGlobal.maximumHeight - d104) + RadarExeFloat(0x858B8C) * s76;
    out->y = base - (s76 * in->y) * RadarExeFloat(0x858B8C);
}

namespace {
// Lo primero: ninguna instruccion anterior escribe EDX (lo vigila contrib/audit_register_residue.py, objeto Radar). /RTC1 y la cookie de /GS pisarian ECX/EDX.
#pragma runtime_checks("", off)
__declspec(safebuffers) CVector2D* __cdecl RadarTransformScreenExe(CVector2D* out, const CVector2D* in) {
    const uint32 entryEdx = uint32(CarEnterExitEntryEdxCpp() >> 32);
    TransformRadarPointToScreenSpaceImpl(out, in);
    // 0x583480 sale con ECX = out ([esp+4]), EAX = in ([esp+8]) y EDX intacto
    return reinterpret_cast<CVector2D*>(ResidueRegsCpp(uint32(reinterpret_cast<uintptr_t>(out)), entryEdx, uint32(reinterpret_cast<uintptr_t>(in))));
}
#pragma runtime_checks("", restore)
}

CVector2D CRadar::TransformRadarPointToScreenSpace(const CVector2D& in) {
    CVector2D out;
    TransformRadarPointToScreenSpaceImpl(&out, &in);
    return out;
}

/*!
 * @brief Transforms a real coordinate to radar coordinate.
 * @addr 0x583530
 */
CVector2D CRadar::TransformRealWorldPointToRadarSpace(const CVector2D& in) {
    // Como el original (0x583537..0x583590): r = 1.0 / m_radarRange (fdiv) y se MULTIPLICA por r; no se divide cada componente.
    const float dx = in.x - vec2DRadarOrigin.x;
    const float dy = in.y - vec2DRadarOrigin.y;
    const float r  = 1.0f / m_radarRange;
    const float rx = r * dx;
    const float ry = r * dy;
    return {
        cachedSin * ry + cachedCos * rx,
        cachedCos * ry - cachedSin * rx
    };
}

/*!
 * @brief Transforms a radar coordinate to real coordinate. (Unused)
 * @addr 0x5835A0
 */
CVector2D CRadar::TransformRadarPointToRealWorldSpace(const CVector2D& in) {
    return CachedRotateCounterclockwise(in) * m_radarRange + vec2DRadarOrigin;
}

/*!
 * @brief Transforms a radar coordinate to texture coordinate. (Inlined, see CRadar::DrawRadarSection)
 * @addr 0x583600
 */
CVector2D CRadar::TransformRealWorldToTexCoordSpace(const CVector2D& in, int32 x, int32 y) {
    // Como el original (DrawRadarSection 0x5863E2..0x586401): se MULTIPLICA por 0x858F44 (0.002f) y -0.002f (0x866B7C); no se divide entre 500.
    const float originX = (float)(500 * x) - 3000.0f;
    const float originY = (float)(500 * ((int32)MAX_RADAR_HEIGHT_TILES - y)) - 3000.0f;
    return CVector2D{
        (in.x - originX) * RadarExeFloat(0x858F44),
        (in.y - originY) * RadarExeFloat(0x866B7C)
    };
}

// 0x583670
void CRadar::CalculateCachedSinCos() {
    if (FrontEndMenuManager.m_bDrawingMap) {
        cachedSin = 0.0f;
        cachedCos = 1.0f;

        return;
    }

    // Como el original: el angulo sale de fpatan (precision completa) y fsin/fcos se calculan SOBRE ese valor sin redondear a float;
    // solo m_fRadarOrientation (fst dword) se redondea. Los ayudantes x87 de X87Math reproducen la secuencia y guardan los tres floats.
    if (TheCamera.GetLookDirection() == LOOKING_DIRECTION_FORWARD) {
        // 0x5837E0..0x583813: TheCamera.m_matrix ? atan2(-forward.x, forward.y) : TheCamera.m_placement.m_fHeading
        const float* forward = TheCamera.m_matrix ? &TheCamera.m_matrix->GetForward().x : nullptr;
        if (forward) {
            RadarSinCosFromAtan2Cpp(forward[0], forward[1], &m_fRadarOrientation, &cachedSin, &cachedCos);
        } else {
            RadarSinCosFromAngleCpp(TheCamera.m_placement.m_fHeading, &m_fRadarOrientation, &cachedSin, &cachedCos);
        }

        return;
    }

    // 0x5836A7..0x583757: modo 1a persona -> forward normalizado del objetivo; si no, posicion del objetivo - fuente antes de mirar atras
    const auto& activeCam    = CCamera::GetActiveCamera();
    const auto  targetEntity = activeCam.m_pCamTargetEntity;

    const auto ForwardOfTarget = [&] {
        CVector forward = targetEntity->GetMatrix().GetForward(); // GetMatrix: AllocateMatrix + UpdateMatrix si no hay matriz (0x5836C5..0x5836DA)
        forward.Normalise();                                      // 0x59C910
        return forward;
    };

    CVector directionToTarget;
    if (activeCam.m_nMode == MODE_1STPERSON) {
        directionToTarget = ForwardOfTarget();
    } else {
        directionToTarget = targetEntity->GetPosition() - activeCam.m_vecSourceBeforeLookBehind;
    }
    // 0x583757..0x5837B3: GetLookDirection() == 0 vuelve a calcular el forward normalizado del objetivo
    if (TheCamera.GetLookDirection() == LOOKING_DIRECTION_UNKNOWN_1) {
        directionToTarget = ForwardOfTarget();
    }

    // 0x5837B5..0x5837D6: fld x; fchs; fld y; fpatan; fsin/fcos sobre el mismo valor sin redondear
    RadarSinCosFromAtan2Cpp(directionToTarget.x, directionToTarget.y, &m_fRadarOrientation, &cachedSin, &cachedCos);
}

/*!
 * @brief Creates a new coordinate blip (i.e. tracing blip with no sprite)
 * @param type Type
 * @param posn Position
 * @param color Color (Unused)
 * @param blipDisplay Display option
 * @param scriptName Script name (Unused) (from Android)
 * @addr 0x583820
 * @returns Handle of the blip created.
 */
tBlipHandle CRadar::SetCoordBlip(eBlipType type, CVector posn, eBlipColour color, eBlipDisplay blipDisplay, const char* scriptName) {
    auto index = FindTraceNotTrackingBlipIndex();
    if (index == -1)
        return -1;

    auto& t = ms_RadarTrace[index];

    t.m_vPosition        = posn;
    t.m_nBlipDisplayFlag = blipDisplay;
    t.m_nBlipType        = type;
    t.m_nColour          = BLIP_COLOUR_DESTINATION;
    t.m_fSphereRadius    = 1.f;
    t.m_nEntityHandle    = 0;
    t.m_nBlipSize        = 1;
    t.m_nBlipSprite      = eRadarSprite::RADAR_SPRITE_NONE;
    // Como el original (0x5838D7 mov byte ptr [eax+0BA8715h],3): el byte de banderas se escribe ENTERO (brillante + seguimiento);
    // short range, amistoso, permanecer, fundido y aspecto de coordenada quedan a cero.
    t.m_bBright               = true;
    t.m_bTrackingBlip         = true;
    t.m_bShortRange           = false;
    t.m_bFriendly             = false;
    t.m_bBlipRemain           = false;
    t.m_bBlipFade             = false;
    t.m_nCoordBlipAppearance  = BLIP_FLAG_FRIEND;
    t.m_pEntryExit       = nullptr;

    return GetNewUniqueBlipIndex(index);
}

/*!
 * @brief Creates a new short-range coordinate blip (i.e. tracing blip with no sprite)
 * @param type Type
 * @param posn Position
 * @param color Color
 * @param blipDisplay Display option
 * @param scriptName Script name (Unused) (from Android)
 * @addr 0x583920
 * @returns Handle of the blip created.
 */
tBlipHandle CRadar::SetShortRangeCoordBlip(eBlipType type, CVector posn, eBlipColour color, eBlipDisplay blipDisplay, const char* scriptName) {
    if (const auto blip = SetCoordBlip(type, posn, color, blipDisplay, scriptName); blip != -1) {
        ms_RadarTrace[GetActualBlipArrayIndex(blip)].m_bShortRange = true;
        return blip;
    }

    return -1;
}

/*!
 * @brief Creates a new coordinate blip (i.e. tracing blip with no sprite) bound to an entity.
 * @param type Type
 * @param entityHandle Opaque entity handle? (CEntity*?)
 * @param arg2 Unknown argument (Unused)
 * @param blipDisplay Display option
 * @addr 0x5839A0
 * @returns Handle of the blip created.
 */
tBlipHandle CRadar::SetEntityBlip(eBlipType type, int32 entityHandle, uint32 arg2, eBlipDisplay blipDisplay) {
    if (const auto index = FindTraceNotTrackingBlipIndex(); index != -1) {
        auto& t = ms_RadarTrace[index];

        t.m_nBlipDisplayFlag = blipDisplay;
        t.m_nColour = (type == BLIP_CHAR || type == BLIP_CAR) ? BLIP_COLOUR_THREAT : BLIP_COLOUR_GREEN;
        t.m_nEntityHandle = entityHandle;
        t.m_fSphereRadius = 1.f;
        t.m_nBlipSize = 1;
        t.m_nBlipDisplayFlag = blipDisplay;
        t.m_nBlipType = type;
        t.m_nBlipSprite = eRadarSprite::RADAR_SPRITE_NONE;
        // Como el original (0x583A60 mov byte ptr [eax+0BA8715h],3): byte de banderas entero = brillante + seguimiento
        t.m_bBright = true;
        t.m_bTrackingBlip = true;
        t.m_bShortRange = false;
        t.m_bFriendly = false;
        t.m_bBlipRemain = false;
        t.m_bBlipFade = false;
        t.m_nCoordBlipAppearance = BLIP_FLAG_FRIEND;
        t.m_pEntryExit = nullptr;

        return GetNewUniqueBlipIndex(index);
    }

    return -1;
}

/*!
 * @brief Change blip color to one of the color of a radar color set.
 * @addr 0x583AB0
 */
void CRadar::ChangeBlipColour(tBlipHandle blip, eBlipColour color) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        ms_RadarTrace[index].m_nColour = color;
    }
}

/*!
 * @brief Checks if the blip is revealed.
 * @param blipIndex Index of the blip in `ms_RadarTrace`
 * @addr 0x583AF0
 * @returns True if it's revealed to the player.
 */
bool CRadar::HasThisBlipBeenRevealed(int32 blipIndex) {
    // Como el original (0x583B0C cmp dword ptr [0BA372Ch],50h / jae): ZonesRevealed >= 80 (sin signo)
    if (!FrontEndMenuManager.m_bDrawingMap
        || !ms_RadarTrace[blipIndex].m_bShortRange
        || uint32(CTheZones::ZonesRevealed) >= 80u
        || CTheZones::GetCurrentZoneLockedOrUnlocked(ms_RadarTrace[blipIndex].m_vPosition)
    ) {
        return true;
    }
    return false;
}

/*!
 * @brief Checks if a blip sprite is allowed to be drawn right now.
 * @param spriteId Blip sprite
 * @param priority Importance of displaying the blip.
 * @addr 0x583B40
 * @returns True if it's allowed to be drawn.
 */
bool CRadar::DisplayThisBlip(eRadarSprite spriteId, int8 priority) {
    if (CGame::CanSeeOutSideFromCurrArea() && FindPlayerPed()->GetAreaCode() == AREA_CODE_NORMAL_WORLD) {
        switch (spriteId) {
        case RADAR_SPRITE_NONE:
        case RADAR_SPRITE_WHITE:
        case RADAR_SPRITE_CENTRE:
        case RADAR_SPRITE_MAP_HERE:
        case RADAR_SPRITE_NORTH:
            return true;

        case RADAR_SPRITE_AIRYARD:
        case RADAR_SPRITE_AMMUGUN:
        case RADAR_SPRITE_BARBERS:
        case RADAR_SPRITE_BOATYARD:
        case RADAR_SPRITE_BURGERSHOT:
        case RADAR_SPRITE_BULLDOZER:
        case RADAR_SPRITE_CHICKEN:
        case RADAR_SPRITE_DINER:
        case RADAR_SPRITE_HOSTPITAL:
        case RADAR_SPRITE_MODGARAGE:
        case RADAR_SPRITE_PIZZA:
        case RADAR_SPRITE_POLICE:
        case RADAR_SPRITE_RACE:
        case RADAR_SPRITE_SAVEGAME:
        case RADAR_SPRITE_SCHOOL:
        case RADAR_SPRITE_TATTOO:
        case RADAR_SPRITE_TSHIRT:
        case RADAR_SPRITE_DATEDISCO:
        case RADAR_SPRITE_DATEDRINK:
        case RADAR_SPRITE_DATEFOOD:
        case RADAR_SPRITE_TRUCK:
        case RADAR_SPRITE_CASH:
        case RADAR_SPRITE_FLAG:
        case RADAR_SPRITE_GYM:
        case RADAR_SPRITE_IMPOUND:
        case RADAR_SPRITE_SPRAY:
            return (FrontEndMenuManager.m_ShowLocationsBlips && priority < 0) || priority == 1;

        case RADAR_SPRITE_BIGSMOKE:
        case RADAR_SPRITE_CATALINAPINK:
        case RADAR_SPRITE_CESARVIAPANDO:
        case RADAR_SPRITE_CJ:
        case RADAR_SPRITE_CRASH1:
        case RADAR_SPRITE_EMMETGUN:
        case RADAR_SPRITE_GIRLFRIEND:
        case RADAR_SPRITE_LOGOSYNDICATE:
        case RADAR_SPRITE_MADDOG:
        case RADAR_SPRITE_MAFIACASINO:
        case RADAR_SPRITE_MCSTRAP:
        case RADAR_SPRITE_OGLOC:
        case RADAR_SPRITE_RYDER:
        case RADAR_SPRITE_QMARK:
        case RADAR_SPRITE_SWEET:
        case RADAR_SPRITE_THETRUTH:
        case RADAR_SPRITE_TORENORANCH:
        case RADAR_SPRITE_TRIADS:
        case RADAR_SPRITE_TRIADSCASINO:
        case RADAR_SPRITE_WOOZIE:
        case RADAR_SPRITE_ZERO:
        case RADAR_SPRITE_GANGB:
        case RADAR_SPRITE_GANGP:
        case RADAR_SPRITE_GANGY:
        case RADAR_SPRITE_GANGN:
        case RADAR_SPRITE_GANGG:
            return (FrontEndMenuManager.m_ShowContactsBlips && priority < 0) || priority == 3;

        default:
            return (FrontEndMenuManager.m_ShowOtherBlips && priority < 0) || priority == 2;
        }
    }

    switch (spriteId) {
    case RADAR_SPRITE_NONE:
    case RADAR_SPRITE_WHITE:
    case RADAR_SPRITE_CENTRE:
    case RADAR_SPRITE_MAP_HERE:
    case RADAR_SPRITE_NORTH:
    case RADAR_SPRITE_MAFIACASINO:
    case RADAR_SPRITE_SCHOOL:
    case RADAR_SPRITE_WAYPOINT:
    case RADAR_SPRITE_TRIADSCASINO:
    case RADAR_SPRITE_CASH:
        return true;

    default:
        return false;
    }
}

/*!
 * @brief Changes the brightness of a blip. (Unused)
 * @param blip Blip handle
 * @param brightness Brightness flag. (=1: bright, ≠1: not bright)
 * @addr 0x583C70
 */
void CRadar::ChangeBlipBrightness(tBlipHandle blip, int32 brightness) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        // Como el original (0x583CA5..0x583CBA): brightness == 1 BORRA el bit (and byte,0FEh); cualquier otro valor lo pone (or byte,cl).
        ms_RadarTrace[index].m_bBright = brightness != 1;
    }
}

/*!
 * @brief Changes the size of a blip.
 * @brief This function does not effect the blip size if the map is being drawn.
 * @addr 0x583CC0
 */
void CRadar::ChangeBlipScale(tBlipHandle blip, int32 size) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        ms_RadarTrace[index].m_nBlipSize = FrontEndMenuManager.m_bDrawingMap ? 1 : size;
    }
}

/*!
 * @brief Changes the display option of a blip.
 * @addr 0x583D20
 */
void CRadar::ChangeBlipDisplay(tBlipHandle blip, eBlipDisplay blipDisplay) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        ms_RadarTrace[index].m_nBlipDisplayFlag = blipDisplay;
    }
}

/*!
 * @brief Changes the sprite of a blip.
 * @param blip Blip
 * @param spriteId Blip sprite
 * @addr 0x583D70
 */
void CRadar::SetBlipSprite(tBlipHandle blip, eRadarSprite spriteId) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1 && ms_RadarTrace[index].m_bTrackingBlip) {
        ms_RadarTrace[index].m_nBlipSprite = spriteId;
    }
}

/*!
 * @brief Sets a blip to shown always in zoom.
 * @addr 0x583DB0
 */
void CRadar::SetBlipAlwaysDisplayInZoom(tBlipHandle blip, bool display) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1 && ms_RadarTrace[index].m_bTrackingBlip) {
        ms_RadarTrace[index].m_bBlipRemain = display;
    }
}

/*!
 * @brief Sets a blip to fade. (Unused?)
 * @addr 0x583E00
 */
void CRadar::SetBlipFade(tBlipHandle blip, bool fade) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        ms_RadarTrace[index].m_bBlipFade = fade;
    }
}

/*!
 * @brief Changes the appearnace of a blip.
 * @addr 0x583E50
 */
void CRadar::SetCoordBlipAppearance(tBlipHandle blip, eBlipAppearance appearance) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        auto& trace = ms_RadarTrace[index];
        // Como el original (0x583E83..0x583EA2): solo blips de tipo COORD (cmp cl,10h = 4 << 2), no CAR; escribe los bits 6-7 del PRIMER byte de
        // banderas (0x583EA2 mov [eax+0BA8715h],dl), es decir m_nCoordBlipAppearance, y solo si appearance <= 2 (ja).
        if (trace.m_nBlipType != eBlipType::BLIP_COORD) {
            return;
        }

        if (uint8(appearance) <= 2) {
            trace.m_nCoordBlipAppearance = uint8(appearance);
        }
    }
}

/*!
 * @brief Sets a blip as friendly.
 * @addr 0x583EB0
 */
void CRadar::SetBlipFriendly(tBlipHandle blip, bool friendly) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        ms_RadarTrace[index].m_bFriendly = friendly;
    }
}

/*!
 * @brief Sets an entry point to a blip.
 * @addr 0x583F00
 */
void CRadar::SetBlipEntryExit(tBlipHandle blip, CEntryExit* enex) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        auto& trace = ms_RadarTrace[index];
        if (trace.m_bTrackingBlip) {
            trace.m_pEntryExit = enex;
        }
    }
}

/*!
 * @brief Limits stretched X, Y values to the map
 * @brief This function does nothing if the map is not being drawn.
 * @brief This code piece seems fairly common.. Perhaps its inlined?
 */
void Limit(float& x, float& y) {
    if (FrontEndMenuManager.m_bDrawingMap) {
        // Como el original (0x583F4C..0x583F7E): x = (ancho * 0x859520) * x ; y = (alto * 0x859524) * y y despues LimitToMap
        x = RadarStretchBaseX() * x;
        y = RadarStretchBaseY() * y;

        CRadar::LimitToMap(x, y);
    }
}

// Como el original: CRect con campos escritos directamente (+0 izquierda, +4 Y mayor, +8 derecha, +0xC Y menor); el constructor de la DLL
// hace assert(!IsFlipped()) y los argumentos son (izquierda, +0xC, derecha, +4).
static CRect RadarRawRect(float left, float plus4, float right, float plusC) {
    CRect result;
    result.left = left; result.top = plus4; result.right = right; result.bottom = plusC;
    return result;
}

/*!
 * @brief Draws a square coordinate blip to the map.
 * @addr 0x583F40
 */
void CRadar::ShowRadarTrace(float x, float y, uint32 size, uint32 r, uint32 g, uint32 b, uint32 a) {
    // Como el original: el EXE recibe el color como 4 enteros (r, g, b, a) en la pila, no como un CRGBA de 4 bytes (0x583F8C.. [esp+38h] = alfa).
    Limit(x, y);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));

    // 0x583F94..0x583FA3: float(size) como entero sin signo (fild + fadd 2^32 si es negativo)
    const float sizeF = (float)size;

    // Primer rectangulo (0x583FA9..0x584011): borde negro 1 pixel mas grande que el cuadro por TODOS los lados
    //   izquierda = (x - size) - 1 ; +0xC = (y - size) - 1 ; derecha = (x + size) + 1 ; +4 = (y + size) + 1
    const float bx0 = (x - sizeF) - 1.0f, by0 = (y - sizeF) - 1.0f;
    const float bx1 = (sizeF + x) + 1.0f, by1 = (sizeF + y) + 1.0f;
    CSprite2d::DrawRect(RadarRawRect(bx0, by1, bx1, by0), CRGBA{ 0, 0, 0, uint8(a) });

    // Segundo rectangulo (0x584016..0x58405D): el cuadro con el color pedido
    CSprite2d::DrawRect(RadarRawRect(x - sizeF, sizeF + y, sizeF + x, y - sizeF), CRGBA{ uint8(r), uint8(g), uint8(b), uint8(a) });
}

/*!
 * @brief Draws a coordinate blip to the map with height information.
 * @addr 0x584070
 */
void CRadar::ShowRadarTraceWithHeight(float x, float y, uint32 size, uint32 R, uint32 G, uint32 B, uint32 A, eRadarTraceHeight height) {
    Limit(x, y);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));

    // NOTE: Those RGBA parameters are 4-bytes per channel.
    // then we construct 1-byte per channel CRGBA. It's stupid isn't it?
    // NOTE: {R,G,B,A} > 255 is possible, so we can't assert for x <= UCHAR_MAX.
    const auto r = static_cast<uint8>(R), g = static_cast<uint8>(G), b = static_cast<uint8>(B), a = static_cast<uint8>(A);

    // Como el original (0x584075..0x584479): cada medida es (ancho * 0x859520) * n o (alto * 0x859524) * n (x87 a 24 bits), n = float(size + k) sin signo
    const float sx = RadarStretchBaseX(), sy = RadarStretchBaseY();
    const float n0 = (float)(size + 0), n1 = (float)(size + 1), n2 = (float)(size + 2), n3 = (float)(size + 3);

    switch (height) {
    case RADAR_TRACE_HIGH: { // 0x5841F8..0x584336
        const float yTop = y - n2 * sy;  // fsubr [esp+54h]: y - size2 * sy
        const float yApex = n3 * sy + y; // fadd y
        const float xs3 = sx * n3;
        CSprite2d::Draw2DPolygon( // draw black border
            x,       yApex,
            x,       yApex,
            xs3 + x, yTop,
            x - xs3, yTop,
            { 0, 0, 0, a }
        );
        const float yTop1 = y - sy * n1;
        const float yApex1 = sy * n1 + y;
        const float xs1 = sx * n1;
        CSprite2d::Draw2DPolygon( // draw triangle
            x,       yApex1,
            x,       yApex1,
            xs1 + x, yTop1,
            x - xs1, yTop1,
            { r, g, b, a }
        );
        break;
    }
    case RADAR_TRACE_NORMAL: { // 0x5840E5..0x5841F7
        const float xs1 = sx * n1, ys1 = sy * n1;
        CSprite2d::DrawRect( // draw black border
            RadarRawRect(x - xs1, ys1 + y, xs1 + x, y - ys1),
            { 0, 0, 0, a }
        );

        const float xs0 = sx * n0, ys0 = sy * n0;
        CSprite2d::DrawRect( // draw box
            RadarRawRect(x - xs0, ys0 + y, xs0 + x, y - ys0),
            { r, g, b, a }
        );
        break;
    }
    case RADAR_TRACE_LOW: { // 0x584337..0x58447F
        const float yTop = y - n3 * sy;  // fsubr [esp+54h]: y - size3 * sy
        const float yBase = n2 * sy + y; // fadd y
        const float xs3 = sx * n3;
        CSprite2d::Draw2DPolygon( // draw black border
            xs3 + x, yBase,
            x - xs3, yBase,
            x,       yTop,
            x,       yTop,
            { 0, 0, 0, a }
        );

        const float yTop1 = y - sy * n1;
        const float yBase1 = sy * n1 + y;
        const float xs1 = sx * n1;
        CSprite2d::Draw2DPolygon( // draw triangle
            xs1 + x, yBase1,
            x - xs1, yBase1,
            x,       yTop1,
            x,       yTop1,
            { r, g, b, a }
        );
        break;
    }
    }
}

// show debug line at this position
// 0x584480
void CRadar::ShowRadarMarker(CVector p, uint32 color, float radius) {
    auto& mat   = TheCamera.m_mCameraMatrix;
    auto& up    = mat.GetUp();
    auto& right = mat.GetRight();

    auto r0 = radius * 0.5f;
    auto r1 = radius * 1.4f;

    CTheScripts::ScriptDebugLine3D(p + r1 * up,    p + r0 * up,    color, color);
    CTheScripts::ScriptDebugLine3D(p - r1 * up,    p - r0 * up,    color, color);
    CTheScripts::ScriptDebugLine3D(p + r1 * right, p + r0 * right, color, color);
    CTheScripts::ScriptDebugLine3D(p - r1 * right, p - r0 * right, color, color);
}

/*!
 * @brief Get game color from the blip color.
 * @addr 0x584770
 * @returns Game color
 */
uint32 CRadar::GetRadarTraceColour(eBlipColour color, bool bright, bool friendly) {
    switch (color) {
    case BLIP_COLOUR_RED:
    case BLIP_COLOUR_REDCOPY:
        return HudColour.GetIntColour(bright ? HUD_COLOUR_RED : HUD_COLOUR_DARK_RED);
    case BLIP_COLOUR_GREEN:
        return HudColour.GetIntColour(bright ? HUD_COLOUR_GREEN : HUD_COLOUR_DARK_GREEN);
    case BLIP_COLOUR_BLUE:
    case BLIP_COLOUR_BLUECOPY:
        return HudColour.GetIntColour(bright ? HUD_COLOUR_LIGHT_BLUE : HUD_COLOUR_BLUE);
    case BLIP_COLOUR_WHITE:
        return HudColour.GetIntColour(bright ? HUD_COLOUR_LIGHT_GRAY : HUD_COLOUR_DARK_GRAY);
    case BLIP_COLOUR_YELLOW:
        return HudColour.GetIntColour(bright ? HUD_COLOUR_CREAM : HUD_COLOUR_GOLD);
    case BLIP_COLOUR_THREAT:
        return HudColour.GetIntColour(friendly ? HUD_COLOUR_BLUE : HUD_COLOUR_RED);
    case BLIP_COLOUR_DESTINATION:
        return HudColour.GetIntColour(HUD_COLOUR_CREAM);
    default:
        return color;
    }
}

/*
 * @brief Draw a sprite to that is rotated.
 * @addr 0x584850
 */
void CRadar::DrawRotatingRadarSprite(CSprite2d& sprite, float x, float y, float angle, uint32 width, uint32 height, CRGBA color) {
    Limit(x, y);

    // Como el original (0x584896..0x584916): angulo = (i * 1.5707964 [0x858FE4]) + (angulo - 0.7853982 [0x859AB0]) redondeado a float; sin/cos x87 sin
    // redondear; x = (0.0 [0x858B50] * cos + sin) * ancho + x ; y = (cos - 0.0 * sin) * alto + y. width/height se leen como enteros sin signo (fild + fadd 2^32).
    // width/height se pasan como enteros (el ayudante reproduce fild + fadd 2^32 sin redondear antes de multiplicar).
    CVector2D verts[4];
    RadarRotatedSpriteVertsCpp(angle, x, y, width, height, RadarExeFloat(0x859AB0), RadarExeFloat(0x858FE4), RadarExeFloat(0x858B50), &verts[0].x);

    sprite.Draw(verts[3].x, verts[3].y, verts[2].x, verts[2].y, verts[0].x, verts[0].y, verts[1].x, verts[1].y, color);
}

// 0x584960
void CRadar::DrawYouAreHereSprite(float x, float y) {
    static auto& mapYouAreHereTimer = StaticRef<uint32>(0xBAA358);
    static auto& mapYouAreHereDisplay = StaticRef<bool>(0x8D0930);

    // Como el original (0x584960..0x5849AA): visible -> se apaga cuando pasan mas de 500 ms; apagado -> se enciende cuando pasan mas de 200 ms
    if (mapYouAreHereDisplay) {
        if (CTimer::GetTimeInMSPauseMode() - mapYouAreHereTimer > 500) {
            mapYouAreHereTimer = CTimer::GetTimeInMSPauseMode();
            mapYouAreHereDisplay = false;
        }
    } else if (CTimer::GetTimeInMSPauseMode() - mapYouAreHereTimer > 200) {
        mapYouAreHereTimer = CTimer::GetTimeInMSPauseMode();
        mapYouAreHereDisplay = true;
    }

    if (mapYouAreHereDisplay) {
        // 0x5849B6..0x584A57: angulo = rumbo + 3.1415927 [0x858CB8]; angulo + 1.5707964 [0x858FE4]; sin/cos x87 de ese valor ya redondeado a float;
        // el desplazamiento es 17.0 [0x85F100] * sin / cos y el tamano (ancho * 0x859520 * 25.0 [0x858FE8]) truncado con _ftol2
        const float angle = FindPlayerHeading(0) + RadarExeFloat(0x858CB8);
        const float circleAngle = angle + RadarExeFloat(0x858FE4);
        const float radius = RadarExeFloat(0x85F100);
        float offsetX = 0.0f, offsetY = 0.0f;
        RadarYouAreHereOffsetCpp(circleAngle, radius, x, y, &offsetX, &offsetY);
        const float scale25 = RadarExeFloat(0x858FE8);
        DrawRotatingRadarSprite(
            RadarBlipSprites[RADAR_SPRITE_MAP_HERE],
            offsetX,
            offsetY,
            angle,
            (uint32)RadarFtol(RadarStretchBaseX() * scale25),
            (uint32)RadarFtol(RadarStretchBaseY() * scale25),
            { 255, 255, 255, 255 }
        );
    }

    MapLegendList.data()[MapLegendCounter] = RADAR_SPRITE_MAP_HERE;
    MapLegendCounter++;
}

// 0x584A80
void CRadar::SetupRadarRect(int32 x, int32 y) {
    m_radarRect.left   = float(500 * (x - 7));
    m_radarRect.top    = float(500 * (7 - y));
    m_radarRect.right  = float(500 * (x - 4));
    m_radarRect.bottom = float(500 * (4 - y));
}

// unused
// 0x584B00
// Returns true if either coords had to be clipped
bool ClipRadarTileCoords(int32& x, int32& y) {
    // Not quite the way they've done it, but nicer.

    const auto ox = x, oy = y;

    x = std::clamp(x, 0, (int32)MAX_RADAR_WIDTH_TILES - 1);
    y = std::clamp(y, 0, (int32)MAX_RADAR_HEIGHT_TILES - 1);

    return ox != x || oy != y;
}

// 0x584B50
void CRadar::RequestMapSection(int32 x, int32 y) {
    if (!IsMapSectionInBounds(x, y))
        return;

    if (const auto texture = gRadarTextures[y][x]; texture != -1) {
        CStreaming::RequestTxdModel(texture, STREAMING_GAME_REQUIRED | STREAMING_KEEP_IN_MEMORY);
    }
}

// 0x584BB0
void CRadar::RemoveMapSection(int32 x, int32 y) {
    if (!IsMapSectionInBounds(x, y))
        return;

    if (const auto texture = gRadarTextures[y][x]; texture != -1) {
        CStreaming::RemoveTxdModel(texture);
    }
}

// 0x584BF0
void CRadar::RemoveRadarSections() {
    // Como el original (0x584BF0..0x584C48): el bucle externo recorre x y el interno y; solo si la ranura de TXD no es -1 llama a
    // CStreaming::RemoveModel(txd + 0x4E20) (4089A0), no a RemoveTxdModel.
    for (auto x = 0u; x < MAX_RADAR_WIDTH_TILES; x++) {
        for (auto y = 0u; y < MAX_RADAR_HEIGHT_TILES; y++) {
            if (const auto texture = gRadarTextures[y][x]; texture != -1) {
                CStreaming::RemoveModel(TXDToModelId(texture));
            }
        }
    }
}

/*
 * @brief Checks if a 2D point is inside the radar. (i.e. a rect that spans -1 to 1 in both directions)
 * @addr 0x584D40
 */
bool IsPointInsideRadar(const CVector2D& point) {
    // Como el original (0x584D40..0x584D82): fcomp con -1.0 (test ah,5 / jnp) y con 1.0 (test ah,41h / je): falla solo si x < -1 o x > 1
    // (ordenados); el limite es inclusivo y un NaN pasa.
    return !(point.x < -1.0f) && !(point.x > 1.0f)
        && !(point.y < -1.0f) && !(point.y > 1.0f);
}

// 0x584D90
void GetTextureCorners(int32 x, int32 y, CVector2D* corners) {
    // Magic numbers probably come from `MAX_RADAR_WIDTH_TILES` and `MAX_RADAR_HEIGHT_TILES`

    corners[0] = {500.0f * float(x - 6), 500.0f * float(5 - y)};
    corners[1] = {500.0f * float(x - 5), 500.0f * float(5 - y)};
    corners[2] = {500.0f * float(x - 5), 500.0f * float(6 - y)};
    corners[3] = {500.0f * float(x - 6), 500.0f * float(6 - y)};
}

// Returns number of intersections
// 0x584E00
int32 LineRadarBoxCollision(CVector2D& result, const CVector2D& lineStart, const CVector2D& lineEnd) {
    auto closestIntersectionParam = 1.0f;
    auto intersectionSide         = -1;

    const auto deltaY = lineEnd.y - lineStart.y;
    const auto deltaX = lineEnd.x - lineStart.x;

    const auto CheckIntersection = [&](float edgeCoord, bool isX, float startDist, float endDist, int side) {
        if (startDist * endDist < 0.0f) {
            float intersectionParam = startDist / (startDist - endDist);
            float intersectionVal   = (isX ? deltaY : deltaX) * intersectionParam + (isX ? lineStart.y : lineStart.x);
            // Como el original (0x584E4C..0x584E86): solo comprueba val >= -1, val <= 1 y t <= el mejor hasta ahora (no hay comprobacion de t >= 0)
            if (intersectionVal >= -1.0f && intersectionVal <= 1.0f && intersectionParam <= closestIntersectionParam) {
                closestIntersectionParam = intersectionParam;
                if (isX) {
                    result.x = edgeCoord;
                    result.y = intersectionVal;
                } else {
                    result.x = intersectionVal;
                    result.y = edgeCoord;
                }
                intersectionSide = side;
                // Como el original: sin retorno anticipado cuando t == 0 (sigue con el resto de lados)
            }
        }
        return false; // No early return
    };

    if (CheckIntersection(-1.0f, true, -1.0f - lineStart.x, -1.0f - lineEnd.x, 3)) {
        return 3;
    }
    if (CheckIntersection(1.0f, true, lineStart.x - 1.0f, lineEnd.x - 1.0f, 1)) {
        return 1;
    }
    if (CheckIntersection(-1.0f, false, -1.0f - lineStart.y, -1.0f - lineEnd.y, 0)) {
        return 0;
    }
    if (CheckIntersection(1.0f, false, lineStart.y - 1.0f, lineEnd.y - 1.0f, 2)) {
        return 2;
    }

    return intersectionSide;
}

// 0x585040
// Recorte del cuadrilatero `in` contra el cuadrado del radar [-1, 1] (transcripcion de 0x585040..0x5853C9; la salida no tiene limite de capacidad).
int32 CRadar::ClipRadarPoly(CVector2D* out, const CVector2D* in) {
    constexpr uint32 ONE = 0x3F800000u, MINUS_ONE = 0xBF800000u;
    // Esquinas del cuadrado en el orden del EXE (0x585054..0x585099: [esp+38h..54h]) precedidas de las dos palabras de pila que lee el EXE con
    // lastSide == -1 ([esp+30h] = direccion del arreglo `inside` - 1 y [esp+34h] = &out[n]); esa ruta (ningun lado intersecado) lee basura en el EXE.
    // [esp+30h] es una direccion de pila ABSOLUTA del marco del EXE (0x5851BF..0x5851D2: lea edx,[esp+14h]; dec edx), no la de un local de la DLL (el marco de la DLL
    // es otro y la salida del EXE la deja en out[]): se reconstruye desde el esp de entrada A (= direccion del retorno): push ebp (ebp = A-4); and esp,-8; sub esp,4Ch;
    // push ebx/esi/edi => esp = ((A-4) & ~7) - 0x58 e `inside` = esp + 0x14, de modo que [esp+30h] = ((A-4) & ~7) - 0x58 + 0x13.
    const uint32 exeInsideMinusOne = uint32(((reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) - 4) & ~uintptr_t(7)) - 0x45);
    uint32 frame[10] = { 0, 0, ONE, MINUS_ONE, ONE, ONE, MINUS_ONE, ONE, MINUS_ONE, MINUS_ONE };

    bool inside[4];
    for (auto k = 0; k < 4; k++) {
        inside[k] = IsPointInsideRadar(in[k]); // fcomp -1.0 / 1.0 ordenados: un NaN cuenta como dentro
    }

    int32 count    = 0;  // [esp+10h]
    int32 lastSide = -1; // [esp+18h]
    for (int32 i = 0; i < 4; i++) {
        if (inside[i]) {
            memcpy(&out[count], &in[i], sizeof(CVector2D)); // 0x5851F7: copia entera de dwords
            count++;
            continue;
        }

        const int32 prev = (i - 1) & 3;
        const int32 next = (i + 1) & 3;

        // Del vertice exterior hacia el anterior y hacia el siguiente
        const int32 sideBack = LineRadarBoxCollision(out[count], in[i], in[prev]);
        if (sideBack != -1) {
            count++;
            lastSide = sideBack;
        }
        const int32 sideForward = LineRadarBoxCollision(out[count], in[i], in[next]);
        if (sideForward == -1) {
            continue;
        }
        if (sideBack != -1) {
            count++; // 0x585307
            continue;
        }

        // Solo salida hacia el siguiente: si no hay lado previo, se busca el ultimo lado intersecado de 3 a i (0x585278..0x5852B1)
        if (lastSide == -1) {
            CVector2D scratch;
            for (int32 e = 3; e >= i; e--) {
                const int32 side = LineRadarBoxCollision(scratch, in[e], in[e == 0 ? 3 : e - 1]);
                if (side != -1) {
                    lastSide = side;
                    break;
                }
            }
        }

        // 0x5852BD..0x5852C3: el punto de interseccion se carga (x87) ANTES de escribir las esquinas, que pueden pisarlo
        const float savedX = out[count].x;
        const float savedY = out[count].y;
        if (lastSide != sideForward) {
            frame[0] = exeInsideMinusOne;
            frame[1] = uint32(reinterpret_cast<uintptr_t>(&out[count]));
            // Esquinas desde lastSide hasta sideForward (exclusive), siempre al menos una (do-while en 0x5852D0..0x5852F5)
            int32 corner = lastSide;
            do {
                memcpy(&out[count], &frame[2 + 2 * corner], sizeof(CVector2D));
                count++;
                corner = (corner + 1) & 3;
            } while (corner != sideForward);
        }
        out[count].x = savedX; // 0x5852FA..0x585303
        out[count].y = savedY;
        count++;
    }

    if (count != 0) {
        return count;
    }

    // Ningun vertice dentro ni lado intersecado: si el cuadrado queda dentro del cuadrilatero se devuelve el cuadrado (0x585333..0x5853B7)
    if (!RadarClipHullInsideCpp(&in[0].x)) {
        return 0;
    }
    memcpy(out, &frame[2], 4 * sizeof(CVector2D));
    return 4;
}

// 0x5853D0
void CRadar::DrawAreaOnRadar(const CRect& rect, const CRGBA& color, bool inMenu) {
    // Como el original (0x5853E2..0x58542B): fuera del menu, el area no se dibuja si no corta el rectangulo del radar. Las comparaciones usan los campos
    // en bruto (+0 izquierda, +4, +8 derecha, +0xC) con el mismo sentido que el EXE (IsRectInside tiene los ejes Y intercambiados) y NaN no devuelve.
    if (!inMenu) {
        if (rect.left > m_radarRect.right)   return; // fcomp [8D0928]; test ah,41h; je
        if (rect.right < m_radarRect.left)   return; // fcomp [8D0920]; test ah,5; jnp
        if (rect.bottom > m_radarRect.top)   return; // [rect+0Ch] > [8D0924]
        if (rect.top < m_radarRect.bottom)   return; // [rect+4] < [8D092C]
    }

    // Corner positions - Not transformed
    const CVector2D rectCorners[]{
        { rect.left,  rect.bottom},
        { rect.right, rect.bottom},
        { rect.right, rect.top   },
        { rect.left,  rect.top   }
    };

    // Corner positions - Transformed, not yet clipped
    CVector2D polyUnclippedVertices[4];
    rng::transform(rectCorners, polyUnclippedVertices, [](auto&& v) {
        return TransformRealWorldPointToRadarSpace(v);
    });

    CVector2D polyVertices[8];
    const auto numVerts = ClipRadarPoly(polyVertices, polyUnclippedVertices);
    if (!numVerts) {
        return;
    }

    CVector2D texCoords[8]{};
    // Como el original (0x58564A..0x58568E): ancho * 0x859520 y alto * 0x859524 (SCREEN_STRETCH_X(1.0f) divide entre 640 y redondea distinto)
    const auto scaleX = RadarStretchBaseX();
    const auto scaleY = RadarStretchBaseY();
    for (auto i = 0; i < numVerts; i++) {
        texCoords[i] = TransformRadarPointToScreenSpace(polyVertices[i]);
        if (FrontEndMenuManager.m_bDrawingMap) {
            texCoords[i] *= CVector2D(scaleX, scaleY);
            polyVertices[i] *= CVector2D(scaleX, scaleY);
        }
    }
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
    CSprite2d::SetVertices(numVerts, texCoords, polyVertices, color);
    if (numVerts > 2) {
        RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::GetVertices(), numVerts);
    }
}

// 0x585700
void CRadar::DrawRadarMask() {
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,         RWRSTATE(rwSHADEMODEFLAT));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, RWRSTATE(rwALPHATESTFUNCTIONALWAYS));

    // Como el original (0x585715..0x58573D): esquinas (-1, 1), (1, 1), (1, -1), (-1, -1) (el codigo anterior las tenia en otro orden y con otros signos)
    const CVector2D corners[4] = {
        { -1.0f,  1.0f },
        {  1.0f,  1.0f },
        {  1.0f, -1.0f },
        { -1.0f, -1.0f }
    };

    // Como el original (0x5857D0..0x585813): primero el cuadrado completo del radar (las 4 esquinas transformadas) a la profundidad lejana del dispositivo
    // (RwEngineInstance +0x1C = zBufferFar) con un abanico de 4 vertices.
    {
        CVector2D square[4];
        for (auto i = 0; i < 4; i++) {
            square[i] = TransformRadarPointToScreenSpace(corners[i]);
        }
        CSprite2d::SetMaskVertices(4, square, RwEngineInstance->dOpenDevice.zBufferFar);
        RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::GetVertices(), 4);
    }

    CVector2D out[8]{};
    CVector2D in;

    // Draw the shape we want to mask out from the radar in four segments
    for (auto& corner : corners) {
        // First point is always the corner itself
        in.x = corner.x;
        in.y = corner.y;
        out[0] = TransformRadarPointToScreenSpace(in);

        // Then generate a quarter of the circle: angulo = j * 0.2617994 [0x858F1C] (float); fcos/fsin x87 sin redondear y cada uno se MULTIPLICA por la
        // coordenada de la esquina antes de guardarse como float (0x585860..0x58586C)
        const float step = RadarExeFloat(0x858F1C);
        for (auto j = 0; j < 7; j++) {
            RadarArcPointCpp(j, step, corner.x, corner.y, &in.x, &in.y);
            out[j + 1] = TransformRadarPointToScreenSpace(in);
        };

        CSprite2d::SetMaskVertices(std::size(out), out, CSprite2d::GetNearScreenZ());
        RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::GetVertices(), std::size(out));
    }

    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, RWRSTATE(rwALPHATESTFUNCTIONGREATER));
}

// 0x5858D0
void CRadar::StreamRadarSections(const CVector& worldPosn) {
    // Como el original (0x5858D0..0x58593B): (coord + 3000.0) * 0x858F44 (0.002f, se multiplica; no se divide entre 500) y floor/ceil sobre el float
    const float scale = RadarExeFloat(0x858F44);
    const int32 tileX = (int32)(int64)std::floor((worldPosn.x + 3000.0f) * scale);
    const int32 tileY = (int32)(int64)std::ceil(11.0f - (worldPosn.y + 3000.0f) * scale);
    if (!CStreaming::ms_disableStreaming) {
        StreamRadarSections(tileX, tileY);
    }
}

// 0x584C50
void CRadar::StreamRadarSections(int32 x, int32 y) {
    // Como el original (0x584C50..0x584D37): el bucle externo recorre x y el interno y (el orden de las peticiones/retiradas importa)
    for (auto curX = 0; curX < (int32)MAX_RADAR_WIDTH_TILES; curX++) {
        for (auto curY = 0; curY < (int32)MAX_RADAR_HEIGHT_TILES; curY++) {
            const auto texture = gRadarTextures[curY][curX];
            if (texture == -1) {
                continue;
            }
            if (curY >= y - 1 && curY <= y + 1 && curX >= x - 1 && curX <= x + 1) {
                CStreaming::RequestModel(TXDToModelId(texture), STREAMING_KEEP_IN_MEMORY | STREAMING_GAME_REQUIRED);
            } else {
                CStreaming::RemoveModel(TXDToModelId(texture));
            }
        }
    }
}

// 0x585960
void CRadar::InitFrontEndMap() {
    CalculateCachedSinCos();
    rng::fill(MapLegendList, RADAR_SPRITE_NONE);

    vec2DRadarOrigin.Set(0.0f, 0.0f);
    m_radarRange = WORLD_BOUND_RANGE - 10.0f;
    MapLegendCounter = 0;

    rng::fill(ArrowBlipColour, CRGBA(0, 0, 0, 0));
}

// 0x5859F0
void CRadar::AddBlipToLegendList(bool noSprite, int32 blipIndex) {
    if (!FrontEndMenuManager.m_bDrawingMap)
        return;

    auto& trace = ms_RadarTrace[blipIndex];

    const auto sprite = [&] {
        if (!noSprite)
            return eRadarSprite(blipIndex);

        switch (trace.m_nBlipType) {
        case BLIP_CAR:
        case BLIP_CHAR:
            return trace.m_bFriendly ? RADAR_SPRITE_FRIEND : RADAR_SPRITE_THREAT;
        case BLIP_OBJECT:
            return RADAR_SPRITE_OBJECT;
        case BLIP_COORD:
            return RADAR_SPRITE_DESTINATION;
        default:
            return RADAR_SPRITE_PLAYER_INTEREST;
        }
    }();

    // check if it's not already added.
    if (!notsa::contains(MapLegendList, sprite)) {
        // Como el original (0x585A87..0x585A8D): el contador no se comprueba contra el tamano de la lista
        MapLegendList.data()[MapLegendCounter++] = sprite;

        if (noSprite) {
            const CRGBA color = GetRadarTraceColour(trace.m_nColour, trace.m_bBright, trace.m_bFriendly);

            ArrowBlipColour.data()[-sprite].Set(color, 255);
        }
    }
}

// 0x585B20
void CRadar::SetMapCentreToPlayerCoords() {
    if (FindPlayerPed() == nullptr)
        return;

    FrontEndMenuManager.m_bDrawingMap = true;

    InitFrontEndMap();

    CVector2D posReal = FindPlayerCentreOfWorldForMap(0);

    if (CTheScripts::HideAllFrontEndMapBlips || CTheScripts::bPlayerIsOffTheMap)
        posReal.Set(0.0f, 0.0f);

    auto posRadar = TransformRealWorldPointToRadarSpace(posReal);
    LimitRadarPoint(posRadar);

    FrontEndMenuManager.m_vMousePos = posReal;
    FrontEndMenuManager.m_vMapOrigin.x = DEFAULT_SCREEN_WIDTH  / 2.0f - FrontEndMenuManager.m_fMapZoom * posRadar.x;
    FrontEndMenuManager.m_vMapOrigin.y = DEFAULT_SCREEN_HEIGHT / 2.0f + FrontEndMenuManager.m_fMapZoom * posRadar.y;
    FrontEndMenuManager.m_bDrawingMap = false;
}

// 0x585BF0
void CRadar::Draw3dMarkers() {
    const auto PutMarkerCone = [](uint32 id, CVector pos, float size, CRGBA color) {
        C3dMarkers::PlaceMarkerCone(id, pos, size, color.r, color.g, color.b, 255, 1024u, 0.2f, 5, true);
    };

    for (auto&& [i, trace] : rngv::enumerate(ms_RadarTrace)) {
        if (!trace.m_bTrackingBlip) {
            continue;
        }

        const auto color = CRGBA{GetRadarTraceColour(trace.m_nColour, trace.m_bBright, trace.m_bFriendly)};
        // TODO: make a tConeHandle or something for this.
        const auto coneHandle = (uint32)i | ((uint32)trace.m_nCounter << (sizeof(uint16) * 8));

        // Como el original (0x585C4E..0x585C6B): en CONTACT_POINT IsPlayerOnAMission se llama ANTES de comprobar la pantalla del blip;
        // en el resto de tipos la comprobacion de pantalla va dentro de cada caso (sin efectos laterales).
        const bool displayOk = trace.m_nBlipDisplayFlag == BLIP_DISPLAY_BOTH || trace.m_nBlipDisplayFlag == BLIP_DISPLAY_MARKERONLY;
        if (trace.m_nBlipType == BLIP_CONTACT_POINT) {
            if (CTheScripts::IsPlayerOnAMission() || !displayOk) {
                continue;
            }
        } else if (!displayOk) {
            continue;
        }

        switch (trace.m_nBlipType) {
        case BLIP_CAR: {
            const auto vehicle = GetVehiclePool()->GetAtRef(trace.m_nEntityHandle);
            assert(vehicle); // NOTSA

            const auto posn = [vehicle] {
                auto bbMaxZ = vehicle->GetColModel()->GetBoundingBox().m_vecMax.z;
                auto vehPos = vehicle->GetPosition();

                // Como el original (0x585D3E..0x585D75): el Nevada multiplica por 0.6 (0x858CC8) y el resto por 1.2 (0x858F08); z = ((maxZ * k) + z) + 2.0
                // (x e y se copian sin sumar nada).
                bbMaxZ *= ModelIndices::IsNevada(vehicle->m_nModelIndex) ? RadarExeFloat(0x858CC8) : RadarExeFloat(0x858F08);

                return CVector{vehPos.x, vehPos.y, (bbMaxZ + vehPos.z) + 2.0f};
            }();

            PutMarkerCone(coneHandle, posn, 2.0f, color);
            break;
        }
        case BLIP_CHAR: {
            const auto ped = GetPedPool()->GetAtRef(trace.m_nEntityHandle);
            assert(ped); // NOTSA

            const auto pedPos = ped->GetRealPosition();
            PutMarkerCone(coneHandle, CVector{pedPos.x, pedPos.y, pedPos.z + 2.7f}, 1.2f, color); // 0x585E48..0x585E63
            break;
        }
        case BLIP_OBJECT:
        case BLIP_PICKUP: {
            const auto posn = [&trace]() {
                CVector ret{};
                if (trace.m_nBlipType == BLIP_OBJECT) {
                    if (const auto obj = GetObjectPool()->GetAtRef(trace.m_nEntityHandle)) {
                        const auto bbMaxZ = obj->GetColModel()->GetBoundingBox().m_vecMax.z;
                        const auto objPos = obj->GetPosition();
                        ret = CVector{objPos.x, objPos.y, objPos.z + bbMaxZ}; // 0x585D3E..0x585D68
                    } else {
                        NOTSA_UNREACHABLE("Couldn't get the object!");
                    }
                } else { // OBJECT_PICKUP
                    if (const auto idx = CPickups::GetActualPickupIndex(trace.m_nEntityHandle); idx >= 0) {
                        const auto pickupPos = CPickups::aPickUps[idx].GetPosn();
                        ret = CVector{pickupPos.x, pickupPos.y, pickupPos.z + 2.0f}; // 0x585F41..0x585F59 (maxZ = 2.0)
                    } else {
                        NOTSA_UNREACHABLE("Couldn't get the pickup!");
                    }
                }
                ret.z += (CGame::currArea != 0 || FindPlayerPed()->GetAreaCode() != AREA_CODE_NORMAL_WORLD) ? 1.6f : 1.8f;

                return ret;
            }();

            PutMarkerCone(coneHandle, posn, 0.8f, color);
            break;
        }
        case BLIP_CONTACT_POINT: {
            if (!FindPlayerPed())
                break;

            // Como el original (0x585C7D..0x585CB5): con entrada/salida asociada (m_pEntryExit != 0) solo se dibuja si el area del jugador es distinta de
            // 0; sin ella, solo si es 0 (el codigo anterior miraba m_bTrackingBlip, que aqui siempre es true). Se llama a FindPlayerPed dos veces.
            if ((trace.m_pEntryExit != nullptr) != (FindPlayerPed()->GetAreaCode() != AREA_CODE_NORMAL_WORLD))
                break;

            C3dMarkers::PlaceMarkerSet(coneHandle, MARKER3D_CYLINDER, trace.m_vPosition, 2.0f, 255, 0, 0, 228, 2048u, 0.2f, 0);
            break;
        }
        default:
            break;
        }
    }
}

// unused
// 0x585FE0
void CRadar::SetRadarMarkerState(int32 counter, bool flag) {
    // NOP

    /* III/VC leftover
    const auto GetEntity = [=]() -> CEntity* {
        const auto handle = ms_RadarTrace[counter].m_nEntityHandle;
        switch (ms_RadarTrace[counter].m_nBlipType) {
        case BLIP_CAR:
            return GetVehiclePool()->GetAt(handle);
        case BLIP_CHAR:
            return GetPedPool()->GetAt(handle);
        case BLIP_OBJECT:
            return GetObjectPool()->GetAt(handle);
        default:
            return nullptr;
        }
    };

    if (auto* entity = GetEntity()) {
        entity->bHasBlip = flag;
    }
    */
}

/*
 * @brief Draw a sprite to specific location and opacity.
 * @addr 0x585FF0
 */
void CRadar::DrawRadarSprite(eRadarSprite spriteId, float x, float y, uint8 alpha) {
    Limit(x, y);

    // Como el original (0x586036..0x586069): (ancho * 0x859520) * 8.0 [0x859000] truncado con _ftol2 a entero (no floor de un float)
    const int32 width  = RadarFtol(RadarStretchBaseX() * 8.0f);
    const int32 height = RadarFtol(RadarStretchBaseY() * 8.0f);

    if (DisplayThisBlip(spriteId, -99)) {
        RadarBlipSprites.data()[(size_t)spriteId].Draw(
            { x - (float)width, y - (float)height, x + (float)width, y + (float)height },
            { 255, 255, 255, alpha }
        );
        AddBlipToLegendList(false, spriteId);
    }
}

// 0x586110
void CRadar::DrawRadarSection(int32 x, int32 y) {
    CVector2D corners[8]{};
    GetTextureCorners(x, y, corners);

    // Como el original (0x58613B..0x5861BB): fuera del mapa (x o y fuera de 0..11) se dibuja el mar; con el jugador fuera del mapa se dibuja gris; en
    // otro caso hace falta la textura: sin ranura de TXD (-1), sin diccionario o sin textura se sale SIN dibujar nada.
    const bool outOfBounds = !IsMapSectionInBounds(x, y);
    RwTexture* texture = nullptr;
    if (!outOfBounds && !CTheScripts::bPlayerIsOffTheMap) {
        const auto txdIndex = gRadarTextures[y][x];
        if (txdIndex == -1) {
            return;
        }
        const auto txd = CTxdStore::GetTxd(txdIndex);
        if (!txd) {
            return;
        }
        texture = GetFirstTexture(txd);
        if (!texture) {
            return;
        }
    }

    // 0x5861C1..0x5862C2: (esquina - origen) * r, con r = 1.0 / m_radarRange, y rotacion en el sentido de las agujas del reloj
    CVector2D rotated[8]{};
    const float r = 1.0f / m_radarRange;
    for (auto i = 0; i < 4; i++) {
        const float dx = corners[i].x - vec2DRadarOrigin.x;
        const float dy = corners[i].y - vec2DRadarOrigin.y;
        const float rx = r * dx;
        const float ry = r * dy;
        rotated[i] = CVector2D{ cachedSin * ry + cachedCos * rx, cachedCos * ry - cachedSin * rx };
    }
    CVector2D clipped[8]{};
    const auto numVerts = ClipRadarPoly(clipped, rotated);
    if (numVerts == 0) {
        return;
    }

    // 0x586337..0x586410
    CVector2D texCoords[8]{};
    CVector2D verts[8]{};
    for (auto i = 0; i < numVerts; i++) {
        const float worldX = (cachedCos * clipped[i].x - cachedSin * clipped[i].y) * m_radarRange + vec2DRadarOrigin.x;
        const float worldY = (cachedSin * clipped[i].x + cachedCos * clipped[i].y) * m_radarRange + vec2DRadarOrigin.y;
        texCoords[i] = TransformRealWorldToTexCoordSpace(CVector2D{ worldX, worldY }, x, y);
        verts[i]     = TransformRadarPointToScreenSpace(clipped[i]);
    }

    if (outOfBounds) { // there is no land here, draw the sea.
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
        CSprite2d::SetVertices(numVerts, verts, texCoords, { 111, 137, 170, 255 });
    } else if (CTheScripts::bPlayerIsOffTheMap) { // Draw blank
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
        CSprite2d::SetVertices(numVerts, verts, texCoords, { 204, 204, 204, 255 });
    } else {
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, texture->raster);
        CSprite2d::SetVertices(numVerts, verts, texCoords, { 255, 255, 255, 255 });
    }

    // Now draw what we have
    if (numVerts > 2) {
        RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices.data(), numVerts);
    }
}

// 0x586520
void CRadar::DrawRadarSectionMap(int32 x, int32 y, CRect rect) {
    if (!IsMapSectionInBounds(x, y))
        return;

    const auto txdIndex = gRadarTextures[y][x];
    if (txdIndex == -1)
        return;

    if (const auto txd = CTxdStore::GetTxd(txdIndex)) {
        if (const auto texture = GetFirstTexture(txd)) {
            const auto bg = CRGBA{255, 255, 255, 255};
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, texture->raster);

            CSprite2d::SetVertices(rect, bg, bg, bg, bg);
            RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices.data(), 4);
        }
    }
}

// Como el original (0x586D8E..0x586DC6): el color de un blip de coordenada depende de m_nCoordBlipAppearance (bits 6-7 del PRIMER byte de banderas):
// 0 -> color del blip (no amistoso), 1 -> azul (13), otro -> rojo (0).
static uint32 RadarTraceStaticColour(const tRadarTrace& trace) {
    switch (trace.m_nCoordBlipAppearance) {
    case 0:  return CRadar::GetRadarTraceColour(trace.m_nColour, trace.m_bBright, false);
    case 1:  return HudColour.GetIntColour(HUD_COLOUR_BLUE);
    default: return HudColour.GetIntColour(HUD_COLOUR_RED);
    }
}

// Como el original (0x5867A2..0x5867F7): fsin x87, + 1.0, * 0.5 y fimul con el alfa de la zona; el resultado se trunca con _ftol2.
static float RadarGangPulse(float angle, int32 alphaInt) {
    return RadarGangPulseCpp(angle, alphaInt, RadarExeFloat(0x858624), RadarExeFloat(0x858B8C));
}

// 0x586650
void CRadar::DrawRadarGangOverlay(bool inMenu) {
    static auto& g_RadarGangResetOverlay = StaticRef<uint32>(0xBAA36C); // bool?
    static auto& g_RadarGangOverlay = StaticRef<CRect>(0xBAA35C);

    if ((g_RadarGangResetOverlay & 1) == 0) {
        g_RadarGangResetOverlay |= 1u;
        g_RadarGangOverlay = CRect{};
    }

    if (!CGangWars::bGangWarsActive || !FrontEndMenuManager.m_abPrefsMapBlips[4])
        return;

    for (auto& zone : CTheZones::GetNavigationZones()) {
        const auto info = CTheZones::GetZoneInfo(&zone);

        if (!info || !info->RadarMode || !CGangWars::CanPlayerStartAGangWarHere(info))
            continue;

        g_RadarGangOverlay = zone.GetRect();

        // todo: enum
        switch (info->RadarMode) {
        case 1:
            DrawAreaOnRadar(g_RadarGangOverlay, info->ZoneColor, inMenu);
            break;
        case 2: {
            const auto color = [&]() -> CRGBA {
                const auto timeInMS = FrontEndMenuManager.m_bDrawingMap ? CTimer::GetTimeInMSPauseMode() : CTimer::GetTimeInMS();

                auto zoneColor = info->ZoneColor;
                // Como el original (0x586779..0x5867FC): (t & 0x3FF) como float sin signo * 0x865034 (2*pi/1024), fsin x87 sin redondear,
                // + 1.0, * 0.5, * alfa (fimul) y _ftol2 (el codigo anterior multiplicaba por 1024 / 2*pi)
                const float angle = (float)(timeInMS & 0x3FF) * RadarExeFloat(0x865034);
                zoneColor.a = (uint8)RadarFtol(RadarGangPulse(angle, (int32)zoneColor.a));

                return zoneColor;
            }();
            DrawAreaOnRadar(g_RadarGangOverlay, color, inMenu);
            break;
        }
        default:
            break;
        }
    }
}

// 0x586880
void CRadar::DrawRadarMap() {
    DrawRadarMask();

    // Como el original (0x58688C..0x5868DD): (coord + 3000.0) * 0x858F44 (0.002f) -> floor, y 11.0 [0x858FBC] - (coord + 3000.0) * 0.002f -> ceil; _ftol2
    const float tileScale = RadarExeFloat(0x858F44);
    const float sumX = vec2DRadarOrigin.x + 3000.0f;
    const float sumY = vec2DRadarOrigin.y + 3000.0f;
    auto x = (int32)(int64)std::floor(sumX * tileScale);
    auto y = (int32)(int64)std::ceil(RadarExeFloat(0x858FBC) - sumY * tileScale);

    SetupRadarRect(x, y);
    StreamRadarSections(x, y);

    RwRenderStateSet(rwRENDERSTATEFOGENABLE,          RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,           RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,          RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,      RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,          RWRSTATE(D3DSHADE_FLAT));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,        RWRSTATE(rwRENDERSTATETEXTURERASTER));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,  RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,     RWRSTATE(D3DTADDRESS_CLAMP));
    RwRenderStateSet(rwRENDERSTATETEXTUREPERSPECTIVE, RWRSTATE(FALSE));

    DrawRadarSection(x - 1,     y - 1);
    DrawRadarSection(x,         y - 1);
    DrawRadarSection(x + 1,     y - 1);
    DrawRadarSection(x - 1,     y    );
    DrawRadarSection(x,         y    );
    DrawRadarSection(x + 1,     y    );
    DrawRadarSection(x - 1,     y + 1);
    DrawRadarSection(x,         y + 1);
    DrawRadarSection(x + 1,     y + 1);

    DrawRadarGangOverlay(false);

    const auto vehicle = FindPlayerVehicle();

    // Draw green rectangle when in plane
    if (vehicle && vehicle->IsSubPlane() && !ModelIndices::IsVortex(vehicle->m_nModelIndex)) {
        CVector playerPos = FindPlayerCentreOfWorldForMap(0);

        const auto cSin = cachedSin;
        const auto cCos = cachedCos;
        // Como el original (0x586A17..0x586A4A): sin/cos x87 de la constante double 0x866B80 (= float(PI) 3.1415927410125732) sin redondear antes
        // La constante double 0x866B80 es exactamente float(PI) (0x40490FDB): float(sin/cos(double(pi))) es fsin/fcos + un fstp dword.
        const float piConst = (float)*reinterpret_cast<const double*>(0x866B80);
        const float sinPi = TrigSimpleSinCpp(piConst);
        const float cosPi = TrigSimpleCosCpp(piConst);
        cachedSin = sinPi;
        cachedCos = cosPi;

        // Pitch and roll combined. Como el original (0x586A76..0x586A99): fpatan (precision completa) * 57.2957763671875 [0x859878] * 2 (fadd st,st) sin
        // redondear el angulo a float; resultado = y - eso.
        const float forwardZ = vehicle->m_matrix->GetForward().z;
        const float upZ      = vehicle->m_matrix->GetUp().z;
        const float degrees  = RadarExeFloat(0x859878);
        const float playerY  = playerPos.y;
        const float bottomY = RadarPlaneBottomYCpp(forwardZ, upZ, degrees, playerY);

        DrawAreaOnRadar(
            RadarRawRect(playerPos.x - 1000.0f, playerPos.y + 2000.0f, playerPos.x + 1000.0f, bottomY),
            { 20, 175, 20, 200 },
            false
        );

        cachedSin = cSin;
        cachedCos = cCos;
    }

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
}

// 0x586B00
void CRadar::DrawMap() {
    CalculateCachedSinCos();

    // Como el original (0x586B08..0x586C9F): FindPlayerPed no se llama aqui (solo al final y solo si currArea == 0); la informacion del jugador es
    // CWorld::Players[PlayerInFocus] (0x586B1C movzx ecx,byte [0B7CD74h] * 0x190 + 0B7CD98h)
    const auto vehicle = FindPlayerVehicle();

    if (!vehicle || CWorld::Players[CWorld::PlayerInFocus].IsPlayerInRemoteMode()) {
        if (CTheScripts::RadarZoomValue)
            m_radarRange = (float)(180 - (int32)CTheScripts::RadarZoomValue);
        else
            m_radarRange = RADAR_MIN_RANGE;
    } else {
        if (FindPlayerVehicle() && FindPlayerVehicle()->IsSubPlane() && !ModelIndices::IsVortex(FindPlayerVehicle()->m_nModelIndex)) {
            // 0x586B83..0x586C03: z * 0x858B4C (0.005f) en vez de z / 200
            const auto speedZ = FindPlayerVehicle()->GetPosition().z * RadarExeFloat(0x858B4C);

            if (speedZ < RadarExeFloat(0x858C24))
                m_radarRange = 340.0f;
            else if (!(speedZ < RadarExeFloat(0x858C20)))
                m_radarRange = RADAR_MAX_RANGE;
            else
                m_radarRange = (speedZ - RadarExeFloat(0x858C24)) * RadarExeFloat(0x866B8C) + RadarExeFloat(0x85F074); // (v - 0.3) * 16.666668 + 340.0
        } else {
            const auto speed = FindPlayerSpeed().Magnitude();

            if (speed < RadarExeFloat(0x858C24))
                m_radarRange = RADAR_MIN_RANGE;
            else if (!(speed < RadarExeFloat(0x858C20)))
                m_radarRange = RADAR_MAX_RANGE;
            else
                m_radarRange = (speed - RadarExeFloat(0x858C24)) * RadarExeFloat(0x866B88) + RadarExeFloat(0x85A994); // (v - 0.3) * 283.33334 + 180.0
        }
    }

    // 0x586C9F..0x586D2F
    if (!CGameLogic::IsCoopGameGoingOn()) [[likely]] {
        const auto centre = FindPlayerCentreOfWorldForMap(0);
        vec2DRadarOrigin = CVector2D{ centre.x, centre.y };
    } else if (CGameLogic::n2PlayerPedInFocus == eFocusedPlayer::PLAYER1) {
        const auto centre = FindPlayerCentreOfWorldForMap(0);
        vec2DRadarOrigin = CVector2D{ centre.x, centre.y };
    } else if (CGameLogic::n2PlayerPedInFocus == eFocusedPlayer::PLAYER2) {
        const auto centre = FindPlayerCentreOfWorldForMap(1);
        vec2DRadarOrigin = CVector2D{ centre.x, centre.y };
    } else {
        // Halfway between the two player's positions (0x586CBE..0x586CFB: el EXE pide primero el jugador 1 y despues el 0)
        const auto centre1 = FindPlayerCentreOfWorldForMap(1);
        const auto centre0 = FindPlayerCentreOfWorldForMap(0);
        vec2DRadarOrigin = CVector2D{ (centre0.x + centre1.x) * 0.5f, (centre0.y + centre1.y) * 0.5f };
    }

    // 0x586D1A..0x586D4E: FindPlayerPed solo se consulta si currArea == 0
    if (CGame::currArea == 0 && FindPlayerPed()->GetAreaCode() == AREA_CODE_NORMAL_WORLD && FrontEndMenuManager.m_nRadarMode != 1) {
        DrawRadarMap();
    }
}

/*
 * @brief Draw a blip by its index in `ms_RadarTrace`
 * @addr 0x586D60
 */
void CRadar::DrawCoordBlip(int32 blipIndex, bool isSprite) {
    auto& trace = ms_RadarTrace.data()[blipIndex];
    if (trace.m_nBlipType == BLIP_CONTACT_POINT && CTheScripts::IsPlayerOnAMission())
        return;

    // 0x586D8E..0x586DC6: el color depende de m_nCoordBlipAppearance (bits 6-7 del primer byte de banderas)
    const uint32 colorInt = RadarTraceStaticColour(trace);

    // 0x586DC8..0x586E23: con depuracion de scripts, un marcador de depuracion pulsante en el blip
    if ((trace.m_nBlipDisplayFlag == BLIP_DISPLAY_BOTH || trace.m_nBlipDisplayFlag == BLIP_DISPLAY_MARKERONLY) && CTheScripts::DbgFlag) {
        ShowRadarMarker(trace.m_vPosition, colorInt, trace.m_fSphereRadius);
        trace.m_fSphereRadius -= RadarExeFloat(0x858B1C); // 0.1
        if (trace.m_fSphereRadius < 1.0f) {
            trace.m_fSphereRadius = 5.0f;
        }
    }

    if (isSprite == !trace.HasSprite())
        return;

    if (trace.m_nBlipDisplayFlag != BLIP_DISPLAY_BOTH && trace.m_nBlipDisplayFlag != BLIP_DISPLAY_BLIPONLY)
        return;

    // 0x586E4B..0x586EE4: posicion en el mundo, radar, limitada, alfa, distancia con zoom y pantalla (en este orden)
    const auto worldPos = trace.GetWorldPos();
    auto radarPos = TransformRealWorldPointToRadarSpace(CVector2D{ worldPos.x, worldPos.y });
    const float realDist = LimitRadarPoint(radarPos);
    uint8 alpha = CalculateBlipAlpha(realDist);
    const float zoomedDist = CTheScripts::RadarZoomValue != 0u ? 255.0f : realDist;
    if (!trace.m_bBlipFade) { // 0x586EC2: test dl,20h / jne: sin fundido el alfa es el del color (bl); con fundido, el calculado por la distancia
        alpha = uint8(colorInt & 0xFF);
    }
    const auto screenPos = TransformRadarPointToScreenSpace(radarPos);

    if (isSprite) {
        if (!trace.HasSprite())
            return;

        // either the blip is close to the player or we're drawing the whole map.
        const auto canBeDrawn = !trace.m_bShortRange || zoomedDist <= 1.0f || FrontEndMenuManager.m_bDrawingMap;

        if (canBeDrawn && HasThisBlipBeenRevealed(blipIndex)) {
            DrawRadarSprite(trace.m_nBlipSprite, screenPos.x, screenPos.y, 255);
        }
        return;
    }

    if (trace.HasSprite())
        return;

    // 0x586F53..0x586FB5: la altura se calcula ANTES de comprobar si se muestran los blips de mision
    const auto playerCentre = FindPlayerCentreOfWorldForMap(PED_TYPE_PLAYER1);
    eRadarTraceHeight height;
    if (worldPos.z - 2.0f > playerCentre.z) {
        height = RADAR_TRACE_LOW; // trace is higher
    } else if (worldPos.z + 4.0f < playerCentre.z) {
        height = RADAR_TRACE_HIGH; // player is higher
    } else {
        height = RADAR_TRACE_NORMAL; // they are at the around the same elevation.
    }

    if (FrontEndMenuManager.m_bDrawingMap && !FrontEndMenuManager.m_ShowMissionBlips) {
        // drawing the whole map but mission blips are hidden by the player.
        return;
    }

    // Los argumentos de color se pasan como en el EXE (b = color >> 8, g = color >> 16, r = color >> 24, sin enmascarar); el destino usa solo el byte bajo.
    CRadar::ShowRadarTraceWithHeight(
        screenPos.x,
        screenPos.y,
        (uint32)(int32)(int16)trace.m_nBlipSize,
        colorInt >> 24,
        colorInt >> 16,
        colorInt >> 8,
        alpha,
        height
    );

    AddBlipToLegendList(true, blipIndex);
}

// 0x587000
void CRadar::DrawEntityBlip(int32 blipIndex, uint8 arg1) {
    plugin::Call<0x587000, int32, uint8>(blipIndex, arg1);
}

/*
 * @brief Clear a blip by its index in `ms_RadarTrace`
 * @addr 0x587C10
 */
void CRadar::ClearActualBlip(int32 blipIndex) {
    if (blipIndex < 0 || blipIndex >= MAX_RADAR_TRACES)
        return;

    ClearActualBlip(ms_RadarTrace[blipIndex]);
}

/*
 * @notsa
 * @brief Clear a blip by its trace reference.
 */
void CRadar::ClearActualBlip(tRadarTrace& trace) {
    trace.m_nBlipSize = 1;
    trace.m_fSphereRadius = 1.0f;
    trace.m_pEntryExit = nullptr;
    trace.m_nBlipSprite = RADAR_SPRITE_NONE;

    trace.m_bBright       = true;
    trace.m_bTrackingBlip = false;
    trace.m_bShortRange   = false;
    trace.m_bFriendly     = false;
    trace.m_bBlipRemain   = false;
    trace.m_bBlipFade     = false;

    trace.m_nCoordBlipAppearance = BLIP_FLAG_FRIEND;
    trace.m_nBlipDisplayFlag = BLIP_DISPLAY_NEITHER;
    trace.m_nBlipType = BLIP_NONE;
}

/*
 * @brief Clear a blip by type and handle of the entity.
 * @addr 0x587C60
 */
void CRadar::ClearBlipForEntity(eBlipType blipType, int32 entityHandle) {
    for (auto& trace : ms_RadarTrace) {
        if (trace.m_nBlipType == blipType && trace.m_nEntityHandle == entityHandle) {
            ClearActualBlip(trace);
        }
    }
}

void CRadar::ClearBlipForEntity(CPed* ped) {
    ClearBlipForEntity(BLIP_CHAR, GetPedPool()->GetIndex(ped));
}

/*
 * @brief Clear a blip
 * @addr 0x587CE0
 */
void CRadar::ClearBlip(tBlipHandle blip) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        ClearActualBlip(index);
    }
}

CVector GetAirStripLocation(eAirstripLocation location) {
    return CVector(airstrip_table[location].position);
}

// 0x587D20
void CRadar::SetupAirstripBlips() {
    if (const auto veh = FindPlayerVehicle(); veh && veh->IsSubPlane() && !ModelIndices::IsVortex(veh->m_nModelIndex)) {
        // Como el original (0x587D72 test byte ptr ds:[0B7CB4Ch],4): contador de fotogramas, no el temporizador de 200 ms del codigo FIX_BUGS.
        if ((CTimer::GetFrameCounter() & 4) == 0) {
            if (airstrip_blip)
                return;

            const auto pos = GetAirStripLocation(airstrip_location);
            airstrip_blip = SetCoordBlip(BLIP_AIRSTRIP, pos, BLIP_COLOUR_RED, BLIP_DISPLAY_BLIPONLY, reinterpret_cast<const char*>(0x866B90)); // "CODEAIR"
            return;
        }

        // 0x587D7F..0x587E35: distancia 2D a cada pista (sqrt(dy*dy + dx*dx))
        float distances[NUM_AIRSTRIPS]{};
        for (auto i = 0u; i < NUM_AIRSTRIPS; i++) {
            const auto vehPos = veh->GetPosition();
            const float dy = airstrip_table[i].position.y - vehPos.y;
            const float dx = airstrip_table[i].position.x - vehPos.x;
            distances[i] = std::sqrt(dy * dy + dx * dx);
        }

        // 0x587E3B..0x587ECC: la pista mas cercana solo si es ESTRICTAMENTE menor que las demas (los empates y los NaN acaban en la ultima)
        const auto location = [&]() -> eAirstripLocation {
            if (distances[0] < distances[1] && distances[0] < distances[2] && distances[0] < distances[3])
                return AIRSTRIP_LS_AIRPORT;
            if (distances[1] < distances[0] && distances[1] < distances[2] && distances[1] < distances[3])
                return AIRSTRIP_SF_AIRPORT;
            if (distances[2] < distances[0] && distances[2] < distances[1] && distances[2] < distances[3])
                return AIRSTRIP_LV_AIRPORT;
            return AIRSTRIP_VERDANT_MEADOWS;
        }();

        // 0x587ECE..0x587EFD: si la pista cambia, solo se actualiza (y se borra el blip) cuando ya habia blip
        if (airstrip_location != location) {
            if (airstrip_blip) {
                ClearBlip(airstrip_blip);
                airstrip_location = location;
            }
        } else if (airstrip_blip) {
            return;
        }

        const auto pos = GetAirStripLocation(airstrip_location);
        airstrip_blip = SetCoordBlip(BLIP_AIRSTRIP, pos, BLIP_COLOUR_RED, BLIP_DISPLAY_BLIPONLY, reinterpret_cast<const char*>(0x866B90));
        return;
    }

    if (airstrip_blip) {
        // player is not flying anymore.
        ClearBlip(airstrip_blip);
        airstrip_blip = 0;
    }
}

// 0x588050
void CRadar::DrawBlips() {
    // Como el original (0x588059..0x5880AD): limites de la regla del mapa del menu = _ftol2 (truncado a entero) de (alto * 0x859524) * 60.0 [0x858B34],
    // de alto - eso, y lo mismo con el ancho; el raton se compara como entero con esos limites.
    const float marginY = RadarStretchBaseY() * RadarExeFloat(0x858B34);
    const float marginX = RadarStretchBaseX() * RadarExeFloat(0x858B34);
    const int32 mouseYMin = RadarFtol(marginY), mouseYMax = RadarFtol((float)RsGlobal.maximumHeight - marginY);
    const int32 mouseXMin = RadarFtol(marginX), mouseXMax = RadarFtol((float)RsGlobal.maximumWidth - marginX);

    SetupAirstripBlips();
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(false));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(false));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(true));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(false));

    const auto TransformRealWorldPointToBlipSpace = [](const CVector2D& pos) {
        auto radar = TransformRealWorldPointToRadarSpace(pos);
        LimitRadarPoint(radar);
        return TransformRadarPointToScreenSpace(radar);
    };

    if (FrontEndMenuManager.m_bDrawingMap) {
        if (FrontEndMenuManager.m_bMapLoaded) {
            // draw the menu map ruler.
            const auto mouseX = FrontEndMenuManager.m_nMousePosX, mouseY = FrontEndMenuManager.m_nMousePosY;

            if (!CPad::IsMouseLButton() &&
                mouseX > mouseXMin && mouseX < mouseXMax &&
                mouseY > mouseYMin && mouseY < mouseYMax ||
                !FrontEndMenuManager.m_DisplayTheMouse)
            {
                auto drawPos = TransformRealWorldPointToBlipSpace(FrontEndMenuManager.m_vMousePos);
                LimitToMap(drawPos.x, drawPos.y);

                // 0x58823D..0x588289 / 0x58828F..0x5882D5: (ancho * 0x859520) * x y (alto * 0x859524) * y; los rectangulos se escriben campo a campo
                const float lineX = RadarStretchBaseX() * drawPos.x;
                CSprite2d::DrawRect(
                    RadarRawRect(lineX - 1.0f, (float)RsGlobal.maximumHeight, lineX + 1.0f, 0.0f),
                    HudColour.GetRGB(HUD_COLOUR_GOLD)
                );

                const float lineY = RadarStretchBaseY() * drawPos.y;
                CSprite2d::DrawRect(
                    RadarRawRect(0.0f, lineY + 1.0f, (float)RsGlobal.maximumWidth, lineY - 1.0f),
                    HudColour.GetRGB(HUD_COLOUR_GOLD)
                );
            }
        }
    } else {
        // draw the (N) blip. Como el original (0x58812D..0x58818D): (0, rango * 1.4142135 [0x866BA4]) + origen
        const auto drawPos = TransformRealWorldPointToBlipSpace(CVector2D{ vec2DRadarOrigin.x, m_radarRange * RadarExeFloat(0x866BA4) + vec2DRadarOrigin.y });
        DrawRadarSprite(RADAR_SPRITE_NORTH, drawPos.x, drawPos.y, 255);
    }

    // Como el original (0x5882E3..0x588434): primero se recorre todo con isSprite = true y despues con false
    for (const auto isSprite : {true, false}) {
        for (auto priority = 1; priority < 4; priority++) {
            for (auto&& [i, trace] : rngv::enumerate(ms_RadarTrace)) { // todo: check if looping all
                if (!trace.m_bTrackingBlip)
                    continue;

                switch (trace.m_nBlipType) {
                case BLIP_CAR:
                case BLIP_CHAR:
                case BLIP_OBJECT:
                case BLIP_PICKUP:
                    if (DisplayThisBlip(trace.m_nBlipSprite, priority)) {
                        DrawEntityBlip(i, isSprite);
                    }
                    break;
                case BLIP_COORD:
                case BLIP_CONTACT_POINT:
                    if (trace.m_nBlipSprite != RADAR_SPRITE_WAYPOINT && DisplayThisBlip(trace.m_nBlipSprite, priority)) {
                        DrawCoordBlip(i, isSprite);
                    }
                    break;
                case BLIP_SPOTLIGHT:
                case BLIP_AIRSTRIP:
                    if (priority == 3 && (!CTheScripts::bPlayerIsOffTheMap || !FrontEndMenuManager.m_bDrawingMap)) {
                        DrawEntityBlip(i, isSprite);
                    }
                    break;
                default:
                    break;
                }
            }
        }

        // 0x5883A2..0x588419: los blips de coordenada con el icono de la baliza (WAYPOINT) se dibujan siempre, con el mismo isSprite del bucle
        // (el EXE los pasa a DrawCoordBlip con el valor de la pasada actual; el codigo anterior pasaba siempre true)
        for (auto&& [i, trace] : rngv::enumerate(ms_RadarTrace)) { // todo: check if looping all, same thing with above.
            if (!trace.m_bTrackingBlip)
                continue;

            switch (trace.m_nBlipType) {
            case BLIP_COORD:
            case BLIP_CONTACT_POINT:
                if (trace.m_nBlipSprite == RADAR_SPRITE_WAYPOINT) {
                    DrawCoordBlip(i, isSprite);
                }
                break;
            default:
                break;
            }
        }
    }

    // Como el original (0x588446..0x5886BE): la posicion del marcador del jugador usa SIEMPRE al jugador 0 (el codigo FIX_BUGS usaba el indice del bucle),
    // se multiplica por 1.0 / rango (no se divide) y el radar se limita como LimitRadarPoint.
    const auto GetPlayerMarkerPosition = [](int32 playerIndex) {
        const auto centre = FindPlayerCentreOfWorldForMap(playerIndex);
        const float dx = centre.x - vec2DRadarOrigin.x;
        const float dy = centre.y - vec2DRadarOrigin.y;
        const float r = 1.0f / m_radarRange;
        const float rx = r * dx;
        const float ry = r * dy;

        CVector2D rotatedPos = {
            cachedSin * ry + cachedCos * rx,
            cachedCos * ry - cachedSin * rx
        };
        LimitRadarPoint(rotatedPos);
        return TransformRadarPointToScreenSpace(rotatedPos);
    };

    if (!FrontEndMenuManager.m_bDrawingMap) {
        for (auto i = 0; i < 2; i++) {
            const auto player = FindPlayerPed(i);
            if (!player)
                continue;

            // we don't draw the player marker while flying.
            if (auto veh = FindPlayerVehicle(i); veh && veh->IsSubPlane() && !ModelIndices::IsVortex(veh->m_nModelIndex))
                continue;

            const auto pos = GetPlayerMarkerPosition(0);

            // 0x5886B9..0x588704: modo TOPDOWN (1): rumbo + PI; si no, rumbo - (orientacion + PI) (el codigo anterior restaba en otro orden); el rumbo es el del jugador i
            const auto heading = FindPlayerHeading(i);
            const auto isHidden = CWorld::Players[i].m_pPed->IsHidden();
            const auto piConst = RadarExeFloat(0x858CB8);
            const float angle = CCamera::GetActiveCamera().m_nMode == MODE_TOPDOWN
                ? heading + piConst
                : heading - (m_fRadarOrientation + piConst);

            // Ancho y alto: ambos (ancho * 0x859520) * 8.0 truncado (el codigo FIX_BUGS usaba el alto para la altura)
            const auto size = (uint32)RadarFtol(RadarStretchBaseX() * RadarExeFloat(0x859000));
            DrawRotatingRadarSprite(
                RadarBlipSprites[RADAR_SPRITE_CENTRE],
                pos.x,
                pos.y,
                angle,
                size,
                size,
                isHidden ? CRGBA{50, 50, 80, 255} : CRGBA{255, 255, 255, 255} // 0x58865A push 50h / 32h / 32h
            );
        }
    } else {
        const auto pos = GetPlayerMarkerPosition(0);
        DrawYouAreHereSprite(pos.x, pos.y);
    }
}

/*
 * @brief Load radar blips from save file
 * @addr 0x5D53C0
 */
bool CRadar::Load() {
    Initialise();

    for (auto& trace : ms_RadarTrace) {
        CGenericGameStorage::LoadDataFromWorkBuffer(trace);
        if (trace.m_EntryExitPoolInd) {
            trace.m_pEntryExit = CEntryExitManager::GetInSlot(trace.m_EntryExitPoolInd - 1);
        }
    }

    return true;
}

/*
 * @brief Save radar blips to save file
 * @addr 0x5D5860
 */
bool CRadar::Save() {
    for (auto& trace : ms_RadarTrace) {
        CEntryExit* savedEnex = nullptr;
        if (trace.m_pEntryExit) {
            const auto index = CEntryExitManager::GetPool()->GetIndex(trace.m_pEntryExit);
            if (CEntryExitManager::GetPool()->IsIndexInBounds(index) && !CEntryExitManager::GetPool()->IsFreeSlotAtIndex(index)) {
                savedEnex = trace.m_pEntryExit;
                trace.m_EntryExitPoolInd = index + 1; // Assign the pool index for save duration, restore it later
            }
        }

        const auto& blipType = trace.m_nBlipType;
        auto bIsTracking = false;
        if (blipType != BLIP_COORD && blipType != BLIP_CONTACT_POINT) {
            if (trace.m_bTrackingBlip) {
                trace.m_bTrackingBlip = false;
                bIsTracking = true;
            }
        }

        CGenericGameStorage::SaveDataToWorkBuffer(&trace, sizeof(trace));
        if (savedEnex) {
            trace.m_pEntryExit = savedEnex;
        }
        if (bIsTracking) {
            trace.m_bTrackingBlip = true;
        }
    }

    return true;

}

/*!
 * @notsa
 * @brief Get name of a blip type. (See CRadar::DrawLegend)
 * @returns The name of the blip type
 */
const GxtChar* CRadar::GetBlipName(eRadarSprite blipType) {
    switch (blipType) {
    case RADAR_SPRITE_PLAYER_INTEREST:  return TheText.Get("LG_56");
    case RADAR_SPRITE_THREAT:           return TheText.Get("LG_55");
    case RADAR_SPRITE_FRIEND:           return TheText.Get("LG_54");
    case RADAR_SPRITE_OBJECT:           return TheText.Get("LG_50");
    case RADAR_SPRITE_DESTINATION:      return TheText.Get("LG_49");
    case RADAR_SPRITE_NONE:             break;
    case RADAR_SPRITE_WHITE:            break;
    case RADAR_SPRITE_CENTRE:           break;
    case RADAR_SPRITE_MAP_HERE:         return TheText.Get("LG_01");
    case RADAR_SPRITE_NORTH:            break; // NOTSA
    case RADAR_SPRITE_AIRYARD:          return TheText.Get("LG_02");
    case RADAR_SPRITE_AMMUGUN:          return TheText.Get("LG_03");
    case RADAR_SPRITE_BARBERS:          return TheText.Get("LG_04");
    case RADAR_SPRITE_BIGSMOKE:         return TheText.Get("LG_05");
    case RADAR_SPRITE_BOATYARD:         return TheText.Get("LG_06");
    case RADAR_SPRITE_BURGERSHOT:       return TheText.Get("LG_07");
    case RADAR_SPRITE_BULLDOZER:        return TheText.Get("LG_66");
    case RADAR_SPRITE_CATALINAPINK:     return TheText.Get("LG_09");
    case RADAR_SPRITE_CESARVIAPANDO:    return TheText.Get("LG_10");
    case RADAR_SPRITE_CHICKEN:          return TheText.Get("LG_11");
    case RADAR_SPRITE_CJ:               return TheText.Get("LG_12");
    case RADAR_SPRITE_CRASH1:           return TheText.Get("LG_13");
    case RADAR_SPRITE_DINER:            return TheText.Get("LG_67");
    case RADAR_SPRITE_EMMETGUN:         return TheText.Get("LG_15");
    case RADAR_SPRITE_ENEMYATTACK:      return TheText.Get("LG_16");
    case RADAR_SPRITE_FIRE:             return TheText.Get("LG_17");
    case RADAR_SPRITE_GIRLFRIEND:       return TheText.Get("LG_18");
    case RADAR_SPRITE_HOSTPITAL:        return TheText.Get("LG_19");
    case RADAR_SPRITE_LOGOSYNDICATE:    return TheText.Get("LG_20");
    case RADAR_SPRITE_MADDOG:           return TheText.Get("LG_21");
    case RADAR_SPRITE_MAFIACASINO:      return TheText.Get("LG_22");
    case RADAR_SPRITE_MCSTRAP:          return TheText.Get("LG_23");
    case RADAR_SPRITE_MODGARAGE:        return TheText.Get("LG_24");
    case RADAR_SPRITE_OGLOC:            return TheText.Get("LG_25");
    case RADAR_SPRITE_PIZZA:            return TheText.Get("LG_26");
    case RADAR_SPRITE_POLICE:           return TheText.Get("LG_27");
    case RADAR_SPRITE_PROPERTYG:        return TheText.Get("LG_28");
    case RADAR_SPRITE_PROPERTYR:        return TheText.Get("LG_29");
    case RADAR_SPRITE_RACE:             return TheText.Get("LG_30");
    case RADAR_SPRITE_RYDER:            return TheText.Get("LG_31");
    case RADAR_SPRITE_SAVEGAME:         return TheText.Get("LG_32");
    case RADAR_SPRITE_SCHOOL:           return TheText.Get("LG_33");
    case RADAR_SPRITE_QMARK:            return TheText.Get("LG_63");
    case RADAR_SPRITE_SWEET:            return TheText.Get("LG_35");
    case RADAR_SPRITE_TATTOO:           return TheText.Get("LG_36");
    case RADAR_SPRITE_THETRUTH:         return TheText.Get("LG_37");
    case RADAR_SPRITE_WAYPOINT:         return TheText.Get("LG_64");
    case RADAR_SPRITE_TORENORANCH:      return TheText.Get("LG_39");
    case RADAR_SPRITE_TRIADS:           return TheText.Get("LG_40");
    case RADAR_SPRITE_TRIADSCASINO:     return TheText.Get("LG_41");
    case RADAR_SPRITE_TSHIRT:           return TheText.Get("LG_42");
    case RADAR_SPRITE_WOOZIE:           return TheText.Get("LG_43");
    case RADAR_SPRITE_ZERO:             return TheText.Get("LG_44");
    case RADAR_SPRITE_DATEDISCO:        return TheText.Get("LG_45");
    case RADAR_SPRITE_DATEDRINK:        return TheText.Get("LG_46");
    case RADAR_SPRITE_DATEFOOD:         return TheText.Get("LG_47");
    case RADAR_SPRITE_TRUCK:            return TheText.Get("LG_48");
    case RADAR_SPRITE_CASH:             return TheText.Get("LG_51");
    case RADAR_SPRITE_FLAG:             return TheText.Get("LG_52");
    case RADAR_SPRITE_GYM:              return TheText.Get("LG_53");
    case RADAR_SPRITE_IMPOUND:          return TheText.Get("LG_57");
    case RADAR_SPRITE_LIGHT:            break;
    case RADAR_SPRITE_RUNWAY:           return TheText.Get("LG_65");
    case RADAR_SPRITE_GANGB:            return TheText.Get("LG_58");
    case RADAR_SPRITE_GANGP:            return TheText.Get("LG_59");
    case RADAR_SPRITE_GANGY:            return TheText.Get("LG_60");
    case RADAR_SPRITE_GANGN:            return TheText.Get("LG_61");
    case RADAR_SPRITE_GANGG:            return TheText.Get("LG_62");
    case RADAR_SPRITE_SPRAY:            return TheText.Get("LG_34");
    case RADAR_SPRITE_TORENO:           break;
    default:
        break;
    }
    NOTSA_UNREACHABLE("Bug in GetBlipName");
    return nullptr;
}

/*!
 * @notsa
 * @brief Returns the first index in `ms_RadarTrace` that is not a tracking blip.
 */
int32 CRadar::FindTraceNotTrackingBlipIndex() {
    for (auto&& [i, v] : rngv::enumerate(ms_RadarTrace)) {
        if (!v.m_bTrackingBlip) {
            return (int32)i;
        }
    }
    return -1;
}


/*!
 * @notsa
 * @brief Returns the color of the blip's apperance (Opaque)
 */
CRGBA tRadarTrace::GetStaticColour() const {
    // Como el original (DrawCoordBlip 0x586D8E..0x586DC6): depende de m_nCoordBlipAppearance (primer byte de banderas), no de m_nAppearance
    return CRGBA{RadarTraceStaticColour(*this)};
}

/*!
 * @notsa
 * @brief Returns the position of the blip always relative to the world.
 */
CVector tRadarTrace::GetWorldPos() const {
    CVector pos = { m_vPosition };
    if (m_pEntryExit) {
        m_pEntryExit->GetPositionRelativeToOutsideWorld(pos);
    }
    return pos;
}

/*!
 * @notsa
 * @brief Returns radar and screen relative positions of the trace.
 * @param radarPointDist Distance to the point in radar. (Optional)
 */
std::pair<CVector2D, CVector2D> tRadarTrace::GetRadarAndScreenPos(float* radarPointDist) const {
    const auto world = GetWorldPos();

    auto radar = CRadar::TransformRealWorldPointToRadarSpace(world);

    const auto dist = CRadar::LimitRadarPoint(radar); // Normalises vector `radar`
    if (radarPointDist) {
        *radarPointDist = dist;
    }

    const auto screen = CRadar::TransformRadarPointToScreenSpace(radar);

    return std::make_pair(radar, screen);
}
