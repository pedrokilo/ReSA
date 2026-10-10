#pragma once
// Casos de CHud: Draw, DrawAfterFade, DrawRadar, DrawWeaponIcon, Initialise y Shutdown.
// Los estados RW (RwRenderStateSet, que el EXE y la DLL leen por la tabla del motor 0xC97B24 +0x20) se doblan cambiando temporalmente ese puntero
// por una tabla falsa cuyo +0x20 es un doble con traza; los colaboradores de dibujo, TXD y sprites son EntryPatch (EXE + simbolo DLL + cuerpo).
#include "Common.hpp"

namespace HudCamFinal2Live {

// ---------------- motor RW falso ----------------
inline uint32 __cdecl SRwState(uint32 state, uint32 value) { if (Mine()) Push(T_RW, { state, value }); return 1; }
inline Buf<0x200> rwGlobals;
inline void SetRwEngine() {
    rwGlobals.Fill(0); At<uint32>(rwGlobals.p(), 0x20) = uint32(reinterpret_cast<uintptr_t>(&SRwState)); Mem<uint32>(0xC97B24) = P(rwGlobals.p());
}

// ---------------- jugador / vehiculo falsos ----------------
inline void* fixPed[2]{}; inline void* fixVeh[2]{};
inline void* realPed{}; inline void* realVeh{};      // los hilos ajenos reciben los valores reales capturados al empezar
inline void* __cdecl SFindPed(int32 id) { if (!Mine()) return realPed; return fixPed[(id < 0 ? Mem<uint8>(PLAYER_FOCUS) : uint32(id)) & 1]; }
inline void* __cdecl SFindVeh(int32 id, uint32) { if (!Mine()) return realVeh; return fixVeh[(id < 0 ? Mem<uint8>(PLAYER_FOCUS) : uint32(id)) & 1]; }

// ---------------- dobles de CHud::Draw / DrawAfterFade ----------------
template<uint32 TAG> inline void __cdecl SV0() { if (Mine()) Push(TAG); }
template<uint32 TAG> inline void __cdecl SV1(uint32 a) { if (Mine()) Push(TAG, { a & 0xFF }); }
inline void __cdecl SSetHelp(const void* text, uint32 quick, uint32 perm, uint32 brief) { if (!Mine()) return; Push(T_SETHELP, { TextHash(text), quick & 0xFF, perm & 0xFF, brief & 0xFF }); }
inline uint32 fixSkipCan{}, fixVital{};
inline uint32 __cdecl SSkipCan() { if (!Mine()) return 0; Push(T_SKIPCAN); return fixSkipCan; }
// CPad::GetDisplayVitalStats(CPed*) (miembro, 1 argumento; el EXE comprueba AX)
inline uint32 __fastcall SVitalPad(void*, void*, void* ped) { if (!Mine()) return 0; Push(T_VITALPAD, { P(ped) }); return fixVital; }

// ---------------- dobles de CHud::DrawRadar ----------------
// CSprite2d::Draw(x1..y4, const CRGBA&) (miembro, 9 argumentos: ret 0x24)
inline void __fastcall SSprite9(void* self, void*, uint32 x1, uint32 y1, uint32 x2, uint32 y2, uint32 x3, uint32 y3, uint32 x4, uint32 y4, const uint32* color) {
    if (!Mine()) return; Push(T_SPRITE9, { (P(self) - uint32(SPRITES)) / 4, x1, y1, x2, y2, x3, y3, x4, y4, *color });
}
// CSprite2d::Draw(const CRect&, const CRGBA&) (miembro, 2 argumentos: ret 8)
inline void __fastcall SSpriteRect(void* self, void*, const uint32* rect, const uint32* color) {
    if (!Mine()) return; Push(T_SPRITEDRAW, { (P(self) - uint32(SPRITES)) / 4, rect[0], rect[1], rect[2], rect[3], *color });
}
// CSprite2d::DrawRect(const CRect&, const CRGBA&) (estatica, cdecl)
inline void __cdecl SDrawRect(const uint32* rect, const uint32* color) { if (!Mine()) return; Push(T_DRAWRECT, { rect[0], rect[1], rect[2], rect[3], *color }); }

// ---------------- dobles de CHud::DrawWeaponIcon ----------------
inline void* fixTex{};
inline void* __cdecl SFindTex(void* dict, uint32 hash) { if (!Mine()) return nullptr; Push(T_FINDTEX, { P(dict), hash }); return fixTex; }
// CSprite::RenderOneXLUSprite(CVector pos, CVector2D half, r, g, b, intensity, rhw, a, u, v): 13 dwords cdecl
inline void __cdecl SXluSprite(uint32 x, uint32 y, uint32 z, uint32 hw, uint32 hh, uint32 r, uint32 g, uint32 b, uint32 inten, uint32 rhw, uint32 a, uint32 u, uint32 v) {
    if (!Mine()) return; Push(T_XLUSPRITE, { x, y, z, hw, hh, r & 0xFF, g & 0xFF, b & 0xFF, inten & 0xFFFF, rhw, a & 0xFF, u & 0xFF, v & 0xFF });
}

// ---------------- dobles de Initialise / Shutdown (TXD y sprites) ----------------
inline int32 __cdecl SAddSlot(const void* name) { if (!Mine()) return -1; Push(T_ADDSLOT, { TextHash(name) }); return 777; }
inline uint32 __cdecl SLoadTxd(int32 slot, const void* file) { if (!Mine()) return 0; Push(T_LOADTXD, { uint32(slot), TextHash(file) }); return 1; }
inline void __cdecl SAddRef(int32 slot) { if (Mine()) Push(T_ADDREF, { uint32(slot) }); }
inline void __cdecl SSetTxd(int32 slot) { if (Mine()) Push(T_SETTXD, { uint32(slot) }); }
inline void __fastcall SSetTexture(void* self, void*, const void* name, const void* mask) { if (Mine()) Push(T_SETTEXTURE, { (P(self) - uint32(SPRITES)) / 4, TextHash(name), TextHash(mask) }); }
inline void __fastcall SDelete(void* self, void*) { if (Mine()) Push(T_DELETE, { (P(self) - uint32(SPRITES)) / 4 }); }
inline int32 __cdecl SFindSlot(const void* name) { if (!Mine()) return -1; Push(T_FINDSLOT, { TextHash(name) }); return 777; }
inline void __cdecl SRemoveSlot(int32 slot) { if (Mine()) Push(T_REMOVESLOT, { uint32(slot) }); }

// ---------------- fixtures compartidos ----------------
inline Buf<0x700> hudVeh; inline Buf<0x800> hudPed; inline Buf<0x48> hudMat, hudMat2;
// Vehiculo falso: subtipo (+0x594), modelo (+0x22), posicion (+4) y, si se pide, matriz (+0x14)
inline void MakeVehicle(uint32& s, uint32 subtype, int32 model, bool withMatrix) {
    hudVeh.Fill(0); uint8* v = hudVeh.p();
    At<uint32>(v, 0x594) = subtype; At<int16>(v, 0x22) = int16(model);
    RandVec(v, 0x04, s);
    if (withMatrix) { RandMatrix(hudMat.p(), s, (Rnd(s) & 1) != 0); At<uint32>(v, 0x14) = P(hudMat.p()); }
}
// Ped falso: ranura de arma activa (+0x718), tipo del arma (+0x5A0 + 0x1C * ranura), posicion (+4) y, si se pide, matriz (+0x14)
inline void MakePed(uint32& s, int32 weaponType, bool withMatrix) {
    hudPed.Fill(0); uint8* p = hudPed.p();
    const int8 slot = int8(Rnd(s) % 13); At<int8>(p, 0x718) = slot; At<uint32>(p, 0x5A0 + uint32(int32(slot) * 0x1C)) = uint32(weaponType);
    RandVec(p, 0x04, s);
    if (withMatrix) { RandMatrix(hudMat2.p(), s, false); At<uint32>(p, 0x14) = P(hudMat2.p()); }
}

// =====================================================================================================================
// CHud::Draw (0x58FAE0)
// =====================================================================================================================
inline json CaseDraw(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x6001); FillHud(0, s); FixTimer(s); SetRwEngine();
    Mem<uint8>(0xA43088) = (Rnd(s) % 9 == 0) ? 1 : 0;                 // CReplay::Mode (1 = MODE_PLAYBACK)
    Mem<uint8>(0xC8A7C1) = (Rnd(s) % 10 == 0);                        // CWeapon::ms_bTakePhoto
    Mem<uint8>(0xBA677B) = (Rnd(s) % 10 == 0);                        // m_bActivateMenuNextFrame
    Mem<uint32>(0xB6EC40) = (Rnd(s) % 10 == 0) ? Mem<uint32>(0xB7CB4C) : Mem<uint32>(0xB7CB4C) + 1 + Rnd(s) % 3;   // gbCineyCamProcessedOnFrame
    Mem<uint8>(0xB6F065) = (Rnd(s) % 5 == 0);                         // m_bWideScreenOn
    Mem<uint8>(0xBA6769) = (Rnd(s) % 5 != 0);                         // m_bHudOn
    Mem<uint8>(0xA444A0) = (Rnd(s) % 5 != 0);                         // CTheScripts::bDisplayHud
    Mem<uint8>(0xBAA3F9) = (Rnd(s) % 4 == 0);                         // bScriptDontDisplayVehicleName
    Mem<uint8>(0xBAA3FB) = (Rnd(s) % 4 == 0);                         // bScriptDontDisplayRadar
    static constexpr uint8 markers[] = { 0, 1, 1, 1, 2, 0x80 }; Mem<uint8>(0xBAA45C) = markers[Rnd(s) % std::size(markers)];   // m_bDraw3dMarkers (el EXE compara con 1)
    Mem<uint8>(0xBAB229) = uint8(Rnd(s) & 1);                         // HelpTripSkipShown
    Mem<uint8>(0xB7CB49) = (Rnd(s) % 6 == 0);                         // CTimer::m_UserPause
    Mem<uint8>(0xBAACC0) = (Rnd(s) % 3 == 0) ? uint8('A') : uint8(0); // m_BigMessage[0][0]
    static constexpr uint8 menus[] = { 0, 0, 1, 2, 5 }; Mem<uint8>(0xBA82E3) = menus[Rnd(s) % std::size(menus)];   // menus en uso
    Mem<uint8>(0xA44488) = uint8(Rnd(s) & 1);                         // bDrawSubtitlesBeforeFade
    MakePed(s, int32(Rnd(s) % 47), false); fixPed[0] = fixPed[1] = hudPed.p();
    MakeVehicle(s, Rnd(s) % 12, 400 + int32(Rnd(s) % 200), false);
    fixVeh[0] = fixVeh[1] = (Rnd(s) % 4 == 0) ? hudVeh.p() : nullptr;
    fixSkipCan = Rnd(s) & 1; fixVital = Rnd(s) & 1;
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        return json{ {"hud", HudSnap()} };
    });
}

// =====================================================================================================================
// CHud::DrawAfterFade (0x58D490)
// =====================================================================================================================
inline json CaseAfterFade(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x6002); FillHud(0, s); FixTimer(s); SetRwEngine();
    Mem<uint8>(0xB7CB49) = (Rnd(s) % 6 == 0);                         // pausa del usuario
    Mem<uint8>(0xA43088) = (Rnd(s) % 9 == 0) ? 1 : 0;
    Mem<uint8>(0xC8A7C1) = (Rnd(s) % 10 == 0);
    Mem<uint8>(0xB5F852) = (Rnd(s) % 4 == 0);                         // CCutsceneMgr::ms_cutsceneProcessing
    Mem<uint8>(0xBA67A4) = (Rnd(s) % 4 == 0);                         // m_bMenuActive
    Mem<uint8>(0xB6F065) = (Rnd(s) % 4 == 0);                         // m_bWideScreenOn
    Mem<uint8>(0xBAA3F8) = (Rnd(s) % 4 == 0);                         // bScriptDontDisplayAreaName
    Mem<uint8>(0xBAACC0) = (Rnd(s) % 3 == 0) ? uint8('A') : uint8(0);
    Mem<uint8>(0xA44488) = uint8(Rnd(s) & 1);
    MakePed(s, int32(Rnd(s) % 47), false); fixPed[0] = fixPed[1] = hudPed.p();
    static constexpr uint32 subtypes[] = { 0, 1, 2, 3, 4, 5, 6, 9 };
    MakeVehicle(s, subtypes[Rnd(s) % std::size(subtypes)], 400 + int32(Rnd(s) % 200), false);
    fixVeh[0] = fixVeh[1] = (Rnd(s) % 3 != 0) ? hudVeh.p() : nullptr;
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        return json{ {"hud", HudSnap()} };
    });
}

// =====================================================================================================================
// CHud::DrawRadar (0x58A330)
// =====================================================================================================================
inline json CaseRadar(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x6003); FillHud(0, s); FixScreen(s); FixTimer(s); SetRwEngine();
    static constexpr int32 exits[] = { 0, 0, 0, 1, 2, 3, 4 }; Mem<int32>(0x96A7CC) = exits[Rnd(s) % std::size(exits)];
    static constexpr int32 modes[] = { 0, 0, 0, 1, 2 }; Mem<int32>(0xBA676C) = modes[Rnd(s) % std::size(modes)];
    static constexpr int16 flashes[] = { -1, 8, 8, 3, 0, 5 }; Mem<int16>(0xBAB1DC) = flashes[Rnd(s) % std::size(flashes)];
    // vehiculo del jugador: ninguno, avion (subtipo 4), helicoptero (3), coche (0), barca (5) o Vortex (modelo 539)
    const uint32 kind = Rnd(s) % 9;
    static constexpr float heights[] = { 0.f, 100.f, 199.99f, 200.f, 200.01f, 500.f, -5.f, 20.5f, 1000.f };
    switch (kind) {
    case 0: case 1: fixVeh[0] = fixVeh[1] = nullptr; break;
    case 2: case 3: MakeVehicle(s, 4, 460 + int32(Rnd(s) % 40), true); fixVeh[0] = fixVeh[1] = hudVeh.p(); break;          // avion con matriz
    case 4: MakeVehicle(s, 3, 417 + int32(Rnd(s) % 10), (Rnd(s) & 1) != 0); fixVeh[0] = fixVeh[1] = hudVeh.p(); break;     // helicoptero
    case 5: MakeVehicle(s, (Rnd(s) & 1) ? 4u : 3u, 539, (Rnd(s) & 1) != 0); fixVeh[0] = fixVeh[1] = hudVeh.p(); break;      // Vortex
    case 6: MakeVehicle(s, 0, 400 + int32(Rnd(s) % 50), (Rnd(s) & 1) != 0); fixVeh[0] = fixVeh[1] = hudVeh.p(); break;     // coche
    default: MakeVehicle(s, 5, 472, (Rnd(s) & 1) != 0); fixVeh[0] = fixVeh[1] = hudVeh.p(); break;                          // barca
    }
    if (fixVeh[0] && At<uint32>(hudVeh.p(), 0x14)) At<float>(hudMat.p(), 0x38) = heights[Rnd(s) % std::size(heights)];       // z de la matriz
    else if (fixVeh[0]) At<float>(hudVeh.p(), 0x0C) = heights[Rnd(s) % std::size(heights)];                                // z de la transformacion simple
    static constexpr int32 weapons[] = { 46, 46, 0, 22, 30, 41, 16 };
    MakePed(s, weapons[Rnd(s) % std::size(weapons)], (Rnd(s) & 1) != 0);
    if (At<uint32>(hudPed.p(), 0x14)) At<float>(hudMat2.p(), 0x38) = heights[Rnd(s) % std::size(heights)]; else At<float>(hudPed.p(), 0x0C) = heights[Rnd(s) % std::size(heights)];
    fixPed[0] = fixPed[1] = hudPed.p();
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        return json{ {"hud", HudSnap()} };
    });
}

// =====================================================================================================================
// CHud::DrawWeaponIcon (0x58D7D0): usa modelos de arma y ranuras de TXD reales (solo lectura); el diccionario RW de la ranura se sustituye
// temporalmente por uno falso (la zona de cada TxdDef usado se guarda y se restaura) y la busqueda de textura esta doblada.
// =====================================================================================================================
struct IconWeapon { int32 type; int32 model; uintptr_t def; };
inline std::vector<IconWeapon> iconWeapons;          // armas con modelo > 0 cuya ranura de TXD esta en uso (se rellena al empezar)
inline Buf<64> fakeDict, fakeTexture, fakeRaster;
inline json CaseWeaponIcon(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x6004); FillHud(0, s); FixScreen(s); SetRwEngine();
    fakeDict.Fill(0xCD); fakeTexture.Fill(0xCD); fakeRaster.Fill(0xCD);
    int32 type = 0;
    if (!iconWeapons.empty() && Rnd(s) % 5 != 0) {
        const auto& w = iconWeapons[Rnd(s) % iconWeapons.size()]; type = w.type;
        Mem<uint32>(w.def) = (Rnd(s) % 4 == 0) ? 0u : P(fakeDict.p());        // TxdDef.m_pRwDictionary
    }
    At<uint32>(fakeTexture.p(), 0) = P(fakeRaster.p());
    fixTex = (Rnd(s) % 4 == 0) ? nullptr : fakeTexture.p();
    MakePed(s, type, false); fixPed[0] = fixPed[1] = hudPed.p();
    // el EXE no usa la ranura 0 si el tipo es 0: la ranura de arma activa vale lo que diga MakePed
    const int32 x = int32(Rnd(s) % 1920), y = int32(Rnd(s) % 1080);
    static constexpr float alphas[] = { 0.f, 100.5f, 255.f, 300.7f, 17.9f, 254.99f, -4.5f };
    const float alpha = alphas[Rnd(s) % std::size(alphas)];
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(void*, int32, int32, float)>(addr)(hudPed.p(), x, y, alpha);
        return json{ {"hud", HudSnap()} };
    });
}

// =====================================================================================================================
// CHud::Initialise (0x5BA850) / CHud::Shutdown (0x588850)
// =====================================================================================================================
inline json CaseInitialise(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x6005); FillHud(I % 2 ? 0xA5 : -1, s);
    for (int k = 0; k < 8; ++k) Mem<uint8>(0x8D0934 + k) = uint8(Rnd(s));
    const uint8 focus = uint8(Rnd(s) % 2); Mem<uint8>(PLAYER_FOCUS) = focus;
    Mem<uint32>(PLAYERS + focus * 0x190 + 0xBC) = Rnd(s); Mem<uint32>(PLAYERS + focus * 0x190 + 0xE4) = Rnd(s);
    for (int k = 0; k < 6; ++k) Mem<uint32>(SPRITES + 4 * k) = Rnd(s);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        return json{ {"hud", HudSnap()} };
    });
}
inline json CaseShutdown(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x6006); FillHud(-1, s);
    for (int k = 0; k < 6; ++k) Mem<uint32>(SPRITES + 4 * k) = (Rnd(s) & 1) ? Rnd(s) : 0u;
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        return json{ {"hud", HudSnap()} };
    });
}

// ---------------- registro de colaboradores de CHud ----------------
inline void RegisterHudSites() {
    using std::bit_cast;
    AddSite("FindPlayerPed", 0x56E210, bit_cast<uintptr_t>(&FindPlayerPed), (&SFindPed));
    AddSite("FindPlayerVehicle", 0x56E0D0, bit_cast<uintptr_t>(&FindPlayerVehicle), (&SFindVeh));
    AddSite("CHud::DrawCrossHairs", 0x58E020, bit_cast<uintptr_t>(&CHud::DrawCrossHairs), (&SV0<T_CROSSHAIRS>));
    AddSite("CHud::DrawPlayerInfo", 0x58EAF0, bit_cast<uintptr_t>(&CHud::DrawPlayerInfo), (&SV0<T_PLAYERINFO>));
    AddSite("CHud::DrawWanted", 0x58D9A0, bit_cast<uintptr_t>(&CHud::DrawWanted), (&SV0<T_WANTED>));
    AddSite("CHud::DrawVehicleName", 0x58AEA0, bit_cast<uintptr_t>(&CHud::DrawVehicleName), (&SV0<T_VEHNAME>));
    AddSite("CHud::DrawMissionTimers", 0x58B180, bit_cast<uintptr_t>(&CHud::DrawMissionTimers), (&SV0<T_MISSIONTIMERS>));
    AddSite("CHud::DrawRadar", 0x58A330, bit_cast<uintptr_t>(&CHud::DrawRadar), (&SV0<T_RADAR>));
    AddSite("CHud::DrawVitalStats", 0x589650, bit_cast<uintptr_t>(&CHud::DrawVitalStats), (&SV0<T_VITAL>));
    AddSite("CHud::DrawTripSkip", 0x58A160, bit_cast<uintptr_t>(&CHud::DrawTripSkip), (&SV0<T_TRIPSKIP>));
    AddSite("CRadar::Draw3dMarkers", 0x585BF0, bit_cast<uintptr_t>(&CRadar::Draw3dMarkers), (&SV0<T_3DMARKERS>));
    AddSite("CMenuSystem::Process", 0x582630, bit_cast<uintptr_t>(&CMenuSystem::Process), (&SV1<T_MENUPROC>));
    AddSite("CHud::DrawScriptText", 0x58C080, bit_cast<uintptr_t>(&CHud::DrawScriptText), (&SV1<T_SCRIPTTEXT>));
    AddSite("CHud::DrawSubtitles", 0x58C250, bit_cast<uintptr_t>(&CHud::DrawSubtitles), (&SV0<T_SUBTITLES>));
    AddSite("CHud::DrawHelpText", 0x58B6E0, bit_cast<uintptr_t>(&CHud::DrawHelpText), (&SV0<T_HELPTEXT>));
    AddSite("CHud::DrawOddJobMessage", 0x58CC80, bit_cast<uintptr_t>(&CHud::DrawOddJobMessage), (&SV1<T_ODDJOB>));
    AddSite("CHud::DrawSuccessFailedMessage", 0x58C6A0, bit_cast<uintptr_t>(&CHud::DrawSuccessFailedMessage), (&SV0<T_SUCCESS>));
    AddSite("CHud::DrawBustedWastedMessage", 0x58CA50, bit_cast<uintptr_t>(&CHud::DrawBustedWastedMessage), (&SV0<T_BUSTED>));
    AddSite("CHud::DrawAreaName", 0x58AA50, bit_cast<uintptr_t>(&CHud::DrawAreaName), (&SV0<T_AREANAME>));
    AddSite("CHud::DrawMissionTitle", 0x58D240, bit_cast<uintptr_t>(&CHud::DrawMissionTitle), (&SV0<T_MISSIONTITLE>));
    AddSite("CHud::SetHelpMessage", 0x588BE0, bit_cast<uintptr_t>(&CHud::SetHelpMessage), (&SSetHelp));
    AddSite("CGameLogic::SkipCanBeActivated", 0x4415C0, bit_cast<uintptr_t>(&CGameLogic::SkipCanBeActivated), (&SSkipCan));
    AddSite("CPad::GetDisplayVitalStats", 0x5408B0, bit_cast<uintptr_t>(&CPad::GetDisplayVitalStats), (&SVitalPad));
    AddSite("CRadar::DrawMap", 0x586B00, bit_cast<uintptr_t>(&CRadar::DrawMap), (&SV0<T_DRAWMAP>));
    AddSite("CRadar::DrawBlips", 0x588050, bit_cast<uintptr_t>(&CRadar::DrawBlips), (&SV0<T_DRAWBLIPS>));
    AddSite("CSprite2d::Draw(9)", 0x728520, bit_cast<uintptr_t>(static_cast<void(CSprite2d::*)(float, float, float, float, float, float, float, float, const CRGBA&)>(&CSprite2d::Draw)), (&SSprite9));
    AddSite("CSprite2d::Draw(rect)", 0x728350, bit_cast<uintptr_t>(static_cast<void(CSprite2d::*)(const CRect&, const CRGBA&)>(&CSprite2d::Draw)), (&SSpriteRect));
    AddSite("CSprite2d::DrawRect", 0x727B60, bit_cast<uintptr_t>(static_cast<void(*)(const CRect&, const CRGBA&)>(&CSprite2d::DrawRect)), (&SDrawRect));
    AddSite("RwTexDictionaryFindHashNamedTexture", 0x734E50, bit_cast<uintptr_t>(&RwTexDictionaryFindHashNamedTexture), (&SFindTex));
    AddSite("CSprite::RenderOneXLUSprite", 0x70D000, bit_cast<uintptr_t>(&CSprite::RenderOneXLUSprite), (&SXluSprite));
    AddSite("CTxdStore::AddTxdSlot", 0x731C80, bit_cast<uintptr_t>(&CTxdStore::AddTxdSlot), (&SAddSlot));
    AddSite("CTxdStore::LoadTxd", 0x7320B0, bit_cast<uintptr_t>(static_cast<bool(*)(int32, const char*)>(&CTxdStore::LoadTxd)), (&SLoadTxd));
    AddSite("CTxdStore::AddRef", 0x731A00, bit_cast<uintptr_t>(&CTxdStore::AddRef), (&SAddRef));
    AddSite("CTxdStore::PushCurrentTxd", 0x7316A0, bit_cast<uintptr_t>(&CTxdStore::PushCurrentTxd), (&SV0<T_PUSHTXD>));
    AddSite("CTxdStore::SetCurrentTxd", 0x7319C0, bit_cast<uintptr_t>(&CTxdStore::SetCurrentTxd), (&SSetTxd));
    AddSite("CTxdStore::PopCurrentTxd", 0x7316B0, bit_cast<uintptr_t>(&CTxdStore::PopCurrentTxd), (&SV0<T_POPTXD>));
    AddSite("CSprite2d::SetTexture", 0x7272B0, bit_cast<uintptr_t>(static_cast<void(CSprite2d::*)(const char*, const char*)>(&CSprite2d::SetTexture)), (&SSetTexture));
    AddSite("CSprite2d::Delete", 0x727240, bit_cast<uintptr_t>(&CSprite2d::Delete), (&SDelete));
    AddSite("CTxdStore::FindTxdSlot", 0x731850, bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)), (&SFindSlot));
    AddSite("CTxdStore::RemoveTxdSlot", 0x731CD0, bit_cast<uintptr_t>(&CTxdStore::RemoveTxdSlot), (&SRemoveSlot));
}

// Patches de cada funcion de CHud (los colaboradores nunca incluyen la funcion probada)
inline std::unique_ptr<Patches> HudPatches(const std::string& name) {
    if (name == "Draw") return Use({ "FindPlayerPed", "FindPlayerVehicle", "CHud::DrawCrossHairs", "CHud::DrawPlayerInfo", "CHud::DrawWanted", "CHud::DrawVehicleName", "CHud::DrawMissionTimers",
        "CHud::DrawRadar", "CHud::DrawVitalStats", "CHud::DrawTripSkip", "CRadar::Draw3dMarkers", "CMenuSystem::Process", "CHud::DrawScriptText", "CHud::DrawSubtitles", "CHud::DrawHelpText",
        "CHud::DrawOddJobMessage", "CHud::DrawSuccessFailedMessage", "CHud::DrawBustedWastedMessage", "CHud::SetHelpMessage", "CGameLogic::SkipCanBeActivated", "CPad::GetDisplayVitalStats" });
    if (name == "DrawAfterFade") return Use({ "FindPlayerVehicle", "CHud::DrawAreaName", "CHud::DrawScriptText", "CHud::DrawSubtitles", "CHud::DrawMissionTitle", "CHud::DrawOddJobMessage" });
    if (name == "DrawRadar") return Use({ "FindPlayerPed", "FindPlayerVehicle", "CRadar::DrawMap", "CRadar::DrawBlips", "CSprite2d::Draw(9)", "CSprite2d::Draw(rect)", "CSprite2d::DrawRect" });
    if (name == "DrawWeaponIcon") return Use({ "RwTexDictionaryFindHashNamedTexture", "CSprite::RenderOneXLUSprite", "CSprite2d::Draw(rect)" });
    if (name == "Initialise") return Use({ "CTxdStore::AddTxdSlot", "CTxdStore::LoadTxd", "CTxdStore::AddRef", "CTxdStore::PushCurrentTxd", "CTxdStore::SetCurrentTxd", "CTxdStore::PopCurrentTxd", "CSprite2d::SetTexture" });
    if (name == "Shutdown") return Use({ "CSprite2d::Delete", "CTxdStore::FindTxdSlot", "CTxdStore::RemoveTxdSlot" });
    return nullptr;
}
}
