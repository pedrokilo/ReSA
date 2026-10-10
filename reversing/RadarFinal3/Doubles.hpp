#pragma once
// Objetos falsos, fixtures compartidos y dobles (stubs) de colaboradores del banco RadarFinal3.
// Cada doble respeta el `ret N` del original (dis.sh) y solo traza si el hilo es el del banco (Mine()).
#include "Common.hpp"

namespace RadarFinal3Live {

// ---------------- motor RW falso ----------------
// RwRenderStateSet / RwIm2DRenderPrimitive pasan por la tabla del motor (0xC97B24): +0x1C zBufferFar, +0x20 fpRenderStateSet, +0x30 fpIm2DRenderPrimitive.
// El EXE llama RwIm2DRenderPrimitive por 0x734E90 (`jmp [eax+30h]`) y la DLL por el macro: ambos acaban en la tabla falsa.
inline uint32 __cdecl SRwState(uint32 state, uint32 value) { if (Mine()) Push(T_RW, { state, value }); return 1; }
inline uint32 __cdecl SIm2D(uint32 prim, void* verts, int32 count) { if (Mine()) Push(T_IM2D, { prim, uint32(count), uint32(verts == CSprite2d::maVertices.data()) }); return 1; }
inline Buf<0x200> rwGlobals;
inline uint32 fixFarZ{};
inline void SetRwEngine(uint32& s) {
    rwGlobals.Fill(0); fixFarZ = Bits(PickIn(s, 0.01f, 1.f));
    At<uint32>(rwGlobals.p(), 0x1C) = fixFarZ;
    At<uint32>(rwGlobals.p(), 0x20) = uint32(reinterpret_cast<uintptr_t>(&SRwState));
    At<uint32>(rwGlobals.p(), 0x30) = uint32(reinterpret_cast<uintptr_t>(&SIm2D));
    Mem<uint32>(0xC97B24) = P(rwGlobals.p());
}

// ---------------- jugadores / vehiculos / objetos falsos ----------------
inline Buf<0x800> fakePed[2]; inline Buf<0xA20> fakeVeh[2]; inline Buf<0x60> fakeMat[4];
inline void* fixPed[2]{}; inline void* fixVeh[2]{};
inline void* realPed{}; inline void* realVeh{};      // los hilos ajenos reciben los valores reales capturados al empezar
inline void* __cdecl SFindPed(int32 id) { if (!Mine()) return realPed; return fixPed[(id < 0 ? Mem<uint8>(PLAYER_FOCUS) : uint32(id)) & 1]; }
inline void* __cdecl SFindVeh(int32 id, uint32) { if (!Mine()) return realVeh; return fixVeh[(id < 0 ? Mem<uint8>(PLAYER_FOCUS) : uint32(id)) & 1]; }
inline CVector fixCentre[2]; inline float fixHeading[2]{}; inline CVector fixSpeed;
// FindPlayerCentreOfWorldForMap(playerId): CVector por puntero oculto (el EXE y la DLL lo devuelven igual). Se traza el id.
inline CVector* __cdecl SCentre(CVector* out, int32 id) { if (!Mine()) { *out = CVector{}; return out; } Push(T_CENTRE, { uint32(id) }); *out = fixCentre[id & 1]; return out; }
inline float __cdecl SHeading(int32 id) { if (!Mine()) return 0.f; Push(T_HEADING, { uint32(id) }); return fixHeading[id & 1]; }
inline CVector* __cdecl SSpeed(int32 id) { if (!Mine()) return &fixSpeed; Push(T_SPEED, { uint32(id) }); return &fixSpeed; }
inline uint32 fixCoop{}, fixMission{}, fixRemote{};
inline uint32 __cdecl SCoop() { if (!Mine()) return 0; Push(T_COOP); return fixCoop; }
inline uint32 __cdecl SMission() { if (!Mine()) return 0; Push(T_ONMISSION); return fixMission; }
// CPlayerInfo::IsPlayerInRemoteMode (miembro): se traza el indice de jugador
inline uint32 __fastcall SRemote(void* self, void*) { if (!Mine()) return 0; Push(T_REMOTE, { (P(self) - uint32(PLAYERS)) / 0x190 }); return fixRemote; }
// CPlayerPed::IsHidden (miembro): se traza el jugador (por el puntero del ped falso)
inline uint32 fixHidden[2]{};
inline uint32 __fastcall SIsHidden(void* self, void*) {
    if (!Mine()) return 0; const uint32 who = self == fakePed[0].p() ? 0u : self == fakePed[1].p() ? 1u : 2u;
    Push(T_ISHIDDEN, { who }); return who < 2 ? fixHidden[who] : 0u;
}

// ---------------- dobles de dibujo ----------------
inline bool traceUv{};
inline uint32 SpriteIdx(const void* self) { return (P(self) - 0xBAA250u) / 4; }
// CSprite2d::Draw(const CRect&, const CRGBA&) (miembro, 2 argumentos: ret 8)
inline void __fastcall SSpriteRect(void* self, void*, const uint32* rect, const uint32* color) { if (Mine()) Push(T_SPRITERECT, { SpriteIdx(self), rect[0], rect[1], rect[2], rect[3], *color }); }
// CSprite2d::Draw(x1..y4, const CRGBA&) (miembro, 9 argumentos: ret 0x24)
inline void __fastcall SSprite9(void* self, void*, uint32 x1, uint32 y1, uint32 x2, uint32 y2, uint32 x3, uint32 y3, uint32 x4, uint32 y4, const uint32* color) {
    if (Mine()) Push(T_SPRITE9, { SpriteIdx(self), x1, y1, x2, y2, x3, y3, x4, y4, *color });
}
// CSprite2d::DrawRect(const CRect&, const CRGBA&) (estatica, cdecl)
inline void __cdecl SDrawRect(const uint32* rect, const uint32* color) { if (Mine()) Push(T_DRAWRECT, { rect[0], rect[1], rect[2], rect[3], *color }); }
// CSprite2d::Draw2DPolygon(x1..y4, const CRGBA&) (estatica, cdecl, 9 argumentos)
inline void __cdecl SPoly(uint32 x1, uint32 y1, uint32 x2, uint32 y2, uint32 x3, uint32 y3, uint32 x4, uint32 y4, const uint32* color) {
    if (Mine()) Push(T_POLY, { x1, y1, x2, y2, x3, y3, x4, y4, *color });
}
// CSprite2d::SetVertices(int, const CVector2D*, const CVector2D*, const CRGBA&) (estatica, cdecl). Las coordenadas de textura solo se trazan si traceUv.
inline void __cdecl SSetVerts(int32 n, const float* pos, const float* uv, const uint32* color) {
    if (!Mine()) return;
    const uint32 cnt = (n > 0 && n <= 8) ? uint32(n) : 0u;
    // Defensa: si algun puntero no es legible se traza una marca en vez de caer (el diagnostico queda en la traza)
    if (IsBadReadPtr(pos, cnt * 8 + 1) || IsBadReadPtr(color, 4) || (traceUv && IsBadReadPtr(uv, cnt * 8 + 1))) { Push(T_SETVERTS, { uint32(n), 0xBAD0BAD0u, P(pos), P(uv), P(color) }); return; }
    Push(T_SETVERTS, { uint32(n), *color, uint32(Fnv(pos, cnt * 8)), traceUv ? uint32(Fnv(uv, cnt * 8)) : 0u, cnt > 0 ? Bits(pos[0]) : 0u, cnt > 0 ? Bits(pos[1]) : 0u, cnt > 1 ? Bits(pos[2]) : 0u, cnt > 1 ? Bits(pos[3]) : 0u });
}
// CSprite2d::SetVertices(const CRect&, c1, c2, c3, c4) (estatica, cdecl, 5 punteros)
inline void __cdecl SSetVertsRect(const uint32* rect, const uint32* c1, const uint32* c2, const uint32* c3, const uint32* c4) {
    if (Mine()) Push(T_SETVERTSRECT, { rect[0], rect[1], rect[2], rect[3], *c1, *c2, *c3, *c4 });
}
// CSprite2d::SetMaskVertices(int, const CVector2D*, float) (estatica, cdecl)
inline void __cdecl SSetMask(int32 n, const float* pos, uint32 depth) {
    if (!Mine()) return; const uint32 cnt = (n > 0 && n <= 8) ? uint32(n) : 0u;
    Push(T_SETMASK, { uint32(n), depth, uint32(Fnv(pos, cnt * 8)), cnt > 0 ? Bits(pos[0]) : 0u, cnt > 0 ? Bits(pos[1]) : 0u, cnt > 1 ? Bits(pos[2]) : 0u, cnt > 1 ? Bits(pos[3]) : 0u });
}
// CFont::PrintString(float, float, const GxtChar*) (cdecl)
inline void __cdecl SPrint(uint32 x, uint32 y, const void* text) { if (Mine()) Push(T_PRINT, { x, y, TextHash(text) }); }
// CText::Get(const char*) (miembro): devuelve un texto falso estable segun la clave
inline char fakeTexts[16][12];
inline const void* __fastcall STextGet(void*, void*, const char* key) {
    if (!Mine()) return fakeTexts[0]; const uint32 h = TextHash(key); Push(T_TEXTGET, { h }); return fakeTexts[h % 16];
}
inline void InitFakeTexts() { for (int i = 0; i < 16; ++i) std::snprintf(fakeTexts[i], sizeof(fakeTexts[i]), "TXT_%02d", i); }

// ---------------- dobles de streaming / TXD / sprites ----------------
inline void __cdecl SReqModel(int32 model, int32 flags) { if (Mine()) Push(T_REQMODEL, { uint32(model), uint32(flags) }); }
inline void __cdecl SRemModel(int32 model) { if (Mine()) Push(T_REMMODEL, { uint32(model) }); }
inline void __cdecl SPushTxd() { if (Mine()) Push(T_PUSHTXD); }
inline void __cdecl SSetTxd(int32 slot) { if (Mine()) Push(T_SETTXD, { uint32(slot) }); }
inline void __cdecl SPopTxd() { if (Mine()) Push(T_POPTXD); }
inline int32 __cdecl SFindSlot(const char* name) {
    if (!Mine()) return -1; const uint32 h = TextHash(name); Push(T_FINDSLOT, { h });
    return (h % 9 == 0) ? -1 : int32(100 + h % 900);
}
// CSprite2d::SetTexture(const char*) (miembro, 1 argumento: ret 4)
inline void __fastcall SSetTex1(void* self, void*, const void* name) { if (Mine()) Push(T_SETTEX, { SpriteIdx(self), TextHash(name) }); }
inline void __fastcall SDelete(void* self, void*) { if (Mine()) Push(T_DELETE, { SpriteIdx(self) }); }
inline Buf<0x40> fakeTexture, fakeRaster, fakeDict[4];
inline void* fixFirstTex{};
inline void* __cdecl SGetFirstTex(void* dict) { if (!Mine()) return nullptr; Push(T_FIRSTTEX, { dict ? uint32(dict == fakeDict[0].p() ? 1 : dict == fakeDict[1].p() ? 2 : dict == fakeDict[2].p() ? 3 : dict == fakeDict[3].p() ? 4 : 9) : 0u }); return fixFirstTex; }

// ---------------- pools falsos (TXD, EntryExit, vehiculos, peds, objetos) ----------------
// CPool: +0 almacenamiento, +4 estados de ranura (bit 7 = libre), +8 capacidad, +0xC ultima libre, +0x10/+0x11 banderas.
inline void InitPool(Buf<0x20>& pool, uint8* storage, uint8* states, int32 cap) {
    pool.Fill(0); At<uint32>(pool.p(), 0) = P(storage); At<uint32>(pool.p(), 4) = P(states); At<int32>(pool.p(), 8) = cap; At<int32>(pool.p(), 0xC) = -1;
}
inline Buf<0x20> poolTxd, poolEnex, poolVeh, poolPed, poolObj;
inline Buf<8 * 12> storeTxd; inline Buf<8> statesTxd;
inline Buf<8 * 0x3C> storeEnex; inline Buf<8> statesEnex;
inline Buf<8 * 0xA18> storeVeh; inline Buf<8> statesVeh;
inline Buf<8 * 0x7C4> storePed; inline Buf<8> statesPed;
inline Buf<8 * 0x19C> storeObj; inline Buf<8> statesObj;
inline Buf<0x100> fakeModelInfo[3], fakeColModel[3];
inline void InitPools() {
    InitPool(poolTxd, storeTxd.p(), statesTxd.p(), 8); InitPool(poolEnex, storeEnex.p(), statesEnex.p(), 8);
    InitPool(poolVeh, storeVeh.p(), statesVeh.p(), 8); InitPool(poolPed, storePed.p(), statesPed.p(), 8); InitPool(poolObj, storeObj.p(), statesObj.p(), 8);
}
inline uint32 EnexIdx(const void* e) { const auto a = uintptr_t(e), b = uintptr_t(storeEnex.p()); return (a >= b && a < b + 8 * 0x3C) ? uint32((a - b) / 0x3C) : 0xFFFFu; }
// CEntryExit::GetPositionRelativeToOutsideWorld(CVector&) (miembro, 1 argumento: ret 4): desplaza la posicion segun la entrada
inline void __fastcall SEnexPos(void* self, void*, float* v) {
    if (!Mine()) return; const uint32 idx = EnexIdx(self);
    Push(T_ENEXPOS, { idx, Bits(v[0]), Bits(v[1]), Bits(v[2]) });
    v[0] += float(idx + 1) * 3.5f; v[1] -= float(idx + 1) * 1.25f; v[2] += 10.f;
}
// CTheZones::GetCurrentZoneLockedOrUnlocked(CVector2D) (estatica; dos floats por pila)
inline uint32 fixZoneLock{};
inline uint32 __cdecl SZoneLock(uint32 x, uint32 y) { if (!Mine()) return 0; Push(T_ZONELOCK, { x, y }); return fixZoneLock; }
// CGangWars::CanPlayerStartAGangWarHere(CZoneInfo*)
inline uint32 fixGangMask{};
inline uint32 __cdecl SGangCan(const void* info) {
    if (!Mine()) return 0; const uint32 idx = uint32((uintptr_t(info) - 0xBA1DF0u) / 0x11u); Push(T_GANGCAN, { idx }); return (fixGangMask >> (idx & 31)) & 1u;
}

// ---------------- marcadores 3D y lineas de depuracion ----------------
// C3dMarkers::PlaceMarkerCone(id, CVector&, size, r, g, b, a, pulsePeriod, pulseFraction, rotateRate, bEnableCollision) (cdecl, 11 argumentos)
inline void __cdecl SCone(uint32 id, const float* pos, uint32 size, uint32 r, uint32 g, uint32 b, uint32 a, uint32 period, uint32 frac, uint32 rotate, uint32 coll) {
    if (Mine()) Push(T_CONE, { id, Bits(pos[0]), Bits(pos[1]), Bits(pos[2]), size, r & 0xFF, g & 0xFF, b & 0xFF, a & 0xFF, period & 0xFFFF, frac, rotate & 0xFFFF, coll & 0xFF });
}
// C3dMarkers::PlaceMarkerSet(id, type, CVector&, size, r, g, b, a, pulsePeriod, pulseFraction, rotateRate) (cdecl, 11 argumentos)
inline void __cdecl SMarkerSet(uint32 id, uint32 type, const float* pos, uint32 size, uint32 r, uint32 g, uint32 b, uint32 a, uint32 period, uint32 frac, uint32 rotate) {
    if (Mine()) Push(T_MARKERSET, { id, type & 0xFF, Bits(pos[0]), Bits(pos[1]), Bits(pos[2]), size, r & 0xFF, g & 0xFF, b & 0xFF, a & 0xFF, period & 0xFFFF, frac, rotate & 0xFFFF });
}
// CTheScripts::ScriptDebugLine3D: el EXE recibe ocho dwords por valor (x1,y1,z1,x2,y2,z2,c1,c2); la DLL declara (const CVector&, const CVector&, uint32, uint32)
inline void __cdecl SDebugLineExe(uint32 x1, uint32 y1, uint32 z1, uint32 x2, uint32 y2, uint32 z2, uint32 c1, uint32 c2) { if (Mine()) Push(T_DEBUGLINE, { x1, y1, z1, x2, y2, z2, c1, c2 }); }
inline void __cdecl SDebugLineRef(const float* a, const float* b, uint32 c1, uint32 c2) { if (Mine()) Push(T_DEBUGLINE, { Bits(a[0]), Bits(a[1]), Bits(a[2]), Bits(b[0]), Bits(b[1]), Bits(b[2]), c1, c2 }); }

// ---------------- dobles de las funciones de CRadar usadas como colaboradores ----------------
template<uint32 TAG> inline void __cdecl SV0() { if (Mine()) Push(TAG); }
template<uint32 TAG> inline void __cdecl SV2(int32 a, int32 b) { if (Mine()) Push(TAG, { uint32(a), uint32(b) }); }
inline void __cdecl SDrawEntity(int32 index, uint32 isSprite) { if (Mine()) Push(T_DRAWENTITY, { uint32(index), isSprite & 0xFF }); }
inline void __cdecl SDrawCoord(int32 index, uint32 isSprite) { if (Mine()) Push(T_DRAWCOORD, { uint32(index), isSprite & 0xFF }); }
inline void __cdecl SDrawSprite(int32 sprite, uint32 x, uint32 y, uint32 alpha) { if (Mine()) Push(T_DRAWSPRITE, { uint32(sprite) & 0xFFFF, x, y, alpha & 0xFF }); }
inline void __cdecl SDrawRot(void* sprite, uint32 x, uint32 y, uint32 angle, uint32 w, uint32 h, uint32 color) { if (Mine()) Push(T_DRAWROT, { SpriteIdx(sprite), x, y, angle, w, h, color }); }
inline void __cdecl SYouAre(uint32 x, uint32 y) { if (Mine()) Push(T_YOUARE, { x, y }); }
inline void __cdecl SLegendAdd(uint32 noSprite, int32 index) { if (Mine()) Push(T_LEGENDADD, { noSprite & 0xFF, uint32(index) }); }
inline void __cdecl STraceHeight(uint32 x, uint32 y, uint32 size, uint32 r, uint32 g, uint32 b, uint32 a, uint32 height) { if (Mine()) Push(T_TRACEHEIGHT, { x, y, size, r & 0xFF, g & 0xFF, b & 0xFF, a & 0xFF, height & 0xFF }); }
inline void __cdecl SMarkerDbg(uint32 x, uint32 y, uint32 z, uint32 color, uint32 radius) { if (Mine()) Push(T_MARKER, { x, y, z, color, radius }); }
inline void __cdecl SAreaOnRadar(const uint32* rect, const uint32* color, uint32 inMenu) { if (Mine()) Push(T_AREA, { rect[0], rect[1], rect[2], rect[3], *color, inMenu & 0xFF }); }
inline void __cdecl SSection(int32 x, int32 y) { if (Mine()) Push(T_SECTION, { uint32(x), uint32(y) }); }
inline void __cdecl SGangOverlay(uint32 inMenu) { if (Mine()) Push(T_GANGOVERLAY, { inMenu & 0xFF }); }
inline void __cdecl SMask() { if (Mine()) Push(T_MASK); }
inline void __cdecl SStreamXY(int32 x, int32 y) { if (Mine()) Push(T_STREAMXY, { uint32(x), uint32(y) }); }
inline void __cdecl SCalcSinCos() { if (Mine()) Push(T_CALCSINCOS); }
inline void __cdecl SDrawMapDbg() { if (Mine()) Push(T_DRAWMAP); }
inline void __cdecl SDrawRadarMapDbg() { if (Mine()) Push(T_DRAWRADARMAP); }
inline void __cdecl SSetupAirstrip() { if (Mine()) Push(T_SETUPAIR); }

// ---------------- trabajo de guardado ----------------
// CGenericGameStorage::SaveDataToWorkBuffer(void*, int32) / LoadDataFromWorkBuffer(void*, int32) (estaticas, cdecl)
inline uint8 fixLoadData[175][0x28]; inline uint32 loadCursor{};
inline uint32 __cdecl SSaveWB(const void* data, int32 size) { if (Mine()) Push(T_SAVEWB, { uint32(size), uint32(Fnv(data, size_t(size))), data ? *reinterpret_cast<const uint32*>(data) : 0u, data ? *(reinterpret_cast<const uint32*>(data) + 8) : 0u }); return 1; }
inline uint32 __cdecl SLoadWB(void* data, int32 size) {
    if (!Mine()) return 1; Push(T_LOADWB, { uint32(size) });
    std::memcpy(data, fixLoadData[loadCursor % 175], size_t(size)); ++loadCursor; return 1;
}

// ---------------- matrices / normalizar ----------------
inline Buf<0x60> allocMatrixBuf;
// CPlaceable::AllocateMatrix(): el doble enlaza la matriz preparada por el fixture (el original toma una de la lista global)
inline void __fastcall SAllocMatrix(void* self, void*) { if (!Mine()) return; Push(T_ALLOCMATRIX); At<uint32>(static_cast<uint8*>(self), 0x14) = P(allocMatrixBuf.p()); }
// CSimpleTransform::UpdateMatrix(CMatrix*) (miembro, 1 argumento)
inline void __fastcall SUpdateMatrix(void*, void*, void* out) { if (Mine()) Push(T_UPDATEMATRIX, { uint32(out == allocMatrixBuf.p()) }); }
// CVector::Normalise() (miembro, sin argumentos): formula fija (el mismo doble en ambos lados)
inline void __fastcall SNormalise(float* v, void*) {
    if (!Mine()) return; Push(T_NORMALISE, { Bits(v[0]), Bits(v[1]), Bits(v[2]) });
    const float mag = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (mag > 0.f) { const float inv = 1.f / mag; v[0] *= inv; v[1] *= inv; v[2] *= inv; }
}

// ---------------- fixtures compartidos ----------------
inline void FixScreen(uint32& s) {
    static constexpr int32 w[] = { 640, 800, 1024, 1280, 1366, 1600, 1920, 1921, 1680, 2560, 1360, 1440 }, h[] = { 480, 600, 768, 720, 768, 900, 1080, 1081, 1050, 1440, 768, 900 };
    const auto k = Rnd(s) % std::size(w); Mem<int32>(0xC17044) = w[k]; Mem<int32>(0xC17048) = h[k];
}
inline void FixTimer(uint32& s) {
    Mem<float>(0xB7CB5C) = 1.f;
    Mem<uint32>(0xB7CB4C) = Rnd(s) % 64; Mem<uint32>(0xB7CB84) = 5000u + Rnd(s) % 600000u; Mem<uint32>(0xB7CB7C) = 5000u + Rnd(s) % 600000u; Mem<uint8>(0xB7CB49) = 0;
}
// FrontEndMenuManager (direcciones del EXE; el evento `layout` comprueba que la DLL usa las mismas): map: 0 = no, 1 = si, otro = aleatorio
inline void FixFrontEnd(uint32& s, int map) {
    Mem<uint8>(0xBA67A1) = map == 0 ? 0 : map == 1 ? 1 : uint8(Rnd(s) & 1);
    static constexpr float zooms[] = { 140.f, 200.f, 255.5f, 300.f, 140.7f, 70.f, 512.3f, 90.9f, 0.f, 33000.5f };
    Mem<float>(0xBA67AC) = zooms[Rnd(s) % std::size(zooms)];
    Mem<float>(0xBA67B0) = PickIn(s, 0.f, 640.f); Mem<float>(0xBA67B4) = PickIn(s, 0.f, 448.f);
    Mem<float>(0xBA67B8) = PickIn(s, -3000.f, 3000.f); Mem<float>(0xBA67BC) = PickIn(s, -3000.f, 3000.f);
    Mem<uint8>(0xBA67C0) = uint8(Rnd(s) % 5 != 0);
    for (uint32 k = 0xBA678D; k <= 0xBA6791; ++k) Mem<uint8>(k) = uint8(Rnd(s) % 3 != 0);
    Mem<int32>(0xBA676C) = int32(Rnd(s) % 3);
    Mem<uint8>(0xBA6800) = uint8(Rnd(s) & 1);
    Mem<int32>(0xBA6804) = int32(Rnd(s) % uint32(Mem<int32>(0xC17044) + 20)); Mem<int32>(0xBA6808) = int32(Rnd(s) % uint32(Mem<int32>(0xC17048) + 20));
}
// Radar: rango, origen, orientacion y seno/coseno guardados
inline void FixRadar(uint32& s) {
    Mem<float>(0xBA8314) = (Rnd(s) % 4 == 0) ? 180.f + 170.f * float(Rnd(s) % 3) / 2.f : PickIn(s, 90.f, 2990.f);
    Mem<float>(0xBAA248) = PickIn(s, -3000.f, 3000.f); Mem<float>(0xBAA24C) = PickIn(s, -3000.f, 3000.f);
    const float a = PickIn(s, -3.15f, 3.15f);
    Mem<float>(0xBA8310) = a; Mem<float>(0xBA830C) = std::sin(a); Mem<float>(0xBA8308) = std::cos(a);
}
// Escribe un vector de 3 floats en un objeto falso
inline void SetVec3(uint8* base, size_t off, float x, float y, float z) { At<float>(base, off) = x; At<float>(base, off + 4) = y; At<float>(base, off + 8) = z; }
// Entrada de la tabla de blips (0x28 bytes): relleno aleatorio y campos dentro del dominio del juego
inline void RandTrace(uint8* t, uint32& s, bool allowEnex) {
    for (int i = 0; i < 0x28; ++i) t[i] = uint8(Rnd(s) >> 11);
    At<uint32>(t, 0) = (Rnd(s) % 12 == 0) ? Rnd(s) % 20u : Rnd(s) % 9u;                 // m_nColour (a veces fuera de 0..8)
    At<uint32>(t, 4) = Rnd(s) % 0x1000u;                                                 // m_nEntityHandle
    SetVec3(t, 8, PickIn(s, -3000.f, 3000.f), PickIn(s, -3000.f, 3000.f), PickIn(s, -100.f, 300.f));
    const uint32 c = Rnd(s) % 8;
    At<uint16>(t, 0x14) = uint16(c == 0 ? 0xFFFEu + Rnd(s) % 2u : c == 1 ? Rnd(s) % 3u : 1u + Rnd(s) % 60000u);   // m_nCounter (cerca de los limites a veces)
    At<float>(t, 0x18) = PickIn(s, 0.2f, 7.f);
    At<uint16>(t, 0x1C) = uint16(Rnd(s) % 12);
    At<uint32>(t, 0x20) = (allowEnex && Rnd(s) % 4 == 0) ? P(storeEnex.p() + 0x3C * (Rnd(s) % 6)) : 0u;
    At<int8>(t, 0x24) = int8((Rnd(s) % 10 == 0) ? 41 : Rnd(s) % 64);                  // m_nBlipSprite
    uint8 f1 = uint8(Rnd(s)); f1 = (Rnd(s) % 100 < 60) ? uint8(f1 | 2) : uint8(f1 & ~2); f1 = uint8((f1 & 0x3F) | ((Rnd(s) % 3) << 6)); At<uint8>(t, 0x25) = f1;   // seguimiento 60 %, aspecto de coordenada 0..2
    const uint32 type = (Rnd(s) % 12 == 0) ? 9u + Rnd(s) % 7u : Rnd(s) % 9u;           // tipos 0..8 (a veces fuera de rango)
    At<uint8>(t, 0x26) = uint8((Rnd(s) % 4) | (type << 2) | ((Rnd(s) % 3) << 6));
}
inline void FixTraces(uint32& s, bool allowEnex) {
    auto* base = reinterpret_cast<uint8*>(0xBA86F0);
    for (int i = 0; i < 175; ++i) RandTrace(base + i * 0x28, s, allowEnex);
}
inline void RandMatrix(uint8* m, uint32& s) {
    std::memset(m, 0, 0x48);
    for (size_t off : { 0x00u, 0x04u, 0x08u, 0x10u, 0x14u, 0x18u, 0x20u, 0x24u, 0x28u }) At<float>(m, off) = PickUnit(s);
    SetVec3(m, 0x30, PickIn(s, -3000.f, 3000.f), PickIn(s, -3000.f, 3000.f), PickIn(s, -50.f, 400.f));
}
// Vehiculo falso: subtipo (+0x594), modelo (+0x22), posicion (+4), matriz opcional (+0x14), tipo de entidad (+0x36 = vehiculo) y sin colision especial (+0x48B = -1)
inline void MakeVehicle(Buf<0xA20>& v, Buf<0x60>& mat, uint32& s, uint32 subtype, int32 model, bool withMatrix) {
    v.Fill(0); uint8* p = v.p();
    At<uint32>(p, 0x594) = subtype; At<int16>(p, 0x22) = int16(model); At<uint8>(p, 0x36) = 2; At<int8>(p, 0x48B) = -1;
    SetVec3(p, 0x04, PickIn(s, -3000.f, 3000.f), PickIn(s, -3000.f, 3000.f), PickIn(s, -50.f, 400.f));
    if (withMatrix) { RandMatrix(mat.p(), s); At<uint32>(p, 0x14) = P(mat.p()); }
}
// Ped falso: area (+0x2F), posicion (+4), matriz opcional (+0x14)
inline void MakePed(Buf<0x800>& ped, Buf<0x60>& mat, uint32& s, uint32 area, bool withMatrix) {
    ped.Fill(0); uint8* p = ped.p();
    At<uint8>(p, 0x2F) = uint8(area);
    SetVec3(p, 0x04, PickIn(s, -3000.f, 3000.f), PickIn(s, -3000.f, 3000.f), PickIn(s, -50.f, 400.f));
    if (withMatrix) { RandMatrix(mat.p(), s); At<uint32>(p, 0x14) = P(mat.p()); }
}
// Camara: modo y direccion de la camara activa, entidad objetivo falsa, matriz/rumbo de TheCamera y vectores de la matriz de camara
inline Buf<0x200> camTarget; inline Buf<0x60> camTargetMat, camMat;
inline void FixCamera(uint32& s, bool mapMode) {
    auto& cam = TheCamera;
    cam.m_nActiveCam = uint8(Rnd(s) % 3);
    auto& ac = cam.m_aCams[cam.m_nActiveCam];
    static constexpr eCamMode modes[] = { MODE_1STPERSON, MODE_FOLLOWPED, MODE_CAM_ON_A_STRING, MODE_BEHINDBOAT, MODE_TOPDOWN, MODE_1STPERSON };
    ac.m_nMode = modes[Rnd(s) % std::size(modes)];
    ac.m_nDirectionWasLooking = Rnd(s) % 4;
    camTarget.Fill(0); camTargetMat.Fill(0);
    uint8* t = camTarget.p();
    SetVec3(t, 4, PickIn(s, -3000.f, 3000.f), PickIn(s, -3000.f, 3000.f), PickIn(s, -50.f, 400.f));
    if (Rnd(s) % 8 != 0) { RandMatrix(camTargetMat.p(), s); At<uint32>(t, 0x14) = P(camTargetMat.p()); }       // sin matriz: AllocateMatrix (doble) enlaza allocMatrixBuf
    allocMatrixBuf.Fill(0); RandMatrix(allocMatrixBuf.p(), s);
    ac.m_pCamTargetEntity = reinterpret_cast<CEntity*>(t);
    ac.m_vecSourceBeforeLookBehind = CVector{ PickIn(s, -3000.f, 3000.f), PickIn(s, -3000.f, 3000.f), PickIn(s, -50.f, 400.f) };
    camMat.Fill(0); RandMatrix(camMat.p(), s);
    cam.m_matrix = (Rnd(s) % 4 == 0) ? nullptr : reinterpret_cast<CMatrixLink*>(camMat.p());
    cam.m_placement.m_fHeading = PickIn(s, -3.14f, 3.14f);
    (void)mapMode;
    // matriz de camara (derecha en +0, arriba en +0x20): usados por ShowRadarMarker
    for (size_t off : { 0x00u, 0x04u, 0x08u, 0x20u, 0x24u, 0x28u }) At<float>(reinterpret_cast<uint8*>(0xB6F99C), off) = PickUnit(s);
}

inline json Snap(json extra = json::object()) {
    json o = std::move(extra);
    o["radar"] = HexAt(RADAR_REGION, RADAR_REGION_SIZE); o["rect"] = HexAt(0x8D0920, 0x14); o["fe"] = HexAt(FE_REGION, FE_REGION_SIZE);
    return o;
}
}
