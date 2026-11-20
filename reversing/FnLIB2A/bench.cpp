// Banco LIB2A: pares EXE original vs libpng 1.0.12 compilada desde la fuente oficial (libs/libpng-1.0.12 + zlib 1.1.3, /arch:IA32, FPU a 24 bits),
// para las funciones de lectura de libpng que el EXE lleva en 0x7D0000..0x7E9400.
//
// El EXE original (solo lectura) se mapea en 0x400000 dentro de este proceso de 32 bits (no se ejecuta el juego) y sus funciones se llaman por su direccion.
// Cada caso se ejecuta DOS veces con exactamente las mismas entradas y la misma arena de memoria (misma direccion): con la funcion del EXE y con la de la biblioteca.
// Tras cada llamada se compara: valor devuelto (entero o float), errores/avisos/lecturas/reservas de memoria (registro de callbacks), memoria de la arena entera (png_struct,
// png_info, buffers, salidas) y estado del FPU (palabra de control y pila). Los punteros a codigo (EXE vs biblioteca) se consideran iguales.
//
// Uso: bench.exe <ruta del EXE> <tabla.txt> [--filtro a,b] [--n pares] [--oracle carpeta] [--find nombre] [--seed s] [--direcciones lista.txt] [--diffs k]
#define NOMINMAX
#define PNG_INTERNAL
#include <windows.h>
#include <setjmp.h>
#include <float.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
extern "C" {
#include "png.h"
}
typedef uint32_t u32; typedef uint8_t u8; typedef uint16_t u16;
static_assert(sizeof(png_struct) == 0x1F4 && sizeof(png_info) == 0x120, "el EXE reserva png_struct de 0x1F4 y png_info de 0x120 (png_create_struct_2): hacen falta PNG_USER_MEM_SUPPORTED, PNG_NO_PROGRESSIVE_READ y PNG_NO_STDIO");

// ---------------------------------------------------------------- RNG
struct Rng {
    uint64_t s;
    explicit Rng(uint64_t seed = 1) : s(seed * 0x9E3779B97F4A7C15ull + 0x1234567) { next(); next(); }
    u32 next() { s ^= s << 13; s ^= s >> 7; s ^= s << 17; return (u32)(s >> 16); }
    u32 below(u32 n) { return n ? next() % n : 0; }
    bool chance(u32 pct) { return below(100) < pct; }
};
static Rng* R;
static bool g_small_ppu, g_wide;

// ---------------------------------------------------------------- mapa del EXE
static bool map_exe(const char* path) {
    FILE* f = fopen(path, "rb"); if (!f) { printf("no se puede abrir el EXE\n"); return false; }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    std::vector<u8> d(n); fread(d.data(), 1, n, f); fclose(f);
    auto nt = (IMAGE_NT_HEADERS32*)(d.data() + ((IMAGE_DOS_HEADER*)d.data())->e_lfanew);
    if (nt->OptionalHeader.ImageBase != 0x400000) return false;
    DWORD oldp;
    if (!VirtualProtect((void*)0x401000, 0x8B0000, PAGE_EXECUTE_READWRITE, &oldp)) { printf("no se pudo abrir la reserva (error %lu)\n", GetLastError()); return false; }
    auto sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        uintptr_t sva = 0x400000 + sec[i].VirtualAddress; size_t vsz = sec[i].Misc.VirtualSize, rsz = std::min<size_t>(sec[i].SizeOfRawData, vsz);
        uintptr_t lo = std::max<uintptr_t>(sva, 0x401000), hi = std::min<uintptr_t>(sva + vsz, 0xCB1000); if (lo >= hi) continue;
        for (uintptr_t p = lo; p < hi; p++) *(u8*)p = (p - sva < rsz) ? d[sec[i].PointerToRawData + (p - sva)] : 0;
    }
    return true;
}
static u32 g_own_end;
static inline bool is_code(u32 v) { return v >= 0x401000 && v < g_own_end; }

// ---------------------------------------------------------------- arena
static const size_t ARENA = 16u << 20;
static u8* g_ar;           // base fija
static size_t g_brk;       // siguiente reserva
static const size_t GAP = 64;
static std::vector<u32> g_log;   // registro de eventos de callbacks
static inline void L(u32 v) { g_log.push_back(v); }
static void Lstr(const char* s) { u32 h = 2166136261u; if (!s) { L(0xDEAD0000); return; } for (; *s; s++) h = (h ^ (u8)*s) * 16777619u; L(h); }
static void* ar_alloc(size_t n, size_t align = 16) {
    g_brk = (g_brk + align - 1) & ~(align - 1);
    if (g_brk + n + GAP > ARENA) return nullptr;
    void* p = g_ar + g_brk; g_brk += n + GAP; return p;
}
static void* ar_alloc_fill(size_t n) { u8* p = (u8*)ar_alloc(n); if (p) memset(p, 0xCD, n); return p; }

// ---------------------------------------------------------------- callbacks
static jmp_buf g_jb;
struct IoState { u8* data; u32 len; u32 pos; };
static void __cdecl cb_error(png_structp, png_const_charp m) { L(0xE0000001); Lstr(m); longjmp(g_jb, 1); }
static void __cdecl cb_warn(png_structp, png_const_charp m) { L(0xE0000002); Lstr(m); }
static png_voidp __cdecl cb_malloc(png_structp, png_size_t n) {
    L(0xE0000003); L((u32)n);
    if (n == 0 || n > (4u << 20)) return nullptr;
    return ar_alloc_fill(n);
}
static void __cdecl cb_free(png_structp, png_voidp p) { L(0xE0000004); L((u32)((u8*)p - g_ar)); }
static void __cdecl cb_read(png_structp png, png_bytep out, png_size_t n) {
    IoState* io = (IoState*)png->io_ptr; L(0xE0000005); L((u32)n);
    if (io->pos + n > io->len) { L(0xE0000006); longjmp(g_jb, 1); }
    memcpy(out, io->data + io->pos, n); io->pos += (u32)n;
}
static void __cdecl cb_status(png_structp, png_uint_32 row, int pass) { L(0xE0000007); L(row); L(pass); }

// ---------------------------------------------------------------- tabla
struct Fn { std::string name, spec; char ret; u32 mask; u32 exe; void* lib; int oracle; };
static std::vector<Fn> g_fns;
static std::vector<std::pair<std::string, void*>> g_libsyms;
static void libsyms_init();
#include "libsyms.inc"

// ---------------------------------------------------------------- estado base
static png_structp g_png; static png_infop g_info; static IoState* g_io;
static size_t g_hw0; static std::vector<u8> g_base_img;
static void build_base() {
    g_brk = 4096;
    g_io = (IoState*)ar_alloc(sizeof(IoState));
    g_png = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, nullptr, cb_error, cb_warn, nullptr, cb_malloc, cb_free);
    g_info = png_create_info_struct(g_png);
    png_set_read_fn(g_png, g_io, cb_read);
    g_hw0 = g_brk;
    g_base_img.assign(g_ar, g_ar + g_hw0);
    g_log.clear();
}

static u32 rint(int kind) {
    if (kind == 0) {
        static const u32 v[] = {0, 1, 2, 3, 4, 5, 7, 8, 15, 16, 31, 32, 255, 256, 1000, 0xFFFF, 0x10000, 0x7FFFFFFF, 0x80000000u, 0xFFFFFFFFu};
        if (R->chance(40)) return v[R->below(20)];
        if (R->chance(40)) return R->below(300);
        return R->next() ^ (R->next() << 16);
    }
    if (R->chance(15)) return R->below(3) ? 0 : 0xFFFFFFFFu;
    return R->below(48);
}
static void fill_buf(u8* p, size_t n, int mode) {
    for (size_t i = 0; i < n; i++) {
        u32 r = R->next();
        switch (mode) {
        case 0: p[i] = (u8)r; break;
        case 1: p[i] = (r % 7 == 0) ? 0 : (u8)(32 + r % 95); break;
        case 2: p[i] = (u8)(r % 3 ? 0 : r >> 8); break;
        default: p[i] = (u8)(0x41 + r % 26); break;
        }
    }
}
static char* mkstr(u32 maxlen = 30) {
    u32 n = R->below(maxlen); char* s = (char*)ar_alloc(n + 2);
    for (u32 i = 0; i < n; i++) s[i] = (char)(R->chance(8) ? 0x20 : 0x30 + R->below(75));
    s[n] = 0; if (R->chance(3)) s[R->below(n + 1)] = 0; return s;
}
static u8* mkbuf(size_t n, int mode = -1) { u8* p = (u8*)ar_alloc(n); fill_buf(p, n, mode < 0 ? (int)R->below(4) : mode); return p; }
static float rf() {
    u32 k = R->below(10); if (k == 0) return 0.f; if (k == 1) return 1.f; if (k == 2) return 2.2f; if (k == 3) return 0.45455f;
    if (k < 6) return (R->below(100000) / 30000.0f); if (k < 8) return (float)R->below(1000) / 1000.f; return (float)(int)R->next() / 7.0f;
}
static double rd() {
    u32 k = R->below(10); if (k == 0) return 0.0; if (k == 1) return 1.0; if (k < 4) return R->below(1000000) / 100000.0; if (k < 6) return (double)(int)R->next() / 1000.0;
    if (k == 6) return 2.2; if (k == 7) return 1e-7 * R->below(100); return (R->next() & 1 ? -1 : 1) * (double)R->below(1u << 31) / 3.0;
}

static void fuzz_info(png_infop in) {
    in->width = R->chance(80) ? R->below(5000) : R->next(); in->height = R->chance(80) ? R->below(5000) : R->next();
    in->valid = R->chance(45) ? 0xFFFFFFFFu : (R->next() & 0x7FFFFF); if (R->chance(20)) in->valid &= ~(1u << R->below(16));
    in->rowbytes = R->below(20000); in->num_palette = (u16)(R->chance(80) ? R->below(257) : R->next()); in->num_trans = (u16)(R->chance(80) ? R->below(257) : R->next());
    static const u8 bds[] = {1, 2, 4, 8, 16}, cts[] = {0, 2, 3, 4, 6};
    in->bit_depth = R->chance(90) ? bds[R->below(5)] : (u8)R->next(); in->color_type = R->chance(90) ? cts[R->below(5)] : (u8)R->next();
    in->compression_type = (u8)(R->chance(90) ? 0 : R->next()); in->filter_type = (u8)(R->chance(90) ? 0 : R->next()); in->interlace_type = (u8)(R->chance(70) ? R->below(2) : R->next());
    in->channels = (u8)(R->chance(90) ? 1 + R->below(4) : R->next()); in->pixel_depth = (u8)(R->chance(90) ? in->channels * in->bit_depth : R->next());
    for (int i = 0; i < 8; i++) in->signature[i] = (u8)R->next();
    in->gamma = rf(); in->srgb_intent = (u8)(R->chance(80) ? R->below(4) : R->next());
    in->mod_time.year = (u16)R->next(); in->mod_time.month = (u8)R->below(14); in->mod_time.day = (u8)R->below(33); in->mod_time.hour = (u8)R->below(26); in->mod_time.minute = (u8)R->below(62); in->mod_time.second = (u8)R->below(63);
    in->sig_bit.red = (u8)R->next(); in->sig_bit.green = (u8)R->next(); in->sig_bit.blue = (u8)R->next(); in->sig_bit.gray = (u8)R->next(); in->sig_bit.alpha = (u8)R->next();
    in->trans_values.index = (u8)R->next(); in->trans_values.red = (u16)R->next(); in->trans_values.green = (u16)R->next(); in->trans_values.blue = (u16)R->next(); in->trans_values.gray = (u16)R->next();
    in->background.index = (u8)R->next(); in->background.red = (u16)R->next(); in->background.green = (u16)R->next(); in->background.blue = (u16)R->next(); in->background.gray = (u16)R->next();
    in->x_offset = (png_int_32)rint(0); in->y_offset = (png_int_32)rint(0); in->offset_unit_type = (u8)(R->chance(80) ? R->below(3) : R->next());
    in->x_pixels_per_unit = rint(0); in->y_pixels_per_unit = R->chance(40) ? in->x_pixels_per_unit : (R->chance(50) ? R->below(5000) : rint(0)); if (R->chance(40)) in->x_pixels_per_unit = R->below(5000); in->phys_unit_type = (u8)(R->chance(60) ? 1 : R->chance(80) ? R->below(3) : R->next());
    if (g_small_ppu) { in->x_pixels_per_unit &= 0xFFFFFF; in->y_pixels_per_unit &= 0xFFFFFF; }
    in->x_white = rf(); in->y_white = rf(); in->x_red = rf(); in->y_red = rf(); in->x_green = rf(); in->y_green = rf(); in->x_blue = rf(); in->y_blue = rf();
    in->pcal_X0 = (png_int_32)rint(0); in->pcal_X1 = (png_int_32)rint(0); in->pcal_type = (u8)R->below(6); in->pcal_nparams = (u8)R->below(6);
    in->free_me = R->chance(30) ? 0xFFFFFFFFu : R->next() & 0xFFFF;
    in->scal_unit = (u8)R->below(4); in->scal_pixel_width = rd(); in->scal_pixel_height = rd();
    in->iccp_proflen = R->below(400); in->iccp_compression = 0;
    in->palette = (png_colorp)mkbuf(256 * 3); in->trans = mkbuf(256); in->hist = (png_uint_16p)mkbuf(512);
    in->pcal_purpose = mkstr(); in->pcal_units = mkstr(); in->pcal_params = (png_charpp)ar_alloc(8 * sizeof(char*), 4); for (int i = 0; i < 8; i++) in->pcal_params[i] = mkstr(12);
    in->iccp_name = mkstr(); in->iccp_profile = (png_charp)mkbuf(512);
    in->scal_s_width = mkstr(12); in->scal_s_height = mkstr(12);
    in->max_text = (int)R->below(8); in->num_text = in->max_text ? (int)R->below(in->max_text + 1) : 0;
    in->text = (png_textp)ar_alloc(8 * sizeof(png_text), 4); memset(in->text, 0, 8 * sizeof(png_text));
    for (int i = 0; i < 8; i++) { in->text[i].compression = (int)(R->below(5)) - 1; in->text[i].key = mkstr(20); in->text[i].text = mkstr(40); in->text[i].text_length = strlen(in->text[i].text); }
    in->unknown_chunks_num = R->below(4); in->unknown_chunks = (png_unknown_chunkp)ar_alloc(4 * sizeof(png_unknown_chunk), 4); memset(in->unknown_chunks, 0, 4 * sizeof(png_unknown_chunk));
    for (int i = 0; i < 4; i++) { memcpy(in->unknown_chunks[i].name, "abCD", 5); in->unknown_chunks[i].data = mkbuf(24); in->unknown_chunks[i].size = R->below(24); in->unknown_chunks[i].location = (png_byte)R->below(8); }
    in->splt_palettes_num = R->below(3); in->splt_palettes = (png_sPLT_tp)ar_alloc(3 * sizeof(png_sPLT_t), 4); memset(in->splt_palettes, 0, 3 * sizeof(png_sPLT_t));
    for (int i = 0; i < 3; i++) { in->splt_palettes[i].name = mkstr(); in->splt_palettes[i].depth = (u8)(R->chance(50) ? 8 : 16); in->splt_palettes[i].entries = (png_sPLT_entryp)mkbuf(40); in->splt_palettes[i].nentries = (png_int_32)R->below(4); }
    in->row_pointers = (png_bytepp)ar_alloc(4 * sizeof(void*), 4); for (int i = 0; i < 4; i++) in->row_pointers[i] = mkbuf(32);
}
static void fuzz_png(png_structp p) {
    static const u8 bds[] = {1, 2, 4, 8, 16}, cts[] = {0, 2, 3, 4, 6}, chs[] = {1, 3, 1, 2, 4};
    p->mode = R->chance(70) ? R->next() & 0x1FF : R->next(); p->flags = R->chance(70) ? R->next() & 0xFFFFFF : R->next(); p->transformations = R->chance(70) ? R->next() & 0xFFFFFF : R->next();
    u32 ci = R->below(5); p->color_type = R->chance(92) ? cts[ci] : (u8)R->next(); p->bit_depth = R->chance(92) ? bds[R->below(5)] : (u8)R->next(); p->channels = chs[ci];
    p->width = R->chance(85) ? 1 + R->below(300) : R->next(); p->height = R->chance(90) ? 1 + R->below(300) : R->next();
    p->pixel_depth = (u8)(p->channels * p->bit_depth); p->rowbytes = ((u32)p->width * p->pixel_depth + 7) >> 3; if (R->chance(10)) p->rowbytes = R->next();
    p->interlaced = (u8)R->below(2); p->pass = (u8)R->below(7); p->row_number = R->below(300); p->num_rows = R->below(300);
    p->iwidth = R->below(300); p->irowbytes = R->below(2000); p->usr_width = R->below(300); p->usr_bit_depth = bds[R->below(5)]; p->usr_channels = (u8)(1 + R->below(4));
    p->idat_size = R->chance(80) ? R->below(5000) : R->next(); p->crc = R->next(); p->num_palette = (u16)R->below(257); p->num_trans = (u16)R->below(257);
    for (int i = 0; i < 4; i++) p->chunk_name[i] = (u8)(R->chance(90) ? (R->chance(50) ? 'a' + R->below(26) : 'A' + R->below(26)) : R->next());
    p->chunk_name[4] = 0;
    p->compression = 0; p->filter = 0; p->do_filter = (u8)R->next(); p->sig_bytes = (u8)R->below(10);
    p->filler = (u16)R->next(); p->background_gamma_type = (u8)R->below(5); p->background_gamma = rf();
    p->background.index = (u8)R->next(); p->background.red = (u16)R->next(); p->background.green = (u16)R->next(); p->background.blue = (u16)R->next(); p->background.gray = (u16)R->next();
    p->background_1 = p->background; p->background_1.red ^= (u16)R->next();
    p->gamma_shift = R->below(17); p->gamma = rf(); p->screen_gamma = rf(); p->int_gamma = (png_fixed_point)(R->chance(80) ? R->below(300000) : R->next());
    p->sig_bit.red = (u8)R->next(); p->sig_bit.green = (u8)R->next(); p->sig_bit.blue = (u8)R->next(); p->sig_bit.gray = (u8)R->next(); p->sig_bit.alpha = (u8)R->next();
    p->shift = p->sig_bit; p->shift.red ^= (u8)R->next();
    p->trans_values.index = (u8)R->next(); p->trans_values.red = (u16)R->next(); p->trans_values.green = (u16)R->next(); p->trans_values.blue = (u16)R->next(); p->trans_values.gray = (u16)R->next();
    p->palette = (png_colorp)mkbuf(256 * 3); p->trans = mkbuf(256); p->hist = (png_uint_16p)mkbuf(512);
    p->rgb_to_gray_status = (u8)R->below(3); p->rgb_to_gray_red_coeff = (u16)R->next(); p->rgb_to_gray_green_coeff = (u16)R->next(); p->rgb_to_gray_blue_coeff = (u16)R->next();
    p->mng_features_permitted = (u8)R->next(); p->free_me = R->next() & 0xFFFF;
    p->read_row_fn = R->chance(50) ? cb_status : nullptr;
    p->idat_size &= 0xFFFFF;
    p->num_chunk_list = (int)R->below(6); p->chunk_list = mkbuf(40, 3);
    p->zstream.total_in = R->next(); p->zstream.total_out = R->next(); p->zstream.msg = R->chance(50) ? mkstr(10) : nullptr; p->zstream.adler = R->next(); p->zstream.data_type = (int)R->below(3);
    p->user_chunk_ptr = (png_voidp)(uintptr_t)R->next(); p->error_ptr = (png_voidp)(uintptr_t)R->next();
    p->zbuf_size = R->chance(50) ? 8192 : (png_size_t)R->next();
    p->zlib_level = (int)R->below(10) - 1; p->zlib_method = (int)R->below(10); p->zlib_window_bits = (int)R->below(20); p->zlib_mem_level = (int)R->below(10); p->zlib_strategy = (int)R->below(5);
    p->row_info.width = R->next(); p->row_info.rowbytes = R->next(); p->row_info.color_type = (png_byte)R->next(); p->row_info.bit_depth = (png_byte)R->next();
    p->row_info.channels = (png_byte)R->next(); p->row_info.pixel_depth = (png_byte)R->next();
}

// ---------------------------------------------------------------- llamada
struct Res { u32 ret = 0; bool err = false, crash = false; u32 exc = 0, fcw = 0, ftag = 0; std::vector<u32> log; std::vector<u8> mem; size_t brk = 0; };
typedef u32 (__cdecl* FNU)(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
typedef float (__cdecl* FNF)(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
static u32 g_exc;
static int filt(unsigned c) { g_exc = c; return EXCEPTION_EXECUTE_HANDLER; }
static u32 fpu_tag_reset() {
    u8 env[28]; u32 tag;
    __asm { lea eax, env
            fnstenv [eax]
            fldenv [eax] }
    tag = *(u16*)(env + 8);
    if (tag != 0xFFFF) { __asm { fninit } }
    _controlfp(_PC_24, _MCW_PC); _clearfp();
    return tag;
}
static void do_call(void* fn, char ret, const u32* a, u32* oret, bool* err, bool* crash, u32* exc) {
    *err = false; *crash = false; *exc = 0; *oret = 0;
    __try {
        if (setjmp(g_jb) == 0) {
            if (ret == 'f') { float f = ((FNF)fn)(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18], a[19]); memcpy(oret, &f, 4); }
            else *oret = ((FNU)fn)(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18], a[19]);
        } else *err = true;
    } __except (filt(GetExceptionCode())) { *crash = true; *exc = g_exc; }
}
// El png_struct del EXE (0x1F4 bytes) no tiene free_me (+0x1C8 en la biblioteca): todos los campos desde +0x1CC estan 4 bytes antes. Antes de llamar a una funcion del EXE
// (o a la biblioteca sobre la disposicion del EXE, modo 2) se traduce la estructura a la disposicion del EXE y despues se devuelve a la de la biblioteca.
static u32 g_saved_free_me;
static const size_t FM_OFF = 0x1C8;
static void to_exe_layout(png_structp) {}   // con PNG_NO_STDIO la disposicion de png_struct es la del EXE (0x1F4); sin traduccion
static void from_exe_layout(png_structp) {}
static void run_side(void* fn, const Fn& f, const u32* a, Res& r, size_t hi, int mode = 0) {
    g_log.clear(); _controlfp(_PC_24, _MCW_PC); _clearfp();
    if (mode) to_exe_layout(g_png);
    do_call(fn, f.ret, a, &r.ret, &r.err, &r.crash, &r.exc);
    if (mode) from_exe_layout(g_png);
    if (f.ret == 's') { u32 h = 2166136261u; const char* p = (const char*)(uintptr_t)r.ret; if (!r.crash && !r.err && p) { __try { for (; *p; p++) h = (h ^ (u8)*p) * 16777619u; } __except (EXCEPTION_EXECUTE_HANDLER) { h = 0xBAD; } r.ret = h; } else if (!r.crash) r.ret = 0; }
    r.ret &= f.mask; r.fcw = _controlfp(0, 0) & (_MCW_PC | _MCW_RC); r.ftag = fpu_tag_reset();
    r.log = g_log; r.brk = g_brk;
    size_t n = std::max(g_brk, hi) + 256; if (n > ARENA) n = ARENA;
    r.mem.assign(g_ar, g_ar + n);
}

static u32 g_ndiffpr; static int g_show = 6;
static bool mem_equal(const Res& a, const Res& b, std::string& why, u32& ptrnorm) {
    size_t n = std::min(a.mem.size(), b.mem.size());
    if (a.mem.size() != b.mem.size()) { why = "tamano de memoria distinto"; return false; }
    if (memcmp(a.mem.data(), b.mem.data(), n) == 0) return true;
    bool ok = true;
    for (size_t i = 0; i + 4 <= n; i += 4) {
        u32 x = *(u32*)&a.mem[i], y = *(u32*)&b.mem[i];
        if (x == y) continue;
        if (is_code(x) && is_code(y)) { ptrnorm++; continue; }
        if (x >= 0x00100000 && x < 0x00400000 && y >= 0x00100000 && y < 0x00400000) { ptrnorm++; continue; }   // direcciones de pila (variables locales distintas en cada compilacion)
        // palabras vecinas: comparar byte a byte para informar
        char t[160]; sprintf(t, "memoria +0x%zX EXE %08X LIB %08X", i, x, y);
        if (ok) why = t; ok = false;
        break;
    }
    if (ok) { // la cola no multiplo de 4
        for (size_t i = n & ~3u; i < n; i++) if (a.mem[i] != b.mem[i]) { why = "memoria (cola)"; return false; }
    }
    return ok;
}

struct Stats { u32 layout_sens = 0, pairs = 0, diffs = 0, crash_exe = 0, crash_lib = 0, err_exe = 0, err_lib = 0, ptrnorm = 0, oracle = 0; };
static FILE* g_orc;
static const char* g_orcdir;

static void build_args(const Fn& f, u32* a, std::vector<std::pair<u32, u32>>& bufs) {
    memset(a, 0, 20 * 4); int k = 0;
    for (char c : f.spec) {
        if (k >= 19) break;
        switch (c) {
        case 'P': a[k++] = R->chance(3) ? 0 : (u32)(uintptr_t)g_png; break;
        case 'I': a[k++] = R->chance(3) ? 0 : (u32)(uintptr_t)g_info; break;
        case 'i': a[k++] = rint(0); break;
        case 'j': a[k++] = rint(1); break;
        case 'b': { u32 p = R->chance(3) ? 0 : (u32)(uintptr_t)mkbuf(1024); a[k++] = p; if (p) bufs.push_back({p, 64}); break; }
        case 'B': { u32 p = (u32)(uintptr_t)mkbuf(1024); a[k++] = p; bufs.push_back({p, 64}); break; }
        case 'S': a[k++] = (u32)(uintptr_t)mkstr(40); break;
        case 'o': { u32 p = R->chance(5) ? 0 : (u32)(uintptr_t)mkbuf(64, 0); a[k++] = p; if (p) bufs.push_back({p, 64}); break; }
        case 'O': { u32 p = (u32)(uintptr_t)mkbuf(64, 0); a[k++] = p; bufs.push_back({p, 64}); break; }
        case 'R': { png_row_infop ri = (png_row_infop)ar_alloc(sizeof(png_row_info), 4); static const u8 pds[] = {1, 2, 4, 8, 16, 24, 32, 48, 64}; u32 pd = pds[R->below(9)];
                    u32 w = 1 + R->below(120); ri->width = w; ri->pixel_depth = (png_byte)pd; ri->rowbytes = (w * pd + 7) >> 3; ri->bit_depth = (png_byte)(pd >= 24 ? (pd % 3 == 0 ? 8 : (pd == 32 ? 8 : 16)) : pd);
                    ri->channels = (png_byte)(pd >= 8 ? pd / ri->bit_depth : 1); if (ri->channels == 0) ri->channels = 1; static const u8 cts[] = {0, 2, 3, 4, 6}; ri->color_type = cts[R->below(5)];
                    a[k++] = (u32)(uintptr_t)ri; break; }
        case 'F': a[k++] = R->chance(20) ? 0 : (u32)(uintptr_t)cb_read; break;
        case 'G': a[k++] = (u32)(uintptr_t)cb_malloc; break;
        case 'H': a[k++] = (u32)(uintptr_t)cb_free; break;
        case 'Y': { u32* slot = (u32*)ar_alloc(8, 4); slot[0] = (u32)(uintptr_t)g_info; slot[1] = 0; a[k++] = (u32)(uintptr_t)slot; break; }
        case 'n': a[k++] = R->below(5); break;
        case 'M': a[k++] = R->chance(10) ? 0 : R->below(201); break;
        case 'N': a[k++] = R->chance(10) ? 256 + R->below(10) : R->below(257); break;
        case 'T': { png_textp t = (png_textp)ar_alloc(8 * sizeof(png_text), 4); memset(t, 0, 8 * sizeof(png_text));
                    for (int i = 0; i < 8; i++) { t[i].compression = (int)R->below(5) - 1; t[i].key = mkstr(24); if (R->chance(10)) t[i].key = (char*)""; t[i].text = R->chance(6) ? nullptr : mkstr(60); t[i].text_length = t[i].text ? strlen(t[i].text) : 0;
                        }
                    a[k++] = (u32)(uintptr_t)(R->chance(3) ? nullptr : t); break; }
        case 'Z': { png_sPLT_tp t = (png_sPLT_tp)ar_alloc(sizeof(png_sPLT_t), 4); t->name = mkstr(); t->depth = (u8)(R->chance(50) ? 8 : 16); t->entries = (png_sPLT_entryp)mkbuf(80); t->nentries = (png_int_32)R->below(6); a[k++] = (u32)(uintptr_t)t; break; }
        case 'U': { png_unknown_chunkp t = (png_unknown_chunkp)ar_alloc(4 * sizeof(png_unknown_chunk), 4); memset(t, 0, 4 * sizeof(png_unknown_chunk));
                    for (int i = 0; i < 4; i++) { for (int j = 0; j < 4; j++) t[i].name[j] = (u8)(R->chance(50) ? 'a' + R->below(26) : 'A' + R->below(26)); t[i].name[4] = 0; t[i].data = mkbuf(40); t[i].size = R->below(40); t[i].location = (png_byte)R->below(8); }
                    a[k++] = (u32)(uintptr_t)t; break; }
        case 'Q': { char** q = (char**)ar_alloc(8 * sizeof(char*), 4); for (int i = 0; i < 8; i++) q[i] = mkstr(12); a[k++] = (u32)(uintptr_t)q; break; }
        case 'C': { char* c = (char*)ar_alloc(8); for (int i = 0; i < 4; i++) c[i] = (char)(R->chance(85) ? (R->chance(50) ? 'a' : 'A') + R->below(26) : R->next()); c[4] = 0; if (g_png->num_chunk_list > 0 && R->chance(45)) memcpy(c, g_png->chunk_list + 5 * R->below(g_png->num_chunk_list), 4); a[k++] = (u32)(uintptr_t)c; break; }
        case 'd': { double d = rd(); memcpy(&a[k], &d, 8); k += 2; break; }
        case 'f': { float v = rf(); memcpy(&a[k], &v, 4); k++; break; }
        }
    }
}

static volatile DWORD g_t0; static volatile u32 g_cur_addr; static const char* g_cur_name = "";
static uint64_t g_seed = 1; static size_t g_maxused;
static int g_show_diffs = 4;
static Stats run_function(const Fn& f, void* exefn, u32 N, bool quiet) {
    Stats st; uint64_t nh = 14695981039346656037ull; for (char c : f.name) nh = (nh ^ (u8)c) * 1099511628211ull;
    FILE* orc = nullptr;
    if (g_orcdir && f.oracle) {
        char p[300]; sprintf(p, "%s\\%s.bin", g_orcdir, f.name.c_str()); orc = fopen(p, "wb");
        if (orc) { u32 h[4] = {0x4F524332, (u32)sizeof(png_struct), (u32)sizeof(png_info), 0}; fwrite(h, 4, 4, orc); }
    }
    int shown = 0;
    for (u32 c = 0; c < N; c++) {
        g_t0 = GetTickCount();
        Rng rng(g_seed * 1000003 + nh + c); R = &rng;
        if (g_maxused > g_hw0) memset(g_ar + g_hw0, 0, g_maxused - g_hw0);
        memcpy(g_ar, g_base_img.data(), g_hw0); g_brk = g_hw0;
        g_small_ppu = !g_wide && (f.name == "png_get_pixel_aspect_ratio");
        fuzz_png(g_png); fuzz_info(g_info);
        { u32 len = R->below(3000); g_io->data = mkbuf(len + 8); g_io->len = len; g_io->pos = 0; }
        u32 a[20]; std::vector<std::pair<u32, u32>> bufs; build_args(f, a, bufs);
        size_t hwpre = g_brk; std::vector<u8> pre(g_ar, g_ar + hwpre);
        Res r1, r2;
        run_side(exefn, f, a, r1, hwpre, 1);
        memset(g_ar + hwpre, 0, std::min(ARENA - hwpre, r1.mem.size() > hwpre ? r1.mem.size() - hwpre : 0));
        memcpy(g_ar, pre.data(), hwpre); g_brk = hwpre;
        run_side(f.lib, f, a, r2, hwpre, 0);
        g_maxused = std::max(g_maxused, std::max(r1.mem.size(), r2.mem.size()));
        st.pairs++; st.crash_exe += r1.crash; st.crash_lib += r2.crash; st.err_exe += r1.err; st.err_lib += r2.err;
        std::string why; bool eq = true; char t[200];
        if (r1.crash != r2.crash || r1.exc != r2.exc) { eq = false; sprintf(t, "excepcion EXE %u/%08X LIB %u/%08X", r1.crash, r1.exc, r2.crash, r2.exc); why = t; }
        else if (!r1.crash) {
            if (r1.ret != r2.ret) { eq = false; sprintf(t, "retorno EXE %08X LIB %08X", r1.ret, r2.ret); why = t; }
            else if (r1.err != r2.err) { eq = false; why = "error (png_error) distinto"; }
            else if (r1.fcw != r2.fcw || r1.ftag != r2.ftag) { eq = false; sprintf(t, "FPU EXE cw %X tag %X LIB cw %X tag %X", r1.fcw, r1.ftag, r2.fcw, r2.ftag); why = t; }
            else if (r1.log != r2.log) { eq = false; sprintf(t, "registro de callbacks distinto (%zu vs %zu eventos)", r1.log.size(), r2.log.size()); why = t; }
            else if (!mem_equal(r1, r2, why, st.ptrnorm)) eq = false;
        }
        if (!eq) {
            st.diffs++;
            if (!quiet && shown < g_show_diffs) {
                shown++; printf("  DIF %s caso %u: %s | args", f.name.c_str(), c, why.c_str());
                for (int i = 0; i < 6; i++) printf(" %08X", a[i]);
                printf("\n");
            }
        }
        if (orc && c < 4000) {
            u32 hd[16]; memcpy(hd, a, 48); fwrite(&c, 4, 1, orc); fwrite(hd, 4, 12, orc);
            u32 rr[4] = {r1.ret, r2.ret, (u32)(r1.err | (r1.crash << 1)), (u32)(r2.err | (r2.crash << 1))}; fwrite(rr, 4, 4, orc);
            fwrite(pre.data() + ((u8*)g_png - g_ar), 1, sizeof(png_struct), orc); fwrite(pre.data() + ((u8*)g_info - g_ar), 1, sizeof(png_info), orc);
            u32 nb = (u32)bufs.size(); fwrite(&nb, 4, 1, orc);
            for (auto& b : bufs) { size_t off = b.first - (u32)(uintptr_t)g_ar; fwrite(&b.first, 4, 1, orc); fwrite(pre.data() + off, 1, 64, orc); fwrite(r1.mem.data() + off, 1, 64, orc); fwrite(r2.mem.data() + off, 1, 64, orc); }
            st.oracle++;
        }
    }
    if (orc) fclose(orc);
    return st;
}

static DWORD WINAPI watchdog(LPVOID) {
    for (;;) { Sleep(250); DWORD t = g_t0; if (t && GetTickCount() - t > 8000) { printf("HANG %s exe %08X\n", g_cur_name, g_cur_addr); fflush(stdout); ExitProcess(9); } }
}
#define OFF(S, f) printf(#S "." #f "=%zu\n", offsetof(S, f))
static void print_layout() {
    printf("sizeof png_struct=%zu png_info=%zu\n", sizeof(png_struct), sizeof(png_info));
    OFF(png_info, width); OFF(png_info, height); OFF(png_info, valid); OFF(png_info, rowbytes); OFF(png_info, palette); OFF(png_info, num_palette); OFF(png_info, num_trans);
    OFF(png_info, bit_depth); OFF(png_info, color_type); OFF(png_info, compression_type); OFF(png_info, filter_type); OFF(png_info, interlace_type); OFF(png_info, channels);
    OFF(png_info, pixel_depth); OFF(png_info, signature); OFF(png_info, gamma); OFF(png_info, srgb_intent); OFF(png_info, num_text); OFF(png_info, max_text); OFF(png_info, text);
    OFF(png_info, mod_time); OFF(png_info, sig_bit); OFF(png_info, trans); OFF(png_info, trans_values); OFF(png_info, background); OFF(png_info, x_offset); OFF(png_info, y_offset);
    OFF(png_info, offset_unit_type); OFF(png_info, x_pixels_per_unit); OFF(png_info, y_pixels_per_unit); OFF(png_info, phys_unit_type); OFF(png_info, hist);
    OFF(png_info, x_white); OFF(png_info, y_white); OFF(png_info, x_red); OFF(png_info, y_red); OFF(png_info, x_green); OFF(png_info, y_green); OFF(png_info, x_blue); OFF(png_info, y_blue);
    OFF(png_info, pcal_purpose); OFF(png_info, pcal_X0); OFF(png_info, pcal_X1); OFF(png_info, pcal_units); OFF(png_info, pcal_params); OFF(png_info, pcal_type); OFF(png_info, pcal_nparams);
    OFF(png_info, free_me); OFF(png_info, unknown_chunks); OFF(png_info, unknown_chunks_num); OFF(png_info, iccp_name); OFF(png_info, iccp_profile); OFF(png_info, iccp_proflen);
    OFF(png_info, iccp_compression); OFF(png_info, splt_palettes); OFF(png_info, splt_palettes_num); OFF(png_info, scal_unit); OFF(png_info, scal_pixel_width); OFF(png_info, scal_pixel_height);
    OFF(png_info, row_pointers);
    OFF(png_struct, mode); OFF(png_struct, flags); OFF(png_struct, transformations); OFF(png_struct, width); OFF(png_struct, height); OFF(png_struct, rowbytes); OFF(png_struct, io_ptr);
    OFF(png_struct, error_ptr); OFF(png_struct, idat_size); OFF(png_struct, crc); OFF(png_struct, sig_bytes); OFF(png_struct, bit_depth); OFF(png_struct, color_type); OFF(png_struct, channels);
    OFF(png_struct, pixel_depth); OFF(png_struct, chunk_name); OFF(png_struct, mem_ptr); OFF(png_struct, interlaced); OFF(png_struct, filter); OFF(png_struct, compression);
    OFF(png_struct, num_palette); OFF(png_struct, num_trans); OFF(png_struct, palette);
}

// Prueba negativa: la biblioteca mutada (mutate.py) consulta esta funcion; sin mutacion (LIB2A_MUT ausente) nunca dispara.
extern "C" int lib2a_mutq(int k) {
    static int m = -2; static unsigned cnt = 0;
    if (m == -2) { const char* e = getenv("LIB2A_MUT"); m = e ? atoi(e) : 0; }
    if (m != k) return 0;
    return (++cnt % 5) == 0;
}

int main(int argc, char** argv) {
    if (argc < 3) { printf("uso: bench <exe> <tabla> [opciones]\n"); return 2; }
    const char* exe = argv[1]; const char* tab = argv[2]; std::vector<std::string> filt_names; u32 N = 16384; const char* findname = nullptr; const char* addrfile = nullptr; bool layout = false;
    for (int i = 3; i < argc; i++) {
        std::string s = argv[i];
        if (s == "--filtro" && i + 1 < argc) { std::string v = argv[++i]; size_t p = 0; while (p < v.size()) { size_t q = v.find(',', p); if (q == std::string::npos) q = v.size(); filt_names.push_back(v.substr(p, q - p)); p = q + 1; } }
        else if (s == "--n" && i + 1 < argc) N = atoi(argv[++i]);
        else if (s == "--oracle" && i + 1 < argc) g_orcdir = argv[++i];
        else if (s == "--find" && i + 1 < argc) findname = argv[++i];
        else if (s == "--direcciones" && i + 1 < argc) addrfile = argv[++i];
        else if (s == "--seed" && i + 1 < argc) g_seed = _strtoui64(argv[++i], 0, 10);
        else if (s == "--diffs" && i + 1 < argc) g_show_diffs = atoi(argv[++i]);
        else if (s == "--layout") layout = true;
        else if (s == "--ancho") g_wide = true;
    }
    if (layout) { print_layout(); return 0; }
    HMODULE self = GetModuleHandleA(nullptr); auto nt = (IMAGE_NT_HEADERS32*)((u8*)self + ((IMAGE_DOS_HEADER*)self)->e_lfanew); g_own_end = 0x400000 + nt->OptionalHeader.SizeOfImage;
    if (!map_exe(exe)) return 3;
    for (uintptr_t base : {0x10000000u, 0x20000000u, 0x30000000u, 0x50000000u, 0x60000000u}) { g_ar = (u8*)VirtualAlloc((void*)base, ARENA, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); if (g_ar) break; }
    if (!g_ar) { printf("no se pudo reservar la arena\n"); return 3; }
    CreateThread(nullptr, 0, watchdog, nullptr, 0, nullptr);
    _controlfp(_PC_24, _MCW_PC); libsyms_init();
    { FILE* f = fopen(tab, "r"); if (!f) { printf("sin tabla\n"); return 3; } char nm[100], sp[40], rt[4]; unsigned mk, ex, orc;
      while (fscanf(f, "%99s %39s %3s %x %x %u", nm, sp, rt, &mk, &ex, &orc) == 6) {
          Fn fn; fn.name = nm; fn.spec = strcmp(sp, "-") ? sp : ""; fn.ret = rt[0]; fn.mask = mk; fn.exe = ex; fn.oracle = orc; fn.lib = nullptr;
          for (auto& s : g_libsyms) if (s.first == fn.name) fn.lib = s.second;
          g_fns.push_back(fn); }
      fclose(f); }
    build_base();
    u32 total_diffs = 0, nfn = 0; u32 minpairs = 0xFFFFFFFF;
    auto wanted = [&](const Fn& f) { if (filt_names.empty()) return true; for (auto& n : filt_names) if (n == f.name) return true; return false; };
    if (findname) {
        std::vector<u32> cand; { FILE* f = fopen(addrfile, "r"); unsigned v; while (f && fscanf(f, "%x", &v) == 1) cand.push_back(v); if (f) fclose(f); }
        for (auto& f : g_fns) if (f.name == findname && f.lib) {
            for (u32 ad : cand) { g_cur_name = findname; g_cur_addr = ad; g_t0 = GetTickCount(); Stats s = run_function(f, (void*)(uintptr_t)ad, 60, true); g_t0 = 0;
                if (s.diffs == 0 && s.crash_exe < s.pairs / 2) printf("CANDIDATA %s 0x%06X\n", findname, ad); }
        }
        printf("fin find\n"); return 0;
    }
    for (auto& f : g_fns) {
        if (!wanted(f) || !f.exe) continue;
        if (!f.lib) { printf("%-34s 0x%06X SIN SIMBOLO EN LA BIBLIOTECA\n", f.name.c_str(), f.exe); continue; }
        g_cur_name = f.name.c_str(); g_cur_addr = f.exe; g_t0 = GetTickCount();
        Stats s = run_function(f, (void*)(uintptr_t)f.exe, N, false);
        g_t0 = 0; nfn++; total_diffs += s.diffs; minpairs = std::min(minpairs, s.pairs);
        printf("%-34s 0x%06X %8u %6u  fallos %u/%u  png_error %u/%u  ptrnorm %u  oraculo %u  disposicion %u\n", f.name.c_str(), f.exe, s.pairs, s.diffs, s.crash_exe, s.crash_lib, s.err_exe, s.err_lib, s.ptrnorm, s.oracle, s.layout_sens);
        fflush(stdout);
    }
    printf("RESULTADO funciones %u  diferencias %u  minimo de pares %u\n", nfn, total_diffs, nfn ? minpairs : 0);
    return total_diffs ? 1 : 0;
}
