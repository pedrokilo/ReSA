// Banco CRT4A: pares EXE original vs C++ reconstruido para el motor de formato (_output 0x827F4B) y toda la pila de coma flotante a texto
// (_cfltcvt, _cftoe/_cftof/_cftog, _fltout2, _fptostr, $I10_OUTPUT, _multtenpow12, _mulld12, ...). Mismo metodo que FnCRT1A/FnCRT2A: el gta_sa.exe
// original (solo lectura) se mapea en 0x400000 dentro de este proceso de 32 bits y se llama por su direccion; el juego no se ejecuta.
// Uso: bench_crt4a.exe <ruta del EXE> [--casos N] [--semilla S] [--solo ld12|i10|cvt|output|scanf]   (devuelve 0 si todo coincide)
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
#include "CrtString.cpp"
#include "CrtHeap.cpp"
#include "CrtState.cpp"
#include "CrtFloat.cpp"
#include "CrtFormat.cpp"
#include "CrtCtype.cpp"
#include "CrtInput.cpp"
#include "CrtFloatIn.cpp"

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

static void fill_iat() {
    *(void**)0x858094 = (void*)&GetLastError; *(void**)0x85809c = (void*)&SetLastError;
    *(void**)0x8581E0 = (void*)&TlsGetValue;  *(void**)0x8581E4 = (void*)&TlsSetValue; *(void**)0x8581E8 = (void*)&GetCurrentThreadId;
    *(void**)0x8581F4 = (void*)&HeapReAlloc;  *(void**)0x8581F8 = (void*)&HeapAlloc;   *(void**)0x8581FC = (void*)&HeapFree;
    *(int*)0xC9C2F8 = 1; *(HANDLE*)0xC9C2F4 = GetProcessHeap();
    *(DWORD*)0x8E3110 = TlsAlloc();
    // tabla de punteros de coma flotante que inicializa 0x82134F en el arranque del EXE
    *(uint32_t*)0x8E30F0 = 0x826E61; *(uint32_t*)0x8E30F4 = 0x826ADB; *(uint32_t*)0x8E30F8 = 0x826B40; *(uint32_t*)0x8E30FC = 0x826A9F; *(uint32_t*)0x8E3100 = 0x826B26; *(uint32_t*)0x8E3104 = 0x826E61;
    // bloque de locale "C" de un byte para isdigit/tolower (+0x14 = 0, +0x24 = 0, +0x28 = 1, +0x48 = tabla de tipos del EXE)
    static uint8_t loc[0x100]; memset(loc, 0, sizeof loc);
    *(int*)(loc + 0x28) = 1; *(uint32_t*)(loc + 0x48) = *(uint32_t*)0x8E3780;
    *(void**)0x8E2F14 = loc;
    ExeCrt::Getptd()->locInfo = loc;
}

static std::mt19937_64 rng;
static uint32_t rnd(uint32_t n) { return n ? (uint32_t)(rng() % n) : 0; }
static uint64_t dbits(double x) { uint64_t b; memcpy(&b, &x, 8); return b; }
static double bitsd(uint64_t b) { double x; memcpy(&x, &b, 8); return x; }

static double rand_double() {
    switch (rnd(12)) {
    case 0: return bitsd(rng());
    case 1: { const double v[] = {0.0, -0.0, 1.0, -1.0, 0.5, 1.5, 2.5, 0.1, 0.05, 0.25, 9.5, 99.5, 999.9999, 99999.99999, 1e10, 1e15, 1e16, 1e17, 1e22, 1e23, 1e100, 1e300, 1e-5, 1e-300, 4.9e-324, 2.2250738585072014e-308, 1.7976931348623157e308, 123456789.123456789, 0.000123456789, 1e21, 1e-7, 5e-5, 0.99999999999999989, 9.9999999999999995e22};
             return v[rnd(sizeof v / sizeof v[0])] * (rnd(2) ? 1 : -1); }
    case 2: return bitsd(0x7FF0000000000000ull | (rnd(2) ? 0x8000000000000000ull : 0) | (rnd(4) ? (rng() & 0xFFFFFFFFFFFFFull) : 0)); // inf / NaN
    case 3: return bitsd((rng() & 0x800FFFFFFFFFFFFFull)); // denormales
    case 4: return (double)(int32_t)rng() / 1000.0;
    case 5: return (double)(int32_t)rng() / 65536.0;
    case 6: return (double)(float)((int32_t)rng() / 65536.0);
    case 7: { double p = 1; int e = (int)rnd(40) - 20; for (int i = 0; i < abs(e); i++) p *= 10; double m = (double)(rnd(2000)) / 2.0; return e < 0 ? m / p : m * p; }
    case 8: return (double)(int64_t)rng() * (double)rnd(1000) / 7.0;
    case 9: return bitsd(((uint64_t)(0x3FF - 30 + rnd(60)) << 52) | (rng() & 0xFFFFFFFFFFFFFull) | ((rng() & 1) << 63));
    case 10: { double v = (double)rnd(100000) / 1000.0; return v + (rnd(2) ? 0.0005 : -0.0005); } // casos de redondeo ".5"
    default: return bitsd(((uint64_t)(rnd(2047)) << 52) | (rng() & 0xFFFFFFFFFFFFFull) | ((rng() & 1) << 63));
    }
}

using namespace ExeCrt;

// ---------------------------------------------------------------- ld12
static bool run_ld12(uint64_t n, uint64_t& fails, uint64_t& total) {
    typedef void(__cdecl* Fn1)(Ld12*); typedef void(__cdecl* Fn2)(Ld12*, const Ld12*); typedef void(__cdecl* Fn3)(Ld12*, int, int);
    typedef void(__cdecl* DtFn)(Ld10*, const double*); typedef uint32_t(__cdecl* AddlFn)(uint32_t, uint32_t, uint32_t*);
    auto randld = [&]() { Ld12 x; x.d[0] = (uint32_t)rng(); x.d[1] = (uint32_t)rng(); x.d[2] = (uint32_t)rng();
        uint16_t* w = (uint16_t*)&x;
        switch (rnd(6)) { case 0: w[5] = (uint16_t)(rnd(2) ? 0x8000 : 0) | (uint16_t)(0x3FFF - 40 + rnd(80)); w[4] |= 0x8000; break;
            case 1: w[5] = (uint16_t)(rnd(2) ? 0x8000 : 0) | (uint16_t)rnd(0x8000); break;
            case 2: w[5] = (uint16_t)(rnd(2) ? 0x8000 : 0) | (uint16_t)(rnd(2) ? 0x7FFF : rnd(3)); break;
            case 3: w[5] = (uint16_t)(0x3FFF - 8000 + rnd(16000)); w[4] |= 0x8000; break;
            case 4: w[5] = (uint16_t)(0x3FFF + rnd(20) - 10); w[4] |= 0x8000; w[0] = rnd(2) ? 0x8000 : (uint16_t)rng(); break;
            default: w[5] = (uint16_t)(0x4000 - 2 + rnd(5)); break; }
        return x; };
    for (uint64_t c = 0; c < n; c++) {
        total++;
        const Ld12 a = randld(), b = randld();
        { Ld12 e = a, o = a; ((Fn2)0x835DD0)(&e, &b); Add12(&o, &b); if (memcmp(&e, &o, 12)) { if (fails++ < 10) printf("DIFERENCIA add12\n"); } }
        { Ld12 e = a, o = a; ((Fn1)0x835E2E)(&e); Shl12(&o); if (memcmp(&e, &o, 12)) { if (fails++ < 10) printf("DIFERENCIA shl12\n"); } }
        { Ld12 e = a, o = a; ((Fn1)0x835E5C)(&e); Shr12(&o); if (memcmp(&e, &o, 12)) { if (fails++ < 10) printf("DIFERENCIA shr12\n"); } }
        { uint32_t x = (uint32_t)rng(), y = rnd(3) ? (uint32_t)rng() : 0xFFFFFFFFu, re = 0, ro = 0;
          const uint32_t ce = ((AddlFn)0x835DAF)(x, y, &re), co = Addl(x, y, &ro); if (ce != co || re != ro) { if (fails++ < 10) printf("DIFERENCIA addl\n"); } }
        { Ld12 e = a, o = a; ((Fn2)0x8363D3)(&e, &b); Mulld12(&o, &b);
          if (memcmp(&e, &o, 12)) { if (fails++ < 10) { printf("DIFERENCIA mulld12 a=%08x%08x%08x b=%08x%08x%08x\n  exe=%08x%08x%08x\n  own=%08x%08x%08x\n", a.d[2], a.d[1], a.d[0], b.d[2], b.d[1], b.d[0], e.d[2], e.d[1], e.d[0], o.d[2], o.d[1], o.d[0]); } } }
        { Ld12 e = a, o = a; const int power = (int)rnd(10001) - 5000, flag = rnd(2); ((Fn3)0x83660B)(&e, power, flag); Multtenpow12(&o, power, flag);
          if (memcmp(&e, &o, 12)) { if (fails++ < 10) printf("DIFERENCIA multtenpow12 power=%d flag=%d\n", power, flag); } }
        { const double v = rand_double(); Ld10 e = {0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC}, o = e; ((DtFn)0x832D62)(&e, &v); Dtold(&o, &v);
          if (e.lo != o.lo || e.hi != o.hi || (e.se & 0xFFFF) != (o.se & 0xFFFF)) { if (fails++ < 10) printf("DIFERENCIA dtold %016llx\n", (unsigned long long)dbits(v)); } }
    }
    return true;
}

// ---------------------------------------------------------------- $I10_OUTPUT / _fltout2 / _fptostr
static std::string i10_str(const I10Out& o) { char b[64]; snprintf(b, sizeof b, "%d|%d|%d|%.*s", (int)o.decpt, (int)(unsigned char)o.sign, (int)o.ndigits, 24, o.digits); return b; }
static void run_i10(uint64_t n, uint64_t& fails, uint64_t& total) {
    typedef unsigned(__cdecl* I10Fn)(Ld10, int, unsigned, I10Out*);
    typedef Strflt*(__cdecl* FoFn)(double, Strflt*, char*);
    typedef void(__cdecl* FpFn)(char*, int, Strflt*);
    for (uint64_t c = 0; c < n; c++) {
        total++;
        const double v = rand_double();
        Ld10 ld; Dtold(&ld, &v);
        if (rnd(4) == 0) { ld.lo = (uint32_t)rng(); ld.hi = (uint32_t)rng() | (rnd(3) ? 0x80000000u : 0); ld.se = (uint32_t)(rnd(2) ? 0x8000 : 0) | rnd(0x8000); if (rnd(3) == 0) ld.se |= 0x7FFF; }
        const int nd = rnd(3) ? 17 : (int)rnd(25), fl = rnd(3) ? 0 : 1;
        I10Out e, o; memset(&e, 0x55, sizeof e); memset(&o, 0x55, sizeof o);
        const unsigned re = ((I10Fn)0x835F6D)(ld, nd, fl, &e), ro = I10Output(ld, nd, fl, &o);
        if (re != ro || i10_str(e) != i10_str(o)) { if (fails++ < 10) printf("DIFERENCIA i10 lo=%08x hi=%08x se=%04x nd=%d fl=%d\n  exe=%d %s\n  own=%d %s\n", ld.lo, ld.hi, ld.se & 0xFFFF, nd, fl, re, i10_str(e).c_str(), ro, i10_str(o).c_str()); }
        // _fltout2 + _fptostr
        Strflt fe, fo; char be[0x40], bo[0x40]; memset(be, 0x77, sizeof be); memset(bo, 0x77, sizeof bo);
        ((FoFn)0x832E1C)(v, &fe, be); Fltout2(v, &fo, bo);
        if (fe.sign != fo.sign || fe.decpt != fo.decpt || fe.flag != fo.flag || strcmp(be, bo)) { if (fails++ < 10) printf("DIFERENCIA fltout2 %016llx\n", (unsigned long long)dbits(v)); continue; }
        const int digits = (int)rnd(30) - 3;
        char oe[0x80], oo[0x80]; memset(oe, 0x66, sizeof oe); memset(oo, 0x66, sizeof oo);
        Strflt fe2 = fe, fo2 = fo; fe2.mantissa = be; fo2.mantissa = bo;
        ((FpFn)0x832CEB)(oe, digits, &fe2); Fptostr(oo, digits, &fo2);
        if (fe2.decpt != fo2.decpt || memcmp(oe, oo, 0x80)) { if (fails++ < 10) printf("DIFERENCIA fptostr %016llx digits=%d\n  exe='%s'\n  own='%s'\n", (unsigned long long)dbits(v), digits, oe, oo); }
    }
}

// ---------------------------------------------------------------- _cfltcvt y compañia
static void run_cvt(uint64_t n, uint64_t& fails, uint64_t& total) {
    typedef char*(__cdecl* CvtFn)(const double*, char*, int, int, int);
    typedef void(__cdecl* Fn1)(char*);
    const char fmts[] = {'e', 'E', 'f', 'g', 'G'};
    for (uint64_t c = 0; c < n; c++) {
        total++;
        const double v = rand_double();
        const int fmt = fmts[rnd(5)], caps = (fmt == 'E' || fmt == 'G') ? 1 : 0;
        const int nd = rnd(3) ? (int)rnd(20) : (int)rnd(200);
        char be[0x1000], bo[0x1000]; memset(be, 0x41, sizeof be); memset(bo, 0x41, sizeof bo);
        ((CvtFn)0x826E61)(&v, be, fmt, nd, caps); Cfltcvt(&v, bo, fmt, nd, caps);
        if (memcmp(be, bo, sizeof be)) { if (fails++ < 10) printf("DIFERENCIA cfltcvt %016llx fmt=%c nd=%d\n  exe='%.60s'\n  own='%.60s'\n", (unsigned long long)dbits(v), fmt, nd, be, bo); }
        // _forcdecpt / _cropzeros sobre el resultado
        char fe[0x1000], fo[0x1000]; memcpy(fe, be, sizeof fe); memcpy(fo, be, sizeof fo);
        if (rnd(2)) { ((Fn1)0x826A9F)(fe); Forcdecpt(fo); } else { ((Fn1)0x826ADB)(fe); Cropzeros(fo); }
        if (memcmp(fe, fo, sizeof fe)) { if (fails++ < 10) printf("DIFERENCIA forcdecpt/cropzeros sobre '%.60s'\n", be); }
    }
}

// ---------------------------------------------------------------- _output
struct ArgBuf { std::vector<uint8_t> b; void i32(uint32_t v) { b.insert(b.end(), (uint8_t*)&v, (uint8_t*)&v + 4); } void i64(uint64_t v) { b.insert(b.end(), (uint8_t*)&v, (uint8_t*)&v + 8); } void dbl(double v) { i64(dbits(v)); } };

static int call_output(int which, ExeCrt::File* f, const char* fmt, va_list va) {
    typedef int(__cdecl* OutFn)(ExeCrt::File*, const char*, va_list);
    __try { return which == 0 ? ((OutFn)0x827F4B)(f, fmt, va) : ExeCrt::Output(f, fmt, va); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return -999; }
}

static uint64_t g_crash = 0, g_chars = 0, g_floats = 0;
static void run_output(uint64_t n, uint64_t& fails, uint64_t& total) {
    typedef int(__cdecl* OutFn)(ExeCrt::File*, const char*, va_list);
    static const char* strs[] = {"", "a", "hola mundo", "0123456789abcdefghijklmnopqrstuvwxyz", "%d %s", "\x80\x81 byte alto"};
    static const wchar_t* wstrs[] = {L"", L"w", L"wide text", L"0123456789"};
    for (uint64_t c = 0; c < n; c++) {
        total++;
        std::string fmt; ArgBuf args; int nconv = 1 + rnd(3);
        int nTarget[2] = {-7, -7}; short sTarget[2] = {-7, -7}; (void)sTarget; (void)nTarget;
        struct { short len, max; char* buf; } ansi = {5, 6, (char*)"ansi"};
        std::vector<int> nPos;
        for (int k = 0; k < nconv; k++) {
            if (rnd(2)) fmt += "tx";
            if (rnd(40) == 0) { fmt += "%Ix"; args.i32((uint32_t)rng() & 0xFFFF); } // 'I' seguido de un tipo: se ignora y se imprime en hexadecimal
            if (rnd(60) == 0) { fmt += "%I?"; } // 'I' sin tipo valido detras: se imprime como caracter normal
            fmt += '%';
            if (rnd(2)) { const char flagsC[] = " #+-0"; int nf = rnd(4); for (int i = 0; i < nf; i++) fmt += flagsC[rnd(5)]; }
            if (rnd(2)) { if (rnd(5) == 0) { fmt += '*'; args.i32((uint32_t)((int)rnd(40) - 10)); } else fmt += std::to_string(rnd(rnd(8) == 0 ? 400 : 30)); }
            if (rnd(2)) { fmt += '.'; if (rnd(5) == 0) { fmt += '*'; args.i32((uint32_t)((int)rnd(40) - 10)); } else if (rnd(2)) fmt += std::to_string(rnd(rnd(8) == 0 ? 600 : 25)); }
            const int sizeK = rnd(8);
            const char* typesAll = "diuoxXpcsefgEGnZCS";
            char t = typesAll[rnd(18)];
            const bool intT = (t == 'd' || t == 'i' || t == 'u' || t == 'o' || t == 'x' || t == 'X');
            bool i64 = false;
            if (intT) { if (sizeK == 0) fmt += 'h'; else if (sizeK == 1) fmt += 'l'; else if (sizeK == 2) { fmt += "I64"; i64 = true; } else if (sizeK == 3) fmt += "I32"; else if (sizeK == 4) fmt += 'w'; }
            else if (t == 's' || t == 'c') { if (sizeK == 0) fmt += 'h'; else if (sizeK == 1) fmt += 'l'; else if (sizeK == 2) fmt += 'w'; }
            else if (t == 'S' || t == 'C') { if (sizeK == 0) fmt += 'h'; else if (sizeK == 1) fmt += 'l'; }
            else if (t == 'Z') { if (sizeK == 0) fmt += 'w'; }
            else if (t == 'n') { if (sizeK == 0) fmt += 'h'; }
            else if (t == 'e' || t == 'f' || t == 'g' || t == 'E' || t == 'G') { if (sizeK == 0) fmt += 'l'; }
            fmt += t;
            const bool wideSz = fmt.size() >= 2 && (fmt[fmt.size() - 2] == 'l' || fmt[fmt.size() - 2] == 'w');
            if (intT) { if (i64) args.i64(rnd(3) ? rng() : (uint64_t)(int64_t)(int)rnd(2000) - 1000); else args.i32(rnd(3) ? (uint32_t)rng() : (uint32_t)((int)rnd(2000) - 1000)); }
            else if (t == 'p') args.i32((uint32_t)rng());
            else if (t == 'c') args.i32(rnd(2) ? 32 + rnd(95) : (uint32_t)rng());
            else if (t == 'C') args.i32(rnd(2) ? 32 + rnd(95) : rnd(256));
            else if (t == 's') args.i32(rnd(10) == 0 ? 0 : (wideSz ? (uint32_t)(uintptr_t)wstrs[rnd(4)] : (uint32_t)(uintptr_t)strs[rnd(6)]));
            else if (t == 'S') args.i32(rnd(10) == 0 ? 0 : (uint32_t)(uintptr_t)((fmt.size() >= 2 && fmt[fmt.size() - 2] == 'h') ? (const void*)strs[rnd(6)] : (const void*)wstrs[rnd(4)]));
            else if (t == 'Z') args.i32(rnd(5) == 0 ? 0 : (uint32_t)(uintptr_t)&ansi);
            else if (t == 'n') { args.i32(0); nPos.push_back((int)(args.b.size() - 4)); }
            else args.dbl(rand_double());
        }
        if (rnd(3) == 0) fmt += "%%end";
        // dos ejecuciones sobre buffers y destinos de %n distintos
        char buf[2][0x4000]; int outN[2][4][1]; short outS[2];
        int ret[2]; ExeCrt::File ff[2]; int nIdx[2][4];
        for (int w = 0; w < 2; w++) {
            memset(buf[w], 0x5A, sizeof buf[w]);
            ff[w].ptr = buf[w]; ff[w].cnt = 0x7FFFFFFF; ff[w].base = buf[w]; ff[w].flag = 0x42; ff[w].file = 0; ff[w].charbuf = 0; ff[w].bufsiz = 0; ff[w].tmpfname = nullptr;
            ArgBuf a = args;
            for (size_t i = 0; i < nPos.size(); i++) { uint32_t p = (uint32_t)(uintptr_t)(rnd(2) ? (void*)&outN[w][i][0] : (void*)&outS[w]); memcpy(&a.b[nPos[i]], &p, 4); }
            // el destino se decide igual para las dos: usar un indicador determinista segun el formato
            for (size_t i = 0; i < nPos.size(); i++) { void* tgt = (fmt.find("hn") != std::string::npos) ? (void*)&outS[w] : (void*)&outN[w][i][0]; memcpy(&a.b[nPos[i]], &tgt, 4); }
            for (int q = 0; q < 4; q++) outN[w][q][0] = -3; outS[w] = -3; (void)nIdx;
            std::vector<uint8_t> av = a.b; av.resize(av.size() + 16, 0);
            va_list va = (va_list)av.data();
            ret[w] = call_output(w, &ff[w], fmt.c_str(), va);
        }
        if (ret[0] == -999) g_crash++; else if (ret[0] > 0) g_chars += ret[0];
        if (fmt.find_first_of("efgEG") != std::string::npos) g_floats++;
        bool same = ret[0] == ret[1] && ff[0].cnt == ff[1].cnt && (ff[0].ptr - buf[0]) == (ff[1].ptr - buf[1]) && !memcmp(buf[0], buf[1], sizeof buf[0]) && !memcmp(outN[0], outN[1], sizeof outN[0]) && outS[0] == outS[1];
        if (!same) { if (fails++ < 10) printf("DIFERENCIA output fmt='%s' ret exe=%d own=%d\n  exe='%.100s'\n  own='%.100s'\n", fmt.c_str(), ret[0], ret[1], buf[0], buf[1]); }
    }
}

// ---------------------------------------------------------------- _input
#include "bench_input.inc"
#include "bench_floatin.inc"

int main(int argc, char** argv) {
    if (argc < 2) { printf("uso: bench_crt4a <gta_sa.exe> [--casos N] [--semilla S] [--solo ld12|i10|cvt|output|scanf]\n"); return 2; }
    uint64_t n = 100000, seed = 4242; std::string solo;
    for (int i = 2; i < argc; i++) { if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--solo") && i + 1 < argc) solo = argv[++i]; }
    if (!map_exe(argv[1])) return 2;
    fill_iat();
    for (int i = 2; i + 2 < argc; i++) if (!strcmp(argv[i], "--input")) { one_input(argv[i + 1], argv[i + 2]); return 0; }
    rng.seed(seed); int rc = 0;
    if (solo.empty() || solo == "ld12") { uint64_t f = 0, t = 0; run_ld12(n, f, t); printf("ld12 (add/shl/shr/addl/mul/multtenpow/dtold): %llu casos, %llu diferencias\n", (unsigned long long)t, (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "i10") { uint64_t f = 0, t = 0; run_i10(n, f, t); printf("i10/fltout2/fptostr: %llu casos, %llu diferencias\n", (unsigned long long)t, (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "cvt") { uint64_t f = 0, t = 0; run_cvt(n, f, t); printf("cfltcvt/cftoe/cftof/cftog/forcdecpt/cropzeros: %llu casos, %llu diferencias\n", (unsigned long long)t, (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "output") { uint64_t f = 0, t = 0; run_output(n, f, t); printf("_output: %llu formatos (%llu con coma flotante, %llu caracteres escritos, %llu fallos SEH en ambos), %llu diferencias\n", (unsigned long long)t, (unsigned long long)g_floats, (unsigned long long)g_chars, (unsigned long long)g_crash, (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "scanf") { uint64_t f = 0, t = 0; run_input(n, f, t); printf("_input: %llu casos (%llu con alguna conversion), %llu diferencias\n", (unsigned long long)t, (unsigned long long)g_inOk, (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "misc") { uint64_t f = 0, t = 0; run_misc(n, f, t); printf("ctype/ungetc/getbuf: %llu casos, %llu diferencias\n", (unsigned long long)t, (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "floatin") { uint64_t f = 0, t = 0; run_floatin(n, f, t); printf("texto -> coma flotante (strgtold12/ld12cvt/mtold12/atodbl/atoflt/fassign): %llu casos, %llu diferencias\n", (unsigned long long)t, (unsigned long long)f); if (f) rc = 1; }
    printf(rc ? "RESULTADO: HAY DIFERENCIAS\n" : "RESULTADO: SIN DIFERENCIAS\n");
    return rc;
}
