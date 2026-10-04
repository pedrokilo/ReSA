#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Fx/FxPrimBP.h"
#include "Fx/FxEmitterPrt.h"
#include "Probe.hpp"
namespace FxManagerLive {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& value) { { static DWORD pumpLast=0; const DWORD pumpNow=GetTickCount(); if(pumpNow-pumpLast>=250){ pumpLast=pumpNow; MSG pumpMsg; PeekMessageA(&pumpMsg,nullptr,0,0,PM_NOREMOVE); } } log << value.dump() << '\n'; log.flush(); }
inline int Report(int type, char* message, int*) {
    if (type != _CRT_ASSERT) return FALSE;
    Write({{"event", "assertion"}, {"message", message ? message : ""}}); ExitProcess(91);
}
struct Fixture {
    FxManager_c manager{};
    FxSystemBP_c empty{};
    std::vector<FxSystem_c*> systems;
    FxSystem_c* foreign{};
    std::vector<FxPrimBP_c*> prims;
    std::vector<TList_c<Particle_c>> lists;
    std::vector<FxEmitterPrt_c> particles;
    std::vector<std::vector<uint8>> payloads;
    bool attached{};
    Fixture(int count, int mode, int density, bool ownMatrix, const std::vector<FxSystemBP_c*>& blueprints) {
        empty.m_nNameKey = 0; empty.m_nNumPrims = 0; empty.m_Prims = nullptr;
        empty.m_fLength = 1.f;
        for (int i = 0; i < count; ++i) {
            auto* bp = mode == 0 ? &empty : blueprints[mode == 1 ? 0 : i % 2];
            RwMatrix matrix{}; RwMatrixSetIdentity(&matrix);
            auto* system = new FxSystem_c(); system->m_allocatedParentMat = false;
            system->Init(bp, matrix, nullptr);
            if (ownMatrix) { system->m_ParentMatrix = RwMatrixCreate(); RwMatrixSetIdentity(system->m_ParentMatrix); system->m_allocatedParentMat = true; }
            systems.push_back(system); manager.m_FxSystems.AddItem(system);
            for (auto* prim : bp->GetPrims()) if (std::find(prims.begin(), prims.end(), prim) == prims.end()) prims.push_back(prim);
        }
        foreign = new FxSystem_c(); foreign->m_allocatedParentMat = false;
        RwMatrix matrix{}; RwMatrixSetIdentity(&matrix); foreign->Init(&empty, matrix, nullptr);
        lists.resize(prims.size());
        size_t total = 3 + (density == 2 ? prims.size() : 0);
        if (density) for (auto* s : systems) total += size_t(s->m_SystemBP->m_nNumPrims) * density;
        particles.resize(total);
        size_t index{};
        auto add = [&](TList_c<Particle_c>& list, FxSystem_c* owner) {
            auto& p = particles[index]; p.m_System = owner;
            p.m_fTotalLife = 10.f; p.m_fCurrentLife = float(index); p.m_Pos = CVector{float(index), 2.f, 3.f};
            list.AddItem(&p); ++index;
        };
        for (int i = 0; i < 3; ++i) add(manager.m_FxEmitterParticles, nullptr);
        for (size_t p = 0; p < prims.size(); ++p) {
            if (density == 2) add(lists[p], foreign);
            if (density) for (auto* s : systems) for (auto* prim : s->m_SystemBP->GetPrims()) if (prim == prims[p]) for (int j = 0; j < density; ++j) add(lists[p], s);
        }
        for (auto& p : particles) { auto* bytes = reinterpret_cast<uint8*>(&p); payloads.emplace_back(bytes + 8, bytes + sizeof(p)); }
    }
    // DestroyFxSystem (EXE 0x4A9810 y la DLL) devuelve las particulas a la lista libre del gestor global (0xA9AE9C), no a
    // la de this: durante cada llamada la lista libre del fixture ocupa la de g_fxMan y despues se devuelve intacta.
    void SwapFree() { std::swap(g_fxMan.m_FxEmitterParticles, manager.m_FxEmitterParticles); }
    void SwapLists() { for (size_t i = 0; i < prims.size(); ++i) std::swap(prims[i]->m_Particles, lists[i]); attached = !attached; }
    int SystemId(FxSystem_c* p) const {
        if (!p) return -1; if (p == foreign) return -2;
        const auto it = std::find(systems.begin(), systems.end(), p);
        return it == systems.end() ? -3 : int(it - systems.begin());
    }
    int ParticleId(Particle_c* p) const {
        if (!p) return -1;
        for (size_t i = 0; i < particles.size(); ++i) if (static_cast<const Particle_c*>(&particles[i]) == p) return int(i);
        return -2;
    }
    json List(const TList_c<Particle_c>& list) const {
        json ids = json::array(); bool links = true; Particle_c* previous{};
        auto* p = list.GetHead();
        while (p && ids.size() <= particles.size()) {
            const auto id = ParticleId(p); if (id < 0) { links = false; break; }
            links &= p->m_pPrev == previous; ids.push_back(id); previous = p; p = p->m_pNext;
        }
        links &= !p && previous == list.GetTail() && ids.size() == list.GetNumItems();
        return {{"ids", ids}, {"links", links}, {"count", list.GetNumItems()}};
    }
    json Snapshot() const {
        json active = json::array(), owners = json::array(), remaining = json::array(); bool payload = true;
        for (auto* s = manager.m_FxSystems.GetHead(); s; s = s->m_pNext) remaining.push_back({SystemId(s), int(s->m_nKillStatus)});
        for (size_t i = 0; i < prims.size(); ++i) active.push_back(List(attached ? prims[i]->m_Particles : lists[i]));
        for (size_t i = 0; i < particles.size(); ++i) {
            auto* bytes = reinterpret_cast<const uint8*>(&particles[i]);
            payload &= std::equal(payloads[i].begin(), payloads[i].end(), bytes + 8);
            owners.push_back(SystemId(particles[i].m_System));
        }
        return {{"remaining", remaining}, {"remaining_systems", manager.m_FxSystems.GetNumItems()}, {"head_null", !manager.m_FxSystems.GetHead()}, {"tail_null", !manager.m_FxSystems.GetTail()}, {"free", List(manager.m_FxEmitterParticles)}, {"active", active}, {"owners", owners}, {"payload_unchanged", payload}};
    }
    ~Fixture() {
        // Cleanup is also valid after a failed/incomplete traversal.
        FxManagerProbe::active = false;
        if (!attached) SwapLists();
        SwapFree(); while (auto* system = manager.m_FxSystems.GetHead()) manager.DestroyFxSystem(system); SwapFree();
        SwapLists(); foreign->Exit(); delete foreign;
    }
};
inline json Run(Fixture& fixture, ReversibleHooks::ReversibleHook::Base* hook, bool candidate, bool invalidate) {
    hook->State(candidate);
    if (hook->Hooked() != candidate) { Write({{"event", "error"}, {"reason", "hook selection"}}); return {}; }
    fixture.SwapLists();
    FxManagerProbe::entered.clear(); FxManagerProbe::destroyed.clear();
    FxManagerProbe::invalidateLink = invalidate; FxManagerProbe::active = true;
    const auto before = FxManagerProbe::calls;
    auto entry = reinterpret_cast<void(__thiscall*)(FxManager_c*)>(0x4A98B0);
    fixture.SwapFree(); entry(&fixture.manager); fixture.SwapFree();
    const auto first = fixture.Snapshot(); const auto firstDestroyed = FxManagerProbe::destroyed.size();
    fixture.SwapFree(); entry(&fixture.manager); fixture.SwapFree();
    auto result = fixture.Snapshot();
    result["idempotent"] = first == result && firstDestroyed == FxManagerProbe::destroyed.size();
    result["candidate_calls"] = FxManagerProbe::calls - before;
    result["entered"] = json::array(); result["destroyed"] = json::array();
    for (auto* system : FxManagerProbe::entered) result["entered"].push_back(fixture.SystemId(system));
    for (auto* system : FxManagerProbe::destroyed) result["destroyed"].push_back(fixture.SystemId(system));
    FxManagerProbe::active = false; fixture.SwapLists(); return result;
}
inline json RunUpdate(Fixture& fixture, ReversibleHooks::ReversibleHook::Base* hook, bool candidate, bool invalidate, RwCamera* camera, float step) {
    hook->State(candidate);
    fixture.manager.m_FxSystemBPs.AddItem(&fixture.empty); // Real BP update, with zero primitives.
    std::memset(&fixture.manager.m_Frustum, 0, sizeof(fixture.manager.m_Frustum));
    fixture.SwapLists();
    FxManagerProbe::entered.clear(); FxManagerProbe::destroyed.clear();
    FxManagerProbe::invalidateLink = invalidate; FxManagerProbe::active = true;
    const auto before = FxManagerProbe::updateCalls;
    json stages = json::array();
    auto entry = reinterpret_cast<void(__thiscall*)(FxManager_c*, RwCamera*, float)>(0x4A9A80);
    for (int i = 0; i < 3; ++i) {
        fixture.SwapFree(); entry(&fixture.manager, camera, step); fixture.SwapFree();
        auto stage = fixture.Snapshot();
        stage["destroyed"] = json::array();
        for (auto* s : FxManagerProbe::destroyed) stage["destroyed"].push_back(fixture.SystemId(s));
        const auto* bytes = reinterpret_cast<const uint8*>(&fixture.manager.m_Frustum);
        stage["frustum"] = std::vector<uint8>(bytes, bytes + sizeof(fixture.manager.m_Frustum));
        stages.push_back(stage);
    }
    FxManagerProbe::active = false; fixture.SwapLists(); fixture.manager.m_FxSystemBPs.RemoveAll();
    return {{"candidate_calls", FxManagerProbe::updateCalls - before}, {"stages", stages}, {"hook_selected", hook->Hooked() == candidate}};
}
inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fxmanager-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fxmanager-runtime.jsonl"); _CrtSetReportHook(Report); }
    auto* player = FindPlayerPed(); if (!player || !player->GetRpClump() || CCutsceneMgr::IsRunning()) return;
    done = true;
    ReversibleHooks::ReversibleHook::Base *hook{}, *updateHook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if (c.Name() == "FxManager_c") for (auto& h : c.Items()) { if (h->Name() == "DestroyAllFxSystems") hook = h.get(); if (h->Name() == "Update") updateHook = h.get(); } });
    if (!hook || !updateHook) { Write({{"event", "error"}, {"reason", "missing hook"}}); return; }
    std::vector<FxSystemBP_c*> blueprints;
    for (auto& bp : g_fxMan.m_FxSystemBPs) if (bp.m_nNumPrims > 0 && bp.m_nNumPrims <= 8) { blueprints.push_back(&bp); if (blueprints.size() == 2) break; }
    if (blueprints.size() != 2) { Write({{"event", "error"}, {"reason", "missing loaded blueprints"}}); return; }
    const bool saved = hook->Hooked(), locked = hook->Locked(); hook->LockState(false);
    const bool updateSaved = updateHook->Hooked(), updateLocked = updateHook->Locked(); updateHook->LockState(false);
    std::vector<uint8> managerBefore(sizeof(g_fxMan)); std::memcpy(managerBefore.data(), &g_fxMan, sizeof(g_fxMan));
    std::vector<std::pair<const void*, std::vector<uint8>>> live;
    auto remember = [&](const void* p, size_t size) { const auto* bytes = static_cast<const uint8*>(p); live.emplace_back(p, std::vector<uint8>(bytes, bytes + size)); };
    for (auto* bp : blueprints) for (auto* prim : bp->GetPrims()) {
        remember(prim, sizeof(FxPrimBP_c)); for (auto& particle : prim->m_Particles) remember(&particle, sizeof(Particle_c));
    }
    for (auto& system : g_fxMan.m_FxSystems) remember(&system, sizeof(FxSystem_c));
    Write({{"event", "player_ready"}, {"blueprints", {blueprints[0]->m_nNameKey, blueprints[1]->m_nNameKey}}});
    int pair{}; bool passed = true;
    for (int count : {0, 1, 2, 8, 32}) for (int mode = 0; mode < 3; ++mode) for (int density = 0; density < 3; ++density) for (bool own : {false, true}) for (bool invalidate : {false, true}) {
        if (!passed) break;
        Fixture a(count, mode, density, own, blueprints), b(count, mode, density, own, blueprints);
        bool independent = a.particles.data() != b.particles.data();
        for (size_t i = 0; i < a.systems.size(); ++i) independent &= a.systems[i] != b.systems[i] && a.systems[i]->m_Prims != b.systems[i]->m_Prims;
        const bool sameInput = a.Snapshot() == b.Snapshot();
        const auto original = Run(a, hook, false, invalidate), candidate = Run(b, hook, true, invalidate);
        auto x = original, y = candidate; x.erase("candidate_calls"); y.erase("candidate_calls");
        passed = independent && sameInput && x == y && original["candidate_calls"] == 0 && candidate["candidate_calls"] == 2 && candidate["remaining_systems"] == 0 && candidate["idempotent"] == true && candidate["destroyed"].size() == size_t(count);
        Write({{"event", "comparison"}, {"pair", pair++}, {"count", count}, {"mode", mode}, {"density", density}, {"owned_parent", own}, {"invalidate_detached_link", invalidate}, {"independent_inputs", independent}, {"same_input", sameInput}, {"original", original}, {"candidate", candidate}, {"equal", passed}});
    }
    int updatePair{};
    for (int count : {0, 1, 2, 8, 32}) for (int pattern = 0; pattern < 3; ++pattern) for (int density = 0; density < 3; ++density) for (bool own : {false, true}) for (bool invalidate : {false, true}) for (float step : {0.f, .016f, .2f}) {
        if (!passed) break;
        Fixture a(count, 2, density, own, blueprints), b(count, 2, density, own, blueprints);
        bool independent = a.particles.data() != b.particles.data();
        for (int i = 0; i < count; ++i) {
            const auto state = pattern == 0 || (pattern == 2 && i % 2 == 0) ? FX_3 : FX_KILLED;
            a.systems[i]->m_nKillStatus = state; b.systems[i]->m_nKillStatus = state;
            independent &= a.systems[i] != b.systems[i] && a.systems[i]->m_Prims != b.systems[i]->m_Prims;
        }
        const bool sameInput = a.Snapshot() == b.Snapshot();
        const auto original = RunUpdate(a, updateHook, false, invalidate, Scene.m_pRwCamera, step);
        const auto candidate = RunUpdate(b, updateHook, true, invalidate, Scene.m_pRwCamera, step);
        passed = independent && sameInput && original["stages"] == candidate["stages"] && original["candidate_calls"] == 0 && candidate["candidate_calls"] == 3 && candidate["stages"][2]["remaining_systems"] == 0;
        Write({{"event", "comparison_update"}, {"pair", updatePair++}, {"count", count}, {"pattern", pattern}, {"density", density}, {"owned_parent", own}, {"invalidate_detached_link", invalidate}, {"step", step}, {"independent_inputs", independent}, {"same_input", sameInput}, {"original", original}, {"candidate", candidate}, {"equal", passed}});
    }
    hook->State(saved); hook->LockState(locked);
    updateHook->State(updateSaved); updateHook->LockState(updateLocked);
    bool restored = std::memcmp(managerBefore.data(), &g_fxMan, sizeof(g_fxMan)) == 0 && hook->Hooked() == saved && hook->Locked() == locked;
    for (const auto& [p, bytes] : live) restored &= std::memcmp(p, bytes.data(), bytes.size()) == 0;
    restored &= updateHook->Hooked() == updateSaved && updateHook->Locked() == updateLocked;
    Write({{"event", passed && restored ? "complete" : "error"}, {"pairs", pair}, {"update_pairs", updatePair}, {"state_restored", restored}});
}
}
