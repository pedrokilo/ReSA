#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h"
#include "../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.h"
#include "../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h"
#include "../../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "../../source/game_sa/Interior/InteriorInfo_t.h"
#include "../../source/game_sa/Interior/Interior_c.h"
#include "../../source/game_sa/Interior/InteriorGroup_c.h"
#include "../../source/game_sa/Interior/InteriorManager_c.h"
#include "../../source/game_sa/Animation/AnimManager.h"
#include "../../source/game_sa/PedIntelligence.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"

// Banco local de:
//   Fase A: InteriorManager_c::AreAnimsLoaded (0x5980F0, __thiscall, ret 4).
//   Fase B: CTaskInteriorUseInfo::CreateFirstSubTask (0x675B60) y
//           CTaskInteriorUseInfo::CreateNextSubTask (0x676880).
// Alterna los hooks ya instalados por InjectHooksMain.cpp (NO llama a ningún
// InjectHooks(): eso duplicaría el registro y dispararía HookCategory.h:60).
//
// Lado original: hook de la función bajo prueba deshabilitado y, en la fase B,
// también el de AreAnimsLoaded (camino 100 % ensamblador original salvo los
// constructores/AddTask/Say/new ya reconstruidos a los que llama el original).
// Lado candidato: ambos hooks habilitados.
//
// Dependencias controladas (iguales en ambos lados):
//  - CTask::operator new/delete (0x61A5A0/0x61A5B0 y sus entradas del DLL):
//    arena propia por lado, rellenada con 0xCD antes de cada llamada. Permite
//    comparar byte a byte los objetos devueltos (incluido el relleno) y no toca
//    el pool real de tareas. delete solo cuenta/registra.
//  - CPed::Say (0x5EFFE0 y su entrada del DLL): registra contexto y argumentos;
//    no se reproduce audio.
//  - CAnimBlock::IsLoaded de "int_house"/"int_shop"/"int_office": se fija por
//    caso y se restaura el valor previo inmediatamente después de cada lado.
namespace UseInfoLive {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline int Report(int type, char* msg, int*) {
    if (type != _CRT_ASSERT) return FALSE;
    Write({{"event", "assertion"}, {"message", msg ? msg : ""}});
    ExitProcess(91);
}

template<class T> struct Storage {
    alignas(T) std::array<uint8, sizeof(T)> bytes{};
    T* get() { return reinterpret_cast<T*>(bytes.data()); }
    const T* get() const { return reinterpret_cast<const T*>(bytes.data()); }
};

template<class M> uintptr_t MemberAddress(M m) {
    static_assert(sizeof(M) == sizeof(uintptr_t));
    uintptr_t a{};
    std::memcpy(&a, &m, sizeof(a));
    return a;
}

// ---------------------------------------------------------------- arena
constexpr size_t kArenaSize = 0x800;
struct Arena {
    alignas(16) std::array<uint8, kArenaSize> bytes{};
    size_t used{};
    std::vector<std::pair<size_t, size_t>> allocs;
    std::vector<int> deletes;
    void Reset() { bytes.fill(0xCD); used = 0; allocs.clear(); deletes.clear(); }
    bool Contains(uintptr_t p) const { return p >= uintptr_t(bytes.data()) && p < uintptr_t(bytes.data()) + kArenaSize; }
};
inline Arena* arena{};
inline unsigned foreignDeletes{};
inline void* __cdecl FakeNew(size_t size) {
    auto& a = *arena;
    const size_t off = (a.used + 15) & ~size_t(15);
    if (off + size > kArenaSize) std::abort();
    a.used = off + size;
    a.allocs.emplace_back(off, size);
    return a.bytes.data() + off;
}
inline void __cdecl FakeDelete(void* p) {
    if (!p) return;
    if (arena && arena->Contains(uintptr_t(p))) arena->deletes.push_back(int(uintptr_t(p) - uintptr_t(arena->bytes.data())));
    else ++foreignDeletes;
}

// ---------------------------------------------------------------- Say
inline json sayEvents = json::array();
inline CPed* sayPed{};
struct SayFake {
    int16 Say(eGlobalSpeechContext ctx, uint32 delay, float probability, bool overrideSilence, bool forceAudible, bool frontEnd) {
        sayEvents.push_back({int(ctx), delay, probability, overrideSilence, forceAudible, frontEnd, reinterpret_cast<CPed*>(this) == sayPed});
        return 0;
    }
};

// ---------------------------------------------------------------- bloques de animación
// CAnimManager::ms_aAnimBlocks (0xB5D4A0) y ms_numAnimBlocks (0xB4EA30) son privados;
// se leen por su dirección original, igual que el ensamblador de 0x5980F0.
inline CAnimBlock* AnimBlocks() { return reinterpret_cast<CAnimBlock*>(0xB5D4A0); }
inline uint32 NumAnimBlocks() { return *reinterpret_cast<uint32*>(0xB4EA30); }
struct Blocks {
    std::array<int32, 3> idx{};
    bool valid{};
    void Resolve() {
        idx = {CAnimManager::GetAnimationBlockIndex("int_house"), CAnimManager::GetAnimationBlockIndex("int_shop"), CAnimManager::GetAnimationBlockIndex("int_office")};
        valid = std::ranges::all_of(idx, [](int32 i) { return i >= 0 && uint32(i) < NumAnimBlocks(); });
    }
    std::array<bool, 3> Get() const { return {AnimBlocks()[idx[0]].IsLoaded, AnimBlocks()[idx[1]].IsLoaded, AnimBlocks()[idx[2]].IsLoaded}; }
    void Set(std::array<bool, 3> v) const { for (int i = 0; i < 3; ++i) AnimBlocks()[idx[i]].IsLoaded = v[i]; }
};
inline Blocks blocks;

// ---------------------------------------------------------------- fase A
// Llamada __thiscall cruda con medida de la pila: un candidato cdecl (`ret` sin
// limpiar) dejaría esp 4 bytes por debajo; el original y un candidato correcto
// dejan delta 0.
inline bool CallAnims(int32 arg, int32* espDelta) {
    uint32 before{}, after{};
    uint8 r{};
    InteriorManager_c* self = &g_interiorMan;
    uint32 fn = 0x5980F0;
    __asm {
        mov before, esp
        push arg
        mov ecx, self
        call fn
        mov r, al
        mov after, esp
        mov esp, before
    }
    *espDelta = int32(before) - int32(after);
    return r != 0;
}

// ---------------------------------------------------------------- fase B
struct Case {
    bool first{};      // CreateFirstSubTask (true) / CreateNextSubTask (false)
    bool nullInfo{};
    int8 type{};
    bool instantly{};
    bool gotoSub{};
    bool eic{};
    bool anims{};
    bool keeperShop{};
    int32 dur{};
    json Describe() const {
        return {{"fn", first ? "CreateFirstSubTask" : "CreateNextSubTask"}, {"nullInfo", nullInfo}, {"type", type}, {"instantly", instantly},
                {"gotoSub", gotoSub}, {"eic", eic}, {"anims", anims}, {"keeperShop", keeperShop}, {"dur", dur}};
    }
};

inline std::vector<Case> BuildCases() {
    std::vector<Case> cs;
    for (bool inst : {false, true}) cs.push_back({.first = true, .nullInfo = true, .instantly = inst, .dur = 1500});
    for (int8 t = 0; t <= 11; ++t)
        for (bool inst : {false, true}) cs.push_back({.first = true, .type = t, .instantly = inst, .dur = 1500 + t * 10});
    for (bool sub : {false, true}) cs.push_back({.first = false, .nullInfo = true, .gotoSub = sub, .dur = 1500});
    for (int8 t = 0; t <= 11; ++t)
        for (bool sub : {false, true})
            for (bool inst : {false, true})
                for (bool eic : {false, true})
                    for (bool anims : {false, true})
                        for (bool shop : {false, true})
                            cs.push_back({.first = false, .type = t, .instantly = inst, .gotoSub = sub, .eic = eic, .anims = anims, .keeperShop = shop, .dur = 2000 + t * 10});
    return cs;
}

struct Side {
    Storage<CPlayerPed>              ped;
    Storage<CPed>                    keeper;
    Storage<CPedIntelligence>        intel;
    Storage<Interior_c>              interior;
    Storage<InteriorGroup_c>         group;
    InteriorInfo_t                   info{};
    Storage<CTaskInteriorUseInfo>    task;
    Storage<CTaskInteriorGoToInfo>   gotoSub;
    Storage<CTaskSimpleStandStill>   otherSub;
    Storage<CTaskInteriorShopKeeper> shopKeeperTask;
    Storage<CTaskSimpleStandStill>   otherKeeperTask;
    CTask*                           keeperTask{};
    CTask*                           subTask{};
    std::array<uint8, 16>            dummyEntity{};
    std::array<uint8, 16>            dummyIgnored{};
    Arena                            arena;
    std::vector<uint8>               pedBefore;

    void Setup(const Case& c, CPlayerPed* source) {
        std::memcpy(ped.get(), source, sizeof(CPlayerPed));
        ped.get()->m_pEntityIgnoredCollision = reinterpret_cast<CEntity*>(dummyIgnored.data());

        info.Type = static_cast<eInteriorInfoType>(c.type);
        info.IsInUse = false;
        info.Pos = CVector{1.f, 2.f, 3.f};
        info.Dir = CVector{0.f, 1.f, 0.f};
        info.EntityIgnoredCollision = c.eic ? reinterpret_cast<CEntity*>(dummyEntity.data()) : nullptr;

        // Desplazamientos tal como los lee el ensamblador original:
        // Interior_c+0xC -> InteriorGroup_c*, InteriorGroup_c+0x38 -> m_peds[0],
        // CPed+0x47C -> CPedIntelligence*, CPedIntelligence+0x14 -> tarea primaria 4.
        *reinterpret_cast<InteriorGroup_c**>(interior.bytes.data() + 0xC) = group.get();
        *reinterpret_cast<CPed**>(group.bytes.data() + 0x38) = keeper.get();
        *reinterpret_cast<CPedIntelligence**>(keeper.bytes.data() + 0x47C) = intel.get();
        keeperTask = c.keeperShop
            ? static_cast<CTask*>(::new (shopKeeperTask.get()) CTaskInteriorShopKeeper(group.get(), false))
            : static_cast<CTask*>(::new (otherKeeperTask.get()) CTaskSimpleStandStill(0, false, false, 8.f));
        *reinterpret_cast<CTask**>(intel.bytes.data() + 0x14) = keeperTask;

        subTask = c.gotoSub
            ? static_cast<CTask*>(::new (gotoSub.get()) CTaskInteriorGoToInfo(&info, interior.get(), false))
            : static_cast<CTask*>(::new (otherSub.get()) CTaskSimpleStandStill(0, false, false, 8.f));
        ::new (task.get()) CTaskInteriorUseInfo(c.nullInfo ? nullptr : &info, interior.get(), c.dur, c.instantly);
        *reinterpret_cast<CTask**>(task.bytes.data() + 8) = subTask; // CTaskComplex::m_pSubTask (protegido)

        arena.Reset();
        pedBefore = ReversingProbe::Bytes(ped.get(), sizeof(CPlayerPed));
    }

    uint32 Norm(uint32 v) const {
        const auto is = [&](const void* p) { return v == uint32(uintptr_t(p)); };
        if (arena.Contains(v)) return 0xA0000000u | uint32(v - uintptr_t(arena.bytes.data()));
        if (is(&info)) return 0xB0000001u;
        if (is(interior.get())) return 0xB0000002u;
        if (is(ped.get())) return 0xB0000003u;
        if (is(keeperTask)) return 0xB0000004u;
        if (is(dummyEntity.data())) return 0xB0000005u;
        if (is(dummyIgnored.data())) return 0xB0000006u;
        if (is(group.get())) return 0xB0000007u;
        if (is(keeper.get())) return 0xB0000008u;
        if (is(subTask)) return 0xB0000009u;
        if (is(task.get())) return 0xB000000Au;
        return v;
    }
    json Dwords(const void* p, size_t from, size_t to) const {
        json out = json::array();
        for (size_t o = from; o + 4 <= to; o += 4) {
            uint32 v; std::memcpy(&v, static_cast<const uint8*>(p) + o, 4);
            out.push_back(Norm(v));
        }
        return out;
    }
    json PedChanges() const {
        json out = json::array();
        const auto* now = reinterpret_cast<const uint8*>(ped.get());
        for (size_t o = 0; o + 4 <= sizeof(CPlayerPed); o += 4) {
            if (std::memcmp(now + o, pedBefore.data() + o, 4)) {
                uint32 v; std::memcpy(&v, now + o, 4);
                out.push_back({o, Norm(v)});
            }
        }
        return out;
    }
    json Snapshot(CTask* result) {
        json allocs = json::array();
        for (auto [off, size] : arena.allocs) {
            auto* obj = reinterpret_cast<CTask*>(arena.bytes.data() + off);
            allocs.push_back({{"offset", off}, {"size", size}, {"taskType", int(obj->GetTaskType())}, {"fields", Dwords(obj, 4, size)}});
        }
        const size_t keeperSize = keeperTask == static_cast<void*>(shopKeeperTask.get()) ? sizeof(CTaskInteriorShopKeeper) : sizeof(CTaskSimpleStandStill);
        return {
            {"result", result ? json(Norm(uint32(uintptr_t(result)))) : json(nullptr)},
            {"resultInArena", result ? arena.Contains(uintptr_t(result)) : true},
            {"allocations", allocs},
            {"useInfo", Dwords(task.get(), 4, sizeof(CTaskInteriorUseInfo))},
            {"infoType", int(*reinterpret_cast<const int8*>(&info))},
            {"infoIsInUse", info.IsInUse},
            {"infoEic", Norm(uint32(uintptr_t(info.EntityIgnoredCollision)))},
            {"pedChanges", PedChanges()},
            {"pedEntityIgnoredCollision", Norm(uint32(uintptr_t(ped.get()->m_pEntityIgnoredCollision)))},
            {"keeperTask", Dwords(keeperTask, 4, keeperSize)},
            {"keeperStartServeAnim", keeperTask == static_cast<void*>(shopKeeperTask.get()) ? json(bool(shopKeeperTask.bytes[0xC])) : json(nullptr)},
            {"say", sayEvents},
        };
    }
};

inline CTask* SehCallTask(uint32 address, CTaskInteriorUseInfo* task, CPed* ped, bool* ok, unsigned* code) {
    __try {
        *ok = true;
        return reinterpret_cast<CTask*(__thiscall*)(CTaskInteriorUseInfo*, CPed*)>(address)(task, ped);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        *ok = false;
        *code = GetExceptionCode();
        return nullptr;
    }
}
inline bool SehDelete(CTask* t, unsigned* code) {
    __try {
        delete t;
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        *code = GetExceptionCode();
        return false;
    }
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(std::string_view cat, std::string_view name) {
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        if (c.Name() == cat) {
            for (auto& h : c.Items()) if (h->Name() == name) hook = h.get();
        }
    });
    return hook;
}

struct HookGuard {
    ReversibleHooks::ReversibleHook::Base* h;
    bool enabled, locked;
    explicit HookGuard(ReversibleHooks::ReversibleHook::Base* hook) : h(hook), enabled(hook->Hooked()), locked(hook->Locked()) { h->LockState(false); }
    void Restore() { h->State(enabled); h->LockState(locked); }
    bool Restored() const { return h->Hooked() == enabled && h->Locked() == locked; }
};

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{};
    GetModuleFileNameA(nullptr, exe, MAX_PATH);
    const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "interioruseinfo-runtime.start")) { done = true; return; }
    if (!log.is_open()) {
        log.open(dir / "interioruseinfo-runtime.jsonl");
        Write({{"event", "waiting_for_player"}});
    }
    auto* player = FindPlayerPed();
    if (!player || !player->m_matrix || CCutsceneMgr::IsRunning()) return;
    done = true;
    auto report = _CrtSetReportHook(Report);

    auto* animsHook = FindHook("InteriorManager_c", "AreAnimsLoaded");
    auto* firstHook = FindHook("CTaskInteriorUseInfo", "CreateFirstSubTask");
    auto* nextHook  = FindHook("CTaskInteriorUseInfo", "CreateNextSubTask");
    if (!animsHook || !firstHook || !nextHook) {
        Write({{"event", "error"}, {"reason", "missing hook"}, {"anims", animsHook != nullptr}, {"first", firstHook != nullptr}, {"next", nextHook != nullptr}});
        _CrtSetReportHook(report);
        return;
    }
    blocks.Resolve();
    if (!blocks.valid) {
        Write({{"event", "error"}, {"reason", "anim blocks"}, {"idx", blocks.idx}, {"num", NumAnimBlocks()}});
        _CrtSetReportHook(report);
        return;
    }
    const auto loadedBefore = blocks.Get();
    const auto livePed = ReversingProbe::Bytes(player, sizeof(CPlayerPed));
    HookGuard ga(animsHook), gf(firstHook), gn(nextHook);
    Write({{"event", "player_ready"}, {"independent_inputs", true}, {"num_anim_blocks", NumAnimBlocks()},
           {"block_idx", blocks.idx}, {"loaded_before", loadedBefore},
           {"hooks_before", {ga.enabled, gf.enabled, gn.enabled}}});

    bool passed = true;
    int pairsA = 0, pairsB = 0;

    // ------------------------------------------------------------ fase A
    {
        const auto RunA = [&](int32 arg, const json& ctx) {
            if (!passed) return;
            const auto s0 = InteriorUseInfoProbe::animsCalls;
            animsHook->State(false);
            int32 dO{}; const bool rO = CallAnims(arg, &dO);
            const auto s1 = InteriorUseInfoProbe::animsCalls;
            animsHook->State(true);
            int32 dC{}; const bool rC = CallAnims(arg, &dC);
            const auto s2 = InteriorUseInfoProbe::animsCalls;
            const bool ok = rO == rC && dO == 0 && dC == 0 && s1 == s0 && s2 == s1 + 1;
            passed = passed && ok;
            Write({{"event", "comparison"}, {"phase", "A"}, {"pair", pairsA}, {"arg", arg}, {"context", ctx},
                   {"original", rO}, {"candidate", rC}, {"original_esp_delta", dO}, {"candidate_esp_delta", dC},
                   {"original_candidate_calls", s1 - s0}, {"candidate_calls", s2 - s1}, {"equal", rO == rC}, {"passed", ok}});
            ++pairsA;
        };
        for (int32 a = 0; a < int32(NumAnimBlocks()); ++a) RunA(a, "natural");
        for (int mask = 0; mask < 8; ++mask) {
            blocks.Set({bool(mask & 1), bool(mask & 2), bool(mask & 4)});
            for (int32 a : {0, 1, 2}) RunA(a, json{{"mask", mask}});
            for (int32 i : blocks.idx) if (i >= 3) RunA(i, json{{"mask", mask}});
            blocks.Set(loadedBefore);
        }
        ga.Restore();
    }

    // ------------------------------------------------------------ fase B
    if (passed) {
        ReversingProbe::EntryPatch newGta(0x61A5A0, FakeNew);
        ReversingProbe::EntryPatch delGta(0x61A5B0, FakeDelete);
        ReversingProbe::EntryPatch newDll(std::bit_cast<uintptr_t>(static_cast<void* (*)(size_t)>(&CTask::operator new)), FakeNew);
        ReversingProbe::EntryPatch delDll(std::bit_cast<uintptr_t>(static_cast<void (*)(void*)>(&CTask::operator delete)), FakeDelete);
        ReversingProbe::EntryPatch sayGta(0x5EFFE0, reinterpret_cast<void*>(MemberAddress(&SayFake::Say)));
        ReversingProbe::EntryPatch sayDll(MemberAddress(&CPed::Say), reinterpret_cast<void*>(MemberAddress(&SayFake::Say)));

        const auto cases = BuildCases();
        auto a = std::make_unique<Side>();
        auto b = std::make_unique<Side>();
        for (size_t i = 0; i < cases.size() && passed; ++i) {
            const auto& c = cases[i];
            auto* hook = c.first ? firstHook : nextHook;
            const uint32 address = c.first ? 0x675B60 : 0x676880;
            auto& counter = c.first ? InteriorUseInfoProbe::firstCalls : InteriorUseInfoProbe::nextCalls;

            const auto RunSide = [&](Side& s, bool candidate, json& snap, json& post, unsigned& calls, unsigned& animCalls) -> bool {
                s.~Side(); ::new (&s) Side{};
                s.Setup(c, player);
                blocks.Set({c.anims, c.anims, c.anims});
                arena = &s.arena;
                sayEvents = json::array();
                sayPed = s.ped.get();
                foreignDeletes = 0;
                hook->State(candidate);
                animsHook->State(candidate);
                const auto c0 = counter, a0 = InteriorUseInfoProbe::animsCalls;
                bool ok{}; unsigned code{};
                CTask* r = SehCallTask(address, s.task.get(), s.ped.get(), &ok, &code);
                calls = counter - c0; animCalls = InteriorUseInfoProbe::animsCalls - a0;
                blocks.Set(loadedBefore);
                if (!ok) { Write({{"event", "error"}, {"reason", candidate ? "seh candidate" : "seh original"}, {"pair", i}, {"code", code}}); return false; }
                snap = s.Snapshot(r);
                unsigned dcode{};
                if (r && !SehDelete(r, &dcode)) { Write({{"event", "error"}, {"reason", "seh delete"}, {"pair", i}, {"code", dcode}}); return false; }
                post = {{"deletes", s.arena.deletes}, {"foreignDeletes", foreignDeletes}, {"infoIsInUse", s.info.IsInUse},
                        {"pedChanges", s.PedChanges()}, {"keeperTask", s.Dwords(s.keeperTask, 4, 0x10)}};
                arena = nullptr;
                return true;
            };
            json so, po, sc, pc;
            unsigned co{}, ao{}, cc{}, ac{};
            if (!RunSide(*a, false, so, po, co, ao) || !RunSide(*b, true, sc, pc, cc, ac)) { passed = false; break; }
            const bool equal = so == sc && po == pc;
            const bool ok = equal && co == 0 && cc == 1 && ao == 0;
            passed = passed && ok;
            Write({{"event", "comparison"}, {"phase", "B"}, {"pair", i}, {"case", c.Describe()},
                   {"original", so}, {"candidate", sc}, {"original_post_delete", po}, {"candidate_post_delete", pc},
                   {"equal", equal}, {"original_candidate_calls", co}, {"candidate_calls", cc},
                   {"original_anims_candidate_calls", ao}, {"candidate_anims_calls", ac}, {"passed", ok}});
            ++pairsB;
        }
        arena = nullptr;
        sayPed = nullptr;
    }

    blocks.Set(loadedBefore);
    ga.Restore(); gf.Restore(); gn.Restore();
    const bool restored = ga.Restored() && gf.Restored() && gn.Restored()
        && blocks.Get() == loadedBefore
        && ReversingProbe::Bytes(player, sizeof(CPlayerPed)) == livePed;
    Write({{"event", passed && restored ? "complete" : "error"}, {"pairs_a", pairsA}, {"pairs_b", pairsB}, {"state_restored", restored}});
    _CrtSetReportHook(report);
}
}
