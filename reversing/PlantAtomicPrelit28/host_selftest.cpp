// Autoprueba SIN juego del banco PlantAtomicPrelit28 (consola x86 de VS; NO es el proyecto): ejecuta una transcripcion LITERAL del desensamblado de 0x5DD100 (por desplazamientos,
// con los mismos accesos, tamanos de copia y orden de llamadas que el EXE) contra los MISMOS falsos y arenas que usa el banco (Fakes.hpp), para los 4096 fixtures reales de
// Fixtures.hpp, y vuelca la fila `observed` de cada caso. `python -I fixture_oracle.py --check-host <salida>` la compara con el oraculo Python: valida el oraculo, los falsos, los
// hashes enmascarados y la deteccion de guardas SIN depender de la DLL ni del juego. NO prueba el C++ de la DLL (eso lo hace el banco dentro del juego).
// Compilacion (consola x86 de VS):  cl /nologo /EHsc /std:c++20 /O2 /Fe:host_selftest.exe host_selftest.cpp      Uso:  host_selftest.exe > host.jsonl
#include <cstdio>
#include <cstdint>
#include "Fakes.hpp"
using namespace PlantPrelitFix;

static u8* P(const u8* base, u32 at) { return reinterpret_cast<u8*>(uintptr_t(Get32(base, at))); }

// Transcripcion de 0x5DD100 (cdecl). Offsets del desensamblado: geometria +8 flags, +10h numTri, +14h numVert, +2Ch triangulos, +34h texCoords[0], +5Ch morph target;
// morph target +14h vertices; atomo +18h geometria.
static u8* Exe5DD100(u8* atomic) {
    u8* const ebx = P(atomic, 0x18);
    if (Get32(ebx, 8) & 8) return atomic;                       // test byte [ebx+8],8 ; jne
    const i32 numTri = i32(Get32(ebx, 0x10));
    const i32 numVert = i32(Get32(ebx, 0x14));
    u8* const oldMorph = P(ebx, 0x5C);
    u8* const ebp = static_cast<u8*>(FakeCreate(numVert, numTri, 0xD));
    u8* const newTex = P(ebp, 0x34);
    u8* const newMorph = P(ebp, 0x5C);
    std::memcpy(P(newMorph, 0x14), P(oldMorph, 0x14), size_t(numVert) * 12);   // lea ecx,[esi+esi*2] ; shl ecx,2 ; rep movs
    std::memcpy(newTex, P(ebx, 0x34), size_t(numVert) * 8);                    // add ecx,ecx ; rep movs dword
    std::memcpy(P(ebp, 0x2C), P(ebx, 0x2C), size_t(numTri) * 8);               // lea ecx,[edx+edx] ; rep movs dword
    if (numTri > 0) {                                                          // test edx,edx ; jle
        u8* const oldTri = P(ebx, 0x2C);
        u8* const newTri = P(ebp, 0x2C);
        for (i32 i = 0; i < numTri; ++i) {
            void* const m = FakeGetMaterial(ebx, oldTri + 8 * i);
            FakeSetMaterial(ebp, newTri + 8 * i, m);
        }
    }
    FakeUnlock(ebp);
    Put32(ebp, 8, Get32(ebp, 8) | 2);                                          // or edi,2 ; mov [ebp+8],edi
    FakeSetGeometry(atomic, ebp, 1);
    return atomic;
}

int main() {
    for (unsigned n = 0; n < kCases; ++n) {
        const Fx f = Make(n);
        FillOld(f); oldRef = oldArena;
        FillNew(f); newRef = newArena;
        nev = 0; evLost = 0;
        Exe5DD100(oldArena.data() + kAtomicAt);
        std::printf("{\"case\":%u,\"input\":{\"group\":%d,\"family\":%d,\"mode\":%u,\"flags\":%u,\"num_v\":%d,\"num_t\":%d,\"old_seed\":%u,\"data_seed\":%u,\"new_seed\":%u,\"new_flags\":%u,\"eax\":0,\"ecx\":0,\"edx\":0,\"stack_seed\":0},",
                    n, f.group, f.family, f.mode, f.flags, f.numV, f.numT, f.oldSeed, f.dataSeed, f.newSeed, f.newFlags);
        std::printf("\"observed\":{\"trace\":[");
        for (unsigned i = 0; i < nev; ++i) std::printf("%s[%u,%u,%u,%u,%u]", i ? "," : "", evs[i].kind, evs[i].a, evs[i].b, evs[i].c, evs[i].d);
        std::printf("],\"ev_lost\":%u,\"new_hash\":\"%s\",\"new_ptrs_ok\":%s,\"old_ok\":%s}}\n", evLost, Hex64(MaskedNewHash()).c_str(), NewPtrsOk() ? "true" : "false", oldArena == oldRef ? "true" : "false");
    }
    return 0;
}
