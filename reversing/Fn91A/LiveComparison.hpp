#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "WaterLevel.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn91A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cuatro funciones (hook S, RH_ScopedGlobalInstall), todas cdecl con `ret` del llamador:
//   0 CWaterLevel::AddToQuadsAndTrianglesList 0x6E5750; 1 CWaterLevel::GetGroundLevel 0x6EA8A0; 2 CWaterLevel::RenderHighDetailWaterRectangle_OneLayer 0x6E91D0; 3 CWaterLevel::RenderHighDetailWaterTriangle_OneLayer 0x6E8780.
// Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (los tres tienen hook RH, apagado durante el banco):
//   FlushWaterRenderBuffer 0x6E7680 (funciones 2 y 3), CWaterLevel::CalculateWavesOnlyForCoordinate 0x6E6EF0 (2 y 3), CWorld::ProcessVerticalLine 0x5674E0 (1).
// Los falsos registran sus argumentos EN ORDEN. El de vaciado NO renderiza: anota los contadores previos y deja los contadores (vertices, indices y de colores) en los valores del caso; el de las olas
// escribe altura, multiplicador de color, brillo y normal segun el caso (y en algunos casos modifica los contadores tras una llamada: el EXE y la DLL deben releerlos); el de ProcessVerticalLine escribe la
// z y la superficie del CColPoint segun el caso y devuelve el resultado con basura en los bytes altos de EAX.
// Memoria real del juego (se guarda y se restaura tras cada lado): ver SaveRegions (Core.hpp). La camara: posicion del placement (0xB6F02C) y puntero a una matriz propia con la posicion en +0x30 (o nulo).
namespace F91ALive {
using namespace F91AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x6E5750, 0x6EA8A0, 0x6E91D0, 0x6E8780};
inline constexpr u32 kPopBytes[kFunctions] = {16, 16, 100, 88};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint16& G16(uintptr_t a) { return *reinterpret_cast<uint16*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }

struct State { const C* c{}; u32 waveCalls{}; };
inline State g;
// Los punteros a la pila (temporales del EXE o de la DLL) no son comparables entre lados: se etiquetan todos igual
inline u32 TagS(uintptr_t v) {
    const uintptr_t lo = __readfsdword(8), hi = __readfsdword(4);
    if (v >= lo && v < hi) return 0x7F000000u;
    return u32(v);
}
inline u32 TagS(const void* p) { return TagS(uintptr_t(p)); }

// Eventos: 1 vaciado del bufer [vertices, indices, colores]; 2 olas [x, y, grandes, pequenas, ptr altura, ptr color, ptr brillo, ptr normal, altura previa]; 3 ProcessVerticalLine [origen.xyz, distancia, ptr colpoint, ptr entidad, 6 banderas, ptr poligono]
inline void __cdecl FkFlush() {                                                                          // 0x6E7680
    Ev& e = PushEv(1, 3); e.v[0] = G32(0xC4B950); e.v[1] = G32(0xC4B954); e.v[2] = G32(0xC1F960);
    G32(0xC4B950) = g.c->vtx0; G32(0xC4B954) = g.c->idx0; G32(0xC1F960) = g.c->ci0;
}
inline void __cdecl FkWaves(u32 x, u32 y, u32 bigAmp, u32 smallAmp, u32* outWave, u32* colorMult, u32* glare, u32* normal) {   // 0x6E6EF0
    Ev& e = PushEv(2, 9);
    e.v[0] = x; e.v[1] = y; e.v[2] = bigAmp; e.v[3] = smallAmp; e.v[4] = TagS(outWave); e.v[5] = TagS(colorMult); e.v[6] = TagS(glare); e.v[7] = u32(uintptr_t(normal)); e.v[8] = *outWave;
    const u32 k = g.waveCalls++;
    *outWave = WaveOut(*g.c, k, 0); *colorMult = WaveOut(*g.c, k, 1); *glare = WaveOut(*g.c, k, 2);
    normal[0] = WaveOut(*g.c, k, 3); normal[1] = WaveOut(*g.c, k, 4); normal[2] = WaveOut(*g.c, k, 5);
    if (g.c->mutCall == i32(k)) { G32(0xC4B950) += g.c->mutDv; G32(0xC4B954) += g.c->mutDi; G32(0xC1F960) += g.c->mutDc; }
}
inline u32 __cdecl FkGround(const u32* origin, u32 dist, u8* colPoint, u32* entity, u32 b0, u32 b1, u32 b2, u32 b3, u32 b4, u32 b5, void* poly) {   // 0x5674E0
    Ev& e = PushEv(3, 13);
    e.v[0] = origin[0]; e.v[1] = origin[1]; e.v[2] = origin[2]; e.v[3] = dist; e.v[4] = TagS(colPoint); e.v[5] = TagS(entity);
    e.v[6] = b0 & 0xFF; e.v[7] = b1 & 0xFF; e.v[8] = b2 & 0xFF; e.v[9] = b3 & 0xFF; e.v[10] = b4 & 0xFF; e.v[11] = b5 & 0xFF; e.v[12] = Tag(uintptr_t(poly));
    memcpy(colPoint + 8, &g.c->gz, 4); colPoint[0x23] = u8(g.c->gsurf); colPoint[0x24] = u8(g.c->gpiece);
    return (g.c->ghit ? 1u : 0u) | g.c->gretHi;
}

inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}
// CRC-32 (zlib, polinomio 0xEDB88320), encadenable como zlib.crc32(datos, valor): el oraculo en Python usa la misma funcion.
inline u32 Crc32(const u8* p, size_t n, u32 crc = 0) {
    static const auto table = [] { std::array<u32, 256> t{}; for (u32 i = 0; i < 256; ++i) { u32 v = i; for (int k = 0; k < 8; ++k) v = (v >> 1) ^ (0xEDB88320u & (0u - (v & 1u))); t[i] = v; } return t; }();
    crc = ~crc;
    for (size_t i = 0; i < n; ++i) crc = table[(crc ^ p[i]) & 0xFF] ^ (crc >> 8);
    return ~crc;
}
// FNV de los eventos (kind, len, valores) y primeros eventos completos (el resto, solo por el FNV)
inline json EventsObserved() {
    std::vector<u8> stream;
    auto feed = [&](u32 v) { for (int k = 0; k < 4; ++k) stream.push_back(u8(v >> (8 * k))); };
    for (unsigned i = 0; i < nev && i < evs.size(); ++i) { feed(evs[i].kind); feed(evs[i].len); for (u32 k = 0; k < evs[i].len; ++k) feed(evs[i].v[k]); }
    json first = json::array();
    for (unsigned i = 0; i < nev && i < 4; ++i) first.push_back(EvJson(evs[i]));
    return {{"n", nev}, {"crc", stream.empty() ? 0u : Crc32(stream.data(), stream.size())}, {"first", first}};
}

// ================================================================================================ funcion 0: AddToQuadsAndTrianglesList
inline void CaseQuad(Env& env, Totals& t, unsigned n) {
    const C c = MakeC(0, n);
    const json input = json::parse(JsonC(c));
    static std::array<u8, kComboBytes> ref{};
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        FillRegion(ref.data(), kComboBytes, c.mode, c.rseed + 11);
        memcpy(ref.data(), &c.qcount, 4);
        const u32 slot = u32(c.qb) + 12u * u32(c.qa);
        const uint16 old = uint16(c.qold);
        memcpy(ref.data() + kComboBlocks + 2 * slot, &old, 2);
        memcpy(reinterpret_cast<void*>(kComboAddr), ref.data(), kComboBytes);
        const u32 args[4] = {u32(c.qa), u32(c.qb), u32(c.qi), c.qtype};
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, 0, kEntries[0], candidate, [&](uintptr_t entry) { return Gate(entry, args, 4, c.eax, c.ecx, c.edx, c.stack, kPopBytes[0]); });
        json ch = json::array();
        const u8* after = reinterpret_cast<const u8*>(kComboAddr);
        for (u32 i = 0; i < kComboBytes; ++i) if (after[i] != ref[i]) ch.push_back(json::array({i, after[i]}));
        json obs = {{"changes", ch}, {"events", EventsObserved()}};
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[0], n, input, rows, sides);
}

// ================================================================================================ funcion 1: GetGroundLevel
inline void CaseGround(Env& env, Totals& t, unsigned n) {
    const C c = MakeC(1, n);
    const json input = json::parse(JsonC(c));
    alignas(16) static std::array<u8, 0x100> arena{}, ref{};
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        FillWords(ref.data(), ref.size(), c.rseed);
        memcpy(ref.data() + 0x40, c.gpos, 12);
        arena = ref;
        const u32 args[4] = {u32(uintptr_t(arena.data() + 0x40)), u32(uintptr_t(arena.data() + 0x50)), c.gcolnull ? 0u : u32(uintptr_t(arena.data() + 0x58)), c.grange};
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, 1, kEntries[1], candidate, [&](uintptr_t entry) { return Gate(entry, args, 4, c.eax, c.ecx, c.edx, c.stack, kPopBytes[1]); });
        json ch = json::array();
        for (u32 i = 0; i < arena.size(); ++i) if (arena[i] != ref[i]) ch.push_back(json::array({i, arena[i]}));
        json obs = {{"changes", ch}, {"events", EventsObserved()}, {"ret_al", sides[s].reg.eax & 0xFF}};
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[1], n, input, rows, sides);
}

// ================================================================================================ funciones 2 y 3: una capa del rectangulo / triangulo de alto detalle
inline void CaseWater(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    static std::array<u8, kBufBytes> refBuf{};
    static std::array<u8, kColBytes> refCol{};
    alignas(16) static std::array<u8, 0x100> camMat{};
    const u32 K = Vertices(fn, c.hx, c.hy), sv = SpanVertices(fn, c.hx, c.hy), si = SpanIndices(fn, c.hx, c.hy), sc = SpanColors(fn, c.hx, c.hy);
    (void)K;
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        // regiones con su imagen previa
        FillRegion(refBuf.data(), kBufBytes, c.mode, c.rseed);
        memcpy(refBuf.data(), &c.preVtx, 4); memcpy(refBuf.data() + 4, &c.preIdx, 4);
        memcpy(reinterpret_cast<void*>(kBufAddr), refBuf.data(), kBufBytes);
        FillRegion(refCol.data(), kColBytes, c.mode, c.rseed + 7);
        memcpy(refCol.data(), &c.preCi, 4);
        memcpy(reinterpret_cast<void*>(kColAddr), refCol.data(), kColBytes);
        FillRegion(reinterpret_cast<u8*>(kGlobAddr), kGlobBytes, c.mode, c.rseed + 13);
        for (u32 k = 0; k < 6; ++k) G32(0xC21170 + 4 * k) = c.shift[k];
        G32(0xC2116C) = c.wcol;
        FillRegion(reinterpret_cast<u8*>(kNormAddr), kNormBytes, c.mode, c.rseed + 17);
        G8(0xC278E0) = u8(c.flag);
        G32(kDetailAddr) = u32(c.detail);
        for (u32 k = 0; k < 3; ++k) G32(kAlphaAddr + 4 * k) = c.alpha[k];
        G32(kK3834Addr) = c.k3834;
        memset(camMat.data(), 0xA5, camMat.size());
        memcpy(camMat.data() + 0x30, c.camM, 12);
        memcpy(reinterpret_cast<void*>(kCamPosAddr), c.camP, 12);
        G32(kCamPosAddr + 12) = 0xDEADBEEFu;
        G32(kCamMatAddr) = c.useMat ? u32(uintptr_t(camMat.data())) : 0u;
        // imagenes de las regiones pequenas (deben quedar iguales salvo la normal y su byte)
        std::vector<u8> smallBefore;
        for (const auto& r : regions) if (r.at != kBufAddr && r.at != kColAddr && r.at != kComboAddr && r.at != kNormAddr) smallBefore.insert(smallBefore.end(), r.Ptr(), r.Ptr() + r.bytes);
        std::array<u8, kNormBytes> normBefore; memcpy(normBefore.data(), reinterpret_cast<void*>(kNormAddr), kNormBytes);
        TagClear(); TagAdd(camMat.data(), camMat.size());
        const bool rect = fn == 2;
        u32 args[25]; unsigned na = 0;
        if (rect) {
            args[na++] = u32(c.g[0]); args[na++] = u32(c.g[1]); args[na++] = u32(c.g[2]); args[na++] = u32(c.g[3]);
            for (int k = 0; k < 4; ++k) for (int m = 0; m < 4; ++m) args[na++] = c.p[k][m];
            args[na++] = u32(c.layer); args[na++] = u32(c.junkA); args[na++] = u32(c.junkB); args[na++] = u32(c.hx); args[na++] = u32(c.hy);
        } else {
            args[na++] = u32(c.g[0]); args[na++] = u32(c.g[1]);
            for (int m = 0; m < 4; ++m) args[na++] = c.p[0][m];
            args[na++] = u32(c.g[2]); args[na++] = u32(c.g[3]);
            for (int m = 0; m < 4; ++m) args[na++] = c.p[1][m];
            args[na++] = u32(c.g[4]); args[na++] = u32(c.g[5]);
            for (int m = 0; m < 4; ++m) args[na++] = c.p[2][m];
            args[na++] = u32(c.layer); args[na++] = u32(c.junkA); args[na++] = u32(c.junkB); args[na++] = u32(c.hx);
        }
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args, na, c.eax, c.ecx, c.edx, c.stack, kPopBytes[fn]); });
        // observacion
        const u8* buf = reinterpret_cast<const u8*>(kBufAddr);
        const u8* col = reinterpret_cast<const u8*>(kColAddr);
        const u32 vtxAfter = G32(0xC4B950), idxAfter = G32(0xC4B954), ciAfter = G32(0xC1F960);
        const u32 v0 = kVtxOff + kVertexBytes * c.vtx0, vlen = kVertexBytes * sv, i0 = kIdxOff + 2 * c.idx0, ilen = 2 * si;
        bool rest = true;
        for (u32 i = 8; i < kBufBytes && rest; ++i) {
            if ((i >= i0 && i < i0 + ilen) || (i >= v0 && i < v0 + vlen)) continue;
            rest = buf[i] == refBuf[i];
        }
        const u32 cspan0 = c.ci0, clen = sc;
        for (u32 i = 4; i < kColBytes && rest; ++i) {
            const bool inB = i >= kColB + cspan0 && i < kColB + cspan0 + clen, inG = i >= kColG + cspan0 && i < kColG + cspan0 + clen, inR = i >= kColR + cspan0 && i < kColR + cspan0 + clen;
            if (inB || inG || inR) continue;
            rest = col[i] == refCol[i];
        }
        u32 colorsFnv = Crc32(col + kColB + cspan0, clen);
        colorsFnv = Crc32(col + kColG + cspan0, clen, colorsFnv);
        colorsFnv = Crc32(col + kColR + cspan0, clen, colorsFnv);
        std::vector<u8> smallAfter;
        for (const auto& r : regions) if (r.at != kBufAddr && r.at != kColAddr && r.at != kComboAddr && r.at != kNormAddr) smallAfter.insert(smallAfter.end(), r.Ptr(), r.Ptr() + r.bytes);
        json obs = {{"vtx_after", vtxAfter}, {"idx_after", idxAfter}, {"ci_after", ciAfter},
                    {"vertices_fnv", Crc32(buf + v0, vlen)}, {"indices_fnv", Crc32(buf + i0, ilen)}, {"colors_fnv", colorsFnv},
                    {"rest_ok", rest}, {"small_ok", smallBefore == smallAfter}, {"norm_after", HexBytes(reinterpret_cast<void*>(kNormAddr), kNormBytes)},
                    {"events", EventsObserved()}};
        (void)normBefore;
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    if (fn == 0) CaseQuad(env, t, n);
    else if (fn == 1) CaseGround(env, t, n);
    else CaseWater(env, t, fn, n);
}

// ================================================================================================ ejecucion
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
        for (const auto& s : env.stubs[f]) {
            if (s.pair.hooked) ready &= s.pair.dll != 0 && ModuleOf(s.pair.dll) == env.asi;
            else ready &= s.pair.dll == 0 || ModuleOf(s.pair.dll) == env.asi;
        }
    }
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR},
        {"layout", {{"buf", {kBufAddr, kBufBytes}}, {"col", {kColAddr, kColBytes}}, {"combo", {kComboAddr, kComboBytes}}, {"vertex_bytes", kVertexBytes}, {"vtx_off", kVtxOff}, {"idx_off", kIdxOff}}}});
    Totals totals;
    if (ready) {
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CaseRun(env, totals, f, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", kCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact() && RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact = env.WatchesIntact() && RegionsIntact();
    bool stateRestored = entriesIntact;
    for (auto& r : env.collab) if (r) stateRestored &= r->Restore();
    for (auto& r : env.route) if (r) stateRestored &= r->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
}

// Busca el hook Simple registrado para una direccion del EXE (categoria y nombre), para crear su ruta (apagada) y obtener el cuerpo de la DLL.
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items())
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
            }
    });
    return found;
}
// Colaborador: si la DLL tiene hook (apagado) se parchea tambien el cuerpo de la DLL; si no (dll = 0), la DLL llama a la direccion del EXE.
inline Pair MakeCollab(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name;
    if (FindHook(exe, cat, name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn91a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn91a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn91a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name;
        const bool found = FindHook(kEntries[f], cat, name);
        if (found) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    const Pair flush = MakeCollab(env, 0x6E7680, hooksFound);
    const Pair waves = MakeCollab(env, 0x6E6EF0, hooksFound);
    const Pair ground = MakeCollab(env, 0x5674E0, hooksFound);
    env.stubs[0] = {};
    env.stubs[1] = {StubSpec{ground, reinterpret_cast<void*>(&FkGround)}};
    env.stubs[2] = {StubSpec{flush, reinterpret_cast<void*>(&FkFlush)}, StubSpec{waves, reinterpret_cast<void*>(&FkWaves)}};
    env.stubs[3] = env.stubs[2];
    const char* real[kFunctions] = {
        "toda la aritmetica de bloques y combinaciones (sin colaboradores)",
        "suma z + rango y ProcessVerticalLine solo como falso; lectura de la z y de la superficie del CColPoint",
        "toda la aritmetica de la rejilla, la camara, la atenuacion, los colores, los vertices y los indices; solo el vaciado del bufer y el calculo de olas son falsos",
        "toda la aritmetica de las filas, la camara, la atenuacion, los colores, los vertices y los indices; solo el vaciado del bufer y el calculo de olas son falsos"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({real[f]})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
