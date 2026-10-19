// Prueba SIN el juego del nucleo del banco (Core.hpp, el mismo archivo que se compila en la sonda) y del analizador (run_local.py): ejecuta FxAdd30Live::Run con
//   - original  = los bytes reales del EXE de AddParticle(vector) reubicados (cphadd30.cpp: exebytes.inc),
//   - candidato = el codigo C++ de FxEmitter.cpp (add_body.inc) con FxAddParticleRotZByteCpp de X87Math.cpp,
// a traves de un trampolin que elige uno u otro segun el "hook" simulado. Los colaboradores (stubs de Fixtures.hpp) los llaman ambos lados directamente, asi que los EntryPatch
// del banco son aqui no-ops. Escribe live_test.jsonl con el MISMO formato que el banco dentro del juego; despues:  python -I run_local_check.py live_test.jsonl
#define CPH_NO_MAIN
#include "cphadd30.cpp"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
typedef uint16_t uint16;

namespace ReversingProbe {
struct EntryPatch { template<class F> EntryPatch(uintptr_t, F) {} };
inline bool gCandidate = false;
inline bool JumpsToDll(uintptr_t) { return gCandidate; }
inline std::vector<uint8> Bytes(const void* ptr, size_t n) { const auto* p = static_cast<const uint8*>(ptr); return {p, p + n}; }
inline void PumpMessages() {}
}
namespace OriginalDependencies499 {
struct HookRoute {
    uintptr_t body{};
    template<class F> auto Invoke(bool candidate, F call) { ReversingProbe::gCandidate = candidate; const auto v = call(); ReversingProbe::gCandidate = false; return v; }
    bool Registered() const { return true; }
    uintptr_t Body() const { return body; }
    bool Restore() { return true; }
};
}
#include "../Core.hpp"

static uintptr_t gCppEntry;
__declspec(naked) static void Trampoline() {
    __asm {
        cmp byte ptr [ReversingProbe::gCandidate], 0
        je viaExe
        jmp dword ptr [gCppEntry]
    viaExe:
        jmp dword ptr [gExeCp]
    }
}
static void Dummy0() {} static void Dummy1() {} static void Dummy2() {} static void Dummy3() {} static void Dummy4() {} static void Dummy5() {} static void Dummy6() {}

int main(int argc, char** argv) {
    InitHarness();
    gCppEntry = MemberAddress();
    FxAdd30Live::log.open(argc > 1 ? argv[1] : "live_test.jsonl");
    FxAdd30Live::Env env;
    env.asi = FxAdd30Live::ModuleOf(uintptr_t(&FxAdd30Live::ModuleMarker));
    env.game = env.asi;
    env.cases = argc > 2 ? atoi(argv[2]) : kCases;
    env.route.reset(new OriginalDependencies499::HookRoute());
    env.route->body = gCppEntry;
    env.entry = uintptr_t(&Trampoline);
    uintptr_t dummies[7] = {uintptr_t(&Dummy0), uintptr_t(&Dummy1), uintptr_t(&Dummy2), uintptr_t(&Dummy3), uintptr_t(&Dummy4), uintptr_t(&Dummy5), uintptr_t(&Dummy6)};
    for (unsigned i = 0; i < 7; ++i) { env.exeAt[i] = dummies[i]; env.dllAt[i] = dummies[i]; }
    json registered = json::object(), bodies = json::object();
    registered["FxEmitter_c::AddParticle(vector)"] = true;
    bodies["FxEmitter_c::AddParticle(vector)"] = gCppEntry;
    FxAdd30Live::Run(env, true, registered, bodies);
    return 0;
}
