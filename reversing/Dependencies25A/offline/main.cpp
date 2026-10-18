// Arnes sin juego del banco Dependencies25A: los bytes REALES del EXE (proceso hijo con la imagen mapeada en 0x400000) frente a las reconstrucciones C++ del arbol de fuentes
// (los textos se extraen sin cambios con extract.py -> inc/*.inc, salvo operator delete -> ExeOperatorDelete) y los mismos casos, colaboradores falsos y compuertas que el banco real
// (Core.hpp). Escribe un comparisons.jsonl con el formato del banco real, que `run_local.py --analyze` valida contra el oraculo (fixture_oracle.py).
// No usa el sistema de hooks RH: el "hook" es un EntryPatch al cuerpo de la funcion (equivalente al salto que instala RH).
#define NOMINMAX
#include <windows.h>
#include <intrin.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <bit>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
#include "../../ProbeSupport.hpp"

// ---------------------------------------------------------------- ruta RH sustituta
namespace OriginalDependencies499 {
class HookRoute {
    uintptr_t m_entry{}, m_body{};
public:
    HookRoute(const char*, const char*, uintptr_t entry, uintptr_t body) : m_entry(entry), m_body(body) {}
    uintptr_t Body() const { return m_body; }
    bool Registered() const { return true; }
    template<class F> auto Invoke(bool candidate, F call) {
        if (candidate) { ReversingProbe::EntryPatch patch(m_entry, reinterpret_cast<void*>(m_body)); return call(); }
        return call();
    }
    bool Restore() { return true; }
};
}

// ---------------------------------------------------------------- entorno minimo del juego para los textos extraidos
#define VALIDATE_SIZE(a, b) static_assert(sizeof(a) == (b))
template<class T> T& StaticRef(uintptr_t address) { return *reinterpret_cast<T*>(address); }
static void ExeOperatorDelete(void* p) { reinterpret_cast<void(__cdecl*)(void*)>(0x8214BD)(p); }   // plugin::Call<0x8214BD, void*>

// 0 CTask
struct CTask { void Destructor(); };
#include "inc/task.inc"
// 1 CPool / CVehiclePool
struct CVehicle { uint8 bytes[0x5A0]; };
struct CHeli { uint8 bytes[0xA18]; };
template<class T, class S = T>
class CPool {
    using StorageType = uint8[sizeof(S)];
    struct SlotState { uint8 Ref : 7 { 0 }; bool IsEmpty : 1 { true }; };
public:
#include "inc/pool_members.inc"
    StorageType* m_Storage{};
    SlotState*   m_SlotState{};
    size_t       m_Capacity{};
    int32        m_LastFreeSlot{ -1 };
    bool         m_OwnsAllocations{};
    bool         m_DealWithNoMemory{};
};
class CVehiclePool : public CPool<CVehicle, CHeli> {};
// 2 CAutoPilot
struct CAutoPilot {
    uint8 pad0[0x68];
    std::array<uint32, 8> m_aPathFindNodesInfo;
    uint16 m_nPathFindNodesCount;
    uint8 pad1[0x98 - 0x8A];
    void RemoveOnePathNode();
};
static_assert(offsetof(CAutoPilot, m_nPathFindNodesCount) == 0x88);
#include "inc/autopilot.inc"
// 3 CPedGroups
struct CPedGroup { uint8 pad[0x2D4]; __declspec(noinline) void RemoveAllFollowers() { volatile int sink = 1; (void)sink; } };
static_assert(sizeof(CPedGroup) == 0x2D4);
struct CPedGroups {
    static inline auto& ms_activeGroups = StaticRef<std::array<char, 8>>(0xC098E0);
    static inline auto& ms_groups = StaticRef<std::array<CPedGroup, 8>>(0xC09920);
    static void RemoveAllFollowersFromGroup(int32 groupId);
};
#include "inc/pedgroups.inc"
// 4 CDecisionMakerTypes
namespace DecisionStub {   // las constantes del espacio anonimo del archivo de la DLL chocan con las de Fixtures.hpp
struct CDecision { static constexpr auto MAX_NUM_DECISIONS = 0x29; uint8 pad[0x3C]; __declspec(noinline) void SetDefault() { volatile int sink = 1; (void)sink; } };
enum eEventType : int32 {};
struct CDecisionMakerTypes { void FlushDecisionMakerEventResponse(int32 decisionMakerIndex, eEventType eventId); };
#include "inc/decision.inc"
}
// 5 CGarages
enum eGarageType : uint8 { GARAGE_INVALID = 0 };
struct CStoredCar { uint8 pad0[0x12]; uint16 m_wModelIndex; uint8 pad1[0x40 - 0x14]; };
static_assert(sizeof(CStoredCar) == 0x40);
constexpr int MAX_NUM_SAFEHOUSES = 20, MAX_CARS_IN_SAFEHOUSE = 4;
struct CGarages {
    static inline auto& aCarsInSafeHouse = StaticRef<CStoredCar[MAX_NUM_SAFEHOUSES][MAX_CARS_IN_SAFEHOUSE]>(0x96ABD8);
    static CStoredCar* GetStoredCarsInSafehouse(int32 iSafehouseInd) { return aCarsInSafeHouse[iSafehouseInd]; }
    __declspec(noinline) static int32 FindSafeHouseIndexForGarageType(eGarageType type) { volatile int v = type; return v; }
    static int32 CountCarsInHideoutGarage(eGarageType type);
};
#include "inc/garages.inc"
// 6 FindReplacementUpgradeCB
struct RwObject { uint8 type; uint8 subType; uint8 flags; uint8 privateFlags; void* parent; };
struct RpAtomic { RwObject object; };
constexpr int rpATOMIC = 1;
#define RwObjectGetType(o) (((const RwObject*)(o))->type)
enum eAtomicComponentFlag { ATOMIC_UPGRADE = 0x800 };
struct CVisibilityPlugins {
    __declspec(noinline) static int32 GetAtomicId(RpAtomic* atomic) { volatile int v = int(atomic->object.type); return v; }
    __declspec(noinline) static int32 GetModelInfoIndex(RpAtomic* atomic) { volatile int v = int(atomic->object.flags); return v; }
};
RwObject* FindReplacementUpgradeCB(RwObject* object, void* data);
#include "inc/upgrade.inc"
// 7 GetAnimHierarchyFromFrameCallback
struct RwFrame { uint8 bytes[0xA0]; };
struct RpHAnimHierarchy { uint8 bytes[16]; };
typedef RwFrame* (*RwFrameCallBack)(RwFrame* frame, void* data);
RpHAnimHierarchy* RpHAnimFrameGetHierarchy(RwFrame* frame) { return ((RpHAnimHierarchy * (__cdecl*)(RwFrame*))0x7C5160)(frame); }
RwFrame* RwFrameForAllChildren(RwFrame* frame, RwFrameCallBack callBack, void* data) { return ((RwFrame * (__cdecl*)(RwFrame*, RwFrameCallBack, void*))0x7F0DC0)(frame, callBack, data); }
RwFrame* GetAnimHierarchyFromFrameCallback(RwFrame* frame, void* data);
#include "inc/hierarchy.inc"
// 8 CAEStreamingDecoder / CAEDataStream
class CAEDataStream {
public:
    virtual void Reserved() {}
    void* m_pFileHandle; char* m_pszFilename; bool m_bIsOpen; uint32 m_nCurrentPosition, m_nStartPosition, m_nLength, m_nTrackId; bool m_bIsEncrypted; uint32 m_lRefCount;
    ~CAEDataStream();
    bool Close() { volatile int sink = 1; (void)sink; return true; }
};
static_assert(sizeof(CAEDataStream) == 0x28);
#include "inc/datastream.inc"
class CAEStreamingDecoder {
public:
    virtual void Reserved() {}
    CAEStreamingDecoder* ScalarDeletingDestructor(int32 flags);
protected:
    CAEDataStream* m_dataStream;
};
static_assert(sizeof(CAEStreamingDecoder) == 8);
#include "inc/decoder.inc"

#include "../Core.hpp"
using namespace Dependencies25ALive;

static uintptr_t Body(auto pointer) { return Resolve(std::bit_cast<uintptr_t>(pointer)); }

// ---------------------------------------------------------------- imagen del EXE
static bool MapExeInto(HANDLE proc) {
    HANDLE f = CreateFileA("D:/Grand Theft Auto San Andreas/gta-sa.exe", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    const DWORD size = GetFileSize(f, nullptr);
    std::vector<uint8> raw(size);
    DWORD got{};
    ReadFile(f, raw.data(), size, &got, nullptr);
    CloseHandle(f);
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(raw.data());
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS32*>(raw.data() + dos->e_lfanew);
    auto* sec = IMAGE_FIRST_SECTION(nt);
    void* whole = VirtualAllocEx(proc, reinterpret_cast<void*>(nt->OptionalHeader.ImageBase), nt->OptionalHeader.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (whole != reinterpret_cast<void*>(nt->OptionalHeader.ImageBase)) { std::printf("no se pudo reservar la imagen\n"); return false; }
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const uintptr_t va = nt->OptionalHeader.ImageBase + sec[i].VirtualAddress;
        SIZE_T written{};
        if (!WriteProcessMemory(proc, reinterpret_cast<void*>(va), raw.data() + sec[i].PointerToRawData, (std::min)(sec[i].SizeOfRawData, sec[i].Misc.VirtualSize), &written)) return false;
    }
    return true;
}
static int Launcher(int argc, char** argv) {
    char self[MAX_PATH]; GetModuleFileNameA(nullptr, self, MAX_PATH);
    std::string cmd = std::string("\"") + self + "\" --child";
    for (int i = 1; i < argc; ++i) cmd += std::string(" \"") + argv[i] + "\"";
    STARTUPINFOA si{}; si.cb = sizeof(si); PROCESS_INFORMATION pi{};
    if (!CreateProcessA(nullptr, cmd.data(), nullptr, nullptr, TRUE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi)) { std::printf("CreateProcess fallo\n"); return 2; }
    if (!MapExeInto(pi.hProcess)) { TerminateProcess(pi.hProcess, 3); return 3; }
    ResumeThread(pi.hThread);
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD code{}; GetExitCodeProcess(pi.hProcess, &code);
    return int(code);
}

static int Child(int argc, char** argv) {
    const unsigned cases = argc > 2 ? unsigned(std::atoi(argv[2])) : kCases;
    const char* path = argc > 3 ? argv[3] : "comparisons.jsonl";
    Dependencies25ALive::log.open(path);
    Env env;
    env.cases = cases;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker));
    env.game = 0x400000;
    struct Row { const char* cls; const char* name; uintptr_t address; uintptr_t body; };
    const Row rows[kFunctions]{
        {"CTask", "Destructor", 0x61A3A0, Body(&CTask::Destructor)},
        {"CVehiclePool", "GetAt", 0x41CC10, Body(&CVehiclePool::GetAt)},
        {"CAutoPilot", "RemoveOnePathNode", 0x41B950, Body(&CAutoPilot::RemoveOnePathNode)},
        {"CPedGroups", "RemoveAllFollowersFromGroup", 0x5FB8A0, Body(&CPedGroups::RemoveAllFollowersFromGroup)},
        {"CDecisionMakerTypes", "FlushDecisionMakerEventResponse", 0x604490, Body(&DecisionStub::CDecisionMakerTypes::FlushDecisionMakerEventResponse)},
        {"CGarages", "CountCarsInHideoutGarage", 0x44A210, Body(&CGarages::CountCarsInHideoutGarage)},
        {"CVehicle", "FindReplacementUpgradeCB", 0x6D3490, Body(&FindReplacementUpgradeCB)},
        {"", nullptr, 0x734A70, 0},
        {"CAEStreamingDecoder", "ScalarDeletingDestructor", 0x4F2860, Body(&CAEStreamingDecoder::ScalarDeletingDestructor)}};
    json registered = json::object(), bodies = json::object();
    bool ready = true;
    for (unsigned i = 0; i < kFunctions; ++i) {
        if (!rows[i].name) continue;
        env.routes[i].reset(new OriginalDependencies499::HookRoute(rows[i].cls, rows[i].name, rows[i].address, rows[i].body));
        registered[std::string(rows[i].cls) + "::" + rows[i].name] = true;
        bodies[std::string(rows[i].cls) + "::" + rows[i].name] = rows[i].body;
        ready &= ModuleOf(rows[i].body) == env.asi;
    }
    env.directDll = Body(&GetAnimHierarchyFromFrameCallback);
    env.dllRemoveAll = Body(&CPedGroup::RemoveAllFollowers);
    env.dllSetDefault = Body(&DecisionStub::CDecision::SetDefault);
    env.dllFindSafehouse = Body(&CGarages::FindSafeHouseIndexForGarageType);
    env.dllAtomicId = Body(&CVisibilityPlugins::GetAtomicId);
    env.dllModelIndex = Body(&CVisibilityPlugins::GetModelInfoIndex);
    env.dllHierarchy = Body(&RpHAnimFrameGetHierarchy);
    env.dllChildren = Body(&RwFrameForAllChildren);
    Run(env, ready, registered, bodies);
    std::printf("Dependencies25A (sin juego): %u pares por funcion, terminado\n", cases);
    Dependencies25ALive::log.close();
    return 0;
}

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "--child") return Child(argc, argv);
    return Launcher(argc, argv);
}
