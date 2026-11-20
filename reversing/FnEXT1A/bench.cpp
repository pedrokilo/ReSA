// Banco EXT1A: funciones matematicas de D3DX9 enlazadas en gta_sa.exe (EXE original mapeado, solo lectura) contra d3dx9_43.dll (DLL del sistema, SysWOW64).
// Uso: bench.exe <cases.bin> <gta_sa.exe> <config: sse2|sse|x87> [mutacion] [archivo de detalle]
// Cada caso se ejecuta con los mismos operandos en buffers identicos (lado EXE y lado DLL); se comparan bit a bit las salidas, el valor devuelto y las guardas de memoria.
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <algorithm>
#pragma pack(push, 1)
struct Rec { uint32_t fn, modo, count, clase; uint32_t in[64]; double exp[64]; double tol[64]; };
#pragma pack(pop)
static bool map_exe(const char* path) {
    FILE* f = fopen(path, "rb"); if (!f) { printf("no se puede abrir el EXE\n"); return false; }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    std::vector<uint8_t> d(n); fread(d.data(), 1, n, f); fclose(f);
    auto nt = (IMAGE_NT_HEADERS32*)(d.data() + ((IMAGE_DOS_HEADER*)d.data())->e_lfanew);
    if (nt->OptionalHeader.ImageBase != 0x400000) return false;
    printf("EXE tamano %ld\n", n);
    DWORD oldp;
    if (!VirtualProtect((void*)0x401000, 0x8B0000, PAGE_EXECUTE_READWRITE, &oldp)) { printf("no se pudo abrir la reserva\n"); return false; }
    auto sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        uintptr_t sva = 0x400000 + sec[i].VirtualAddress; size_t vsz = sec[i].Misc.VirtualSize, rsz = std::min<size_t>(sec[i].SizeOfRawData, vsz);
        uintptr_t lo = std::max<uintptr_t>(sva, 0x401000), hi = std::min<uintptr_t>(sva + vsz, 0x401000 + 0x8B0000);
        for (uintptr_t p = lo; p < hi; p++) *(uint8_t*)p = (p - sva < rsz) ? d[sec[i].PointerToRawData + (p - sva)] : 0;
    }
    return true;
}
typedef void* (__stdcall* t3)(void*, const void*, const void*);   // Vec3Transform*, MatrixMultiply*
typedef void* (__stdcall* t2)(void*, const void*);                // MatrixTranspose
typedef void* (__stdcall* tH)(void*, const void*, uint32_t);      // Float16To32Array
struct Fn { uint32_t exe; const char* dll; int kind; };
static const Fn FN[7] = {{0, "", 0},
    {0x766A32, "D3DXVec3TransformCoord", 3}, {0x766AB7, "D3DXVec3TransformNormal", 3}, {0x7672E1, "D3DXMatrixMultiply", 3},
    {0x7675EE, "D3DXMatrixTranspose", 2}, {0x7676C7, "D3DXMatrixMultiplyTranspose", 3}, {0x76626B, "D3DXFloat16To32Array", 4}};
static uintptr_t dllfn[7];
static int g_mut = 0, g_alt5 = 0;
static const uint32_t GUARD = 0xA5A5A5A5u;
static const int PRE = 8;
struct Out { uint32_t w[64]; int n; uintptr_t ret; int guardbad; };
static bool run_one(const Rec& r, bool exe, Out& o) {
    alignas(16) static uint32_t A[160], B[160], O[160];
    const int mis = (r.modo >> 4) & 3;
    int nA = 0, nB = 0, no = 0;
    switch (r.fn) { case 1: case 2: nA = 3; nB = 16; no = 3; break; case 3: case 5: nA = 16; nB = 16; no = 16; break; case 4: nA = 16; no = 16; break; case 6: nA = (r.count + 1) / 2; no = r.count; break; }
    for (int i = 0; i < 160; i++) A[i] = B[i] = O[i] = GUARD;
    uint32_t* pa = A + PRE + mis; uint32_t* pb = B + PRE + mis; uint32_t* po = O + PRE + mis;
    for (int i = 0; i < nA; i++) pa[i] = r.in[i];
    for (int i = 0; i < nB; i++) pb[i] = r.in[nA + i];
    uint32_t* out = po;
    if (r.fn != 6) { if (r.modo & 1) out = pa; else if ((r.modo & 2) && (r.fn == 3 || r.fn == 5)) out = pb; }
    void* res = nullptr;
    const uintptr_t fp = exe ? (uintptr_t)FN[r.fn].exe : dllfn[r.fn];
    __try {
        switch (FN[r.fn].kind) {
        case 3:
            if (!exe && r.fn == 5 && g_alt5) { alignas(16) static float T[16]; ((t3)dllfn[3])(T, pa, pb); res = ((t2)dllfn[4])(out, T); }
            else res = ((t3)fp)(out, pa, pb);
            break;
        case 2: res = ((t2)fp)(out, pa); break;
        case 4: res = ((tH)fp)(out, pa, r.count); break;
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    o.ret = (uintptr_t)res - (uintptr_t)out; o.n = no;
    for (int i = 0; i < no; i++) o.w[i] = out[i];
    int bad = 0;
    // guardas: palabras previas y posteriores a cada region (salvo la region de salida si es la misma que una entrada)
    for (int i = 0; i < PRE + mis; i++) { bad += (A[i] != GUARD) + (B[i] != GUARD) + (O[i] != GUARD); }
    for (int i = PRE + mis + 16 + 1; i < 160; i++) { /* tras la region mayor posible */ if (out != po || i >= PRE + mis + no) bad += (O[i] != GUARD && out == po); }
    for (int i = PRE + mis + nA; i < 160; i++) { if (out != pa && A[i] != GUARD) bad++; }
    for (int i = PRE + mis + nB; i < 160; i++) { if (out != pb && B[i] != GUARD) bad++; }
    for (int i = PRE + mis + no; i < 160; i++) { if (out == po && O[i] != GUARD) bad++; }
    o.guardbad = bad;
    return true;
}
// Prueba negativa: alteraciones del resultado del lado DLL (la DLL del sistema no se modifica) solo en entradas muy concretas.
static int mutate(const Rec& r, Out& d) {
    switch (g_mut) {
    case 1:  if (r.fn == 3 && (r.in[3] & 7) == 5) { d.w[5] ^= 1; return 1; } break;
    case 2:  if (r.fn == 1 && (r.in[0] & 0xff) == 0x11) { d.w[1] ^= 0x80000000u; return 1; } break;
    case 3:  if (r.fn == 2 && ((r.in[1] >> 12) & 0xff) == 0x33) { d.w[2] += 1; return 1; } break;
    case 4:  if (r.fn == 4 && ((r.in[7] >> 3) & 0x3f) == 9) { std::swap(d.w[1], d.w[4]); return 1; } break;
    case 5:  if (r.fn == 5 && ((r.in[9] >> 4) & 0x7f) == 21) { d.w[15] ^= 0x800; return 1; } break;
    case 6:  if (r.fn == 6 && r.count == 17) { d.w[16] ^= 0x400000; return 1; } break;
    case 11: if (r.fn == 3 && r.modo == 0 && ((r.in[16] >> 5) & 0x1ff) == 77) { d.w[10] += 1; return 1; } break;
    case 12: if (r.fn == 1 && (r.modo & 1) && ((r.in[2] >> 9) & 0x7f) == 5) { d.w[0] += 1; return 1; } break;
    case 13: if (r.fn == 2 && (r.modo & 1) && ((r.in[2] >> 3) & 0x3f) == 33) { d.w[1] ^= 1; return 1; } break;
    case 14: if (r.fn == 4 && (r.modo & 1) && ((r.in[5] >> 2) & 0x7f) == 13) { d.w[2] ^= 0x100; return 1; } break;
    case 15: if (r.fn == 5 && (r.modo & 2) && ((r.in[0] >> 6) & 0x7f) == 44) { d.w[0] ^= 0x800; return 1; } break;
    case 16: if (r.fn == 6 && r.count == 8 && (r.in[0] & 0xf) == 7) { d.w[3] += 1; return 1; } break;
    }
    return 0;
}
struct Stat { uint64_t cas[2] = {0, 0}, dif[2] = {0, 0}, nanonly = 0, orcheck = 0, orfail_exe = 0, orfail_dll = 0, tolfail = 0, retdif = 0, guard = 0, fault = 0, mutapl = 0, mutdet = 0; };
int main(int argc, char** argv) {
    if (argc < 4) { printf("uso: bench cases.bin gta_sa.exe sse2|sse|x87 [mut] [detalle]\n"); return 2; }
    if (argc > 4) g_mut = atoi(argv[4]);
    if (getenv("EXT1A_ALT5")) g_alt5 = 1;
    if (!map_exe(argv[2])) return 2;
    const char* cfg = argv[3];
    // Selector de variante del EXE como 0x781A87: copia de la tabla por defecto (0x8D75B0 -> 0x8D7490), 0x7ACF59 (x87) y despues SSE2 (0x7AC5E2) o SSE (0x7AC441)
    uint32_t* table = (uint32_t*)0x8d7490; memcpy(table, (void*)0x8d75b0, 0x47 * 4);
    ((void(__cdecl*)(void*))0x7acf59)(table);
    if (!strcmp(cfg, "sse2")) ((void(__cdecl*)(void*))0x7ac5e2)(table);
    else if (!strcmp(cfg, "sse")) ((void(__cdecl*)(void*))0x7ac441)(table);
    printf("configuracion EXE: %s; punteros: [0x8D749C]=%08x [0x8D74A0]=%08x [0x8D74A8]=%08x [0x8D74B8]=%08x [0x8D7524]=%08x\n", cfg,
           *(uint32_t*)0x8d749c, *(uint32_t*)0x8d74a0, *(uint32_t*)0x8d74a8, *(uint32_t*)0x8d74b8, *(uint32_t*)0x8d7524);
    HMODULE h = LoadLibraryA("d3dx9_43.dll"); if (!h) { printf("no se carga d3dx9_43.dll\n"); return 2; }
    char path[MAX_PATH]; GetModuleFileNameA(h, path, MAX_PATH); printf("DLL: %s\n", path);
    for (int i = 1; i <= 6; i++) { dllfn[i] = (uintptr_t)GetProcAddress(h, FN[i].dll); if (!dllfn[i]) { printf("falta el export %s\n", FN[i].dll); return 2; } }
    FILE* f = fopen(argv[1], "rb"); if (!f) { printf("sin casos\n"); return 2; }
    static Stat S[7]; static Rec r; FILE* det = argc > 5 ? fopen(argv[5], "w") : nullptr; int shown[7] = {0};
    while (fread(&r, sizeof r, 1, f) == 1) {
        if (r.fn < 1 || r.fn > 6) continue;
        Stat& s = S[r.fn]; const int c = r.clase;
        Out e, d;
        if (!run_one(r, true, e) || !run_one(r, false, d)) { s.fault++; continue; }
        const int mu = mutate(r, d); if (mu) s.mutapl++;
        s.cas[c]++;
        bool same = e.n == d.n && !memcmp(e.w, d.w, e.n * 4);
        if (!same) {
            bool nanonly = true;
            for (int i = 0; i < e.n; i++) if (e.w[i] != d.w[i]) { float x, y; memcpy(&x, &e.w[i], 4); memcpy(&y, &d.w[i], 4); if (!(isnan(x) && isnan(y))) { nanonly = false; break; } }
            if (nanonly) s.nanonly++;
            else { s.dif[c]++;
                   if (det && shown[r.fn] < 8) { shown[r.fn]++; fprintf(det, "fn%u clase%d modo%x n%u: ", r.fn, c, r.modo, r.count); for (int i = 0; i < e.n; i++) fprintf(det, "%08x/%08x ", e.w[i], d.w[i]); fprintf(det, "\n"); } }
        }
        if (e.ret != d.ret) s.retdif++;
        if (e.guardbad || d.guardbad) s.guard++;
        bool tolbad = false;
        if (c == 0) {
            bool chk = false, badE = false, badD = false;
            for (int i = 0; i < e.n; i++) {
                const double tol = r.tol[i]; if (tol < 0) continue; chk = true;
                float x, y; memcpy(&x, &e.w[i], 4); memcpy(&y, &d.w[i], 4);
                if (!(fabs((double)x - r.exp[i]) <= tol)) badE = true;
                if (!(fabs((double)y - r.exp[i]) <= tol)) badD = true;
                if (!(fabs((double)x - (double)y) <= tol)) tolbad = true;
            }
            if (chk) { s.orcheck++; s.orfail_exe += badE; s.orfail_dll += badD; }
        }
        if (c == 0 && tolbad) s.tolfail++;
        if (mu && !same && (r.fn != 5 || tolbad)) s.mutdet++;
    }
    fclose(f);
    uint64_t tot = 0;
    printf("\nfn,nombre,pares_normal,dif_normal,pares_borde,dif_borde,solo_NaN,oraculo_comprobados,fallos_oraculo_EXE,fallos_oraculo_DLL,dif_fuera_tolerancia_oraculo,dif_retorno,guardas,fallos_SEH,mutaciones_aplicadas,detectadas\n");
    for (int i = 1; i <= 6; i++) { Stat& s = S[i]; tot += s.dif[0];
        printf("%d,%s,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu\n", i, FN[i].dll, s.cas[0], s.dif[0], s.cas[1], s.dif[1], s.nanonly, s.orcheck, s.orfail_exe, s.orfail_dll, s.tolfail, s.retdif, s.guard, s.fault, s.mutapl, s.mutdet); }
    printf("TOTAL diferencias (clase normal): %llu\n", tot);
    return 0;
}
