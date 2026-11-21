// Banco LIB4A de deflate: pares EXE original vs zlib 1.1.3 oficial (libs/zlib-1.1.3 compilada con /arch:IA32 y -Dlocal= en deflate.c y trees.c).
// Modos:
//   unit   <exe> N [semilla]            : todo el API publico de deflate del EXE (direcciones fijas) contra el de la biblioteca; compara bytes de salida, retornos,
//                                         contadores del z_stream, registro de zalloc/zfree y la memoria entera de la arena.
//   insitu <exe> N semilla nombre=0xDIR,... : API del EXE en las dos pasadas; en la segunda se sustituyen las funciones indicadas del EXE por la de la biblioteca (jmp).
//   probar <exe> N semilla nombre=0xDIR : igual que insitu pero con salida de una linea (para la busqueda por comportamiento: 0 = igual, 1 = difiere, 2 = fallo/cuelgue).
#include "common.h"
#define NO_DUMMY_DECL
extern "C" {
#include "zlib.h"
#include "deflate.h"
}
extern "C" {
// funciones `local` de deflate.c y trees.c (externas por -Dlocal=)
void lm_init(deflate_state*); void putShortMSB(deflate_state*, uInt); void flush_pending(z_streamp);
int read_buf(z_streamp, Bytef*, unsigned); void fill_window(deflate_state*); uInt longest_match(deflate_state*, IPos);
int deflate_stored(deflate_state*, int); int deflate_fast(deflate_state*, int); int deflate_slow(deflate_state*, int);
void _tr_init(deflate_state*); void _tr_stored_block(deflate_state*, charf*, ulg, int); void _tr_align(deflate_state*);
void _tr_flush_block(deflate_state*, charf*, ulg, int); int _tr_tally(deflate_state*, unsigned, unsigned);
}
// las de trees.c que son `local` y no tienen cabecera
struct ct_data_s;
extern "C" {
void init_block(deflate_state*); void pqdownheap(deflate_state*, void*, int); void gen_bitlen(deflate_state*, void*); void gen_codes(void*, int, ush*);
void build_tree(deflate_state*, void*); void scan_tree(deflate_state*, void*, int); void send_tree(deflate_state*, void*, int);
int build_bl_tree(deflate_state*); void send_all_trees(deflate_state*, int, int, int); void compress_block(deflate_state*, void*, void*);
void set_data_type(deflate_state*); unsigned bi_reverse(unsigned, int); void bi_flush(deflate_state*); void bi_windup(deflate_state*); void copy_block(deflate_state*, charf*, unsigned, int);
}
#define LS(n) { #n, (void*)&n }
static std::map<std::string, void*> libsyms() {
    std::map<std::string, void*> m;
    std::pair<const char*, void*> t[] = {
        LS(deflateInit_), LS(deflateInit2_), LS(deflateSetDictionary), LS(deflateReset), LS(deflateParams), LS(deflate), LS(deflateEnd), LS(deflateCopy),
        LS(lm_init), LS(putShortMSB), LS(flush_pending), LS(read_buf), LS(fill_window), LS(longest_match), LS(deflate_stored), LS(deflate_fast), LS(deflate_slow),
        LS(_tr_init), LS(_tr_stored_block), LS(_tr_align), LS(_tr_flush_block), LS(_tr_tally),
        LS(init_block), LS(pqdownheap), LS(gen_bitlen), LS(gen_codes), LS(build_tree), LS(scan_tree), LS(send_tree), LS(build_bl_tree), LS(send_all_trees),
        LS(compress_block), LS(set_data_type), LS(bi_reverse), LS(bi_flush), LS(bi_windup), LS(copy_block), LS(compress), LS(compress2), LS(adler32), LS(crc32),
    };
    for (auto& e : t) m[e.first] = e.second;
    return m;
}

typedef int (__cdecl* fInit2)(z_streamp, int, int, int, int, int, const char*, int);
typedef int (__cdecl* fInit)(z_streamp, int, const char*, int);
typedef int (__cdecl* fDef)(z_streamp, int);
typedef int (__cdecl* fEnd)(z_streamp);
typedef int (__cdecl* fSetDict)(z_streamp, const Bytef*, uInt);
typedef int (__cdecl* fParams)(z_streamp, int, int);
typedef int (__cdecl* fCopy)(z_streamp, z_streamp);
struct DefApi { fInit init; fInit2 init2; fSetDict setdict; fEnd reset; fParams params; fDef def; fEnd end; fCopy copy; };

// direcciones del EXE (confirmadas por el propio banco: mismo resultado en 'unit')
static DefApi exe_api() {
    DefApi a; a.init = (fInit)0x7DF480; a.init2 = (fInit2)0x7DF4B0; a.setdict = (fSetDict)0x7DF7B0; a.reset = (fEnd)0x7DF8B0; a.params = (fParams)0x7DF9C0;
    a.def = (fDef)0x7DFA90; a.end = (fEnd)0x7DFF00; a.copy = (fCopy)0x7DFFB0; return a;
}
static DefApi lib_api() {
    DefApi a; a.init = (fInit)deflateInit_; a.init2 = (fInit2)deflateInit2_; a.setdict = (fSetDict)deflateSetDictionary; a.reset = (fEnd)deflateReset; a.params = (fParams)deflateParams;
    a.def = (fDef)deflate; a.end = (fEnd)deflateEnd; a.copy = (fCopy)deflateCopy; return a;
}

static int g_fail_at, g_nalloc; static size_t g_hw = ARENA;
static voidpf __cdecl za(voidpf, uInt n, uInt s) {
    L(0xA0000001); L(n); L(s);
    if (g_fail_at && ++g_nalloc == g_fail_at) { L(0xA0000009); return nullptr; }
    size_t sz = (size_t)n * s; if (sz == 0 || sz > (8u << 20)) return nullptr;
    return ar_alloc_fill(sz);
}
static void __cdecl zf(voidpf, voidpf p) { L(0xA0000002); L((u32)((u8*)p - g_ar)); }

static void gen_data(std::vector<u8>& d, size_t n, int mode) {
    d.resize(n);
    static const char* words[] = {"the ", "quick ", "brown ", "fox ", "jumps ", "over ", "lazy ", "dog ", "PNG ", "image ", "data ", "\n", "zlib ", "deflate ", "0000 ", "  "};
    size_t i = 0;
    while (i < n) {
        switch (mode) {
        case 0: d[i++] = (u8)R->next(); break;
        case 1: { const char* w = words[R->below(16)]; for (; *w && i < n; w++) d[i++] = *w; break; }
        case 2: { u8 v = (u8)R->next(); size_t k = 1 + R->below(400); for (; k-- && i < n;) d[i++] = v; break; }
        case 3: { if (i > 8 && R->chance(70)) { size_t from = R->below((u32)i), k = 3 + R->below(R->chance(20) ? 600 : 40); for (; k-- && i < n; from++) d[i++] = d[from]; if (i < n && R->chance(30)) d[i++] = (u8)R->next(); } else d[i++] = (u8)R->next(); break; }
        case 4: { u8 v = (u8)(i > 0 ? d[i - 1] : 0); d[i++] = (u8)(v + (int)R->below(5) - 2); break; }
        default: { if (R->chance(50)) d[i++] = (u8)(R->below(4)); else d[i++] = (u8)R->below(256); break; }
        }
    }
}

struct Dig { std::vector<u32> tr; u32 mem; u32 outh; size_t outn; int crash; std::vector<u8> snap; };
static bool is_img(u32 v) { return v >= 0x401000 && v < 0x1000000; }
// compara dos memorias palabra a palabra: dos punteros a codigo/datos de imagen (EXE o biblioteca) se consideran iguales
static long mem_diff(const std::vector<u8>& a, const std::vector<u8>& b, const std::vector<u8>* a2 = nullptr) {
    if (a.size() != b.size()) return 0;
    for (size_t i = 0; i + 4 <= a.size(); i += 4) {
        u32 x = *(const u32*)&a[i], y = *(const u32*)&b[i];
        if (x == y) continue;
        if (a2 && a2->size() == a.size() && *(const u32*)&(*a2)[i] != x) continue; // memoria no determinista en el propio EXE (relleno/basura de pila): no comparable
        if (is_img(x) && is_img(y)) continue;
        return (long)i;
    }
    return -1;
}
static const char* VER = ZLIB_VERSION; static bool g_tr = getenv("LIB4A_V") != nullptr;
#define CHK(w) do { if (g_tr && !HeapValidate(GetProcessHeap(), 0, 0)) printf("heap corrupto tras %s", w); } while (0)
#define TR(...) do { if (g_tr) printf(__VA_ARGS__); } while (0)

static bool one_scenario(u64 seed, const DefApi& A, Dig& D) {
    R = new Rng(seed); g_log.clear(); memset(g_ar, 0, g_hw); g_hw = 0; g_brk = 4096;
    D.tr.clear(); D.outh = 2166136261u; D.outn = 0;
    auto T = [&](u32 v) { D.tr.push_back(v); if (g_tr && !HeapValidate(GetProcessHeap(), 0, 0)) printf("heap corrupto en traza %zu", D.tr.size()); };
    z_stream* z = (z_stream*)ar_alloc(sizeof(z_stream)); memset(z, 0, sizeof *z);
    z->zalloc = (alloc_func)za; z->zfree = (free_func)zf; z->opaque = (voidpf)0;
    g_nalloc = 0; g_fail_at = R->chance(6) ? 1 + R->below(5) : 0;
    int level = R->chance(15) ? (int)R->below(14) - 2 : (R->chance(40) ? Z_DEFAULT_COMPRESSION : (int)R->below(10));
    int wbits = R->chance(10) ? (int)R->below(20) : 9 + R->below(7);
    if (wbits == 8) wbits = 9; // windowBits 8 esta permitido por zlib 1.1.3 pero falla (fuera de dominio, tambien en el EXE)
    int raw = R->chance(10); if (raw && wbits >= 9 && wbits <= 15) wbits = -wbits;
    int mem = R->chance(8) ? (int)R->below(12) : 1 + R->below(9);
    int strategy = R->chance(8) ? (int)R->below(5) : (int)R->below(3);
    int method = R->chance(4) ? 7 + R->below(3) : Z_DEFLATED;
    int variant = R->below(10);
    if (getenv("LIB4A_V")) printf("seed %llu level %d wbits %d mem %d strat %d method %d variant %d failat %d\n", (unsigned long long)seed, level, wbits, mem, strategy, method, variant, g_fail_at);
    int rc;
    if (variant < 6) rc = A.init2(z, level, method, wbits, mem, strategy, VER, sizeof(z_stream));
    else if (variant < 8) { rc = A.init(z, level, VER, sizeof(z_stream)); }
    else { const char* v = R->chance(50) ? "2.0.0" : VER; int ss = R->chance(50) ? 0x38 : 0x30; rc = A.init2(z, level, method, wbits, mem, strategy, v, ss); }
    T(rc); L(rc);
    if (rc != Z_OK) { z->next_in = z->next_out = nullptr; T(z->msg ? fnv(z->msg, strlen(z->msg)) : 0); z->msg = nullptr; D.mem = fnv(g_ar, g_brk); D.snap.assign(g_ar, g_ar + g_brk); return true; }
    bool israw = wbits < 0;
    std::vector<u8> in; size_t total = R->chance(30) ? R->below(300) : (R->chance(80) ? R->below(20000) : R->below(400000));
    gen_data(in, total, R->below(6));
    // diccionario
    if (R->chance(15)) {
        std::vector<u8> dict; gen_data(dict, R->below(40000), R->below(6));
        rc = A.setdict(z, dict.data(), (uInt)dict.size()); T(rc); T(z->adler); L(rc);
    }
    z_stream* zc2 = nullptr;
    std::vector<u8> out; size_t ipos = 0;
    auto do_params = [&](int nl, int ns) {
        std::vector<u8> ob((1 << 17) + 64, 0xAA); z->next_out = ob.data(); z->avail_out = (uInt)(1 << 17);
        CHK("antes params"); int r = A.params(z, nl, ns); CHK("params"); TR("params r=%d avail_out=%u avail_in=%u", r, z->avail_out, z->avail_in); size_t pr = (1 << 17) - z->avail_out; for (int q = 0; q < 64; q++) if (ob[(1 << 17) + q] != 0xAA) { puts("DESBORDE params"); break; } out.insert(out.end(), ob.begin(), ob.begin() + pr);
        T(r); T((u32)pr); T((u32)z->total_in); T((u32)z->total_out); L(r); z->next_out = nullptr; z->avail_out = 0; }; int guard = 0; bool copied = false; z_stream* z2 = nullptr; std::vector<u8> out2;
    bool done = false; int rounds = R->chance(15) ? 2 : 1;
    for (int round = 0; round < rounds && !done; round++) {
        if (round == 1) { // reset y segundo flujo
            rc = A.reset(z); T(rc); T(z->adler); ipos = 0; gen_data(in, R->below(30000), R->below(6));
            if (R->chance(40)) { int nl = R->below(10), ns = R->below(3); do_params(nl, ns); }
        }
        while (guard++ < 4000) {
            size_t rem = in.size() - ipos;
            size_t chunk = R->chance(25) ? rem : std::min<size_t>(rem, 1 + R->below(R->chance(30) ? 70000 : 3000));
            int flush = Z_NO_FLUSH; bool last = (ipos + chunk >= in.size());
            if (last) flush = Z_FINISH; else { u32 r = R->below(100); flush = r < 62 ? Z_NO_FLUSH : r < 76 ? Z_SYNC_FLUSH : r < 86 ? Z_FULL_FLUSH : r < 96 ? Z_PARTIAL_FLUSH : (int)R->below(6); }
            if (R->chance(5) && !last) { TR("params\n"); int nl = R->below(10), ns = R->below(3); do_params(nl, ns); }
            if (!copied && R->chance(4) && !last) { TR("copy\n"); z2 = (z_stream*)ar_alloc(sizeof(z_stream)); rc = A.copy(z2, z); T(rc); T(z2->adler); copied = (rc == Z_OK); if (!copied) z2 = nullptr; else zc2 = z2; }
            z->next_in = in.data() + ipos; z->avail_in = (uInt)chunk;
            bool stop = false;
            for (int k = 0; k < 6000; k++) {
                size_t osz = R->chance(30) ? 1 + R->below(7) : 1 + R->below(R->chance(20) ? 70000 : 2000);
                std::vector<u8> ob(osz + 64, 0xAA); z->next_out = ob.data(); z->avail_out = (uInt)osz;
                uInt ai = z->avail_in;
                TR("def flush=%d avail_in=%u avail_out=%u\n", flush, z->avail_in, z->avail_out);
                rc = A.def(z, flush); CHK("def");
                size_t produced = osz - z->avail_out; for (int q = 0; q < 64; q++) if (ob[osz + q] != 0xAA) { printf("DESBORDE de la salida (osz=%zu)", osz); break; }
                out.insert(out.end(), ob.begin(), ob.begin() + produced);
                T(rc); T(z->avail_in); T((u32)produced); T((u32)z->total_in); T((u32)z->total_out); T(z->adler); T(z->data_type);
                L(rc);
                if (rc == Z_STREAM_END) { stop = true; done = true; break; }
                if (rc != Z_OK && rc != Z_BUF_ERROR) { stop = true; done = true; break; }
                if (z->avail_in == 0 && z->avail_out != 0 && (flush != Z_FINISH)) break;
                if (rc == Z_BUF_ERROR && z->avail_in == ai && produced == 0 && flush != Z_FINISH) break;
                if (rc == Z_BUF_ERROR && produced == 0 && z->avail_in == ai) break;
            }
            ipos += chunk - z->avail_in;
            if (copied && z2 && R->chance(30)) { // el duplicado termina con los datos restantes
                std::vector<u8> ob(1 << 16); z2->next_in = in.data() + ipos; z2->avail_in = (uInt)(in.size() - ipos);
                for (int k = 0; k < 4000; k++) { z2->next_out = ob.data(); z2->avail_out = (uInt)ob.size(); int r2 = A.def(z2, Z_FINISH); out2.insert(out2.end(), ob.begin(), ob.begin() + (ob.size() - z2->avail_out)); T(r2); if (r2 != Z_OK && r2 != Z_BUF_ERROR) break; }
                T(A.end(z2)); T(fnv(out2.data(), out2.size())); z2 = nullptr; copied = false;
            }
            if (stop || last) { if (last && !stop) {} break; }
        }
        if (round + 1 < rounds && rc == Z_STREAM_END) { done = false; copied = false; z2 = nullptr; } // segundo flujo tras deflateReset
    }
    (void)israw;
    if (copied && z2) { T(A.end(z2)); }
    rc = A.end(z); T(rc); L(rc);
    D.outh = fnv(out.data(), out.size()); D.outn = out.size(); T(D.outh); T((u32)out.size());
    z->next_in = z->next_out = nullptr; z->msg = nullptr; if (zc2) { zc2->next_in = zc2->next_out = nullptr; zc2->msg = nullptr; }
    D.mem = fnv(g_ar, g_brk); D.snap.assign(g_ar, g_ar + g_brk);
    return true;
}

static LONG WINAPI vex(EXCEPTION_POINTERS*) { return EXCEPTION_CONTINUE_SEARCH; }
static bool guarded(u64 seed, const DefApi& A, Dig& D) {
    __try { bool r = one_scenario(seed, A, D); g_hw = g_brk + 4096; return r; }
    __except (EXCEPTION_EXECUTE_HANDLER) { g_hw = ARENA; D.crash = 1; D.tr.push_back(0xDEADC0DE); D.mem = 0; return false; }
}

int main(int argc, char** argv) {
    if (argc < 4) { printf("uso\n"); return 2; }
    setvbuf(stdout, 0, _IONBF, 0); _controlfp(_PC_24, _MCW_PC);
    g_ar = (u8*)VirtualAlloc((void*)0x20000000, ARENA, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!g_ar) { printf("arena\n"); return 2; }
    if (!map_exe(argv[1])) return 2;
    std::string mode = argv[2]; int N = atoi(argv[3]); u64 seed0 = argc > 4 ? strtoull(argv[4], 0, 0) : 1;
    auto syms = libsyms();
    std::vector<Patch> patches;
    if (argc > 5) {
        std::string s = argv[5]; size_t p = 0;
        while (p < s.size()) { size_t e = s.find(',', p); if (e == std::string::npos) e = s.size(); std::string it = s.substr(p, e - p); p = e + 1;
            size_t q = it.find('='); std::string nm = it.substr(0, q); u32 ad = (u32)strtoul(it.c_str() + q + 1, 0, 16);
            if (nm == "trampa") { Patch pt{}; pt.addr = ad; pt.fn = nullptr; patches.push_back(pt); continue; }
            if (!syms.count(nm)) { printf("simbolo desconocido %s\n", nm.c_str()); return 2; }
            Patch pt{}; pt.addr = ad; pt.fn = syms[nm]; patches.push_back(pt); }
    }
    DefApi E = exe_api(), Lb = lib_api();
    int diffs = 0, crashes = 0; long long steps = 0; int shown = 0;
    for (int i = 0; i < N; i++) {
        u64 seed = seed0 + i; Dig a, b; a.crash = b.crash = 0;
        if (mode == "soloexe") { guarded(seed, E, a); continue; }
        if (mode == "sololib") { guarded(seed, Lb, a); continue; }
        bool oka = guarded(seed, E, a);
        if (mode == "unit") { guarded(seed, Lb, b); }
        else { for (auto& p : patches) patch_on(p); guarded(seed, E, b); for (auto& p : patches) patch_off(p); }
        steps += (long long)a.tr.size();
        long md = (a.mem == b.mem) ? -1 : mem_diff(a.snap, b.snap);
        if (md >= 0 && a.tr == b.tr && !b.crash) { Dig a2; a2.crash = 0; guarded(seed, E, a2); md = mem_diff(a.snap, b.snap, &a2.snap); }
        bool same = oka && !b.crash && a.tr == b.tr && md < 0;
        if (!same) {
            if (b.crash || !oka) crashes++; diffs++;
            if (shown < 3 && mode != "probar") {
                shown++; if (md >= 0 && getenv("LIB4A_D")) { int c = 0; for (size_t q = 0; q + 4 <= a.snap.size() && c < 12; q += 4) { u32 x = *(u32*)&a.snap[q], y = *(u32*)&b.snap[q]; if (x != y && !(is_img(x) && is_img(y))) { printf("  +%zu: exe %08x lib %08x", q, x, y); c++; } } }
                size_t k = 0; while (k < a.tr.size() && k < b.tr.size() && a.tr[k] == b.tr[k]) k++;
                printf("DIFERENCIA semilla %llu: traza difiere en %zu (exe %zu pasos, lib %zu), memoria difiere en %ld de %zu, crash exe=%d lib=%d\n", (unsigned long long)seed, k, a.tr.size(), b.tr.size(), md, a.snap.size(), a.crash, b.crash);
            }
        }
    }
    if (mode == "probar") { printf("%d\n", crashes ? 2 : (diffs ? 1 : 0)); return crashes ? 2 : (diffs ? 1 : 0); }
    printf("%s: %d escenarios, %lld pasos comparados, %d diferencias (%d fallos)\n", mode.c_str(), N, steps, diffs, crashes);
    return diffs ? 1 : 0;
}
