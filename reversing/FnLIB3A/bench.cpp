// Banco LIB3A: pares EXE original vs libjpeg 6b compilada desde la fuente oficial (libs/libjpeg-6b), para callbacks de libjpeg del EXE (0x5C2xxx-0x5CFxxx).
//
// El EXE original (solo lectura) se mapea en 0x400000 dentro de este proceso de 32 bits (sin ejecutar el juego) y se llama a sus funciones por direccion. Cada caso se ejecuta DOS veces con
// exactamente las mismas entradas y la misma arena de memoria (misma direccion): con la funcion del EXE y con la de la biblioteca (jpeg6b_find, las funciones static de la fuente oficial);
// se comparan valor devuelto, error lanzado y TODA la arena usada (salvo campos de puntero a funcion enmascarados). La precision del FPU se fija a 24 bits como en el juego.
// Cada caso escribe ademas una traza (trace_<fn>.bin) que lee oraculo.py (implementacion independiente en Python) para comprobar la salida contra la especificacion del algoritmo.
//
// Uso: bench.exe <carpeta de salida> <ruta del EXE> [--filtro texto] [--n N] [--rondas k] (devuelve 0 si todo coincide)
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <float.h>
#include <vector>
#include <string>
#include <random>
#include <functional>
#include <algorithm>
#include <memory>
#include <stdarg.h>
#include <math.h>

#define XMD_H /* INT16/INT32 de windows.h; jmorecfg.h no los redefine */
#undef FAR
extern "C" {
#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"
#include "jdct.h"
#include "jpeg6b_export.h"
#include "exe_config.h"
}

static_assert(sizeof(void*) == 4, "32 bits");
extern "C" { int lib3a_mut = 0; } // numero de mutacion (solo lo leen las copias mutadas de la biblioteca: prueba negativa, mutate.py); 0 = biblioteca oficial

// ---------------------------------------------------------------- mapa del EXE
static bool map_exe(const char* path) {
    FILE* f = fopen(path, "rb"); if (!f) { printf("no se puede abrir el EXE\n"); return false; }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    std::vector<uint8_t> d(n); fread(d.data(), 1, n, f); fclose(f);
    auto nt = (IMAGE_NT_HEADERS32*)(d.data() + ((IMAGE_DOS_HEADER*)d.data())->e_lfanew);
    if (nt->OptionalHeader.ImageBase != 0x400000) return false;
    DWORD oldp;
    if (!VirtualProtect((void*)0x401000, 0x8B0000, PAGE_EXECUTE_READWRITE, &oldp)) { printf("no se pudo abrir la reserva (error %lu)\n", GetLastError()); return false; }
    if ((uintptr_t)&map_exe < 0xCB1000 && (uintptr_t)&map_exe >= 0x400000) { printf("el codigo del banco esta dentro de la reserva\n"); return false; }
    auto sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        uintptr_t sva = 0x400000 + sec[i].VirtualAddress; size_t vsz = sec[i].Misc.VirtualSize, rsz = std::min<size_t>(sec[i].SizeOfRawData, vsz);
        uintptr_t lo = std::max<uintptr_t>(sva, 0x401000), hi = std::min<uintptr_t>(sva + vsz, 0x401000 + 0x8B0000); if (lo >= hi) continue;
        for (uintptr_t p = lo; p < hi; p++) *(uint8_t*)p = (p - sva < rsz) ? d[sec[i].PointerToRawData + (p - sva)] : 0;
    }
    return true;
}

// ---------------------------------------------------------------- arena determinista
static uint8_t g_arena[1 << 20];
static size_t g_top = 0, g_hi = 0;
static void* A(size_t n, size_t al = 16) { g_top = (g_top + al - 1) & ~(al - 1); void* p = g_arena + g_top; g_top += n; if (g_top > sizeof g_arena) { puts("arena agotada"); exit(2); } if (g_top > g_hi) g_hi = g_top; return p; }
template <class T> static T* AT(size_t n = 1) { return (T*)A(sizeof(T) * n, 8); }

static jmp_buf g_jb; static int g_err_code; static int g_err_calls;
static void __cdecl mock_error_exit(j_common_ptr c) { g_err_code = c->err->msg_code; g_err_calls++; longjmp(g_jb, 1); }
static void __cdecl mock_emit(j_common_ptr, int) {}
static void* __cdecl mock_alloc_small(j_common_ptr, int, size_t n) { void* p = A(n, 8); if (getenv("DBGALLOC")) printf("   alloc_small off=%zu n=%zu\n", (size_t)((uint8_t*)p - g_arena), n); return p; }
static void* __cdecl mock_alloc_large(j_common_ptr, int, size_t n) { return A(n, 8); }
static JSAMPARRAY __cdecl mock_alloc_sarray(j_common_ptr, int, JDIMENSION w, JDIMENSION h) {
    JSAMPARRAY a = (JSAMPARRAY)A(sizeof(JSAMPROW) * h, 8); for (JDIMENSION i = 0; i < h; i++) a[i] = (JSAMPROW)A(w, 8); return a;
}
static jvirt_sarray_ptr __cdecl mock_dummy(void) { return 0; }
static JSAMPARRAY __cdecl mock_access_virt(j_common_ptr, jvirt_sarray_ptr v, JDIMENSION a, JDIMENSION b, boolean w) { return (JSAMPARRAY)(uintptr_t)(0x30000000u + a * 64u + b * 4u + (w ? 1u : 0u)); }
static jpeg_memory_mgr g_mem; static jpeg_error_mgr g_errmgr;
static void init_mocks() {
    memset(&g_mem, 0, sizeof g_mem); g_mem.alloc_small = mock_alloc_small; g_mem.alloc_large = mock_alloc_large; g_mem.alloc_sarray = mock_alloc_sarray; g_mem.access_virt_sarray = mock_access_virt;
    memset(&g_errmgr, 0, sizeof g_errmgr); g_errmgr.error_exit = mock_error_exit; g_errmgr.emit_message = mock_emit;
}

// ---------------------------------------------------------------- casos
typedef std::mt19937 Rng;
struct Mask { size_t off, len; };
struct Ctx {
    std::function<int(void*)> call;            // llama a la funcion (puntero) con los argumentos del caso
    std::function<void(bool)> pre;             // antes de cada llamada (true = EXE): fija punteros a funcion segun el origen en el arena; puede ser vacio
    std::function<void(std::vector<uint8_t>&)> dump; // traza para el oraculo (entradas + salidas)
    std::vector<Mask> masks;
    struct FpSite { size_t off; std::vector<std::pair<uint32_t, std::pair<const char*, const char*>>> map; }; // puntero a funcion en el arena: direccion del EXE <-> (archivo, funcion) de la biblioteca
    std::vector<FpSite> fps;
    std::function<void(std::vector<uint8_t>&)> norm;  // normaliza la instantanea de cada lado antes de comparar (bytes indeterminados de la biblioteca original)
    bool check_ret = false;
    uint32_t ret_mask = 0xFFFFFFFFu;   // boolean = unsigned char en el EXE y en la biblioteca: solo cuenta AL
};
struct Test {
    const char* name; uint32_t exe_addr; const char* lib_file; const char* lib_name;
    std::function<void(Rng&, Ctx&)> build;
    int trace_cases = 1 << 30;                 // cuantos casos escriben traza
};
static std::vector<Test> g_tests; static int g_opt_dct = -1;

static uint32_t R(Rng& r, uint32_t lo, uint32_t hi) { return lo + (uint32_t)(r() % (hi - lo + 1)); }
static uint8_t RB(Rng& r) { uint32_t k = r() & 15; if (k == 0) return 0; if (k == 1) return 255; if (k == 2) return 128; if (k == 3) return (uint8_t)(r() & 1 ? 127 : 129); return (uint8_t)r(); }
static void put32(std::vector<uint8_t>& v, uint32_t x) { for (int i = 0; i < 4; i++) v.push_back((uint8_t)(x >> (8 * i))); }
static void putn(std::vector<uint8_t>& v, const void* p, size_t n) { v.insert(v.end(), (const uint8_t*)p, (const uint8_t*)p + n); }

static JSAMPARRAY make_rows(Rng& r, uint32_t nrows, uint32_t w, uint32_t slack, bool rnd) {
    JSAMPARRAY a = AT<JSAMPROW>(nrows);
    for (uint32_t i = 0; i < nrows; i++) { a[i] = (JSAMPROW)A(w + slack + 8, 8); for (uint32_t x = 0; x < w + slack + 8; x++) a[i][x] = rnd ? RB(r) : (uint8_t)r(); }
    return a;
}

// ---- compresion: conversion de color (jccolor.c)
struct my_cconvert { struct jpeg_color_converter pub; INT32* rgb_ycc_tab; };
static void* lib(const char* f, const char* n) { void* p = jpeg6b_find(f, n); if (!p) { printf("falta %s:%s\n", f, n); exit(2); } return p; }

static jpeg_compress_struct* new_cinfo() {
    auto c = AT<jpeg_compress_struct>(); memset(c, 0, sizeof *c); c->err = &g_errmgr; c->mem = &g_mem; return c;
}
static jpeg_decompress_struct* new_dinfo() {
    auto c = AT<jpeg_decompress_struct>(); memset(c, 0, sizeof *c); c->err = &g_errmgr; c->mem = &g_mem; c->is_decompressor = TRUE; return c;
}
typedef void(__cdecl* t_cvt)(j_compress_ptr, JSAMPARRAY, JSAMPIMAGE, JDIMENSION, int);

static void add_color_tests() {
    g_tests.push_back({"rgb_ycc_start", 0x5C31E0, "jccolor", "rgb_ycc_start", [](Rng& r, Ctx& x) {
        auto c = new_cinfo(); auto cv = AT<my_cconvert>(); memset(cv, 0, sizeof *cv); c->cconvert = &cv->pub;
        x.call = [c](void* f) { ((void(__cdecl*)(j_compress_ptr))f)(c); return 0; };
        x.dump = [cv](std::vector<uint8_t>& v) { putn(v, cv->rgb_ycc_tab, 2048 * 4); };
    }, 4});
    struct CT { const char* name; uint32_t addr; const char* lib; int in_comp, out_comp; };
    CT cts[] = {{"rgb_ycc_convert", 0x5C32C0, "rgb_ycc_convert", RGB_PIXELSIZE, 3}, {"rgb_gray_convert", 0x5C33E0, "rgb_gray_convert", RGB_PIXELSIZE, 1}, {"cmyk_ycck_convert", 0x5C3490, "cmyk_ycck_convert", 4, 4},
                {"grayscale_convert", 0x5C35F0, "grayscale_convert", 0, 1}, {"null_convert", 0x5C3660, "null_convert", 0, 0}};
    for (auto& ct : cts) {
        CT t = ct;
        g_tests.push_back({t.name, t.addr, "jccolor", t.lib, [t](Rng& r, Ctx& x) {
            auto c = new_cinfo(); auto cv = AT<my_cconvert>(); memset(cv, 0, sizeof *cv); c->cconvert = &cv->pub;
            int nin = t.in_comp ? t.in_comp : (int)R(r, 1, 4), nout = t.out_comp ? t.out_comp : nin;
            if (t.out_comp == 0) nout = nin;
            c->input_components = nin; c->num_components = nout; c->image_width = R(r, 1, 96);
            if (t.in_comp >= 3) ((void(__cdecl*)(j_compress_ptr))lib("jccolor", "rgb_ycc_start"))(c); // llena la tabla (la misma en los dos lados); cmyk usa la misma
            uint32_t W = c->image_width, rows = R(r, 1, 4), orow = R(r, 0, 3);
            JSAMPARRAY in = make_rows(r, rows, W * nin, 0, true);
            JSAMPARRAY* ob = AT<JSAMPARRAY>(nout);
            for (int ci = 0; ci < nout; ci++) ob[ci] = make_rows(r, orow + rows, W, 0, false);
            x.call = [=](void* f) { ((t_cvt)f)(c, in, ob, orow, (int)rows); return 0; };
            x.dump = [=](std::vector<uint8_t>& v) {
                put32(v, W); put32(v, rows); put32(v, orow); put32(v, nin); put32(v, nout);
                for (uint32_t i = 0; i < rows; i++) putn(v, in[i], W * nin);
                for (int ci = 0; ci < nout; ci++) for (uint32_t i = 0; i < rows; i++) putn(v, ob[ci][orow + i], W);
            };
        }});
    }
    g_tests.push_back({"null_method", 0x5C36F0, "jccolor", "null_method", [](Rng& r, Ctx& x) {
        auto c = new_cinfo(); x.call = [c](void* f) { ((void(__cdecl*)(j_compress_ptr))f)(c); return 0; };
        x.dump = [](std::vector<uint8_t>&) {};
    }, 1});
}

// ---- compresion: DCT (jcdctmgr.c, jfdctflt.c)
struct my_fdct { struct jpeg_forward_dct pub; void* do_dct; DCTELEM* divisors[NUM_QUANT_TBLS]; void* do_float_dct; FAST_FLOAT* float_divisors[NUM_QUANT_TBLS]; };
static void add_dct_tests() {
    g_tests.push_back({"start_pass_fdctmgr", 0x5C3940, "jcdctmgr", "start_pass_fdctmgr", [](Rng& r, Ctx& x) {
        auto c = new_cinfo(); auto fd = AT<my_fdct>(); memset(fd, 0, sizeof *fd); c->fdct = &fd->pub;
        c->num_components = R(r, 1, 4); c->comp_info = AT<jpeg_component_info>(4); memset(c->comp_info, 0, sizeof(jpeg_component_info) * 4);
        c->dct_method = g_opt_dct >= 0 ? (J_DCT_METHOD)g_opt_dct : (r() % 8 == 0 ? (J_DCT_METHOD)R(r, 0, 1) : JDCT_FLOAT); // solo el flotante esta compilado en el EXE; 1/8 de casos con los otros (error)
        bool have[4]; for (int i = 0; i < 4; i++) { have[i] = (r() % 24) != 0; if (have[i]) { auto q = AT<JQUANT_TBL>(); c->quant_tbl_ptrs[i] = q; for (int k = 0; k < 64; k++) q->quantval[k] = (UINT16)((r() % 5 == 0) ? R(r, 1, 255) : R(r, 1, 120)); q->sent_table = 0; } }
        for (int i = 0; i < 4; i++) c->comp_info[i].quant_tbl_no = (r() % 20 == 0) ? (int)R(r, 4, 6) : (int)R(r, 0, 3);
        x.call = [c](void* f) { ((void(__cdecl*)(j_compress_ptr))f)(c); return 0; };
        x.dump = [c, fd](std::vector<uint8_t>& v) {
            put32(v, c->num_components); put32(v, c->dct_method); put32(v, g_err_calls);
            for (int i = 0; i < 4; i++) put32(v, c->comp_info[i].quant_tbl_no);
            for (int i = 0; i < 4; i++) { put32(v, c->quant_tbl_ptrs[i] != 0); if (c->quant_tbl_ptrs[i]) putn(v, c->quant_tbl_ptrs[i]->quantval, 128); }
            for (int i = 0; i < 4; i++) { put32(v, fd->divisors[i] != 0); if (fd->divisors[i]) putn(v, fd->divisors[i], 256); put32(v, fd->float_divisors[i] != 0); if (fd->float_divisors[i]) putn(v, fd->float_divisors[i], 256); }
        };
    }});
    g_tests.push_back({"jpeg_fdct_float", 0x5CD860, "jfdctflt", "jpeg_fdct_float", [](Rng& r, Ctx& x) {
        auto d = AT<FAST_FLOAT>(64); int mode = (int)R(r, 0, 3);
        for (int i = 0; i < 64; i++) d[i] = mode == 0 ? (float)((int)(r() % 256) - 128) : mode == 1 ? (float)((int)RB(r) - 128) : mode == 2 ? (float)((int)(r() % 31) - 15) : (float)((double)(int)(r() % 40001) / 100.0 - 200.0);
        std::vector<float> in(d, d + 64);
        x.call = [d](void* f) { ((void(__cdecl*)(FAST_FLOAT*))f)(d); return 0; };
        x.dump = [d, in](std::vector<uint8_t>& v) { putn(v, in.data(), 256); putn(v, d, 256); };
    }, 1500});
    // forward_DCT_float: el puntero do_float_dct es el del origen (EXE o LIB); enmascarado
    g_tests.push_back({"forward_DCT_float", 0x5C3B70, "jcdctmgr", "forward_DCT_float", [](Rng& r, Ctx& x) {
        auto c = new_cinfo(); auto fd = AT<my_fdct>(); memset(fd, 0, sizeof *fd); c->fdct = &fd->pub;
        c->num_components = 1; c->comp_info = AT<jpeg_component_info>(1); memset(c->comp_info, 0, sizeof(jpeg_component_info)); c->dct_method = JDCT_FLOAT;
        auto q = AT<JQUANT_TBL>(); c->quant_tbl_ptrs[0] = q; for (int k = 0; k < 64; k++) q->quantval[k] = (UINT16)((r() % 3 == 0) ? R(r, 1, 255) : R(r, 1, 40));
        ((void(__cdecl*)(j_compress_ptr))lib("jcdctmgr", "start_pass_fdctmgr"))(c);
        uint32_t nb = R(r, 1, 3), srow = R(r, 0, 3), scol = R(r, 0, 2) * 8;
        JSAMPARRAY sd = make_rows(r, srow + 8, scol + nb * 8, 0, true); JBLOCKROW out = AT<JBLOCK>(nb); memset(out, 0xA5, sizeof(JBLOCK) * nb);
        auto cp = c->comp_info;
        x.pre = [fd](bool exe) { fd->do_float_dct = exe ? (void*)0x5CD860 : lib("jfdctflt", "jpeg_fdct_float"); };
        x.masks.push_back({(size_t)((uint8_t*)&fd->do_float_dct - g_arena), 4});
        x.call = [=](void* f) { ((void(__cdecl*)(j_compress_ptr, jpeg_component_info*, JSAMPARRAY, JBLOCKROW, JDIMENSION, JDIMENSION, JDIMENSION))f)(c, cp, sd, out, srow, scol, nb); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) {
            put32(v, nb); putn(v, q->quantval, 128);
            for (uint32_t b = 0; b < nb; b++) for (int y = 0; y < 8; y++) putn(v, sd[srow + y] + scol + b * 8, 8);
            putn(v, out, 128 * nb);
        };
    }, 1500});
}

// ---- descompresion: sobremuestreo (jdsample.c)
struct my_upsampler { struct jpeg_upsampler pub; JSAMPARRAY color_buf[MAX_COMPONENTS]; void* methods[MAX_COMPONENTS]; int next_row_out; JDIMENSION rows_to_go; int rowgroup_height[MAX_COMPONENTS]; UINT8 h_expand[MAX_COMPONENTS]; UINT8 v_expand[MAX_COMPONENTS]; };
typedef void(__cdecl* t_up)(j_decompress_ptr, jpeg_component_info*, JSAMPARRAY, JSAMPARRAY*);
static void add_upsample_tests() {
    g_tests.push_back({"start_pass_upsample", 0x5CCE60, "jdsample", "start_pass_upsample", [](Rng& r, Ctx& x) {
        auto c = new_dinfo(); auto u = AT<my_upsampler>(); memset(u, 0xAB, sizeof *u); c->upsample = &u->pub; c->max_v_samp_factor = R(r, 1, 4); c->output_height = r();
        x.call = [c](void* f) { ((void(__cdecl*)(j_decompress_ptr))f)(c); return 0; };
        x.dump = [c, u](std::vector<uint8_t>& v) { put32(v, c->max_v_samp_factor); put32(v, c->output_height); put32(v, u->next_row_out); put32(v, u->rows_to_go); };
    }, 1 << 30});
    struct UT { const char* name; uint32_t addr; const char* lib; int kind; };
    UT uts[] = {{"fullsize_upsample", 0x5CCF60, "fullsize_upsample", 0}, {"noop_upsample", 0x5CCF70, "noop_upsample", 1}, {"int_upsample", 0x5CCF80, "int_upsample", 2}, {"h2v1_upsample", 0x5CD080, "h2v1_upsample", 3},
                {"h2v2_upsample", 0x5CD0E0, "h2v2_upsample", 4}, {"h2v1_fancy_upsample", 0x5CD160, "h2v1_fancy_upsample", 5}, {"h2v2_fancy_upsample", 0x5CD220, "h2v2_fancy_upsample", 6}};
    for (auto& ut : uts) {
        UT t = ut;
        g_tests.push_back({t.name, t.addr, "jdsample", t.lib, [t](Rng& r, Ctx& x) {
            auto c = new_dinfo(); auto u = AT<my_upsampler>(); memset(u, 0, sizeof *u); c->upsample = &u->pub;
            auto cp = AT<jpeg_component_info>(); memset(cp, 0, sizeof *cp); int ci = (int)R(r, 0, 3); cp->component_index = ci;
            uint32_t W = R(r, 1, 60), hx = 1, vx = 1, maxv = 1;
            switch (t.kind) {
            case 0: case 1: maxv = R(r, 1, 4); break;
            case 2: hx = R(r, 1, 4); vx = R(r, 1, 4); maxv = vx * R(r, 1, 2); break;
            case 3: maxv = R(r, 1, 3); break;
            case 4: maxv = 2 * R(r, 1, 2); break;
            case 5: maxv = R(r, 1, 3); break;
            case 6: maxv = 2 * R(r, 1, 2); break;
            }
            c->output_width = W; c->max_v_samp_factor = (int)maxv; u->h_expand[ci] = (UINT8)hx; u->v_expand[ci] = (UINT8)vx;
            uint32_t D = (t.kind >= 3) ? (W + 1) / 2 : (W + hx - 1) / hx; if (t.kind == 5 || t.kind == 6) { D = R(r, 3, 40); W = 2 * D; c->output_width = W; }
            cp->downsampled_width = D;
            uint32_t inrows = (t.kind == 2) ? (maxv + vx - 1) / vx : (t.kind == 4 || t.kind == 6) ? maxv / 2 : maxv;
            uint32_t ctxrows = (t.kind == 6) ? 1 : 0;
            JSAMPARRAY inb = make_rows(r, inrows + 2 * ctxrows, D, 4, true); JSAMPARRAY in = inb + ctxrows;
            JSAMPARRAY outp = nullptr; JSAMPARRAY* pout = AT<JSAMPARRAY>();
            *pout = (t.kind <= 1) ? (JSAMPARRAY)0xDEADBEEF : (JSAMPARRAY)0; // el valor inicial importa solo para noop
            if (t.kind >= 2) { outp = make_rows(r, maxv, W, 4 + hx, false); *pout = outp; }
            x.call = [=](void* f) { ((t_up)f)(c, cp, in, pout); return 0; };
            x.dump = [=](std::vector<uint8_t>& v) {
                put32(v, t.kind); put32(v, W); put32(v, D); put32(v, maxv); put32(v, hx); put32(v, vx); put32(v, inrows); put32(v, ctxrows);
                for (uint32_t i = 0; i < inrows + 2 * ctxrows; i++) putn(v, inb[i], D);
                put32(v, (uint32_t)(*pout == in)); put32(v, (uint32_t)(*pout == 0));
                if (t.kind >= 2) for (uint32_t i = 0; i < maxv; i++) putn(v, outp[i], W);
            };
        }});
    }
}

// ---- descompresion: conversion de color (jdcolor.c)
struct my_dconvert { struct jpeg_color_deconverter pub; int* Cr_r_tab; int* Cb_b_tab; INT32* Cr_g_tab; INT32* Cb_g_tab; };
typedef void(__cdecl* t_dcvt)(j_decompress_ptr, JSAMPIMAGE, JDIMENSION, JSAMPARRAY, int);
static void make_range_limit(j_decompress_ptr c) {
    JSAMPLE* table = (JSAMPLE*)A(5 * (MAXJSAMPLE + 1) + CENTERJSAMPLE, 8);
    table += (MAXJSAMPLE + 1); c->sample_range_limit = table;
    memset(table - (MAXJSAMPLE + 1), 0, MAXJSAMPLE + 1); for (int i = 0; i <= MAXJSAMPLE; i++) table[i] = (JSAMPLE)i;
    table += CENTERJSAMPLE; for (int i = CENTERJSAMPLE; i < 2 * (MAXJSAMPLE + 1); i++) table[i] = MAXJSAMPLE;
    memset(table + 2 * (MAXJSAMPLE + 1), 0, 2 * (MAXJSAMPLE + 1) - CENTERJSAMPLE); memcpy(table + 4 * (MAXJSAMPLE + 1) - CENTERJSAMPLE, c->sample_range_limit, CENTERJSAMPLE);
}
static void add_dcolor_tests(uint32_t a_ycc, uint32_t a_null, uint32_t a_gray, uint32_t a_grgb, uint32_t a_ycck, uint32_t a_start, uint32_t a_build) {
    struct DT { const char* name; uint32_t addr; const char* lib; int kind; };
    DT dts[] = {{"ycc_rgb_convert", a_ycc, "ycc_rgb_convert", 0}, {"dc_null_convert", a_null, "null_convert", 1}, {"dc_grayscale_convert", a_gray, "grayscale_convert", 2}, {"gray_rgb_convert", a_grgb, "gray_rgb_convert", 3},
                {"ycck_cmyk_convert", a_ycck, "ycck_cmyk_convert", 4}};
    if (a_start) g_tests.push_back({"start_pass_dcolor", a_start, "jdcolor", "start_pass_dcolor", [](Rng& r, Ctx& x) {
        auto c = new_dinfo(); x.call = [c](void* f) { ((void(__cdecl*)(j_decompress_ptr))f)(c); return 0; }; x.dump = [](std::vector<uint8_t>&) {};
    }, 1});
    for (auto& dt : dts) {
        DT t = dt; if (!t.addr) continue;
        g_tests.push_back({t.name, t.addr, "jdcolor", t.lib, [t](Rng& r, Ctx& x) {
            auto c = new_dinfo(); auto cv = AT<my_dconvert>(); memset(cv, 0, sizeof *cv); c->cconvert = &cv->pub; make_range_limit(c);
            uint32_t W = R(r, 1, 96), rows = R(r, 1, 4), irow = R(r, 0, 3); c->output_width = W;
            int nin = t.kind == 0 ? 3 : t.kind == 1 ? (int)R(r, 1, 4) : t.kind == 2 ? 1 : t.kind == 3 ? 1 : 4; int nout = t.kind == 0 ? 3 : t.kind == 1 ? nin : t.kind == 2 ? 1 : t.kind == 3 ? 3 : 4; int obpp = (t.kind == 0 || t.kind == 3) ? RGB_PIXELSIZE : nout;
            c->num_components = nin; c->out_color_components = nout;
            if (t.kind == 0 || t.kind == 4) { // build_ycc_rgb_table (de la biblioteca: es la misma en los dos lados)
                ((void(__cdecl*)(j_decompress_ptr))lib("jdcolor", "build_ycc_rgb_table"))(c); }
            JSAMPIMAGE ib = (JSAMPIMAGE)AT<JSAMPARRAY>(nin); for (int ci = 0; ci < nin; ci++) ib[ci] = make_rows(r, irow + rows, W, 0, true);
            JSAMPARRAY ob = make_rows(r, rows, W * obpp, 0, false);
            x.call = [=](void* f) { ((t_dcvt)f)(c, ib, irow, ob, (int)rows); return 0; };
            x.dump = [=](std::vector<uint8_t>& v) {
                put32(v, t.kind); put32(v, W); put32(v, rows); put32(v, irow); put32(v, nin); put32(v, nout); put32(v, obpp);
                for (int ci = 0; ci < nin; ci++) for (uint32_t i = 0; i < rows; i++) putn(v, ib[ci][irow + i], W);
                for (uint32_t i = 0; i < rows; i++) putn(v, ob[i], W * obpp);
            };
        }});
    }
}


// ---------------------------------------------------------------- LIB3B
static float f_special(Rng& r) {
    switch (r() % 12) {
    case 0: return 0.0f; case 1: return -0.0f; case 2: { uint32_t b = 0x7FC00000u | (r() & 0x3FFFFF); float f; memcpy(&f, &b, 4); return f; }
    case 3: return (r() & 1) ? INFINITY : -INFINITY; case 4: { uint32_t b = r() & 0x807FFFFFu; float f; memcpy(&f, &b, 4); return f; } // denormal
    case 5: return 1e30f; case 6: return -1e30f;
    default: return (float)((double)(int)(r() % 2000001) / 1000.0 - 1000.0);
    }
}
static const double aanscale[8] = {1.0, 1.387039845, 1.306562965, 1.175875602, 1.0, 0.785694958, 0.541196100, 0.275899379};
static void add_idct_tests() {
    g_tests.push_back({"jpeg_idct_float", 0x5CDA90, "jidctflt", "jpeg_idct_float", [](Rng& r, Ctx& x) {
        auto c = new_dinfo(); make_range_limit(c);
        auto cp = AT<jpeg_component_info>(); memset(cp, 0, sizeof *cp);
        int mode = (int)R(r, 0, 5); // 0-4: tabla de cuantizacion realista (oraculo); 5: tabla con NaN/inf/denormales (solo EXE vs biblioteca)
        FAST_FLOAT* tab = AT<FAST_FLOAT>(64); cp->dct_table = tab; uint16_t q[64];
        for (int i = 0; i < 64; i++) {
            q[i] = (uint16_t)((r() % 5 == 0) ? R(r, 1, 255) : R(r, 1, 40));
            tab[i] = mode == 5 ? f_special(r) : (FAST_FLOAT)((double)q[i] * aanscale[i / 8] * aanscale[i % 8]);
        }
        JCOEF* coef = AT<JCOEF>(64); memset(coef, 0, 128);
        int cm = mode == 5 ? (int)R(r, 0, 3) : mode;
        switch (cm) {
        case 0: coef[0] = (JCOEF)((int)R(r, 0, 2000) - 1000); break;                                    // solo DC (atajo de columnas nulas)
        case 1: { int nc = R(r, 1, 3); for (int k = 0; k < nc; k++) coef[(r() % 8) * 8 + (r() % 3)] = (JCOEF)((int)R(r, 0, 800) - 400); break; } // pocas columnas
        case 2: { int nz = R(r, 1, 12); for (int k = 0; k < nz; k++) coef[R(r, 0, 63)] = (JCOEF)((int)R(r, 0, 400) - 200); break; }
        case 3: for (int i = 0; i < 64; i++) coef[i] = (JCOEF)((int)R(r, 0, 60) - 30); break;
        default: for (int i = 0; i < 64; i++) coef[i] = (r() % 4) ? (JCOEF)((int)R(r, 0, 24) - 12) : (JCOEF)(int16_t)r(); break;       // con valores extremos de 16 bits
        }
        if (mode < 4) for (int i = 0; i < 64; i++) { int lim = std::max(1, 160 / (int)q[i]); if (coef[i] > lim) coef[i] = (JCOEF)lim; if (coef[i] < -lim) coef[i] = (JCOEF)-lim; } // dentro del dominio de la tabla de recorte (oraculo)
        // algunas filas de coeficientes nulas (rama de ceros por columna: ramas 1..7 en cero)
        if (r() % 3 == 0) { int col = R(r, 0, 7); for (int y = 1; y < 8; y++) coef[y * 8 + col] = 0; }
        uint32_t ocol = R(r, 0, 3);
        JSAMPARRAY ob = make_rows(r, 8, ocol + 8, 4, false);
        x.call = [=](void* f) { ((void(__cdecl*)(j_decompress_ptr, jpeg_component_info*, JCOEFPTR, JSAMPARRAY, JDIMENSION))f)(c, cp, coef, ob, ocol); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) {
            put32(v, mode); putn(v, q, 128); putn(v, tab, 256); putn(v, coef, 128);
            for (int y = 0; y < 8; y++) putn(v, ob[y] + ocol, 8);
        };
    }, 4000});
}


// ---- helpers LIB3B
#define W32(p, off) (*(uint32_t*)((uint8_t*)(p) + (off)))
static void randwords(Rng& r, void* p, size_t n) { for (size_t i = 0; i + 4 <= n; i += 4) { uint32_t v = (r() % 3 == 0) ? 0 : r(); memcpy((uint8_t*)p + i, &v, 4); } }
static void* priv(Rng& r, size_t n) { void* p = A(n, 8); randwords(r, p, n); return p; }  // estructura privada con palabras aleatorias (1/3 a cero)
static size_t aoff(const void* p) { return (size_t)((const uint8_t*)p - g_arena); }
static j_compress_ptr rand_cinfo(Rng& r) { auto c = new_cinfo(); jpeg_error_mgr* e = (jpeg_error_mgr*)c->err; jpeg_memory_mgr* m = c->mem; randwords(r, c, sizeof *c); c->err = e; c->mem = m; c->is_decompressor = FALSE; return c; }
static j_decompress_ptr rand_dinfo(Rng& r) { auto c = new_dinfo(); jpeg_error_mgr* e = (jpeg_error_mgr*)c->err; jpeg_memory_mgr* m = c->mem; randwords(r, c, sizeof *c); c->err = e; c->mem = m; c->is_decompressor = TRUE; return c; }
static int __cdecl mock_sprintf(char* b, const char* f, ...) { va_list a; va_start(a, f); int n = vsprintf(b, f, a); va_end(a); return n; }
static void patch_call(uint32_t at, void* target) { uint8_t* p = (uint8_t*)at; if (p[0] != 0xE8) { printf("patch_call: 0x%X no es un call\n", at); exit(2); } *(int32_t*)(p + 1) = (int32_t)((uintptr_t)target - (at + 5)); }
static void __cdecl mock_output_message(j_common_ptr c) { c->err->msg_parm.i[7]++; }
static void __cdecl mock_finish_pass_cq(j_decompress_ptr c) { W32(c->cquantize, 0x10)++; }
static void __cdecl mock_finish_pass_entropy(j_compress_ptr c) { W32(c->entropy, 0x20)++; }
static boolean g_mock_flag = TRUE;
static boolean __cdecl mock_empty_output(j_compress_ptr c) { auto d = c->dest; d->next_output_byte = (JOCTET*)(((uint8_t*)d) + 0x40); d->free_in_buffer = 7; W32(d, 0x1C)++; return g_mock_flag; }
typedef Ctx::FpSite FpS;
static jpeg_error_mgr* own_err(j_common_ptr c) { auto e = AT<jpeg_error_mgr>(); *e = g_errmgr; c->err = e; return e; }
static const char* const g_msgs[] = {"<tabla 0: codigo %d>", "plain text", "one %d", "a=%d b=%d c=%d", "str=%s end", "%d%d%d%d%d%d%d%d", "pct 100%% sure %d", "hex %x %X", "tail %d%%", "%s", "", "%5d|%-5d|%05d", "%c%c", "just%%"};
static const char* copy_msg(const char* m) { size_t L = strlen(m) + 1; char* d = (char*)A(L, 1); memcpy(d, m, L); return d; }

static void add_state_tests() {
    // ---- jerror
    g_tests.push_back({"emit_message", 0x5CD700, "jerror", "emit_message", [](Rng& r, Ctx& x) {
        auto c = new_dinfo(); auto e = own_err((j_common_ptr)c); e->output_message = mock_output_message;
        e->trace_level = (int)R(r, 0, 5); e->num_warnings = (long)R(r, 0, 3); int lvl = (int)R(r, 0, 8) - 4; e->msg_parm.i[7] = 0; long nw0 = e->num_warnings;
        x.call = [=](void* f) { ((void(__cdecl*)(j_common_ptr, int))f)((j_common_ptr)c, lvl); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) { put32(v, (uint32_t)lvl); put32(v, (uint32_t)e->trace_level); put32(v, (uint32_t)nw0); put32(v, (uint32_t)e->num_warnings); put32(v, (uint32_t)e->msg_parm.i[7]); };
    }});
    g_tests.push_back({"reset_error_mgr", 0x5CD7F0, "jerror", "reset_error_mgr", [](Rng& r, Ctx& x) {
        auto c = new_dinfo(); auto e = own_err((j_common_ptr)c); e->num_warnings = (long)r(); e->msg_code = (int)r();
        x.call = [=](void* f) { ((void(__cdecl*)(j_common_ptr))f)((j_common_ptr)c); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) { put32(v, (uint32_t)e->num_warnings); put32(v, (uint32_t)e->msg_code); };
    }, 2});
    g_tests.push_back({"format_message", 0x5CD740, "jerror", "format_message", [](Rng& r, Ctx& x) {
        auto c = new_dinfo(); auto e = own_err((j_common_ptr)c);
        int nm = (int)R(r, 3, 15); const char** tab = AT<const char*>(nm + 4); for (int i = 0; i < nm; i++) tab[i] = copy_msg(g_msgs[(i == 0) ? 0 : R(r, 1, 13)]);
        int na = (int)R(r, 1, 5); const char** add = AT<const char*>(na + 1); for (int i = 0; i < na; i++) add[i] = copy_msg(g_msgs[R(r, 1, 13)]);
        if (r() % 6 == 0) add[R(r, 0, na - 1)] = nullptr;
        e->jpeg_message_table = tab; e->last_jpeg_message = (int)R(r, 0, nm - 1); e->addon_message_table = (r() % 5 == 0) ? nullptr : add; e->first_addon_message = (int)R(r, 1, 5) + e->last_jpeg_message; e->last_addon_message = e->first_addon_message + na - 1;
        e->msg_code = (int)R(r, 0, (uint32_t)(e->last_addon_message + 3)); if (r() % 8 == 0) e->msg_code = -(int)R(r, 0, 3);
        for (int i = 0; i < 8; i++) e->msg_parm.i[i] = (r() % 4 == 0) ? (int)r() : (int)R(r, 0, 300) - 50;
        if (r() % 3 == 0) { for (int i = 0; i < 79; i++) e->msg_parm.s[i] = (char)('a' + r() % 26); e->msg_parm.s[R(r, 1, 78)] = 0; e->msg_parm.s[79] = 0; }
        char* buf = (char*)A(256, 8); memset(buf, 0x7E, 256); int code = e->msg_code;
        x.call = [=](void* f) { ((void(__cdecl*)(j_common_ptr, char*))f)((j_common_ptr)c, buf); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) {
            put32(v, nm); put32(v, (uint32_t)e->last_jpeg_message); put32(v, na); put32(v, e->addon_message_table != nullptr); put32(v, (uint32_t)e->first_addon_message); put32(v, (uint32_t)e->last_addon_message); put32(v, (uint32_t)code);
            for (int i = 0; i < nm; i++) { put32(v, (uint32_t)strlen(tab[i])); putn(v, tab[i], strlen(tab[i])); }
            for (int i = 0; i < na; i++) { put32(v, add[i] ? (uint32_t)strlen(add[i]) : 0xFFFFFFFFu); if (add[i]) putn(v, add[i], strlen(add[i])); }
            putn(v, e->msg_parm.i, 32); putn(v, e->msg_parm.s, 80); put32(v, (uint32_t)strlen(buf)); putn(v, buf, strlen(buf));
        };
    }});
    // ---- jdmaster / jdmarker / jdinput / jdpostct / jdmainct / jdcoefct
    g_tests.push_back({"finish_output_pass", 0x5CCC90, "jdmaster", "finish_output_pass", [](Rng& r, Ctx& x) {
        auto c = rand_dinfo(r); void* m = priv(r, 0x40); c->master = (struct jpeg_decomp_master*)m; void* cq = A(0x40, 8); memset(cq, 0, 0x40); c->cquantize = (struct jpeg_color_quantizer*)cq; c->cquantize->finish_pass = mock_finish_pass_cq;
        c->quantize_colors = (boolean)(r() & 1); uint32_t pn0 = W32(m, 0xC);
        x.call = [=](void* f) { ((void(__cdecl*)(j_decompress_ptr))f)(c); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) { put32(v, c->quantize_colors); put32(v, pn0); put32(v, W32(m, 0xC)); put32(v, W32(cq, 0x10)); };
    }});
    g_tests.push_back({"reset_marker_reader", 0x5CC7A0, "jdmarker", "reset_marker_reader", [](Rng& r, Ctx& x) {
        auto c = rand_dinfo(r); void* m = priv(r, 0x100); c->marker = (struct jpeg_marker_reader*)m;
        x.call = [=](void* f) { ((void(__cdecl*)(j_decompress_ptr))f)(c); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) { put32(v, (uint32_t)(uintptr_t)c->comp_info); put32(v, (uint32_t)c->input_scan_number); put32(v, (uint32_t)c->unread_marker); put32(v, W32(m, 0xC)); put32(v, W32(m, 0x14)); };
    }});
    g_tests.push_back({"finish_input_pass", 0x5CA820, "jdinput", "finish_input_pass", [](Rng& r, Ctx& x) {
        auto c = rand_dinfo(r); void* ic = priv(r, 0x40); c->inputctl = (struct jpeg_input_controller*)ic;
        x.fps.push_back({aoff(ic), {{0x5CA720, {"jdinput", "consume_markers"}}}});
        x.call = [=](void* f) { ((void(__cdecl*)(j_decompress_ptr))f)(c); return 0; };
        x.dump = [](std::vector<uint8_t>&) {};
    }, 1});
    g_tests.push_back({"start_pass_dpost", 0x5CCD70, "jdpostct", "start_pass_dpost", [](Rng& r, Ctx& x) {
        auto c = rand_dinfo(r); c->mem = &g_mem; void* po = priv(r, 0x40); c->post = (struct jpeg_d_post_controller*)po; void* up = priv(r, 0x20); c->upsample = (struct jpeg_upsampler*)up;
        c->quantize_colors = (boolean)(r() & 1); int mode = (r() % 4) ? 0 : (int)R(r, 0, 5) - 1;
        x.fps.push_back({aoff(po) + 4, {{0x5CCCF0, {"jdpostct", "post_process_1pass"}}}});
        x.call = [=](void* f) { ((void(__cdecl*)(j_decompress_ptr, int))f)(c, mode); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) { put32(v, (uint32_t)mode); put32(v, c->quantize_colors); put32(v, W32(po, 0x14)); put32(v, W32(po, 0x18)); };
    }});
    g_tests.push_back({"start_pass_main_d", 0x5CADC0, "jdmainct", "start_pass_main", [](Rng& r, Ctx& x) {
        auto c = rand_dinfo(r); int nc = (int)R(r, 1, 4); int M = 1 << R(r, 0, 3); c->num_components = nc; c->min_DCT_scaled_size = M;
        auto ci = AT<jpeg_component_info>(4); memset(ci, 0, sizeof(jpeg_component_info) * 4); c->comp_info = ci;
        void* mn = A(0x40, 8); memset(mn, 0xAB, 0x40); c->main = (struct jpeg_d_main_controller*)mn; void* up = priv(r, 0x20); c->upsample = (struct jpeg_upsampler*)up; ((uint8_t*)up)[8] = (uint8_t)(r() & 1);
        for (int i = 0; i < nc; i++) { ci[i].v_samp_factor = (int)R(r, 1, 4); ci[i].DCT_scaled_size = M * (int)R(r, 1, 2); int rg = ci[i].v_samp_factor * ci[i].DCT_scaled_size / M; JSAMPROW* b = AT<JSAMPROW>(rg * (M + 2)); for (int k = 0; k < rg * (M + 2); k++) b[k] = (JSAMPROW)(uintptr_t)r(); W32(mn, 8 + 4 * i) = (uint32_t)(uintptr_t)b; }
        ((void(__cdecl*)(j_decompress_ptr))lib("jdmainct", "alloc_funny_pointers"))(c);
        int mode = (r() % 5 == 0) ? (int)R(r, 1, 3) : 0; if (r() % 12 == 0) mode = 4;
        x.fps.push_back({aoff(mn) + 4, {{0x5CAC80, {"jdmainct", "process_data_context_main"}}, {0x5CAC10, {"jdmainct", "process_data_simple_main"}}}});
        x.call = [=](void* f) { ((void(__cdecl*)(j_decompress_ptr, int))f)(c, mode); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) { put32(v, (uint32_t)mode); put32(v, ((uint8_t*)up)[8]); put32(v, (uint32_t)nc); put32(v, (uint32_t)M); for (int i = 0; i < nc; i++) { put32(v, (uint32_t)ci[i].v_samp_factor); put32(v, (uint32_t)ci[i].DCT_scaled_size); } };
    }});
    g_tests.push_back({"start_input_pass_coef", 0x5C8B40, "jdcoefct", "start_input_pass", [](Rng& r, Ctx& x) {
        auto c = rand_dinfo(r); void* co = priv(r, 0x80); c->coef = (struct jpeg_d_coef_controller*)co;
        auto cc = AT<jpeg_component_info>(); memset(cc, 0, sizeof *cc); cc->v_samp_factor = (int)R(r, 1, 4); cc->last_row_height = (int)R(r, 1, 4); c->cur_comp_info[0] = cc; c->comps_in_scan = (int)R(r, 1, 3); c->total_iMCU_rows = R(r, 0, 5); c->input_iMCU_row = R(r, 0, 5);
        x.call = [=](void* f) { ((void(__cdecl*)(j_decompress_ptr))f)(c); return 0; };
        x.dump = [](std::vector<uint8_t>&) {};
    }, 1});
    g_tests.push_back({"start_output_pass_coef", 0x5C8B60, "jdcoefct", "start_output_pass", [](Rng& r, Ctx& x) {
        auto c = rand_dinfo(r); void* co = priv(r, 0x80); c->coef = (struct jpeg_d_coef_controller*)co; if (r() % 2) c->coef->coef_arrays = nullptr; c->do_block_smoothing = (boolean)(r() & 1);
        x.call = [=](void* f) { ((void(__cdecl*)(j_decompress_ptr))f)(c); return 0; };
        x.dump = [](std::vector<uint8_t>&) {};
    }, 1});
    // ---- compresion: jcmainct / jccoefct / jcprepct / jcmaster / jcmarker
    g_tests.push_back({"start_pass_main_c", 0x5C53D0, "jcmainct", "start_pass_main", [](Rng& r, Ctx& x) {
        auto c = rand_cinfo(r); void* mn = priv(r, 0x40); c->main = (struct jpeg_c_main_controller*)mn; c->raw_data_in = (boolean)(r() % 4 == 0);
        int mode = (r() % 3 == 0) ? (int)R(r, 1, 4) : 0;
        x.fps.push_back({aoff(mn) + 4, {{0x5C5320, {"jcmainct", "process_data_simple_main"}}}});
        x.call = [=](void* f) { ((void(__cdecl*)(j_compress_ptr, int))f)(c, mode); return 0; };
        x.dump = [](std::vector<uint8_t>&) {};
    }});
    g_tests.push_back({"start_pass_coef_c", 0x5C2FF0, "jccoefct", "start_pass_coef", [](Rng& r, Ctx& x) {
        auto c = rand_cinfo(r); void* co = priv(r, 0x80); c->coef = (struct jpeg_c_coef_controller*)co;
        auto cc = AT<jpeg_component_info>(); memset(cc, 0, sizeof *cc); cc->v_samp_factor = (int)R(r, 1, 4); cc->last_row_height = (int)R(r, 1, 4); c->cur_comp_info[0] = cc; c->comps_in_scan = (int)R(r, 1, 3); c->total_iMCU_rows = R(r, 0, 5);
        int mode = (int)R(r, 0, 4); if (r() % 4 == 0) mode = (int)R(r, 0, 3);
        x.fps.push_back({aoff(co) + 4, {{0x5C2910, {"jccoefct", "compress_data"}}, {0x5C2DD0, {"jccoefct", "compress_first_pass"}}, {0x5C2BC0, {"jccoefct", "compress_output"}}}});
        x.call = [=](void* f) { ((void(__cdecl*)(j_compress_ptr, int))f)(c, mode); return 0; };
        x.dump = [](std::vector<uint8_t>&) {};
    }});
    g_tests.push_back({"start_pass_prep", 0x5C76A0, "jcprepct", "start_pass_prep", [](Rng& r, Ctx& x) {
        auto c = rand_cinfo(r); void* pr = priv(r, 0x100); c->prep = (struct jpeg_c_prep_controller*)pr; int mode = (r() % 4 == 0) ? (int)R(r, 1, 4) : 0;
        x.call = [=](void* f) { ((void(__cdecl*)(j_compress_ptr, int))f)(c, mode); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) { put32(v, (uint32_t)mode); put32(v, c->image_height); put32(v, W32(pr, 0x18)); put32(v, W32(pr, 0x1C)); };
    }});
    g_tests.push_back({"finish_pass_master", 0x5C6A10, "jcmaster", "finish_pass_master", [](Rng& r, Ctx& x) {
        auto c = rand_cinfo(r); void* ms = priv(r, 0x40); c->master = (struct jpeg_comp_master*)ms; void* en = A(0x40, 8); memset(en, 0, 0x40); c->entropy = (struct jpeg_entropy_encoder*)en; c->entropy->finish_pass = mock_finish_pass_entropy;
        W32(ms, 0x10) = R(r, 0, 3); W32(ms, 0x14) = R(r, 0, 9); W32(ms, 0x1C) = R(r, 0, 9); c->optimize_coding = (boolean)(r() & 1); uint32_t pt0 = W32(ms, 0x10), pn0 = W32(ms, 0x14), sn0 = W32(ms, 0x1C);
        x.call = [=](void* f) { ((void(__cdecl*)(j_compress_ptr))f)(c); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) { put32(v, pt0); put32(v, pn0); put32(v, sn0); put32(v, c->optimize_coding); put32(v, W32(ms, 0x10)); put32(v, W32(ms, 0x14)); put32(v, W32(ms, 0x1C)); put32(v, W32(en, 0x20)); };
    }});
    auto dest_fixture = [](Rng& r, Ctx& x, int kind) {
        auto c = new_cinfo(); c->image_width = r(); auto d = (jpeg_destination_mgr*)A(0x200, 8); memset(d, 0, 0x200); c->dest = d;
        uint8_t* out = (uint8_t*)A(64, 8); memset(out, 0x55, 64); d->next_output_byte = out + R(r, 0, 8); d->free_in_buffer = R(r, 1, 6); d->empty_output_buffer = mock_empty_output; g_mock_flag = (boolean)(r() % 5 != 0);
        int val = (int)R(r, 0, 255); int mk = (int)R(r, 0xC0, 0xFE); unsigned len = (r() % 6 == 0) ? R(r, 65530, 65540) : R(r, 0, 300);
        x.call = [=](void* f) { if (kind == 0) ((void(__cdecl*)(j_compress_ptr, int))f)(c, val); else ((void(__cdecl*)(j_compress_ptr, int, unsigned))f)(c, mk, len); return 0; };
        uint32_t off0 = (uint32_t)(d->next_output_byte - out), free0 = (uint32_t)d->free_in_buffer; boolean fl = g_mock_flag;
        x.dump = [=](std::vector<uint8_t>& v) { put32(v, (uint32_t)kind); put32(v, (uint32_t)val); put32(v, (uint32_t)mk); put32(v, len); put32(v, off0); put32(v, free0); put32(v, (uint32_t)fl); put32(v, (uint32_t)(d->next_output_byte - out)); put32(v, (uint32_t)d->free_in_buffer); put32(v, W32(d, 0x1C)); putn(v, out, 24); };
    };
    g_tests.push_back({"emit_byte", 0x5C6070, "jcmarker", "emit_byte", [dest_fixture](Rng& r, Ctx& x) { dest_fixture(r, x, 0); }});
    g_tests.push_back({"write_marker_header", 0x5C6030, "jcmarker", "write_marker_header", [dest_fixture](Rng& r, Ctx& x) { dest_fixture(r, x, 1); }});
}


// ---- compresion: submuestreo (jcsample.c)
static uint32_t* g_log;   // registro de llamadas de los mocks (dentro del arena)
static void __cdecl mock_down(j_compress_ptr c, jpeg_component_info* cp, JSAMPARRAY in, JSAMPARRAY out) {
    uint32_t* L = (uint32_t*)c->client_data; uint32_t n = L[0]++; L[1 + 4 * n] = (uint32_t)(uintptr_t)cp; L[2 + 4 * n] = (uint32_t)(uintptr_t)in; L[3 + 4 * n] = (uint32_t)(uintptr_t)out; L[4 + 4 * n] = (uint32_t)c->image_width ^ (uint32_t)cp->component_index;
}
static void add_downsample_tests() {
    g_tests.push_back({"start_pass_downsample", 0x5C79E0, "jcsample", "start_pass_downsample", [](Rng& r, Ctx& x) {
        auto c = rand_cinfo(r); x.call = [c](void* f) { ((void(__cdecl*)(j_compress_ptr))f)(c); return 0; }; x.dump = [](std::vector<uint8_t>&) {};
    }, 1});
    g_tests.push_back({"sep_downsample", 0x5C7A50, "jcsample", "sep_downsample", [](Rng& r, Ctx& x) {
        auto c = new_cinfo(); int nc = (int)R(r, 0, 4); c->num_components = nc; auto ci = AT<jpeg_component_info>(4); memset(ci, 0, sizeof(jpeg_component_info) * 4); c->comp_info = ci;
        uint32_t* L = (uint32_t*)A(4 * 4 * 5 + 16, 8); memset(L, 0, 16 * 5 + 16); c->client_data = L; c->image_width = r();
        void* ds = A(0x40, 8); memset(ds, 0, 0x40); c->downsample = (struct jpeg_downsampler*)ds;
        for (int i = 0; i < 4; i++) { ci[i].component_index = i; ci[i].v_samp_factor = (int)R(r, 1, 4); W32(ds, 0xC + 4 * i) = (uint32_t)(uintptr_t)mock_down; }
        JSAMPIMAGE ib = (JSAMPIMAGE)AT<JSAMPARRAY>(4), ob = (JSAMPIMAGE)AT<JSAMPARRAY>(4); for (int i = 0; i < 4; i++) { ib[i] = (JSAMPARRAY)(uintptr_t)(0x1000 * (i + 1) + 0x100); ob[i] = (JSAMPARRAY)(uintptr_t)(0x9000 * (i + 1) + 0x200); }
        uint32_t ir = R(r, 0, 20), orow = R(r, 0, 9);
        x.call = [=](void* f) { ((void(__cdecl*)(j_compress_ptr, JSAMPIMAGE, JDIMENSION, JSAMPIMAGE, JDIMENSION))f)(c, ib, ir, ob, orow); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) { put32(v, (uint32_t)nc); put32(v, ir); put32(v, orow); put32(v, c->image_width); for (int i = 0; i < 4; i++) { put32(v, (uint32_t)ci[i].v_samp_factor); put32(v, (uint32_t)(uintptr_t)ib[i]); put32(v, (uint32_t)(uintptr_t)ob[i]); } putn(v, L, 4 * 17); put32(v, (uint32_t)(uintptr_t)ci); };
    }});
    struct DS { const char* name; uint32_t addr; const char* lib; int kind; };
    DS dss[] = {{"int_downsample", 0x5C7AE0, "int_downsample", 0}, {"fullsize_downsample", 0x5C7C10, "fullsize_downsample", 1}, {"h2v1_downsample", 0x5C7C60, "h2v1_downsample", 2}, {"h2v2_downsample", 0x5C7D10, "h2v2_downsample", 3}};
    for (auto& d0 : dss) {
        DS t = d0;
        g_tests.push_back({t.name, t.addr, "jcsample", t.lib, [t](Rng& r, Ctx& x) {
            auto c = new_cinfo(); auto cp = AT<jpeg_component_info>(); memset(cp, 0, sizeof *cp);
            int hs = (int)R(r, 1, 2), vs = (int)R(r, 1, 3), he = 1, ve = 1;
            switch (t.kind) { case 0: he = (int)R(r, 1, 4); ve = (int)R(r, 1, 4); break; case 1: break; case 2: he = 2; break; case 3: he = 2; ve = 2; break; }
            cp->h_samp_factor = hs; cp->v_samp_factor = vs; cp->width_in_blocks = R(r, 1, 5); c->max_h_samp_factor = hs * he; c->max_v_samp_factor = vs * ve;
            uint32_t OC = cp->width_in_blocks * 8, inw = OC * (uint32_t)he;
            uint32_t iw = (r() % 3 == 0) ? inw : R(r, 1, inw); if (t.kind == 1) iw = std::min(iw, OC); if (r() % 5 == 0) iw = std::max<uint32_t>(1, std::min(iw, inw));
            c->image_width = iw; c->num_components = 1;
            uint32_t inrows = (uint32_t)c->max_v_samp_factor, outrows = (t.kind == 1) ? inrows : (uint32_t)vs;
            JSAMPARRAY in = make_rows(r, inrows, inw, 8, true); JSAMPARRAY out = make_rows(r, outrows, OC, 8, false);
            std::vector<uint8_t> in0; for (uint32_t i = 0; i < inrows; i++) putn(in0, in[i], iw);
            x.call = [=](void* f) { ((void(__cdecl*)(j_compress_ptr, jpeg_component_info*, JSAMPARRAY, JSAMPARRAY))f)(c, cp, in, out); return 0; };
            x.dump = [=](std::vector<uint8_t>& v) {
                put32(v, (uint32_t)t.kind); put32(v, (uint32_t)hs); put32(v, (uint32_t)vs); put32(v, (uint32_t)he); put32(v, (uint32_t)ve); put32(v, cp->width_in_blocks); put32(v, iw); put32(v, inrows); put32(v, outrows);
                putn(v, in0.data(), in0.size()); for (uint32_t i = 0; i < outrows; i++) putn(v, out[i], OC);
            };
        }});
    }
}


// ---- descompresion: gestor de IDCT (jddctmgr.c)
static void add_idctmgr_tests() {
    g_tests.push_back({"start_pass_idctmgr", 0x5C9510, "jddctmgr", "start_pass", [](Rng& r, Ctx& x) {
        auto c = rand_dinfo(r); int nc = (int)R(r, 0, 4); c->num_components = nc;
        void* idct = A(0x40, 8); memset(idct, 0xAB, 0x40); c->idct = (struct jpeg_inverse_dct*)idct;
        auto ci = AT<jpeg_component_info>(4); memset(ci, 0, sizeof(jpeg_component_info) * 4); c->comp_info = ci;
        c->dct_method = (r() % 8 == 0) ? (J_DCT_METHOD)R(r, 0, 1) : JDCT_FLOAT;
        std::vector<uint16_t> qv[4]; JQUANT_TBL* qt[4]; FAST_FLOAT* tab[4];
        for (int i = 0; i < 4; i++) {
            ci[i].DCT_scaled_size = (r() % 14 == 0) ? (int)R(r, 0, 9) : 8; ci[i].component_needed = (boolean)(r() % 5 != 0);
            qt[i] = nullptr; if (r() % 6) { qt[i] = AT<JQUANT_TBL>(); for (int k = 0; k < 64; k++) qt[i]->quantval[k] = (UINT16)((r() % 7 == 0) ? r() : (r() % 5 == 0 ? R(r, 1, 255) : R(r, 1, 80))); }
            ci[i].quant_table = qt[i]; tab[i] = AT<FAST_FLOAT>(64); for (int k = 0; k < 64; k++) tab[i][k] = (FAST_FLOAT)(r() % 1000); ci[i].dct_table = tab[i];
            W32(idct, 0x14 + 4 * i) = (r() % 3 == 0) ? 2u : (uint32_t)R(r, 0, 3);
        }
        uint32_t cm0[4]; for (int i = 0; i < 4; i++) cm0[i] = W32(idct, 0x14 + 4 * i);
        x.fps.push_back({aoff(idct) + 4, {{0x5CDA90, {"jidctflt", "jpeg_idct_float"}}}}); x.fps.push_back({aoff(idct) + 8, {{0x5CDA90, {"jidctflt", "jpeg_idct_float"}}}});
        x.fps.push_back({aoff(idct) + 12, {{0x5CDA90, {"jidctflt", "jpeg_idct_float"}}}}); x.fps.push_back({aoff(idct) + 16, {{0x5CDA90, {"jidctflt", "jpeg_idct_float"}}}});
        x.call = [=](void* f) { ((void(__cdecl*)(j_decompress_ptr))f)(c); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) {
            put32(v, (uint32_t)nc); put32(v, (uint32_t)c->dct_method);
            for (int i = 0; i < 4; i++) { put32(v, (uint32_t)ci[i].DCT_scaled_size); put32(v, (uint32_t)(ci[i].component_needed & 0xFF)); put32(v, qt[i] != nullptr); if (qt[i]) putn(v, qt[i]->quantval, 128); put32(v, cm0[i]); put32(v, W32(idct, 0x14 + 4 * i)); putn(v, tab[i], 256); put32(v, W32(idct, 4 + 4 * i) != 0); }
        };
    }});
}


// ---- compresion: preprocesamiento (jcprepct.c, sin contexto)
static uint32_t mk_hash(uint32_t call, uint32_t ci, uint32_t row, uint32_t x) { return (call * 31 + ci * 17 + row * 7 + x * 3 + 5) & 255; }
static void __cdecl mock_cconv(j_compress_ptr c, JSAMPARRAY in, JSAMPIMAGE out, JDIMENSION orow, int n) {
    uint32_t* L = (uint32_t*)c->client_data; uint32_t call = L[0]++; uint32_t* e = L + 2 + 4 * call; e[0] = (uint32_t)(uintptr_t)in; e[1] = (uint32_t)(uintptr_t)out; e[2] = orow; e[3] = (uint32_t)n;
    for (int ci = 0; ci < c->num_components; ci++) for (int i = 0; i < n; i++) for (JDIMENSION x = 0; x < c->image_width; x++) out[ci][orow + i][x] = (JSAMPLE)mk_hash(call, (uint32_t)ci, orow + i, x);
}
static void __cdecl mock_dsamp(j_compress_ptr c, JSAMPIMAGE in, JDIMENSION in_idx, JSAMPIMAGE out, JDIMENSION ogi) {
    uint32_t* L = (uint32_t*)c->client_data; uint32_t call = L[1]++; uint32_t* e = L + 2 + 4 * 24 + 4 * call; e[0] = (uint32_t)(uintptr_t)in; e[1] = in_idx; e[2] = (uint32_t)(uintptr_t)out; e[3] = ogi;
    for (int ci = 0; ci < c->num_components; ci++) { jpeg_component_info* cp = c->comp_info + ci; for (int r = 0; r < cp->v_samp_factor; r++) for (JDIMENSION x = 0; x < cp->width_in_blocks * 8; x++) out[ci][ogi * cp->v_samp_factor + r][x] = (JSAMPLE)(mk_hash(100 + call, (uint32_t)ci, ogi * cp->v_samp_factor + r, x)); }
}
static void add_prep_tests() {
    g_tests.push_back({"pre_process_data", 0x5C7710, "jcprepct", "pre_process_data", [](Rng& r, Ctx& x) {
        auto c = new_cinfo(); int nc = (int)R(r, 1, 3); c->num_components = nc; int maxv = (int)R(r, 1, 4); c->max_v_samp_factor = maxv; c->image_width = R(r, 1, 40);
        auto ci = AT<jpeg_component_info>(4); memset(ci, 0, sizeof(jpeg_component_info) * 4); c->comp_info = ci;
        uint32_t* L = (uint32_t*)A(4 * (2 + 4 * 48), 8); memset(L, 0, 4 * (2 + 4 * 48)); c->client_data = L;
        void* cv = A(0x20, 8); memset(cv, 0, 0x20); c->cconvert = (struct jpeg_color_converter*)cv; W32(cv, 4) = (uint32_t)(uintptr_t)mock_cconv;
        void* ds = A(0x20, 8); memset(ds, 0, 0x20); c->downsample = (struct jpeg_downsampler*)ds; W32(ds, 4) = (uint32_t)(uintptr_t)mock_dsamp;
        void* pr = A(0x100, 8); memset(pr, 0xAB, 0x100); c->prep = (struct jpeg_c_prep_controller*)pr;
        uint32_t W = c->image_width, inavail = R(r, 1, 12), inctr = R(r, 0, inavail - 1), ogavail = R(r, 1, 4), ogctr = R(r, 0, ogavail - 1);
        for (int i = 0; i < nc; i++) { ci[i].component_index = i; ci[i].v_samp_factor = (int)R(r, 1, (uint32_t)maxv); ci[i].width_in_blocks = R(r, 1, 6); }
        JSAMPIMAGE colb = (JSAMPIMAGE)AT<JSAMPARRAY>(nc); JSAMPIMAGE ob = (JSAMPIMAGE)AT<JSAMPARRAY>(nc);
        for (int i = 0; i < nc; i++) { colb[i] = make_rows(r, (uint32_t)maxv, W, 0, true); ob[i] = make_rows(r, ogavail * (uint32_t)ci[i].v_samp_factor, ci[i].width_in_blocks * 8, 0, true); W32(pr, 8 + 4 * i) = (uint32_t)(uintptr_t)colb[i]; }
        JSAMPARRAY in = make_rows(r, inavail, W * 3, 0, true);
        uint32_t rtg = (r() % 4 == 0) ? (inavail - inctr) : R(r, 1, inavail + 6); uint32_t nbr = R(r, 0, (uint32_t)maxv - 1); W32(pr, 0x18) = rtg; W32(pr, 0x1C) = nbr;
        uint32_t* pin = (uint32_t*)A(8, 8); uint32_t* pout = (uint32_t*)A(8, 8); *pin = inctr; *pout = ogctr;
        std::vector<uint8_t> init; for (int i = 0; i < nc; i++) { for (int k = 0; k < maxv; k++) putn(init, colb[i][k], W); for (uint32_t k = 0; k < ogavail * (uint32_t)ci[i].v_samp_factor; k++) putn(init, ob[i][k], ci[i].width_in_blocks * 8); }
        x.call = [=](void* f) { ((void(__cdecl*)(j_compress_ptr, JSAMPARRAY, JDIMENSION*, JDIMENSION, JSAMPIMAGE, JDIMENSION*, JDIMENSION))f)(c, in, pin, inavail, ob, pout, ogavail); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) {
            put32(v, (uint32_t)nc); put32(v, (uint32_t)maxv); put32(v, W); put32(v, inavail); put32(v, inctr); put32(v, ogavail); put32(v, ogctr); put32(v, rtg); put32(v, nbr);
            for (int i = 0; i < nc; i++) { put32(v, (uint32_t)ci[i].v_samp_factor); put32(v, ci[i].width_in_blocks); }
            putn(v, init.data(), init.size());
            for (int i = 0; i < nc; i++) { for (int k = 0; k < maxv; k++) putn(v, colb[i][k], W); for (uint32_t k = 0; k < ogavail * (uint32_t)ci[i].v_samp_factor; k++) putn(v, ob[i][k], ci[i].width_in_blocks * 8); }
            put32(v, *pin); put32(v, *pout); put32(v, W32(pr, 0x18)); put32(v, W32(pr, 0x1C)); putn(v, L, 4 * (2 + 4 * 48));
            put32(v, (uint32_t)(uintptr_t)in); put32(v, (uint32_t)(uintptr_t)pr); put32(v, (uint32_t)(uintptr_t)ob);
        };
    }, 4000});
}


// ---- descompresion: coeficientes de una pasada (jdcoefct.c decompress_onepass)
static boolean __cdecl mock_decode_mcu_b(j_decompress_ptr c, JBLOCKROW* mcu) {
    uint32_t* L = (uint32_t*)c->client_data; uint32_t n = L[0]++; L[2 + n] = (uint32_t)(uintptr_t)mcu[0];
    for (int b = 0; b < c->blocks_in_MCU; b++) mcu[b][0][0] = (JCOEF)(n * 3 + (uint32_t)b + 1);
    uint32_t stop = L[1]; return (stop != 0 && n + 1 == stop) ? FALSE : TRUE;
}
static void __cdecl mock_idct_o(j_decompress_ptr c, jpeg_component_info* cp, JCOEFPTR blk, JSAMPARRAY out, JDIMENSION col) {
    uint32_t* L = (uint32_t*)c->client_data; uint32_t n = L[64 + 1]++; uint32_t* e = L + 64 + 2 + 5 * n; e[0] = (uint32_t)(uintptr_t)cp; e[1] = (uint32_t)(uintptr_t)blk; e[2] = (uint32_t)(uintptr_t)out; e[3] = col; e[4] = (uint32_t)(uint16_t)blk[0];
}
static void __cdecl mock_finish_input(j_decompress_ptr c) { uint32_t* L = (uint32_t*)c->client_data; L[64]++; }
static void add_onepass_tests() {
    g_tests.push_back({"decompress_onepass", 0x5C8B70, "jdcoefct", "decompress_onepass", [](Rng& r, Ctx& x) {
        auto c = new_dinfo(); c->mem = &g_mem; x.check_ret = true;
        ((void(__cdecl*)(j_decompress_ptr, boolean))lib("jdcoefct", "jinit_d_coef_controller"))(c, FALSE);
        void* co = c->coef;
        int nc = (int)R(r, 1, 3); c->comps_in_scan = nc; int tot = 0;
        auto ci = AT<jpeg_component_info>(4); memset(ci, 0, sizeof(jpeg_component_info) * 4);
        for (int i = 0; i < nc; i++) {
            auto& p = ci[i]; p.component_index = i; p.component_needed = (boolean)(r() % 5 != 0); p.MCU_width = i == 2 ? 1 : (int)R(r, 1, 2); p.MCU_height = i == 2 ? 1 : (int)R(r, 1, 2); p.v_samp_factor = (int)R(r, 1, 3); p.MCU_blocks = p.MCU_width * p.MCU_height; tot += p.MCU_blocks;
            p.DCT_scaled_size = 8; p.MCU_sample_width = p.MCU_width * 8; p.last_col_width = (int)R(r, 1, (uint32_t)p.MCU_width); p.last_row_height = (int)R(r, 1, (uint32_t)p.MCU_height);
            c->cur_comp_info[i] = &p;
        }
        c->blocks_in_MCU = tot; c->MCUs_per_row = R(r, 1, 4); c->total_iMCU_rows = R(r, 1, 4); c->input_iMCU_row = R(r, 0, c->total_iMCU_rows - 1 + (r() % 3 == 0 ? 1 : 0)); c->output_iMCU_row = R(r, 0, 5);
        uint32_t rows_per = R(r, 1, 3); W32(co, 0x1C) = rows_per; W32(co, 0x18) = R(r, 0, rows_per - 1); W32(co, 0x14) = R(r, 0, c->MCUs_per_row - 1);
        void* idct = A(0x40, 8); memset(idct, 0, 0x40); c->idct = (struct jpeg_inverse_dct*)idct; for (int i = 0; i < 4; i++) W32(idct, 4 + 4 * i) = (uint32_t)(uintptr_t)mock_idct_o;
        void* en = A(0x40, 8); memset(en, 0, 0x40); c->entropy = (struct jpeg_entropy_decoder*)en; c->entropy->decode_mcu = mock_decode_mcu_b;
        void* ic = A(0x40, 8); memset(ic, 0, 0x40); c->inputctl = (struct jpeg_input_controller*)ic; c->inputctl->finish_input_pass = mock_finish_input;
        uint32_t* L = (uint32_t*)A(4 * (64 + 2 + 5 * 256), 8); memset(L, 0, 4 * (64 + 2 + 5 * 256)); c->client_data = L; L[1] = (r() % 3 == 0) ? R(r, 1, 6) : 0;
        JSAMPIMAGE ob = (JSAMPIMAGE)AT<JSAMPARRAY>(4); for (int i = 0; i < 4; i++) ob[i] = (JSAMPARRAY)(uintptr_t)(0x100000 * (i + 1));
        // fija la mascara de estado del controlador que el banco compara: todo el arena (incluye los 10 bloques del MCU)
        uint32_t ctr0 = W32(co, 0x14), vo0 = W32(co, 0x18);
        std::vector<uint32_t> cinfo_vals = {c->MCUs_per_row, c->total_iMCU_rows, c->input_iMCU_row, c->output_iMCU_row, (uint32_t)c->comps_in_scan, (uint32_t)c->blocks_in_MCU, rows_per, vo0, ctr0, L[1]};
        x.call = [=](void* f) { return (int)((int(__cdecl*)(j_decompress_ptr, JSAMPIMAGE))f)(c, ob); };
        x.dump = [=](std::vector<uint8_t>& v) {
            for (auto q : cinfo_vals) put32(v, q);
            for (int i = 0; i < nc; i++) { auto& p = ci[i]; put32(v, (uint32_t)p.component_needed & 0xFF); put32(v, (uint32_t)p.MCU_width); put32(v, (uint32_t)p.MCU_height); put32(v, (uint32_t)p.last_col_width); put32(v, (uint32_t)p.last_row_height); put32(v, (uint32_t)(uintptr_t)&p); }
            for (int i = 0; i < nc; i++) put32(v, (uint32_t)(uintptr_t)ob[i]);
            put32(v, c->input_iMCU_row); put32(v, c->output_iMCU_row); put32(v, W32(co, 0x14)); put32(v, W32(co, 0x18)); put32(v, W32(co, 0x1C)); put32(v, W32(c->coef, 0x14));
            putn(v, L, 4 * (64 + 2 + 5 * 256)); put32(v, (uint32_t)(uintptr_t)((JBLOCKROW*)((uint8_t*)co + 0x20))[0]); put32(v, (uint32_t)ci[0].v_samp_factor);
        };
    }, 4000});
}


// ---- compresion: cabeceras de marcadores (jcmarker.c write_tables_only / write_scan_header)
static void add_marker_tests() {
    for (int kind = 0; kind < 2; kind++) {
        g_tests.push_back({kind == 0 ? "write_tables_only" : "write_scan_header", kind == 0 ? 0x5C62F0u : 0x5C61E0u, "jcmarker", kind == 0 ? "write_tables_only" : "write_scan_header", [kind](Rng& r, Ctx& x) {
            auto c = new_cinfo(); auto d = (jpeg_destination_mgr*)A(0x40, 8); memset(d, 0, 0x40); c->dest = d; uint8_t* out = (uint8_t*)A(4200, 8); memset(out, 0x55, 4200);
            d->next_output_byte = out; d->free_in_buffer = 4000; d->empty_output_buffer = mock_empty_output; g_mock_flag = TRUE;
            void* mk = priv(r, 0x40); c->marker = (struct jpeg_marker_writer*)mk;
            c->arith_code = (boolean)(r() % 6 == 0); c->progressive_mode = (boolean)(r() % 3 == 0); c->Ss = (int)R(r, 0, 2) * (r() % 2); c->Se = (int)R(r, 0, 63); c->Ah = (int)R(r, 0, 3); c->Al = (int)R(r, 0, 3);
            c->restart_interval = (r() % 3 == 0) ? 0 : R(r, 1, 600); uint32_t lri = (r() % 2) ? c->restart_interval : R(r, 0, 600); W32(mk, 0x1C) = lri;
            struct Q { bool p; int sent; uint16_t qv[64]; }; struct H { bool p; int sent; uint8_t bits[17]; uint8_t hv[256]; };
            auto qs = std::make_shared<std::vector<Q>>(4); auto hs = std::make_shared<std::vector<H>>(8);
            JQUANT_TBL* qp[4]; JHUFF_TBL* hp[8];
            for (int i = 0; i < 4; i++) { qp[i] = nullptr; (*qs)[i].p = false; if (r() % 4) { qp[i] = AT<JQUANT_TBL>(); (*qs)[i].p = true; for (int k = 0; k < 64; k++) qp[i]->quantval[k] = (UINT16)((r() % 12 == 0) ? R(r, 256, 1000) : R(r, 1, 255)); qp[i]->sent_table = (boolean)(r() % 3 == 0); (*qs)[i].sent = qp[i]->sent_table; memcpy((*qs)[i].qv, qp[i]->quantval, 128); c->quant_tbl_ptrs[i] = qp[i]; } }
            for (int i = 0; i < 8; i++) { hp[i] = nullptr; (*hs)[i].p = false; if (r() % 5) { hp[i] = AT<JHUFF_TBL>(); (*hs)[i].p = true; hp[i]->bits[0] = 0; for (int k = 1; k <= 16; k++) hp[i]->bits[k] = (UINT8)((r() % 4 == 0) ? R(r, 0, 12) : R(r, 0, 2)); for (int k = 0; k < 256; k++) hp[i]->huffval[k] = (UINT8)r(); hp[i]->sent_table = (boolean)(r() % 3 == 0); (*hs)[i].sent = hp[i]->sent_table; memcpy((*hs)[i].bits, hp[i]->bits, 17); memcpy((*hs)[i].hv, hp[i]->huffval, 256); if (i < 4) c->dc_huff_tbl_ptrs[i] = hp[i]; else c->ac_huff_tbl_ptrs[i - 4] = hp[i]; } }
            int nc = (int)R(r, 1, 4); c->comps_in_scan = nc; auto ci = AT<jpeg_component_info>(4); memset(ci, 0, sizeof(jpeg_component_info) * 4); c->comp_info = ci; c->num_components = nc;
            for (int i = 0; i < 4; i++) { ci[i].component_id = (int)R(r, 0, 255); ci[i].dc_tbl_no = (int)R(r, 0, 3); ci[i].ac_tbl_no = (int)R(r, 0, 3); c->cur_comp_info[i] = &ci[i]; }
            bool arith = c->arith_code, prog = c->progressive_mode; int Ss = c->Ss, Se = c->Se, Ah = c->Ah, Al = c->Al; uint32_t ri = c->restart_interval;
            x.call = [=](void* f) { ((void(__cdecl*)(j_compress_ptr))f)(c); return 0; };
            x.dump = [=](std::vector<uint8_t>& v) {
                put32(v, (uint32_t)kind); put32(v, arith); put32(v, prog); put32(v, (uint32_t)Ss); put32(v, (uint32_t)Se); put32(v, (uint32_t)Ah); put32(v, (uint32_t)Al); put32(v, ri); put32(v, lri); put32(v, (uint32_t)nc);
                for (int i = 0; i < nc; i++) { put32(v, (uint32_t)ci[i].component_id); put32(v, (uint32_t)ci[i].dc_tbl_no); put32(v, (uint32_t)ci[i].ac_tbl_no); }
                for (int i = 0; i < 4; i++) { put32(v, (*qs)[i].p); put32(v, (uint32_t)(*qs)[i].sent); putn(v, (*qs)[i].qv, 128); put32(v, qp[i] ? (uint32_t)(qp[i]->sent_table & 0xFF) : 0); }
                for (int i = 0; i < 8; i++) { put32(v, (*hs)[i].p); put32(v, (uint32_t)(*hs)[i].sent); putn(v, (*hs)[i].bits, 17); putn(v, (*hs)[i].hv, 256); put32(v, hp[i] ? (uint32_t)(hp[i]->sent_table & 0xFF) : 0); }
                put32(v, W32(mk, 0x1C)); uint32_t n = (uint32_t)(d->next_output_byte - out); put32(v, n); putn(v, out, n > 4100 ? 4100 : n);
            };
        }, 4000});
    }
}


// ---- gestor de memoria: acceso a matrices virtuales (jmemmgr.c access_virt_sarray / access_virt_barray)
static void __cdecl mock_bs_read(j_common_ptr c, void*, void* buf, long off, long cnt) {
    uint32_t* L = (uint32_t*)c->client_data; uint32_t n = L[0]++; uint32_t* e = L + 1 + 4 * n; e[0] = 0; e[1] = (uint32_t)(uintptr_t)buf; e[2] = (uint32_t)off; e[3] = (uint32_t)cnt;
    for (long k = 0; k < cnt; k++) ((uint8_t*)buf)[k] = (uint8_t)((off + k) * 7 + 3);
}
static void __cdecl mock_bs_write(j_common_ptr c, void*, void* buf, long off, long cnt) {
    uint32_t* L = (uint32_t*)c->client_data; uint32_t n = L[0]++; uint32_t* e = L + 1 + 4 * n; e[0] = 1; e[1] = (uint32_t)(uintptr_t)buf; e[2] = (uint32_t)off; e[3] = (uint32_t)cnt;
}
static void add_virt_tests() {
    for (int kind = 0; kind < 2; kind++) {
        g_tests.push_back({kind == 0 ? "access_virt_sarray" : "access_virt_barray", kind == 0 ? 0x5CF2F0u : 0x5CF430u, "jmemmgr", kind == 0 ? "access_virt_sarray" : "access_virt_barray", [kind](Rng& r, Ctx& x) {
            x.check_ret = true;
            auto c = new_cinfo(); uint32_t* L = (uint32_t*)A(4 * (1 + 4 * 64), 8); memset(L, 0, 4 * (1 + 4 * 64)); c->client_data = L;
            uint32_t bpr_unit = kind == 0 ? 1 : 128; uint32_t spr = R(r, 1, kind == 0 ? 24 : 4), bpr = spr * bpr_unit;
            uint32_t rows = R(r, 1, 30), maxacc = R(r, 1, 6), rim = std::max<uint32_t>(maxacc, R(r, 1, 12)), rpc = R(r, 1, rim);
            uint32_t cur = (r() % 3 == 0) ? 0 : R(r, 0, rows), undef = R(r, 0, rows);
            void* p = A(0x80, 8); memset(p, 0, 0x80);
            uint8_t* data = (uint8_t*)A((size_t)rim * bpr + 16, 8); for (size_t i = 0; i < (size_t)rim * bpr; i++) data[i] = (uint8_t)r();
            uint8_t** rp = (uint8_t**)A(4 * rim, 8); for (uint32_t i = 0; i < rim; i++) rp[i] = data + (size_t)i * bpr;
            W32(p, 0) = (r() % 12 == 0) ? 0u : (uint32_t)(uintptr_t)rp; W32(p, 4) = rows; W32(p, 8) = spr; W32(p, 0xC) = maxacc; W32(p, 0x10) = rim; W32(p, 0x14) = rpc; W32(p, 0x18) = cur; W32(p, 0x1C) = undef;
            ((uint8_t*)p)[0x20] = (uint8_t)(r() & 1); ((uint8_t*)p)[0x21] = (uint8_t)(r() & 1); ((uint8_t*)p)[0x22] = (uint8_t)(r() % 8 != 0); W32(p, 0x28) = (uint32_t)(uintptr_t)mock_bs_read; W32(p, 0x2C) = (uint32_t)(uintptr_t)mock_bs_write;
            uint32_t start = R(r, 0, rows), num = R(r, 0, maxacc + (r() % 8 == 0 ? 1 : 0)); boolean wr = (boolean)(r() & 1);
            if (r() % 2) { start = std::min(start, rows - std::min(rows, num)); }
            struct Pre { uint32_t rows, spr, maxacc, rim, rpc, cur, undef, pz, dirty, bso, start, num, wr, mb; std::vector<uint8_t> data; };
            auto pre = std::make_shared<Pre>(); pre->rows = rows; pre->spr = spr; pre->maxacc = maxacc; pre->rim = rim; pre->rpc = rpc; pre->cur = cur; pre->undef = undef; pre->pz = ((uint8_t*)p)[0x20]; pre->dirty = ((uint8_t*)p)[0x21]; pre->bso = ((uint8_t*)p)[0x22];
            pre->start = start; pre->num = num; pre->wr = wr; pre->mb = W32(p, 0); pre->data.assign(data, data + (size_t)rim * bpr);
            x.call = [=](void* f) { return (int)((int(__cdecl*)(j_common_ptr, void*, JDIMENSION, JDIMENSION, boolean))f)((j_common_ptr)c, p, start, num, wr); };
            x.dump = [=](std::vector<uint8_t>& v) {
                put32(v, (uint32_t)kind); put32(v, pre->rows); put32(v, pre->spr); put32(v, pre->maxacc); put32(v, pre->rim); put32(v, pre->rpc); put32(v, pre->cur); put32(v, pre->undef); put32(v, pre->pz); put32(v, pre->dirty); put32(v, pre->bso);
                put32(v, pre->start); put32(v, pre->num); put32(v, pre->wr); put32(v, pre->mb); put32(v, (uint32_t)(uintptr_t)rp); put32(v, (uint32_t)(uintptr_t)data);
                putn(v, pre->data.data(), pre->data.size());
                put32(v, W32(p, 0x18)); put32(v, W32(p, 0x1C)); put32(v, ((uint8_t*)p)[0x21]); putn(v, data, (size_t)rim * bpr); putn(v, L, 4 * (1 + 4 * 64));
            };
        }, 4000});
    }
}


// ---- descompresion: lector de marcadores (jdmarker.c): read_markers, get_interesting_appn, jinit_marker_reader
// Fuente de bytes simulada: el flujo se reparte en trozos; fill_input_buffer pasa al siguiente (o suspende si es el trozo marcado); skip_input_data como jdatasrc.c.
struct SrcState { uint32_t nchunks; uint32_t cur; uint32_t suspend_at; uint32_t off[40]; uint32_t len[40]; uint8_t* base; uint32_t fills; };
static SrcState* g_src_state(j_decompress_ptr c) { return (SrcState*)c->client_data; }
static boolean __cdecl mock_fill(j_decompress_ptr c) {
    SrcState* s = g_src_state(c); s->fills++;
    if (s->cur + 1 >= s->nchunks) return FALSE;                    // sin mas datos: suspension
    if (s->suspend_at != 0xFFFFFFFFu && s->cur + 1 == s->suspend_at) { s->suspend_at = 0xFFFFFFFFu; return FALSE; } // suspension en este limite (una vez)
    s->cur++; c->src->next_input_byte = s->base + s->off[s->cur]; c->src->bytes_in_buffer = s->len[s->cur]; return TRUE;
}
static void __cdecl mock_skip(j_decompress_ptr c, long n) {
    if (n <= 0) return; struct jpeg_source_mgr* src = c->src;
    while (n > (long)src->bytes_in_buffer) { n -= (long)src->bytes_in_buffer; (void)mock_fill(c); if (src->bytes_in_buffer == 0 && g_src_state(c)->cur + 1 >= g_src_state(c)->nchunks) { return; } }
    src->next_input_byte += n; src->bytes_in_buffer -= (size_t)n;
}
static void __cdecl mock_init_source(j_decompress_ptr) {}
static boolean __cdecl mock_resync(j_decompress_ptr, int) { return TRUE; }
static void __cdecl mock_term_source(j_decompress_ptr) {}
static void put8(std::vector<uint8_t>& v, int b) { v.push_back((uint8_t)b); }
static void put16be(std::vector<uint8_t>& v, int b) { v.push_back((uint8_t)(b >> 8)); v.push_back((uint8_t)b); }
// construye un flujo de cabeceras JPEG; si hay mutacion se rompe despues
static void gen_jpeg_header(Rng& r, std::vector<uint8_t>& s, bool full) {
    auto seg = [&](int marker, const std::vector<uint8_t>& body) { put8(s, 0xFF); put8(s, marker); put16be(s, (int)body.size() + 2); s.insert(s.end(), body.begin(), body.end()); };
    put8(s, 0xFF); put8(s, 0xD8);
    if (r() % 3 == 0) { std::vector<uint8_t> b = {'J', 'F', 'I', 'F', 0, (uint8_t)R(r, 1, 1), (uint8_t)R(r, 0, 2), (uint8_t)R(r, 0, 2), (uint8_t)(r() % 256), (uint8_t)r(), (uint8_t)(r() % 256), (uint8_t)r(), 0, 0}; if (r() % 4 == 0) { b[12] = 1; b[13] = 1; b.push_back(1); b.push_back(2); b.push_back(3); } seg(0xE0, b); }
    if (r() % 6 == 0) { std::vector<uint8_t> b = {'J', 'F', 'X', 'X', 0, 0x10, 1, 2, 3}; seg(0xE0, b); }
    if (r() % 4 == 0) { std::vector<uint8_t> b = {'A', 'd', 'o', 'b', 'e', 0, 100, 0, 0, 0, 0, (uint8_t)R(r, 0, 2)}; if (r() % 6 == 0) b.resize(R(r, 0, 11)); seg(0xEE, b); }
    if (r() % 5 == 0) { std::vector<uint8_t> b(R(r, 0, 20)); for (auto& q : b) q = (uint8_t)r(); seg(0xFE, b); }
    if (r() % 5 == 0) { std::vector<uint8_t> b(R(r, 0, 30)); for (auto& q : b) q = (uint8_t)r(); seg((int)R(r, 0xE1, 0xED), b); }
    int ndqt = (int)R(r, 1, 3); static const int dummy = 0; (void)dummy;
    for (int i = 0; i < ndqt; i++) { int prec = r() % 5 == 0; std::vector<uint8_t> b; put8(b, (prec << 4) | (int)R(r, 0, 3)); for (int k = 0; k < 64; k++) { int v = prec ? (int)R(r, 1, 65535) : (int)R(r, 1, 255); if (prec) put8(b, v >> 8); put8(b, v); } seg(0xDB, b); }
    int nc = (int)R(r, 1, 4); int sof = (int)(r() % 8 == 0 ? R(r, 0xC0, 0xC2) : (r() % 2 ? 0xC0 : 0xC2)); if (full) sof = (int)R(r, 0xC0, 0xC2); if (r() % 12 == 0) sof = 0xC9;
    { std::vector<uint8_t> b; put8(b, 8); put16be(b, (int)R(r, 1, 3000)); put16be(b, (int)R(r, 1, 3000)); put8(b, nc); for (int i = 0; i < nc; i++) { put8(b, (int)R(r, 0, 255)); put8(b, ((int)R(r, 1, 4) << 4) | (int)R(r, 1, 4)); put8(b, (int)R(r, 0, 3)); } seg(sof, b);
      // ids distintos para que SOS los encuentre
      size_t base = s.size() - b.size(); for (int i = 0; i < nc; i++) s[base + 6 + 3 * i] = (uint8_t)(i + 1 + (r() % 3 == 0 ? 10 * (int)R(r, 0, 3) : 0)); }
    int ndht = (int)R(r, 1, 4);
    for (int i = 0; i < ndht; i++) { std::vector<uint8_t> b; put8(b, ((r() & 1) << 4) | (int)R(r, 0, 3)); int tot = 0; uint8_t bits[17]; for (int k = 1; k <= 16; k++) { bits[k] = (uint8_t)((tot < 200 && r() % 3 == 0) ? R(r, 0, 4) : 0); tot += bits[k]; } for (int k = 1; k <= 16; k++) put8(b, bits[k]); for (int k = 0; k < tot; k++) put8(b, (int)r() & 255); seg(0xC4, b); }
    if (r() % 3 == 0) { std::vector<uint8_t> b; put16be(b, (int)R(r, 0, 600)); seg(0xDD, b); }
    { // SOS: ids conocidos de SOF
      int ns = (int)R(r, 1, nc); std::vector<uint8_t> b; put8(b, ns); size_t sofpos = 0; (void)sofpos;
      for (int i = 0; i < ns; i++) { put8(b, i + 1); put8(b, ((int)R(r, 0, 3) << 4) | (int)R(r, 0, 3)); } put8(b, (int)R(r, 0, 63) * (r() % 3 == 0)); put8(b, (int)R(r, 0, 63)); put8(b, ((int)R(r, 0, 3) << 4) | (int)R(r, 0, 3)); seg(0xDA, b); }
    int nd = (int)R(r, 0, 12); for (int i = 0; i < nd; i++) put8(s, (int)(r() % 256) == 0xFF ? 0 : (int)(r() & 255)); if (r() % 3 == 0) { put8(s, 0xFF); put8(s, 0xD9); }
}
static void add_markerrd_tests() {
    // jinit_marker_reader (0x5CC7D0)
    g_tests.push_back({"jinit_marker_reader", 0x5CC7D0, "jdmarker", "jinit_marker_reader", [](Rng& r, Ctx& x) {
        auto c = rand_dinfo(r); c->mem = &g_mem; uint8_t* m = g_arena + ((g_top + 7) & ~(size_t)7);
        static const std::pair<uint32_t, std::pair<const char*, const char*>> fps[] = {{0x5CC7A0, {"jdmarker", "reset_marker_reader"}}, {0x5CC290, {"jdmarker", "read_markers"}}, {0x5CC610, {"jdmarker", "read_restart_marker"}}, {0x5CC050, {"jdmarker", "skip_variable"}}, {0x5CBF00, {"jdmarker", "get_interesting_appn"}}};
        Ctx::FpSite s0; for (auto& f : fps) s0.map.push_back(f);
        for (size_t off : {(size_t)0, (size_t)4, (size_t)8, (size_t)0x18, (size_t)0x1C, (size_t)0x20, (size_t)0x24, (size_t)0x28, (size_t)0x2C, (size_t)0x30, (size_t)0x34, (size_t)0x38, (size_t)0x3C, (size_t)0x40, (size_t)0x44, (size_t)0x48, (size_t)0x4C, (size_t)0x50, (size_t)0x54, (size_t)0x58}) { Ctx::FpSite q = s0; q.off = (size_t)(m - g_arena) + off; x.fps.push_back(q); }
        x.call = [c](void* f) { ((void(__cdecl*)(j_decompress_ptr))f)(c); return 0; };
        x.dump = [](std::vector<uint8_t>&) {};
    }, 1});
    for (int kind = 0; kind < 2; kind++) {
        g_tests.push_back({kind == 0 ? "read_markers" : "get_interesting_appn", kind == 0 ? 0x5CC290u : 0x5CBF00u, "jdmarker", kind == 0 ? "read_markers" : "get_interesting_appn", [kind](Rng& r, Ctx& x) {
            x.check_ret = true; if (kind == 1) x.ret_mask = 0xFF;
            auto c = new_dinfo(); c->mem = &g_mem; c->is_decompressor = TRUE;
            std::vector<uint8_t> s;
            bool mutated = false;
            if (kind == 0) {
                gen_jpeg_header(r, s, false);
                if (r() % 3 == 0) { mutated = true; int m = (int)R(r, 1, 3); for (int k = 0; k < m && !s.empty(); k++) { switch (r() % 4) { case 0: s[r() % s.size()] ^= (uint8_t)(1 << (r() % 8)); break; case 1: s[r() % s.size()] = (uint8_t)r(); break; case 2: s.resize(R(r, 1, (uint32_t)s.size())); break; default: { size_t p = r() % s.size(); s.insert(s.begin() + p, (uint8_t)r()); } } } }
            } else { // segmento APP0/APP14 suelto: el marcador ya se leyo (unread_marker) y el flujo empieza en la longitud
                int mk = (r() % 2) ? 0xE0 : 0xEE; std::vector<uint8_t> b; if (mk == 0xE0) { b = {'J', 'F', 'I', 'F', 0, (uint8_t)R(r, 1, 1), (uint8_t)R(r, 0, 2), (uint8_t)R(r, 0, 2), (uint8_t)(r() % 256), (uint8_t)r(), (uint8_t)(r() % 256), (uint8_t)r(), (uint8_t)R(r, 0, 2), (uint8_t)R(r, 0, 2)}; if (r() % 3 == 0) b[R(r, 0, 4)] ^= 1; }
                else { b = {'A', 'd', 'o', 'b', 'e', 0, 100, 0, 0, 0, 0, (uint8_t)R(r, 0, 2)}; if (r() % 3 == 0) b[R(r, 0, 4)] ^= 1; }
                if (r() % 4 == 0) b.resize(R(r, 0, (uint32_t)b.size() + 6)); if (r() % 5 == 0) { size_t add = R(r, 1, 20); for (size_t i = 0; i < add; i++) b.push_back((uint8_t)r()); }
                put16be(s, (int)b.size() + 2); s.insert(s.end(), b.begin(), b.end()); put8(s, 0xFF); put8(s, 0xD9); c->unread_marker = mk;
                if (r() % 8 == 0) { s[1] = (uint8_t)(s[1] + (r() % 3)); mutated = true; } mutated = mutated || false;
            }
            uint32_t len = (uint32_t)s.size(); uint8_t* buf = (uint8_t*)A(len + 16, 8); memcpy(buf, s.data(), len);
            SrcState* st = (SrcState*)A(sizeof(SrcState), 8); memset(st, 0, sizeof *st); st->base = buf;
            uint32_t pos = 0; while (pos < len && st->nchunks < 39) { uint32_t n = (r() % 3 == 0) ? R(r, 1, 3) : R(r, 1, 40); if (r() % 6 == 0) n = len; n = std::min(n, len - pos); st->off[st->nchunks] = pos; st->len[st->nchunks] = n; st->nchunks++; pos += n; }
            if (pos < len) { st->off[st->nchunks - 1] = st->off[st->nchunks - 1]; st->len[st->nchunks - 1] += len - pos; }
            if (st->nchunks == 0) { st->nchunks = 1; st->off[0] = 0; st->len[0] = 0; }
            st->suspend_at = (r() % 4 == 0 && st->nchunks > 1) ? R(r, 1, st->nchunks - 1) : 0xFFFFFFFFu;
            c->client_data = st; auto src = (struct jpeg_source_mgr*)A(sizeof(jpeg_source_mgr), 8); memset(src, 0, sizeof *src); c->src = src;
            src->next_input_byte = buf + st->off[0]; src->bytes_in_buffer = st->len[0]; src->init_source = mock_init_source; src->fill_input_buffer = mock_fill; src->skip_input_data = mock_skip; src->resync_to_restart = mock_resync; src->term_source = mock_term_source;
            // marcador: estructura privada de 0xA8 bytes con los punteros del lado correspondiente (se fijan en pre)
            uint8_t* mk = (uint8_t*)A(0xA8, 8); memset(mk, 0, 0xA8); c->marker = (struct jpeg_marker_reader*)mk;
            if (kind == 1) { ((uint8_t*)mk)[0xC] = 1; }
            if (getenv("DBGOFF")) printf("OFF c=%zu buf=%zu st=%zu src=%zu mk=%zu top=%zu len=%u\n", aoff(c), aoff(buf), aoff(st), aoff(src), aoff(mk), g_top, len);
            uint32_t sus = st->suspend_at; bool mut = mutated;
            static const std::pair<uint32_t, std::pair<const char*, const char*>> fpl[] = {{0x5CC050, {"jdmarker", "skip_variable"}}, {0x5CBF00, {"jdmarker", "get_interesting_appn"}}};
            for (size_t off = 0x18; off <= 0x58; off += 4) { Ctx::FpSite q; q.off = (size_t)(mk - g_arena) + off; for (auto& f : fpl) q.map.push_back(f); x.fps.push_back(q); }
            x.norm = [c](std::vector<uint8_t>& sn) { // huffval mas alla de sum(bits): en la biblioteca 6b es basura de la pila (get_dht copia un arreglo local sin inicializar)
                for (int i = 0; i < 8; i++) { JHUFF_TBL* h = i < 4 ? c->dc_huff_tbl_ptrs[i] : c->ac_huff_tbl_ptrs[i - 4]; (void)h; }
                auto rd32 = [&](size_t off) { uint32_t v = 0; if (off + 4 <= sn.size()) memcpy(&v, sn.data() + off, 4); return v; };
                for (int i = 0; i < 8; i++) { size_t po = (size_t)((uint8_t*)(i < 4 ? (void*)&c->dc_huff_tbl_ptrs[i] : (void*)&c->ac_huff_tbl_ptrs[i - 4]) - g_arena); uint32_t pv = rd32(po); if (!pv) continue; size_t to = pv - (uint32_t)(uintptr_t)g_arena; if (to + 274 > sn.size()) continue; int cnt = 0; for (int k = 1; k <= 16; k++) cnt += sn[to + k]; if (cnt > 256) cnt = 256; for (int k = cnt; k < 256; k++) sn[to + 17 + k] = 0; }
            };
            x.pre = [=](bool exe) { uint32_t skipv = exe ? 0x5CC050 : (uint32_t)(uintptr_t)lib("jdmarker", "skip_variable"), gi = exe ? 0x5CBF00 : (uint32_t)(uintptr_t)lib("jdmarker", "get_interesting_appn");
                W32(mk, 0x18) = skipv; for (int i = 0; i < 16; i++) W32(mk, 0x1C + 4 * i) = skipv; W32(mk, 0x1C) = gi; W32(mk, 0x54) = gi; };
            auto sbuf = std::make_shared<std::vector<uint8_t>>(s);
            x.call = [=](void* f) { return (int)((int(__cdecl*)(j_decompress_ptr))f)(c); };
            x.dump = [=](std::vector<uint8_t>& v) {
                put32(v, (uint32_t)kind); put32(v, mut); put32(v, sus != 0xFFFFFFFFu); put32(v, (uint32_t)sbuf->size()); putn(v, sbuf->data(), sbuf->size());
                // consumido = bytes entregados al analizador
                SrcState* S = (SrcState*)c->client_data; uint32_t consumed = S->off[S->cur] + S->len[S->cur] - (uint32_t)c->src->bytes_in_buffer; put32(v, consumed);
                put32(v, c->image_width); put32(v, c->image_height); put32(v, (uint32_t)c->data_precision); put32(v, (uint32_t)c->num_components); put32(v, c->progressive_mode & 0xFF); put32(v, c->arith_code & 0xFF);
                put32(v, c->saw_JFIF_marker & 0xFF); put32(v, c->JFIF_major_version); put32(v, c->JFIF_minor_version); put32(v, c->density_unit); put32(v, c->X_density); put32(v, c->Y_density);
                put32(v, c->saw_Adobe_marker & 0xFF); put32(v, c->Adobe_transform); put32(v, c->restart_interval); put32(v, (uint32_t)c->comps_in_scan);
                for (int i = 0; i < 4; i++) if (i < c->num_components && c->comp_info) { put32(v, (uint32_t)c->comp_info[i].component_id); put32(v, (uint32_t)c->comp_info[i].h_samp_factor); put32(v, (uint32_t)c->comp_info[i].v_samp_factor); put32(v, (uint32_t)c->comp_info[i].quant_tbl_no); } else for (int k = 0; k < 4; k++) put32(v, 0);
                for (int i = 0; i < 4; i++) if (i < c->comps_in_scan && c->cur_comp_info[i]) { put32(v, (uint32_t)c->cur_comp_info[i]->component_id); put32(v, (uint32_t)c->cur_comp_info[i]->dc_tbl_no); put32(v, (uint32_t)c->cur_comp_info[i]->ac_tbl_no); } else for (int k = 0; k < 3; k++) put32(v, 0);
                put32(v, (uint32_t)c->Ss); put32(v, (uint32_t)c->Se); put32(v, (uint32_t)c->Ah); put32(v, (uint32_t)c->Al); put32(v, (uint32_t)c->input_scan_number); put32(v, (uint32_t)c->unread_marker);
                for (int i = 0; i < 4; i++) { put32(v, c->quant_tbl_ptrs[i] != nullptr); if (c->quant_tbl_ptrs[i]) putn(v, c->quant_tbl_ptrs[i]->quantval, 128); }
                for (int i = 0; i < 8; i++) { JHUFF_TBL* h = i < 4 ? c->dc_huff_tbl_ptrs[i] : c->ac_huff_tbl_ptrs[i - 4]; put32(v, h != nullptr); if (h) { putn(v, h->bits, 17); putn(v, h->huffval, 256); } }
                put32(v, ((uint8_t*)c->marker)[0xC]); put32(v, ((uint8_t*)c->marker)[0xD]);
            };
        }, 4000});
    }
}


// ---- descompresion: Huffman secuencial (jdhuff.c): start_pass_huff_decoder, decode_mcu
// Codificador de referencia propio del banco (JPEG Anexo C/F, tablas de Anexo K con huffval permutado): el flujo de entrada es de verdad Huffman valido; los coeficientes codificados son el oraculo.
static const int g_nat[64] = {0,1,8,16,9,2,3,10,17,24,32,25,18,11,4,5,12,19,26,33,40,48,41,34,27,20,13,6,7,14,21,28,35,42,49,56,57,50,43,36,29,22,15,23,30,37,44,51,58,59,52,45,38,31,39,46,53,60,61,54,47,55,62,63};
struct HT { uint8_t bits[17]; uint8_t hv[256]; int n; uint32_t code[256]; uint8_t size[256]; int pos[256]; };
static void ht_make(Rng& r, HT& h, bool ac) {
    memset(&h, 0, sizeof h); for (int i = 0; i < 256; i++) h.pos[i] = -1;
    static const uint8_t bdc[17] = {0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}; static const uint8_t bac[17] = {0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d};
    memcpy(h.bits, ac ? bac : bdc, 17); std::vector<int> syms;
    if (ac) { syms.push_back(0x00); syms.push_back(0xF0); for (int run = 0; run < 16; run++) for (int sz = 1; sz <= 10; sz++) syms.push_back((run << 4) | sz); } else for (int i = 0; i < 12; i++) syms.push_back(i);
    for (size_t i = syms.size() - 1; i > 0; i--) std::swap(syms[i], syms[r() % (i + 1)]);
    h.n = (int)syms.size(); for (int i = 0; i < h.n; i++) h.hv[i] = (uint8_t)syms[i];
    uint32_t code = 0; int k = 0; for (int l = 1; l <= 16; l++) { for (int i = 0; i < h.bits[l]; i++) { h.code[k] = code; h.size[k] = (uint8_t)l; h.pos[h.hv[k]] = k; code++; k++; } code <<= 1; }
}
struct BitW { std::vector<uint8_t>* v; uint32_t acc = 0; int nb = 0;
    void put(uint32_t bits, int n) { for (int i = n - 1; i >= 0; i--) { acc = (acc << 1) | ((bits >> i) & 1); if (++nb == 8) { v->push_back((uint8_t)acc); if ((acc & 255) == 0xFF) v->push_back(0); acc = 0; nb = 0; } } }
    void align() { while (nb) put(1, 1); } };
static int cat(int v) { int a = v < 0 ? -v : v, n = 0; while (a) { n++; a >>= 1; } return n; }
static void put_val(BitW& w, int v, int n) { if (n) w.put((uint32_t)(v < 0 ? v - 1 : v) & ((1u << n) - 1), n); }
static void enc_block(BitW& w, const HT& dc, const HT& ac, const int16_t* blk, int& last_dc) {
    int diff = blk[0] - last_dc; last_dc = blk[0]; int s = cat(diff); int p = dc.pos[s]; w.put(dc.code[p], dc.size[p]); put_val(w, diff, s);
    int run = 0; for (int k = 1; k < 64; k++) { int v = blk[g_nat[k]]; if (v == 0) { run++; continue; } while (run > 15) { int q = ac.pos[0xF0]; w.put(ac.code[q], ac.size[q]); run -= 16; } int sz = cat(v); int q = ac.pos[(run << 4) | sz]; w.put(ac.code[q], ac.size[q]); put_val(w, v, sz); run = 0; }
    if (run > 0) { int q = ac.pos[0x00]; w.put(ac.code[q], ac.size[q]); }
}
static void add_huffd_tests() {
    for (int kind = 0; kind < 2; kind++) {
        g_tests.push_back({kind == 0 ? "start_pass_huff_decoder" : "decode_mcu", kind == 0 ? 0x5CA110u : 0x5C9CE0u, "jdhuff", kind == 0 ? "start_pass_huff_decoder" : "decode_mcu", [kind](Rng& r, Ctx& x) {
            x.check_ret = true; x.ret_mask = 0xFF;
            auto c = new_dinfo(); c->mem = &g_mem; ((void(__cdecl*)(j_decompress_ptr))lib("jdhuff", "jinit_huff_decoder"))(c);
            void* en = c->entropy;
            int nc = (int)R(r, 1, 3); c->comps_in_scan = nc; c->num_components = nc; auto ci = AT<jpeg_component_info>(4); memset(ci, 0, sizeof(jpeg_component_info) * 4); c->comp_info = ci;
            int tot = 0; for (int i = 0; i < nc; i++) { auto& p = ci[i]; p.component_id = i + 1; p.component_index = i; p.dc_tbl_no = (int)R(r, 0, 3); p.ac_tbl_no = (int)R(r, 0, 3); p.component_needed = (boolean)(r() % 6 != 0); int w = (int)R(r, 1, 2), h = (int)R(r, 1, 2); if (i == 2) w = h = 1; p.MCU_blocks = w * h; p.DCT_scaled_size = (r() % 10 == 0) ? 1 : 8; tot += p.MCU_blocks; c->cur_comp_info[i] = &p; }
            c->blocks_in_MCU = tot; { int b = 0; for (int i = 0; i < nc; i++) for (int k = 0; k < ci[i].MCU_blocks; k++) c->MCU_membership[b++] = i; }
            c->Ss = 0; c->Se = 63; c->Ah = c->Al = 0; if (r() % 12 == 0) { c->Ss = (int)R(r, 0, 2); c->Al = (int)R(r, 0, 1); }
            c->restart_interval = (r() % 5 < 3) ? 0 : R(r, 1, 3);
            // tablas Huffman: reales (Anexo K con huffval permutado); a veces se omite una tabla usada (error) o se deja sin permutar
            HT* dct = AT<HT>(4); HT* act = AT<HT>(4); bool drop_dc = false, drop_ac = false;
            for (int t = 0; t < 4; t++) { ht_make(r, dct[t], false); ht_make(r, act[t], true); auto d = AT<JHUFF_TBL>(); auto a = AT<JHUFF_TBL>(); memcpy(d->bits, dct[t].bits, 17); memcpy(d->huffval, dct[t].hv, 256); d->sent_table = 0; memcpy(a->bits, act[t].bits, 17); memcpy(a->huffval, act[t].hv, 256); a->sent_table = 0; c->dc_huff_tbl_ptrs[t] = d; c->ac_huff_tbl_ptrs[t] = a; }
            if (kind == 0 && r() % 8 == 0) { c->dc_huff_tbl_ptrs[R(r, 0, 3)] = nullptr; drop_dc = true; }
            if (kind == 0 && r() % 8 == 0) { c->ac_huff_tbl_ptrs[R(r, 0, 3)] = nullptr; drop_ac = true; }
            (void)drop_dc; (void)drop_ac;
            // marcador y estado de entropia
            uint8_t* mk = (uint8_t*)A(0xA8, 8); memset(mk, 0, 0xA8); c->marker = (struct jpeg_marker_reader*)mk;
            if (kind == 0) {
                x.call = [c](void* f) { ((void(__cdecl*)(j_decompress_ptr))f)(c); return 0; };
                x.dump = [c, en, nc, ci](std::vector<uint8_t>& v) {
                    put32(v, (uint32_t)nc); put32(v, (uint32_t)c->Ss); put32(v, (uint32_t)c->Se); put32(v, (uint32_t)c->Ah); put32(v, (uint32_t)c->Al); put32(v, c->restart_interval);
                    for (int i = 0; i < nc; i++) { put32(v, (uint32_t)ci[i].dc_tbl_no); put32(v, (uint32_t)ci[i].ac_tbl_no); }
                    for (int t = 0; t < 4; t++) { put32(v, c->dc_huff_tbl_ptrs[t] != nullptr); put32(v, c->ac_huff_tbl_ptrs[t] != nullptr); }
                    put32(v, W32(en, 0x24)); put32(v, (uint32_t)g_err_calls); put32(v, (uint32_t)c->err->msg_code);
                };
                return;
            }
            // datos: nm MCU codificados
            int nm = (int)R(r, 1, 4); std::vector<std::vector<int16_t>> blocks; std::vector<uint8_t> s; BitW w; w.v = &s; int last_dc[4] = {0, 0, 0, 0}; uint32_t rst = 0; uint32_t ri = c->restart_interval, togo = ri; bool valid = true;
            int nbm = c->blocks_in_MCU;
            for (int m = 0; m < nm; m++) {
                if (ri && togo == 0) { w.align(); s.push_back(0xFF); s.push_back((uint8_t)(0xD0 + (rst++ & 7))); for (int i = 0; i < 4; i++) last_dc[i] = 0; togo = ri; }
                for (int b = 0; b < nbm; b++) {
                    std::vector<int16_t> blk(64, 0); int ci_ = c->MCU_membership[b]; blk[0] = (int16_t)((r() % 3 == 0) ? (int)R(r, 0, 2000) - 1000 : (int)R(r, 0, 200) - 100);
                    int nzs = (int)R(r, 0, 10); for (int q = 0; q < nzs; q++) { int k = (int)R(r, 1, 63); int mag = (r() % 5 == 0) ? (int)R(r, 1, 1023) : (int)R(r, 1, 20); blk[g_nat[k]] = (int16_t)((r() & 1) ? mag : -mag); }
                    if (r() % 40 == 0) { for (int k = 1; k < 64; k++) blk[g_nat[k]] = (int16_t)((k & 1) ? 7 : -9); } // bloque denso (sin EOB)
                    blocks.push_back(blk);
                    enc_block(w, dct[ci[ci_].dc_tbl_no], act[ci[ci_].ac_tbl_no], blk.data(), last_dc[ci_]);
                }
                if (ri) togo--;
            }
            w.align(); s.push_back(0xFF); s.push_back(0xD9);
            if (r() % 3 == 0) { valid = false; int mm = (int)R(r, 1, 3); for (int k = 0; k < mm && !s.empty(); k++) { switch (r() % 4) { case 0: s[r() % s.size()] ^= (uint8_t)(1 << (r() % 8)); break; case 1: s[r() % s.size()] = (uint8_t)r(); break; case 2: s.resize(R(r, 0, (uint32_t)s.size())); break; default: { size_t p = r() % s.size(); s.insert(s.begin() + p, (uint8_t)r()); } } } }

            uint32_t len = (uint32_t)s.size(); uint8_t* buf = (uint8_t*)A(len + 16, 8); memcpy(buf, s.data(), len);
            SrcState* st = (SrcState*)A(sizeof(SrcState), 8); memset(st, 0, sizeof *st); st->base = buf;
            uint32_t pos = 0; while (pos < len && st->nchunks < 39) { uint32_t n = (r() % 3 == 0) ? R(r, 1, 3) : R(r, 1, 40); if (r() % 6 == 0) n = len; n = std::min(n, len - pos); st->off[st->nchunks] = pos; st->len[st->nchunks] = n; st->nchunks++; pos += n; }
            if (pos < len) st->len[st->nchunks - 1] += len - pos; if (st->nchunks == 0) { st->nchunks = 1; st->off[0] = 0; st->len[0] = 0; }
            bool susp = (r() % 5 == 0 && st->nchunks > 1); st->suspend_at = susp ? R(r, 1, st->nchunks - 1) : 0xFFFFFFFFu;
            c->client_data = st; auto src = (struct jpeg_source_mgr*)A(sizeof(jpeg_source_mgr), 8); memset(src, 0, sizeof *src); c->src = src;
            src->next_input_byte = buf + st->off[0]; src->bytes_in_buffer = st->len[0]; src->init_source = mock_init_source; src->fill_input_buffer = mock_fill; src->skip_input_data = mock_skip; src->resync_to_restart = mock_resync; src->term_source = mock_term_source;
            std::vector<JBLOCKROW*> mcus; for (int m = 0; m < nm; m++) { JBLOCKROW* rows = AT<JBLOCKROW>(10); for (int b = 0; b < 10; b++) { rows[b] = AT<JBLOCK>(); memset(rows[b], 0, sizeof(JBLOCK)); } mcus.push_back(rows); }
            x.pre = [mk, c](bool exe) { W32(mk, 8) = exe ? 0x5CC610u : (uint32_t)(uintptr_t)lib("jdmarker", "read_restart_marker"); ((void(__cdecl*)(j_decompress_ptr))(exe ? (void*)0x5CA110 : lib("jdhuff", "start_pass_huff_decoder")))(c); }; // las tablas derivadas se construyen con la start_pass de cada lado
            x.fps.push_back({aoff(mk) + 8, {{0x5CC610, {"jdmarker", "read_restart_marker"}}}});
            auto exp = std::make_shared<std::vector<std::vector<int16_t>>>(blocks); auto sb = std::make_shared<std::vector<uint8_t>>(s);
            bool needed[4], acn[4]; for (int i = 0; i < nc; i++) { needed[i] = ci[i].component_needed; acn[i] = ci[i].DCT_scaled_size > 1; }
            auto done = std::make_shared<int>(0); auto okall = std::make_shared<int>(1);
            x.call = [=](void* f) { *done = 0; int okc = 1; for (int m = 0; m < nm; m++) { boolean rr = ((boolean(__cdecl*)(j_decompress_ptr, JBLOCKROW*))f)(c, mcus[m]); if (!(rr & 0xFF)) { okc = 0; break; } (*done)++; } *okall = okc; return okc; };
            x.dump = [=](std::vector<uint8_t>& v) {
                put32(v, (uint32_t)nm); put32(v, (uint32_t)nbm); put32(v, valid); put32(v, susp); put32(v, (uint32_t)*done); put32(v, (uint32_t)*okall); put32(v, ri);
                for (int b = 0; b < nbm; b++) put32(v, (uint32_t)c->MCU_membership[b]);
                for (int i = 0; i < 3; i++) { put32(v, i < nc ? needed[i] : 0); put32(v, i < nc ? acn[i] : 0); }
                for (int m = 0; m < nm; m++) for (int b = 0; b < nbm; b++) putn(v, &(*exp)[m * nbm + b][0], 128);
                for (int m = 0; m < nm; m++) for (int b = 0; b < nbm; b++) putn(v, mcus[m][b], 128);
            };
        }, 4000});
    }
}


// ---- compresion: Huffman (jchuff.c start_pass_huff): tablas derivadas del codificador y modo de estadisticas
static void add_huffe_tests() {
    g_tests.push_back({"start_pass_huff", 0x5C5090, "jchuff", "start_pass_huff", [](Rng& r, Ctx& x) {
        auto c = new_cinfo(); c->mem = &g_mem; ((void(__cdecl*)(j_compress_ptr))lib("jchuff", "jinit_huff_encoder"))(c); void* en = c->entropy;
        int nc = (int)R(r, 1, 4); c->comps_in_scan = nc; c->num_components = nc; auto ci = AT<jpeg_component_info>(4); memset(ci, 0, sizeof(jpeg_component_info) * 4); c->comp_info = ci;
        bool gather = (r() % 3 == 0); c->restart_interval = (r() % 2) ? 0 : R(r, 1, 700);
        bool idx_bad = false;
        for (int i = 0; i < nc; i++) { int d = (int)R(r, 0, 3), a = (int)R(r, 0, 3); if (r() % 14 == 0) { d = (r() & 1) ? -1 : (int)R(r, 4, 7); idx_bad = true; } if (r() % 14 == 0) { a = (r() & 1) ? -2 : (int)R(r, 4, 9); idx_bad = true; } ci[i].dc_tbl_no = d; ci[i].ac_tbl_no = a; c->cur_comp_info[i] = &ci[i]; }
        HT* dct = AT<HT>(4); HT* act = AT<HT>(4); bool corrupt = false;
        for (int t = 0; t < 4; t++) {
            ht_make(r, dct[t], false); ht_make(r, act[t], true); auto d = AT<JHUFF_TBL>(); auto a = AT<JHUFF_TBL>(); memcpy(d->bits, dct[t].bits, 17); memcpy(d->huffval, dct[t].hv, 256); d->sent_table = 0; memcpy(a->bits, act[t].bits, 17); memcpy(a->huffval, act[t].hv, 256); a->sent_table = 0;
            if (r() % 25 == 0) { corrupt = true; a->bits[R(r, 1, 16)] = (UINT8)R(r, 100, 255); }
            if (r() % 25 == 0) { corrupt = true; d->bits[R(r, 1, 4)] = (UINT8)R(r, 3, 9); }
            c->dc_huff_tbl_ptrs[t] = (r() % 30 == 0) ? nullptr : d; c->ac_huff_tbl_ptrs[t] = (r() % 30 == 0) ? nullptr : a;
        }
        // tablas ya reservadas de una pasada anterior (con contenido basura)
        for (int t = 0; t < 4; t++) {
            if (r() % 4 == 0) { void* p = A(1284, 8); randwords(r, p, 1284); W32(en, 0x2C + 4 * t) = (uint32_t)(uintptr_t)p; }
            if (r() % 4 == 0) { void* p = A(1284, 8); randwords(r, p, 1284); W32(en, 0x3C + 4 * t) = (uint32_t)(uintptr_t)p; }
            if (r() % 4 == 0) { void* p = A(0x410, 8); randwords(r, p, 0x410); W32(en, 0x4C + 4 * t) = (uint32_t)(uintptr_t)p; }
            if (r() % 4 == 0) { void* p = A(0x410, 8); randwords(r, p, 0x410); W32(en, 0x5C + 4 * t) = (uint32_t)(uintptr_t)p; }
        }
        for (int off : {4, 8}) { Ctx::FpSite q; q.off = aoff(en) + off; if (off == 4) { q.map.push_back({0x5C4C80, {"jchuff", "encode_mcu_gather"}}); q.map.push_back({0x5C49C0, {"jchuff", "encode_mcu_huff"}}); } else { q.map.push_back({0x5C4FA0, {"jchuff", "finish_pass_gather"}}); q.map.push_back({0x5C4AF0, {"jchuff", "finish_pass_huff"}}); } x.fps.push_back(q); }
        auto pre = std::make_shared<std::vector<uint8_t>>(); for (int t = 0; t < 4; t++) { putn(*pre, dct[t].bits, 17); putn(*pre, dct[t].hv, 256); }
        std::vector<int> dcn, acn; for (int i = 0; i < nc; i++) { dcn.push_back(ci[i].dc_tbl_no); acn.push_back(ci[i].ac_tbl_no); }
        auto hv = std::make_shared<std::vector<uint8_t>>(); for (int t = 0; t < 4; t++) { putn(*hv, c->dc_huff_tbl_ptrs[t] ? c->dc_huff_tbl_ptrs[t]->bits : (uint8_t*)"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 17); putn(*hv, c->dc_huff_tbl_ptrs[t] ? c->dc_huff_tbl_ptrs[t]->huffval : (uint8_t*)A(256, 1), 256); put32(*hv, c->dc_huff_tbl_ptrs[t] != nullptr); putn(*hv, c->ac_huff_tbl_ptrs[t] ? c->ac_huff_tbl_ptrs[t]->bits : (uint8_t*)"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 17); putn(*hv, c->ac_huff_tbl_ptrs[t] ? c->ac_huff_tbl_ptrs[t]->huffval : (uint8_t*)A(256, 1), 256); put32(*hv, c->ac_huff_tbl_ptrs[t] != nullptr); }
        x.call = [=](void* f) { ((void(__cdecl*)(j_compress_ptr, boolean))f)(c, (boolean)gather); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) {
            put32(v, gather); put32(v, c->restart_interval); put32(v, (uint32_t)nc); put32(v, idx_bad); put32(v, corrupt);
            for (int i = 0; i < nc; i++) { put32(v, (uint32_t)dcn[i]); put32(v, (uint32_t)acn[i]); }
            putn(v, hv->data(), hv->size());
            put32(v, W32(en, 0xC)); put32(v, W32(en, 0x10)); for (int i = 0; i < 4; i++) put32(v, W32(en, 0x14 + 4 * i)); put32(v, W32(en, 0x24)); put32(v, W32(en, 0x28));
            for (int t = 0; t < 4; t++) { for (int k = 0; k < 2; k++) { uint32_t pv = W32(en, (k ? 0x3C : 0x2C) + 4 * t); put32(v, pv != 0); if (pv) { uint8_t* tb = (uint8_t*)(uintptr_t)pv; putn(v, tb, 1280); } } }
            for (int t = 0; t < 4; t++) { for (int k = 0; k < 2; k++) { uint32_t pv = W32(en, (k ? 0x5C : 0x4C) + 4 * t); put32(v, pv != 0); if (pv) { uint8_t* tb = (uint8_t*)(uintptr_t)pv; putn(v, tb, 1028); } } }
        };
    }, 4000});
}


// ---- gestor de memoria: self_destruct (jmemmgr.c) con malloc/free de la biblioteca (jmemnobs) y del EXE redirigidos a un asignador del banco con registro
//   - biblioteca: w_jmemnobs.c se compila con -Dmalloc=lib3b_malloc -Dfree=lib3b_free (solo en el banco; el .c oficial no se toca)
//   - EXE: las 4 llamadas del CRT en jpeg_get/free_small/large (0x5CF800-0x5CF835) se desvian a los mismos ganchos
static uint32_t* g_flog = nullptr; static int g_tag_pool = -1, g_tag_kind = 0;
enum { FL_NMALLOC = 1, FL_NFREE = 0, FL_NCLOSE = 2, FL_MAL = 8, FL_FREE = 8 + 3 * 64, FL_CLOSE = 8 + 3 * 64 + 128 };
extern "C" void* __cdecl lib3b_malloc(size_t n) { void* p = A(n, 8); if (g_flog && g_flog[FL_NMALLOC] < 64) { uint32_t k = g_flog[FL_NMALLOC]++; uint32_t* e = g_flog + FL_MAL + 3 * k; e[0] = (uint32_t)(uintptr_t)p; e[1] = (uint32_t)g_tag_pool; e[2] = (uint32_t)g_tag_kind; } return p; }
extern "C" void __cdecl lib3b_free(void* p) { if (g_flog && g_flog[FL_NFREE] < 128) { uint32_t k = g_flog[FL_NFREE]++; g_flog[FL_FREE + k] = (uint32_t)(uintptr_t)p; } }
static void* __cdecl exe_malloc_hook(size_t n, int) { return lib3b_malloc(n); }
static void __cdecl exe_free_hook(void* p) { lib3b_free(p); }
static void __cdecl mock_bs_close(j_common_ptr, void* info) { if (g_flog && g_flog[FL_NCLOSE] < 128) { uint32_t k = g_flog[FL_NCLOSE]++; g_flog[FL_CLOSE + k] = (uint32_t)(uintptr_t)info; } }
static void add_selfd_tests() {
    g_tests.push_back({"self_destruct", 0x5CF690, "jmemmgr", "self_destruct", [](Rng& r, Ctx& x) {
        auto c = new_cinfo(); g_flog = (uint32_t*)A(4 * (FL_CLOSE + 128), 8); memset(g_flog, 0, 4 * (FL_CLOSE + 128));
        c->mem = nullptr; g_tag_pool = -1; g_tag_kind = 9; ((void(__cdecl*)(j_common_ptr))lib("jmemmgr", "jinit_memory_mgr"))((j_common_ptr)c);
        jpeg_memory_mgr* m = c->mem; std::vector<void*> sa, ba; int nops = (int)R(r, 1, 12);
        for (int i = 0; i < nops; i++) {
            int pool = (int)R(r, 0, 1); g_tag_pool = pool; uint32_t k = r() % 4; if (getenv("DBGSD")) printf("   op %d pool %d k %u mem=%p\n", i, pool, k, (void*)m);
            if (k == 0) { g_tag_kind = 0; m->alloc_small((j_common_ptr)c, pool, R(r, 1, 400)); }
            else if (k == 1) { g_tag_kind = 1; m->alloc_large((j_common_ptr)c, pool, R(r, 1, 3000)); }
            else if (k == 2) { g_tag_kind = 0; g_tag_pool = 1; sa.push_back(m->request_virt_sarray((j_common_ptr)c, 1, FALSE, R(r, 1, 50), R(r, 1, 50), R(r, 1, 4))); }
            else { g_tag_kind = 0; g_tag_pool = 1; ba.push_back(m->request_virt_barray((j_common_ptr)c, 1, FALSE, R(r, 1, 5), R(r, 1, 50), R(r, 1, 4))); }
        }
        for (void* p : sa) if (r() % 2) { ((uint8_t*)p)[0x22] = 1; W32(p, 0x30) = (uint32_t)(uintptr_t)mock_bs_close; }
        for (void* p : ba) if (r() % 2) { ((uint8_t*)p)[0x22] = 1; W32(p, 0x30) = (uint32_t)(uintptr_t)mock_bs_close; }
        auto sas = std::make_shared<std::vector<uint32_t>>(); for (void* p : sa) sas->push_back((uint32_t)(uintptr_t)p + 0x28); auto bas = std::make_shared<std::vector<uint32_t>>(); for (void* p : ba) bas->push_back((uint32_t)(uintptr_t)p + 0x28);
        auto sopen = std::make_shared<std::vector<uint32_t>>(); for (void* p : sa) sopen->push_back(((uint8_t*)p)[0x22]); auto bopen = std::make_shared<std::vector<uint32_t>>(); for (void* p : ba) bopen->push_back(((uint8_t*)p)[0x22]);
        uint32_t* L = g_flog; uint32_t mgr = (uint32_t)(uintptr_t)m; (void)mgr;
        x.call = [=](void* f) { ((void(__cdecl*)(j_common_ptr))f)((j_common_ptr)c); return 0; };
        x.dump = [=](std::vector<uint8_t>& v) {
            put32(v, (uint32_t)(uintptr_t)c->mem); put32(v, L[FL_NMALLOC]); for (uint32_t k = 0; k < L[FL_NMALLOC]; k++) { put32(v, L[FL_MAL + 3 * k]); put32(v, L[FL_MAL + 3 * k + 1]); put32(v, L[FL_MAL + 3 * k + 2]); }
            put32(v, L[FL_NFREE]); for (uint32_t k = 0; k < L[FL_NFREE]; k++) put32(v, L[FL_FREE + k]);
            put32(v, L[FL_NCLOSE]); for (uint32_t k = 0; k < L[FL_NCLOSE]; k++) put32(v, L[FL_CLOSE + k]);
            put32(v, (uint32_t)sas->size()); for (size_t k = 0; k < sas->size(); k++) { put32(v, (*sas)[k]); put32(v, (*sopen)[k]); }
            put32(v, (uint32_t)bas->size()); for (size_t k = 0; k < bas->size(); k++) { put32(v, (*bas)[k]); put32(v, (*bopen)[k]); }
        };
    }, 4000});
}

// ---- jdcoefct: dummy_consume_data (0x5C8E10): devuelve JPEG_SUSPENDED
static void add_dummy_tests() {
    g_tests.push_back({"dummy_consume_data", 0x5C8E10, "jdcoefct", "dummy_consume_data", [](Rng& r, Ctx& x) {
        x.check_ret = true; auto c = rand_dinfo(r); x.call = [c](void* f) { return (int)((int(__cdecl*)(j_decompress_ptr))f)(c); }; x.dump = [](std::vector<uint8_t>&) {};
    }, 1});
}

// ---------------------------------------------------------------- ejecucion
static void fill_rand(Rng& r, size_t n) { for (size_t i = 0; i < n; i += 4) { uint32_t x = r(); memcpy(g_arena + i, &x, std::min<size_t>(4, n - i)); } }

int main(int argc, char** argv) {
    if (argc < 3) { puts("uso: bench <salida> <exe> [--filtro s] [--n N]"); return 2; }
    std::string outdir = argv[1], flt; int N = 20000; unsigned seed = 12345;
    for (int i = 3; i < argc; i++) { if (!strcmp(argv[i], "--filtro") && i + 1 < argc) flt = argv[++i]; else if (!strcmp(argv[i], "--n") && i + 1 < argc) N = atoi(argv[++i]); else if (!strcmp(argv[i], "--dct") && i + 1 < argc) g_opt_dct = atoi(argv[++i]); else if (!strcmp(argv[i], "--seed") && i + 1 < argc) seed = (unsigned)atoi(argv[++i]); }
    if (const char* m = getenv("LIB3A_MUT")) lib3a_mut = atoi(m);
    setvbuf(stdout, NULL, _IONBF, 0);
    _controlfp(_PC_24, _MCW_PC); // x87 a 24 bits de mantisa, como el juego
    printf("precision FPU: control word 0x%04x\n", _control87(0, 0));
    if (!map_exe(argv[2])) return 2;
    patch_call(0x5CD7C2, (void*)mock_sprintf); patch_call(0x5CD7DC, (void*)mock_sprintf); // format_message llama al sprintf del CRT del EXE (no inicializado en el banco): se redirige al del banco
    patch_call(0x5CF807, (void*)exe_malloc_hook); patch_call(0x5CF815, (void*)exe_free_hook); patch_call(0x5CF827, (void*)exe_malloc_hook); patch_call(0x5CF835, (void*)exe_free_hook); // jpeg_get/free_small/large del EXE -> asignador del banco
    init_mocks();
    CreateDirectoryA(outdir.c_str(), nullptr);
    add_color_tests(); add_dct_tests(); add_upsample_tests();
    add_idct_tests(); add_state_tests(); add_downsample_tests(); add_idctmgr_tests(); add_prep_tests(); add_onepass_tests(); add_marker_tests(); add_virt_tests(); add_markerrd_tests(); add_huffd_tests(); add_huffe_tests(); add_selfd_tests(); add_dummy_tests();
    add_dcolor_tests(0x5C8FB0, 0x5C90D0, 0x5C9150, 0x5C9180, 0x5C91E0, 0x5C9340, 0x5C8EE0);
    int total_fail = 0;
    printf("%-24s %-10s %9s %9s %9s\n", "funcion", "direccion", "pares", "difs", "errores");
    for (auto& t : g_tests) {
        if (!flt.empty() && (flt[0] == '=' ? std::string(t.name) != flt.substr(1) : std::string(t.name).find(flt) == std::string::npos)) continue;
        void* exef = (void*)(uintptr_t)t.exe_addr; void* libf = lib(t.lib_file, t.lib_name);
        Rng rng(seed ^ (t.exe_addr * 2654435761u));
        std::vector<uint8_t> trace; int diffs = 0, errs = 0, pairs = 0;
        for (int k = 0; k < N; k++) {
            g_top = 0; Rng fr(rng()); fill_rand(fr, std::min(g_hi + 4096, sizeof g_arena)); g_err_calls = 0; g_err_code = 0;
            Ctx ctx; if (setjmp(g_jb) != 0) { printf("ERROR en el montaje del caso %d de %s (codigo %d)\n", k, t.name, g_err_code); exit(3); } t.build(rng, ctx); size_t top0 = g_top; size_t lim0 = std::min(sizeof g_arena, std::max(top0, g_hi) + 65536); std::vector<uint8_t> S0(g_arena, g_arena + lim0); // se restaura tambien lo que queda por encima de top0: las asignaciones hechas durante la llamada no deben heredar restos del otro lado
            int ret[2] = {0, 0}, ec[2] = {0, 0}, ecode[2] = {0, 0}; size_t topE[2] = {0, 0}; std::vector<uint8_t> snap[2];
            for (int side = 0; side < 2; side++) {
                memcpy(g_arena, S0.data(), lim0); g_top = top0; g_err_calls = 0; g_err_code = 0;
                void* f = side == 0 ? exef : libf; if (ctx.pre) ctx.pre(side == 0);
                if (setjmp(g_jb) == 0) ret[side] = ctx.call(f); else { ec[side] = 1; }
                ecode[side] = g_err_code; topE[side] = g_top; snap[side].assign(g_arena, g_arena + g_top);
                if (side == 0) { /* conservar */ }
            }
            if (ctx.norm) for (int s = 0; s < 2; s++) ctx.norm(snap[s]);
            // punteros a funcion: el EXE escribe direcciones propias y la biblioteca las suyas; se sustituyen por el indice del par declarado (si el valor no es ninguno de los declarados se deja crudo y delata la diferencia)
            for (auto& fp : ctx.fps) for (int s = 0; s < 2; s++) if (fp.off + 4 <= snap[s].size()) {
                uint32_t v; memcpy(&v, snap[s].data() + fp.off, 4); uint32_t id = v;
                for (size_t q = 0; q < fp.map.size(); q++) { uint32_t cand = s == 0 ? fp.map[q].first : (uint32_t)(uintptr_t)lib(fp.map[q].second.first, fp.map[q].second.second); if (v == cand) { id = 0xF00D0000u + (uint32_t)q; break; } }
                memcpy(snap[s].data() + fp.off, &id, 4);
            }
            // mascara
            for (auto& m : ctx.masks) for (int s = 0; s < 2; s++) if (m.off + m.len <= snap[s].size()) memset(snap[s].data() + m.off, 0, m.len);
            pairs++;
            bool same = ec[0] == ec[1] && ecode[0] == ecode[1] && topE[0] == topE[1] && snap[0] == snap[1] && (!ctx.check_ret || (((uint32_t)ret[0] ^ (uint32_t)ret[1]) & ctx.ret_mask) == 0);
            if (!same) {
                diffs++; if (diffs <= 3) {
                    size_t off = 0; size_t n = std::min(snap[0].size(), snap[1].size()); while (off < n && snap[0][off] == snap[1][off]) off++;
                    printf("  DIF %s caso %d: err %d/%d cod %d/%d top %zu/%zu primer byte distinto en %zu (EXE %02x LIB %02x)\n", t.name, k, ec[0], ec[1], ecode[0], ecode[1], topE[0], topE[1], off, off < n ? snap[0][off] : 0, off < n ? snap[1][off] : 0);
                    if (diffs == 1) { size_t o0 = off & ~(size_t)15; for (int sd = 0; sd < 2; sd++) { printf("    %s@%zu:", sd ? "LIB" : "EXE", o0); for (size_t q = o0; q < o0 + 48 && q < snap[sd].size(); q++) printf(" %02x", snap[sd][q]); printf("\n"); } }
                }
            }
            if (ec[0]) errs++;
            if (k < t.trace_cases && ctx.dump) {
                // la traza se escribe con el estado final del lado LIB (igual al del EXE si no hay diferencias); se restaura el del lado LIB
                std::vector<uint8_t> rec; put32(rec, ec[1]); put32(rec, (uint32_t)ret[1]); ctx.dump(rec); put32(trace, (uint32_t)rec.size()); trace.insert(trace.end(), rec.begin(), rec.end());
            }
        }
        std::string fn = outdir + "\\trace_" + t.name + ".bin"; FILE* f = fopen(fn.c_str(), "wb"); if (f) { fwrite(trace.data(), 1, trace.size(), f); fclose(f); }
        printf("%-24s 0x%06X %9d %9d %9d\n", t.name, t.exe_addr, pairs, diffs, errs); fflush(stdout);
        total_fail += diffs;
    }
    printf("TOTAL de diferencias: %d\n", total_fail);
    return total_fail ? 1 : 0;
}
