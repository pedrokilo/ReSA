// FnD3DX9C: banco del ensamblador propio (D3DX9NativeAssembler) contra D3DXAssembleShader de d3dx9_43.dll, con las fuentes que genera el propio
// generador del juego (out/gen_fuente.inc, extraido de rpworld.cpp) para miles de descriptores, mas un corpus de lineas sueltas. Compilar con build_ensamblador.cmd.
// Uso: banco_ensamblador.exe [descriptores=200000] [semilla]
#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <set>
#include <random>
#include <map>
#include "../../source/game_sa/RenderWare/rw/D3DX9NativeAssembler.h"
typedef unsigned char RwUInt8; typedef unsigned int RwUInt32; typedef int RwInt32;
#include "out/gen_fuente.inc"

using Asm = int32_t(__stdcall*)(const void*, uint32_t, const void*, void*, uint32_t, void**, void**);
struct Buf { void** vtbl; };
static const void* Ptr(void* b) { return ((const void*(__stdcall*)(void*))((Buf*)b)->vtbl[3])(b); }
static uint32_t Size(void* b) { return ((uint32_t(__stdcall*)(void*))((Buf*)b)->vtbl[4])(b); }
static void Rel(void* b) { if (b) ((uint32_t(__stdcall*)(void*))((Buf*)b)->vtbl[2])(b); }

static Asm g_dll;
static uint64_t nSrc, nSame, nDiff, nUnsup, nDllFail, nSkipVal;
static std::map<std::string, int> unsupReasons;
static std::map<std::string, std::string> unsupExample;
static std::map<std::string, int> diffByOp;
static std::set<std::string> seen;

static int Dll(const std::string& s, uint32_t flags, std::vector<uint32_t>& bytes, std::string& err) {
    void* code = nullptr; void* errs = nullptr;
    int32_t hr = g_dll(s.data(), (uint32_t)s.size(), nullptr, nullptr, flags, &code, &errs);
    if (errs) { err.assign((const char*)Ptr(errs), Size(errs)); Rel(errs); }
    bytes.clear();
    if (hr >= 0 && code) { const uint32_t* p = (const uint32_t*)Ptr(code); bytes.assign(p, p + Size(code) / 4); }
    Rel(code);
    return hr;
}
static void Probar(const std::string& s, int verbose) {
    if (getenv("TRAZA")) { fprintf(stderr, "PROBAR:\n%s\n", s.c_str()); fflush(stderr); }
    if (!seen.insert(s).second) return;
    nSrc++;
    std::vector<uint32_t> nat, dll; std::string why, err;
    auto r = D3DX9Native::AssembleVertexShader(s.data(), s.size(), nat, why);
    if (r == D3DX9Native::AssembleResult::Unsupported) { nUnsup++; { std::string k = why.substr(why.find(':') == std::string::npos ? 0 : why.find(':') + 2); k = k.substr(0, k.find(' [')); if (!unsupReasons[k]++) unsupExample[k] = why; } return; }
    int hr = Dll(s, 0, dll, err);
    if (hr < 0) { hr = Dll(s, 2, dll, err); nSkipVal++; if (hr < 0) { nDllFail++; if (verbose && nDllFail <= 3) printf("DLL falla incluso sin validar: %s\n--\n%s\n", err.c_str(), s.c_str()); return; } }
    if (nat == dll) { nSame++; return; }
    nDiff++;
    if (verbose && nDiff <= 5) {
        printf("DIFERENCIA (%zu vs %zu tokens)\n%s\n", nat.size(), dll.size(), s.c_str());
        for (size_t i = 0; i < std::max(nat.size(), dll.size()); ++i) printf("  %3zu nativo=%08X dll=%08X %s\n", i, i < nat.size() ? nat[i] : 0, i < dll.size() ? dll[i] : 0, (i < nat.size() && i < dll.size() && nat[i] == dll[i]) ? "" : "<--");
    }
}
int main(int argc, char** argv) {
    int N = argc > 1 ? atoi(argv[1]) : 200000;
    std::mt19937 rng(argc > 2 ? atoi(argv[2]) : 7);
    HMODULE m = LoadLibraryExA("d3dx9_43.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!m) { printf("no se pudo cargar d3dx9_43.dll\n"); return 2; }
    g_dll = (Asm)GetProcAddress(m, "D3DXAssembleShader");
    RwUInt32 ct[16]; for (int i = 0; i < 16; ++i) ct[i] = (i * 3) % 7;
    static char buf[0x40000];
    // 1) generador del juego: descriptores aleatorios y estructurados, versiones 0x14 y 0x19
    for (int i = 0; i < N; ++i) {
        RwUInt8 d[4], out[8];
        if (i % 3 == 0) { for (auto& b : d) b = (RwUInt8)rng(); }
        else { d[0] = (RwUInt8)((rng() % 5) | (rng() % 5) << 4); d[1] = (RwUInt8)((rng() % 4) | (rng() % 5) << 4); d[2] = (RwUInt8)(((rng() % 2)) | ((rng() % 5) << 1) | ((rng() % 2) << 4) | ((rng() % 2) << 5) | ((rng() % 2) << 6) | ((rng() % 2) << 7)); d[3] = (RwUInt8)((rng() % 4) | ((rng() % 13) << 2)); }
        for (RwUInt32 ver : {0x14u, 0x19u}) {
            int len = BuildVertexShaderSource(d, out, buf, ver, 256, ct, ::sprintf);
            if (len > 0 && len < 0xFFF) Probar(std::string(buf, len), 1);
        }
        if (i % 50000 == 0) { printf("... %d descriptores, %llu fuentes distintas, dif=%llu\n", i, (unsigned long long)nSrc, (unsigned long long)nDiff); fflush(stdout); }
    }
    printf("generador: fuentes distintas=%llu iguales=%llu DIFERENTES=%llu no soportadas=%llu (la DLL rechazo la validacion y se uso SKIPVALIDATION: %llu; la DLL fallo del todo: %llu)\n",
           (unsigned long long)nSrc, (unsigned long long)nSame, (unsigned long long)nDiff, (unsigned long long)nUnsup, (unsigned long long)nSkipVal, (unsigned long long)nDllFail);
    for (auto& kv : unsupReasons) printf("  no soportada: %s x%d (ej: %s)\n", kv.first.c_str(), kv.second, unsupExample[kv.first].c_str());
    // 2) casos sueltos de gramatica (cada instruccion con variantes de destino, origen, permutacion, negacion y relativo)
    uint64_t base = nSrc;
    const char* ops1[] = {"mov", "rcp", "rsq", "expp"}; const char* ops2[] = {"add", "mul", "dp3", "dp4", "max", "min", "slt", "sge", "m4x4", "m4x3", "m3x3"};
    const char* dsts[] = {"r0", "r5.xyz", "r7.w", "r3.xy", "oT0.xy", "oT3", "oT2.y", "oD0", "oD1.xyz", "oPos.xyzw", "oPos", "oFog", "oFog.x", "r11.yzw", "r2.xw"};
    const char* srcs[] = {"v0", "v15", "r0", "r11.wzyx", "c0", "c255", "c[5]", "c[3 + 5]", "c[a0.x + 4]", "c[a0.w + 200]", "c[a0.y]", "-r6.x", "-c[1 + 5].xyz", "v3.zxyw", "r1.wwww", "r2.xy", "c[a0.z + 7].yx", "-v5.xxxx"};
    for (const char* ver : {"vs_2_0", "vs_2_x"}) {
        for (auto o : ops1) for (auto d : dsts) for (auto s : srcs) Probar(std::string(ver) + "\n" + o + " " + d + ", " + s + "\n", 1);
        for (auto o : ops2) for (auto d : dsts) for (int a = 0; a < 18; a += 1) for (int b = 0; b < 18; b += 3) Probar(std::string(ver) + "\n" + o + " " + d + ", " + srcs[a] + ", " + srcs[b] + "\n", 1);
        for (auto d : dsts) for (int a = 0; a < 18; a += 2) for (int b = 1; b < 18; b += 2) for (int c = 0; c < 18; c += 5) Probar(std::string(ver) + "\nmad " + d + ", " + srcs[a] + ", " + srcs[b] + ", " + srcs[c] + "\n", 1);
        for (auto s : srcs) for (const char* a : {"a0.x", "a0.xy", "a0.xyzw", "a0"}) Probar(std::string(ver) + "\nmova " + a + ", " + s + "\n", 1);
    }
    for (const char* dcl : {"dcl_position0 v0", "dcl_position1 v14", "dcl_blendweight0 v1", "dcl_blendindices0 v2", "dcl_normal0 v3", "dcl_normal1 v15", "dcl_color0 v4", "dcl_tangent0 v13", "dcl_texcoord0 v5", "dcl_texcoord7 v12", "dcl_texcoord v5", "dcl_psize0 v7", "dcl_binormal0 v8"})
        Probar(std::string("vs_2_x\n") + dcl + "\nmov oPos, v0\n", 1);
    printf("lineas sueltas: fuentes=%llu\n", (unsigned long long)(nSrc - base));
    printf("TOTAL: fuentes=%llu iguales=%llu DIFERENTES=%llu no soportadas=%llu (SKIPVALIDATION usado %llu, DLL fallo del todo %llu)\n", (unsigned long long)nSrc, (unsigned long long)nSame, (unsigned long long)nDiff, (unsigned long long)nUnsup, (unsigned long long)nSkipVal, (unsigned long long)nDllFail);
    for (auto& kv : unsupReasons) printf("  no soportada: %s x%d (ej: %s)\n", kv.first.c_str(), kv.second, unsupExample[kv.first].c_str());
    return nDiff ? 1 : 0;
}
