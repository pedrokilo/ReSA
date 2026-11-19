#pragma once

// zlib 1.1.3 (inflate: infblock.c, infcodes.c, inftrees.c, inffast.c, infutil.c) enlazado estaticamente en el EXE, entre 0x7E9430 y 0x7EB5B0 (el EXE lleva "inflate 1.1.3 Copyright 1995-1998 Mark Adler" en 0x8841B1; no hay cadena 1.1.4; correccion del lote LIB1A).
// No es codigo del juego ni de RenderWare (el listado de RenderWare lo atribuia por vecindad a RtQuat). Lote R5A (sin validar); ver reversing/PendienteValidar/R5A.md.
// Las estructuras reproducen la disposicion del EXE (z_stream 0x34 bytes, inflate_blocks_state 0x40, inflate_codes_state 0x1C, inflate_huft 8). Los nombres llevan prefijo
// para no chocar con las de la zlib del proyecto. Todas las funciones son cdecl y los callbacks de reserva/liberacion/suma de comprobacion tambien.

void ZlibInflateInjectHooks();

struct ZInflateStream;
struct ZInflateBlocksState;
struct ZInflateCodesState;

// inflate_huft: { Byte exop; Byte bits; uInt base; }
struct ZInflateHuft {
    uint8  exop;
    uint8  bits;
    uint32 base;
};
static_assert(sizeof(ZInflateHuft) == 8 && offsetof(ZInflateHuft, base) == 4);

typedef void*  (__cdecl* ZInflateAllocFn)(void* opaque, uint32 items, uint32 size);
typedef void   (__cdecl* ZInflateFreeFn)(void* opaque, void* address);
typedef uint32 (__cdecl* ZInflateCheckFn)(uint32 check, const uint8* buffer, uint32 length);

// z_stream de zlib 1.1.3
struct ZInflateStream {
    uint8*          next_in;    // +0x00
    uint32          avail_in;   // +0x04
    uint32          total_in;   // +0x08
    uint8*          next_out;   // +0x0C
    uint32          avail_out;  // +0x10
    uint32          total_out;  // +0x14
    const char*     msg;        // +0x18
    void*           state;      // +0x1C
    ZInflateAllocFn zalloc;     // +0x20
    ZInflateFreeFn  zfree;      // +0x24
    void*           opaque;     // +0x28
    int32           data_type;  // +0x2C
    uint32          adler;      // +0x30
    uint32          reserved;   // +0x34
};
static_assert(sizeof(ZInflateStream) == 0x38 && offsetof(ZInflateStream, zalloc) == 0x20 && offsetof(ZInflateStream, adler) == 0x30);

// inflate_blocks_state de zlib 1.1.3 (la union `sub` esta en +4)
struct ZInflateBlocksState {
    uint32 mode;                        // +0x00
    union {
        uint32 left;                    // +0x04 (STORED)
        struct {
            uint32         table;       // +0x04
            uint32         index;       // +0x08
            uint32*        blens;       // +0x0C
            uint32         bb;          // +0x10
            ZInflateHuft*  tb;          // +0x14
        } trees;
        struct {
            ZInflateCodesState* codes;  // +0x04
        } decode;
    } sub;
    uint32          last;               // +0x18
    uint32          bitk;               // +0x1C
    uint32          bitb;               // +0x20
    ZInflateHuft*   hufts;              // +0x24
    uint8*          window;             // +0x28
    uint8*          end;                // +0x2C
    uint8*          read;               // +0x30
    uint8*          write;              // +0x34
    ZInflateCheckFn checkfn;            // +0x38
    uint32          check;              // +0x3C
};
static_assert(sizeof(ZInflateBlocksState) == 0x40 && offsetof(ZInflateBlocksState, last) == 0x18 && offsetof(ZInflateBlocksState, check) == 0x3C);

// inflate_codes_state de zlib 1.1.3
struct ZInflateCodesState {
    uint32 mode;                        // +0x00
    uint32 len;                         // +0x04
    union {
        struct {
            const ZInflateHuft* tree;   // +0x08
            uint32              need;   // +0x0C
        } code;
        uint32 lit;                     // +0x08
        struct {
            uint32 get;                 // +0x08
            uint32 dist;                // +0x0C
        } copy;
    } sub;
    uint8               lbits;          // +0x10
    uint8               dbits;          // +0x11
    const ZInflateHuft* ltree;          // +0x14
    const ZInflateHuft* dtree;          // +0x18
};
static_assert(sizeof(ZInflateCodesState) == 0x1C && offsetof(ZInflateCodesState, ltree) == 0x14);

// 0x7E9430: inflate_blocks_reset(s, z, c)
void __cdecl InflateBlocksReset(ZInflateBlocksState* s, ZInflateStream* z, uint32* c);
// 0x7E94B0: inflate_blocks_new(z, checkfn, window size)
ZInflateBlocksState* __cdecl InflateBlocksNew(ZInflateStream* z, ZInflateCheckFn checkfn, uint32 windowSize);
// 0x7E9570: inflate_blocks(s, z, r)
int __cdecl InflateBlocks(ZInflateBlocksState* s, ZInflateStream* z, int r);
// 0x7EA1C0: inflate_blocks_free(s, z)
int __cdecl InflateBlocksFree(ZInflateBlocksState* s, ZInflateStream* z);
// 0x7EA250: inflate_set_dictionary(s, dictionary, length)
void __cdecl InflateSetDictionary(ZInflateBlocksState* s, const uint8* dictionary, uint32 length);
// 0x7EA2A0: inflate_codes_new(bl, bd, tl, td, z)
ZInflateCodesState* __cdecl InflateCodesNew(uint32 bl, uint32 bd, const ZInflateHuft* tl, const ZInflateHuft* td, ZInflateStream* z);
// 0x7EA2E0: inflate_codes(s, z, r)
int __cdecl InflateCodes(ZInflateBlocksState* s, ZInflateStream* z, int r);
// 0x7EA9E0: inflate_codes_free(c, z)
void __cdecl InflateCodesFree(ZInflateCodesState* c, ZInflateStream* z);
// 0x7EAA00: inflate_trees_bits(c, bb, tb, hp, z)
int __cdecl InflateTreesBits(uint32* c, uint32* bb, ZInflateHuft** tb, ZInflateHuft* hp, ZInflateStream* z);
// 0x7EAAB0: huft_build(b, n, s, d, e, t, m, hp, hn, v)
int __cdecl InflateHuftBuild(uint32* b, uint32 n, uint32 s, const uint32* d, const uint32* e, ZInflateHuft** t, uint32* m, ZInflateHuft* hp, uint32* hn, uint32* v);
// 0x7EAF60: inflate_trees_dynamic(nl, nd, c, bl, bd, tl, td, hp, z)
int __cdecl InflateTreesDynamic(uint32 nl, uint32 nd, uint32* c, uint32* bl, uint32* bd, ZInflateHuft** tl, ZInflateHuft** td, ZInflateHuft* hp, ZInflateStream* z);
// 0x7EB0F0: inflate_trees_fixed(bl, bd, tl, td, z): el EXE no lee `z` (los llamadores lo empujan igualmente); devuelve las tablas fijas ya construidas del .data (0x8E0FC0..0x8E1FC8)
int __cdecl InflateTreesFixed(uint32* bl, uint32* bd, ZInflateHuft** tl, ZInflateHuft** td, ZInflateStream* z);
// 0x7EB120: inflate_flush(s, z, r)
int __cdecl InflateFlush(ZInflateBlocksState* s, ZInflateStream* z, int r);
// 0x7EB260: inflate_fast(bl, bd, tl, td, s, z)
int __cdecl InflateFast(uint32 bl, uint32 bd, const ZInflateHuft* tl, const ZInflateHuft* td, ZInflateBlocksState* s, ZInflateStream* z);
