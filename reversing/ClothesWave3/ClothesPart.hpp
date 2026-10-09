#pragma once
// ClothesWave3: CClothesBuilder::CreateSkinnedClump (0x5A69D0), la ultima funcion pendiente de la clase.
// Enfoque (a): se doblan TODOS los colaboradores externos (EntryPatch en la direccion del EXE y en el simbolo de la DLL) con
// trazas y argumentos crudos; no se toca el streaming real, ni PLAYER.IMG, ni objetos RenderWare reales, ni pools.
// Original = hook desactivado (codigo del EXE); candidato = hook activo (fuente de la DLL). Se compara: valor devuelto, flujo y
// orden de llamadas con argumentos, el descriptor nuevo (SetModel por defecto), las banderas 0x8D0AA4..AA6, 0xBC1288, los
// materiales falsos (color escrito en linea), la geometria temporal (flags |= 2), el clump falso (parent) y los borrados.
#include "WaveCommon.hpp"
#include "ClothesBuilder.h"
#include "PedClothesDesc.h"
#include "Directory.h"
#include "Stats.h"
#include "RwHelper.h"

namespace ClothesWave3Live { namespace Cl {

constexpr uintptr_t FLAGS = 0x8D0AA4;   // ratios / geometria / texturas (3 bytes + 1)
constexpr uintptr_t ODSCR = 0xBC1288;   // ms_pOldDesc (dword)

// ---------------------------------------------------------------------------------------------------------------------
// Fixtures (todos estaticos: los punteros son identicos entre la ejecucion original y la candidata)
// ---------------------------------------------------------------------------------------------------------------------
inline Buf<0x78> bNd, bOd;                            // descriptores (CPedClothesDesc, 0x78 bytes)
inline Buf<0x80> bBonesClump, bDict, bMats, bSkin, bSkinNew, bHierNew, bAnim;
inline Buf<0x100> bAtom, bAtomNew; inline Buf<0x80> bGeo, bClumpNew, bHier, bTmpGeo;
inline Buf<0x40> bWeights, bIndices;
inline std::array<Buf<0x30>, 10> gGeo, gMat; inline std::array<Buf<0x30>, 9> gTex; inline std::array<Buf<0x30>, 3> gCbTex;
inline std::array<Buf<0x30>, 2> gFrame; inline uint32 nMat{}, nFrame{};
inline uint32 geoMask{}, nameMask{}, numBones{}; inline float statMuscle{};
inline uintptr_t pNd{};

constexpr uint32 NAMES = 9;
inline const char* const kNames[NAMES] = { "torso", "head", "legs", "feet", "necklace", "watch", "glasses", "hat", "extra1" };

inline void Reset() {
    nrec = 0; nMat = nFrame = 0;
    for (auto* b : { &bBonesClump, &bDict, &bMats, &bSkin, &bSkinNew, &bHierNew, &bAnim, &bGeo, &bClumpNew, &bHier, &bTmpGeo }) b->Fill(0xCD);
    bAtom.Fill(0xCD); bAtomNew.Fill(0xCD); bNd.Fill(0xCD); bOd.Fill(0xCD); bWeights.Fill(0xCD); bIndices.Fill(0xCD);
    for (auto& b : gGeo) b.Fill(0xCD); for (auto& b : gMat) b.Fill(0xCD); for (auto& b : gTex) b.Fill(0xCD); for (auto& b : gCbTex) b.Fill(0xCD); for (auto& b : gFrame) b.Fill(0xCD);
}

// ---------------------------------------------------------------------------------------------------------------------
// Stubs (solo traza; las llamadas de hilos ajenos devuelven un valor neutro y no se trazan)
// ---------------------------------------------------------------------------------------------------------------------
inline void __fastcall StDirInit(void* self, void*, int32 cap, void* entries) { if (!Mine()) return; Push(3001, { P(self), uint32(cap), P(entries) }); }
inline void __fastcall StDirRead(void* self, void*, const char* f) { if (!Mine()) return; Push(3002, { P(self), uint32(Fnv(f, strlen(f))) }); }
inline float __cdecl StStat(int32 id) { if (!Mine()) return 0.f; Push(3003, { uint32(id) }); return statMuscle; }
inline void __cdecl StPreprocess(uint32* d, uint32 cut) {
    if (!Mine()) return;
    Push(3004, { d == reinterpret_cast<uint32*>(pNd) ? 1u : 2u, cut, uint32(Fnv(d, 0x78)) });   // 1 = el propio ndscr (no es una copia)
    d[1] += 0x1000u + cut; d[12] ^= 0x00A5A500u;   // muta la copia: los colaboradores posteriores deben ver los valores ya preprocesados
}
inline void* __cdecl StRemoveTex(void* t) { if (!Mine()) return t; Push(3007, { P(t) }); return t; }
inline int32 __cdecl StTexDestroy(void* t) { if (!Mine()) return 1; Push(3008, { P(t) }); return 1; }
inline void* __cdecl StForAll(void* dict, void* (__cdecl* cb)(void*, void*), void* data) {
    if (!Mine()) return dict;
    Push(3005, { P(dict), P(data), cb ? 1u : 0u });   // la direccion del callback (EXE 0x5A44A0 o lambda de la DLL) no se compara
    if (cb) for (uint32 k = 0; k < gCbTex.size(); ++k) { void* r = cb(gCbTex[k].p(), data); Push(3006, { k, P(r) }); }
    return dict;
}
inline void __cdecl StConstructTextures(void* dict, uint32* keys, float a, float b, float c) {
    if (!Mine()) return; Push(3010, { P(dict), uint32(Fnv(keys, 18 * 4)), Bits(a), Bits(b), Bits(c) });
}
inline void* __cdecl StFind(void* dict, const char* name) {
    if (!Mine()) return nullptr;
    uint32 idx = 99; for (uint32 k = 0; k < NAMES; ++k) if (!std::strcmp(name, kNames[k])) idx = k;
    Push(3011, { P(dict), idx, idx == 99 ? uint32(Fnv(name, strlen(name))) : 0u });
    return idx < NAMES && ((nameMask >> idx) & 1) ? gTex[idx].p() : nullptr;
}
inline void* __cdecl StMatCreate() { if (!Mine()) return nullptr; Push(3012); return nMat < gMat.size() ? gMat[nMat++].p() : nullptr; }
inline void* __cdecl StMatSetTex(void* m, void* t) { if (!Mine()) return m; Push(3013, { P(m), P(t) }); At<void*>(static_cast<uint8*>(m), 0) = t; return m; }
inline int32 __cdecl StMatDestroy(void* m) { if (!Mine()) return 1; Push(3014, { P(m) }); return 1; }
inline void __cdecl StGeoArray(void** gs, uint32* keys, float a, float b, float c) {
    if (!Mine()) return;
    Push(3015, { uint32(Fnv(keys, 10 * 4)), Bits(a), Bits(b), Bits(c) });   // 'gs' es un local de pila: su direccion no se traza
    for (uint32 i = 0; i < 10; ++i) gs[i] = ((geoMask >> i) & 1) ? gGeo[i].p() : nullptr;
}
inline void* __cdecl StFirstAtomic(void* clump) { if (!Mine()) return nullptr; Push(3016, { P(clump) }); return bAtom.p(); }
inline void* __cdecl StSkinGet(void* geo) { if (!Mine()) return nullptr; Push(3017, { P(geo) }); return bSkin.p(); }
inline void* __cdecl StHierGet(const void* atomic) { if (!Mine()) return nullptr; Push(3018, { P(atomic) }); return bHier.p(); }
inline void __cdecl StBuildArrays(void* hier, void** ppGeo, void** ppW, void** ppI, uint32 n, void** gs, void** ms) {
    if (!Mine()) return;
    Push(3019, { P(hier), n });
    Push(3020, { P(gs[0]), P(gs[1]), P(gs[2]), P(gs[3]), P(gs[4]), P(gs[5]), P(gs[6]), P(gs[7]) }); Push(3021, { P(gs[8]), P(gs[9]) });
    Push(3022, { P(ms[0]), P(ms[1]), P(ms[2]), P(ms[3]), P(ms[4]), P(ms[5]), P(ms[6]), P(ms[7]) }); Push(3023, { P(ms[8]), P(ms[9]) });
    *ppGeo = bTmpGeo.p(); *ppW = bWeights.p(); *ppI = bIndices.p();
}
inline uint32 __cdecl StNumBones(void* skin) { if (!Mine()) return 0; Push(3024, { P(skin) }); return numBones; }
inline void* __cdecl StSkinMats(void* skin) { if (!Mine()) return nullptr; Push(3025, { P(skin) }); return bMats.p(); }
inline void* __cdecl StSkinCreate(uint32 nv, uint32 nb, void* w, void* i, void* m) { if (!Mine()) return nullptr; Push(3026, { nv, nb, P(w), P(i), P(m) }); return bSkinNew.p(); }
inline void* __cdecl StSetSkin(void* g, void* s) { if (!Mine()) return g; Push(3027, { P(g), P(s) }); return g; }
inline void __cdecl StDelete(void* p) { Push(3030, { P(p) }); }
inline void* __cdecl StHierFrom(void* h, uint32 flags, int32 size) { if (!Mine()) return nullptr; Push(3031, { P(h), flags, uint32(size) }); return bHierNew.p(); }
inline void* __cdecl StFrameCreate() { if (!Mine()) return nullptr; Push(3032); return nFrame < gFrame.size() ? gFrame[nFrame++].p() : nullptr; }
inline int32 __cdecl StHAnimSet(void* f, void* h) { if (!Mine()) return 1; Push(3033, { P(f), P(h) }); return 1; }
inline void* __cdecl StAtomCreate() { if (!Mine()) return nullptr; Push(3034); return bAtomNew.p(); }
inline void* __cdecl StAtomSetGeo(void* a, void* g, uint32 fl) { if (!Mine()) return a; Push(3035, { P(a), P(g), fl }); return a; }
inline void* __cdecl StSkinAtSetH(void* a, void* h) { if (!Mine()) return a; Push(3036, { P(a), P(h) }); return a; }
inline void* __cdecl StAtomSetFrame(void* a, void* f) { if (!Mine()) return a; Push(3037, { P(a), P(f) }); return a; }
inline void* __cdecl StSkinAtSetType(void* a, int32 t) { if (!Mine()) return a; Push(3038, { P(a), uint32(t) }); return a; }
inline void* __cdecl StAddChild(void* p, void* c) { if (!Mine()) return p; Push(3039, { P(p), P(c) }); return p; }
inline void* __cdecl StClumpCreate() { if (!Mine()) return nullptr; Push(3040); return bClumpNew.p(); }
inline void* __cdecl StClumpAdd(void* c, void* a) { if (!Mine()) return c; Push(3041, { P(c), P(a) }); return c; }
inline int32 __cdecl StGeoDestroy(void* g) { if (!Mine()) return 1; Push(3042, { P(g) }); return 1; }
inline void* __cdecl StDictCurrent(void* d) { if (!Mine()) return d; Push(3043, { P(d) }); return d; }

inline std::vector<Stub> Stubs() {
    using std::bit_cast;
    std::vector<Stub> v;
    auto add = [&](const char* n, uintptr_t exe, uintptr_t dll, auto fn) { v.push_back({ n, exe, dll, bit_cast<uintptr_t>(fn) }); };
    add("CDirectory::Init", 0x5322F0, bit_cast<uintptr_t>(&CDirectory::Init), &StDirInit);
    add("CDirectory::ReadDirFile", 0x532350, bit_cast<uintptr_t>(&CDirectory::ReadDirFile), &StDirRead);
    add("CStats::GetStatValue", 0x558E40, bit_cast<uintptr_t>(static_cast<float (*)(eStats)>(&CStats::GetStatValue)), &StStat);
    add("CClothesBuilder::PreprocessClothesDesc", 0x5A44C0, bit_cast<uintptr_t>(&CClothesBuilder::PreprocessClothesDesc), &StPreprocess);
    add("RwTexDictionaryForAllTextures", 0x7F3730, bit_cast<uintptr_t>(&RwTexDictionaryForAllTextures), &StForAll);
    add("RwTexDictionaryRemoveTexture", 0x7F39C0, bit_cast<uintptr_t>(&RwTexDictionaryRemoveTexture), &StRemoveTex);
    add("RwTextureDestroy", 0x7F3820, bit_cast<uintptr_t>(&RwTextureDestroy), &StTexDestroy);
    add("CClothesBuilder::ConstructTextures", 0x5A6040, bit_cast<uintptr_t>(&CClothesBuilder::ConstructTextures), &StConstructTextures);
    add("RwTexDictionaryFindNamedTexture", 0x7F39F0, bit_cast<uintptr_t>(&RwTexDictionaryFindNamedTexture), &StFind);
    add("RpMaterialCreate", 0x74D990, bit_cast<uintptr_t>(&RpMaterialCreate), &StMatCreate);
    add("RpMaterialSetTexture", 0x74DBC0, bit_cast<uintptr_t>(&RpMaterialSetTexture), &StMatSetTex);
    add("RpMaterialDestroy", 0x74DA20, bit_cast<uintptr_t>(&RpMaterialDestroy), &StMatDestroy);
    add("CClothesBuilder::ConstructGeometryArray", 0x5A55A0, bit_cast<uintptr_t>(&CClothesBuilder::ConstructGeometryArray), &StGeoArray);
    add("GetFirstAtomic", 0x734820, bit_cast<uintptr_t>(&GetFirstAtomic), &StFirstAtomic);
    add("RpSkinGeometryGetSkin", 0x7C7550, bit_cast<uintptr_t>(&RpSkinGeometryGetSkin), &StSkinGet);
    add("RpSkinAtomicGetHAnimHierarchy", 0x7C7540, bit_cast<uintptr_t>(&RpSkinAtomicGetHAnimHierarchy), &StHierGet);
    add("CClothesBuilder::ConstructGeometryAndSkinArrays", 0x5A6530, bit_cast<uintptr_t>(&CClothesBuilder::ConstructGeometryAndSkinArrays), &StBuildArrays);
    add("RpSkinGetNumBones", 0x7C77E0, bit_cast<uintptr_t>(&RpSkinGetNumBones), &StNumBones);
    add("RpSkinGetSkinToBoneMatrices", 0x7C7810, bit_cast<uintptr_t>(&RpSkinGetSkinToBoneMatrices), &StSkinMats);
    add("RpSkinCreate", 0x7C75B0, bit_cast<uintptr_t>(&RpSkinCreate), &StSkinCreate);
    add("RpSkinGeometrySetSkin", 0x7C7560, bit_cast<uintptr_t>(&RpSkinGeometrySetSkin), &StSetSkin);
    add("RpHAnimHierarchyCreateFromHierarchy", 0x7C4ED0, bit_cast<uintptr_t>(&RpHAnimHierarchyCreateFromHierarchy), &StHierFrom);
    add("RwFrameCreate", 0x7F0410, bit_cast<uintptr_t>(&RwFrameCreate), &StFrameCreate);
    add("RpHAnimFrameSetHierarchy", 0x7C5130, bit_cast<uintptr_t>(&RpHAnimFrameSetHierarchy), &StHAnimSet);
    add("RpAtomicCreate", 0x749C50, bit_cast<uintptr_t>(&RpAtomicCreate), &StAtomCreate);
    add("RpAtomicSetGeometry", 0x749D40, bit_cast<uintptr_t>(&RpAtomicSetGeometry), &StAtomSetGeo);
    add("RpSkinAtomicSetHAnimHierarchy", 0x7C7520, bit_cast<uintptr_t>(&RpSkinAtomicSetHAnimHierarchy), &StSkinAtSetH);
    add("RpAtomicSetFrame", 0x74BF20, bit_cast<uintptr_t>(&RpAtomicSetFrame), &StAtomSetFrame);
    add("RpSkinAtomicSetType", 0x7C7830, bit_cast<uintptr_t>(&RpSkinAtomicSetType), &StSkinAtSetType);
    add("RwFrameAddChild", 0x7F0B00, bit_cast<uintptr_t>(&RwFrameAddChild), &StAddChild);
    add("RpClumpCreate", 0x74A290, bit_cast<uintptr_t>(&RpClumpCreate), &StClumpCreate);
    add("RpClumpAddAtomic", 0x74A490, bit_cast<uintptr_t>(&RpClumpAddAtomic), &StClumpAdd);
    add("RpGeometryDestroy", 0x74CCC0, bit_cast<uintptr_t>(&RpGeometryDestroy), &StGeoDestroy);
    add("RwTexDictionarySetCurrent", 0x7F3A70, bit_cast<uintptr_t>(&RwTexDictionarySetCurrent), &StDictCurrent);
    return v;
}

inline std::vector<GlobalRange> Globals() { return { {FLAGS, 4}, {ODSCR, 4} }; }

// ---------------------------------------------------------------------------------------------------------------------
// Fixtures aleatorios
// ---------------------------------------------------------------------------------------------------------------------
inline float PickFat(uint32& s) {
    static constexpr float t[] = { 0.f, 100.f, 200.f, 200.5f, 250.f, 300.f, 500.f, 701.f, 1000.f, -50.f, 5000.f, 1e10f, 0.25f, 600.f, 1.f };
    if (Rnd(s) % 40 == 0) return std::bit_cast<float>(0x7FC00000u);
    if (Rnd(s) % 30 == 0) return std::numeric_limits<float>::infinity();
    return t[Rnd(s) % std::size(t)];
}
inline float PickMuscle(uint32& s) {
    static constexpr float t[] = { 0.f, 1.f, 100.f, 300.f, 500.f, 800.f, 1000.f, 1500.f, -5.f, -0.f, 0.5f, 2000.f, 123.456f, 1e10f };
    if (Rnd(s) % 40 == 0) return std::numeric_limits<float>::infinity();
    return t[Rnd(s) % std::size(t)];
}
inline uint32 Key(uint32& s) { uint32 k = Rnd(s); return k ? k : 1u; }

inline json Run(size_t, unsigned I, bool cand, Hook* hook) {
    const uintptr_t addr = 0x5A69D0;
    uint32 s = Seed(I, 0xC3C3); Reset();
    // --- descriptor nuevo ---
    uint8* nd = bNd.p(); uint8* od = bOd.p(); pNd = reinterpret_cast<uintptr_t>(nd);
    static const char* const defNames[5] = { "torso", "head", "hands", "legs", "feet" };
    for (uint32 i = 0; i < 10; ++i) At<uint32>(nd, i * 4) = (i < 5 ? Rnd(s) % 10 < 4 : Rnd(s) % 10 < 6) ? 0u : Key(s);
    for (uint32 i = 0; i < 18; ++i) At<uint32>(nd, 0x28 + i * 4) = Rnd(s) % 5 == 0 ? 0u : Key(s);
    At<float>(nd, 0x70) = PickFat(s); At<float>(nd, 0x74) = PickMuscle(s);
    statMuscle = PickMuscle(s);
    // --- descriptor viejo: nullptr o copia con distintas diferencias; los modelos por defecto ya aplicados (SetModel) ---
    const uint32 mode = Rnd(s) % 9;
    const bool hasOld = mode >= 2;
    if (hasOld) {
        std::memcpy(od, nd, 0x78);
        for (uint32 i = 0; i < 5; ++i) if (!At<uint32>(od, i * 4)) At<uint32>(od, i * 4) = CKeyGen::GetUppercaseKey(defNames[i]);
        switch (mode) {
        case 2: break;                                                                   // identico -> salida anticipada (nullptr)
        case 3: At<float>(od, 0x70) += 1.f; break;                                       // grasa distinta
        case 4: At<float>(od, 0x74) += 1.f; break;                                       // musculo distinto
        case 5: At<uint32>(od, (Rnd(s) % 10) * 4) ^= 0x10u; break;                      // un modelo distinto
        case 6: At<uint32>(od, 0x28 + (Rnd(s) % 18) * 4) ^= 0x20u; break;               // una textura distinta
        case 7: At<float>(nd, 0x70) = std::bit_cast<float>(0x7FC00000u); At<float>(od, 0x70) = std::bit_cast<float>(0x7FC00000u); break;  // NaN: se considera distinto
        default: At<uint32>(od, (Rnd(s) % 10) * 4) ^= 0x4u; At<uint32>(od, 0x28 + (Rnd(s) % 18) * 4) ^= 0x8u; At<float>(od, 0x70) -= 3.f; break;
        }
    }
    const bool cut = Rnd(s) % 4 == 0;
    // --- colaboradores ---
    geoMask = Rnd(s) % 6 == 0 ? 0u : (Rnd(s) & 0x3FFu); nameMask = Rnd(s) % 6 == 0 ? 0u : Rnd(s) % 5 == 0 ? 0x1FFu : (Rnd(s) & 0x1FFu);
    numBones = Rnd(s) % 5 == 0 ? 0u : Rnd(s) % 70;
    At<void*>(bAtom.p(), 0x18) = bGeo.p();
    At<int32>(bHier.p(), 0) = int32(Rnd(s)); At<void*>(bHier.p(), 0x20) = bAnim.p(); At<int32>(bAnim.p(), 0x20) = int32(Rnd(s) % 1000);
    At<uint32>(bTmpGeo.p(), 8) = Rnd(s) % 3 == 0 ? 0u : Rnd(s); At<int32>(bTmpGeo.p(), 0x14) = int32(Rnd(s) % 5000);
    json j;
    uint32 ret{};
    {
        hook->State(cand);
        const bool sw = ReversingProbe::JumpsToDll(addr) == cand;
        const unsigned savedFP = _controlfp(0, 0); _controlfp(_PC_24, _MCW_PC);
        {
            ReversingProbe::EntryPatch d1(0x8214BD, StDelete);   // solo durante la llamada (las trazas no reservan memoria)
            ret = P(reinterpret_cast<void*(__cdecl*)(void*, void*, void*, const void*, bool)>(addr)(bBonesClump.p(), bDict.p(), nd, hasOld ? od : nullptr, cut));
        }
        const unsigned fp = _controlfp(0, 0); _controlfp(savedFP, _MCW_PC | _MCW_RC);
        j = json{ {"switches", sw}, {"fp", fp} };
    }
    j["ret"] = ret; j["trace"] = Recs();
    j["nd"] = bNd.Bytes(); j["od"] = bOd.Bytes();
    j["flags"] = ReversingProbe::Bytes(reinterpret_cast<void*>(FLAGS), 4); j["odscr_ptr"] = *reinterpret_cast<uint32*>(ODSCR);
    json mats = json::array(); for (auto& m : gMat) mats.push_back(m.Bytes()); j["mats"] = mats;
    j["tmpgeo"] = bTmpGeo.Bytes(); j["clump"] = bClumpNew.Bytes(); j["atom"] = bAtomNew.Bytes(); j["hier"] = bHier.Bytes();
    j["weights"] = bWeights.Bytes(); j["indices"] = bIndices.Bytes();
    j["counts"] = { nMat, nFrame };
    j["in"] = { mode, uint32(cut), geoMask, nameMask, numBones, Bits(statMuscle) };
    return j;
}

}}
