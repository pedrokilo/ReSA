// Vuelca las filas `input` de las 5 x 4096 fixtures de Fixtures.hpp (tal cual, compilado con cl) y las constantes de arena, para fixture_selfcheck.py. Una linea JSON por caso:
// la primera linea es `layout`. Las claves coinciden con las que registra Core.hpp (FxInput / PrjInput / RollInput).
#include <cstdio>
#include <string>
#include "Fixtures.hpp"
using namespace Helpers65CFix;
static std::string Hex(const u8* p, size_t n) { static const char d[] = "0123456789abcdef"; std::string o; for (size_t i = 0; i < n; ++i) { o += d[p[i] >> 4]; o += d[p[i] & 15]; } return o; }
static void V3(const char* k, const u32* v) { std::printf("\"%s\":[%u,%u,%u],", k, v[0], v[1], v[2]); }
int main() {
    std::printf("{\"layout\":{\"fx_arena\":%u,\"prj_arena\":%u,\"roll_arena\":%u,\"obj_bytes\":%u,\"task_bytes\":%u,\"veh_bytes\":%u,\"hand_bytes\":%u,\"cases\":%u}}\n",
        kFxArena, kPrjArena, kRollArena, kObjBytes, kTaskBytes, kVehBytes, kHandBytes, kCases);
    for (unsigned func = 0; func < 3; ++func) for (unsigned n = 0; n < kCases; ++n) {
        const FxCase f = MakeFx(func, n);
        std::printf("{\"func\":%u,\"n\":%u,\"family\":%u,\"cam_mode\":%u,", f.func, f.n, f.family, f.camMode);
        V3("cam", f.cam); V3("other", f.other); V3("pos", f.pos); V3("dir", f.dir);
        std::printf("\"create_kind\":%u,\"this_kind\":%u,\"mseed\":%u,\"mode\":%u,\"seed\":%u,\"eax\":%u,\"edx\":%u,\"stack_seed\":%u}\n", f.createKind, f.thisKind, f.mseed, f.mode, f.seedArena, f.eax, f.edx, f.stack);
    }
    for (unsigned n = 0; n < kCases; ++n) {
        const PrjCase f = MakePrj(n);
        static u8 ref[kPrjArena];
        FillArena(ref, kPrjArena, f.mode, f.seedArena);
        std::printf("{\"func\":3,\"n\":%u,\"family\":%u,\"model_kind\":%u,\"mode\":%u,\"obj_off\":%u,\"model_id\":%u,\"seed_arena\":%u,\"seed_p\":%u,\"pre\":\"%s\",\"eax\":%u,\"edx\":%u,\"stack_seed\":%u}\n",
            f.n, f.family, f.modelKind, f.mode, f.objOff, f.modelId, f.seedArena, f.seedP, Hex(ref + f.objOff, kObjBytes).c_str(), f.eax, f.edx, f.stack);
    }
    for (unsigned n = 0; n < kCases; ++n) {
        const RollCase f = MakeRoll(n);
        static u8 ref[kRollArena];
        FillArena(ref, kRollArena, f.mode, f.seedArena);
        u8 task[kTaskBytes];
        std::memcpy(task, ref + kTaskOff, kTaskBytes);
        const u32 tv = f.taskVeh == 3 ? 0u : 1u + kVehOff + f.taskVeh * kVehBytes;      // etiqueta de arena (1 + desplazamiento)
        std::memcpy(task + kTaskVehicle, &tv, 4);
        task[kTaskFlags] = u8(f.flags);
        std::printf("{\"func\":4,\"n\":%u,\"family\":%u,\"mode\":%u,\"seed_arena\":%u,\"task_veh\":%u,\"flags\":%u,", f.n, f.family, f.mode, f.seedArena, f.taskVeh, f.flags);
        V3("driver", f.driver); V3("handling", f.handling); V3("anim_group", f.animGroup);
        std::printf("\"mut_a\":%u,\"idx_a\":%u,\"mut_b\":%u,\"idx_b\":%u,\"mut_c\":%u,\"idx_c\":%u,\"new_driver\":%u,\"group_ret\":%u,\"assoc\":%u,\"eax\":%u,\"ecx\":%u,\"edx\":%u,\"stack_seed\":%u,\"task_pre\":\"%s\"}\n",
            f.mutA, f.idxA, f.mutB, f.idxB, f.mutC, f.idxC, f.newDriver, f.groupRet, f.assoc, f.eax, f.ecx, f.edx, f.stack, Hex(task, kTaskBytes).c_str());
    }
}
