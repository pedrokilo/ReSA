// Banco CRT4A-locale: pares EXE original vs C++ reconstruido para el locale del CRT (CrtLocale.cpp, CrtCtype.cpp, CrtState.cpp::Isprint).
// Mismo metodo que FnCRT1A..5A: el gta_sa.exe original (solo lectura) se mapea en 0x400000 dentro de este proceso de 32 bits y se llama por su direccion;
// el juego no se ejecuta. Dos grupos:
//   refcount: __updatetlocinfo (nucleo 0x82524D), __freetlocinfo, __free_lc_time, __free_lconv_num/mon sobre estructuras de locale falsas (objetos del montón
//             del EXE con identificador); se compara el valor devuelto, las cuentas de referencias y la SECUENCIA de liberaciones (HeapFree registrado).
//   api:      _isctype, _tolower_lk, _toupper_lk, __crtLCMapStringA, __crtGetStringTypeA, __ansicp, __convertcp, _wctomb, _mbtowc, isalpha..isprint, _tolower/_toupper,
//             _atol, __stricmp, __strnicmp, _strlwr, _strupr, con bloques de locale reales construidos con la API de Windows (inglés, alemán, ruso, japonés, chino,
//             coreano, tailandés...) y con las dos variantes de la API ancha/ANSI (0xC9AF48 y 0xC9ADC8 = 1 o 2).
// Uso: bench_locale.exe <ruta del EXE> [--casos N] [--semilla S] [--solo refcount|api|ctype|str]   (devuelve 0 si todo coincide)
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include <random>
#include <algorithm>

#define CRT_BENCH
#include "CrtString.cpp"
static LPVOID WINAPI ZeroHeapAlloc(HANDLE h, DWORD f, SIZE_T n) { return HeapAlloc(h, f | HEAP_ZERO_MEMORY, n); }
static std::vector<void*> g_freed;
static BOOL WINAPI LogHeapFree(HANDLE h, DWORD f, LPVOID p) { if (p) g_freed.push_back(p); return HeapFree(h, f, p); }
#define HeapAlloc ZeroHeapAlloc
#include "CrtHeap.cpp"
#include "CrtState.cpp"
#include "CrtFloat.cpp"
#include "CrtFormat.cpp"
#include "CrtLocale.cpp"
#include "CrtCtype.cpp"
#include "CrtInput.cpp"
#include "CrtFloatIn.cpp"
#include "CrtLowIo.cpp"
#include "CrtStdio.cpp"
#include "CrtFile.cpp"
#undef HeapAlloc

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
    auto dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    for (auto* d0 = (IMAGE_IMPORT_DESCRIPTOR*)(0x400000 + dir.VirtualAddress); d0->Name; d0++) {
        if (_stricmp((const char*)(0x400000 + d0->Name), "KERNEL32.dll") != 0) continue;
        HMODULE k = GetModuleHandleA("kernel32.dll");
        auto ilt = (uint32_t*)(0x400000 + (d0->OriginalFirstThunk ? d0->OriginalFirstThunk : d0->FirstThunk)); auto iat = (void**)(0x400000 + d0->FirstThunk);
        for (int i = 0; ilt[i]; i++) if (!(ilt[i] & 0x80000000)) { void* p = (void*)GetProcAddress(k, (const char*)(0x400000 + ilt[i] + 2)); if (p) iat[i] = p; }
    }
    return true;
}

typedef int(__cdecl* IFn)();
static bool g_trace_api = false;
static bool g_lcmapZero = false;   // el EXE hizo una consulta de tama�o con LCMapStringA que devolvio 0 (devuelve un valor de pila sin inicializar)
static int WINAPI TrLCMapA(LCID l, DWORD f, LPCSTR s, int n, LPSTR d, int nd) { int r = LCMapStringA(l, f, s, n, d, nd); if (d == nullptr && nd == 0 && r == 0) g_lcmapZero = true; return r; }
static int WINAPI TrMBTWC(UINT cp, DWORD fl, LPCCH s, int n, LPWSTR w, int nw) { int r = MultiByteToWideChar(cp, fl, s, n, w, nw); if (g_trace_api) printf("  MBTWC(cp %u, fl %lu, n %d, nw %d) = %d\n", cp, fl, n, nw, r); return r; }
static int WINAPI TrWCTMB(UINT cp, DWORD fl, LPCWCH w, int nw, LPSTR s, int n, LPCCH d, LPBOOL u) { int r = WideCharToMultiByte(cp, fl, w, nw, s, n, d, u); if (g_trace_api) printf("  WCTMB(cp %u, fl %lu, nw %d, n %d, dst %p) = %d\n", cp, fl, nw, n, (void*)s, r); return r; }
static BOOL WINAPI TrCPInfo(UINT cp, LPCPINFO i) { BOOL r = GetCPInfo(cp, i); if (g_trace_api) printf("  GetCPInfo(%u) = %d max %u\n", cp, r, i->MaxCharSize); return r; }
static void init_crt_state() {
    *(void**)0x8581F8 = (void*)&ZeroHeapAlloc;                    // HeapAlloc del EXE tambien a cero
    *(void**)0x8581FC = (void*)&LogHeapFree;                      // HeapFree del EXE con registro
    *(int*)0xC9C2F8 = 1; *(HANDLE*)0xC9C2F4 = GetProcessHeap();   // montón del CRT = montón del proceso (__active_heap 1)
    *(DWORD*)0x8E3110 = TlsAlloc();
    *(void**)0x8581C8 = (void*)&TrLCMapA;                          // LCMapStringA del EXE con deteccion del camino indefinido
    *(int*)0xC9AC08 = 2;                                          // _osplatform
    ((IFn)0x82AC74)();                                            // _mtinitlocks: cerrojos estaticos
}

static std::mt19937_64 rng;
static uint32_t rnd(uint32_t n) { return n ? (uint32_t)(rng() % n) : 0; }
static uint32_t pick(std::initializer_list<uint32_t> l) { return *(l.begin() + rnd((uint32_t)l.size())); }

using ExeCrt::LocInfo;
static void* exe_malloc(size_t n) { return ((void*(__cdecl*)(unsigned))0x824257)((unsigned)n); }

// ---------------------------------------------------------------------------------------------------------------- grupo refcount
// Escenario de estructuras de locale falsas (todas con malloc del EXE, con identificador para normalizar las direcciones)
struct Sc {
    std::vector<void*> ptrs; std::map<void*, int> ids; std::vector<std::pair<void*, int*>> counters;
    void* mk(size_t n) { void* p = exe_malloc(n); ids[p] = (int)ptrs.size(); ptrs.push_back(p); return p; }
    int id(void* p) const { if (!p) return -2; auto it = ids.find(p); return it == ids.end() ? -1 : it->second; }
    int* counter(int v) { int* c = (int*)mk(4); *c = v; counters.push_back({c, c}); return c; }
};
struct Snap { std::vector<uint8_t> a, b; };
static const uintptr_t STATIC_LOCINFO = 0x8E2EC0;
static std::vector<uint8_t> save(uintptr_t a, size_t n) { return std::vector<uint8_t>((uint8_t*)a, (uint8_t*)a + n); }
static void restore(uintptr_t a, const std::vector<uint8_t>& v) { memcpy((void*)a, v.data(), v.size()); }

struct Pool { std::vector<int*> intl, mon, num, ctype; std::vector<void*> times, lconvs, strs; };

static void fill_lconv(Sc& s, Pool& pl, char** f) { for (int i = 0; i < 10; i++) f[i] = (char*)s.mk(4); }

// Construye el mundo: devuelve (A, B) y fija los globales que leen las funciones
struct World { LocInfo* A; LocInfo* B; void* lconvA; };
static LocInfo* mk_locinfo(Sc& s, Pool& pl, int refc) {
    LocInfo* li = (LocInfo*)s.mk(sizeof(LocInfo));
    li->refCount = refc;
    auto pickc = [&](std::vector<int*>& v) -> int* { return rnd(5) == 0 ? nullptr : v[rnd((uint32_t)v.size())]; };
    li->lconvIntlRef = pl.intl[rnd((uint32_t)pl.intl.size())]; li->lconvMonRef = pickc(pl.mon); li->lconvNumRef = pickc(pl.num); li->ctype1Ref = pickc(pl.ctype);
    li->lconv = rnd(6) == 0 ? nullptr : pl.lconvs[rnd((uint32_t)pl.lconvs.size())];
    li->ctype1 = (unsigned short*)s.mk(8);
    li->timeCur = pl.times[rnd((uint32_t)pl.times.size())];
    li->timeOwn = rnd(4) == 0 ? nullptr : pl.times[rnd((uint32_t)pl.times.size())];
    return li;
}
static void build_pool(Sc& s, Pool& pl, uint32_t k) {
    for (uint32_t i = 0; i < k; i++) {
        pl.intl.push_back(s.counter(pick({0, 0, 1, 2}))); pl.mon.push_back(s.counter(pick({0, 0, 1, 2})));
        pl.num.push_back(s.counter(pick({0, 0, 1, 2}))); pl.ctype.push_back(s.counter(pick({0, 0, 1, 2})));
        void* t = s.mk(0xB8); *(int*)((char*)t + 0xB4) = pick({0, 0, 1, 2}); for (int j = 0; j < 43; j++) *(void**)((char*)t + j * 4) = s.mk(4);
        pl.times.push_back(t); s.counters.push_back({t, (int*)((char*)t + 0xB4)});
        void* lc = s.mk(0x30); fill_lconv(s, pl, (char**)lc); pl.lconvs.push_back(lc);
    }
}
// Globales: tablas por defecto (strings y refcounts por defecto)
static void set_defaults(Sc& s, Pool& pl, bool tie) {
    auto cur = (void**)0x8E37BC; *cur = pl.lconvs[0];
    for (int i = 0; i < 3; i++) *(void**)(0x8E378C + i * 4) = tie && rnd(2) ? ((char**)pl.lconvs[rnd((uint32_t)pl.lconvs.size())])[i] : s.mk(4);
    for (int i = 0; i < 7; i++) *(void**)(0x8E3798 + i * 4) = tie && rnd(2) ? ((char**)pl.lconvs[rnd((uint32_t)pl.lconvs.size())])[3 + i] : s.mk(4);
    *(void**)0xC9AF00 = rnd(2) ? pl.lconvs[rnd((uint32_t)pl.lconvs.size())] : s.mk(0x30);
    *(void**)0xC9C2C8 = rnd(2) ? pl.mon[rnd((uint32_t)pl.mon.size())] : s.mk(4);
    *(void**)0xC9C2D0 = rnd(2) ? pl.num[rnd((uint32_t)pl.num.size())] : s.mk(4);
    *(void**)0xC9C2C4 = rnd(2) ? pl.ctype[rnd((uint32_t)pl.ctype.size())] : s.mk(4);
    *(void**)0xC9AEFC = rnd(2) ? pl.times[rnd((uint32_t)pl.times.size())] : s.mk(4);
}
static void mutate_lconv_to_share(Sc& s, Pool& pl) {
    // algunos campos de un lconv coinciden con los del actual o con los por defecto (ramas "no se libera")
    char** cur = (char**)*(void**)0x8E37BC;
    for (void* l : pl.lconvs) for (int i = 0; i < 10; i++) {
        uint32_t r = rnd(6);
        if (r == 0) ((char**)l)[i] = cur[i];
        else if (r == 1) ((char**)l)[i] = *(char**)(i < 3 ? 0x8E378C + i * 4 : 0x8E3798 + (i - 3) * 4);
    }
}

enum Kind { K_UPDATE, K_FREEINFO, K_FREETIME, K_FREENUM, K_FREEMON, K_UPDATE_NOLOCK };
static std::string run_refcount(Kind kind, uint64_t seed, bool exe) {
    rng.seed(seed);
    Sc s; Pool pl; build_pool(s, pl, 1 + rnd(3));
    auto st1 = save(STATIC_LOCINFO, 0x54), st2 = save(0x8E36C8, 0xB8);
    set_defaults(s, pl, true); mutate_lconv_to_share(s, pl);
    LocInfo* A = mk_locinfo(s, pl, pick({0, 1, 1, 2})); LocInfo* B = mk_locinfo(s, pl, pick({0, 1, 2}));
    void* oldGlobal = *(void**)0x8E2F14; ExeCrt::ThreadData* ptd = ExeCrt::Getptd(); void* oldPtd = ptd->locInfo;
    *(void**)0x8E2F14 = B;
    switch (rnd(4)) { case 0: ptd->locInfo = A; break; case 1: ptd->locInfo = B; break; case 2: ptd->locInfo = nullptr; break; default: ptd->locInfo = (void*)STATIC_LOCINFO; }
    if ((uintptr_t)ptd->locInfo == STATIC_LOCINFO) { // el bloque estatico del EXE: refcount 1, partes compartidas del escenario
        LocInfo* si = (LocInfo*)STATIC_LOCINFO; si->refCount = pick({1, 1, 2}); si->lconvIntlRef = pl.intl[0]; si->lconvMonRef = pl.mon[0]; si->lconvNumRef = pl.num[0];
        si->ctype1Ref = pl.ctype[0]; si->timeCur = pl.times[0]; si->timeOwn = nullptr; si->lconv = nullptr;
    }
    g_freed.clear();
    void* ret = nullptr; void* loc0 = ptd->locInfo;
    uintptr_t arg = 0;
    switch (kind) {
    case K_UPDATE: ret = exe ? ((void*(__cdecl*)())0x825456)() : (void*)ExeCrt::UpdateTLocInfo(); break;
    case K_UPDATE_NOLOCK: ret = exe ? ((void*(__cdecl*)())0x82524D)() : (void*)ExeCrt::UpdateTLocInfoNoLock(); break;
    case K_FREEINFO: arg = (uintptr_t)A; if (exe) ((void(__cdecl*)(void*))0x825183)(A); else ExeCrt::FreeTLocInfo(A); break;
    case K_FREETIME: arg = 1; { void* t = pl.times[0]; if (rnd(8) == 0) t = nullptr; if (exe) ((void(__cdecl*)(void*))0x82FBE7)(t); else ExeCrt::FreeLcTime(t); } break;
    case K_FREENUM: { void* l = pl.lconvs[rnd((uint32_t)pl.lconvs.size())]; if (rnd(10) == 0) l = nullptr; if (exe) ((void(__cdecl*)(void*))0x82FE09)(l); else ExeCrt::FreeLconvNum(l); } break;
    case K_FREEMON: { void* l = pl.lconvs[rnd((uint32_t)pl.lconvs.size())]; if (rnd(10) == 0) l = nullptr; if (exe) ((void(__cdecl*)(void*))0x830068)(l); else ExeCrt::FreeLconvMon(l); } break;
    }
    (void)loc0; (void)arg;
    std::string sig = "ret=" + std::to_string(s.id(ret)) + (ret == (void*)STATIC_LOCINFO ? "S" : "") + " ptd=" + std::to_string(s.id(ptd->locInfo)) + ((uintptr_t)ptd->locInfo == STATIC_LOCINFO ? "S" : "") + " frees=";
    for (void* p : g_freed) sig += std::to_string(s.id(p)) + ",";
    auto wasFreed = [&](void* q) { return std::find(g_freed.begin(), g_freed.end(), q) != g_freed.end(); };
    sig += " cnt=";
    for (auto& c : s.counters) sig += (wasFreed(c.first) ? std::string("F") : std::to_string(*c.second)) + ",";
    sig += " A=" + (wasFreed(A) ? std::string("F") : std::to_string(A->refCount)) + " B=" + (wasFreed(B) ? std::string("F") : std::to_string(B->refCount));
    LocInfo* si = (LocInfo*)STATIC_LOCINFO; sig += " S=" + std::to_string(si->refCount);
    // limpieza: lo que quede vivo se libera sin registrar
    *(void**)0x8E2F14 = oldGlobal; ptd->locInfo = oldPtd; restore(STATIC_LOCINFO, st1); restore(0x8E36C8, st2);
    for (void* p : s.ptrs) { if (std::find(g_freed.begin(), g_freed.end(), p) == g_freed.end()) HeapFree(GetProcessHeap(), 0, p); }
    g_freed.clear();
    return sig;
}

// ---------------------------------------------------------------------------------------------------------------- grupo api
struct Loc { const char* name; unsigned lcid, cp; };
static const Loc kLocs[] = { {"en-US", 0x409, 1252}, {"de-DE", 0x407, 1252}, {"ru-RU", 0x419, 1251}, {"ja-JP", 0x411, 932}, {"zh-CN", 0x804, 936}, {"ko-KR", 0x412, 949}, {"zh-TW", 0x404, 950}, {"th-TH", 0x41E, 874}, {"el-GR", 0x408, 1253}, {"tr-TR", 0x41F, 1254} };
struct FakeLoc { LocInfo li; std::vector<unsigned short> table; };
static FakeLoc* make_loc(const Loc* l) {
    FakeLoc* f = new FakeLoc(); memset(&f->li, 0, sizeof f->li); f->table.assign(0x101 + 0x100, 0);
    f->li.refCount = 1;
    if (l) {
        f->li.codePage = l->cp; f->li.collateCodePage = l->cp; for (int i = 0; i < 6; i++) f->li.handle[i] = l->lcid;
        CPINFO ci; GetCPInfo(l->cp, &ci); f->li.mbCurMax = ci.MaxCharSize; f->li.clike = (l->lcid == 0x409) ? 1 : 0;
        for (int c = 0; c < 256; c++) { char b = (char)c; WORD t = 0; GetStringTypeA(l->lcid, CT_CTYPE1, &b, 1, &t); f->table[1 + c] = t; }
        for (int i = 0; i < MAX_LEADBYTES && ci.LeadByte[i]; i += 2) for (int c = ci.LeadByte[i]; c <= ci.LeadByte[i + 1]; c++) f->table[1 + c] |= 0x8000;
    } else { // "C": sin manejador, tabla del EXE (se rellena con la clasificacion ASCII)
        f->li.codePage = 0; f->li.mbCurMax = 1; f->li.clike = 1;
        for (int c = 0; c < 128; c++) { char b = (char)c; WORD t = 0; GetStringTypeA(0x409, CT_CTYPE1, &b, 1, &t); f->table[1 + c] = t; }
    }
    f->li.pctype = (const unsigned char*)(f->table.data() + 1);
    return f;
}
static std::string hex(const void* p, size_t n) { std::string s; char b[4]; for (size_t i = 0; i < n; i++) { sprintf(b, "%02x", ((const uint8_t*)p)[i]); s += b; } return s; }
static uint32_t rnd_char(bool wide) {
    switch (rnd(wide ? 6 : 4)) {
    case 0: return rnd(0x80); case 1: return rnd(0x100); case 2: return 0xFFFFFFFFu;
    case 3: return pick({'A', 'Z', 'a', 'z', '0', '9', ' ', 0x7F, 0x80, 0xFF, 0xE0, 0xC9, 0xDF, 0xFF});
    case 4: return rnd(0x10000); default: return (uint32_t)((rnd(0xFF) << 8) | rnd(0x100));
    }
}
static std::string rnd_str(size_t maxn, bool lead) {
    size_t n = rnd((uint32_t)maxn + 1); std::string s;
    for (size_t i = 0; i < n; i++) s += (char)(lead && rnd(4) == 0 ? pick({0x81, 0x82, 0x88, 0xA1, 0xB0, 0xE0, 0xFC, 0x40, 0x7E}) : (rnd(3) == 0 ? rnd(256) : "aAbBzZ09 -+xYÄÖäöß\xE9\xC9"[rnd(19)]));
    return s;
}
static void set_thread_loc(LocInfo* li) { *(void**)0x8E2F14 = li; ExeCrt::Getptd()->locInfo = li; }

static uint64_t g_cmp, g_diff, g_shown;
static void check(const char* what, const std::string& a, const std::string& b, const std::string& ctx) {
    g_cmp++;
    if (a != b) { g_diff++; if (g_shown++ < 25) printf("DIFERENCIA %s [%s]\n  EXE   : %s\n  propio: %s\n", what, ctx.c_str(), a.c_str(), b.c_str()); }
}

template <class FE, class FO> static void both(const char* what, const std::string& ctx, FE fe, FO fo) { std::string a = fe(), b = fo(); check(what, a, b, ctx); }

static void run_api(uint64_t cases, bool only_ctype, bool only_str) {
    std::vector<FakeLoc*> locs; locs.push_back(make_loc(nullptr)); for (auto& l : kLocs) locs.push_back(make_loc(&l));
    for (uint64_t c = 0; c < cases; c++) {
        FakeLoc* fl = locs[rnd((uint32_t)locs.size())]; LocInfo* li = &fl->li;
        size_t li_idx = std::find(locs.begin(), locs.end(), fl) - locs.begin(); const char* lname = li_idx == 0 ? "C" : kLocs[li_idx - 1].name;
        int mode = 1 + (int)rnd(2); char ctxb[96]; sprintf(ctxb, "locale %s modo %d caso %llu", lname, mode, (unsigned long long)c); const std::string ctx = ctxb;
        *(int*)0xC9AF48 = mode; *(int*)0xC9ADC8 = mode; *(unsigned*)0xC9AF40 = li->codePage ? li->codePage : 1252; *(unsigned*)0xC9AF30 = li->handle[2] ? li->handle[2] : 0x409;
        set_thread_loc(li);
        uint32_t ch = rnd_char(li->mbCurMax > 1 || rnd(3) == 0);
        if (li->mbCurMax <= 1 && !(ch == (uint32_t)-1 || ch < 0x100)) ch &= 0xFF;
        uint32_t mask = pick({1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x103, 0x107, 0x157, 0x8000, 0xFFFF});
        if (!only_str) {
            // isctype y clasificacion
            uint32_t chw = ch; // para IsctypeMt cualquier valor
            both("_isctype", ctx, [&] { return std::to_string(((int(__cdecl*)(void*, int, int))0x82AAE2)(li, (int)chw, (int)mask)); }, [&] { return std::to_string(ExeCrt::IsctypeMt(li, (int)chw, (int)mask)); });
            if (ch == (uint32_t)-1 || ch < 0x100 || li->mbCurMax > 1) {
                struct { const char* n; unsigned a; int(__cdecl* o)(int); } fns[] = {
                    {"isalpha", 0x8226D6, ExeCrt::Isalpha}, {"isupper", 0x822715, ExeCrt::Isupper}, {"islower", 0x82274F, ExeCrt::Islower}, {"isdigit", 0x822789, ExeCrt::Isdigit},
                    {"isspace", 0x822802, ExeCrt::Isspace}, {"isxdigit", 0x8227C3, ExeCrt::Isxdigit}, {"isalnum", 0x822876, ExeCrt::Isalnum}, {"isprint", 0x8228B5, ExeCrt::Isprint},
                    {"tolower", 0x825D50, ExeCrt::Tolower}, {"toupper", 0x823A0E, ExeCrt::Toupper} };
                for (auto& f : fns) both(f.n, ctx, [&] { return std::to_string(((int(__cdecl*)(int))f.a)((int)ch)); }, [&] { return std::to_string(f.o((int)ch)); });
            }
            both("_tolower_lk", ctx, [&] { return std::to_string(((int(__cdecl*)(void*, int))0x825C88)(li, (int)ch)); }, [&] { return std::to_string(ExeCrt::TolowerLk(li, (int)ch)); });
            both("_toupper_lk", ctx, [&] { return std::to_string(((int(__cdecl*)(void*, int))0x823945)(li, (int)ch)); }, [&] { return std::to_string(ExeCrt::ToupperLk(li, (int)ch)); });
        }
        if (only_ctype) continue;
        if (!only_str) {
            // __ansicp / __convertcp / __crtGetStringTypeA / __crtLCMapStringA
            unsigned lc = pick({0, 0x409, 0x407, 0x419, 0x411, 0x804, 0x412, 0x404, 0x41E, 0x408, 0x41F, 0xFFFF, 0x1234});
            both("__ansicp", ctx, [&] { return std::to_string(((int(__cdecl*)(unsigned))0x8344CA)(lc)); }, [&] { return std::to_string(ExeCrt::Ansicp(lc)); });
            std::string src = rnd_str(24, true); int cnt0 = pick({0, 1, 2, 3, 0xFFFFFFFFu, 0xFFFFFFFFu}) == 0xFFFFFFFFu ? -1 : (int)rnd((uint32_t)src.size() + 3);
            int srcCp = pick({1252, 932, 936, 949, 950, 1251, 874, 65001, 437, 850, 1253, 0xFFFF}), dstCp = pick({1252, 932, 936, 949, 950, 1251, 874, 65001, 437, 850, 1253, 1252});
            int dsize = pick({0, 4, 16, 64}); bool withDst = rnd(2);
            { std::vector<char> d1(80, 'z'), d2(80, 'z'); int n1 = cnt0, n2 = cnt0; char* r1 = nullptr; char* r2 = nullptr; size_t g0 = g_freed.size(); (void)g0;
              both("__convertcp", ctx + " src " + hex(src.c_str(), src.size() + 1) + " cp " + std::to_string(srcCp) + "->" + std::to_string(dstCp) + " cnt " + std::to_string(cnt0) + " dst " + std::to_string(withDst) + "/" + std::to_string(dsize),
                [&] { r1 = ((char*(__cdecl*)(int, int, const char*, int*, char*, int))0x834513)(srcCp, dstCp, src.c_str(), &n1, withDst ? d1.data() : nullptr, withDst ? dsize : 0);
                      std::string s = std::to_string(n1) + " " + (r1 ? (withDst ? "dst" : hex(r1, HeapSize(GetProcessHeap(), 0, r1))) : "null") + " " + hex(d1.data(), 80); if (r1 && !withDst) ((void(__cdecl*)(void*))0x82413F)(r1); return s; },
                [&] { r2 = ExeCrt::Convertcp(srcCp, dstCp, src.c_str(), &n2, withDst ? d2.data() : nullptr, withDst ? dsize : 0);
                      std::string s = std::to_string(n2) + " " + (r2 ? (withDst ? "dst" : hex(r2, HeapSize(GetProcessHeap(), 0, r2))) : "null") + " " + hex(d2.data(), 80); if (r2 && !withDst) ((void(__cdecl*)(void*))0x82413F)(r2); return s; }); }
            { unsigned short o1[64] = {}, o2[64] = {}; int cch = pick({0, 1, 2, 0xFFFFFFFFu, 0xFFFFFFFFu, 3}) == 0xFFFFFFFFu ? -1 : (int)(1 + rnd((uint32_t)src.size() + 2)); if (src.empty()) cch = rnd(2) ? -1 : 1; int it = pick({1, 2, 4}); int cp = pick({0, 1252, 932, 936, 1251, 437}); int lcid = pick({0, 0x409, 0x411, 0x419, 0x804});
              int be = rnd(2); cch = (cch > 60) ? 60 : cch; std::string s2 = src + std::string(4, 'x');
              both("__crtGetStringTypeA", ctx + " tipo " + std::to_string(it) + " cch " + std::to_string(cch) + " cp " + std::to_string(cp) + " lcid " + std::to_string(lcid),
                [&] { int r = ((int(__cdecl*)(unsigned, const char*, int, unsigned short*, int, int, int))0x830FB8)(it, s2.c_str(), cch, o1, cp, lcid, be); return std::to_string(r) + " " + hex(o1, sizeof o1); },
                [&] { int r = ExeCrt::CrtGetStringTypeA(it, s2.c_str(), cch, o2, cp, lcid, be); return std::to_string(r) + " " + hex(o2, sizeof o2); }); }
            { unsigned fl = pick({0x100, 0x200, 0x400, 0x100, 0x200, 0x10000, 0x20000, 0x8, 0x1000, 0x100 | 0x400}); unsigned loc = pick({0, 0x409, 0x407, 0x419, 0x411, 0x804, 0x412, 0x404}); int cp = pick({0, 1252, 932, 936, 949, 1251, 437, 65001});
              int bufn = pick({0, 3, 8, 24, 64}); int cchS = (int)pick({0xFFFFFFFFu, 0xFFFFFFFFu, 0, 1, 5, 40}); std::vector<char> d1(100, 'z'), d2(100, 'z'); int be = rnd(2); std::string s2 = src + std::string(8, 'q');
              if ((int)s2.size() < cchS) cchS = (int)s2.size();
              both("__crtLCMapStringA", ctx + " flags " + std::to_string(fl) + " loc " + std::to_string(loc) + " cp " + std::to_string(cp) + " cchS " + std::to_string(cchS) + " cchD " + std::to_string(bufn),
                [&] { g_lcmapZero = false; int r = ((int(__cdecl*)(unsigned, unsigned, const char*, int, char*, int, int, int))0x82BB1E)(loc, fl, s2.c_str(), cchS, bufn ? d1.data() : nullptr, bufn, cp, be); return (g_lcmapZero ? std::string("indefinido") : std::to_string(r)) + " " + hex(d1.data(), 100); },
                [&] { int r = ExeCrt::CrtLCMapStringA(loc, fl, s2.c_str(), cchS, bufn ? d2.data() : nullptr, bufn, cp, be); return (g_lcmapZero ? std::string("indefinido") : std::to_string(r)) + " " + hex(d2.data(), 100); }); }
            // _wctomb / _mbtowc (con el bloque actual del hilo)
            { char d1[8] = {}, d2[8] = {}; int wc = (int)(rnd(3) == 0 ? rnd(0x10000) : rnd(0x300)); bool nul = rnd(10) == 0; ExeCrt::Getptd(); int* en = ExeCrt::ErrnoPtr();
              both("_wctomb", ctx + " wc " + std::to_string(wc),
                [&] { *en = 0; int r = ((int(__cdecl*)(char*, int))0x83353C)(nul ? nullptr : d1, wc); return std::to_string(r) + " e" + std::to_string(*en) + " " + hex(d1, 8); },
                [&] { *en = 0; int r = ExeCrt::Wctomb(nul ? nullptr : d2, (unsigned short)wc); return std::to_string(r) + " e" + std::to_string(*en) + " " + hex(d2, 8); });
              std::string ms = rnd_str(3, true) + std::string(2, 'k'); unsigned n = pick({0, 1, 2, 3, 4}); unsigned short w1 = 0x5555, w2 = 0x5555; bool nd = rnd(8) == 0, ns = rnd(10) == 0;
              both("_mbtowc", ctx + " bytes " + hex(ms.data(), 3) + " n " + std::to_string(n),
                [&] { *en = 0; int r = ((int(__cdecl*)(void*, const char*, unsigned))0x8338CE)(nd ? nullptr : &w1, ns ? nullptr : ms.c_str(), n); return std::to_string(r) + " e" + std::to_string(*en) + " " + std::to_string(w1); },
                [&] { *en = 0; int r = ExeCrt::Mbtowc(nd ? nullptr : &w2, ns ? nullptr : ms.c_str(), n); return std::to_string(r) + " e" + std::to_string(*en) + " " + std::to_string(w2); }); }
        }
        if (only_ctype && !only_str) continue;
        // cadenas con el locale del hilo
        { std::string a = rnd_str(12, li->mbCurMax > 1), b = rnd_str(12, li->mbCurMax > 1); if (rnd(3) == 0) b = a; if (rnd(4) == 0) { for (auto& ch2 : b) if (rnd(2)) ch2 = (char)(ch2 ^ 0x20); }
          unsigned n = rnd(14);
          both("__stricmp", ctx + " '" + a + "' '" + b + "'", [&] { return std::to_string(((int(__cdecl*)(const char*, const char*))0x8229B6)(a.c_str(), b.c_str())); }, [&] { return std::to_string(ExeCrt::Stricmp(a.c_str(), b.c_str())); });
          both("__strnicmp", ctx + " n " + std::to_string(n), [&] { return std::to_string(((int(__cdecl*)(const char*, const char*, unsigned))0x835AC7)(a.c_str(), b.c_str(), n)); }, [&] { return std::to_string(ExeCrt::Strnicmp(a.c_str(), b.c_str(), n)); });
          both("__ascii_stricmp", ctx, [&] { return std::to_string(((int(__cdecl*)(const char*, const char*))0x82ABC0)(a.c_str(), b.c_str())); }, [&] { return std::to_string(ExeCrt::AsciiStricmp(a.c_str(), b.c_str())); });
          both("__ascii_strnicmp", ctx, [&] { return std::to_string(((int(__cdecl*)(const char*, const char*, unsigned))0x836990)(a.c_str(), b.c_str(), n)); }, [&] { return std::to_string(ExeCrt::AsciiStrnicmp(a.c_str(), b.c_str(), n)); });
          std::string x1 = a + std::string(40, '\0'), x2 = x1;
          both("_strlwr", ctx + " '" + a + "'", [&] { ((char*(__cdecl*)(char*))0x836C58)(&x1[0]); return hex(x1.data(), a.size() + 2); }, [&] { ExeCrt::Strlwr(&x2[0]); return hex(x2.data(), a.size() + 2); });
          x1 = a + std::string(40, '\0'); x2 = x1;
          both("_strupr", ctx + " '" + a + "'", [&] { ((char*(__cdecl*)(char*))0x836FBA)(&x1[0]); return hex(x1.data(), a.size() + 2); }, [&] { ExeCrt::Strupr(&x2[0]); return hex(x2.data(), a.size() + 2); });
          std::string num; { uint32_t k = rnd(8); for (uint32_t i = 0; i < k; i++) num += " \t\n\r\v\f"[rnd(6)]; if (rnd(2)) num += "+-"[rnd(2)]; uint32_t d = rnd(14); for (uint32_t i = 0; i < d; i++) num += (char)('0' + rnd(10)); if (rnd(3) == 0) num += rnd_str(3, false); }
          both("_atol", ctx + " '" + num + "'", [&] { return std::to_string(((int(__cdecl*)(const char*))0x822506)(num.c_str())); }, [&] { return std::to_string(ExeCrt::Atol(num.c_str())); }); }
    }
    for (auto* f : locs) delete f;
}

int main(int argc, char** argv) {
    if (argc < 2) { printf("uso: bench_locale <gta_sa.exe> [--casos N] [--semilla S] [--solo refcount|api|ctype|str]\n"); return 2; }
    uint64_t n = 20000, seed = 12345; std::string solo;
    for (int i = 2; i < argc; i++) { if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--solo") && i + 1 < argc) solo = argv[++i]; }
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (!map_exe(argv[1])) return 2;
    init_crt_state();
    int rc = 0;
    if (solo == "conv") { // depuracion: --solo conv  (convierte un texto fijo con las dos versiones)
        const char* t = "\x7e\xa4\xb0\x2d\x5a\xfc\xab\x81"; int c1 = 3, c2 = 3;
        { WCHAR w[16]; int nw = MultiByteToWideChar(950, 1, t, 3, w, 16); int nb = WideCharToMultiByte(949, 0, w, nw, nullptr, 0, nullptr, nullptr); printf("nw %d nb %d err %lu\n", nw, nb, GetLastError()); }
        *(void**)0x8580f4 = (void*)&TrMBTWC; *(void**)0x858150 = (void*)&TrWCTMB; *(void**)0x858058 = (void*)&TrCPInfo; g_trace_api = true;
        printf("EXE:\n");
        char* r1 = ((char*(__cdecl*)(int, int, const char*, int*, char*, int))0x834513)(950, 949, t, &c1, nullptr, 0);
        printf("propio:\n");
        char* r2 = ExeCrt::Convertcp(950, 949, t, &c2, nullptr, 0);
        printf("EXE %p %d  propio %p %d\n", r1, c1, r2, c2); return 0;
    }
    if (solo.empty() || solo == "refcount") {
        const char* names[] = { "__updatetlocinfo", "__updatetlocinfo_nolock", "__freetlocinfo", "__free_lc_time", "__free_lconv_num", "__free_lconv_mon" };
        Kind kinds[] = { K_UPDATE, K_UPDATE_NOLOCK, K_FREEINFO, K_FREETIME, K_FREENUM, K_FREEMON };
        for (int k = 0; k < 6; k++) {
            uint64_t bad = 0, shown = 0;
            for (uint64_t c = 0; c < n; c++) { if (getenv("BENCH_TRACE")) printf("k%d c%llu\n", k, (unsigned long long)c); std::string a = run_refcount(kinds[k], seed + c, true), b = run_refcount(kinds[k], seed + c, false);
                if (a != b) { bad++; if (shown++ < 5) printf("DIFERENCIA %s caso %llu\n  EXE   : %s\n  propio: %s\n", names[k], (unsigned long long)(seed + c), a.c_str(), b.c_str()); } }
            printf("%s: %llu escenarios, %llu diferencias\n", names[k], (unsigned long long)n, (unsigned long long)bad); if (bad) rc = 1;
        }
    }
    if (solo.empty() || solo == "api" || solo == "ctype" || solo == "str") {
        rng.seed(seed); g_cmp = g_diff = g_shown = 0;
        run_api(n, solo == "ctype", solo == "str");
        printf("api (_isctype, _tolower_lk, _toupper_lk, __crtLCMapStringA, __crtGetStringTypeA, __ansicp, __convertcp, _wctomb, _mbtowc, is*, to*, strings, _atol): %llu comparaciones, %llu diferencias\n", (unsigned long long)g_cmp, (unsigned long long)g_diff);
        if (g_diff) rc = 1;
    }
    printf(rc ? "RESULTADO: HAY DIFERENCIAS\n" : "RESULTADO: SIN DIFERENCIAS\n");
    return rc;
}
