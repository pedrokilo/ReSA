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
static void* __cdecl mock_alloc_small(j_common_ptr, int, size_t n) { return A(n, 8); }
static void* __cdecl mock_alloc_large(j_common_ptr, int, size_t n) { return A(n, 8); }
static JSAMPARRAY __cdecl mock_alloc_sarray(j_common_ptr, int, JDIMENSION w, JDIMENSION h) {
    JSAMPARRAY a = (JSAMPARRAY)A(sizeof(JSAMPROW) * h, 8); for (JDIMENSION i = 0; i < h; i++) a[i] = (JSAMPROW)A(w, 8); return a;
}
static jpeg_memory_mgr g_mem; static jpeg_error_mgr g_errmgr;
static void init_mocks() {
    memset(&g_mem, 0, sizeof g_mem); g_mem.alloc_small = mock_alloc_small; g_mem.alloc_large = mock_alloc_large; g_mem.alloc_sarray = mock_alloc_sarray;
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
    bool check_ret = false;
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

// ---------------------------------------------------------------- ejecucion
static void fill_rand(Rng& r, size_t n) { for (size_t i = 0; i < n; i += 4) { uint32_t x = r(); memcpy(g_arena + i, &x, std::min<size_t>(4, n - i)); } }

int main(int argc, char** argv) {
    if (argc < 3) { puts("uso: bench <salida> <exe> [--filtro s] [--n N]"); return 2; }
    std::string outdir = argv[1], flt; int N = 20000; unsigned seed = 12345;
    for (int i = 3; i < argc; i++) { if (!strcmp(argv[i], "--filtro") && i + 1 < argc) flt = argv[++i]; else if (!strcmp(argv[i], "--n") && i + 1 < argc) N = atoi(argv[++i]); else if (!strcmp(argv[i], "--dct") && i + 1 < argc) g_opt_dct = atoi(argv[++i]); else if (!strcmp(argv[i], "--seed") && i + 1 < argc) seed = (unsigned)atoi(argv[++i]); }
    if (const char* m = getenv("LIB3A_MUT")) lib3a_mut = atoi(m);
    _controlfp(_PC_24, _MCW_PC); // x87 a 24 bits de mantisa, como el juego
    printf("precision FPU: control word 0x%04x\n", _control87(0, 0));
    if (!map_exe(argv[2])) return 2;
    init_mocks();
    CreateDirectoryA(outdir.c_str(), nullptr);
    add_color_tests(); add_dct_tests(); add_upsample_tests();
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
            Ctx ctx; t.build(rng, ctx); size_t top0 = g_top; std::vector<uint8_t> S0(g_arena, g_arena + top0);
            int ret[2] = {0, 0}, ec[2] = {0, 0}, ecode[2] = {0, 0}; size_t topE[2] = {0, 0}; std::vector<uint8_t> snap[2];
            for (int side = 0; side < 2; side++) {
                memcpy(g_arena, S0.data(), top0); g_top = top0; g_err_calls = 0; g_err_code = 0;
                void* f = side == 0 ? exef : libf; if (ctx.pre) ctx.pre(side == 0);
                if (setjmp(g_jb) == 0) ret[side] = ctx.call(f); else { ec[side] = 1; }
                ecode[side] = g_err_code; topE[side] = g_top; snap[side].assign(g_arena, g_arena + g_top);
                if (side == 0) { /* conservar */ }
            }
            // mascara
            for (auto& m : ctx.masks) for (int s = 0; s < 2; s++) if (m.off + m.len <= snap[s].size()) memset(snap[s].data() + m.off, 0, m.len);
            pairs++;
            bool same = ec[0] == ec[1] && ecode[0] == ecode[1] && topE[0] == topE[1] && snap[0] == snap[1] && (!ctx.check_ret || ret[0] == ret[1]);
            if (!same) {
                diffs++; if (diffs <= 3) {
                    size_t off = 0; size_t n = std::min(snap[0].size(), snap[1].size()); while (off < n && snap[0][off] == snap[1][off]) off++;
                    printf("  DIF %s caso %d: err %d/%d cod %d/%d top %zu/%zu primer byte distinto en %zu (EXE %02x LIB %02x)\n", t.name, k, ec[0], ec[1], ecode[0], ecode[1], topE[0], topE[1], off, off < n ? snap[0][off] : 0, off < n ? snap[1][off] : 0);
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
