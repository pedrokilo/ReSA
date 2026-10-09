#pragma once
// GfxWave2 parte Cl: CClothesBuilder, 14 funciones (docs/hooks.csv validated=0). Trazas con tags 1000+.
// Original (hook desactivado) frente a candidato (hook activo) sobre OBJETOS FALSOS con guardas de 16 bytes.
// Colaboradores doblados (Stubs()): CDirectory::{Init,ReadDirFile,FindItem}, CStreaming::{RequestFile,LoadRequestedModels,
// LoadAllRequestedModels,RemoveModel}, CTimer::{Suspend,Resume}, GetFrameNodeName, RpSkinAtomicGetHAnimHierarchy, RpHAnimIDGetIndex,
// RwRasterCreate/Lock/Unlock, RwTextureCreate, GetFirstTexture. operator delete (0x8214BD) solo durante DestroySkinArrays.
// Fuera: CreateSkinnedClump 0x5A69D0 (ver notes_Cl.md).
#include "GfxCommon.hpp"
#include "ClothesBuilder.h"
#include "Directory.h"
#include "Streaming.h"
#include "Timer.h"
#include "TxdStore.h"
#include "Plugins/NodeNamePlugin/NodeName.h"

namespace GfxWave2Live { namespace Cl {

// ---------------------------------------------------------------------------------------------------------------------
// Tabla de funciones
// ---------------------------------------------------------------------------------------------------------------------
inline const std::array<Fn, 14> fns{{
    {"CClothesBuilder","AddWeightToBoneVertex",0x5A4840,0,-1,512},
    {"CClothesBuilder","BlendTextures-Dst-Src",0x5A5820,0,-1,256},
    {"CClothesBuilder","BlendTextures-Dst-Src1-Src2",0x5A59C0,0,-1,256},
    {"CClothesBuilder","BlendTextures-Dst-Src1-Src2-Tat",0x5A5BC0,0,-1,256},
    {"CClothesBuilder","BuildBoneIndexConversionTable",0x5A56E0,0,-1,256},
    {"CClothesBuilder","CopyTexture",0x5A5730,0,-1,128},
    {"CClothesBuilder","DestroySkinArrays",0x5A56C0,0,-1,64},
    {"CClothesBuilder","GetAtomicWithName",0x5A4810,0,-1,512},
    {"CClothesBuilder","GetTextureFromTxdAndLoadNextTxd",0x5A5F70,0,-1,512},
    {"CClothesBuilder","LoadCdDirectory",0x5A4190,0,-1,64},
    {"CClothesBuilder","PlaceTextureOnTopOfTexture",0x5A57B0,0,-1,256},
    {"CClothesBuilder","RequestGeometry",0x5A41C0,0,-1,256},
    {"CClothesBuilder","RequestTexture",0x5A4220,0,-1,256},
    {"CClothesBuilder","StoreBoneArray",0x5A48B0,0,-1,512},
}};

// ---------------------------------------------------------------------------------------------------------------------
// Fixtures y estado de los colaboradores
// ---------------------------------------------------------------------------------------------------------------------
constexpr uintptr_t CL_BONES = 0xBBC8C8;   // gBoneIndices: int16[10][64]
constexpr size_t    CL_BONES_SIZE = 10 * 64 * 2;

inline std::array<Buf<0x58>, 4> clTex; inline std::array<Buf<0x30>, 4> clRas; inline std::array<Buf<256>, 4> clPix;   // dst, src1, src2, tatuaje
inline std::array<Buf<0x58>, 4> clATex; inline std::array<Buf<0x30>, 4> clARas; inline std::array<Buf<256>, 4> clAPix;  // arena de RwRasterCreate/RwTextureCreate
inline uint32 clANextTex{}, clANextRas{};

// Clump falso (hasta 6 atomos) para GetAtomicWithName / StoreBoneArray
constexpr int CL_MAXAT = 6;
inline Buf<0x80> clClump; inline std::array<Buf<0x100>, CL_MAXAT> clAtom; inline std::array<Buf<0x20>, CL_MAXAT> clFrame;
inline std::array<std::array<char, 16>, CL_MAXAT> clNames{}; inline uint32 clNumAt{};
inline Buf<0x40> clHier; inline Buf<16 * 80> clNodes;
inline Buf<0x40> clModelInfo; inline Buf<0x20> clDir;
inline Buf<64> clTable; inline Buf<48> clWeights; inline Buf<16> clBoneIdx;
// TXD falso: pool {defs, flags, size,...} y diccionarios
inline std::array<uint8, 12 * 16> clDefs{}; inline std::array<uint8, 16> clFlags{}; inline std::array<uintptr_t, 6> clPool{};
inline uint32 clFirstSel{};   // textura que devuelve GetFirstTexture
inline Buf<0x20> clDelA, clDelB;

inline void ClReset() {
    nrec = 0; clANextTex = clANextRas = 0;
    for (auto& b : clATex) b.Fill(0xCD); for (auto& b : clARas) b.Fill(0xCD); for (auto& b : clAPix) b.Fill(0xCD);
}

// Textura falsa k: raster con ancho w, alto h, 32 bits, stride w*4; pixeles aleatorios
inline void ClMakeTex(int k, unsigned w, unsigned h, uint32& s, bool allAlpha = false) {
    clTex[k].Fill(0xCD); clRas[k].Fill(0xCD); clPix[k].Fill(0xCD);
    static constexpr uint8 fmts[] = { 0x0A, 0x8A, 0x1A, 0x6A, 0xFF, 0x0F, 0x00, 0x4A };
    uint8* r = clRas[k].p();
    At<void*>(r, 0x00) = nullptr; At<void*>(r, 0x04) = clPix[k].p(); At<void*>(r, 0x08) = nullptr;
    At<int32>(r, 0x0C) = int32(w); At<int32>(r, 0x10) = int32(h); At<int32>(r, 0x14) = 32; At<int32>(r, 0x18) = int32(w * 4);
    At<uint8>(r, 0x20) = 4; At<uint8>(r, 0x23) = fmts[Rnd(s) % std::size(fmts)];
    At<void*>(clTex[k].p(), 0) = r;
    uint8* px = clPix[k].p();
    for (unsigned i = 0; i < w * h * 4; ++i) {
        const uint32 q = Rnd(s) % 8;
        px[i] = q == 0 ? 0 : q == 1 ? 0xFF : uint8(Rnd(s));
    }
    if (allAlpha) for (unsigned i = 3; i < w * h * 4; i += 4) px[i] = (Rnd(s) % 3) ? uint8(Rnd(s) | 1) : 0;   // alfa (los tatuajes lo usan)
}
inline json ClTexBytes(int k) { return { clTex[k].Bytes(), clRas[k].Bytes(), clPix[k].Bytes() }; }
inline json ClArena() {
    json a = json::array(); a.push_back(clANextTex); a.push_back(clANextRas);
    for (auto& b : clATex) a.push_back(b.Bytes()); for (auto& b : clARas) a.push_back(b.Bytes()); for (auto& b : clAPix) a.push_back(b.Bytes());
    return a;
}
inline float ClRatio(uint32& s) {
    static constexpr float t[] = { 0.f, .25f, .5f, .75f, 1.f, .3333333f, .1f };
    const uint32 q = Rnd(s) % 10;
    return q < 5 ? t[Rnd(s) % std::size(t)] : q < 9 ? PickNear(s) : PickF(s);
}

// ---------------------------------------------------------------------------------------------------------------------
// Stubs (solo traza; no dibujan, no leen ficheros, no tocan el streaming real)
// ---------------------------------------------------------------------------------------------------------------------
inline void __fastcall StDirInit(void* self, void*, int32 cap, void* entries) { if (!Mine()) return; Push(1001, { P(self), uint32(cap), P(entries) }); }
inline void __fastcall StDirRead(void* self, void*, const char* f) { if (!Mine()) return; Push(1002, { P(self), uint32(Fnv(f, strlen(f))) }); }
inline bool __fastcall StDirFind(void* self, void*, uint32 key, uint32* pos, uint32* size) {
    if (!Mine()) return true; Push(1003, { P(self), key }); *pos = key * 3u + 1u; *size = key ^ 0x55u; return true;
}
inline void __cdecl StRequestFile(int32 id, uint32 pos, uint32 size, int32 img, int32 fl) { if (!Mine()) return; Push(1004, { uint32(id), pos, size, uint32(img), uint32(fl) }); }
inline void __cdecl StLoadReq() { if (!Mine()) return; Push(1005); }
inline void __cdecl StLoadAll(bool prio) { if (!Mine()) return; Push(1006, { uint32(prio) }); }
inline void __cdecl StRemove(int32 id) { if (!Mine()) return; Push(1007, { uint32(id) }); }
inline void __cdecl StSuspend() { if (!Mine()) return; Push(1008); }
inline void __cdecl StResume() { if (!Mine()) return; Push(1009); }
inline const char* __cdecl StFrameName(RwFrame* f) {
    if (!Mine()) return "";
    Push(1010, { P(f) });
    for (uint32 k = 0; k < clNumAt; ++k) if (clFrame[k].p() == reinterpret_cast<uint8*>(f)) return clNames[k].data();
    return "";
}
inline void* __cdecl StSkinHier(const void* atomic) { if (!Mine()) return nullptr; Push(1011, { P(atomic) }); return clHier.p(); }
inline int32 __cdecl StHAnimIndex(void* hier, int32 id) {
    if (!Mine()) return -1; Push(1012, { P(hier), uint32(id) });
    return ((uint32(id) * 2654435761u) >> 28) < 3 ? -1 : int32((uint32(id) * 37u) % 300u) - 20;   // incluye -1, 0xFF y valores > 0xFF
}
inline void* __cdecl StRasterCreate(int32 w, int32 h, int32 d, int32 flags) {
    if (!Mine()) return nullptr; Push(1020, { uint32(w), uint32(h), uint32(d), uint32(flags) });
    if (clANextRas >= clARas.size() || w <= 0 || h <= 0 || w * h * 4 > 256) return nullptr;
    const uint32 k = clANextRas++; uint8* r = clARas[k].p();
    At<void*>(r, 0x04) = clAPix[k].p(); At<int32>(r, 0x0C) = w; At<int32>(r, 0x10) = h; At<int32>(r, 0x14) = d; At<int32>(r, 0x18) = w * d / 8;
    At<uint8>(r, 0x20) = uint8(flags & 0xF); At<uint8>(r, 0x23) = uint8((flags >> 8) & 0xFF);
    return r;
}
inline uint8* __cdecl StRasterLock(void* r, int32 level, int32 mode) { if (!Mine()) return nullptr; Push(1021, { P(r), uint32(level), uint32(mode) }); return At<uint8*>(static_cast<uint8*>(r), 4); }
inline void* __cdecl StRasterUnlock(void* r) { if (!Mine()) return r; Push(1022, { P(r) }); return r; }
inline void* __cdecl StTextureCreate(void* raster) {
    if (!Mine()) return nullptr; Push(1023, { P(raster) });
    if (clANextTex >= clATex.size()) return nullptr;
    uint8* t = clATex[clANextTex++].p(); At<void*>(t, 0) = raster; return t;
}
inline void* __cdecl StFirstTexture(void* dict) { if (!Mine()) return nullptr; Push(1024, { P(dict) }); return clTex[clFirstSel & 3].p(); }
inline void __cdecl StDelete(void* p) { Push(1030, { P(p) }); }

inline std::vector<Stub> Stubs() {
    using std::bit_cast;
    std::vector<Stub> v;
    auto add = [&](const char* n, uintptr_t exe, uintptr_t dll, auto fn) { v.push_back({ n, exe, dll, bit_cast<uintptr_t>(fn) }); };
    add("CDirectory::Init", 0x5322F0, bit_cast<uintptr_t>(&CDirectory::Init), &StDirInit);
    add("CDirectory::ReadDirFile", 0x532350, bit_cast<uintptr_t>(&CDirectory::ReadDirFile), &StDirRead);
    add("CDirectory::FindItem", 0x5324D0, bit_cast<uintptr_t>(static_cast<bool (CDirectory::*)(uint32, CdStreamPos&, uint32&) const>(&CDirectory::FindItem)), &StDirFind);
    add("CStreaming::RequestFile", 0x40A080, bit_cast<uintptr_t>(&CStreaming::RequestFile), &StRequestFile);
    add("CStreaming::LoadRequestedModels", 0x40E3A0, bit_cast<uintptr_t>(&CStreaming::LoadRequestedModels), &StLoadReq);
    add("CStreaming::LoadAllRequestedModels", 0x40EA10, bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels), &StLoadAll);
    add("CStreaming::RemoveModel", 0x4089A0, bit_cast<uintptr_t>(&CStreaming::RemoveModel), &StRemove);
    add("CTimer::Suspend", 0x5619D0, bit_cast<uintptr_t>(&CTimer::Suspend), &StSuspend);
    add("CTimer::Resume", 0x561A00, bit_cast<uintptr_t>(&CTimer::Resume), &StResume);
    add("GetFrameNodeName", 0x72FB30, bit_cast<uintptr_t>(&GetFrameNodeName), &StFrameName);
    add("RpSkinAtomicGetHAnimHierarchy", 0x7C7540, bit_cast<uintptr_t>(&RpSkinAtomicGetHAnimHierarchy), &StSkinHier);
    add("RpHAnimIDGetIndex", 0x7C51A0, bit_cast<uintptr_t>(&RpHAnimIDGetIndex), &StHAnimIndex);
    add("RwRasterCreate", 0x7FB230, bit_cast<uintptr_t>(&RwRasterCreate), &StRasterCreate);
    add("RwRasterLock", 0x7FB2D0, bit_cast<uintptr_t>(&RwRasterLock), &StRasterLock);
    add("RwRasterUnlock", 0x7FAEC0, bit_cast<uintptr_t>(&RwRasterUnlock), &StRasterUnlock);
    add("RwTextureCreate", 0x7F37C0, bit_cast<uintptr_t>(&RwTextureCreate), &StTextureCreate);
    add("GetFirstTexture", 0x734940, bit_cast<uintptr_t>(&GetFirstTexture), &StFirstTexture);
    return v;
}

inline std::vector<GlobalRange> Globals() {
    return { {CL_BONES, CL_BONES_SIZE}, {0xBC12D0, 4}, {0xC88004, 8}, {0xC8800C, 4}, {0xA9B0C8 + 384 * 4, 10 * 4} };
}

// ---------------------------------------------------------------------------------------------------------------------
// Ejecucion: conmuta el hook, comprueba el salto, fija x87 a 24 bits durante la llamada
// ---------------------------------------------------------------------------------------------------------------------
template<class F> inline json ClExec(Hook* hook, uintptr_t addr, bool cand, F&& call) {
    hook->State(cand);
    const bool sw = ReversingProbe::JumpsToDll(addr) == cand;
    const unsigned savedFP = _controlfp(0, 0); _controlfp(_PC_24, _MCW_PC);
    call();
    const unsigned fp = _controlfp(0, 0); _controlfp(savedFP, _MCW_PC | _MCW_RC);
    return json{ {"switches", sw}, {"fp", fp} };
}
inline void ClFillBones(uint32& s) { auto* b = reinterpret_cast<int16*>(CL_BONES); for (size_t i = 0; i < 640; ++i) b[i] = int16(Rnd(s) % 7 == 0 ? -1 : Rnd(s) % 900); }
inline json ClBonesBytes() { return ReversingProbe::Bytes(reinterpret_cast<void*>(CL_BONES), CL_BONES_SIZE); }

// ---------------------------------------------------------------------------------------------------------------------
// Casos
// ---------------------------------------------------------------------------------------------------------------------
inline json CaseAddWeight(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0xC101); ClReset();
    clWeights.Fill(0xCD); clBoneIdx.Fill(0xCD);
    float* w = reinterpret_cast<float*>(clWeights.p()); uint8* b = clBoneIdx.p();
    const uint32 k = Rnd(s) % 9;   // ranuras ocupadas (8 = todas)
    for (uint32 j = 0; j < 10; ++j) {
        if (j < k) { float v = PickNear(s); if (v == 0.f || Rnd(s) % 25 == 0) v = (Rnd(s) % 12 == 0) ? std::bit_cast<float>(0x7FC00000u) : 0.5f; w[j] = v; }
        else if (j == k) w[j] = 0.f;
        else w[j] = (Rnd(s) % 3) ? PickNear(s) : 0.f;
    }
    for (uint32 j = 0; j < 8; ++j) b[j] = uint8(Rnd(s) % 6 == 0 ? Rnd(s) : j * 3 + 1);
    float add = Rnd(s) % 7 == 0 ? 0.f : Rnd(s) % 30 == 0 ? std::bit_cast<float>(0x7FC00000u) : (Rnd(s) % 4) ? PickNear(s) : PickF(s);
    uint32 target = Rnd(s) % 2 && k ? b[Rnd(s) % k] : Rnd(s) % 12 == 0 ? (Rnd(s) & 0x3FF) : (Rnd(s) % 40);
    auto j = ClExec(hook, addr, cand, [&] { reinterpret_cast<void(__cdecl*)(float*, uint8*, float, uint32)>(addr)(w, b, add, target); });
    j["trace"] = Recs(); j["w"] = clWeights.Bytes(); j["b"] = clBoneIdx.Bytes(); j["in"] = { k, Bits(add), target };
    return j;
}

// 0: dst/src (2 texturas), 1: dst/src1/src2, 2: dst/src1/src2/tatuaje
inline json CaseBlend(unsigned I, bool cand, Hook* hook, uintptr_t addr, int kind) {
    uint32 s = Seed(I, 0xC200 + kind); ClReset();
    const unsigned w = 1 + Rnd(s) % 6, h = 1 + Rnd(s) % 6;
    for (int k = 0; k < 4; ++k) ClMakeTex(k, w, h, s, k == 3);
    const float r1 = ClRatio(s), r2 = ClRatio(s), r3 = ClRatio(s); const int32 n = int32(Rnd(s) % 5);
    json j = ClExec(hook, addr, cand, [&] {
        if (kind == 0) reinterpret_cast<void(__cdecl*)(void*, void*, float, float, int32)>(addr)(clTex[0].p(), clTex[1].p(), r1, r2, n);
        else if (kind == 1) reinterpret_cast<void(__cdecl*)(void*, void*, void*, float, float, float, int32)>(addr)(clTex[0].p(), clTex[1].p(), clTex[2].p(), r1, r2, r3, n);
        else reinterpret_cast<void(__cdecl*)(void*, void*, void*, float, float, float, int32, void*)>(addr)(clTex[0].p(), clTex[1].p(), clTex[2].p(), r1, r2, r3, n, clTex[3].p());
    });
    j["trace"] = Recs(); j["t"] = { ClTexBytes(0), ClTexBytes(1), ClTexBytes(2), ClTexBytes(3) }; j["in"] = { Bits(r1), Bits(r2), Bits(r3), n };
    return j;
}

inline json CaseBuildTable(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0xC300); ClReset(); ClFillBones(s);
    clTable.Fill(0xCD);
    const int32 index = int32(Rnd(s) % 10);
    // fila: prefijo valido y -1 de cierre en posicion aleatoria (a veces ninguno = 64 entradas)
    auto* row = reinterpret_cast<int16*>(CL_BONES) + index * 64; const uint32 len = Rnd(s) % 5 == 0 ? 64 : Rnd(s) % 64;
    for (uint32 q = 0; q < len; ++q) row[q] = int16(Rnd(s) % 8 == 0 ? Rnd(s) % 70000 : Rnd(s) % 400);
    if (len < 64) row[len] = -1;
    auto j = ClExec(hook, addr, cand, [&] { reinterpret_cast<void(__cdecl*)(uint8*, void*, int32)>(addr)(clTable.p(), clHier.p(), index); });
    j["trace"] = Recs(); j["table"] = clTable.Bytes(); j["bones"] = ClBonesBytes();
    return j;
}

inline json CaseCopyTexture(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0xC400); ClReset();
    ClMakeTex(0, 1 + Rnd(s) % 6, 1 + Rnd(s) % 6, s);
    void* ret{};
    auto j = ClExec(hook, addr, cand, [&] { ret = reinterpret_cast<void*(__cdecl*)(void*)>(addr)(clTex[0].p()); });
    j["ret"] = P(ret); j["trace"] = Recs(); j["src"] = ClTexBytes(0); j["arena"] = ClArena();
    return j;
}

inline json CaseDestroy(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0xC500); ClReset();
    void* a = (Rnd(s) % 6) ? static_cast<void*>(clDelA.p()) : nullptr; void* b = (Rnd(s) % 6) ? static_cast<void*>(clDelB.p()) : nullptr;
    clDelA.Fill(0xCD); clDelB.Fill(0xCD);
    hook->State(cand);
    const bool sw = ReversingProbe::JumpsToDll(addr) == cand;
    const unsigned savedFP = _controlfp(0, 0); _controlfp(_PC_24, _MCW_PC);
    {
        ReversingProbe::EntryPatch d1(0x8214BD, StDelete);   // solo durante la llamada
        reinterpret_cast<void(__cdecl*)(void*, void*)>(addr)(a, b);
    }
    const unsigned fp = _controlfp(0, 0); _controlfp(savedFP, _MCW_PC | _MCW_RC);
    return { {"switches", sw}, {"fp", fp}, {"trace", Recs()}, {"a", clDelA.Bytes()}, {"b", clDelB.Bytes()} };
}

// Clump falso con n atomos (nombres de un repertorio con duplicados y diferencias de mayusculas)
inline void ClMakeClump(uint32& s, uint32 n, bool forceNormal) {
    static constexpr const char* pool[] = { "normal", "fat", "ripped", "NORMAL", "Normal", "torso", "head", "", "normal2" };
    clClump.Fill(0xCD); for (auto& a : clAtom) a.Fill(0xCD); for (auto& f : clFrame) f.Fill(0xCD);
    clNumAt = n;
    auto* c = reinterpret_cast<RpClump*>(clClump.p());
    c->object.type = 2; c->object.subType = 0; c->object.flags = 0; c->object.privateFlags = 0; c->object.parent = nullptr;
    RwLLLink* head = &c->atomicList.link; RwLLLink* prev = head;
    for (uint32 k = 0; k < n; ++k) {
        auto* a = reinterpret_cast<RpAtomic*>(clAtom[k].p());
        a->object.object.type = 1; a->object.object.parent = reinterpret_cast<RwFrame*>(clFrame[k].p());
        a->clump = c;
        a->inClumpLink.prev = prev; prev->next = &a->inClumpLink; prev = &a->inClumpLink;
        const char* nm = (k == 0 && forceNormal) ? "normal" : pool[Rnd(s) % std::size(pool)];
        clNames[k].fill(0); std::strncpy(clNames[k].data(), nm, 15);
    }
    prev->next = head; head->prev = prev;
}

inline json CaseGetAtomic(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0xC600); ClReset();
    ClMakeClump(s, Rnd(s) % (CL_MAXAT + 1), false);
    static constexpr const char* q[] = { "normal", "fat", "ripped", "NORMAL", "Normal", "torso", "head", "", "missing", "NoRMal" };
    const char* name = q[Rnd(s) % std::size(q)];
    void* ret{};
    auto j = ClExec(hook, addr, cand, [&] { ret = reinterpret_cast<void*(__cdecl*)(void*, const char*)>(addr)(clClump.p(), name); });
    j["ret"] = P(ret); j["trace"] = Recs(); j["clump"] = clClump.Bytes(); j["name"] = name;
    json at = json::array(); for (uint32 k = 0; k < CL_MAXAT; ++k) at.push_back(clAtom[k].Bytes()); j["atoms"] = at;
    return j;
}

inline json CaseStoreBone(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0xC700); ClReset(); ClFillBones(s);
    ClMakeClump(s, 1 + Rnd(s) % CL_MAXAT, true);
    const int32 idx = int32(Rnd(s) % 10);
    const uint32 maxN = idx == 9 ? 64 : 72;
    const int32 numNodes = Rnd(s) % 8 == 0 ? 0 : int32(Rnd(s) % (maxN + 1));
    clHier.Fill(0xCD); clNodes.Fill(0xCD);
    At<int32>(clHier.p(), 4) = numNodes; At<void*>(clHier.p(), 0x10) = clNodes.p();
    for (int32 k = 0; k < numNodes; ++k) { At<int32>(clNodes.p(), k * 16) = int32(Rnd(s) % 5 == 0 ? Rnd(s) : Rnd(s) % 300); }
    auto j = ClExec(hook, addr, cand, [&] { reinterpret_cast<void(__cdecl*)(void*, int32)>(addr)(clClump.p(), idx); });
    j["trace"] = Recs(); j["bones"] = ClBonesBytes(); j["hier"] = clHier.Bytes(); j["nodes"] = clNodes.Bytes(); j["in"] = { idx, numNodes };
    return j;
}

inline json CasePlace(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0xC800); ClReset();
    const unsigned w = 1 + Rnd(s) % 6, h = 1 + Rnd(s) % 6;
    ClMakeTex(0, w, h, s); ClMakeTex(1, w, h, s, true);
    auto j = ClExec(hook, addr, cand, [&] { reinterpret_cast<void(__cdecl*)(void*, void*)>(addr)(clTex[0].p(), clTex[1].p()); });
    j["trace"] = Recs(); j["t"] = { ClTexBytes(0), ClTexBytes(1) };
    return j;
}

inline json CaseGetTexture(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0xC900); ClReset();
    const unsigned w = 1 + Rnd(s) % 6, h = 1 + Rnd(s) % 6;
    ClMakeTex(0, w, h, s); ClMakeTex(1, w, h, s, true);
    const bool hasDst = Rnd(s) % 2; const int32 txdId = Rnd(s) % 4 == 0 ? -1 : int32(Rnd(s) % 8);
    const int32 crc = Rnd(s) % 3 == 0 ? 0 : int32(Rnd(s) | 1);
    // pool de TXD falso: 16 entradas, todas en uso (bit 7 limpio)
    clDefs.fill(0); clFlags.fill(0);
    for (uint32 k = 0; k < 16; ++k) At<uintptr_t>(clDefs.data(), k * 12) = 0x510000 + k * 0x100;
    clPool = { uintptr_t(clDefs.data()), uintptr_t(clFlags.data()), 16, 0, 0, 0 };
    CTxdStore::ms_pTxdPool = reinterpret_cast<CTxdPool*>(clPool.data());
    *reinterpret_cast<uint32*>(0xBC12D0) = Rnd(s) % 4;
    for (int k = 0; k < 4; ++k) reinterpret_cast<int16*>(0xC88004)[k] = int16(Rnd(s) % 16);
    clFirstSel = 1;
    int32 next = int32(0xAAAAAAAAu); void* ret{};
    auto j = ClExec(hook, addr, cand, [&] { ret = reinterpret_cast<void*(__cdecl*)(void*, int32, int32, int32*)>(addr)(hasDst ? clTex[0].p() : nullptr, txdId, crc, &next); });
    j["ret"] = P(ret); j["next"] = next; j["trace"] = Recs(); j["t"] = { ClTexBytes(0), ClTexBytes(1) }; j["arena"] = ClArena();
    j["idx"] = *reinterpret_cast<uint32*>(0xBC12D0); j["in"] = { hasDst, txdId, crc };
    return j;
}

inline json CaseLoadCd(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    ClReset();
    auto j = ClExec(hook, addr, cand, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); });
    j["trace"] = Recs(); j["case"] = I; return j;
}

inline json CaseReqGeometry(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0xCA00); ClReset();
    const int32 id = 384 + int32(Rnd(s) % 10); const uint32 key = Rnd(s) % 10 == 0 ? 0u : Rnd(s);
    clModelInfo.Fill(Rnd(s) % 2 ? 0xCD : 0x00);
    *reinterpret_cast<void**>(0xA9B0C8 + id * 4) = clModelInfo.p();
    auto j = ClExec(hook, addr, cand, [&] { reinterpret_cast<void(__cdecl*)(int32, uint32)>(addr)(id, key); });
    j["trace"] = Recs(); j["mi"] = clModelInfo.Bytes(); j["in"] = { id, key }; return j;
}

inline json CaseReqTexture(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0xCB00); ClReset();
    const uint32 key = Rnd(s) % 5 == 0 ? 0u : Rnd(s);
    *reinterpret_cast<uint32*>(0xBC12D0) = Rnd(s) % 4;
    for (int k = 0; k < 4; ++k) reinterpret_cast<int16*>(0xC88004)[k] = int16(Rnd(s) % 3 == 0 ? -int(Rnd(s) % 5) : int(Rnd(s) % 300));
    int32 ret{};
    auto j = ClExec(hook, addr, cand, [&] { ret = reinterpret_cast<int32(__cdecl*)(uint32)>(addr)(key); });
    j["ret"] = ret; j["trace"] = Recs(); j["idx"] = *reinterpret_cast<uint32*>(0xBC12D0); j["in"] = key; return j;
}

// Despachador: f = indice en fns
inline json Run(size_t f, unsigned i, bool cand, Hook* hook) {
    const auto a = fns[f].address;
    switch (f) {
    case 0:  return CaseAddWeight(i, cand, hook, a);
    case 1:  return CaseBlend(i, cand, hook, a, 0);
    case 2:  return CaseBlend(i, cand, hook, a, 1);
    case 3:  return CaseBlend(i, cand, hook, a, 2);
    case 4:  return CaseBuildTable(i, cand, hook, a);
    case 5:  return CaseCopyTexture(i, cand, hook, a);
    case 6:  return CaseDestroy(i, cand, hook, a);
    case 7:  return CaseGetAtomic(i, cand, hook, a);
    case 8:  return CaseGetTexture(i, cand, hook, a);
    case 9:  return CaseLoadCd(i, cand, hook, a);
    case 10: return CasePlace(i, cand, hook, a);
    case 11: return CaseReqGeometry(i, cand, hook, a);
    case 12: return CaseReqTexture(i, cand, hook, a);
    default: return CaseStoreBone(i, cand, hook, a);
    }
}

}}
