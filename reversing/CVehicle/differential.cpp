// Executes only the extracted callbacks/caller. All engine callees are controlled doubles.
// This is a differential unit test, not a substitute for a full build or an in-game test.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <tuple>
using int32 = int32_t;
struct RwFrame;
struct RpClump { int tag; };
struct RwObject { uint8_t type, subType, flags, privateFlags; RwFrame* parent; };
struct RpAtomic { RwObject object; char pad[0x3c - sizeof(RwObject)]; RpClump* clump; };
struct RwFrame { int32 hierarchy; RpAtomic* atomic; RwFrame* child; };
static_assert(sizeof(void*) == 4);
static_assert(offsetof(RpAtomic, clump) == 0x3c);
enum eAtomicComponentFlag { ATOMIC_UPGRADE = 0x800 };
constexpr int rpATOMIC = 1;
using Event = std::tuple<int, uintptr_t, uintptr_t>;
std::vector<Event> events;
void log(int kind, const void* a = nullptr, const void* b = nullptr) {
    events.emplace_back(kind, reinterpret_cast<uintptr_t>(a), reinterpret_cast<uintptr_t>(b));
}
struct CBaseModelInfo {
    char pad[0x1c]{};
    RpClump* clump{};
    int refs = 10;
    void RemoveRef() { log(7, this); --refs; }
    RpClump* GetRpClump() { return clump; }
};
static_assert(offsetof(CBaseModelInfo, clump) == 0x1c);
int32 atomicId;
CBaseModelInfo* info;
struct CVisibilityPlugins {
    static int32 GetAtomicId(RpAtomic* a) { log(1, a); return atomicId; }
    static CBaseModelInfo* GetModelInfo(RpAtomic* a) { log(2, a); return info; }
    static int32 GetFrameHierarchyId(RwFrame* f) { log(5, f); return f->hierarchy; }
};
int RwObjectGetType(RwObject* o) { return o->type; }
RwFrame* RpAtomicGetFrame(RpAtomic* a) { return a->object.parent; }
RpClump* RpAtomicGetClump(RpAtomic* a) { return a->clump; }
RpClump* RpClumpRemoveAtomic(RpClump* c, RpAtomic* a) { log(3, c, a); a->clump = nullptr; return c; }
int RpAtomicDestroy(RpAtomic* a) { log(4, a); a->object.parent = nullptr; return 1; }
int RwFrameDestroy(RwFrame* f) { log(6, f); return 1; }
using ObjectCB = RwObject*(__cdecl*)(RwObject*, void*);
using FrameCB = RwFrame*(__cdecl*)(RwFrame*, void*);
RwFrame* __cdecl RwFrameForAllObjects(RwFrame* frame, ObjectCB cb, void* data) {
    log(8, frame);
    if (frame->atomic) cb(&frame->atomic->object, data);
    return frame;
}
RwFrame* __cdecl RwFrameForAllChildren(RwFrame* frame, FrameCB cb, void* data) {
    log(9, frame);
    if (frame->child) cb(frame->child, data);
    return frame;
}
RpClump vehicleClump{1}, templateClump{2};
RpAtomic gAtomic{};
RwFrame gFrame{}, child{}, templateFrame{};
CBaseModelInfo model{};
struct CClumpModelInfo {
    static RwFrame* GetFrameFromId(RpClump* clump, int32 id) {
        log(10, clump, reinterpret_cast<void*>(id));
        return clump == &vehicleClump ? &gFrame : &templateFrame;
    }
};
uintptr_t testBase;
uintptr_t address(uintptr_t original) { return testBase + original - 0x400000; }
#define CopyObjectsCB_TargetClump (*reinterpret_cast<RpClump**>(address(0xC1CB58)))
RwObject* __cdecl CopyObjectsCB(RwObject* object, void* data) {
    log(11, data, CopyObjectsCB_TargetClump);
    return object;
}
struct CVehicle {
    char pad[0x18]{};
    RpClump* clump{};
    char pad2[6]{};
    int16_t modelIndex{};
    RpClump* GetRpClump() { return clump; }
    CBaseModelInfo* GetModelInfo() { return &model; }
    void RemoveReplacementUpgrade(int32 frameId);
};
static_assert(offsetof(CVehicle, modelIndex) == 0x22);
#include "candidate.inc"
#include "original_bytes.inc"
void __fastcall RemoveRefShim(CBaseModelInfo* self, void*) { self->RemoveRef(); }
template<class Fn> void jump(uintptr_t at, Fn fn) {
    at = address(at);
    auto p = reinterpret_cast<uint8_t*>(at);
    p[0] = 0xE9;
    const uint32_t rel = reinterpret_cast<uintptr_t>(fn) - at - 5;
    std::memcpy(p + 1, &rel, 4);
}
void setup() {
    testBase = reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr, 0x820000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE));
    if (!testBase) { std::printf("Allocation failed: %lu\n", GetLastError()); std::exit(2); }
    std::memcpy(reinterpret_cast<void*>(address(0x6D33B0)), originalObject, sizeof originalObject);
    std::memcpy(reinterpret_cast<void*>(address(0x6D3420)), originalFrame, sizeof originalFrame);
    std::memcpy(reinterpret_cast<void*>(address(0x6D39E0)), originalCaller, sizeof originalCaller);
    // Relocate only the seven absolute image-address operands; all relative branches
    // retain their original displacement because the entire address window moves together.
    for (auto [operand, expected] : {
        std::pair{0x6D342Cu, 0x6D33B0u}, std::pair{0x6D3438u, 0x6D3420u},
        std::pair{0x6D39FBu, 0x6D33B0u}, std::pair{0x6D3A13u, 0x6D3420u},
        std::pair{0x6D3A24u, 0xA9B0C8u}, std::pair{0x6D3A37u, 0x6D3450u},
        std::pair{0x6D3A3Eu, 0xC1CB58u}
    }) {
        uint32_t value;
        auto p = reinterpret_cast<void*>(address(operand));
        std::memcpy(&value, p, 4);
        if (value != expected) { std::puts("Unexpected relocation operand"); std::exit(2); }
        value = uint32_t(address(value));
        std::memcpy(p, &value, 4);
    }
    jump(0x732370, CVisibilityPlugins::GetAtomicId);
    jump(0x732260, CVisibilityPlugins::GetModelInfo);
    jump(0x74A4C0, RpClumpRemoveAtomic);
    jump(0x749DC0, RpAtomicDestroy);
    jump(0x732A20, CVisibilityPlugins::GetFrameHierarchyId);
    jump(0x7F05A0, RwFrameDestroy);
    jump(0x4C4BB0, RemoveRefShim);
    jump(0x4C53C0, CClumpModelInfo::GetFrameFromId);
    jump(0x7F1200, RwFrameForAllObjects);
    jump(0x7F0DC0, RwFrameForAllChildren);
    jump(0x6D3450, CopyObjectsCB);
    *reinterpret_cast<CBaseModelInfo**>(address(0xA9B0C8)) = &model;
    FlushInstructionCache(GetCurrentProcess(), nullptr, 0);
}
void reset(uint8_t type, int32 flags, int hierarchy, bool hasModel) {
    events.clear();
    gAtomic = {};
    gAtomic.object.type = type;
    gAtomic.object.parent = &gFrame;
    gAtomic.clump = &vehicleClump;
    gFrame = {hierarchy, &gAtomic, nullptr};
    child = {1, nullptr, nullptr};
    templateFrame = {1, &gAtomic, nullptr};
    model.refs = 10;
    model.clump = &templateClump;
    atomicId = flags;
    info = hasModel ? &model : nullptr;
    CopyObjectsCB_TargetClump = nullptr;
}
struct Result {
    std::vector<Event> calls;
    int32 output;
    int refs;
    bool sameReturn;
    RwFrame* parent;
    RpClump* clump;
    bool operator==(const Result&) const = default;
};
Result run(ObjectCB fn, uint8_t type, int32 flags, int hierarchy, bool hasModel, bool nullData = false) {
    reset(type, flags, hierarchy, hasModel);
    int32 output = 0x12345678;
    auto ret = fn(&gAtomic.object, nullData ? nullptr : &output);
    return {events, output, model.refs, ret == &gAtomic.object, gAtomic.object.parent, gAtomic.clump};
}
void fail(const char* test, int32 value) {
    std::printf("FAIL %s value=%08X\n", test, unsigned(value));
    std::exit(1);
}
int main() {
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    setup();
    auto original = reinterpret_cast<ObjectCB>(address(0x6D33B0));
    auto candidate = static_cast<ObjectCB>(&RemoveObjectsCB);
    size_t cases = 0;
    for (int32 flags = 0; flags <= 0xffff; ++flags) {
        for (int hierarchy : {0, 7, -1}) for (bool hasModel : {false, true}) {
            auto a = run(original, 1, flags, hierarchy, hasModel);
            auto b = run(candidate, 1, flags, hierarchy, hasModel);
            if (!(a == b)) fail("gAtomic", flags);
            if (a.output != flags || !a.sameReturn) fail("output", flags);
            ++cases;
        }
    }
    for (int type = 0; type <= 255; ++type) {
        if (type == 1) continue;
        if (!(run(original, uint8_t(type), 0, 0, true, true) == run(candidate, uint8_t(type), 0, 0, true, true)))
            fail("non-gAtomic/null data", type);
        ++cases;
    }
    // Check 32-bit storage/sign bits, independently of the game's usual 16-bit flag domain.
    for (int32 flags : {-1, INT32_MIN, INT32_MAX, 0x12345678}) {
        if (!(run(original, 1, flags, 0, true) == run(candidate, 1, flags, 0, true)))
            fail("wide flags", flags);
        ++cases;
    }
    // Exercise original caller vs actual edited source. A nonzero gAtomic ID must not replace the gFrame pointer.
    using Caller = void(__thiscall*)(CVehicle*, int32);
    auto originalCallerFn = reinterpret_cast<Caller>(address(0x6D39E0));
    CVehicle vehicle{};
    vehicle.clump = &vehicleClump;
    size_t callerCases = 0;
    for (int32 flags : {0, 1, 0x40, 0x800, 0x840, 0xffff}) {
        for (bool withChild : {false, true}) {
            reset(1, flags, 7, true);
            gFrame.child = withChild ? &child : nullptr;
            originalCallerFn(&vehicle, 3);
            auto expected = events;
            const auto refs = model.refs;
            reset(1, flags, 7, true);
            gFrame.child = withChild ? &child : nullptr;
            vehicle.RemoveReplacementUpgrade(3);
            if (events != expected || model.refs != refs || CopyObjectsCB_TargetClump != &vehicleClump)
                fail("RemoveReplacementUpgrade", flags);
            ++callerCases;
        }
    }
    std::printf("PASS: %zu callback cases; %zu caller cases. Original x86 bytes vs compiled source; engine calls mocked.\n", cases, callerCases);
}

