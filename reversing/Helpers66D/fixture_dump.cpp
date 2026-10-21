// Vuelca las filas `input` de las 4 x 4096 fixtures de Fixtures.hpp (tal cual, compilado con cl) y las constantes de arena, para fixture_selfcheck.py. Una linea JSON por caso:
// la primera linea es `layout`. Las claves coinciden con las que registra Core.hpp (ClumpInput / GarageInput / CopInput / HeliInput). Direccion base nominal de la arena: 0x10000000.
#include <cstdio>
#include <string>
#include <vector>
#include "Fixtures.hpp"
using namespace H66Fix;
static std::string Hex(const u8* p, size_t n) { static const char d[] = "0123456789abcdef"; std::string o; for (size_t i = 0; i < n; ++i) { o += d[p[i] >> 4]; o += d[p[i] & 15]; } return o; }
static void V3(const char* k, const u32* v) { std::printf("\"%s\":[%u,%u,%u],", k, v[0], v[1], v[2]); }
constexpr u32 kBase = 0x10000000u;
int main() {
    std::printf("{\"layout\":{\"clump_arena\":%u,\"garage_arena\":%u,\"cop_arena\":%u,\"heli_arena\":%u,\"obj_bytes\":%u,\"heli_bytes\":%u,\"cases\":%u}}\n",
        kClumpArena, kGarageArena, kCopArena, kHeliArena, kObjBytes, kHeliBytes, kCases);
    static u8 ref[0x1800];
    for (unsigned n = 0; n < kCases; ++n) {
        const ClumpCase f = MakeClump(n);
        BuildClumpRef(f, kBase, ref);
        std::printf("{\"func\":0,\"n\":%u,\"family\":%u,\"clump_kind\":%u,\"sphere_kind\":%u,\"use_ltm\":%u,\"count\":%d,", f.n, f.family, f.clumpKind, f.sphereKind, f.useLtm, f.count);
        V3("sum", f.sum);
        std::printf("\"rad_mode\":%u,\"rad\":%u,\"cmod_mode\":%u,", f.radMode, f.rad, f.cmodMode);
        V3("cmod", f.cmod);
        std::printf("\"parent_kind\":%u,\"inv_mask\":%u,", f.parentKind, f.invMask);
        V3("tr_mask", f.trMask);
        std::printf("\"pre_sphere\":\"%s\",\"frame_mat\":\"%s\",\"ltm\":\"%s\",\"mode\":%u,\"seed\":%u,\"eax\":%u,\"ecx\":%u,\"edx\":%u,\"stack_seed\":%u}\n",
            Hex(ref + kSphereOff, 0x10).c_str(), Hex(ref + kFrameOff + 0x10, 0x40).c_str(), Hex(ref + kLtmOff, 0x40).c_str(), f.mode, f.seedArena, f.eax, f.ecx, f.edx, f.stack);
    }
    for (unsigned n = 0; n < kCases; ++n) {
        const GarageCase f = MakeGarage(n);
        u32 thisAddr{}, gb{};
        BuildGarageRef(f, kBase, ref, thisAddr, gb);
        const std::string gbytes = f.garageKind == 0 ? Hex(ref + kGarageOff + f.garageDelta, kGarageReadBytes) : std::string();   // rdata: la rellena fixture_selfcheck.py con los bytes del EXE
        std::printf("{\"func\":1,\"n\":%u,\"family\":%u,\"garage_kind\":%u,\"rdata\":%u,\"garage_addr\":%u,\"garage_bytes\":\"%s\",\"gb\":%u,\"size\":%u,\"base_mode\":%u,\"null_slot\":%u,\"slots\":[",
            f.n, f.family, f.garageKind, f.rdata, thisAddr, gbytes.c_str(), gb, f.size, f.baseMode, f.nullSlot);
        for (u32 j = 0; j < kMaxSlots; ++j) {
            const DoorSlot& s = f.s[j];
            std::printf("%s{\"flag\":%u,\"mat\":%u,\"pos\":[%u,%u,%u],\"other\":[%u,%u,%u],\"byte\":%u}", j ? "," : "", unsigned(ref[kFlagsOff + j]), s.matKind, s.pos[0], s.pos[1], s.pos[2],
                s.other[0], s.other[1], s.other[2], unsigned(ref[kObjOff + j * kObjBytes + kObjByte]));
        }
        std::printf("],\"first_pre\":%u,\"second_pre\":%u,\"mode\":%u,\"seed\":%u,\"eax\":%u,\"ecx\":%u,\"edx\":%u,\"stack_seed\":%u}\n", f.firstPre, f.secondPre, f.mode, f.seedArena, f.eax, f.ecx, f.edx, f.stack);
    }
    for (unsigned n = 0; n < kCases; ++n) {
        const CopCase f = MakeCop(n);
        BuildCopRef(f, kBase, ref);
        std::printf("{\"func\":2,\"n\":%u,\"family\":%u,\"target_kind\":%u,\"cop_kind\":%u,\"mat_kind\":%u,", f.n, f.family, f.targetKind, f.copKind, f.matKind);
        V3("cop_pos", f.copPos); V3("cop_other", f.copOther); V3("dest", f.dest); V3("source_pre", f.sourcePre);
        std::printf("\"source_w3\":%u,\"nrm_mode\":%u,", unsigned(*reinterpret_cast<const u32*>(ref + kSourceOff + 12)), f.nrmMode);
        V3("nrm", f.nrm); V3("cross", f.cross);
        std::printf("\"ground\":%u,\"found_mode\":%u,\"found_byte\":%u,\"mode\":%u,\"seed\":%u,\"eax\":%u,\"ecx\":%u,\"edx\":%u,\"stack_seed\":%u}\n", f.ground, f.foundMode, f.foundByte, f.mode, f.seedArena, f.eax, f.ecx, f.edx, f.stack);
    }
    for (unsigned n = 0; n < kCases; ++n) {
        const HeliCase f = MakeHeli(n);
        std::printf("{\"func\":3,\"n\":%u,\"family\":%u,\"model\":%u,\"created_by\":%u,\"timer\":%u,\"rand_ret\":%u,\"obj_off\":%u,\"mode\":%u,\"seed\":%u,\"seed_p\":%u,\"eax\":%u,\"edx\":%u,\"stack_seed\":%u}\n",
            f.n, f.family, f.model, f.createdBy, f.timer, f.randRet, f.objOff, f.mode, f.seedArena, f.seedP, f.eax, f.edx, f.stack);
    }
}
