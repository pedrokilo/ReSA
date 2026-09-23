#pragma once
#include <fstream>
#include <stacktrace>
#include <crtdbg.h>
#include "CarGenerator.h"
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "RuntimeProbe.hpp"

// Opt-in integration test running on the game's thread. No synthetic engine doubles.
// Test vehicles are never added to the world or saved. UI/visual review is separate.
namespace CVehicleRuntime {
inline std::ofstream output;
inline bool done{};
inline int step{};
inline std::filesystem::path directory;
inline void Write(const json& row) { output << row.dump() << '\n'; output.flush(); }
inline int ReportHook(int type, char* message, int*) {
    if (type != _CRT_ASSERT) return FALSE;
    Write({{"event", "assertion"}, {"message", message ? message : ""},
           {"stack", std::to_string(std::stacktrace::current())}});
    ExitProcess(91); // Stop this diagnostic process; never ignore an assertion.
}
inline bool SetOriginal(bool original) {
    bool foundObject{}, foundCaller{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category) {
        if (category.Name() != "CVehicle") return;
        for (auto& hook : category.Items()) {
            if (hook->Name() == "RemoveObjectsCB-Object" || hook->Name() == "RemoveReplacementUpgrade") {
                category.SetItemEnabled(hook, !original);
                if (hook->Hooked() == original) return;
                if (hook->Name() == "RemoveObjectsCB-Object") foundObject = true;
                else foundCaller = true;
            }
        }
    });
    return foundObject && foundCaller;
}
inline json Snapshot(CVehicle* vehicle) {
    std::vector<std::string> atoms;
    RpClumpForAllAtomics(vehicle->GetRpClump(), [](RpAtomic* a, void* data) -> RpAtomic* {
        const auto frame = RpAtomicGetFrame(a);
        const auto geo = RpAtomicGetGeometry(a);
        json atom = {
            {"model", CVisibilityPlugins::GetModelInfoIndex(a)},
            {"flags", CVisibilityPlugins::GetAtomicId(a)},
            {"hierarchy", CVisibilityPlugins::GetFrameHierarchyId(frame)},
            {"vertices", geo ? geo->numVertices : 0},
            {"triangles", geo ? geo->numTriangles : 0}
        };
        static_cast<std::vector<std::string>*>(data)->push_back(atom.dump());
        return a;
    }, &atoms);
    std::sort(atoms.begin(), atoms.end());
    return json{{"atomics", atoms}, {"upgrades", vehicle->m_anUpgrades}};
}
inline json Refs(int model) {
    json refs = json::array();
    refs.push_back(CModelInfo::GetModelInfo(model)->m_nRefCount);
    for (int id = 1000; id <= 1193; ++id) refs.push_back(CModelInfo::GetModelInfo(id)->m_nRefCount);
    return refs;
}
inline json Run(int model, int upgrade) {
    const auto beforeRefs = Refs(model);
    CVehicle* vehicle = CCarGenerator::CreateVehicle(static_cast<eModelID>(model), MISSION_VEHICLE);
    if (!vehicle) return {{"error", "vehicle creation failed"}};
    json result;
    result["initial"] = Snapshot(vehicle);
    for (int i = 0; i < 3; ++i) {
        vehicle->AddVehicleUpgrade(upgrade);
        result["cycles"][i]["added"] = Snapshot(vehicle);
        vehicle->RemoveVehicleUpgrade(upgrade);
        result["cycles"][i]["removed"] = Snapshot(vehicle);
    }
    delete vehicle;
    const auto afterRefs = Refs(model);
    auto delta = json::array();
    for (size_t i = 0; i < beforeRefs.size(); ++i)
        delta.push_back(afterRefs[i].get<int>() - beforeRefs[i].get<int>());
    result["reference_delta"] = delta;
    return result;
}
inline json EdgeCase(int flags) {
    int32 data = 0x12345678;
    if (flags < 0) {
        auto frame = RwFrameCreate();
        const auto object = reinterpret_cast<RwObject*>(frame);
        const auto ret = RemoveObjectsCB(object, &data);
        json result = {{"same_return", ret == object}, {"data", data}};
        RwFrameDestroy(frame);
        return result;
    }
    auto atomic = RpAtomicCreate();
    auto frame = RwFrameCreate();
    RpAtomicSetFrame(atomic, frame);
    CVisibilityPlugins::SetModelInfoIndex(atomic, -1);
    CVisibilityPlugins::SetAtomicId(atomic, flags);
    const auto object = reinterpret_cast<RwObject*>(atomic);
    const auto ret = RemoveObjectsCB(object, &data);
    json result = {{"same_return", ret == object}, {"data", data},
                   {"frame_preserved", RpAtomicGetFrame(atomic) == frame}};
    RpAtomicDestroy(atomic);
    RwFrameDestroy(frame);
    return result;
}
inline bool CheckEdges() {
    for (int flags : {-1, 0x800, 0xffff}) {
        if (!SetOriginal(true)) return false;
        CVehicleRuntimeProbe::armed = false;
        const auto original = EdgeCase(flags);
        if (!SetOriginal(false)) return false;
        CVehicleRuntimeProbe::counts = {};
        CVehicleRuntimeProbe::armed = true;
        const auto candidate = EdgeCase(flags);
        CVehicleRuntimeProbe::armed = false;
        const auto n = CVehicleRuntimeProbe::counts;
        Write({{"event", "edge_comparison"}, {"flags", flags}, {"equal", original == candidate},
               {"original", original}, {"candidate", candidate},
               {"coverage", {{"calls", n.calls}, {"non_atomic", n.nonAtomic}, {"kept_upgrade", n.keptUpgrade}}}});
        if (original != candidate) return false;
    }
    return true;
}


inline void Update() {
    if (done) return;
    if (directory.empty()) {
        char path[MAX_PATH]{};
        GetModuleFileNameA(nullptr, path, MAX_PATH);
        directory = std::filesystem::path(path).parent_path();
        if (!std::filesystem::exists(directory / "cvehicle-runtime.start")) { done = true; return; }
        output.open(directory / "cvehicle-runtime.jsonl", std::ios::trunc);
        Write({{"event", "waiting_for_player"}});
        _CrtSetReportHook(ReportHook);
    }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    struct Case { int model, upgrade; };
    constexpr Case cases[] = {
        {560, 1073}, {560, 1028}, {560, 1169}, {560, 1140},
        {562, 1074}, {562, 1034}, {562, 1171}, {562, 1148},
        {565, 1075}, {565, 1046}, {565, 1153}, {565, 1150}
    };
    constexpr int rounds = 5;
    if (step >= int(std::size(cases)) * rounds) {
        SetOriginal(false);
        CVehicleRuntimeProbe::armed = false;
        Write({{"event", "complete"}, {"pairs", step}});
        done = true;
        return;
    }
    if (step == 0 && !CheckEdges()) { Write({{"event", "error"}, {"reason", "edge comparison failed"}}); done = true; return; }
    const auto c = cases[step % std::size(cases)];
    Write({{"event", "begin"}, {"pair", step}, {"model", c.model}, {"upgrade", c.upgrade}});
    CStreaming::RequestModel(c.model, STREAMING_GAME_REQUIRED);
    CStreaming::RequestModel(c.upgrade, STREAMING_GAME_REQUIRED);
    CStreaming::LoadAllRequestedModels(false);
    if (!CStreaming::IsModelLoaded(c.model) || !CStreaming::IsModelLoaded(c.upgrade)) {
        Write({{"event", "error"}, {"reason", "model not loaded"}});
        done = true;
        return;
    }
    if (!SetOriginal(true)) {
        Write({{"event", "error"}, {"reason", "original hook selection failed"}});
        done = true; return;
    }
    CVehicleRuntimeProbe::armed = false;
    const auto original = Run(c.model, c.upgrade);
    if (!SetOriginal(false)) {
        Write({{"event", "error"}, {"reason", "candidate hook selection failed"}});
        done = true; return;
    }
    CVehicleRuntimeProbe::counts = {};
    CVehicleRuntimeProbe::armed = true;
    const auto candidate = Run(c.model, c.upgrade);
    CVehicleRuntimeProbe::armed = false;
    const auto n = CVehicleRuntimeProbe::counts;
    Write({{"event", "comparison"}, {"pair", step}, {"model", c.model}, {"upgrade", c.upgrade},
        {"equal", original == candidate}, {"original", original}, {"candidate", candidate},
        {"coverage", {{"calls", n.calls}, {"non_atomic", n.nonAtomic}, {"kept_upgrade", n.keptUpgrade},
                      {"removed", n.removed}, {"destroyed_frame", n.destroyedFrame}, {"model_ref", n.modelRef}}}});
    if (original != candidate || original.contains("error")) {
        Write({{"event", "stopped"}, {"reason", "comparison differs; inspect before proceeding"}});
        done = true; return;
    }
    ++step;
}
}

