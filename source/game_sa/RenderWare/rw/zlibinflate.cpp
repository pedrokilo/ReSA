/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: ZlibInflate
 * Reconstrucción: COMPLETA — 14/14 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/14 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "zlibinflate.h"

// Lote R5A (sin validar). zlib 1.1.3 enlazado en el EXE (inflate). Cada cuerpo se transcribio del desensamblado (0x7E9430..0x7EB5B0) y coincide con infblock.c, infcodes.c,
// inftrees.c, inffast.c e infutil.c de zlib 1.1.3 (MANY = 1440, BMAX = 15, sin PKZIP_BUG_WORKAROUND). Todo es entero: no hay x87.
// Las tablas constantes (mascaras, orden de longitudes, bases/extras) y los mensajes de error se leen del .data/.rdata del EXE por direccion fija, de modo que `z->msg`
// apunta a las mismas cadenas que antes. Las funciones se llaman entre si por su direccion en C++ (todas estan enganchadas).

namespace {
// Modos de inflate_blocks_state
enum { B_TYPE = 0, B_LENS, B_STORED, B_TABLE, B_BTREE, B_DTREE, B_CODES, B_DRY, B_DONE, B_BAD };
// Modos de inflate_codes_state
enum { C_START = 0, C_LEN, C_LENEXT, C_DIST, C_DISTEXT, C_COPY, C_LIT, C_WASH, C_END, C_BADCODE };

constexpr int ZI_OK            = 0;
constexpr int ZI_STREAM_END    = 1;
constexpr int ZI_STREAM_ERROR  = -2;
constexpr int ZI_DATA_ERROR    = -3;
constexpr int ZI_MEM_ERROR     = -4;
constexpr int ZI_BUF_ERROR     = -5;
constexpr uint32 MANY         = 1440;
constexpr uint32 BMAX         = 15;

// Datos del EXE
const uint32* const inflate_mask = reinterpret_cast<const uint32*>(0x8E21B8); // 17 entradas: (1 << i) - 1
const uint32* const border       = reinterpret_cast<const uint32*>(0x884160); // 19 entradas: orden de las longitudes de codigo
const uint32* const cplens       = reinterpret_cast<const uint32*>(0x8841E0); // 31 entradas
const uint32* const cplext       = reinterpret_cast<const uint32*>(0x884260); // 31 entradas
const uint32* const cpdist       = reinterpret_cast<const uint32*>(0x8842E0); // 30 entradas
const uint32* const cpdext       = reinterpret_cast<const uint32*>(0x884358); // 30 entradas

// Mensajes (cadenas del EXE)
const char* const MSG_INVALID_BLOCK_TYPE     = reinterpret_cast<const char*>(0x8E0F74); // "invalid block type"
const char* const MSG_INVALID_STORED_LENGTHS = reinterpret_cast<const char*>(0x8E0F54); // "invalid stored block lengths"
const char* const MSG_TOO_MANY_SYMBOLS       = reinterpret_cast<const char*>(0x8E0F30); // "too many length or distance symbols"
const char* const MSG_INVALID_BIT_REPEAT     = reinterpret_cast<const char*>(0x8E0F14); // "invalid bit length repeat"
const char* const MSG_INVALID_DISTANCE       = reinterpret_cast<const char*>(0x8E0FA4); // "invalid distance code"
const char* const MSG_INVALID_LITLEN         = reinterpret_cast<const char*>(0x8E0F88); // "invalid literal/length code"
const char* const MSG_OVERSUB_BITLEN         = reinterpret_cast<const char*>(0x8E20EC); // "oversubscribed dynamic bit lengths tree"
const char* const MSG_INCOMPLETE_BITLEN      = reinterpret_cast<const char*>(0x8E20C8); // "incomplete dynamic bit lengths tree"
const char* const MSG_OVERSUB_LITLEN         = reinterpret_cast<const char*>(0x8E2134); // "oversubscribed literal/length tree"
const char* const MSG_INCOMPLETE_LITLEN      = reinterpret_cast<const char*>(0x8E2114); // "incomplete literal/length tree"
const char* const MSG_OVERSUB_DIST           = reinterpret_cast<const char*>(0x8E2198); // "oversubscribed distance tree"
const char* const MSG_INCOMPLETE_DIST        = reinterpret_cast<const char*>(0x8E217C); // "incomplete distance tree"
const char* const MSG_EMPTY_DIST             = reinterpret_cast<const char*>(0x8E2158); // "empty distance tree with lengths"

inline void* ZAlloc(ZInflateStream* z, uint32 items, uint32 size) { return z->zalloc(z->opaque, items, size); }
inline void  ZFree(ZInflateStream* z, void* address) { z->zfree(z->opaque, address); }
} // namespace


#ifdef GTASA_WITH_ZLIB_LIBPNG
// Lote LIB1A: con la opcion CMake GTASA_WITH_ZLIB_LIBPNG, los 14 cuerpos delegan en zlib 1.1.3 compilada desde la fuente oficial (libs/zlib-1.1.3).
// Se declaran aqui los prototipos con punteros opacos (no se incluye zlib.h para no chocar con los typedefs Byte/uInt/uLong del proyecto).
// La estructura z_stream, inflate_blocks_state e inflate_codes_state de la biblioteca tienen la misma disposicion que las Z* de zlibinflate.h (tamanos 0x38, 0x40, 0x1C).
extern "C" {
    void  inflate_blocks_reset(void* s, void* z, unsigned long* c);
    void* inflate_blocks_new(void* z, void* checkfn, unsigned int w);
    int   inflate_blocks(void* s, void* z, int r);
    int   inflate_blocks_free(void* s, void* z);
    void  inflate_set_dictionary(void* s, const unsigned char* d, unsigned int n);
    void* inflate_codes_new(unsigned int bl, unsigned int bd, void* tl, void* td, void* z);
    int   inflate_codes(void* s, void* z, int r);
    void  inflate_codes_free(void* c, void* z);
    int   inflate_trees_bits(unsigned int* c, unsigned int* bb, void** tb, void* hp, void* z);
    int   huft_build(unsigned int* b, unsigned int n, unsigned int s, const unsigned int* d, const unsigned int* e, void** t, unsigned int* m, void* hp, unsigned int* hn, unsigned int* v); // inftrees.c se compila con -Dlocal= (CMake) para exportarla
    int   inflate_trees_dynamic(unsigned int nl, unsigned int nd, unsigned int* c, unsigned int* bl, unsigned int* bd, void** tl, void** td, void* hp, void* z);
    int   inflate_trees_fixed(unsigned int* bl, unsigned int* bd, void** tl, void** td, void* z);
    int   inflate_flush(void* s, void* z, int r);
    int   inflate_fast(unsigned int bl, unsigned int bd, void* tl, void* td, void* s, void* z);
}
#endif

void ZlibInflateInjectHooks() {
    RH_ScopedNamespaceName("ZlibInflate");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(InflateBlocksReset, 0x7E9430);
    RH_ScopedGlobalInstall(InflateBlocksNew, 0x7E94B0);
    RH_ScopedGlobalInstall(InflateBlocks, 0x7E9570);
    RH_ScopedGlobalInstall(InflateBlocksFree, 0x7EA1C0);
    RH_ScopedGlobalInstall(InflateSetDictionary, 0x7EA250);
    RH_ScopedGlobalInstall(InflateCodesNew, 0x7EA2A0);
    RH_ScopedGlobalInstall(InflateCodes, 0x7EA2E0);
    RH_ScopedGlobalInstall(InflateCodesFree, 0x7EA9E0);
    RH_ScopedGlobalInstall(InflateTreesBits, 0x7EAA00);
    RH_ScopedGlobalInstall(InflateHuftBuild, 0x7EAAB0);
    RH_ScopedGlobalInstall(InflateTreesDynamic, 0x7EAF60);
    RH_ScopedGlobalInstall(InflateTreesFixed, 0x7EB0F0);
    RH_ScopedGlobalInstall(InflateFlush, 0x7EB120);
    RH_ScopedGlobalInstall(InflateFast, 0x7EB260);
}

// Macros de infutil.h de zlib 1.1.3 (variables locales s, z, r, b, k, p, n, q, m).
#define WAVAIL (uint32)(q < s->read ? s->read - q - 1 : s->end - q)
#define LOADIN { p = z->next_in; n = z->avail_in; b = s->bitb; k = s->bitk; }
#define LOADOUT { q = s->write; m = WAVAIL; }
#define LOAD { LOADIN LOADOUT }
#define UPDATEIN { s->bitb = b; s->bitk = k; z->avail_in = n; z->total_in += (uint32)(p - z->next_in); z->next_in = p; }
#define UPDATEOUT { s->write = q; }
#define UPDATE { UPDATEIN UPDATEOUT }
#define LEAVE { UPDATE return InflateFlush(s, z, r); }
#define WRAP { if (q == s->end && s->read != s->window) { q = s->window; m = WAVAIL; } }
#define FLUSH { UPDATEOUT r = InflateFlush(s, z, r); LOADOUT }
#define NEEDBYTE { if (n) r = ZI_OK; else LEAVE }
#define NEXTBYTE (n--, *p++)
#define NEEDBITS(j) { while (k < (j)) { NEEDBYTE b |= ((uint32)NEXTBYTE) << k; k += 8; } }
#define GRABBITS(j) { while (k < (j)) { b |= ((uint32)NEXTBYTE) << k; k += 8; } }
#define DUMPBITS(j) { b >>= (j); k -= (j); }
#define NEEDOUT { if (m == 0) { WRAP if (m == 0) { FLUSH WRAP if (m == 0) LEAVE } } r = ZI_OK; }
#define OUTBYTE(a) { *q++ = (uint8)(a); m--; }
#define UNGRAB { c = z->avail_in - n; c = (k >> 3) < c ? k >> 3 : c; n += c; p -= c; k -= c << 3; }

// 0x7E9430
void __cdecl InflateBlocksReset(ZInflateBlocksState* s, ZInflateStream* z, uint32* c) {
#ifdef GTASA_WITH_ZLIB_LIBPNG
    ::inflate_blocks_reset(s, z, reinterpret_cast<unsigned long*>(c));
#else
    if (c != nullptr) {
        *c = s->check;
    }
    if (s->mode == B_BTREE || s->mode == B_DTREE) {
        ZFree(z, s->sub.trees.blens);
    }
    if (s->mode == B_CODES) {
        InflateCodesFree(s->sub.decode.codes, z);
    }
    s->mode  = B_TYPE;
    s->bitk  = 0;
    s->bitb  = 0;
    s->read  = s->write = s->window;
    if (s->checkfn != nullptr) {
        z->adler = s->check = s->checkfn(0, nullptr, 0);
    }
#endif
}

// 0x7E94B0
ZInflateBlocksState* __cdecl InflateBlocksNew(ZInflateStream* z, ZInflateCheckFn checkfn, uint32 windowSize) {
#ifdef GTASA_WITH_ZLIB_LIBPNG
    return static_cast<ZInflateBlocksState*>(::inflate_blocks_new(z, reinterpret_cast<void*>(checkfn), windowSize));
#else
    auto* s = static_cast<ZInflateBlocksState*>(ZAlloc(z, 1, sizeof(ZInflateBlocksState)));
    if (s == nullptr) {
        return s;
    }
    s->hufts = static_cast<ZInflateHuft*>(ZAlloc(z, sizeof(ZInflateHuft), MANY));
    if (s->hufts == nullptr) {
        ZFree(z, s);
        return nullptr;
    }
    s->window = static_cast<uint8*>(ZAlloc(z, 1, windowSize));
    if (s->window == nullptr) {
        ZFree(z, s->hufts);
        ZFree(z, s);
        return nullptr;
    }
    s->end     = s->window + windowSize;
    s->checkfn = checkfn;
    s->mode    = B_TYPE;
    InflateBlocksReset(s, z, nullptr);
    return s;
#endif
}

// 0x7E9570
int __cdecl InflateBlocks(ZInflateBlocksState* s, ZInflateStream* z, int r) {
#ifdef GTASA_WITH_ZLIB_LIBPNG
    return ::inflate_blocks(s, z, r);
#else
    uint32 t; // temporal
    uint32 b; // bitb
    uint32 k; // bits en bitb
    uint8* p; // entrada
    uint32 n; // bytes disponibles
    uint8* q; // escritura en la ventana
    uint32 m; // bytes hasta el final de la ventana o el puntero de lectura

    LOAD

    while (true) {
        switch (s->mode) {
        case B_TYPE:
            NEEDBITS(3)
            t = b & 7;
            s->last = t & 1;
            switch (t >> 1) {
            case 0: // stored
                DUMPBITS(3)
                t = k & 7; // al limite de byte
                DUMPBITS(t)
                s->mode = B_LENS;
                break;
            case 1: { // fixed
                uint32 bl, bd;
                ZInflateHuft *tl, *td;
                InflateTreesFixed(&bl, &bd, &tl, &td, z);
                s->sub.decode.codes = InflateCodesNew(bl, bd, tl, td, z);
                if (s->sub.decode.codes == nullptr) {
                    r = ZI_MEM_ERROR;
                    LEAVE
                }
                DUMPBITS(3)
                s->mode = B_CODES;
                break;
            }
            case 2: // dynamic
                DUMPBITS(3)
                s->mode = B_TABLE;
                break;
            case 3: // illegal
                DUMPBITS(3)
                s->mode = B_BAD;
                z->msg = MSG_INVALID_BLOCK_TYPE;
                r = ZI_DATA_ERROR;
                LEAVE
            }
            break;
        case B_LENS:
            NEEDBITS(32)
            if ((((~b) >> 16) & 0xFFFF) != (b & 0xFFFF)) {
                s->mode = B_BAD;
                z->msg = MSG_INVALID_STORED_LENGTHS;
                r = ZI_DATA_ERROR;
                LEAVE
            }
            s->sub.left = b & 0xFFFF;
            b = k = 0;
            s->mode = s->sub.left ? B_STORED : (s->last ? B_DRY : B_TYPE);
            break;
        case B_STORED:
            if (n == 0) {
                LEAVE
            }
            NEEDOUT
            t = s->sub.left;
            if (t > n) t = n;
            if (t > m) t = m;
            memcpy(q, p, t);
            p += t;  n -= t;
            q += t;  m -= t;
            if ((s->sub.left -= t) != 0) {
                break;
            }
            s->mode = s->last ? B_DRY : B_TYPE;
            break;
        case B_TABLE:
            NEEDBITS(14)
            s->sub.trees.table = t = b & 0x3FFF;
            if ((t & 0x1F) > 29 || ((t >> 5) & 0x1F) > 29) {
                s->mode = B_BAD;
                z->msg = MSG_TOO_MANY_SYMBOLS;
                r = ZI_DATA_ERROR;
                LEAVE
            }
            t = 258 + (t & 0x1F) + ((t >> 5) & 0x1F);
            s->sub.trees.blens = static_cast<uint32*>(ZAlloc(z, t, sizeof(uint32)));
            if (s->sub.trees.blens == nullptr) {
                r = ZI_MEM_ERROR;
                LEAVE
            }
            DUMPBITS(14)
            s->sub.trees.index = 0;
            s->mode = B_BTREE;
            [[fallthrough]];
        case B_BTREE:
            while (s->sub.trees.index < 4 + (s->sub.trees.table >> 10)) {
                NEEDBITS(3)
                s->sub.trees.blens[border[s->sub.trees.index++]] = b & 7;
                DUMPBITS(3)
            }
            while (s->sub.trees.index < 19) {
                s->sub.trees.blens[border[s->sub.trees.index++]] = 0;
            }
            s->sub.trees.bb = 7;
            t = InflateTreesBits(s->sub.trees.blens, &s->sub.trees.bb, &s->sub.trees.tb, s->hufts, z);
            if (t != ZI_OK) {
                ZFree(z, s->sub.trees.blens);
                r = static_cast<int>(t);
                if (r == ZI_DATA_ERROR) {
                    s->mode = B_BAD;
                }
                LEAVE
            }
            s->sub.trees.index = 0;
            s->mode = B_DTREE;
            [[fallthrough]];
        case B_DTREE:
            while (t = s->sub.trees.table, s->sub.trees.index < 258 + (t & 0x1F) + ((t >> 5) & 0x1F)) {
                ZInflateHuft* h;
                uint32 i, j, c;

                t = s->sub.trees.bb;
                NEEDBITS(t)
                h = s->sub.trees.tb + (b & inflate_mask[t]);
                t = h->bits;
                c = h->base;
                if (c < 16) {
                    DUMPBITS(t)
                    s->sub.trees.blens[s->sub.trees.index++] = c;
                } else { // c == 16..18
                    i = c == 18 ? 7 : c - 14;
                    j = c == 18 ? 11 : 3;
                    NEEDBITS(t + i)
                    DUMPBITS(t)
                    j += b & inflate_mask[i];
                    DUMPBITS(i)
                    i = s->sub.trees.index;
                    t = s->sub.trees.table;
                    if (i + j > 258 + (t & 0x1F) + ((t >> 5) & 0x1F) || (c == 16 && i < 1)) {
                        ZFree(z, s->sub.trees.blens);
                        s->mode = B_BAD;
                        z->msg = MSG_INVALID_BIT_REPEAT;
                        r = ZI_DATA_ERROR;
                        LEAVE
                    }
                    c = c == 16 ? s->sub.trees.blens[i - 1] : 0;
                    do {
                        s->sub.trees.blens[i++] = c;
                    } while (--j);
                    s->sub.trees.index = i;
                }
            }
            s->sub.trees.tb = nullptr;
            {
                uint32 bl, bd;
                ZInflateHuft *tl, *td;
                ZInflateCodesState* c;

                bl = 9; // debe ser <= 9
                bd = 6; // debe ser <= 6
                t = s->sub.trees.table;
                t = InflateTreesDynamic(257 + (t & 0x1F), 1 + ((t >> 5) & 0x1F), s->sub.trees.blens, &bl, &bd, &tl, &td, s->hufts, z);
                ZFree(z, s->sub.trees.blens);
                if (t != static_cast<uint32>(ZI_OK)) {
                    if (t == static_cast<uint32>(ZI_DATA_ERROR)) {
                        s->mode = B_BAD;
                    }
                    r = static_cast<int>(t);
                    LEAVE
                }
                c = InflateCodesNew(bl, bd, tl, td, z);
                if (c == nullptr) {
                    r = ZI_MEM_ERROR;
                    LEAVE
                }
                s->sub.decode.codes = c;
            }
            s->mode = B_CODES;
            [[fallthrough]];
        case B_CODES:
            UPDATE
            r = InflateCodes(s, z, r);
            if (r != ZI_STREAM_END) {
                return InflateFlush(s, z, r);
            }
            r = ZI_OK;
            InflateCodesFree(s->sub.decode.codes, z);
            LOAD
            if (!s->last) {
                s->mode = B_TYPE;
                break;
            }
            s->mode = B_DRY;
            [[fallthrough]];
        case B_DRY:
            FLUSH
            if (s->read != s->write) {
                LEAVE
            }
            s->mode = B_DONE;
            [[fallthrough]];
        case B_DONE:
            r = ZI_STREAM_END;
            LEAVE
        case B_BAD:
            r = ZI_DATA_ERROR;
            LEAVE
        default:
            r = ZI_STREAM_ERROR;
            LEAVE
        }
    }
#endif
}

// 0x7EA1C0
int __cdecl InflateBlocksFree(ZInflateBlocksState* s, ZInflateStream* z) {
#ifdef GTASA_WITH_ZLIB_LIBPNG
    return ::inflate_blocks_free(s, z);
#else
    InflateBlocksReset(s, z, nullptr);
    ZFree(z, s->window);
    ZFree(z, s->hufts);
    ZFree(z, s);
    return ZI_OK;
#endif
}

// 0x7EA250
void __cdecl InflateSetDictionary(ZInflateBlocksState* s, const uint8* dictionary, uint32 length) {
#ifdef GTASA_WITH_ZLIB_LIBPNG
    ::inflate_set_dictionary(s, dictionary, length);
#else
    memcpy(s->window, dictionary, length);
    s->read = s->write = s->window + length;
#endif
}

// 0x7EA2A0
ZInflateCodesState* __cdecl InflateCodesNew(uint32 bl, uint32 bd, const ZInflateHuft* tl, const ZInflateHuft* td, ZInflateStream* z) {
#ifdef GTASA_WITH_ZLIB_LIBPNG
    return static_cast<ZInflateCodesState*>(::inflate_codes_new(bl, bd, const_cast<ZInflateHuft*>(tl), const_cast<ZInflateHuft*>(td), z));
#else
    auto* c = static_cast<ZInflateCodesState*>(ZAlloc(z, 1, sizeof(ZInflateCodesState)));
    if (c != nullptr) {
        c->mode  = C_START;
        c->lbits = static_cast<uint8>(bl);
        c->dbits = static_cast<uint8>(bd);
        c->ltree = tl;
        c->dtree = td;
    }
    return c;
#endif
}

// 0x7EA2E0
int __cdecl InflateCodes(ZInflateBlocksState* s, ZInflateStream* z, int r) {
#ifdef GTASA_WITH_ZLIB_LIBPNG
    return ::inflate_codes(s, z, r);
#else
    uint32 j;               // temporal
    const ZInflateHuft* t;  // puntero temporal
    uint32 e;               // bits extra u operacion
    uint32 b;               // bitb
    uint32 k;               // bits en bitb
    uint8* p;               // entrada
    uint32 n;               // bytes disponibles
    uint8* q;               // escritura en la ventana
    uint32 m;               // bytes hasta el final de la ventana o el puntero de lectura
    uint8* f;               // origen de la copia
    ZInflateCodesState* c = s->sub.decode.codes;

    LOAD

    while (true) {
        switch (c->mode) {
        case C_START:
            if (m >= 258 && n >= 10) {
                UPDATE
                r = InflateFast(c->lbits, c->dbits, c->ltree, c->dtree, s, z);
                LOAD
                if (r != ZI_OK) {
                    c->mode = r == ZI_STREAM_END ? C_WASH : C_BADCODE;
                    break;
                }
            }
            c->sub.code.need = c->lbits;
            c->sub.code.tree = c->ltree;
            c->mode = C_LEN;
            [[fallthrough]];
        case C_LEN:
            j = c->sub.code.need;
            NEEDBITS(j)
            t = c->sub.code.tree + (b & inflate_mask[j]);
            DUMPBITS(t->bits)
            e = t->exop;
            if (e == 0) { // literal
                c->sub.lit = t->base;
                c->mode = C_LIT;
                break;
            }
            if (e & 16) { // longitud
                c->sub.copy.get = e & 15;
                c->len = t->base;
                c->mode = C_LENEXT;
                break;
            }
            if ((e & 64) == 0) { // siguiente tabla
                c->sub.code.need = e;
                c->sub.code.tree = t + t->base;
                break;
            }
            if (e & 32) { // fin de bloque
                c->mode = C_WASH;
                break;
            }
            c->mode = C_BADCODE;
            z->msg = MSG_INVALID_LITLEN;
            r = ZI_DATA_ERROR;
            LEAVE
        case C_LENEXT:
            j = c->sub.copy.get;
            NEEDBITS(j)
            c->len += b & inflate_mask[j];
            DUMPBITS(j)
            c->sub.code.need = c->dbits;
            c->sub.code.tree = c->dtree;
            c->mode = C_DIST;
            [[fallthrough]];
        case C_DIST:
            j = c->sub.code.need;
            NEEDBITS(j)
            t = c->sub.code.tree + (b & inflate_mask[j]);
            DUMPBITS(t->bits)
            e = t->exop;
            if (e & 16) { // distancia
                c->sub.copy.get = e & 15;
                c->sub.copy.dist = t->base;
                c->mode = C_DISTEXT;
                break;
            }
            if ((e & 64) == 0) { // siguiente tabla
                c->sub.code.need = e;
                c->sub.code.tree = t + t->base;
                break;
            }
            c->mode = C_BADCODE;
            z->msg = MSG_INVALID_DISTANCE;
            r = ZI_DATA_ERROR;
            LEAVE
        case C_DISTEXT:
            j = c->sub.copy.get;
            NEEDBITS(j)
            c->sub.copy.dist += b & inflate_mask[j];
            DUMPBITS(j)
            c->mode = C_COPY;
            [[fallthrough]];
        case C_COPY:
            f = static_cast<uint32>(q - s->window) < c->sub.copy.dist ? s->end - (c->sub.copy.dist - static_cast<uint32>(q - s->window)) : q - c->sub.copy.dist;
            while (c->len) {
                NEEDOUT
                OUTBYTE(*f++)
                if (f == s->end) {
                    f = s->window;
                }
                c->len--;
            }
            c->mode = C_START;
            break;
        case C_LIT:
            NEEDOUT
            OUTBYTE(c->sub.lit)
            c->mode = C_START;
            break;
        case C_WASH:
            if (k > 7) { // devuelve el byte sobrante, si lo hay
                k -= 8;
                n++;
                p--;
            }
            FLUSH
            if (s->read != s->write) {
                LEAVE
            }
            c->mode = C_END;
            [[fallthrough]];
        case C_END:
            r = ZI_STREAM_END;
            LEAVE
        case C_BADCODE:
            r = ZI_DATA_ERROR;
            LEAVE
        default:
            r = ZI_STREAM_ERROR;
            LEAVE
        }
    }
#endif
}

// 0x7EA9E0
void __cdecl InflateCodesFree(ZInflateCodesState* c, ZInflateStream* z) {
#ifdef GTASA_WITH_ZLIB_LIBPNG
    ::inflate_codes_free(c, z);
#else
    ZFree(z, c);
#endif
}

// 0x7EAA00
int __cdecl InflateTreesBits(uint32* c, uint32* bb, ZInflateHuft** tb, ZInflateHuft* hp, ZInflateStream* z) {
#ifdef GTASA_WITH_ZLIB_LIBPNG
    return ::inflate_trees_bits(c, bb, reinterpret_cast<void**>(tb), hp, z);
#else
    int r;
    uint32 hn = 0;
    uint32* v = static_cast<uint32*>(ZAlloc(z, 19, sizeof(uint32)));
    if (v == nullptr) {
        return ZI_MEM_ERROR;
    }
    r = InflateHuftBuild(c, 19, 19, nullptr, nullptr, tb, bb, hp, &hn, v);
    if (r == ZI_DATA_ERROR) {
        z->msg = MSG_OVERSUB_BITLEN;
    } else if (r == ZI_BUF_ERROR || *bb == 0) {
        z->msg = MSG_INCOMPLETE_BITLEN;
        r = ZI_DATA_ERROR;
    }
    ZFree(z, v);
    return r;
#endif
}

// 0x7EAAB0
int __cdecl InflateHuftBuild(uint32* b, uint32 n, uint32 s, const uint32* d, const uint32* e, ZInflateHuft** t, uint32* m, ZInflateHuft* hp, uint32* hn, uint32* v) {
#ifdef GTASA_WITH_ZLIB_LIBPNG
    return ::huft_build(b, n, s, d, e, reinterpret_cast<void**>(t), m, hp, hn, v);
#else
    uint32 a;              // contador de codigos de longitud k
    uint32 c[BMAX + 1];    // tabla de cuentas por longitud
    uint32 f;              // i se repite en la tabla cada f entradas
    int g;                 // longitud maxima de codigo
    int h;                 // nivel de tabla
    uint32 i;              // contador, codigo actual
    uint32 j;              // contador
    int k;                 // bits del codigo actual
    int l;                 // bits por tabla (se devuelve en m)
    uint32 mask;           // (1 << w) - 1
    uint32* p;             // puntero en c[], b[] o v[]
    ZInflateHuft* q;       // tabla actual
    ZInflateHuft r;        // entrada de tabla
    ZInflateHuft* u[BMAX]; // pila de tablas
    int w;                 // bits antes de esta tabla == (l * h)
    uint32 x[BMAX + 1];    // desplazamientos de bit, luego pila de codigos
    uint32* xp;            // puntero en x[]
    int y;                 // codigos ficticios anadidos
    uint32 z;              // entradas de la tabla actual

    // cuenta por longitud de bit
    p = c;
    for (int idx = 0; idx < 16; idx++) {
        *p++ = 0;
    }
    p = b;
    i = n;
    do {
        c[*p++]++;
    } while (--i);
    if (c[0] == n) { // entrada nula: todos los codigos de longitud cero
        *t = nullptr;
        *m = 0;
        return ZI_OK;
    }

    // longitud minima y maxima; se acota *m con ellas
    l = static_cast<int>(*m);
    for (j = 1; j <= BMAX; j++) {
        if (c[j]) {
            break;
        }
    }
    k = static_cast<int>(j); // longitud minima
    if (static_cast<uint32>(l) < j) {
        l = static_cast<int>(j);
    }
    for (i = BMAX; i; i--) {
        if (c[i]) {
            break;
        }
    }
    g = static_cast<int>(i); // longitud maxima
    if (static_cast<uint32>(l) > i) {
        l = static_cast<int>(i);
    }
    *m = static_cast<uint32>(l);

    // ajusta la ultima cuenta para completar los codigos
    for (y = 1 << j; j < i; j++, y <<= 1) {
        if ((y -= static_cast<int>(c[j])) < 0) {
            return ZI_DATA_ERROR;
        }
    }
    if ((y -= static_cast<int>(c[i])) < 0) {
        return ZI_DATA_ERROR;
    }
    c[i] += static_cast<uint32>(y);

    // desplazamientos iniciales en la tabla de valores para cada longitud
    x[1] = j = 0;
    p = c + 1;
    xp = x + 2;
    while (--i) { // i == g aqui
        *xp++ = (j += *p++);
    }

    // tabla de valores en orden de longitud de bit
    p = b;
    i = 0;
    do {
        if ((j = *p++) != 0) {
            v[x[j]++] = i;
        }
    } while (++i < n);
    n = x[g]; // n pasa a ser la longitud de v

    // genera los codigos de Huffman y sus entradas de tabla
    x[0] = i = 0;
    p = v;
    h = -1;
    w = -l;
    u[0] = nullptr;
    q = nullptr;
    z = 0;

    for (; k <= g; k++) {
        a = c[k];
        while (a--) {
            // i es el codigo de Huffman de k bits para el valor *p; se hacen las tablas hasta el nivel necesario
            while (k > w + l) {
                h++;
                w += l; // la tabla anterior siempre es de l bits

                // tamano minimo de tabla menor o igual que l bits
                z = static_cast<uint32>(g - w);
                z = z > static_cast<uint32>(l) ? static_cast<uint32>(l) : z;
                if ((f = 1u << (j = static_cast<uint32>(k - w))) > a + 1) { // prueba una tabla de k-w bits
                    f -= a + 1;
                    xp = c + k;
                    if (j < z) {
                        while (++j < z) { // prueba tablas menores hasta z bits
                            if ((f <<= 1) <= *++xp) {
                                break;
                            }
                            f -= *xp;
                        }
                    }
                }
                z = 1u << j; // entradas de la tabla de j bits

                // reserva la nueva tabla
                if (*hn + z > MANY) {
                    return ZI_DATA_ERROR;
                }
                u[h] = q = hp + *hn;
                *hn += z;

                // la conecta con la anterior, si la hay
                if (h) {
                    x[h] = i;
                    r.bits = static_cast<uint8>(l);
                    r.exop = static_cast<uint8>(j);
                    j = i >> (w - l);
                    r.base = static_cast<uint32>(q - u[h - 1]) - j;
                    u[h - 1][j].exop = r.exop;
                    u[h - 1][j].bits = r.bits;
                    u[h - 1][j].base = r.base;
                } else {
                    *t = q; // la primera tabla es el resultado
                }
            }

            // entrada de tabla en r
            r.bits = static_cast<uint8>(k - w);
            if (p >= v + n) {
                r.exop = 128 + 64; // sin valores: codigo invalido
            } else if (*p < s) {
                r.exop = static_cast<uint8>(*p < 256 ? 0 : 32 + 64); // 256 es el fin de bloque
                r.base = *p++;
            } else {
                r.exop = static_cast<uint8>(e[*p - s] + 16 + 64);
                r.base = d[*p++ - s];
            }

            // rellena las entradas con r
            f = 1u << (k - w);
            for (j = i >> w; j < z; j += f) {
                q[j].exop = r.exop;
                q[j].bits = r.bits;
                q[j].base = r.base;
            }

            // incremento hacia atras del codigo i de k bits
            for (j = 1u << (k - 1); i & j; j >>= 1) {
                i ^= j;
            }
            i ^= j;

            // retrocede sobre las tablas terminadas
            mask = (1u << w) - 1;
            while ((i & mask) != x[h]) {
                h--;
                w -= l;
                mask = (1u << w) - 1;
            }
        }
    }

    // ZI_BUF_ERROR si la tabla esta incompleta
    return y != 0 && g != 1 ? ZI_BUF_ERROR : ZI_OK;
#endif
}

// 0x7EAF60
int __cdecl InflateTreesDynamic(uint32 nl, uint32 nd, uint32* c, uint32* bl, uint32* bd, ZInflateHuft** tl, ZInflateHuft** td, ZInflateHuft* hp, ZInflateStream* z) {
#ifdef GTASA_WITH_ZLIB_LIBPNG
    return ::inflate_trees_dynamic(nl, nd, c, bl, bd, reinterpret_cast<void**>(tl), reinterpret_cast<void**>(td), hp, z);
#else
    int r;
    uint32 hn = 0;
    uint32* v = static_cast<uint32*>(ZAlloc(z, 288, sizeof(uint32)));
    if (v == nullptr) {
        return ZI_MEM_ERROR;
    }

    // arbol de literales/longitudes
    r = InflateHuftBuild(c, nl, 257, cplens, cplext, tl, bl, hp, &hn, v);
    if (r != ZI_OK || *bl == 0) {
        if (r == ZI_DATA_ERROR) {
            z->msg = MSG_OVERSUB_LITLEN;
        } else if (r != ZI_MEM_ERROR) {
            z->msg = MSG_INCOMPLETE_LITLEN;
            r = ZI_DATA_ERROR;
        }
        ZFree(z, v);
        return r;
    }

    // arbol de distancias
    r = InflateHuftBuild(c + nl, nd, 0, cpdist, cpdext, td, bd, hp, &hn, v);
    if (r != ZI_OK || (*bd == 0 && nl > 257)) {
        if (r == ZI_DATA_ERROR) {
            z->msg = MSG_OVERSUB_DIST;
        } else if (r == ZI_BUF_ERROR) {
            z->msg = MSG_INCOMPLETE_DIST;
            r = ZI_DATA_ERROR;
        } else if (r != ZI_MEM_ERROR) {
            z->msg = MSG_EMPTY_DIST;
            r = ZI_DATA_ERROR;
        }
        ZFree(z, v);
        return r;
    }

    ZFree(z, v);
    return ZI_OK;
#endif
}

// 0x7EB0F0
int __cdecl InflateTreesFixed(uint32* bl, uint32* bd, ZInflateHuft** tl, ZInflateHuft** td, ZInflateStream* /*z*/) {
#ifdef GTASA_WITH_ZLIB_LIBPNG
    return ::inflate_trees_fixed(bl, bd, reinterpret_cast<void**>(tl), reinterpret_cast<void**>(td), nullptr); // el original no lee z
#else
    *bl = *reinterpret_cast<const uint32*>(0x8E0FC0);
    *bd = *reinterpret_cast<const uint32*>(0x8E0FC4);
    *tl = reinterpret_cast<ZInflateHuft*>(0x8E0FC8);
    *td = reinterpret_cast<ZInflateHuft*>(0x8E1FC8);
    return ZI_OK;
#endif
}

// 0x7EB120
int __cdecl InflateFlush(ZInflateBlocksState* s, ZInflateStream* z, int r) {
#ifdef GTASA_WITH_ZLIB_LIBPNG
    return ::inflate_flush(s, z, r);
#else
    uint32 n;
    uint8* p;
    uint8* q;

    // copias locales de los punteros de origen y destino
    p = z->next_out;
    q = s->read;

    // bytes a copiar hasta el final de la ventana
    n = static_cast<uint32>((q <= s->write ? s->write : s->end) - q);
    if (n > z->avail_out) n = z->avail_out;
    if (n && r == ZI_BUF_ERROR) r = ZI_OK;

    // contadores
    z->avail_out -= n;
    z->total_out += n;

    // suma de comprobacion
    if (s->checkfn != nullptr) {
        z->adler = s->check = s->checkfn(s->check, q, n);
    }

    // copia hasta el final de la ventana
    memcpy(p, q, n);
    p += n;
    q += n;

    // mas que copiar al principio de la ventana
    if (q == s->end) {
        q = s->window;
        if (s->write == s->end) {
            s->write = s->window;
        }

        n = static_cast<uint32>(s->write - q);
        if (n > z->avail_out) n = z->avail_out;
        if (n && r == ZI_BUF_ERROR) r = ZI_OK;

        z->avail_out -= n;
        z->total_out += n;

        if (s->checkfn != nullptr) {
            z->adler = s->check = s->checkfn(s->check, q, n);
        }

        memcpy(p, q, n);
        p += n;
        q += n;
    }

    // punteros
    z->next_out = p;
    s->read = q;
    return r;
#endif
}

// 0x7EB260
int __cdecl InflateFast(uint32 bl, uint32 bd, const ZInflateHuft* tl, const ZInflateHuft* td, ZInflateBlocksState* s, ZInflateStream* z) {
#ifdef GTASA_WITH_ZLIB_LIBPNG
    return ::inflate_fast(bl, bd, const_cast<ZInflateHuft*>(tl), const_cast<ZInflateHuft*>(td), s, z);
#else
    const ZInflateHuft* t; // puntero temporal
    uint32 e;              // bits extra u operacion
    uint32 b;              // bitb
    uint32 k;              // bits en bitb
    uint8* p;              // entrada
    uint32 n;              // bytes disponibles
    uint8* q;              // escritura en la ventana
    uint32 m;              // bytes hasta el final de la ventana o el puntero de lectura
    uint32 ml;             // mascara del arbol de literales/longitudes
    uint32 md;             // mascara del arbol de distancias
    uint32 c;              // bytes a copiar
    uint32 d;              // distancia hacia atras
    uint8* r;              // origen de la copia

    LOAD

    ml = inflate_mask[bl];
    md = inflate_mask[bd];

    do { // se llama con m >= 258 && n >= 10
        // codigo de literal/longitud
        GRABBITS(20)
        if ((e = (t = tl + (b & ml))->exop) == 0) {
            DUMPBITS(t->bits)
            *q++ = static_cast<uint8>(t->base);
            m--;
            continue;
        }
        do {
            DUMPBITS(t->bits)
            if (e & 16) {
                // bits extra de la longitud
                e &= 15;
                c = t->base + (b & inflate_mask[e]);
                DUMPBITS(e)

                // base de la distancia a copiar
                GRABBITS(15)
                e = (t = td + (b & md))->exop;
                do {
                    DUMPBITS(t->bits)
                    if (e & 16) {
                        // bits extra a sumar a la base de la distancia
                        e &= 15;
                        GRABBITS(e)
                        d = t->base + (b & inflate_mask[e]);
                        DUMPBITS(e)

                        // la copia
                        m -= c;
                        if (static_cast<uint32>(q - s->window) >= d) { // distancia antes del destino: copia directa
                            r = q - d;
                            *q++ = *r++;  c--; // el minimo es tres: se desenrolla un poco
                            *q++ = *r++;  c--;
                        } else { // distancia despues del destino
                            e = d - static_cast<uint32>(q - s->window); // bytes desde la distancia hasta el final
                            r = s->end - e;
                            if (c > e) { // el origen cruza
                                c -= e;
                                do {
                                    *q++ = *r++;
                                } while (--e);
                                r = s->window;
                            }
                        }
                        do {
                            *q++ = *r++;
                        } while (--c);
                        break;
                    } else if ((e & 64) == 0) {
                        t += t->base;
                        e = (t += (b & inflate_mask[e]))->exop;
                    } else {
                        z->msg = MSG_INVALID_DISTANCE;
                        UNGRAB
                        UPDATE
                        return ZI_DATA_ERROR;
                    }
                } while (true);
                break;
            }
            if ((e & 64) == 0) {
                t += t->base;
                if ((e = (t += (b & inflate_mask[e]))->exop) == 0) {
                    DUMPBITS(t->bits)
                    *q++ = static_cast<uint8>(t->base);
                    m--;
                    break;
                }
            } else if (e & 32) {
                UNGRAB
                UPDATE
                return ZI_STREAM_END;
            } else {
                z->msg = MSG_INVALID_LITLEN;
                UNGRAB
                UPDATE
                return ZI_DATA_ERROR;
            }
        } while (true);
    } while (m >= 258 && n >= 10);

    // sin entrada o salida suficientes: se restauran los punteros
    UNGRAB
    UPDATE
    return ZI_OK;
#endif
}
