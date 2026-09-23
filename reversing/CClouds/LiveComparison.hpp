#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Clouds.h"
#include "Probe.hpp"
namespace CloudsLive {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& value) { log << value.dump() << '\n'; log.flush(); }
inline int Report(int type, char* message, int*) {
    if (type != _CRT_ASSERT) return FALSE;
    Write({{"event", "assertion"}, {"message", message ? message : ""}}); ExitProcess(91);
}
// Instrumentation only: temporarily restore the entry while forwarding to RenderWare.
struct EntryPatch {
    uintptr_t address; std::array<uint8, 5> saved, jump;
    EntryPatch(uintptr_t at, void* target) : address(at) {
        std::memcpy(saved.data(), reinterpret_cast<void*>(at), 5);
        jump[0] = 0xE9; const auto offset = uint32(reinterpret_cast<uintptr_t>(target) - at - 5);
        std::memcpy(jump.data() + 1, &offset, 4); Set(true);
    }
    void Set(bool on) {
        DWORD old; VirtualProtect(reinterpret_cast<void*>(address), 5, PAGE_EXECUTE_READWRITE, &old);
        std::memcpy(reinterpret_cast<void*>(address), (on ? jump : saved).data(), 5);
        DWORD ignored; VirtualProtect(reinterpret_cast<void*>(address), 5, old, &ignored);
        FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(address), 5);
    }
    ~EntryPatch() { Set(false); }
};
inline EntryPatch *transformPatch{}, *indicesPatch{}, *endPatch{};
inline RwRenderStateSetFunction originalSet{};
inline RwIm2DRenderIndexedPrimitiveFunction original2D{};
inline RwBool Set(RwRenderState state, void* value) {
    CloudsProbe::Record("state", {}, {state, int64_t(reinterpret_cast<uintptr_t>(value))}); return originalSet(state, value);
}
inline void* Transform(RwIm3DVertex* vertices, RwUInt32 count, RwMatrix* matrix, RwUInt32 flags) {
    if (CloudsProbe::recording) {
        json values = json::array(), exact = {count, flags, matrix != nullptr};
        for (unsigned i = 0; i < count; ++i) {
            const auto& v = vertices[i];
            for (float f : {v.objVertex.x, v.objVertex.y, v.objVertex.z, v.u, v.v}) values.push_back(f);
            exact.push_back(v.color);
        }
        CloudsProbe::commands.push_back({{"kind", "geometry"}, {"numbers", values}, {"exact", exact}});
    }
    transformPatch->Set(false); auto* result = RwIm3DTransform(vertices, count, matrix, flags); transformPatch->Set(true);
    CloudsProbe::Record("transform_result", {}, {result != nullptr}); return result;
}
inline RwBool Indices(RwPrimitiveType type, RwImVertexIndex* indices, RwInt32 count) {
    if (CloudsProbe::recording) CloudsProbe::commands.push_back({{"kind", "indices"}, {"exact", {int(type), std::vector<uint16>(indices, indices + count)}}});
    indicesPatch->Set(false); const auto result = RwIm3DRenderIndexedPrimitive(type, indices, count); indicesPatch->Set(true); return result;
}
inline RwBool End() {
    CloudsProbe::Record("end3d", {}, {}); endPatch->Set(false); const auto result = RwIm3DEnd(); endPatch->Set(true); return result;
}
inline RwBool Draw2D(RwPrimitiveType type, RwIm2DVertex* vertices, RwInt32 count, RwImVertexIndex* indices, RwInt32 indexCount) {
    if (CloudsProbe::recording) {
        json values = json::array(), colors = json::array();
        for (int i = 0; i < count; ++i) {
            const auto& v = vertices[i]; for (float f : {v.x, v.y, v.z, v.rhw, v.u, v.v}) values.push_back(f); colors.push_back(v.emissiveColor);
        }
        CloudsProbe::commands.push_back({{"kind", "draw2d"}, {"numbers", values}, {"exact", {int(type), colors, std::vector<uint16>(indices, indices + indexCount)}}});
    }
    return original2D(type, vertices, count, indices, indexCount);
}
struct State {
    CVector camera = TheCamera.GetPosition();
    CColourSet colours = CTimeCycle::m_CurrentColours;
    float wind = CWeather::Wind, step = CTimer::GetTimeStep(), shift = StaticRef<float>(0xC6E954);
    std::vector<uint8> buffer;
    std::array<uint8, 8> counts = StaticRef<std::array<uint8, 8>>(0xC4B950);
    std::array<uint8, 8> sprite = StaticRef<std::array<uint8, 8>>(0xC6A158);
    std::array<uint8, 12> screen = StaticRef<std::array<uint8, 12>>(0xC4B8D0);
    std::vector<uint16> indices;
    std::array<void*, 32> states{}; std::array<bool, 32> valid{};
    State() {
        const auto* ptr = reinterpret_cast<const uint8*>(&TempBufferVertices); buffer.assign(ptr, ptr + sizeof(TempBufferVertices));
        indices.assign(std::begin(aTempBufferIndices), std::end(aTempBufferIndices));
        for (int i : {1, 6, 8, 10, 11, 12, 14}) valid[i] = RwRenderStateGet(RwRenderState(i), &states[i]) != FALSE;
    }
    void Restore() const {
        TheCamera.SetPosn(camera); CTimeCycle::m_CurrentColours = colours; CWeather::Wind = wind; CTimer::SetTimeStep(step); StaticRef<float>(0xC6E954) = shift;
        std::memcpy(&TempBufferVertices, buffer.data(), buffer.size()); std::copy(indices.begin(), indices.end(), std::begin(aTempBufferIndices));
        StaticRef<std::array<uint8, 8>>(0xC4B950) = counts; StaticRef<std::array<uint8, 8>>(0xC6A158) = sprite; StaticRef<std::array<uint8, 12>>(0xC4B8D0) = screen;
        for (int i = 1; i < 32; ++i) if (valid[i]) RwRenderStateSet(RwRenderState(i), states[i]);
    }
    bool Matches() const {
        for (int i = 1; i < 32; ++i) {
            if (!valid[i]) continue;
            void* value{};
            if (!RwRenderStateGet(RwRenderState(i), &value) || value != states[i]) return false;
        }
        return TheCamera.GetPosition() == camera && std::memcmp(&colours, &CTimeCycle::m_CurrentColours, sizeof(colours)) == 0 && CWeather::Wind == wind && CTimer::GetTimeStep() == step && StaticRef<float>(0xC6E954) == shift && std::memcmp(&TempBufferVertices, buffer.data(), buffer.size()) == 0 && std::equal(indices.begin(), indices.end(), std::begin(aTempBufferIndices)) && StaticRef<std::array<uint8, 8>>(0xC4B950) == counts && StaticRef<std::array<uint8, 8>>(0xC6A158) == sprite && StaticRef<std::array<uint8, 12>>(0xC4B8D0) == screen;
    }
};
inline json Snapshot() {
    json states = json::array();
    for (int i : {1, 6, 8, 10, 11, 12, 14}) { void* value{}; const bool valid = RwRenderStateGet(RwRenderState(i), &value) != FALSE; states.push_back({i, valid, reinterpret_cast<uintptr_t>(value)}); }
    return {{"numbers", {StaticRef<float>(0xC6E954), CSprite::m_f2DNearScreenZ, CSprite::m_f2DFarScreenZ}}, {"exact", {StaticRef<uint32>(0xC4B950), StaticRef<uint32>(0xC4B954), StaticRef<int32>(0xC6A158), CSprite::m_bFlushSpriteBufferSwitchZTest, states}}};
}
inline bool Equal(const json& a, const json& b, double& error) {
    if (a.is_object()) { if (!b.is_object() || a.size() != b.size()) return false; for (auto it = a.begin(); it != a.end(); ++it) { if (!b.contains(it.key())) return false; if (it.key() == "exact") { if (it.value() != b[it.key()]) return false; } else if (!Equal(it.value(), b[it.key()], error)) return false; } return true; }
    if (a.is_array()) { if (!b.is_array() || a.size() != b.size()) return false; for (size_t i = 0; i < a.size(); ++i) if (!Equal(a[i], b[i], error)) return false; return true; }
    if (a.is_number_float() || b.is_number_float()) { if (!a.is_number() || !b.is_number()) return false; const double x = a, y = b; if (!std::isfinite(x) || !std::isfinite(y)) return false; error = std::max(error, std::abs(x-y)); return x == y; }
    return a == b;
}
inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "clouds-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "clouds-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning() || !gpCloudMaskTex) return;
    done = true; auto previousReport = _CrtSetReportHook(Report); const State saved;
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if (c.Name() == "CClouds") for (auto& h : c.Items()) if (h->Name() == "RenderBottomFromHeight") hook = h.get(); });
    if (!hook) { Write({{"event", "error"}, {"reason", "missing hook"}}); _CrtSetReportHook(previousReport); return; }
    const bool enabled = hook->Hooked(), locked = hook->Locked(); hook->LockState(false);
    EntryPatch transform(0x7EF450, reinterpret_cast<void*>(Transform)), render(0x7EF550, reinterpret_cast<void*>(Indices)), end(0x7EF520, reinterpret_cast<void*>(End));
    transformPatch = &transform; indicesPatch = &render; endPatch = &end;
    originalSet = RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; RWSRCGLOBAL(dOpenDevice).fpRenderStateSet = Set;
    original2D = RWSRCGLOBAL(dOpenDevice).fpIm2DRenderIndexedPrimitive; RWSRCGLOBAL(dOpenDevice).fpIm2DRenderIndexedPrimitive = Draw2D;
    Write({{"event", "player_ready"}, {"fp_control", _controlfp(0, 0)}, {"camera", {saved.camera.x, saved.camera.y, saved.camera.z}}}); int pair{}; bool passed = true;
    for (float z : {-100.f, -90.f, -89.99f, 0.f, 120.f, 160.f, 190.f, 199.99f, 200.f, 200.01f, 250.f, 400.f, 800.f}) for (int xy = 0; xy < 2; ++xy) for (float alpha : {0.f, .9f, 1.f, 127.75f, 255.f}) for (int weather = 0; weather < 3; ++weather) for (int color = 0; color < 3; ++color) {
        if (!passed) break;
        saved.Restore(); auto position = saved.camera; position.z = z; if (xy) { position.x = -256.125f; position.y = 128.875f; } TheCamera.SetPosn(position);
        CWeather::Wind = std::array{0.f, .6f, 1.f}[weather]; CTimer::SetTimeStep(std::array{0.f, 1.f, 2.f}[weather]); StaticRef<float>(0xC6E954) = std::array{0.f, 123.75f, -511.9f}[weather];
        auto& cc = CTimeCycle::m_CurrentColours; cc.m_fCloudAlpha = alpha;
        cc.m_nFluffyCloudsBottomRed = std::array{0, 100, 255}[color]; cc.m_nFluffyCloudsBottomGreen = std::array{0, 120, 255}[color]; cc.m_nFluffyCloudsBottomBlue = std::array{0, 160, 255}[color];
        StaticRef<int32>(0xC6A158) = 0; StaticRef<uint32>(0xC4B950) = 4; StaticRef<uint32>(0xC4B954) = 6;
        const State input; const auto before = CloudsProbe::calls;
        auto entry = reinterpret_cast<void(__cdecl*)()>(0x7154B0);
        hook->State(false); CloudsProbe::commands = json::array(); CloudsProbe::recording = true; entry(); CloudsProbe::recording = false;
        const auto a = Snapshot(), da = CloudsProbe::commands; const auto middle = CloudsProbe::calls;
        input.Restore(); const bool restored = input.Matches();
        hook->State(true); CloudsProbe::commands = json::array(); CloudsProbe::recording = true; entry(); CloudsProbe::recording = false;
        const auto b = Snapshot(), db = CloudsProbe::commands; const auto after = CloudsProbe::calls;
        double error{}; passed = restored && Equal(a,b,error) && Equal(da,db,error) && middle == before && after == middle + 1;
        Write({{"event", "comparison"}, {"pair", pair++}, {"z", z}, {"xy", xy}, {"alpha", alpha}, {"weather", weather}, {"color", color}, {"equal", passed}, {"max_error", error}, {"input_restored", restored}, {"original_candidate_calls", middle-before}, {"candidate_calls", after-middle}, {"original", a}, {"candidate", b}, {"draw_original", da}, {"draw_candidate", db}});
    }
    CloudsProbe::recording = false; saved.Restore(); hook->State(enabled); hook->LockState(locked);
    RWSRCGLOBAL(dOpenDevice).fpRenderStateSet = originalSet; RWSRCGLOBAL(dOpenDevice).fpIm2DRenderIndexedPrimitive = original2D;
    transform.Set(false); render.Set(false); end.Set(false);
    const bool restored = saved.Matches() && hook->Hooked() == enabled && hook->Locked() == locked;
    Write({{"event", passed && restored ? "complete" : "error"}, {"pairs", pair}, {"state_restored", restored}}); _CrtSetReportHook(previousReport);
}
}
