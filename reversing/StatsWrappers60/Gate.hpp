#pragma once
// Compuertas de ABI y estado x87 del banco StatsWrappers60 (compartidas por Core.hpp y el arnes offline). Mismas que DecisionMakeDecision27 / FileLoaderLoad2dEffect27.
// Requiere, declarado antes: json (nlohmann) y Cases.hpp (Reg, HexBytes, u8/u32...). Usa __asm (solo en el banco; la produccion no lo usa).
#include <array>
#include <cfloat>
#include <cstring>
#include <float.h>
#include "Cases.hpp"

namespace Stw60 {
inline void SaveFP(void* p) { __asm { mov eax,p } __asm { fnsave [eax] } __asm { frstor [eax] } }
inline void RestoreFP(void* p) { __asm { mov eax,p } __asm { frstor [eax] } }
inline u32 ReadMXCSR() { u32 v{}; __asm { stmxcsr v } return v; }
inline void SetMXCSR(u32 v) { __asm { ldmxcsr v } }
alignas(16) inline std::array<u8, 108> savedFP{};
inline u32 savedMXCSR{};
inline bool savedFPActive{};
inline void RestoreFloatingState() { if (savedFPActive) { RestoreFP(savedFP.data()); SetMXCSR(savedMXCSR); } }
inline bool FloatingStateRestored() { alignas(16) std::array<u8, 108> current{}; SaveFP(current.data()); return current == savedFP && ReadMXCSR() == savedMXCSR; }
inline void PrimeX87() {
    static const double stale[8]{1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
    const double* p = stale;
    __asm { mov eax,p } __asm { fld qword ptr [eax] } __asm { fld qword ptr [eax+8] } __asm { fld qword ptr [eax+16] } __asm { fld qword ptr [eax+24] }
    __asm { fld qword ptr [eax+32] } __asm { fld qword ptr [eax+40] } __asm { fld qword ptr [eax+48] } __asm { fld qword ptr [eax+56] }
    __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) }
}
inline void PrepareFloatingState() {
    RestoreFloatingState(); PrimeX87();
    _controlfp(_PC_24 | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);   // 24 bits, al mas cercano, todas las excepciones enmascaradas (estado del juego)
}
inline u32 FpWord(const std::array<u8, 108>& fp, std::size_t at) { u32 v{}; std::memcpy(&v, fp.data() + at, 4); return v; }
inline bool FpRegisterIsDead(const std::array<u8, 108>& fp, unsigned st) { const u32 ftw = FpWord(fp, 8) & 0xFFFF, top = (FpWord(fp, 4) >> 11) & 7; return ((ftw >> (2 * ((top + st) & 7))) & 3) == 3; }
inline std::array<u8, 108> MaskDeadRegisters(std::array<u8, 108> fp) { for (unsigned st = 0; st < 8; ++st) if (FpRegisterIsDead(fp, st)) std::memset(fp.data() + 28 + 10 * st, 0, 10); return fp; }
inline std::array<u8, 108> NormalizeFp(std::array<u8, 108> fp) {
    u16 fsw{}; std::memcpy(&fsw, fp.data() + 4, 2); fsw &= 0x3800; std::memcpy(fp.data() + 4, &fsw, 2);   // solo TOP
    std::memset(fp.data() + 2, 0, 2); std::memset(fp.data() + 6, 0, 2); std::memset(fp.data() + 10, 0, 2);  // mitades reservadas
    std::memset(fp.data() + 12, 0, 16);                                                                    // FIP, FCS/FOP, FDP, FDS
    return MaskDeadRegisters(fp);
}
inline json FpDifference(const std::array<u8, 108>& a, const std::array<u8, 108>& b) {
    json out = json::array();
    const struct { const char* name; std::size_t at, size; } fields[]{ {"fcw", 0, 2}, {"fsw_top", 4, 2}, {"ftw", 8, 2} };
    for (const auto& f : fields) if (std::memcmp(a.data() + f.at, b.data() + f.at, f.size)) out.push_back(f.name);
    for (unsigned st = 0; st < 8; ++st) if (std::memcmp(a.data() + 28 + 10 * st, b.data() + 28 + 10 * st, 10)) out.push_back("st" + std::to_string(st) + (FpRegisterIsDead(a, st) ? "(dead)" : "(valid)"));
    return out;
}
inline json ResidueJson(const Reg& r) { return {{"eax", r.eax}, {"ecx", r.ecx}, {"edx", r.edx}, {"eflags", r.eflags}}; }

// Llamada con medicion de ABI: siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; argsPtr[0] queda en la cima de la pila; captura los registros y EFLAGS justo tras el `call`.
__declspec(noinline) inline Reg Gate(std::uintptr_t entryAddress, const u32* argsPtr, u32 argCount, u32 seedEax, u32 seedEcx, u32 seedEdx, u32 stackSeed, u32 popBytes) {
    u32 espBefore{}, espAfter{}, outEax{}, outEcx{}, outEdx{}, outFlags{}, ebxAfter{}, esiAfter{}, ediAfter{}, ebpBefore{}, ebpAfter{};
    const u32 entry = static_cast<u32>(entryAddress);
    __asm {
        push ebx
        push esi
        push edi
        mov ebpBefore,ebp
        mov edx,stackSeed
        xor esi,esi
        mov eax,esp
        mov ecx,64
    seedloop:
        sub eax,4
        add esi,9E3779B1h
        mov edi,edx
        xor edi,esi
        mov [eax],edi
        dec ecx
        jnz seedloop
        mov espBefore,esp
        mov ebx,13579BDFh
        mov esi,2468ACE0h
        mov edi,10293847h
        mov ecx,argCount
        mov edx,argsPtr
    pushloop:
        test ecx,ecx
        jz pushed
        push dword ptr [edx+ecx*4-4]
        dec ecx
        jmp pushloop
    pushed:
        mov eax,seedEax
        mov ecx,seedEcx
        mov edx,seedEdx
        call entry
        mov outEax,eax
        mov outEcx,ecx
        mov outEdx,edx
        pushfd
        pop eax
        mov outFlags,eax
        add esp,popBytes
        mov espAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
        pop edi
        pop esi
        pop ebx
    }
    return { outEax, outEcx, outEdx, outFlags, espBefore == espAfter, ebxAfter == 0x13579BDFu && esiAfter == 0x2468ACE0u && ediAfter == 0x10293847u && ebpBefore == ebpAfter };
}
}
