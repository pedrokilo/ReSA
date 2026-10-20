#pragma once
// Preparacion, observacion y restauracion de los casos del banco StatsWrappers60 (compartido por Core.hpp, el banco dentro del juego, y por el arnes offline).
// Requiere, declarado antes: json (nlohmann). Cada funcion tiene Input*(fixture) -> JSON de entrada (lo unico que ve el oraculo), Setup*(fixture) (escribe el estado del juego y
// el contexto de los falsos) y Observe*(fixture) -> JSON de salida. `RunCase` orquesta: restaurar base, preparar, llamar (con las compuertas del llamador), observar y restaurar.
#include <functional>
#include "Fakes.hpp"

namespace Stw60 {
struct Reg { u32 eax, ecx, edx, eflags; bool stack, nonvolatile; };

inline std::string HexBytes(const void* p, std::size_t n) {
    constexpr char d[] = "0123456789abcdef";
    const auto* b = static_cast<const u8*>(p);
    std::string o; o.reserve(n * 2);
    for (std::size_t i = 0; i < n; ++i) { o += d[b[i] >> 4]; o += d[b[i] & 15]; }
    return o;
}
inline std::string Hex64(u64 v) { char t[20]; std::snprintf(t, sizeof t, "%016llx", static_cast<unsigned long long>(v)); return t; }
inline json BufObs(u32 address, u32 size) {
    return {{"h", Hex64(Fnv64(Mem(address), size))}, {"t", HexBytes(Mem(address), size < 64 ? size : 64)}};
}
inline json TraceJson() {
    json t = json::array();
    for (unsigned i = 0; i < nev; ++i) t.push_back(json::array({evs[i].kind, evs[i].a, evs[i].b, evs[i].c, evs[i].d, evs[i].e, evs[i].f}));
    return t;
}
inline json AbiJson(const Abi& a) { return {{"eax", a.eax}, {"ecx", a.ecx}, {"edx", a.edx}, {"stack_seed", a.stack}}; }

// ------------------------------------------------------------------------------------------------ orquestacion
template<class Setup, class Call, class Observe>
json RunCase(Snapshot& base, const std::vector<Region>& allowed, Setup&& setup, Call&& call, Observe&& observe, Reg& reg) {
    base.Restore();
    ResetTrace();
    ctx = Ctx{};
    setup();
    Snapshot before; before.Take();
    reg = call();
    json obs = observe();
    obs["trace"] = TraceJson();
    obs["trace_dropped"] = dropped;
    const u32 outside = FirstOutside(before, allowed);
    obs["outside_ok"] = outside == 0;
    obs["first_outside"] = outside;
    base.Restore();
    return obs;
}

// ------------------------------------------------------------------------------------------------ FindLeastFavoriteRadioStation
inline json InputFind(const FxFind& f) {
    json list = json::array();
    for (i32 v : f.list) list.push_back(v);
    json j = {{"family", f.family}, {"list", list}}; j.update(AbiJson(f.abi));
    return j;
}
inline void SetupFind(const FxFind& f) {
    for (unsigned i = 0; i < 14; ++i) Poke32(0xB78E58 + 4 * i, static_cast<u32>(f.list[i]));
}
inline json ObserveFind(const Reg& reg) { return {{"ret", reg.eax}}; }
inline const std::vector<Region> kAllowedFind{};

// ------------------------------------------------------------------------------------------------ BuildStatLine
constexpr u32 kBuildArena = 1024, kBuildLineAt = 0x40, kBuildP1At = 0x240, kBuildP2At = 0x280;
inline std::array<u8, kBuildArena> buildArena{};
inline std::array<u8, kBuildArena> buildArenaRef{};
inline json InputBuild(const FxBuild& f) {
    json j = {{"group", f.group}, {"line", HexBytes(f.line.data(), f.line.size())}, {"line_null", f.lineNull}, {"p1_null", f.p1Null}, {"p2_null", f.p2Null},
              {"v1", f.v1}, {"v2", f.v2}, {"metrics", f.metrics}, {"type", f.type}, {"metric", f.metric}, {"pat", f.pat}};
    j.update(AbiJson(f.abi));
    return j;
}
struct BuildArgs { const char* line; void* p1; i32 metrics; void* p2; i32 type; };
inline BuildArgs SetupBuild(const FxBuild& f) {
    FillBuffers(f.pat);
    buildArena.fill(0xB7);
    std::memcpy(buildArena.data() + kBuildLineAt, f.line.c_str(), f.line.size() + 1);
    std::memcpy(buildArena.data() + kBuildP1At, &f.v1, 4);
    std::memcpy(buildArena.data() + kBuildP2At, &f.v2, 4);
    buildArenaRef = buildArena;
    ctx.metric = f.metric;
    return BuildArgs{ f.lineNull ? nullptr : reinterpret_cast<const char*>(buildArena.data() + kBuildLineAt), f.p1Null ? nullptr : buildArena.data() + kBuildP1At, f.metrics,
                      f.p2Null ? nullptr : buildArena.data() + kBuildP2At, f.type };
}
inline json ObserveBuild() {
    return {{"s2", BufObs(kAddrGString2, kLenGString)}, {"s1", BufObs(kAddrGString, kLenGString)}, {"g1", BufObs(kAddrGxt1, kLenGxt)}, {"g2", BufObs(kAddrGxt2, kLenGxt)},
            {"args_ok", buildArena == buildArenaRef}};
}
inline const std::vector<Region> kAllowedBuild{ {kAddrGString2, kLenGString}, {kAddrGxt1, kLenGxt}, {kAddrGxt2, kLenGxt} };

// ------------------------------------------------------------------------------------------------ UpdateRespectStat
inline json InputResp(const FxResp& f) {
    json j = {{"family", f.family}, {"arg0", f.arg0}, {"respect", f.respect}, {"girl", f.girl}, {"w1", f.w1}, {"w2", f.w2}, {"territory", f.territory},
              {"missions", f.missions}, {"total", f.total}, {"money", json::array({f.money[0], f.money[1]})}, {"focus", f.focus}, {"cheat", f.cheat},
              {"low", f.low}, {"high", f.high}, {"prev_total", f.prevTotal}, {"tagged", f.tagged}, {"threshold", f.threshold}};
    j.update(AbiJson(f.abi));
    return j;
}
inline void SetupResp(const FxResp& f) {
    Poke32(0xB79490, f.respect); Poke32(0xB79484, f.girl); Poke32(0xB793DC, f.w1); Poke32(0xB79488, f.w2); Poke32(0xB79480, f.prevTotal);
    Poke32(0x96AB9C, f.territory);
    Poke32(0xB791A0, static_cast<u32>(f.missions)); Poke32(0xB791B0, static_cast<u32>(f.total));
    Poke32(0xB7CE50, static_cast<u32>(f.money[0])); Poke32(0xB7CE50 + 0x190, static_cast<u32>(f.money[1]));
    *Mem(0xB7CD74) = static_cast<u8>(f.focus);
    *Mem(0x96917F) = static_cast<u8>(f.cheat);
    Poke32(0x8CDEC4, f.low); Poke32(0x8CDEC8, f.high);
    ctx.tagged = f.tagged; ctx.threshold = f.threshold;
}
inline json ObserveResp() { return {{"total", Peek32(0xB79480)}, {"low", Peek32(0x8CDEC4)}, {"high", Peek32(0x8CDEC8)}}; }
inline const std::vector<Region> kAllowedResp{ {0xB79480, 4}, {0x8CDEC4, 8} };

// ------------------------------------------------------------------------------------------------ ConstructStatLine
// Arena del jugador falso: el EXE direcciona ped + ranura*0x1C + 0x5A0 con la ranura en -128..127 (de -2144 a +5000 bytes alrededor del jugador) y los campos 0x548 y 0x718.
constexpr u32 kPedArena = 8192, kPedAt = 3072;
static_assert(kPedAt >= 2144 + 16 && kPedArena - kPedAt >= 5000 + 16, "la arena del jugador falso no cubre todas las ranuras");
inline std::array<u8, kPedArena> pedArena{};
inline std::array<u8, kPedArena> pedArenaRef{};
inline json InputCons(const FxCons& f) {
    json gang = json::array();
    for (u32 g : f.gang) gang.push_back(g);
    json ws = json::array();
    for (u32 w : f.ws) ws.push_back(w);
    json j = {{"family", f.family}, {"arg0", f.arg0}, {"arg1", f.arg1}, {"synth", f.synth}, {"tab", f.synth ? HexBytes(f.tab.data(), kTableBytes) : std::string()},
              {"words", HexBytes(f.words.data(), sizeof(f.words))}, {"fl", HexBytes(f.fl.data(), sizeof(f.fl))},
              {"a9a89c", f.a9a89c}, {"a9ad70", f.a9ad70}, {"a9ad74", f.a9ad74}, {"timer", f.timer}, {"gang", gang}, {"territory", f.territory},
              {"german", f.german}, {"metric", f.metric}, {"populate", f.populate}, {"most", f.most}, {"least", f.least},
              {"ped_seed", f.pedSeed}, {"ped_slot", f.pedSlot}, {"ped_type", f.pedType}, {"ped_armor", f.pedArmor}, {"ws", ws},
              {"mins", f.mins}, {"secs", f.secs}, {"progress", f.progress}, {"pat", f.pat}, {"bar0", static_cast<u32>(f.pat >> 7) & 0xFFFFu}};
    j.update(AbiJson(f.abi));
    return j;
}
inline void SetupCons(const FxCons& f) {
    FillBuffers(f.pat);
    std::memcpy(Mem(kStatWordsBase), f.words.data(), sizeof(f.words));
    std::memcpy(Mem(kStatFloatsBase), f.fl.data(), sizeof(f.fl));
    Poke32(0xA9A89C, f.a9a89c); Poke32(0xA9AD70, f.a9ad70); Poke32(0xA9AD74, f.a9ad74); Poke32(0xB7CB84, f.timer);
    for (unsigned i = 0; i < 6; ++i) Poke32(0x96AB2C + 4 * i, f.gang[i]);
    Poke32(0x96AB9C, f.territory);
    {   // jugador falso: relleno determinista y los tres campos que lee la funcion
        for (u32 i = 0; i < kPedArena; i += 4) { const u32 w = Rand(f.pedSeed, i / 4); std::memcpy(pedArena.data() + i, &w, 4); }
        u8* ped = pedArena.data() + kPedAt;
        std::memcpy(ped + 0x548, &f.pedArmor, 4);
        const u32 type = f.pedType;
        std::memcpy(ped + static_cast<std::ptrdiff_t>(static_cast<i8>(f.pedSlot)) * 0x1C + 0x5A0, &type, 4);
        ped[0x718] = static_cast<u8>(f.pedSlot);
        pedArenaRef = pedArena;
        ctx.ped = ped;
        Poke32(0xB7CD98, static_cast<u32>(reinterpret_cast<std::uintptr_t>(ped)));
    }
    *reinterpret_cast<u16*>(Mem(0xB794CC)) = static_cast<u16>((f.pat >> 7) & 0xFFFFu);
    if (f.synth) std::memcpy(Mem(kTableDefault), f.tab.data(), kTableBytes);
    ctx.german = f.german; ctx.metric = f.metric; ctx.populate = f.populate; ctx.most = f.most; ctx.least = f.least;
    ctx.progress = f.progress; ctx.mins = f.mins; ctx.secs = f.secs; ctx.ws = f.ws;
}
inline json ObserveCons(const Reg& reg) {
    return {{"ret", reg.eax}, {"bar", static_cast<u32>(*reinterpret_cast<const u16*>(Mem(0xB794CC)))},
            {"s2", BufObs(kAddrGString2, kLenGString)}, {"s1", BufObs(kAddrGString, kLenGString)}, {"g1", BufObs(kAddrGxt1, kLenGxt)}, {"g2", BufObs(kAddrGxt2, kLenGxt)},
            {"ped_ok", pedArena == pedArenaRef}};
}
inline const std::vector<Region> kAllowedCons{ {kAddrGString, kLenGString}, {kAddrGString2, kLenGString}, {kAddrGxt1, kLenGxt}, {kAddrGxt2, kLenGxt}, {0xB794CC, 2} };
}
