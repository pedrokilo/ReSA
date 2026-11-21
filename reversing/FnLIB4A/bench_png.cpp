// Banco LIB4A de escritura PNG: pares EXE original vs libpng 1.0.12 (+ zlib 1.1.3) oficiales, escenarios completos de escritura de un PNG.
// Las funciones de libpng de escritura del EXE (0x7CFE00..0x7DFF00, direcciones en este archivo) se mapean en memoria dentro de este proceso de 32 bits (no se ejecuta el juego).
// Modos:
//   unit   <exe> N [semilla]                  : API publico de escritura del EXE contra el de la biblioteca (resto del flujo con las funciones de la biblioteca, ya validadas en LIB2A)
//   insitu <exe> N semilla nombre=0xDIR,...   : API del EXE en las dos pasadas; en la segunda se sustituyen las funciones indicadas por la de la biblioteca (jmp en vivo)
//   probar <exe> N semilla nombre=0xDIR,...   : igual con una sola linea (0 = igual, 1 = difiere, 2 = fallo/cuelgue); nombre `trampa` pone `xor eax,eax; ret`
//   volcar <exe> N semilla dir                : escribe los PNG de salida de la biblioteca (para el oraculo de Python)
#include "common.h"
#define NO_DUMMY_DECL
#define PNG_INTERNAL
extern "C" {
#include "png.h"
#include "zlib.h"
}
#include <time.h>
static_assert(sizeof(png_struct) == 0x1F4 && sizeof(png_info) == 0x120, "png_struct 0x1F4 y png_info 0x120 (PNG_USER_MEM_SUPPORTED, PNG_NO_PROGRESSIVE_READ, PNG_NO_STDIO)");

struct PngErr { int x; };
static std::vector<u8> g_out;
static void Lstr(const char* s) { u32 h = 2166136261u; if (!s) { L(0xDEAD0000); return; } for (; *s; s++) h = (h ^ (u8)*s) * 16777619u; L(h); }
static bool g_tr0 = getenv("LIB4A_V") != nullptr;
static void __cdecl cb_error(png_structp, png_const_charp m) { if (g_tr0) printf("png_error: %s\n", m ? m : "(null)"); L(0xE0000001); Lstr(m); throw PngErr{1}; }
static void __cdecl cb_warn(png_structp, png_const_charp m) { L(0xE0000002); Lstr(m); }
static int g_fail_at, g_nalloc; static void* g_png0;
static png_voidp __cdecl cb_malloc(png_structp, png_size_t n) {
    L(0xE0000003); L((u32)n);
    if (n == sizeof(png_struct) && !g_png0) { void* q = ar_alloc_fill(n); g_png0 = q; return q; }
    if (g_fail_at && ++g_nalloc == g_fail_at) { L(0xE0000009); return nullptr; }
    if (n == 0 || n > (4u << 20)) return nullptr;
    return ar_alloc_fill(n);
}
static void __cdecl cb_free(png_structp, png_voidp p) { L(0xE0000004); L((u32)((u8*)p - g_ar)); }
static void __cdecl cb_write(png_structp, png_bytep d, png_size_t n) { if (g_tr0 && n > 100000) printf("cb_write n=%u (primer bytes %02x %02x %02x %02x)\n", (u32)n, d[0], d[1], d[2], d[3]); L(0xE0000005); L((u32)n); g_out.insert(g_out.end(), d, d + n); }
static void __cdecl cb_flush(png_structp) { L(0xE0000006); }
static void __cdecl cb_user(png_structp pp, png_row_infop ri, png_bytep row) { L(0xE0000008); L((u32)pp->user_transform_ptr); L(ri->width); L(ri->rowbytes); L(ri->pixel_depth); if (ri->rowbytes) row[0] ^= 0x55; }
static void __cdecl cb_status(png_structp, png_uint_32 row, int pass) { L(0xE0000007); L(row); L(pass); }

struct PngApi {
    png_structp (__cdecl* create)(png_const_charp, png_voidp, png_error_ptr, png_error_ptr, png_voidp, png_malloc_ptr, png_free_ptr);
    png_structp (__cdecl* create1)(png_const_charp, png_voidp, png_error_ptr, png_error_ptr);
    void (__cdecl* set_write_fn)(png_structp, png_voidp, png_rw_ptr, png_flush_ptr);
    void (__cdecl* write_info)(png_structp, png_infop);
    void (__cdecl* write_info_before_PLTE)(png_structp, png_infop);
    void (__cdecl* write_row)(png_structp, png_bytep);
    void (__cdecl* write_rows)(png_structp, png_bytepp, png_uint_32);
    void (__cdecl* write_image)(png_structp, png_bytepp);
    void (__cdecl* write_end)(png_structp, png_infop);
    void (__cdecl* write_flush)(png_structp);
    void (__cdecl* set_flush)(png_structp, int);
    void (__cdecl* destroy)(png_structpp, png_infopp);
    void (__cdecl* write_destroy)(png_structp);
    void (__cdecl* set_filter)(png_structp, int, int);
    void (__cdecl* set_heur)(png_structp, int, int, png_doublep, png_doublep);
    void (__cdecl* set_level)(png_structp, int);
    void (__cdecl* set_memlevel)(png_structp, int);
    void (__cdecl* set_strategy)(png_structp, int);
    void (__cdecl* set_window)(png_structp, int);
    void (__cdecl* set_method)(png_structp, int);
    void (__cdecl* set_status)(png_structp, png_write_status_ptr);
    void (__cdecl* write_png)(png_structp, png_infop, int, png_voidp);
    void (__cdecl* set_bgr)(png_structp); void (__cdecl* set_swap)(png_structp); void (__cdecl* set_packing)(png_structp); void (__cdecl* set_packswap)(png_structp);
    void (__cdecl* set_shift)(png_structp, png_color_8p); int (__cdecl* set_interlace)(png_structp); void (__cdecl* set_filler)(png_structp, png_uint_32, int);
    void (__cdecl* set_swap_alpha)(png_structp); void (__cdecl* set_invert_alpha)(png_structp); void (__cdecl* set_invert_mono)(png_structp);
    void (__cdecl* set_user_info)(png_structp, png_voidp, int, int); void (__cdecl* set_user_fn)(png_structp, png_user_transform_ptr);
    void (__cdecl* set_error_fn)(png_structp, png_voidp, png_error_ptr, png_error_ptr); void (__cdecl* set_mem_fn)(png_structp, png_voidp, png_malloc_ptr, png_free_ptr);
};
static PngApi exe_api() {
    PngApi a{};
    a.create = (decltype(a.create))0x7D03B0; a.create1 = (decltype(a.create1))0x7D0380;
    a.set_write_fn = (decltype(a.set_write_fn))0x7D2C20;
    a.write_info = (decltype(a.write_info))0x7CFFA0; a.write_info_before_PLTE = (decltype(a.write_info_before_PLTE))0x7CFE00;
    a.write_row = (decltype(a.write_row))0x7D0720; a.write_rows = (decltype(a.write_rows))0x7D0690; a.write_image = (decltype(a.write_image))0x7D06C0;
    a.write_end = (decltype(a.write_end))0x7D0250; a.write_flush = (decltype(a.write_flush))0x7D0960; a.set_flush = (decltype(a.set_flush))0x7D0940;
    a.destroy = (decltype(a.destroy))0x7D0A40; a.write_destroy = (decltype(a.write_destroy))0x7D0AE0;
    a.set_filter = (decltype(a.set_filter))0x7D0BF0; a.set_heur = (decltype(a.set_heur))0x7D0DF0;
    a.set_level = (decltype(a.set_level))0x7D10E0; a.set_memlevel = (decltype(a.set_memlevel))0x7D1100; a.set_strategy = (decltype(a.set_strategy))0x7D1120;
    a.set_window = (decltype(a.set_window))0x7D1140; a.set_method = (decltype(a.set_method))0x7D11A0; a.set_status = (decltype(a.set_status))0x7D11D0;
    a.write_png = (decltype(a.write_png))0x7D1200;
    a.set_bgr = (decltype(a.set_bgr))0x7D1310; a.set_swap = (decltype(a.set_swap))0x7D1320; a.set_packing = (decltype(a.set_packing))0x7D1340; a.set_packswap = (decltype(a.set_packswap))0x7D1360;
    a.set_shift = (decltype(a.set_shift))0x7D1380; a.set_interlace = (decltype(a.set_interlace))0x7D13B0; a.set_filler = (decltype(a.set_filler))0x7D13E0;
    a.set_swap_alpha = (decltype(a.set_swap_alpha))0x7D1440; a.set_invert_alpha = (decltype(a.set_invert_alpha))0x7D1450; a.set_invert_mono = (decltype(a.set_invert_mono))0x7D1460;
    a.set_user_info = (decltype(a.set_user_info))0x7D1800; a.set_user_fn = (decltype(a.set_user_fn))0x7D11E0;
    a.set_error_fn = (decltype(a.set_error_fn))0x7D3670; a.set_mem_fn = (decltype(a.set_mem_fn))0x7DED30;
    return a;
}
static PngApi lib_api() {
    PngApi a{};
    a.create = png_create_write_struct_2; a.create1 = png_create_write_struct; a.set_write_fn = png_set_write_fn;
    a.write_info = png_write_info; a.write_info_before_PLTE = png_write_info_before_PLTE; a.write_row = png_write_row; a.write_rows = png_write_rows;
    a.write_image = png_write_image; a.write_end = png_write_end; a.write_flush = png_write_flush; a.set_flush = png_set_flush;
    a.destroy = png_destroy_write_struct; a.write_destroy = png_write_destroy; a.set_filter = png_set_filter; a.set_heur = png_set_filter_heuristics;
    a.set_level = png_set_compression_level; a.set_memlevel = png_set_compression_mem_level; a.set_strategy = png_set_compression_strategy;
    a.set_window = png_set_compression_window_bits; a.set_method = png_set_compression_method; a.set_status = png_set_write_status_fn; a.write_png = png_write_png;
    a.set_bgr = png_set_bgr; a.set_swap = png_set_swap; a.set_packing = png_set_packing; a.set_packswap = png_set_packswap; a.set_shift = png_set_shift;
    a.set_interlace = png_set_interlace_handling; a.set_filler = png_set_filler; a.set_swap_alpha = png_set_swap_alpha; a.set_invert_alpha = png_set_invert_alpha;
    a.set_invert_mono = png_set_invert_mono; a.set_user_info = png_set_user_transform_info; a.set_user_fn = png_set_write_user_transform_fn;
    a.set_error_fn = png_set_error_fn; a.set_mem_fn = png_set_mem_fn;
    return a;
}

#define LS(n) { #n, (void*)&n }
extern "C" {
void fill_window(void*); int deflate_stored(void*, int); int deflate_fast(void*, int); int deflate_slow(void*, int); unsigned longest_match(void*, unsigned);
void _tr_init(void*); void _tr_stored_block(void*, char*, unsigned long, int); void _tr_align(void*); void _tr_flush_block(void*, char*, unsigned long, int);
void build_tree(void*, void*); void pqdownheap(void*, void*, int); void scan_tree(void*, void*, int); void send_tree(void*, void*, int); void compress_block(void*, void*, void*); void bi_windup(void*);
}
extern "C" { int png_text_compress(png_structp, png_charp, png_size_t, int, void*); void png_write_compressed_data_out(png_structp, void*); }
static std::map<std::string, void*> libsyms() {
    std::map<std::string, void*> m;
    std::pair<const char*, void*> t[] = {
        LS(png_create_write_struct_2), LS(png_create_write_struct), LS(png_set_write_fn), LS(png_write_info), LS(png_write_info_before_PLTE), LS(png_write_row), LS(png_write_rows),
        LS(png_write_image), LS(png_write_end), LS(png_write_flush), LS(png_set_flush), LS(png_destroy_write_struct), LS(png_write_destroy), LS(png_set_filter),
        LS(png_set_filter_heuristics), LS(png_set_compression_level), LS(png_set_compression_mem_level), LS(png_set_compression_strategy), LS(png_set_compression_window_bits),
        LS(png_set_compression_method), LS(png_set_write_status_fn), LS(png_write_png), LS(png_write_init_3), LS(png_write_init_2), LS(png_write_init),
        LS(png_write_chunk_start), LS(png_write_chunk_data), LS(png_write_chunk_end), LS(png_write_chunk), LS(png_write_sig), LS(png_write_IHDR), LS(png_write_PLTE),
        LS(png_write_IDAT), LS(png_write_IEND), LS(png_write_gAMA), LS(png_write_gAMA_fixed), LS(png_write_sRGB), LS(png_write_iCCP), LS(png_write_sPLT), LS(png_write_sBIT),
        LS(png_write_cHRM), LS(png_write_cHRM_fixed), LS(png_write_tRNS), LS(png_write_bKGD), LS(png_write_hIST), LS(png_check_keyword), LS(png_write_tEXt), LS(png_write_zTXt),
        LS(png_write_oFFs), LS(png_write_pCAL), LS(png_write_sCAL_s), LS(png_write_pHYs), LS(png_write_start_row), LS(png_write_finish_row),
        LS(png_do_write_interlace), LS(png_write_find_filter), LS(png_write_filtered_row), LS(png_do_write_transformations), LS(png_do_pack), LS(png_do_shift),
        LS(png_do_write_swap_alpha), LS(png_do_write_invert_alpha), LS(png_do_invert), LS(png_do_swap), LS(png_do_packswap), LS(png_do_bgr), LS(png_do_strip_filler),
        LS(png_write_data), LS(png_flush), LS(png_set_filler), LS(png_set_packing), LS(png_set_packswap), LS(png_set_swap), LS(png_set_invert_mono), LS(png_set_invert_alpha),
        LS(png_set_swap_alpha), LS(png_set_bgr), LS(png_set_shift), LS(png_set_interlace_handling), LS(png_set_user_transform_info), LS(png_set_write_user_transform_fn),
        LS(png_set_compression_buffer_size), LS(png_set_error_fn), LS(png_set_mem_fn), LS(png_error), LS(png_warning), LS(png_get_mem_ptr), LS(png_get_user_transform_ptr), LS(png_text_compress), LS(png_write_compressed_data_out), LS(png_save_uint_32), LS(png_save_int_32), LS(png_save_uint_16),
        LS(deflateInit_), LS(deflateInit2_), LS(deflateSetDictionary), LS(deflateReset), LS(deflateParams), LS(deflate), LS(deflateEnd), LS(deflateCopy), LS(fill_window), LS(deflate_stored), LS(deflate_fast), LS(deflate_slow), LS(longest_match),
        LS(_tr_init), LS(_tr_stored_block), LS(_tr_align), LS(_tr_flush_block), LS(build_tree), LS(pqdownheap), LS(scan_tree), LS(send_tree), LS(compress_block), LS(bi_windup), LS(adler32), LS(crc32),
    };
    for (auto& e : t) m[e.first] = e.second;
    return m;
}

static int pick(std::initializer_list<int> l) { return *(l.begin() + R->below((u32)l.size())); }
static void fill_row(u8* p, size_t n, int mode) {
    u8 v = (u8)R->next();
    for (size_t i = 0; i < n; i++) {
        u32 r = R->next();
        switch (mode) {
        case 0: p[i] = (u8)r; break;
        case 1: v = (u8)(v + (int)(r % 5) - 2); p[i] = v; break;
        case 2: p[i] = (r % 11 == 0) ? (u8)r : v; break;
        case 3: p[i] = (u8)(i * 3 + (r % 3)); break;
        default: p[i] = (u8)(r & 0xF0); break;
        }
    }
}
static char* mk_cstr(u32 maxlen, bool valid_kw) {
    u32 n = 1 + R->below(maxlen); char* s = (char*)ar_alloc(n + 2);
    for (u32 i = 0; i < n; i++) s[i] = (char)(valid_kw ? (R->chance(10) ? ' ' : 0x41 + R->below(50)) : (R->chance(10) ? (R->below(2) ? 0x01 : 0x20) : 0x30 + R->below(75)));
    s[n] = 0; return s;
}

struct Dig { std::vector<u32> tr; u32 outh; size_t outn; int crash; std::vector<u8> snap; std::vector<u8> out; };
static bool is_img(u32 v) { return v >= 0x401000 && v < 0x1100000; }
static long mem_diff(const std::vector<u8>& a, const std::vector<u8>& b) {
    if (a.size() != b.size()) return 0;
    for (size_t i = 0; i + 4 <= a.size(); i += 4) {
        u32 x = *(const u32*)&a[i], y = *(const u32*)&b[i];
        if (x == y) continue; if (is_img(x) && is_img(y)) continue; return (long)i;
    }
    return -1;
}
static const char* VER = PNG_LIBPNG_VER_STRING;
static bool g_tr = getenv("LIB4A_V") != nullptr;

static void flow(u64 seed, const PngApi& A, Dig& D, png_structp& png, png_infop& info);

static bool one_scenario(u64 seed, const PngApi& A, Dig& D) {
    R = new Rng(seed); g_log.clear(); memset(g_ar, 0, g_brk > 4096 ? g_brk + 4096 : 4096); g_brk = 4096; g_out.clear(); D.tr.clear();
    g_png0 = nullptr; g_nalloc = 0; g_fail_at = R->chance(4) ? 1 + R->below(8) : 0;
    png_structp png = nullptr; png_infop info = nullptr;
    try { flow(seed, A, D, png, info); } catch (std::exception& e) { if (g_tr) printf("std::exception %s", e.what()); throw; }
    // lo escrito
    D.out = g_out; D.outh = fnv(g_out.data(), g_out.size()); D.outn = g_out.size();
    D.tr.push_back(D.outh); D.tr.push_back((u32)D.outn);
    for (u32 v : g_log) D.tr.push_back(v);
    if (g_png0) memset(g_png0, 0, sizeof(jmp_buf)); g_png0 = nullptr; // jmpbuf: contenido de _setjmp3 distinto (registros y EIP) en el EXE y en la biblioteca
    D.snap.assign(g_ar, g_ar + g_brk);
    delete R;
    return true;
}

static void flow(u64 seed, const PngApi& A, Dig& D, png_structp& png, png_infop& info) {
    (void)D;
    try { png = A.create(R->chance(2) ? VER : (R->chance(50) ? "1.0.11" : "0.9.0"), nullptr, cb_error, cb_warn, nullptr, cb_malloc, cb_free); } catch (PngErr&) { L(0xF0000005); return; }
    L(0xF0000001); L(png ? 1 : 0);
    if (!png) return;
    info = png_create_info_struct(png);
    if (!info) { png_structp p2 = png; png_infop i2 = nullptr; A.destroy(&p2, &i2); return; }
    try {
    A.set_write_fn(png, nullptr, cb_write, R->chance(70) ? cb_flush : nullptr);
    {
        // --- parametros de la imagen
        static const int ct[] = {0, 2, 3, 4, 6};
        int color_type = ct[R->below(5)];
        int depth;
        switch (color_type) {
        case 0: depth = pick({1, 2, 4, 8, 16}); break;
        case 3: depth = pick({1, 2, 4, 8}); break;
        default: depth = R->chance(70) ? 8 : 16; break;
        }
        if (R->chance(3)) depth = pick({0, 3, 5, 7, 12, 32});
        if (R->chance(2)) color_type = pick({1, 5, 7, 255});
        int interlace = R->chance(30) ? 1 : 0; if (R->chance(2)) interlace = 2;
        u32 w = R->chance(10) ? 1 + R->below(2) : (R->chance(85) ? 1 + R->below(70) : 1 + R->below(500));
        u32 h = R->chance(10) ? 1 : 1 + R->below(R->chance(85) ? 30 : 160);
        if (R->chance(1)) w = 0; if (R->chance(1)) h = 0;
        int channels = (color_type == 0 || color_type == 3) ? 1 : (color_type == 2 ? 3 : (color_type == 4 ? 2 : 4));
        u32 pixbits = depth * channels; u32 rowbytes = (w * pixbits + 7) / 8;
        int comp_type = R->chance(3) ? 1 : 0, filt_type = R->chance(3) ? 1 : 0;
        if (g_tr) printf("seed %llu w %u h %u ct %d depth %d interlace %d\n", (unsigned long long)seed, w, h, color_type, depth, interlace);
        // --- compresion y filtros
        if (R->chance(70)) { int lv = R->chance(15) ? (int)R->below(14) - 2 : (R->chance(40) ? -1 : (int)R->below(10)); A.set_level(png, lv); L(lv); }
        if (R->chance(40)) { int m = R->chance(10) ? (int)R->below(12) : 1 + R->below(9); A.set_memlevel(png, m); }
        if (R->chance(40)) { int s = R->chance(10) ? (int)R->below(5) : (int)R->below(3); A.set_strategy(png, s); }
        if (R->chance(40)) { int wb = R->chance(10) ? (int)R->below(20) : 9 + R->below(7); if (wb == 8) wb = 9; A.set_window(png, wb); }
        if (R->chance(5)) A.set_method(png, R->chance(50) ? 8 : 7);
        if (R->chance(10)) png_set_compression_buffer_size(png, R->chance(20) ? 100 + R->below(100) : 100 + R->below(9000));
        if (R->chance(50)) { int fl = R->chance(30) ? 0x08 : (R->chance(30) ? 0xF8 : (int)(R->below(256) & 0xF8)); A.set_filter(png, R->chance(5) ? 1 : 0, fl); L(fl); }
        if (R->chance(25)) {
            double wt[8], cs[8]; int nw = R->below(8);
            for (int i = 0; i < 8; i++) { wt[i] = R->chance(10) ? (R->below(2) ? 0.0 : 1.5) : R->below(1001) / 1000.0; cs[i] = R->chance(10) ? 0.5 : 1.0 + R->below(5000) / 1000.0; }
            A.set_heur(png, R->chance(5) ? 5 : (int)R->below(3), nw, nw ? wt : nullptr, nw ? cs : nullptr);
        }
        if (R->chance(15)) A.set_status(png, cb_status);
        bool use_shift = R->chance(10);
        // --- informacion
        png_set_IHDR(png, info, w, h, depth, color_type, interlace, comp_type, filt_type);
        bool pal = color_type == 3 && depth >= 1 && depth <= 8;
        if (pal || (R->chance(10) && color_type != 0 && color_type != 4)) {
            int np = pal ? 1 + R->below(1u << depth) : 1 + R->below(256); if (R->chance(3)) np = 257 + R->below(10);
            png_color* pl = (png_color*)ar_alloc(sizeof(png_color) * (np + 1));
            for (int i = 0; i < np; i++) { pl[i].red = (u8)R->next(); pl[i].green = (u8)R->next(); pl[i].blue = (u8)R->next(); }
            png_set_PLTE(png, info, pl, np > 256 ? 256 : np);
            if (R->chance(40)) { u8* tr = (u8*)ar_alloc(256); for (int i = 0; i < 256; i++) tr[i] = (u8)R->next(); png_set_tRNS(png, info, tr, 1 + R->below(np > 256 ? 256 : np), nullptr); }
            if (R->chance(30)) { png_color_16* bg = (png_color_16*)ar_alloc(sizeof(png_color_16)); memset(bg, 0, sizeof *bg); bg->index = (u8)R->below(np); png_set_bKGD(png, info, bg); }
            if (R->chance(30)) { u16* hist = (u16*)ar_alloc(512); for (int i = 0; i < 256; i++) hist[i] = (u16)R->next(); png_set_hIST(png, info, hist); }
        } else {
            if (R->chance(25)) { png_color_16* tc = (png_color_16*)ar_alloc(sizeof(png_color_16)); tc->index = 0; tc->red = (u16)(R->next() & ((1u << (depth > 0 && depth <= 16 ? depth : 8)) - 1)); tc->green = (u16)R->below(256); tc->blue = (u16)R->below(256); tc->gray = tc->red; png_set_tRNS(png, info, nullptr, 0, tc); }
            if (R->chance(25)) { png_color_16* bg = (png_color_16*)ar_alloc(sizeof(png_color_16)); memset(bg, 0, sizeof *bg); bg->gray = (u16)R->next(); bg->red = (u16)R->next(); bg->green = (u16)R->next(); bg->blue = (u16)R->next(); png_set_bKGD(png, info, bg); }
        }
        if (R->chance(30)) png_set_gAMA(png, info, R->chance(10) ? 0.0 : R->below(300000) / 100000.0);
        if (R->chance(10)) png_set_gAMA_fixed(png, info, (png_fixed_point)R->below(300000));
        if (R->chance(20)) png_set_sRGB(png, info, R->chance(10) ? 7 : (int)R->below(4));
        if (R->chance(20)) { double v[8]; for (int i = 0; i < 8; i++) v[i] = R->below(80000) / 100000.0; png_set_cHRM(png, info, v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]); }
        if (R->chance(10)) png_set_cHRM_fixed(png, info, R->below(80000), R->below(80000), R->below(80000), R->below(80000), R->below(80000), R->below(80000), R->below(80000), R->below(80000));
        {   // sBIT: valores validos (1..profundidad) cuando se usa PNG_SHIFT (con 0 o mayores, png_do_shift no termina: fuera de dominio)
            int md = (color_type == 3) ? 8 : (depth >= 1 && depth <= 16 ? depth : 8);
            auto sbv = [&]() { return (u8)(1 + R->below(md)); };
            bool want = R->chance(20) || use_shift;
            if (want) { png_color_8 sb; memset(&sb, 0, sizeof sb); sb.red = sbv(); sb.green = sbv(); sb.blue = sbv(); sb.gray = sbv(); sb.alpha = sbv();
                if (R->chance(20)) png_set_sBIT(png, info, &sb);
                if (use_shift) A.set_shift(png, &sb); }
        }
        if (R->chance(25)) png_set_pHYs(png, info, R->next(), R->next(), R->chance(10) ? 3 : (int)R->below(2));
        if (R->chance(15)) png_set_oFFs(png, info, (png_int_32)R->next(), (png_int_32)R->next(), R->chance(10) ? 3 : (int)R->below(2));
        if (R->chance(15)) { char* ws = (char*)ar_alloc(24); char* hs = (char*)ar_alloc(24); snprintf(ws, 24, R->chance(8) ? "-%u.5" : "%u.%u", R->below(5000), R->below(100)); snprintf(hs, 24, R->chance(8) ? "x%u" : "%u.%u", R->below(5000), R->below(100)); info->scal_unit = (png_byte)(R->chance(10) ? 3 : 1 + R->below(2)); info->scal_s_width = ws; info->scal_s_height = hs; info->valid |= PNG_INFO_sCAL; }
        if (R->chance(15)) { png_time* t = (png_time*)ar_alloc(sizeof(png_time)); t->year = (u16)(1990 + R->below(40)); t->month = (u8)(R->chance(5) ? 13 : 1 + R->below(12)); t->day = (u8)(1 + R->below(31)); t->hour = (u8)R->below(24); t->minute = (u8)R->below(60); t->second = (u8)R->below(R->chance(5) ? 70 : 61); png_set_tIME(png, info, t); }
        if (R->chance(10)) { int np = R->below(4); png_charp params[4]; for (int i = 0; i < np; i++) params[i] = mk_cstr(10, false); png_charpp pp = (png_charpp)ar_alloc(sizeof(png_charp) * 5); for (int i = 0; i < np; i++) pp[i] = params[i];
            png_set_pCAL(png, info, mk_cstr(20, true), (png_int_32)R->below(100), (png_int_32)R->below(100), R->chance(10) ? 5 : (int)R->below(4), np, mk_cstr(8, false), pp); }
        if (R->chance(12)) { u32 pl = R->below(R->chance(80) ? 400 : 5000); u8* prof = (u8*)ar_alloc(pl + 1); fill_row(prof, pl, R->below(4)); png_set_iCCP(png, info, mk_cstr(30, true), R->chance(5) ? 1 : 0, (png_charp)prof, pl); }
        if (R->chance(40)) {
            int nt = 1 + R->below(4); png_text* tx = (png_text*)ar_alloc(sizeof(png_text) * nt); memset(tx, 0, sizeof(png_text) * nt);
            for (int i = 0; i < nt; i++) {
                tx[i].compression = R->chance(5) ? 3 : (int)R->below(2) - 1; tx[i].key = mk_cstr(R->chance(5) ? 79 : 30, R->chance(95)) /* mas de 79 caracteres: png_check_keyword hace new_key[79] = 0 sobre el char** (error de libpng 1.0.12, corrompe la pila del llamador; el EXE lo tolera por azar de su disposicion y la biblioteca no): fuera de dominio */;
                u32 tl = R->below(R->chance(80) ? 300 : 4000); u8* tb = (u8*)ar_alloc(tl + 1); fill_row(tb, tl, R->below(2) ? 1 : 2); for (u32 k = 0; k < tl; k++) if (tb[k] == 0 && !R->chance(5)) tb[k] = 'a'; tb[tl] = 0;
                tx[i].text = (png_charp)tb; tx[i].text_length = tl;
            }
            if (g_tr) for (int i = 0; i < nt; i++) printf("texto %d: compression %d clave(%zu) '%s' texto_len %zu\n", i, tx[i].compression, strlen(tx[i].key), tx[i].key, tx[i].text_length);
            png_set_text(png, info, tx, nt);
        }
        if (R->chance(10)) { // sPLT directo en png_info (png_set_sPLT no esta en el EXE)
            png_sPLT_t* sp = (png_sPLT_t*)ar_alloc(sizeof(png_sPLT_t)); int sd = R->chance(50) ? 8 : 16; int ne = R->below(6);
            sp->name = mk_cstr(30, true); sp->depth = (u8)sd; sp->nentries = ne; sp->entries = (png_sPLT_entryp)ar_alloc(sizeof(png_sPLT_entry) * (ne + 1));
            for (int i = 0; i < ne; i++) { sp->entries[i].red = (u16)R->next(); sp->entries[i].green = (u16)R->next(); sp->entries[i].blue = (u16)R->next(); sp->entries[i].alpha = (u16)R->next(); sp->entries[i].frequency = (u16)R->next(); }
            info->splt_palettes = sp; info->splt_palettes_num = 1; info->valid |= PNG_INFO_sPLT;
        }
        if (R->chance(10)) {
            int nu = 1 + R->below(3); png_unknown_chunk* uc = (png_unknown_chunk*)ar_alloc(sizeof(png_unknown_chunk) * nu); memset(uc, 0, sizeof(png_unknown_chunk) * nu);
            for (int i = 0; i < nu; i++) { const char* nm = R->chance(50) ? "prVt" : "sTEr"; memcpy(uc[i].name, nm, 5); uc[i].size = R->below(60); uc[i].data = (png_byte*)ar_alloc(uc[i].size + 1); fill_row(uc[i].data, uc[i].size, 0); uc[i].location = (u8)(R->chance(50) ? 1 : (R->chance(50) ? 2 : 8)); }
            png_set_unknown_chunks(png, info, uc, nu); for (int i = 0; i < nu; i++) png_set_unknown_chunk_location(png, info, i, R->chance(60) ? 1 : (R->chance(50) ? 2 : 8)); if (R->chance(80)) png_set_keep_unknown_chunks(png, R->chance(50) ? 3 : 2, nullptr, 0);
        }
        // --- escritura
        int mode = R->below(10);
        if (mode == 0) { A.write_info_before_PLTE(png, info); A.write_info(png, info); } else A.write_info(png, info);
        // --- transformaciones de escritura (por las funciones del API; set_shift se hace con la sBIT valida mas abajo)
        if (R->chance(15)) A.set_bgr(png); if (R->chance(15)) A.set_packing(png); if (R->chance(10)) A.set_swap(png);
        if (R->chance(10)) A.set_invert_mono(png); if (R->chance(10)) A.set_packswap(png); if (R->chance(10)) A.set_swap_alpha(png); if (R->chance(10)) A.set_invert_alpha(png);
        if (R->chance(10) && (color_type == 0 || color_type == 2) && (depth == 8 || depth == 16)) A.set_filler(png, R->next() & (R->chance(50) ? 0xFF : 0xFFFF), R->chance(50) ? PNG_FILLER_AFTER : (R->chance(5) ? 2 : PNG_FILLER_BEFORE));
        bool filler_used = false; (void)filler_used;
        if (R->chance(8)) { A.set_user_info(png, (png_voidp)(size_t)(0x1000 + R->below(0x10000)), depth, channels); /* profundidad/canales del usuario distintos de los de la imagen desbordan los buffers de fila: fuera de dominio */ A.set_user_fn(png, cb_user); }
        if (R->chance(15)) A.set_error_fn(png, nullptr, cb_error, cb_warn);
        if (R->chance(15)) A.set_mem_fn(png, nullptr, cb_malloc, cb_free);
        if (R->chance(5)) A.set_flush(png, R->chance(50) ? 0 : 1 + R->below(5));
        int passes = (png->interlaced) ? 7 : 1;
        if (R->chance(70)) passes = A.set_interlace(png);
        else if (png->interlaced) png->transformations |= PNG_INTERLACE;
        size_t bufn = (size_t)w * 8 + 64;
        std::vector<u8*> rows(h);
        int rmode = R->below(5);
        for (u32 y = 0; y < h; y++) { rows[y] = (u8*)ar_alloc(bufn); fill_row(rows[y], bufn, rmode == 4 ? (int)R->below(5) : rmode); }
        int how = R->below(10); int stop_early = R->chance(5) ? 1 + R->below(h + 1) : 0;
        if (g_tr) printf("how %d stop_early %d passes %d transformations %x filler %x usrdepth %d usrch %d\n", how, stop_early, passes, (u32)png->transformations, (u32)png->filler, png->usr_bit_depth, png->usr_channels);
        if (how < 3) {
            png_bytepp rp = (png_bytepp)ar_alloc(sizeof(png_bytep) * (h + 1)); for (u32 y = 0; y < h; y++) rp[y] = rows[y];
            A.write_image(png, rp);
        } else if (how < 5) {
            png_bytepp rp = (png_bytepp)ar_alloc(sizeof(png_bytep) * (h + 1)); for (u32 y = 0; y < h; y++) rp[y] = rows[y];
            for (int p = 0; p < passes; p++) { u32 y = 0; while (y < h) { u32 n = 1 + R->below(h - y); A.write_rows(png, rp + y, n); y += n; } }
        } else {
            for (int p = 0; p < passes; p++) for (u32 y = 0; y < h; y++) {
                if (stop_early && (u32)(p * h + y) >= (u32)stop_early) goto endrows;
                A.write_row(png, rows[y]);
                if (R->chance(2)) A.write_flush(png);
            }
        }
    endrows:
        if (R->chance(4)) A.write_flush(png);
        // texto/tiempo tras los datos
        if (R->chance(25)) { png_text* tx = (png_text*)ar_alloc(sizeof(png_text)); memset(tx, 0, sizeof *tx); tx->compression = (int)R->below(2) - 1; tx->key = mk_cstr(20, true); u32 tl = R->below(100); u8* tb = (u8*)ar_alloc(tl + 1); fill_row(tb, tl, 1); for (u32 k = 0; k < tl; k++) if (!tb[k]) tb[k] = 'z'; tb[tl] = 0; tx->text = (png_charp)tb; tx->text_length = tl; png_set_text(png, info, tx, 1); }
        if (R->chance(10)) { png_time* t = (png_time*)ar_alloc(sizeof(png_time)); t->year = 2025; t->month = 6; t->day = 7; t->hour = 8; t->minute = 9; t->second = 10; png_set_tIME(png, info, t); }
        A.write_end(png, R->chance(90) ? info : nullptr);
    }
    } catch (PngErr&) { L(0xF0000002); }
    {
        png_structp p2 = png; png_infop i2 = info;
        try { if (R->chance(10)) { A.destroy(&p2, nullptr); } else A.destroy(&p2, &i2); } catch (PngErr&) { L(0xF0000004); }
    }
    L(0xF0000003);
    return;
}

static u32 g_code, g_addr;
static bool guarded(u64 seed, const PngApi& A, Dig& D) {
    __try { return one_scenario(seed, A, D); }
    __except (g_code = GetExceptionCode(), g_addr = (u32)(GetExceptionInformation())->ExceptionRecord->ExceptionAddress, EXCEPTION_EXECUTE_HANDLER) { if (g_tr) printf("excepcion %08x en %08x seed %llu", g_code, g_addr, (unsigned long long)seed); D.crash = 1; D.tr.push_back(0xDEADC0DE); D.snap.clear(); return false; }
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
    if (argc > 5 && mode != "volcar") {
        std::string s = argv[5]; size_t p = 0;
        while (p < s.size()) { size_t e = s.find(',', p); if (e == std::string::npos) e = s.size(); std::string it = s.substr(p, e - p); p = e + 1;
            size_t q = it.find('='); std::string nm = it.substr(0, q); u32 ad = (u32)strtoul(it.c_str() + q + 1, 0, 16);
            if (nm == "trampa") { Patch pt{}; pt.addr = ad; pt.fn = nullptr; patches.push_back(pt); continue; }
            if (!syms.count(nm)) { printf("simbolo desconocido %s\n", nm.c_str()); return 2; }
            Patch pt{}; pt.addr = ad; pt.fn = syms[nm]; patches.push_back(pt); }
    }
    PngApi E = exe_api(), Lb = lib_api();
    int diffs = 0, crashes = 0, errs = 0; long long steps = 0, bytes = 0; int shown = 0;
    for (int i = 0; i < N; i++) {
        u64 seed = seed0 + i; Dig a, b; a.crash = b.crash = 0; if (getenv("LIB4A_P")) fprintf(stderr, "seed %llu\n", (unsigned long long)seed);
        if (mode == "volcar") { guarded(seed, Lb, a); char nm[512]; snprintf(nm, sizeof nm, "%s\\png_%llu.png", argv[5], (unsigned long long)seed); FILE* f = fopen(nm, "wb"); if (f) { fwrite(a.out.data(), 1, a.out.size(), f); fclose(f); } continue; }
        if (mode == "soloexe") { guarded(seed, E, a); continue; }
        if (mode == "sololib") { guarded(seed, Lb, a); continue; }
        bool oka = guarded(seed, E, a);
        if (mode == "unit") { guarded(seed, Lb, b); }
        else { for (auto& p : patches) patch_on(p); guarded(seed, E, b); for (auto& p : patches) patch_off(p); }
        steps += (long long)a.tr.size(); bytes += (long long)a.outn;
        long md = (a.snap.empty() || b.snap.empty()) ? 0 : mem_diff(a.snap, b.snap);
        bool same = oka && !b.crash && a.tr == b.tr && md < 0;
        if (!oka) crashes++;
        if (!same) {
            if (b.crash || !oka) crashes++; diffs++;
            if (shown < 3 && mode != "probar") {
                shown++; size_t k = 0; while (k < a.tr.size() && k < b.tr.size() && a.tr[k] == b.tr[k]) k++;
                printf("DIFERENCIA semilla %llu: traza difiere en %zu (exe %zu pasos, lib %zu; salida exe %zu B lib %zu B), memoria difiere en %ld de %zu, crash exe=%d lib=%d\n", (unsigned long long)seed, k, a.tr.size(), b.tr.size(), a.outn, b.outn, md, a.snap.size(), a.crash, b.crash);
            }
        }
    }
    if (mode == "probar") { printf("%d\n", crashes ? 2 : (diffs ? 1 : 0)); return 0; }
    (void)errs;
    printf("%s: %d escenarios, %lld pasos comparados, %lld bytes de PNG, %d diferencias (%d fallos)\n", mode.c_str(), N, steps, bytes, diffs, crashes);
    return diffs ? 1 : 0;
}
