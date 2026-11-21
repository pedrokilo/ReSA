// Banco CRT2A: pares EXE original vs C++ reconstruido para floor (0x8219F0), ceil (0x823820), srand/rand (0x821B11/0x821B1E), strtok (0x82244B), isprint (0x8228B5)
// y __getptd (0x827B3D). Mismo metodo que FnCRT1A: el gta_sa.exe original (solo lectura) se mapea en 0x400000 dentro de este proceso de 32 bits y se llama por su direccion;
// el juego no se ejecuta. La IAT que usan esas funciones (GetLastError, SetLastError, TlsGetValue, TlsSetValue, GetCurrentThreadId, Heap*) se rellena con las del sistema.
// Uso: bench_crt2a.exe <ruta del EXE> [--casos N] [--semilla S] [--solo mate|rand|strtok|isprint|ptd]   (devuelve 0 si todo coincide)
// Limite: NaN no se compara con el EXE (llama al manejador de errores del CRT, que necesita el estado de arranque): solo se comprueba que el nucleo lo detecta.
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

#define CRT_BENCH
#include "CrtMath.cpp"
#include "CrtState.cpp"

static bool map_exe(const char* path) {
    FILE* f = fopen(path, "rb"); if (!f) { printf("no se puede abrir el EXE\n"); return false; }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    std::vector<uint8_t> d(n); fread(d.data(), 1, n, f); fclose(f);
    auto nt = (IMAGE_NT_HEADERS32*)(d.data() + ((IMAGE_DOS_HEADER*)d.data())->e_lfanew);
    if (nt->OptionalHeader.ImageBase != 0x400000) return false;
    DWORD oldp;
    if (!VirtualProtect((void*)0x401000, 0x8B0000, PAGE_EXECUTE_READWRITE, &oldp)) { printf("no se pudo abrir la reserva (error %lu)\n", GetLastError()); return false; }
    auto sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        uintptr_t sva = 0x400000 + sec[i].VirtualAddress; size_t vsz = sec[i].Misc.VirtualSize, rsz = std::min<size_t>(sec[i].SizeOfRawData, vsz);
        uintptr_t lo = std::max<uintptr_t>(sva, 0x401000), hi = std::min<uintptr_t>(sva + vsz, 0x401000 + 0x8B0000); if (lo >= hi) continue;
        for (uintptr_t p = lo; p < hi; p++) *(uint8_t*)p = (p - sva < rsz) ? d[sec[i].PointerToRawData + (p - sva)] : 0;
    }
    return true;
}

static std::mt19937_64 rng;
static uint32_t rnd(uint32_t n) { return n ? (uint32_t)(rng() % n) : 0; }
static uint64_t dbits(double x) { uint64_t b; memcpy(&b, &x, 8); return b; }
static double bitsd(uint64_t b) { double x; memcpy(&x, &b, 8); return x; }

typedef double(__cdecl* DFn)(double);

// ------------------------------------------------------------------ floor / ceil
static void run_math(uint64_t n, uint64_t fails[2], uint64_t counts[2], uint64_t& nanSeen) {
    *(int*)0xC9C400 = 1; // rama SSE2
    DFn exe[2] = {(DFn)0x8219F0, (DFn)0x823820}, ours[2] = {ExeCrt::Floor, ExeCrt::Ceil};
    // valores de borde
    std::vector<double> edge = {0.0, -0.0, 1.0, -1.0, 0.5, -0.5, 1.5, -1.5, 2.5, -2.5, 0.999999999999999, -0.999999999999999, 1.0000000000000002, -1.0000000000000002,
        4503599627370496.0, -4503599627370496.0, 4503599627370495.5, -4503599627370495.5, 4503599627370497.0, 9007199254740992.0, -9007199254740992.0, 1e300, -1e300,
        bitsd(0x7FF0000000000000ull), bitsd(0xFFF0000000000000ull), bitsd(1), bitsd(0x8000000000000001ull), bitsd(0x000FFFFFFFFFFFFFull), bitsd(0x800FFFFFFFFFFFFFull),
        2.2250738585072014e-308, -2.2250738585072014e-308, 0.1, -0.1, 3.0, -3.0, 123456.789, -123456.789};
    for (uint64_t c = 0; c < n; c++) {
        double x;
        const uint32_t k = rnd(8);
        if (k == 0) x = edge[rnd((uint32_t)edge.size())];
        else if (k == 1) x = bitsd(rng());                                                         // patron de bits arbitrario (incluye NaN)
        else if (k == 2) x = (double)(int32_t)rng() / 256.0;                                       // fracciones exactas
        else if (k == 3) x = (double)(int32_t)rng() / 1000.0;
        else if (k == 4) x = bitsd(((uint64_t)(0x3FF - 3 + rnd(60)) << 52) | (rng() & 0xFFFFFFFFFFFFFull) | ((rng() & 1) << 63)); // exponentes cerca de 0..52
        else if (k == 5) x = std::floor((double)(int32_t)rng() / 7.0) + (rnd(2) ? 0.0 : 1e-9);
        else if (k == 6) x = bitsd(((uint64_t)(rnd(2047)) << 52) | (rng() & 0xFFFFFFFFFFFFFull) | ((rng() & 1) << 63)); // cualquier exponente
        else x = (double)(float)((int32_t)rng() / 65536.0);                                        // valores float como los que usa el juego
        for (int w = 0; w < 2; w++) {
            if (x != x) { // NaN: el nucleo debe avisar
                double r; const bool isnan = w == 0 ? ExeCrt::FloorCore(x, r) : ExeCrt::CeilCore(x, r);
                nanSeen++; if (!isnan) { fails[w]++; printf("NaN no detectado w=%d bits=%016llx\n", w, (unsigned long long)dbits(x)); }
                continue;
            }
            const double a = exe[w](x), b = ours[w](x); counts[w]++;
            if (dbits(a) != dbits(b)) { if (fails[w]++ < 10) printf("DIFERENCIA %s x=%016llx exe=%016llx ours=%016llx\n", w ? "ceil" : "floor", (unsigned long long)dbits(x), (unsigned long long)dbits(a), (unsigned long long)dbits(b)); }
        }
    }
}

// ------------------------------------------------------------------ estado por hilo
static void fill_iat() {
    *(void**)0x858094 = (void*)&GetLastError; *(void**)0x85809c = (void*)&SetLastError;
    *(void**)0x8581E0 = (void*)&TlsGetValue;  *(void**)0x8581E4 = (void*)&TlsSetValue; *(void**)0x8581E8 = (void*)&GetCurrentThreadId;
    *(void**)0x8581F4 = (void*)&HeapReAlloc;  *(void**)0x8581F8 = (void*)&HeapAlloc;   *(void**)0x8581FC = (void*)&HeapFree;
    *(int*)0xC9C2F8 = 1; *(HANDLE*)0xC9C2F4 = GetProcessHeap();
    *(DWORD*)0x8E3110 = TlsAlloc();
}

struct PtdSnap { uint8_t b[0x88]; DWORD err; };
static DWORD WINAPI th_ptd(LPVOID p) { // p[0] = 0 EXE, 1 propio; p[1] = PtdSnap*
    auto args = (uintptr_t*)p; auto snap = (PtdSnap*)args[1];
    SetLastError(0x1234);
    void* ptd = args[0] == 0 ? ((void*(__cdecl*)())0x827B3D)() : (void*)ExeCrt::Getptd();
    snap->err = GetLastError();
    memcpy(snap->b, ptd, 0x88);
    // segunda llamada: debe devolver el mismo bloque
    void* ptd2 = args[0] == 0 ? ((void*(__cdecl*)())0x827B3D)() : (void*)ExeCrt::Getptd();
    if (ptd2 != ptd) snap->err = 0xDEAD;
    return 0;
}

static void run_ptd(uint64_t& fails) {
    PtdSnap s[2];
    for (int i = 0; i < 2; i++) {
        uintptr_t args[2] = {(uintptr_t)i, (uintptr_t)&s[i]};
        HANDLE h = CreateThread(nullptr, 0, th_ptd, args, 0, nullptr); WaitForSingleObject(h, INFINITE); CloseHandle(h);
    }
    *(uint32_t*)s[0].b = 0; *(uint32_t*)s[1].b = 0; // el identificador del hilo difiere
    if (memcmp(s[0].b, s[1].b, 0x88) != 0 || s[0].err != s[1].err) { fails++; printf("DIFERENCIA en __getptd (error %lx/%lx)\n", s[0].err, s[1].err); }
    uint32_t t4 = *(uint32_t*)(s[1].b + 4), h14 = *(uint32_t*)(s[1].b + 0x14), t54 = *(uint32_t*)(s[1].b + 0x54);
    printf("  ptd propio: +4=%x +0x14=%x +0x54=%x ultimo error conservado=%lx\n", t4, h14, t54, s[1].err);
}

// ------------------------------------------------------------------ rand / srand
static void run_rand(uint64_t n, uint64_t& fails, uint64_t& total) {
    typedef int(__cdecl* RFn)(); typedef void(__cdecl* SFn)(unsigned);
    RFn rexe = (RFn)0x821B1E; SFn sexe = (SFn)0x821B11;
    for (uint64_t c = 0; c < n / 16 + 1; c++) {
        const unsigned seed = (c % 7 == 0) ? (unsigned)(c / 7) : (unsigned)rng();
        // 1) srand EXE + rand EXE vs srand propio + rand propio vs mezcla
        int a[24], b[24], m1[24], m2[24];
        sexe(seed); for (int i = 0; i < 24; i++) a[i] = rexe();
        ExeCrt::Srand(seed); for (int i = 0; i < 24; i++) b[i] = ExeCrt::Rand();
        sexe(seed); for (int i = 0; i < 24; i++) m1[i] = (i & 1) ? ExeCrt::Rand() : rexe();   // alternando: el estado es el mismo
        ExeCrt::Srand(seed); for (int i = 0; i < 24; i++) m2[i] = (i & 1) ? rexe() : ExeCrt::Rand();
        total += 24 * 4;
        if (memcmp(a, b, sizeof a) || memcmp(a, m1, sizeof a) || memcmp(a, m2, sizeof a)) { if (fails++ < 5) printf("DIFERENCIA rand seed=%u\n", seed); }
    }
}

// ------------------------------------------------------------------ strtok
static void run_strtok(uint64_t n, uint64_t& fails, uint64_t& total) {
    typedef char*(__cdecl* TFn)(char*, const char*);
    TFn texe = (TFn)0x82244B, tours = ExeCrt::Strtok;
    const char alpha[] = " ,;.-abXY\t\x80\xFF";
    for (uint64_t c = 0; c < n; c++) {
        char s[96], d[12]; const int len = rnd(60), dl = rnd(5);
        for (int i = 0; i < len; i++) s[i] = alpha[rnd(sizeof alpha - 1)];
        s[len] = 0;
        for (int i = 0; i < dl; i++) d[i] = alpha[rnd(sizeof alpha - 1)];
        d[dl] = 0;
        char buf[2][96]; memcpy(buf[0], s, 96); memcpy(buf[1], s, 96);
        std::vector<int> off[2];
        for (int w = 0; w < 2; w++) {
            TFn f = w ? tours : texe; char* t = f(buf[w], d);
            for (int k = 0; k < 70; k++) { off[w].push_back(t ? (int)(t - buf[w]) : -1); if (!t) break; t = f(nullptr, d); }
            // el segundo ciclo cambia de delimitadores a mitad
        }
        total++;
        if (off[0] != off[1] || memcmp(buf[0], buf[1], 96)) { if (fails++ < 5) printf("DIFERENCIA strtok caso %llu\n", (unsigned long long)c); }
    }
}

// ------------------------------------------------------------------ isprint
static void run_isprint(uint64_t& fails, uint64_t& total) {
    static uint16_t table[1 + 256]; // table[0] = entrada para -1
    for (auto& t : table) t = (uint16_t)rng();
    static uint8_t loc[0x80]; memset(loc, 0, sizeof loc);
    *(int*)(loc + 0x28) = 1; *(uint16_t**)(loc + 0x48) = table + 1;
    *(void**)0x8E2F14 = loc;
    ExeCrt::Getptd()->locInfo = loc; // mismo bloque para el EXE (las dos versiones usan el bloque del hilo)
    typedef int(__cdecl* IFn)(int); IFn iexe = (IFn)0x8228B5;
    for (int c = -1; c < 256; c++) { total++; if (iexe(c) != ExeCrt::Isprint(c)) { if (fails++ < 5) printf("DIFERENCIA isprint %d\n", c); } }
}

int main(int argc, char** argv) {
    if (argc < 2) { printf("uso: bench_crt2a <gta_sa.exe> [--casos N] [--semilla S] [--solo mate|rand|strtok|isprint|ptd]\n"); return 2; }
    uint64_t n = 200000, seed = 12345; std::string solo;
    for (int i = 2; i < argc; i++) { if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--solo") && i + 1 < argc) solo = argv[++i]; }
    if (!map_exe(argv[1])) return 2;
    fill_iat();
    rng.seed(seed); int rc = 0;
    if (solo.empty() || solo == "mate") {
        uint64_t fails[2] = {}, counts[2] = {}, nan = 0; run_math(n, fails, counts, nan);
        printf("floor: %llu casos, %llu diferencias; ceil: %llu casos, %llu diferencias; NaN detectados por el nucleo: %llu\n", (unsigned long long)counts[0], (unsigned long long)fails[0], (unsigned long long)counts[1], (unsigned long long)fails[1], (unsigned long long)nan);
        if (fails[0] || fails[1]) rc = 1;
    }
    if (solo.empty() || solo == "ptd") { uint64_t f = 0; run_ptd(f); printf("__getptd: %llu diferencias\n", (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "rand") { uint64_t f = 0, t = 0; run_rand(n, f, t); printf("rand/srand: %llu valores comparados, %llu diferencias\n", (unsigned long long)t, (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "strtok") { uint64_t f = 0, t = 0; run_strtok(n, f, t); printf("strtok: %llu casos, %llu diferencias\n", (unsigned long long)t, (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "isprint") { uint64_t f = 0, t = 0; run_isprint(f, t); printf("isprint: %llu valores, %llu diferencias\n", (unsigned long long)t, (unsigned long long)f); if (f) rc = 1; }
    printf(rc ? "RESULTADO: HAY DIFERENCIAS\n" : "RESULTADO: SIN DIFERENCIAS\n");
    return rc;
}
