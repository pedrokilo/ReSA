// Comprobacion SIN el juego (ver host_check.py): compila el CUERPO REAL de las cuatro GetVal (extraido de source/game_sa/Fx/Blueprints/Interp/*.cpp) y la plantilla real
// FxInterpInfo_c::InterpolateKeys (FxInterpInfo.h) con /O2 /Oi /arch:IA32 /fp:fast (como en la DLL), fija la palabra de control x87 a 24 bits y ejecuta las 16384 fixtures
// sobre la arena del banco; imprime la arena resultante (punteros como desplazamientos) para compararla con fixture_oracle.py.
#include "FxTools.h"
#include "FxInterpInfo.h"
#include "FxInterpInfoFloat.h"
#include "FxInterpInfoU255.h"
#include "FxInterpInfo32.h"
#include "FxInterpInfo255.h"
#include "Fixtures.hpp"
#include <cfloat>
#include <cstdio>
#include <float.h>
using namespace FxInterp61Fix;
#include "getval_extract.inc"
alignas(16) static u8 g_arena[kArena];
static void Normalize(u8* a, u32 base) {
    for (u32 off : {kObj + 8u, kObj + 12u}) { u32 v; std::memcpy(&v, a + off, 4); v -= base; std::memcpy(a + off, &v, 4); }
    for (u32 r = 0; r < kPtrCap; ++r) { u32 v; std::memcpy(&v, a + kPtrs + 4 * r, 4); v -= base; std::memcpy(a + kPtrs + 4 * r, &v, 4); }
}
int main() {
    _controlfp(_PC_24 | _RC_NEAR, _MCW_PC | _MCW_RC);
    const u32 base = u32(reinterpret_cast<std::uintptr_t>(g_arena));
    for (unsigned func = 0; func < kFunctions; ++func) {
        for (unsigned n = 0; n < kCases; ++n) {
            const F f = MakeF(func, n);
            BuildArena(f, g_arena, base);
            void* obj = g_arena + kObj;
            float* out = reinterpret_cast<float*>(g_arena + kOut);
            const float delta = AsFloat(f.delta);
            switch (func) {
            case 0: static_cast<FxInterpInfoFloat_c*>(obj)->GetVal(out, delta); break;
            case 1: static_cast<FxInterpInfoU255_c*>(obj)->GetVal(out, delta); break;
            case 2: static_cast<FxInterpInfo32_c*>(obj)->GetVal(out, delta); break;
            default: static_cast<FxInterpInfo255_c*>(obj)->GetVal(out, delta); break;
            }
            Normalize(g_arena, base);
            std::printf("{\"func\":%u,\"case\":%u,\"arena\":\"", func, n);
            for (u32 i = 0; i < kArena; ++i) std::printf("%02x", g_arena[i]);
            std::printf("\"}\n");
        }
    }
}
